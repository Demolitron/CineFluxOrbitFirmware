#include "LCD.h"
#include "WaypointProgram.h"
#include "Main.h"
#include "UserInterface.h"
#include "Timings.h"
#include "Settings.h"
#include "OrbitProgram.h"
#include "MotionEngine.h"

signed char CreateOrbitProgram() {
    UI_Location = UI_LOC_ORBITSETUP;
    signed char ret;

    const char *DIRECTION_1 = "CLOCKWISE\0";
    const char *DIRECTION_0 = "COUNTER CLOCKWISE\0";
    const char*DirectionMenu[2];
    DirectionMenu[0] = DIRECTION_0;
    DirectionMenu[1] = DIRECTION_1;

    const char *SpeedMODE_0 = "MANUAL\0";
    const char *SpeedMODE_1 = "PER ORBIT\0";
    const char *SpeedMODE_2 = "FOR ALL ORBITS\0";
    const char*SpeedModeMenu[3];
    SpeedModeMenu[0] = SpeedMODE_0;
    SpeedModeMenu[1] = SpeedMODE_1;
    SpeedModeMenu[2] = SpeedMODE_2;

    const char *ENDMODE_0 = "BY ORBIT COUNT\0";
    const char *ENDMODE_1 = "BY TOTAL TIME\0";
    const char *ENDMODE_2 = "NEVER ENDING\0";
    const char*EndModeMenu[3];
    EndModeMenu[0] = ENDMODE_0;
    EndModeMenu[1] = ENDMODE_1;
    EndModeMenu[2] = ENDMODE_2;

    const char *COMMAND_0 = "RUN PROGRAM\0";
    const char *COMMAND_1 = "SAVE PRESET\0";
    const char*CommandMenu[2];
    CommandMenu[0] = COMMAND_0;
    CommandMenu[1] = COMMAND_1;

labelOrigin:
    LCD_ClearDisplay();
    LCD_PrintString("MOVE TO START...\0");
    LCD_SetPosition(1, 0);
    LCD_PrintString("THEN CLICK.\0");
    bFollowMode = 1;
    ret = GetClick();
    bFollowMode = 0;
    if (ret < 0) return ret;    
    CurrentOrbitProgram.Origin_deg = (unsigned int) GetCurrentAngle();

labelDirection:
    ret = DisplayChoices(DirectionMenu, 1, "ROTATION DIRECTION:\0", CurrentOrbitProgram.IsClockWise);
    if (ret == -1) goto labelOrigin;
    if (ret == -2) return (-2);
    if (ret < 0) return ret;
    CurrentOrbitProgram.IsClockWise = ret;

labelEndMode:
    ret = DisplayChoices(EndModeMenu, 2, "PROGRAM END MODE:\0", CurrentOrbitProgram.EndMode);
    if (ret == -1) goto labelDirection;
    if (ret == -2) return (-2);
    if (ret < 0) return ret;
    CurrentOrbitProgram.EndMode = ret;
    switch (CurrentOrbitProgram.EndMode) {
        case 0: //End based on Cycle Count.
            labelCycleCount :
            if (CurrentOrbitProgram.CycleCount_rev < 1) CurrentOrbitProgram.CycleCount_rev = 1;
            if (CurrentOrbitProgram.CycleCount_rev > 999) CurrentOrbitProgram.CycleCount_rev = 999;

            ret = GetFloat("ORBIT LIMIT:\0", "", &CurrentOrbitProgram.CycleCount_rev, 1, 999, 1.0);
            if (ret == -1) goto labelEndMode;
            if (ret == -2) return (-2);
            if (ret < 0) return ret;

labelSpeedMode:
            if (CurrentOrbitProgram.CycleCount_rev > 1) ret = DisplayChoices(SpeedModeMenu, 2, "ORBIT SPEED:\0", CurrentOrbitProgram.SpeedMode);
            else ret = DisplayChoices(SpeedModeMenu, 1, "ORBIT SPEED:\0", CurrentOrbitProgram.SpeedMode);
            if (ret == -1) goto labelCycleCount;
            if (ret == -2) return (-2);
            if (ret < 0) return ret;
            switch (ret) {
                case 0:

                    if (CurrentOrbitProgram.Speed_deg_sec < 0.01) CurrentOrbitProgram.Speed_deg_sec = 0.01;
                    ret = GetFloat("SPEED\0", "\xDF/Sec\0", &CurrentOrbitProgram.Speed_deg_sec, 0.01, 90, 0.01);
                    if (ret == -1) goto labelSpeedMode;
                    if (ret == -2) return (-2);
                    break;
                case 1:
                    if (CurrentOrbitProgram.CycleTime_sec < 4) CurrentOrbitProgram.CycleTime_sec = 4;
                    ret = GetTime("TIME PER ORBIT\0", &CurrentOrbitProgram.CycleTime_sec, 4, 86400, 0b1110);
                    if (ret == -1) goto labelSpeedMode;
                    if (ret == -2) return (-2);
                    CurrentOrbitProgram.Speed_deg_sec = 360 / CurrentOrbitProgram.CycleTime_sec;
                    break;
                case 2:
                    if (CurrentOrbitProgram.CycleTime_sec < (4 * CurrentOrbitProgram.CycleCount_rev)) CurrentOrbitProgram.CycleTime_sec = 4 * CurrentOrbitProgram.CycleCount_rev;
                    ret = GetTime("TIME FOR ALL ORBITS\0", &CurrentOrbitProgram.CycleTime_sec, 4 * CurrentOrbitProgram.CycleCount_rev, 86400, 0b1110);
                    if (ret == -1) goto labelSpeedMode;
                    if (ret == -2) return (-2);
                    CurrentOrbitProgram.Speed_deg_sec = (360 * CurrentOrbitProgram.CycleCount_rev) / CurrentOrbitProgram.CycleTime_sec;
                    break;
            }
            break;
        case 1://End based on runtime.
            labelProgramRuntime :

                    ret = GetTime("TOTAL RUNTIME:\0", &CurrentOrbitProgram.ProgramRunTime_sec, 1, 86400, 0b1110);
            if (ret == -1) goto labelEndMode;
            if (ret == -2) return (-2);
            if (ret < 0) return ret;
labelSpeedMode2:

            ret = DisplayChoices(SpeedModeMenu, 1, "ORBIT SPEED:\0", CurrentOrbitProgram.SpeedMode);
            if (ret == -1) goto labelProgramRuntime;
            if (ret == -2) return (-2);
            if (ret < 0) return ret;
            switch (ret) {
                case 0:
                    if (CurrentOrbitProgram.Speed_deg_sec < 0.01) CurrentOrbitProgram.Speed_deg_sec = 0.01;
                    ret = GetFloat("SPEED\0", "\xDF/Sec\0", &CurrentOrbitProgram.Speed_deg_sec, 0.01, 90, 0.01);
                    if (ret == -1) goto labelSpeedMode2;
                    if (ret == -2) return (-2);
                    break;
                case 1:
                    if (CurrentOrbitProgram.CycleTime_sec < 4) CurrentOrbitProgram.CycleTime_sec = 4;
                    ret = GetTime("TIME PER ORBIT:\0", &CurrentOrbitProgram.CycleTime_sec, 4, 86400, 0b1110);
                    if (ret == -1) goto labelSpeedMode2;
                    if (ret == -2) return (-2);
                    CurrentOrbitProgram.Speed_deg_sec = 360 / CurrentOrbitProgram.CycleTime_sec;
                    break;
            }
            CurrentOrbitProgram.CycleCount_rev = (CurrentOrbitProgram.Speed_deg_sec * CurrentOrbitProgram.ProgramRunTime_sec) / 360;
            break;
        case 2: //NO END AT ALL
            labelSpeedMode3 :
                    ret = DisplayChoices(SpeedModeMenu, 1, "ORBIT SPEED:\0", CurrentOrbitProgram.SpeedMode);
            if (ret == -1) goto labelEndMode;
            if (ret == -2) return (-2);
            if (ret < 0) return ret;
            switch (ret) {
                case 0:
                    if (CurrentOrbitProgram.Speed_deg_sec < 0.01) CurrentOrbitProgram.Speed_deg_sec = 0.01;
                    ret = GetFloat("SPEED\0", "\xDF/Sec\0", &CurrentOrbitProgram.Speed_deg_sec, 0.01, 90, 0.01);
                    if (ret == -1) goto labelSpeedMode3;
                    if (ret == -2) return (-2);
                    if (ret < 0) return ret;
                    break;
                case 1:
                    if (CurrentOrbitProgram.CycleTime_sec < 4) CurrentOrbitProgram.CycleTime_sec = 4;
                    ret = GetTime("TIME PER ORBIT:\0", &CurrentOrbitProgram.CycleTime_sec, 4, 86400, 0b1110);
                    if (ret == -1) goto labelSpeedMode3;
                    if (ret == -2) return (-2);
                    if (ret < 0) return ret;
                    CurrentOrbitProgram.Speed_deg_sec = 360 / CurrentOrbitProgram.CycleTime_sec;
                    break;
            }
            break;

    }

labelActions:
    ret = DisplayChoices(CommandMenu, 1, "ACTION:\0", 0);
    if (ret == -1) goto labelCycleCount;
    if (ret == -2) return (-2);
    if (ret < 0) return ret;
    if (ret == 0) {
        return RunOrbitProgram();
    }

    ret = GetPresetNumber();
    if (ret == -1) goto labelActions;
    if (ret == -2) return (-2);
    if (ret < 0) return ret;

    CurrentOrbitProgram.Type = 2;
    SavePreset(ret, (unsigned char *) &CurrentOrbitProgram);
    goto labelActions;
}

signed char RunOrbitProgram() {
    signed char retCode = 0;
    UI_Location = UI_LOC_ORBITRUN;
    signed char ret;
    double EndPos;
    double OriginPos;
    double TimeRemaining;
    double DistanceRemaining;
    double MPos;

    LCD_ClearDisplay();
    LCD_PrintString("MOVING TO START\0");
    LCD_SetPosition(1,0);
    LCD_PrintLong(CurrentOrbitProgram.Origin_deg,3,0);
    Wait_ms(5000);
    MoveToAngle(CurrentOrbitProgram.Origin_deg, 90);
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


    double Distance = CurrentOrbitProgram.CycleCount_rev * 360;
    if (CurrentOrbitProgram.EndMode == 2) Distance = 7776000;
    if (!CurrentOrbitProgram.IsClockWise) Distance = -Distance;

    bLock_Motor_Position = 1;
    OriginPos = Motor_Position;
    bLock_Motor_Position = 0;
    OriginPos *= Config.Degrees_Per_Count;

    EndPos = OriginPos + Distance;


    LCD_ClearDisplay();
    LCD_PrintString("RUNNING: T-HH:MM:SS\0");

    if (CurrentOrbitProgram.EndMode == 2) {
        LCD_ClearDisplay();
        LCD_PrintString("ORBIT RUNNING\0");
        LCD_SetPosition(1, 0);
        LCD_PrintString("INFINITE RUNTIME\0");
    }

    double SpdConvert = 1 / CurrentOrbitProgram.Speed_deg_sec;
    ret = GetInput_nonblocking();

    Move(Distance, CurrentOrbitProgram.Speed_deg_sec);
    while (bMove_InProgress) {

        bLock_Motor_Position = 1;
        MPos = Motor_Position;
        bLock_Motor_Position = 0;
        MPos *= Config.Degrees_Per_Count;

        DistanceRemaining = EndPos - MPos;
        if (DistanceRemaining < 0) DistanceRemaining = -DistanceRemaining;
        TimeRemaining = DistanceRemaining*SpdConvert;
        DistanceRemaining *= 0.002778;

        if (!(CurrentOrbitProgram.EndMode == 2)) {
            LCD_SetPosition(0, 11);
            PrintTime(TimeRemaining, 0b1110, 0);
            LCD_SetPosition(1, 0);
            LCD_PrintFloat(DistanceRemaining, 5, 2, 0);
            LCD_PrintString(" REV REMAIN\0");
        }

        ret = GetInput_nonblocking();
        if (ret == USER_INPUT_CANCEL) {
            retCode = -2;
            goto labelFinished;
        }
        if (ret < 0) {
            retCode = ret;
            goto labelFinished;
        }

        Idle();
    }

labelFinished:
    bMove_InProgress = 0;
    bFollowMode = 1;
    LCD_ClearDisplay();
    LCD_PrintString("STOPPING...\0");
    Idle();
    bMove_InProgress = 0;
    bFollowMode = 1;
    Wait_ms(500);
    bFollowMode = 0;
    return retCode;
}
