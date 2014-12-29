#include "WaypointProgram.h"
#include "Main.h"
#include "UserInterface.h"
#include "Timings.h"
#include "Settings.h"
#include "MotionEngine.h"
#include "lcd.h"

signed char CreateWaypointProgram() {
    UI_Location = UI_LOC_WAYPOINTSETUP;
    signed char ret = 0;
    double LastPos = 0;
    double CurrentPos = 0;
    double DeltaDistance = 0;
    signed int intDistance = 0;
    double TravelTime = 0;
    double MinTravelTime = 0;
    unsigned int intTravelTime = 0;
    double DwellTime = 0;
    double CycleCount = 0;

    char Done = 0;
    signed char idx = 0;
    CurrentPath.PointCount = 0;

    const char *DIRECTION_0 = "ONE-WAY\0";
    const char *DIRECTION_1 = "BOUNCE\0";
    const char*DirectionMenu[2];
    DirectionMenu[0] = DIRECTION_0;
    DirectionMenu[1] = DIRECTION_1;

    const char *COMMAND_0 = "RUN PROGRAM\0";
    const char *COMMAND_1 = "SAVE PRESET\0";
    const char*CommandMenu[2];
    CommandMenu[0] = COMMAND_0;
    CommandMenu[1] = COMMAND_1;

labelSetOrigin:
    TravelTime = 0;
    MinTravelTime = 0;
    intDistance = 0;
    DeltaDistance = 0;

    CurrentPos = 0;
    LastPos = 0;
    ret = 0;
    DwellTime = 0;

    idx = 0;

    LCD_ClearDisplay();
    LCD_PrintString("MOVE TO START...\0");
    LCD_SetPosition(1, 0);
    LCD_PrintString("THEN CLICK.\0");
    bFollowMode = 1;
    ret = GetClick();
    if (ret < 0) return ret;
    bFollowMode = 0;
    CurrentPath.Origin_deg = (unsigned int) GetCurrentAngle();

    LastPos = GetCurrentLinearPosition();

    ret = GetTime("DWELL TIME:\0", &DwellTime, 0, 65535, 0b1110);
    if (ret == -2) return -2;
    if (ret == -1) goto labelSetOrigin;
    if (ret < 0) return ret;
    CurrentPath.DwellTimes_sec[idx] = (unsigned int) (DwellTime);

    while (!Done) {
labelInputWaypoint1:
        LCD_ClearDisplay();
        LCD_PrintString("MOVE TO NEXT POINT\0");
        LCD_SetPosition(1, 0);
        LCD_PrintString("+XXXX\xDF THEN CLICK\0");

        bFollowMode = 1;

        ClearUI_Event = 1;
        while (ClearUI_Event) Idle();
        ret = 0;
        while (!(ret == USER_INPUT_CLICK)) {
            Idle();
            CurrentPos = GetCurrentLinearPosition();
            DeltaDistance = CurrentPos - LastPos;
            intDistance = (signed int) (DeltaDistance);
            LCD_SetPosition(1, 0);
            LCD_PrintLong(intDistance, 4, 1);

            bLock_UI_Event = 1;
            ret = UI_Event;
            bLock_UI_Event = 0;

            if (ret == USER_INPUT_CANCEL) {
                bFollowMode = 0;
                return -2;
            }

            if (ret == USER_INPUT_BACK) {
                bFollowMode = 0;
                if (idx == 0) {
                    goto labelSetOrigin;
                } else {
                    idx--;
                    goto labelInputWaypoint1;
                }
            }

            if (ret == USER_INPUT_DEC) {
                ClearUI_Event = 1;
                bFollowMode = 0;
                Move(-1, 90);
                while (bMove_InProgress) Idle();
                bFollowMode = 1;
                while (ClearUI_Event) Idle();
            }

            if (ret == USER_INPUT_INC) {
                ClearUI_Event = 1;
                bFollowMode = 0;
                Move(1, 90);
                while (bMove_InProgress) Idle();
                bFollowMode = 1;
                while (ClearUI_Event) Idle();
            }

            if (ret < 0) return ret;
        }
        bFollowMode = 0;


        CurrentPath.Distances_deg[idx] = intDistance;
        MinTravelTime = MinTimeToMove(DeltaDistance);
        intTravelTime = (unsigned int) MinTravelTime;
        MinTravelTime -= intTravelTime;
        if (MinTravelTime > 0) MinTravelTime = intTravelTime + 1;
        else MinTravelTime = intTravelTime;


labelInputWaypoint2:
        ret = GetTime("TRAVEL TIME:\0", &TravelTime, MinTravelTime, 65535, 0b1110);
        if (ret == -2) return -2;
        if (ret == -1) goto labelInputWaypoint1;
        if (ret < 0) return ret;
        CurrentPath.TravelTimes_sec[idx] = (unsigned int) (TravelTime);

labelInputWaypoint3:
        ret = GetTime("DWELL TIME:\0", &DwellTime, 0, 65535, 0b1110);
        if (ret == -2) return -2;
        if (ret == -1) goto labelInputWaypoint2;
        if (ret < 0) return ret;
        CurrentPath.DwellTimes_sec[idx + 1] = (unsigned int) (DwellTime);

        if (idx < 17) {
            ret = GetYesNo("ADD ANOTHER POINT?", 1);
            if (ret == -2) return -2;
            if (ret == -1) goto labelInputWaypoint3;
            if (ret == 0) Done = 1;
            if (ret == 1) {
                idx++;
                LastPos = CurrentPos;
                goto labelInputWaypoint1;
            }
            if (ret < 0) return ret;
        } else {
            LCD_ClearDisplay();
            LCD_PrintString("MAX WAYPOINTS HAVE\0");
            LCD_SetPosition(1, 0);
            LCD_PrintString("BEEN ADDED\0");
            ret = GetClick();
            if (ret == -2) return -2;
            if (ret == -1) goto labelInputWaypoint3;
            Done = 1;
        }
    }

    CurrentPath.PointCount = idx;
labelDirection:
    ret = DisplayChoices(DirectionMenu, 1, "LOOP MODE?\0", CurrentPath.Bounce);
    if (ret == -1) goto labelInputWaypoint3;
    if (ret == -2) return (-2);
    if (ret < 0) return ret;
    CurrentPath.Bounce = ret;

labelCycleCount:
    ret = GetFloat("CYCLE COUNT:0=INF.\0", " CYCLES\0", &CycleCount, 0, 999, 1.0);
    if (ret == -1) goto labelDirection;
    if (ret == -2) return (-2);
    if (ret < 0) return ret;
    CurrentPath.LoopCount = (unsigned int) (CycleCount);

labelActions:
    ret = DisplayChoices(CommandMenu, 1, "ACTION:\0", 0);
    if (ret == -1) goto labelCycleCount;
    if (ret == -2) return (-2);
    if (ret < 0) return ret;
    if (ret == 0) {
        return RunWaypointProgram();
    }

    ret = GetPresetNumber();
    if (ret == -1) goto labelActions;
    if (ret == -2) return (-2);
    if (ret < 0) return ret;

    CurrentPath.Type = 1;
    SavePreset(ret, (unsigned char *) &CurrentPath);

    goto labelActions;

}

signed char RunWaypointProgram() {
    signed char retCode = 0;
    UI_Location = UI_LOC_WAYPOINTRUN;
    signed char ret;
    double D;
    double T;
    double S;
    char PingPong = 0;
    signed char idx;
    unsigned int CycleIndex = 1;

    LCD_ClearDisplay();
    LCD_PrintString("MOVING TO START\0");
    MoveToAngle(CurrentPath.Origin_deg, 90);
    while (bMove_InProgress) {
        Idle();
        ret = GetInput_nonblocking();
        if (ret == USER_INPUT_CANCEL) {
            retCode = -2;
            goto labelFinished;
        }
        if (ret < 0) {
            retCode = ret;
            goto labelFinished;
        }
    }

    LCD_ClearDisplay();
    LCD_PrintString("DWELL ON START\0");
    Wait_seconds_nonblocking(CurrentPath.DwellTimes_sec[0]);
    while (bWaiting) {
        Idle();
        ret = GetInput_nonblocking();
        if (ret == USER_INPUT_CANCEL) {
            retCode = -2;
            goto labelFinished;
        }
        if (ret < 0) {
            retCode = ret;
            goto labelFinished;
        }
    }

    while (1) {
        if (PingPong) idx = CurrentPath.PointCount;
        else idx = 0;

        while ((idx >= 0) && (idx <= CurrentPath.PointCount)) {
            D = CurrentPath.Distances_deg[idx];
            T = CurrentPath.TravelTimes_sec[idx];
            S = SpeedRequiredToMoveInTime(D, T);
            if (PingPong) {
                LCD_ClearDisplay();
                LCD_PrintString("MOVING TO POINT #\0");
                LCD_PrintLong(idx, 2, 0);
                LCD_SetPosition(1, 0);
                if (CurrentPath.LoopCount == 0) LCD_PrintString("INFINITE CYCLES\0");
                else {
                    LCD_PrintString("CYCLE \0");
                    LCD_PrintLong(CycleIndex, 3, 0);
                    LCD_PrintString(" OF \0");
                    LCD_PrintLong(CurrentPath.LoopCount, 3, 0);
                }

                Move(-D, S);
                while (bMove_InProgress) {
                    Idle();
                    ret = GetInput_nonblocking();
                    if (ret == USER_INPUT_CANCEL) {
                        retCode = -2;
                        goto labelFinished;
                    }
                    if (ret < 0) {
                        retCode = ret;
                        goto labelFinished;
                    }
                }

                LCD_ClearDisplay();
                LCD_PrintString("DWELL ON POINT #\0");
                LCD_PrintLong(idx, 2, 0);
                LCD_SetPosition(1, 0);
                if (CurrentPath.LoopCount == 0) LCD_PrintString("INFINITE CYCLES\0");
                else {
                    LCD_PrintString("CYCLE \0");
                    LCD_PrintLong(CycleIndex, 3, 0);
                    LCD_PrintString(" OF \0");
                    LCD_PrintLong(CurrentPath.LoopCount, 3, 0);
                }

                Wait_seconds_nonblocking(CurrentPath.DwellTimes_sec[idx]);
                while (bWaiting) {
                    Idle();
                    ret = GetInput_nonblocking();
                    if (ret == USER_INPUT_CANCEL) {
                        retCode = -2;
                        goto labelFinished;
                    }
                    if (ret < 0) {
                        retCode = ret;
                        goto labelFinished;
                    }
                }
                idx--;
            } else {

                LCD_ClearDisplay();
                LCD_PrintString("MOVING TO POINT #\0");
                LCD_PrintLong(idx + 1, 2, 0);
                LCD_SetPosition(1, 0);
                if (CurrentPath.LoopCount == 0) LCD_PrintString("INFINITE CYCLES\0");
                else {
                    LCD_PrintString("CYCLE \0");
                    LCD_PrintLong(CycleIndex, 3, 0);
                    LCD_PrintString(" OF \0");
                    LCD_PrintLong(CurrentPath.LoopCount, 3, 0);
                }

                Move(D, S);
                while (bMove_InProgress) {
                    Idle();
                    ret = GetInput_nonblocking();
                    if (ret == USER_INPUT_CANCEL) {
                        retCode = -2;
                        goto labelFinished;
                    }
                    if (ret < 0) {
                        retCode = ret;
                        goto labelFinished;
                    }
                }

                LCD_ClearDisplay();
                LCD_PrintString("DWELL ON POINT #\0");
                LCD_PrintLong(idx + 1, 2, 0);
                LCD_SetPosition(1, 0);
                if (CurrentPath.LoopCount == 0) LCD_PrintString("INFINITE CYCLES\0");
                else {
                    LCD_PrintString("CYCLE \0");
                    LCD_PrintLong(CycleIndex, 3, 0);
                    LCD_PrintString(" OF \0");
                    LCD_PrintLong(CurrentPath.LoopCount, 3, 0);
                }

                Wait_seconds_nonblocking(CurrentPath.DwellTimes_sec[idx + 1]);
                while (bWaiting) {
                    Idle();
                    ret = GetInput_nonblocking();
                    if (ret == USER_INPUT_CANCEL) {
                        retCode = -2;
                        goto labelFinished;
                    }
                    if (ret < 0) {
                        retCode = ret;
                        goto labelFinished;
                    }
                }
                idx++;
            }
        }

        if (CurrentPath.Bounce) PingPong = !PingPong;
        else {
            PingPong = 0;
            LCD_ClearDisplay();
            LCD_PrintString("MOVING TO ORIGIN\0");
            LCD_SetPosition(1, 0);
            if (CurrentPath.LoopCount == 0) LCD_PrintString("INFINITE CYCLES\0");
            else {
                LCD_PrintString("CYCLE \0");
                LCD_PrintLong(CycleIndex, 3, 0);
                LCD_PrintString(" OF \0");
                LCD_PrintLong(CurrentPath.LoopCount, 3, 0);
            }

            MoveToAngle(CurrentPath.Origin_deg, 90);
            while (bMove_InProgress) {
                Idle();
                ret = GetInput_nonblocking();
                if (ret == USER_INPUT_CANCEL) {
                    retCode = -2;
                    goto labelFinished;
                }
                if (ret < 0) {
                    retCode = ret;
                    goto labelFinished;
                }
            }

            LCD_ClearDisplay();
            LCD_PrintString("DWELL ON POINT #00\0");
            LCD_SetPosition(1, 0);
            if (CurrentPath.LoopCount == 0) LCD_PrintString("INFINITE CYCLES\0");
            else {
                LCD_PrintString("CYCLE \0");
                LCD_PrintLong(CycleIndex, 3, 0);
                LCD_PrintString(" OF \0");
                LCD_PrintLong(CurrentPath.LoopCount, 3, 0);
            }
            Wait_seconds_nonblocking(CurrentPath.DwellTimes_sec[0]);
            while (bWaiting) {
                Idle();
                ret = GetInput_nonblocking();
                if (ret == USER_INPUT_CANCEL) {
                    retCode = -2;
                    goto labelFinished;
                }
                if (ret < 0) {
                    retCode = ret;
                    goto labelFinished;
                }
            }
        }

        if (CurrentPath.LoopCount) {
            CycleIndex++;
            if (CycleIndex > CurrentPath.LoopCount) goto labelFinished;
        }
    }

labelFinished:
    bMove_InProgress = 0;
    bFollowMode = 0;
    bSpeedMode = 1;
    Speed_SetToThis = 0;
    Speed_SetPending = 1;
    LCD_ClearDisplay();
    LCD_PrintString("STOPPING...\0");
    while (Speed_SetPending);
    while (Speed_IsAccelerating){
        Idle();
    }
    bMove_InProgress = 0;
    bFollowMode = 0;
    bSpeedMode=0;
    return retCode;
}
