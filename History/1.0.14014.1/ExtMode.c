#include "Main.h"
#include "Timings.h"
#include "MotionEngine.h"
#include "Settings.h"
#include "lcd.h"
#include "userInterface.h"

unsigned char ExtMode() {
    UI_Location = UI_LOC_EXTMODE;

    unsigned char PathIdx = 0;
    unsigned char PathCount = 0;
    signed int PathDistances[100];
    unsigned int PathTravelTimes[100];
    unsigned int PathDwellTimes[100];

    ExtModeActive = 1;
    signed char ret;
    unsigned char CmdID;    
    unsigned char idx;
    unsigned char State = 0;
    MULTI mTemp;

    char PrepMove_Ready = 0;
    double PrepMove_Distance, PrepMove_Speed, PrepMove_Acceleration;
    double S, D, T;

    LCD_ClearDisplay();
    LCD_PrintString("EXTERNAL MODE");

    while (1) {
        ret = GetInput_nonblocking();
        if (ret == USER_INPUT_CANCEL) {
            goto labelShutdown;
        }
        switch (State) {
            case 0: //Idle Mode - Nothing is happening...
                bMove_InProgress = 0;
                bFollowMode = 0;
                bSpeedMode = 0;
                break;
            case 1: //Stopping
                bMove_InProgress = 0;
                bFollowMode = 0;
                bSpeedMode = 1;
                Speed_SetToThis = 0;
                Speed_SetPending = 1;
                while(Speed_SetPending);
                if (!Speed_IsAccelerating) State = 0;
                break;
            case 2: //Single Move in Progress
                if (!bMove_InProgress) State = 1;
                break;
            case 3: //Moving to a Point along a Path;
                if (!bMove_InProgress) {
                    Wait_seconds_nonblocking(PathDwellTimes[PathIdx]);
                    PathIdx += 1;
                    State = 4;
                }
                break;
            case 4: //Waiting for the Dwell time to expire for this waypoint...
                if (!bWaiting) {
                    if (PathIdx == PathCount) {
                        State = 1;
                    } else {
                        D = PathDistances[PathIdx];
                        T = PathTravelTimes[PathIdx];
                        S = SpeedRequiredToMoveInTime(D, T);
                        Move(D, S);
                        State = 3;
                    }
                }
                break;
        }

        if (RX_MsgComplete) {
            ChkSum = 0;
            ReadIdx = 1;
            RX_MsgComplete = 0;
            if (MessageStream_ReadByte() == MyID) {
                CmdID = MessageStream_ReadByte();
                switch (CmdID) {
                    case CMD_PREP_MOVE:
                        mTemp.ub[0] = MessageStream_ReadByte();
                        mTemp.ub[1] = MessageStream_ReadByte();
                        mTemp.ub[2] = MessageStream_ReadByte();
                        mTemp.ub[3] = MessageStream_ReadByte();
                        PrepMove_Distance = mTemp.dbl;
                        ChkSum += mTemp.ub[3];
                        ChkSum += mTemp.ub[2];
                        ChkSum += mTemp.ub[1];
                        ChkSum += mTemp.ub[0];

                        mTemp.ub[0] = MessageStream_ReadByte();
                        mTemp.ub[1] = MessageStream_ReadByte();
                        mTemp.ub[2] = MessageStream_ReadByte();
                        mTemp.ub[3] = MessageStream_ReadByte();
                        PrepMove_Speed = mTemp.dbl;
                        ChkSum += mTemp.ub[3];
                        ChkSum += mTemp.ub[2];
                        ChkSum += mTemp.ub[1];
                        ChkSum += mTemp.ub[0];

                        mTemp.ub[0] = MessageStream_ReadByte();
                        mTemp.ub[1] = MessageStream_ReadByte();
                        mTemp.ub[2] = MessageStream_ReadByte();
                        mTemp.ub[3] = MessageStream_ReadByte();
                        PrepMove_Acceleration = mTemp.dbl;
                        ChkSum += mTemp.ub[3];
                        ChkSum += mTemp.ub[2];
                        ChkSum += mTemp.ub[1];
                        ChkSum += mTemp.ub[0];

                        if (ChkSum == MessageStream_ReadByte()) {
                            PrepMove_Ready = 1;
                            AckCmd(CmdID);
                        } else {
                            PrepMove_Ready = 0;
                            NackCmd(CmdID, 0xFA);
                        }
                        break;
                    case CMD_EXEC_MOVE:
                        if (!PrepMove_Ready) {
                            NackCmd(CmdID, 1);
                            break;
                        }
                        if (State) {
                            NackCmd(CmdID, 2);
                            break;
                        }
                        MoveAdvanced(PrepMove_Distance, PrepMove_Speed, PrepMove_Acceleration);
                        PrepMove_Ready = 0;
                        State = 2;
                        AckCmd(CmdID);

                        break;
                    case CMD_STOP:
                        if (!State == 0) State = 1;
                        AckCmd(CmdID);
                        break;
                    case CMD_STATUS:
                        TX_Idx = 0;
                        TXBuffer[TX_Idx++] = '$';
                        MessageStream_WriteByte(CmdID);

                        MessageStream_WriteByte(State);
                        ChkSum += State;

                        MessageStream_WriteByte(PrepMove_Ready);
                        ChkSum += PrepMove_Ready;

                        bLock_Motor_Position = 1;
                        mTemp.dbl = Motor_Position;
                        bLock_Motor_Position = 0;
                        mTemp.dbl *= Config.Degrees_Per_Count;
                        MessageStream_WriteByte(mTemp.ub[0]);
                        MessageStream_WriteByte(mTemp.ub[1]);
                        MessageStream_WriteByte(mTemp.ub[2]);
                        MessageStream_WriteByte(mTemp.ub[3]);
                        ChkSum += mTemp.ub[0];
                        ChkSum += mTemp.ub[1];
                        ChkSum += mTemp.ub[2];
                        ChkSum += mTemp.ub[3];

                        mTemp.dbl = 0;
                        while (!(mTemp.dbl == Move_speedQ24)) mTemp.dbl = Move_speedQ24;
                        mTemp.dbl *= INV_Conversion_Q24_500Hz;
                        mTemp.dbl *= Config.Degrees_Per_Count;
                        MessageStream_WriteByte(mTemp.ub[0]);
                        MessageStream_WriteByte(mTemp.ub[1]);
                        MessageStream_WriteByte(mTemp.ub[2]);
                        MessageStream_WriteByte(mTemp.ub[3]);
                        ChkSum += mTemp.ub[0];
                        ChkSum += mTemp.ub[1];
                        ChkSum += mTemp.ub[2];
                        ChkSum += mTemp.ub[3];

                        bLock_Tick500Hz = 1;
                        mTemp.dbl = Tick500Hz;
                        bLock_Tick500Hz = 0;
                        mTemp.dbl *= 0.002;
                        MessageStream_WriteByte(mTemp.ub[0]);
                        MessageStream_WriteByte(mTemp.ub[1]);
                        MessageStream_WriteByte(mTemp.ub[2]);
                        MessageStream_WriteByte(mTemp.ub[3]);
                        ChkSum += mTemp.ub[0];
                        ChkSum += mTemp.ub[1];
                        ChkSum += mTemp.ub[2];
                        ChkSum += mTemp.ub[3];

                        bLock_BatteryVoltage = 1;
                        mTemp.dbl = BatteryVoltage;
                        bLock_BatteryVoltage = 0;
                        mTemp.dbl *= Config.Volts_per_Count;
                        MessageStream_WriteByte(mTemp.ub[0]);
                        MessageStream_WriteByte(mTemp.ub[1]);
                        MessageStream_WriteByte(mTemp.ub[2]);
                        MessageStream_WriteByte(mTemp.ub[3]);
                        ChkSum += mTemp.ub[0];
                        ChkSum += mTemp.ub[1];
                        ChkSum += mTemp.ub[2];
                        ChkSum += mTemp.ub[3];

                        MessageStream_WriteByte(ChkSum);

                        TXBuffer[TX_Idx++] = '#';
                        idx = TX_Idx;
                        TX_Idx = 0;
                        TX_bCount = idx;
                        break;
                    case CMD_PATH_INIT:
                        if ((State == 3) || (State == 4)) {
                            NackCmd(CmdID, 1);
                            break;
                        }
                        PathIdx = 0;
                        PathCount = 0;
                        AckCmd(CmdID);
                        break;
                    case CMD_PATH_ADD:
                        if ((State == 3) || (State == 4)) {
                            NackCmd(CmdID, 1);
                            break;
                        }

                        if (PathCount > 99) {
                            NackCmd(CmdID, 2);
                            break;
                        }

                        mTemp.ub[0] = MessageStream_ReadByte();
                        mTemp.ub[1] = MessageStream_ReadByte();
                        ChkSum += mTemp.ub[0];
                        ChkSum += mTemp.ub[1];
                        PathDistances[PathCount] = mTemp.i[0];

                        mTemp.ub[0] = MessageStream_ReadByte();
                        mTemp.ub[1] = MessageStream_ReadByte();
                        ChkSum += mTemp.ub[0];
                        ChkSum += mTemp.ub[1];
                        PathTravelTimes[PathCount] = mTemp.ui[0];

                        mTemp.ub[0] = MessageStream_ReadByte();
                        mTemp.ub[1] = MessageStream_ReadByte();
                        ChkSum += mTemp.ub[0];
                        ChkSum += mTemp.ub[1];
                        PathDwellTimes[PathCount] = mTemp.ui[0];

                        if (ChkSum == MessageStream_ReadByte()) {
                            AckCmd(CmdID);
                            PathCount++;
                        } else {
                            PrepMove_Ready = 0;
                            NackCmd(CmdID, 0xFA);
                        }
                        break;
                    case CMD_PATH_RUN:
                        if (!(State == 0)) {
                            NackCmd(CmdID, 1);
                            break;
                        }
                        PathIdx = 0;
                        D = PathDistances[PathIdx];
                        T = PathTravelTimes[PathIdx];
                        S = SpeedRequiredToMoveInTime(D, T);
                        Move(D, S);
                        State = 3;
                        AckCmd(CmdID);
                        break;
                    case CMD_EXIT:
                        AckCmd(CmdID);
                        goto labelShutdown;                       
                    default:
                        NackCmd(CmdID, 0xFE);
                        break;
                }
            }
        }
    }

labelShutdown:
    bMove_InProgress = 0;
    bFollowMode = 0;
    bSpeedMode = 1;
    Speed_SetToThis = 0;
    Speed_SetPending = 1;
    while (Speed_IsAccelerating) {
        idx = 0xff;
        while (idx--);
    }
    ExtModeActive = 0;

    return 0;
}
