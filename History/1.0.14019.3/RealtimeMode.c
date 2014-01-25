#include "main.h"
#include "UserInterface.h"
#include "lcd.h"
#include "settings.h"
#include "RealtimeMode.h"

signed char RealtimeMode() {
    signed char retCode=0;
    UI_Location=UI_LOC_REALTIMERUN;
    double SetSpeed = 0;

    signed char ret;

    bLock_Motor_Position = 1;
    Move_Origin = Motor_Position;
    bLock_Motor_Position = 0;


    Move_AccelValueQ24 = (unsigned long) (180 * Config.Counts_Per_Degree * Conversion_Q24_500Hz_500Hz);


    Move_shifted_position.ul = 0;
    Move_speedQ24 = 0;
    Move_position[0].ul = 0;
    Move_position[1].ul = 0;
    bFollowMode=0;
    bMove_InProgress=0;
    bSpeedMode = 1;
    bSpeedMode = 1;

    ret = GetInput_nonblocking();
    LCD_ClearDisplay();
    LCD_PrintString("REALTIME MODE\0");
    LCD_SetPosition(1, 0);
    LCD_PrintString("SPEED: XX.XX\xDF/Sec\0");

    while (1) {
        ret = GetInput_nonblocking();
        if (ret == USER_INPUT_CANCEL){ retCode=-2; goto labelExit;}
        if (ret == USER_INPUT_BACK) { retCode=-1; goto labelExit;}
        if (ret == USER_INPUT_CLICK) SetSpeed = 0;
        if (ret == USER_INPUT_INC) {
            SetSpeed += GetRotaryMultiplier() * 0.05;
            if (SetSpeed > 90) SetSpeed = 90;
        }
        if (ret == USER_INPUT_DEC) {
            SetSpeed -= GetRotaryMultiplier() * 0.05;
            if (SetSpeed<-90) SetSpeed = -90;
        }
        if (ret<0) { retCode=ret; goto labelExit;}

        LCD_SetPosition(1, 6);

        LCD_PrintFloat(SetSpeed, 2, 2, 1);


        if (bMove_Neg) {
            if (SetSpeed < 0) {
                Speed_SetToThis = (unsigned long) (-SetSpeed * Config.Counts_Per_Degree * Conversion_Q24_500Hz);
                Speed_SetPending = 1;
                while (Speed_SetPending) Idle();
            } else if (SetSpeed > 0) {
                Speed_SetToThis = 1;
                Speed_SetPending = 1;
                while (Speed_SetPending) Idle();
                if (!Speed_IsAccelerating) {
                    bSpeedMode = 0;
                    bSpeedMode = 0;
                    Move_shifted_position.ul = 0;
                    Move_speedQ24 = 0;
                    Move_position[0].ul = 0;
                    Move_position[1].ul = 0;

                    bMove_Neg = 0;
                    bLock_Motor_Position = 1;
                    Move_Origin = Motor_Position;
                    bLock_Motor_Position = 0;
                    bSpeedMode = 1;
                    bSpeedMode = 1;
                }
            } else {
                Speed_SetToThis = 1;
                Speed_SetPending = 1;
                while (Speed_SetPending) Idle();
            }
        } else {
            if (SetSpeed > 0) {
                Speed_SetToThis = (unsigned long) (SetSpeed * Config.Counts_Per_Degree * Conversion_Q24_500Hz);
                Speed_SetPending = 1;
                while (Speed_SetPending) Idle();
            } else if (SetSpeed < 0) {
                Speed_SetToThis = 1;
                Speed_SetPending = 1;
                while (Speed_SetPending) Idle();

                if (!Speed_IsAccelerating) {
                    bSpeedMode = 0;
                    bSpeedMode = 0;
                    Move_shifted_position.ul = 0;
                    Move_speedQ24 = 0;
                    Move_position[0].ul = 0;
                    Move_position[1].ul = 0;
                    bMove_Neg = 1;
                    bLock_Motor_Position = 1;
                    Move_Origin = Motor_Position;
                    bLock_Motor_Position = 0;
                    bSpeedMode = 1;
                    bSpeedMode = 1;
                }
            } else {
                Speed_SetToThis = 1;
                Speed_SetPending = 1;
                while (Speed_SetPending) Idle();
            }
        }

        Idle();

    }
labelExit:

    LCD_ClearDisplay();
    LCD_PrintString("STOPPING...\0");
    while (1) {
        Speed_SetToThis = 1;
        Speed_SetPending = 1;
        while (Speed_SetPending) Idle();
        if (!Speed_IsAccelerating) {
            bSpeedMode = 0;
            bSpeedMode = 0;
            return retCode;
        }
        Idle();
    }
    return(0);

}
