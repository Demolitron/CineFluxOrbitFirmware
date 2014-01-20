#include <htc.h>
#include <math.h>
#include "main.h"
#include "timings.h"
#include "UserInterface.h"
#include "LCD.h"
#include "WaypointProgram.h"
#include "settings.h"
#include "OrbitProgram.h"
#include "RealtimeMode.h"
#include "MotionEngine.h"
#include "ExtMode.h"
#include "Version.h"

#pragma config INTOSCSEL = HIGH, SOSCSEL = DIG, XINST = OFF, RETEN = OFF, FOSC = EC3, PLLCFG = ON, BOREN =OFF, WDTEN = OFF, RTCOSC = INTOSCREF, MCLRE = ON

void main(void) {
    signed char ret = 0;
    signed char lastMenu = 0;
    char pnum = 0;

    SetupIO();
    MOTOR_FORWARD = 0;
    MOTOR_REVERSE = 0;

    Move_shifted_position.ul = 0;
    Move_position[0].ul = 0;
    Move_position[1].ul = 0;
    //PID_SetPoint = 0;
    char idx;
    for (idx = 0; idx < 16; idx++) RotaryDetentIntervals[idx] = 0xFF;

    LoadSettings();
    //FactoryDefault();

    SetupHardware();

    LCD_PowerUp();
    LCD_ClearDisplay();

    LCD_PrintString("CINEFLUX ORBIT\0");
    LCD_SetPosition(1, 0);
    LCD_PrintString(VER);
    Wait_ms(2000);

    const char *COMMAND_0 = "ORBIT MODE\0";
    const char *COMMAND_1 = "WAYPOINT MODE\0";
    const char *COMMAND_3 = "RUN PRESET\0";
    const char *COMMAND_2 = "REALTIME MODE\0";
    const char *COMMAND_4 = "BATTERY VOLTAGE\0";
    const char *COMMAND_5 = "GO TO SLEEP\0";
    const char *COMMAND_6 = "EXTERNAL CTRL MODE\0";
    const char*CommandMenu[7];
    CommandMenu[0] = COMMAND_0;
    CommandMenu[1] = COMMAND_1;
    CommandMenu[2] = COMMAND_2;
    CommandMenu[3] = COMMAND_3;
    CommandMenu[4] = COMMAND_4;
    CommandMenu[5] = COMMAND_5;
    CommandMenu[6] = COMMAND_6;

    bFollowMode = 1;
    LCD_ClearDisplay();
    LCD_PrintString("MOVE TO ZERO DEGREES\0");
    LCD_SetPosition(1, 0);
    LCD_PrintString("THEN CLICK...\0");
    GetClick();
    bClear_MotorPosition = 1;
    while (bClear_MotorPosition) Idle();
    Wait_ms(10);
    bFollowMode = 0;
    LCD_ClearDisplay();

    while (1) {
        UI_Location = UI_LOC_MAINMENU;
        ret = DisplayMenu(CommandMenu, 6, lastMenu);
labelProcessCommand:
        switch (ret) {
            case 0:
            case USER_INPUT_MACRO_ORBITMODE:
                lastMenu = 0;
                ret = CreateOrbitProgram();
                if (ret == -2) RealtimeMode();
                if (ret == -1) RealtimeMode();
                if (ret<-2) goto labelProcessCommand;
                break;
            case 1:
            case USER_INPUT_MACRO_WAYMODE:
                lastMenu = 1;
                ret = CreateWaypointProgram();
                if (ret<-2) goto labelProcessCommand;
                if (ret < 0) {
                    ret = RealtimeMode();
                    if (ret<-2) goto labelProcessCommand;
                }
                break;
            case 3:
                lastMenu = 3;
                ret = GetPresetNumber();
                if (ret == -1) break;
                if (ret == -2) break;
                if (ret<-2) goto labelProcessCommand;
                pnum = ret;
                ret = RunPreset(pnum);
                if (ret<-2) goto labelProcessCommand;
                break;
            case 2:
            case USER_INPUT_MACRO_REALMODE:
                lastMenu = 0;
                ret = RealtimeMode();
                if (ret<-2) goto labelProcessCommand;
                break;
            case 4:
                lastMenu = 0;
                ret = ShowVoltage();
                if (ret<-2) goto labelProcessCommand;
                break;
            case 5:
            case USER_INPUT_MACRO_SLEEP:
                lastMenu = 0;
                GoToSleep();
                break;
            case 6:
            case USER_INPUT_MACRO_EXTMODE:
                lastMenu = 0;
                ret = ExtMode();
                if (ret<-2) goto labelProcessCommand;
                break;
            case USER_INPUT_MACRO_RUNPRESET0:
                ret = RunPreset(1);
                if (ret<-2) goto labelProcessCommand;
                break;
            case USER_INPUT_MACRO_RUNPRESET1:
                ret = RunPreset(2);
                if (ret<-2) goto labelProcessCommand;
                break;
            case USER_INPUT_MACRO_RUNPRESET2:
                ret = RunPreset(3);
                if (ret<-2) goto labelProcessCommand;
                break;
            case USER_INPUT_MACRO_RUNPRESET3:
                ret = RunPreset(4);
                if (ret<-2) goto labelProcessCommand;
                break;
            case USER_INPUT_MACRO_RUNPRESET4:
                ret = RunPreset(5);
                if (ret<-2) goto labelProcessCommand;
                break;

        }
    }
}

signed char RunPreset(signed char pnum) {
    signed char ret;
    switch (GetPresetType(pnum)) {
        case 1:
            LoadPreset(pnum, (unsigned char *) &CurrentPath);
            ret = RunWaypointProgram();
            if (ret<-2) return ret;
            if (ret < 0) {
                ret = RealtimeMode();
                if (ret<-2) return ret;
            }
            break;
        case 2:
            LoadPreset(pnum, (unsigned char *) &CurrentOrbitProgram);
            ret = RunOrbitProgram();
            if (ret<-2) return ret;
            if (ret < 0) {
                ret = RealtimeMode();
                if (ret<-2) return ret;
            }
            break;
    }
    return 0;
}

void interrupt FastTick(void) {
    static unsigned char LowTrigger = 80;
    static unsigned char Motor_Encoder = 0;
    static signed long interal_PID_MeasuredPoint = 10;
    static unsigned int TimmingError = 0;
    static unsigned char BacklightPWM_PR = 8;
    unsigned char RXDat;

    //Reset for 40kHz
    TMR0H = 0xFE;
    TMR0L = 0x83;
    TMR0IF = 0; //Clear the Timer Interrupt Flag

    if ((TXSTA1bits.TRMT) && (TX_bCount)) {
        TXREG = TXBuffer[TX_Idx];
        TX_Idx++;
        TX_bCount--;
    }

    if ((RCSTA1bits.OERR) | (RCSTA1bits.FERR)) {
        RCSTA1bits.CREN = 0;
        NOP();
        RCSTA1bits.CREN = 1;
        RXDat = RCREG;
        RXDat = RCREG;
        RX_Idx = 0;
        RX_MsgComplete = 0;
        RX_InProgress = 0;
    }

    if (RC1IF) {
        RXDat = RCREG;
        if (RXDat == '@') {
            RX_Idx = 0;
            RX_MsgComplete = 0;
            RX_InProgress = 1;
        }
        if (RXDat == '#') {
            RX_MsgComplete = 1;
            RX_InProgress = 0;
        }
        if (RX_Idx > 254) RX_Idx = 0;
        if (RX_InProgress) {
            RXBuffer[RX_Idx] = RXDat;
            RX_Idx++;
        }
    }

    if (BacklightPWM_PR) BacklightPWM_PR--;
    else BacklightPWM_PR = 7;
    if (LCD_BacklightEnabled) {
        if (Config.BacklightPWM_Duty > BacklightPWM_PR) LCD_BL = 1;
        else LCD_BL = 0;
    } else LCD_BL = 0;

    //LowTrigger--;
    bTick40Khz = !bTick40Khz;
    /*if (LowTrigger == 0) {
        TimmingError++;
        if (TimmingError == 401) {
            LowTrigger = 79;
            TimmingError = 0;
        } else LowTrigger = 80;
        TMR1IF = 1;
    }*/

    Motor_Encoder <<= 2;
    Motor_Encoder |= MOTOR_ENCODER;
    Motor_Encoder &= 0x0F;
    interal_PID_MeasuredPoint -= enc_states[Motor_Encoder];

    if (bClear_MotorPosition) {
        bClear_MotorPosition = 0;
        interal_PID_MeasuredPoint = 0;
    }

    if (!bLock_Motor_Position) Motor_Position = interal_PID_MeasuredPoint;
}

void interrupt low_priority SlowTick() {
    unsigned int uiTemp;
    static unsigned char RotaryEncoder = 0;
    static signed char RawRotaryEncoder = 0;
    static unsigned char RotaryTickInterval = 0;
    static unsigned char RotaryDetentIntervalIndex = 0;
    static unsigned int SwitchPressDuration = 0;
    static unsigned int internal_BatteryVoltage = 1000;
    static unsigned int LCD_IdleTimer = 0;
    static unsigned long internal_Tick500Hz = 0;
    static signed long internal_PID_SetPoint;
    static signed int PID_LastError;
    static signed long LastMotorPosition = 0;

    static bit isPendingUIEvent = 0;
    static signed char PendingUIEvent = 0;

    static bit isPendingRotaryTick = 0;
    static unsigned int PendingRotaryTick = 0;

    signed int MotorSpeed;
    signed int ResponseOutput;
    signed int PID_Error_delta;
    signed long PID_Error;
    signed long CurrentMotorPosition;
    unsigned long PID_ResponseLimit = 0;

    TMR1H = 0x83;
    TMR1L = 0x1A;
    TMR1IF = 0;


    //---------------------------------------------------------------------------
    //Timer Functions
    //---------------------------------------------------------------------------
    internal_Tick500Hz++;
    if (!bLock_Tick500Hz) Tick500Hz = internal_Tick500Hz;
    if (bWaiting) {
        if (internal_Tick500Hz > WaitUntil_Tick500Hz) {
            bWaiting = 0;
        }
    }

    //---------------------------------------------------------------------------
    //LCD Backlight PWM
    //---------------------------------------------------------------------------
    if (LCD_IdleTimer) {
        LCD_IdleTimer--;
        if (bPowerOff) LCD_BacklightEnabled = 0;
        else LCD_BacklightEnabled = 1;
    } else {
        LCD_BacklightEnabled = 0;
    }

    //---------------------------------------------------------------------------
    //Process the Rotary Input
    //---------------------------------------------------------------------------
    RotaryEncoder <<= 2; //remember previous state
    RotaryEncoder |= UI_ENCODER; //add current state
    RotaryEncoder & = 0x0F;
    RawRotaryEncoder -= enc_states[RotaryEncoder];

    RotaryTickInterval++;
    if (RotaryTickInterval == 0xFF) {
        RotaryTickInterval = 0;
        RawRotaryEncoder = 0;
        ZeroRotaryDetentIntervals = 1;
    }
    if (RawRotaryEncoder > 2) {
        PendingUIEvent = USER_INPUT_INC;
        isPendingUIEvent = 1;

        LCD_IdleTimer = Config.BacklightIdleTimeout;

        isPendingRotaryTick = 1;
        PendingRotaryTick = RotaryTickInterval;
        RotaryTickInterval = 0;

        RawRotaryEncoder = 0;
    }
    if (RawRotaryEncoder<-2) {
        PendingUIEvent = USER_INPUT_DEC;
        isPendingUIEvent = 1;

        LCD_IdleTimer = Config.BacklightIdleTimeout;

        isPendingRotaryTick = 1;
        PendingRotaryTick = RotaryTickInterval;
        RotaryTickInterval = 0;

        RawRotaryEncoder = 0;
    }

    //---------------------------------------------------------------------------
    //Process the Rotary Input Switch
    //---------------------------------------------------------------------------
    if (UI_SWITCH) {
        SwitchPressDuration++;
        LCD_IdleTimer = Config.BacklightIdleTimeout;
    } else {
        if ((SwitchPressDuration > 1) & (SwitchPressDuration < BACK_DURATION)) {
            PendingUIEvent = USER_INPUT_CLICK;
            isPendingUIEvent = 1;
        }
        SwitchPressDuration = 0;
    }
    //if (SwitchPressDuration > RESET_DURATION) SwitchPressDuration = RESET_DURATION;
    if (SwitchPressDuration > RESET_DURATION) RESET();
    if (SwitchPressDuration == CANCEL_DURATION) {
        PendingUIEvent = USER_INPUT_CANCEL;
        isPendingUIEvent = 1;
    }
    if (SwitchPressDuration == BACK_DURATION) {
        PendingUIEvent = USER_INPUT_BACK;
        isPendingUIEvent = 1;
    }

    if (ClearUI_Event) {
        isPendingUIEvent = 0;
        PendingUIEvent = 0;
        UI_Event = 0;
        ClearUI_Event = 0;
        bLock_UI_Event = 0;
    }

    if (!bLock_UI_Event) {
        if (isPendingUIEvent) {
            UI_Event = PendingUIEvent;
            isPendingUIEvent = 0;
        }

        if (isPendingExternalUIEvent) {
            isPendingExternalUIEvent = 0;
            UI_Event = PendingExternalUIEvent;
        }
    }

    if (!bLock_RotaryDetentIntervals) {
        if (isPendingRotaryTick) {
            RotaryDetentIntervals[RotaryDetentIntervalIndex] = PendingRotaryTick;
            RotaryDetentIntervalIndex++;
            if (RotaryDetentIntervalIndex > 15) RotaryDetentIntervalIndex = 0;
            isPendingRotaryTick = 0;
        }
    }

    //---------------------------------------------------------------------------
    //Grab the Current Motor's Position in an Interrupt Safe Way
    //---------------------------------------------------------------------------
    bLock_Motor_Position = 1;
    CurrentMotorPosition = Motor_Position;
    bLock_Motor_Position = 0;

    //---------------------------------------------------------------------------
    //Process the Motion Trajectory Engine
    //---------------------------------------------------------------------------
    if (bInjectMoveValues) {
        bInjectMoveValues = 0;
        Move_AccelValueQ24 = SetMove_AccelValueQ24;
        Move_CoastSpeedQ24 = SetMove_CoastSpeedQ24;
        Move_DecelPosition = SetMove_DecelPosition;
        Move_FinalPosition = SetMove_FinalPosition;
    }
    if (bMove_InProgress) {
        bFollowMode = 0;
        if (Move_shifted_position.ul > Move_DecelPosition) {
            Move_speedQ24 -= Move_AccelValueQ24;
            if (Move_speedQ24 < Move_AccelValueQ24) {
                internal_PID_SetPoint = Move_FinalPosition;
                Move_speedQ24 = 0;
                if (bMove_Queued) {
                    bMove_Queued = 0;
                    Move_AccelValueQ24 = QueuedMove_AccelValueQ24;
                    Move_CoastSpeedQ24 = QueuedMove_CoastSpeedQ24;
                    Move_DecelPosition = QueuedMove_DecelPosition;
                    Move_FinalPosition = QueuedMove_FinalPosition;
                    Move_Origin = Move_FinalPosition;
                    Move_position[0].ul = 0;
                    Move_position[1].ul = 0;
                    bMove_Neg = bQueuedMove_Neg;
                } else {
                    bMove_InProgress = 0;
                }
            }
        } else {
            if (Move_speedQ24 < Move_CoastSpeedQ24) {
                Move_speedQ24 += Move_AccelValueQ24;
                if (Move_speedQ24 > Move_CoastSpeedQ24) Move_speedQ24 = Move_CoastSpeedQ24;
            }

            if (Move_speedQ24 > Move_CoastSpeedQ24) {
                Move_speedQ24 -= Move_AccelValueQ24;
                if (Move_speedQ24 < Move_CoastSpeedQ24) Move_speedQ24 = Move_CoastSpeedQ24;
            }
        }

        Move_position[0].ul += Move_speedQ24;
        if (Move_position[0].ul < Move_speedQ24) Move_position[1].ul++;

        Move_shifted_position.ub[0] = Move_position[0].ub[3];
        Move_shifted_position.ub[1] = Move_position[1].ub[0];
        Move_shifted_position.ub[2] = Move_position[1].ub[1];
        Move_shifted_position.ub[3] = Move_position[1].ub[2];

        if (bMove_InProgress) {
            if (bMove_Neg) internal_PID_SetPoint = Move_Origin - Move_shifted_position.ul;
            else internal_PID_SetPoint = Move_Origin + Move_shifted_position.ul;
        }

    } else if (bFollowMode) {
        PID_Error = internal_PID_SetPoint - CurrentMotorPosition;
        internal_PID_SetPoint = CurrentMotorPosition;
        //internal_PID_SetPoint -= (PID_Error >> 1);
    } else if (bSpeedMode) {
        if (Speed_SetPending) {
            Speed_SetPending = 0;
            Move_CoastSpeedQ24 = Speed_SetToThis;
        }

        if (Move_speedQ24 < Move_CoastSpeedQ24) {
            Speed_IsAccelerating = 1;
            Move_speedQ24 += Move_AccelValueQ24;
            if (Move_speedQ24 > Move_CoastSpeedQ24) Move_speedQ24 = Move_CoastSpeedQ24;
        } else if (Move_speedQ24 > Move_CoastSpeedQ24) {
            Speed_IsAccelerating = 1;
            if (Move_speedQ24 < Move_AccelValueQ24) Move_speedQ24 = 0;
            else Move_speedQ24 -= Move_AccelValueQ24;
            if (Move_speedQ24 < Move_CoastSpeedQ24) Move_speedQ24 = Move_CoastSpeedQ24;
        } else Speed_IsAccelerating = 0;

        Move_position[0].ul += Move_speedQ24;
        if (Move_position[0].ul < Move_speedQ24) Move_position[1].ul++;

        Move_shifted_position.ub[0] = Move_position[0].ub[3];
        Move_shifted_position.ub[1] = Move_position[1].ub[0];
        Move_shifted_position.ub[2] = Move_position[1].ub[1];
        Move_shifted_position.ub[3] = Move_position[1].ub[2];

        if (bMove_Neg) internal_PID_SetPoint = Move_Origin - Move_shifted_position.ul;
        else internal_PID_SetPoint = Move_Origin + Move_shifted_position.ul;
    }



    //---------------------------------------------------------------------------
    //Report back the PID Setpoint in a interrupt safe manner.
    //---------------------------------------------------------------------------
    //if (!bLock_PID_SetPoint) PID_SetPoint = internal_PID_SetPoint;


    //---------------------------------------------------------------------------
    //Read the Battery Level which is filtered simply by single incrementing.  It will take ~2000 cycles to get the real battery value...
    //Check for low battery voltage trips
    //---------------------------------------------------------------------------
    uiTemp = ADRESH;
    uiTemp <<= 8;
    uiTemp += ADRESL;
    GO = 1; //Start the next conversion
    signed int diff = uiTemp - internal_BatteryVoltage;
    diff >>= 4;
    internal_BatteryVoltage += diff;
    if (uiTemp > internal_BatteryVoltage) internal_BatteryVoltage++;
    if (uiTemp < internal_BatteryVoltage) internal_BatteryVoltage--;
    if (!bLock_BatteryVoltage) BatteryVoltage = internal_BatteryVoltage;
    if (internal_BatteryVoltage < Config.BatteryLowVoltageLevel) bLowVoltageTrip = 1;

    //---------------------------------------------------------------------------
    //Figure out the Power Scaling...
    //---------------------------------------------------------------------------    
    MotorSpeed = LastMotorPosition - CurrentMotorPosition;
    LastMotorPosition = CurrentMotorPosition;
    if (MotorSpeed < 0) MotorSpeed = -MotorSpeed;

    //First we get a linear scale from 1Tick/Count=50% power to 23Ticks/Count=100% power
    //The resulting Limit is from 128 to 256    
    uiTemp = (MotorSpeed * Config.PowerResponseLimiter_Slope);
    uiTemp += Config.PowerResponseLimiter_Intercept;
    if (uiTemp > 0xFF) uiTemp = 0xFF;

    //Now use Fixed-Point Math to calculate the battery voltage scaling
    //A 10V battery would require a higher PWM value to power the motor with the right voltage than a 15V Battery
    unsigned char ucBatt = (internal_BatteryVoltage >> 4);
    unsigned int BatteryScaleQ8 = Config.Battery12V_Q8 / ucBatt;

    PID_ResponseLimit = uiTemp;
    PID_ResponseLimit *= BatteryScaleQ8;
    PID_ResponseLimit >>= 6;
    if (PID_ResponseLimit > 1023) PID_ResponseLimit = 1023;
    PID_ResponseLimit = 1023;

    //---------------------------------------------------------------------------
    //Calculate the PID Error
    //---------------------------------------------------------------------------

    PID_Error = internal_PID_SetPoint - CurrentMotorPosition;
    if (PID_Error > Config.PID_MaxError) {
        PID_Error = Config.PID_MaxError;
        bMotionError = 1;
    } else if (PID_Error<-Config.PID_MaxError) {
        PID_Error = -Config.PID_MaxError;
        bMotionError = 1;
    }

    //---------------------------------------------------------------------------
    //Calculate the PID Response
    //---------------------------------------------------------------------------
    PID_Error_delta = PID_Error - PID_LastError;
    PID_LastError = PID_Error;

    ResponseOutput = (long) Config.PID_Kp * (long) PID_Error;
    ResponseOutput += (long) Config.PID_Kd * (long) PID_Error_delta;

    //---------------------------------------------------------------------------
    //Set the PWM Output and Direction
    //---------------------------------------------------------------------------
    if (ResponseOutput < 0) {
        MOTOR_FORWARD = 0;
        MOTOR_REVERSE = 1;
        ResponseOutput = -ResponseOutput;
    } else {
        MOTOR_FORWARD = 1;
        MOTOR_REVERSE = 0;
    }
    if (ResponseOutput > PID_ResponseLimit) ResponseOutput = PID_ResponseLimit;

    if (bPowerOff) {
        MOTOR_FORWARD = 0;
        MOTOR_REVERSE = 0;
        ResponseOutput = 0;
        internal_PID_SetPoint = CurrentMotorPosition;
    }

    //Enforce the Minimum Off Time Requirements for the PWM Driver IC (6us Min)
    //As the time on increases the time off decreases. So once we get above a certain
    //level we need to skip righ to 100% on.
    if (ResponseOutput > 900) ResponseOutput = 0x3FF;

    //Invert the output due to the NPN transistors.
    ResponseOutput = 0x3FF - ResponseOutput;
    //Set the PWM Duty Cycle MSB's
    ResponseOutput >>= 2;


    MOTOR_PWM_H = (unsigned char) ResponseOutput;
}

void SetupHardware() {
    //Interal Oscillator @ 16Mhz
    OSCCON = 0b01110000;

    //Enable the 4X PLL
    PLLEN = 1;


    TMR0ON = 0; //Turn it off for now
    T08BIT = 0; //16 bit mode
    T0CS = 0; //Internal Clock (Fosc/4 = 16 mHz)
    PSA = 1; //Bypass the prescaler

    //Setup for 800 Cycles = 20kHz
    TMR0H = 0xFC;
    TMR0L = 0xF2;

    TMR0IF = 0; //Clear the Timer Interrupt Flag
    TMR0IE = 1; //Enable the timer interrupt
    TMR0IP = 1; //Timer 0 is High level Interrupt

    TMR0ON = 1; //ON!


    //Setup Timer 1
    //Setup for 32000 Cycles = 500Hz
    TMR1H = 0x83;
    TMR1L = 0x50;

    TMR1IF = 0; //Clear the Timer Interrupt Flag
    TMR1IE = 1; //Enable the timer interrupt
    TMR1IP = 0; //Timer 1 is Low Level interrupt

    TMR1ON = 1;

    //Setup PWM
    TMR2 = 0;

    //Setup for 15.56kHz Operation
    PR2 = 0xFF;
    //Set TMR2 Prescaler to 4:1
    T2CONbits.T2CKPS0 = 1;
    T2CONbits.T2CKPS1 = 0;

    //Ensure 0% output
    MOTOR_PWM_H = 0;

    //Make the PWM pin and output
    MOTOR_PWM_TRIS = 0;

    //Activate Timer2
    TMR2ON = 1;

    //Turn on CCP1
    CCP1CON = 0b00001100;

    //Setup Serial UART
    //Setup Pins
    TRISCbits.TRISC7 = 1;
    TRISCbits.TRISC = 0;

    //Setup USART
    TXSTA1bits.BRGH = 1; //High Bit Rate Mode
    BAUDCON1bits.BRG16 = 1; //16-bit Baud Rate Generator Only
    BAUDCON1bits.RXDTP = 0; //RX signal is NOT inverted (Active-high)
    BAUDCON1bits.TXCKP = 0; //TX Signal is NOT inverted (Active-high)
    BAUDCON1bits.WUE = 0; //Wake Up is NOT enabled
    BAUDCON1bits.ABDEN = 0; //Auto baud rate is NOT enabled


    //Fosc=64Mhz
    //Baud=115200bps
    //BRGH=1, 16-Bit
    //Baud=Fosc/[4/(BRG+1)]
    //BRG=137 (115,942bps @ 0.64% Error)
    SPBRGH1 = 0;
    SPBRG1 = 137;

    TXSTA1bits.SYNC = 0; //Async Mode
    RCSTA1bits.SPEN = 1; //Serial Port Enabled

    TX1IE = 0; //Disable Interrupts
    TXSTA1bits.TX9 = 0; //8-bit Mode
    TXSTA1bits.TXEN = 1; //TX Enabled

    RC1IE = 0; //Disable Interrupts
    RCSTA1bits.RC9 = 0; //8-bit Mode
    RCSTA1bits.CREN = 1; //Async Mode Enable Reciever

    IPEN = 1; //Enable Interrupt Priority
    PEIE = 1; //Peripheral Interrupts Enabled.
    GIEL = 1; //Low Level Interrupts Enabled.
    ei(); //Enable Interrupts


}

void SetupIO() {
    //Setup IO Pins
    //PORTA:
    //RA0 - MOTOR ENCODER A (INPUT)
    //RA1 - MOTOR ENCODER A (INPUT)
    //RA2 - N/C (Output)
    //RA3 - N/C (Output)
    //RA4 - N/C (Output)
    //RA5 - N/C (Output)
    //RA6 - OSC. OUT (Output)
    //RA7 - OSCILLATOR (Input)
    LATA = 0b00000000;
    TRISA = 0b10000011;


    //PORTB
    //RB0 - Motor Reverse (Output)
    //RB1 - LCD Back Light (Output)
    //RB2 - UI Switch (Input)
    //RB3 - UI Encoder A (Input)
    //RB4 - UI Encoder B (Input)
    //RB5 - N/C (Output)
    //RB6 - Program Clock (PGC) (Output)
    //RB7 - Pogram Data (PGD) (Output)
    LATB = 0b00000000;
    TRISB = 0b00011100;
    RBPU = 0; //Enable Pull Ups


    //PORTC
    //RC0 - Motor Forward (Output)
    //RC1 - N/C (Output)
    //RC2 - Motor PWM (Output)
    //RC3 - N/C (Output)
    //RC4 - N/C (Output)
    //RC5 - N/C (Output)
    //RC6 - MCU Serial TX (Output)
    //RC7 - MCU Serial RX (Input)
    LATC = 0b00000000;
    TRISC = 0b10000000;

    //PORTD
    //RB0-RB7 - LCD Data Bus (Outputs)
    LATD = 0b00000000;
    TRISD = 0b00000000;

    //PORTE
    //RE0 - N/C (Output)
    //RE1 - N/C (Output)
    //RE2 - N/C (Output)
    //RE3 - N/C (Output)
    //RE4 - N/C (Output)
    //RE5 - LCD RS (Output)
    //RE6 - LCD R/W (Output)
    //RE7 - LCD E Strobe (Output)
    LATE = 0b00000000;
    TRISE = 0b00000000;

    //PORTF
    //RF0 - NO PIN (Output)
    //RF1 - N/C (Output)
    //RF2 - Battery Voltage (Input)
    //RF3 - N/C (Output)
    //RF4 - N/C (Output)
    //RF5 - N/C (Output)
    //RF6 - N/C (Output)
    //RF7 - N/C (Output)
    LATF = 0b00000000;
    TRISF = 0b00000100;

    //PORTG
    //RG0 - N/C (Output)
    //RG1 - N/C (Output)
    //RG2 - N/C (Output)
    //RG3 - N/C (Output)
    //RG4 - N/C (Output)
    //RG5 - No PIN (Output)
    //RG6 - No PIN (Output)
    //RG7 - No PIN (Output)
    LATG = 0b00000000;
    TRISG = 0b00000000;

    //Setup ADC Pins
    //AN7 - Battery Input
    //All Others --- DISABLE ---
    ANCON0 = 0b10000000;
    ANCON1 = 0;
    ANCON2 = 0;

    //Setup ADC
    //AN7 Selected & ADC Off
    ADCON0 = 0b00011100;
    //Right Justified, ADC Clock= Fosc/64, ACQ Time = 20TAD
    ADCON2 = 0b10111110;
    //Turn on the ADC
    ADON = 1;
}

void AckCmd(unsigned char CmdID) {
    TX_Idx = 0;
    TXBuffer[TX_Idx++] = '$';
    MessageStream_WriteByte(CmdID);
    TXBuffer[TX_Idx++] = '#';
    TX_Idx = 0;
    TX_bCount = 4;
}

void NackCmd(unsigned char CmdID, unsigned char Reason) {
    TX_Idx = 0;
    TXBuffer[TX_Idx++] = '!';
    MessageStream_WriteByte(CmdID);
    MessageStream_WriteByte(Reason);
    TXBuffer[TX_Idx++] = '#';
    TX_Idx = 0;
    TX_bCount = 6;
}

void Idle() {
    unsigned int addr = 0;
    unsigned char MsgAddr = 0;
    unsigned char PresetNumber = 0;
    unsigned char *ConfigPtr;
    ConfigStruct TempConfig;
    char idx;
    MULTI temp;
    PRESET tempPset;

    if (ExtModeActive) {
        idx = 255;
        while (idx--);
        return;
    }

    if (!RX_MsgComplete) {
        idx = 255;
        while (idx--);
        return;
    }

    ChkSum = 0;
    ReadIdx = 1;
    RX_MsgComplete = 0;
    MsgAddr = MessageStream_ReadByte();
    if (MsgAddr == 0xFF) {
        TX_Idx = 0;
        TXBuffer[TX_Idx++] = '$';
        MessageStream_WriteByte(MyID);
        MessageStream_WriteByte('O');
        MessageStream_WriteByte('R');
        MessageStream_WriteByte('B');
        MessageStream_WriteByte('I');
        MessageStream_WriteByte('T');
        TXBuffer[TX_Idx++] = '#';
        idx = TX_Idx;
        TX_Idx = 0;
        TX_bCount = idx;
    } else if (MsgAddr == MyID) {
        unsigned char CmdID = MessageStream_ReadByte();
        switch (CmdID) {
            case CMD_GET_COMPLETE_STATUS:
                TX_Idx = 0;
                TXBuffer[TX_Idx++] = '$';
                MessageStream_WriteByte(CmdID);
                for (idx = 0; idx < 20; idx++) {
                    MessageStream_WriteByte(LCD_Line2[idx]);
                    ChkSum += LCD_Line2[idx];
                }
                for (idx = 0; idx < 20; idx++) {
                    MessageStream_WriteByte(LCD_Line1[idx]);
                    ChkSum += LCD_Line1[idx];
                }
                bLock_Motor_Position = 1;
                temp.dbl = Motor_Position;
                bLock_Motor_Position = 0;
                temp.dbl *= Config.Degrees_Per_Count;
                MessageStream_WriteByte(temp.ub[0]);
                MessageStream_WriteByte(temp.ub[1]);
                MessageStream_WriteByte(temp.ub[2]);
                MessageStream_WriteByte(temp.ub[3]);
                ChkSum += temp.ub[0];
                ChkSum += temp.ub[1];
                ChkSum += temp.ub[2];
                ChkSum += temp.ub[3];

                temp.dbl = 0;
                while (!(temp.dbl == Move_speedQ24)) temp.dbl = Move_speedQ24;
                temp.dbl *= INV_Conversion_Q24_500Hz;
                temp.dbl *= Config.Degrees_Per_Count;
                MessageStream_WriteByte(temp.ub[0]);
                MessageStream_WriteByte(temp.ub[1]);
                MessageStream_WriteByte(temp.ub[2]);
                MessageStream_WriteByte(temp.ub[3]);
                ChkSum += temp.ub[0];
                ChkSum += temp.ub[1];
                ChkSum += temp.ub[2];
                ChkSum += temp.ub[3];

                temp.dbl = 0;
                bLock_BatteryVoltage = 1;
                temp.dbl = BatteryVoltage;
                bLock_BatteryVoltage = 0;
                temp.dbl *= Config.Volts_per_Count;

                MessageStream_WriteByte(temp.ub[0]);
                MessageStream_WriteByte(temp.ub[1]);
                MessageStream_WriteByte(temp.ub[2]);
                MessageStream_WriteByte(temp.ub[3]);
                ChkSum += temp.ub[0];
                ChkSum += temp.ub[1];
                ChkSum += temp.ub[2];
                ChkSum += temp.ub[3];

                MessageStream_WriteByte(ChkSum);
                TXBuffer[TX_Idx++] = '#';
                idx = TX_Idx;
                TX_Idx = 0;
                TX_bCount = idx;
                break;
            case CMD_GETDISPLAY:
                TX_Idx = 0;
                TXBuffer[TX_Idx++] = '$';
                MessageStream_WriteByte(CmdID);
                for (idx = 0; idx < 20; idx++) {
                    MessageStream_WriteByte(LCD_Line2[idx]);
                    ChkSum += LCD_Line2[idx];
                }
                for (idx = 0; idx < 20; idx++) {
                    MessageStream_WriteByte(LCD_Line1[idx]);
                    ChkSum += LCD_Line1[idx];
                }
                MessageStream_WriteByte(ChkSum);
                TXBuffer[TX_Idx++] = '#';
                idx = TX_Idx;
                TX_Idx = 0;
                TX_bCount = idx;
                break;
            case CMD_UI_CLICK:
                isPendingExternalUIEvent = 0;
                PendingExternalUIEvent = USER_INPUT_CLICK;
                isPendingExternalUIEvent = 1;
                AckCmd(CmdID);
                break;
            case CMD_UI_CANCEL:
                isPendingExternalUIEvent = 0;
                PendingExternalUIEvent = USER_INPUT_CANCEL;
                isPendingExternalUIEvent = 1;
                AckCmd(CmdID);
                break;
            case CMD_UI_BACK:
                isPendingExternalUIEvent = 0;
                PendingExternalUIEvent = USER_INPUT_BACK;
                isPendingExternalUIEvent = 1;
                AckCmd(CmdID);
                break;
            case CMD_UI_INC:
                isPendingExternalUIEvent = 0;
                PendingExternalUIEvent = USER_INPUT_INC;
                isPendingExternalUIEvent = 1;
                AckCmd(CmdID);
                break;
            case CMD_UI_DEC:
                isPendingExternalUIEvent = 0;
                PendingExternalUIEvent = USER_INPUT_DEC;
                isPendingExternalUIEvent = 1;
                AckCmd(CmdID);
                break;
            case CMD_UI_MACRO:
                isPendingExternalUIEvent = 0;
                PendingExternalUIEvent = -MessageStream_ReadByte();
                isPendingExternalUIEvent = 1;
                AckCmd(CmdID);
                break;
            case CMD_GET_UI_LOC:
                TX_Idx = 0;
                TXBuffer[TX_Idx++] = '$';
                MessageStream_WriteByte(CmdID);
                MessageStream_WriteByte(UI_Location);
                MessageStream_WriteByte(UI_Location);
                TXBuffer[TX_Idx++] = '#';
                idx = TX_Idx;
                TX_Idx = 0;
                TX_bCount = idx;
                break;
            case CMD_GET_POS:
                bLock_Motor_Position = 1;
                temp.dbl = Motor_Position;
                bLock_Motor_Position = 0;
                temp.dbl *= Config.Degrees_Per_Count;
                MessageStream_WriteMULTI(temp, CmdID);
                break;
            case CMD_GET_SPEED:
                temp.dbl = 0;
                while (!(temp.dbl == Move_speedQ24)) temp.dbl = Move_speedQ24;
                temp.dbl *= INV_Conversion_Q24_500Hz;
                temp.dbl *= Config.Degrees_Per_Count;
                MessageStream_WriteMULTI(temp, CmdID);
                break;
            case CMD_GET_BATTERY:
                temp.dbl = 0;
                bLock_BatteryVoltage = 1;
                temp.dbl = BatteryVoltage;
                bLock_BatteryVoltage = 0;
                temp.dbl *= Config.Volts_per_Count;
                MessageStream_WriteMULTI(temp, CmdID);
                break;
            case CMD_GET_PRESET:
                PresetNumber = MessageStream_ReadByte();

                if (PresetNumber > 5) {
                    NackCmd(CmdID, 1);
                    break;
                }
                if (PresetNumber == 0) {
                    NackCmd(CmdID, 1);
                    break;
                }

                LoadPreset(PresetNumber, (unsigned char *) &tempPset);
                TX_Idx = 0;
                TXBuffer[TX_Idx++] = '$';
                MessageStream_WriteByte(CmdID);
                for (idx = 0; idx<sizeof (tempPset); idx++) {
                    MessageStream_WriteByte(tempPset.ub[idx]);
                    ChkSum += tempPset.ub[idx];
                }
                MessageStream_WriteByte(ChkSum);
                TXBuffer[TX_Idx++] = '#';
                idx = TX_Idx;
                TX_Idx = 0;
                TX_bCount = idx;
                break;
            case CMD_SET_PRESET:
                PresetNumber = MessageStream_ReadByte();
                if (PresetNumber > 5) {
                    NackCmd(CmdID, 1);
                    break;
                }
                if (PresetNumber == 0) {
                    NackCmd(CmdID, 1);
                    break;
                }

                ChkSum += PresetNumber;

                for (idx = 0; idx < 120; idx++) {
                    tempPset.ub[idx] = MessageStream_ReadByte();
                    ChkSum += tempPset.ub[idx];
                }

                if (ChkSum == MessageStream_ReadByte()) {
                    SavePreset(PresetNumber, (unsigned char *) &tempPset);
                    AckCmd(CmdID);
                } else {
                    NackCmd(CmdID, ChkSum);
                }
                break;
            case CMD_GET_CONFIG:
                ConfigPtr = (unsigned char *) &Config;
                TX_Idx = 0;
                TXBuffer[TX_Idx++] = '$';
                MessageStream_WriteByte(CmdID);
                for (idx = 0; idx<sizeof (Config); idx++) {
                    MessageStream_WriteByte((*ConfigPtr));
                    ChkSum += (*ConfigPtr);
                    ConfigPtr++;
                }
                MessageStream_WriteByte(ChkSum);
                TXBuffer[TX_Idx++] = '#';
                idx = TX_Idx;
                TX_Idx = 0;
                TX_bCount = idx;
                break;
            case CMD_SET_CONFIG:
                ConfigPtr = (unsigned char *) &TempConfig;
                for (idx = 0; idx < sizeof (Config); idx++) {
                    (*ConfigPtr) = MessageStream_ReadByte();
                    ChkSum += (*ConfigPtr);
                    ConfigPtr++;
                }

                if (ChkSum == MessageStream_ReadByte()) {
                    addr=0;
                    EEprom_write(&addr, (unsigned char *) &TempConfig, sizeof (TempConfig));
                    AckCmd(CmdID);
                } else {
                    NackCmd(CmdID, ChkSum);
                }
                break;
            default:
                NackCmd(CmdID, 0xFF);
        }
    }
    return;
}

void MessageStream_WriteMULTI(MULTI dat, unsigned char CmdID) {
    unsigned char bCount = 0;
    TX_Idx = 0;
    TXBuffer[TX_Idx++] = '$';
    MessageStream_WriteByte(CmdID);
    MessageStream_WriteByte(dat.ub[0]);
    MessageStream_WriteByte(dat.ub[1]);
    MessageStream_WriteByte(dat.ub[2]);
    MessageStream_WriteByte(dat.ub[3]);
    ChkSum += dat.ub[0];
    ChkSum += dat.ub[1];
    ChkSum += dat.ub[2];
    ChkSum += dat.ub[3];
    MessageStream_WriteByte(ChkSum);
    TXBuffer[TX_Idx++] = '#';
    bCount = TX_Idx;
    TX_Idx = 0;
    TX_bCount = bCount;
}

void MessageStream_WriteByte(unsigned char dat) {
    unsigned char temp;
    temp = dat >> 4;
    TXBuffer[TX_Idx++] = BINARY_TO_ASCII_HEX[temp];
    temp = dat & 0x0F;
    TXBuffer[TX_Idx++] = BINARY_TO_ASCII_HEX[temp];
}

unsigned char MessageStream_ReadByte() {
    unsigned char temp = ASCII_HEX_TO_BINARY[RXBuffer[ReadIdx++]];
    temp <<= 4;
    temp += ASCII_HEX_TO_BINARY[RXBuffer[ReadIdx++]];
    return temp;
}

signed char GoToSleep() {
    signed char retCode = 0;
    UI_Location = UI_LOC_SLEEP;
    signed char inp;
    int bin;
    char idx;

    LCD_ClearDisplay();
    LCD_PrintString("SLEEP\0");
    LCD_DisplayOnOff(0, 0, 0);
    bPowerOff = 1;

    double Expires;
    double Time;

step1:
    ClearUI_Event = 1;
    while (ClearUI_Event);
    while (1) {
        Idle();
        inp = GetInput_nonblocking();
        if (inp<-2) {
            retCode = inp;
            goto WAKEUP;
        }
        switch (inp) {
            case USER_INPUT_CANCEL: //The user pressed cancel
                Expires = Now();
                Expires += 5000;
                bin = 0;
                goto step2;
        }
        idx = 0xff;
        while (idx--);
    }

step2:
    while (1) {
        Idle();
        inp = GetInput_nonblocking();
        if (inp<-2) {
            retCode = inp;
            goto WAKEUP;
        }
        Time = Now();
        if (Time > Expires) goto step1;

        switch (inp) {
            case USER_INPUT_INC: //The user pressed cancel
                bin++;
                if (bin > 8) goto WAKEUP;
                break;
            case USER_INPUT_DEC:
                bin = 0;
                break;
        }
        idx = 0xff;
        while (idx--);
    }
WAKEUP:
    LCD_DisplayOnOff(1, 0, 0);
    bPowerOff = 0;
    return retCode;
}

