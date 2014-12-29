#include <htc.h>
#include <math.h>
#include "main.h"
#include "timings.h"
#include "UserInterface.h"
#include "WaypointProgram.h"
#include "settings.h"
#include "OrbitProgram.h"
#include "RealtimeMode.h"
#include "MotionEngine.h"
#include "ExtMode.h"
#include "Version.h"
#include "lcd.h"

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

    bFollowMode = 0;
    LCD_ClearDisplay();
    bPowerOff=1;
    bPowerOff=0;

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
    static signed long interal_PID_MeasuredPoint = 10;
    unsigned char RXDat;
    unsigned int internal_BatteryVoltage=0;
    unsigned int internal_MotorCurrent=0;

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

    if (INTCONbits.INT0IF) {
        if (PORTBbits.RB1) interal_PID_MeasuredPoint++; else interal_PID_MeasuredPoint--;
        INTCONbits.INT0IF=0;
    }

    if (PIR1bits.ADIF) {
        if (IsSamplingVoltage) {
            ADCON0bits.CHS=4;
            IsSamplingVoltage=0;
            internal_BatteryVoltage=ADRESH;
            internal_BatteryVoltage<<=8;
            internal_BatteryVoltage+=ADRESL;
            if (!bLock_BatteryVoltage) BatteryVoltage = internal_BatteryVoltage;
        } else {
            ADCON0bits.CHS=11;
            IsSamplingVoltage=1;
            internal_MotorCurrent=ADRESH;
            internal_MotorCurrent<<=8;
            internal_MotorCurrent+=ADRESL;
            if (!bLock_MotorCurrent) MotorCurrent = internal_MotorCurrent;
        }
        ADCON0bits.GO=1;
        PIR1bits.ADIF=0;
    }

    if (TMR0IF)
    {
        //Reset for 40kHz
        TMR0H = 0xFE;
        TMR0L = 0x83;
        TMR0IF = 0; //Clear the Timer Interrupt Flag

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

        if ((TXSTA1bits.TRMT) && (TX_bCount)) {
            TXREG = TXBuffer[TX_Idx];
            TX_Idx++;
            TX_bCount--;
        }

        if (bClear_MotorPosition) {
            bClear_MotorPosition = 0;
            interal_PID_MeasuredPoint = 0;
        }

        if (!bLock_Motor_Position) Motor_Position = interal_PID_MeasuredPoint;

        bTick40Khz = !bTick40Khz;
    }
    
}

void interrupt low_priority SlowTick() {
    unsigned int internal_BatteryVoltage = 1000;
    unsigned int internal_MotorCurrent=0;
    static unsigned long internal_Tick500Hz = 0;
    static signed long internal_PID_SetPoint;
    static signed int PID_LastError;

    signed int ResponseOutput;
    signed int PID_Error_delta;
    signed long PID_Error;
    signed long CurrentMotorPosition;
    signed int PID_ResponseLimit = 0;

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
    //External UI Event Implementation
    //---------------------------------------------------------------------------
    if (ClearUI_Event)
    {
        UI_Event=0;
        isPendingExternalUIEvent=0;
        PendingExternalUIEvent=0;
        ClearUI_Event=0;
    }
    
    if (!bLock_UI_Event) {
        if (isPendingExternalUIEvent) {
            isPendingExternalUIEvent = 0;
            UI_Event = PendingExternalUIEvent;
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
    if (bMove_InProgress) {
        bFollowMode = 0;
        if (Move_shifted_position.ul > Move_DecelPosition) {
            Move_speedQ24 -= Move_AccelValueQ24;
            if (Move_speedQ24 < Move_AccelValueQ24) {
                internal_PID_SetPoint = Move_FinalPosition;
                Move_speedQ24 = 0;
                bMove_InProgress = 0;
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
    //Read the Battery Level which is done in the fast hardware loop
    //---------------------------------------------------------------------------
    bLock_BatteryVoltage=1;
    internal_BatteryVoltage=BatteryVoltage;
    bLock_BatteryVoltage=0;

    bLock_MotorCurrent=1;
    internal_MotorCurrent=MotorCurrent;
    bLock_MotorCurrent=0;

    unsigned char VoltageRL = 255;
    unsigned char CurrentRL = 255;

    //---------------------------------------------------------------------------
    //Figure out the Power Scaling...
    //---------------------------------------------------------------------------
    if (internal_BatteryVoltage<2068) {
        VoltageRL = 0;
    } else if (internal_BatteryVoltage<2322) {
        VoltageRL=internal_BatteryVoltage-2068;
    }

    if (internal_MotorCurrent>1473) {
        CurrentRL=0;
    } else if (internal_MotorCurrent>1346) {
        CurrentRL=255-((internal_MotorCurrent-1346)<<1);
    }

    if (CurrentRL<VoltageRL) PID_ResponseLimit=(signed int)CurrentRL; else PID_ResponseLimit=(signed int)VoltageRL;
    PID_ResponseLimit>>=2;
    PID_ResponseLimit=1024;

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
    OSCCONbits.IRCF=0b111;

    //Enable the 4X PLL
    PLLEN=1;

    SetupIO();

    //-------------------------------------------
    //Setup Fast 20kHz Timer
    //-------------------------------------------
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

    //-------------------------------------------
    //Setup Slower 500Hz Timer
    //-------------------------------------------
    //Setup Timer 1
    //Setup for 32000 Cycles = 500Hz
    TMR1H = 0x83;
    TMR1L = 0x50;

    TMR1IF = 0; //Clear the Timer Interrupt Flag
    TMR1IE = 1; //Enable the timer interrupt
    TMR1IP = 0; //Timer 1 is Low Level interrupt

    TMR1ON = 1;


    //-------------------------------------------
    //Setup PWM
    //-------------------------------------------
    TMR2 = 0;

    //Setup for 15.56kHz Operation
    PR2 = 0xFF;
    //Set TMR2 Prescaler to 4:1
    T2CONbits.T2CKPS0 = 1;
    T2CONbits.T2CKPS1 = 0;

    //Ensure 0% output
    MOTOR_PWM_H = 0;

    //Activate Timer2
    TMR2ON = 1;

    //Turn on CCP1
    CCP2CON = 0b00001100;

    //-------------------------------------------
    //Setup Serial UART
    //-------------------------------------------
    //Setup Pins
    TRISCbits.TRISC7 = 1;
    TRISCbits.TRISC = 0;

    //Setup USART
    //Setup Pins
    TRISCbits.TRISC7 = 1;
    TRISCbits.TRISC6 = 0;

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

    //-------------------------------------------
    //Setup ENCODER
    //-------------------------------------------
    //Mode 4X
    LATBbits.LATB2=0;
    TRISBbits.TRISB2=1;

    //Interrupt on Rising Edge
    INTCON2bits.INTEDG0=1;

    //Clear the Flag then enable the interrupt...
    INTCONbits.INT0IF=0;
    INTCONbits.INT0IE=1;

    //-------------------------------------------
    //Setup ADC
    //-------------------------------------------
    //Setup to acquire the system voltage first...
    ADCON0bits.CHS=11;

    //Set +VREF=AVDD
    ADCON1bits.VCFG=0;

    //Set -VREF=AVss
    ADCON1bits.VNCFG=0;

    //Set Differential Amp (-) to Vss
    ADCON1bits.CHSN=0;

    //Right Justified Format
    ADCON2bits.ADFM=1;

    ADCON2bits.ACQT=0b111; //Aquisition Time 20TAD

    ADCON2bits.ADCS=0b110; //ADC Clock = Fosc/64

    ANCON0=0;
    ANCON0bits.ANSEL4=1;
    ANCON1=0;
    ANCON1bits.ANSEL11=1;
    ANCON2=0;

    ADCON0bits.ADON=1;

    PIR1bits.ADIF=0;
    PIE1bits.ADIE=1;

    IsSamplingVoltage=1;
    ADCON0bits.GO=1;

    //-------------------------------------------
    //Enable Interrupts
    //-------------------------------------------
    IPEN = 1; //Enable Interrupt Priority
    PEIE = 1; //Peripheral Interrupts Enabled.
    GIEL = 1; //Low Level Interrupts Enabled.
    ei(); //Enable Interrupts


}

void SetupIO() {
    //PORTA:
    //RA0 - N/C (Output)
    //RA1 - N/C (Output)
    //RA2 - N/C (Output)
    //RA3 - N/C (Output)
    //RA4 - ServoX (Output)
    //RA5 - Current Sense (Analog Input)
    //RA6 - LED#4 (Input) - Open Collector Pull Low to turn on
    //RA7 - LED#5 (Input) - Open Collector Pull Low to turn on
    LATA  = 0b00000000;
    TRISA = 0b11100000;


    //PORTB
    //RB0 - Encoder Clock (Input)
    //RB1 - Encoder Up/Down (Input)
    //RB2 - Encoder Mode (Output)
    //RB3 - LED#1
    //RB4 - LED#2
    //RB5 - LED#3
    //RB6 - Program Clock (PGC) (Output)
    //RB7 - Pogram Data (PGD) (Output)
    LATB  = 0b00000000;
    TRISB = 0b00111011;
    RBPU = 1; //Enable Pull Ups


    //PORTC
    //RC0 - Top Switch (Input)
    //RC1 - ServoX (Output)
    //RC2 - N/C (Output)
    //RC3 - N/C (Output)
    //RC4 - N/C (Output)
    //RC5 - N/C (Output)
    //RC6 - MCU Serial TX (Output)
    //RC7 - MCU Serial RX (Input)
    LATC  = 0b00000000;
    TRISC = 0b10000001;

    //PORTD
    //RB0-RB7 - LCD Data Bus (Outputs)
    //RD0 - N/C (Output)
    //RD1 - N/C (Output)
    //RD2 - N/C (Output)
    //RD3 - N/C (Output)
    //RD4 - N/C (Output)
    //RD5 - N/C (Output)
    //RD6 - Motor Diag B (Input)
    //RD7 - Motor CCW Direction (Output)
    LATD =  0b00000000;
    TRISD = 0b01000000;

    //PORTE
    //RE0 - N/C (Output)
    //RE1 - N/C (Output)
    //RE2 - N/C (Output)
    //RE3 - N/C (Output)
    //RE4 - Motor CW Direction (Output)
    //RE5 - Motor Diag A (Input)
    //RE6 - N/C (Output)
    //RE7 - Motor PWN (Output)
    LATE =  0b00000000;
    TRISE = 0b00100000;

    //PORTF
    //RF0 - NO PIN (Output)
    //RF1 - Middle Switch (Input)
    //RF2 - N/C (Output)
    //RF3 - N/C (Output)
    //RF4 - Focus (Output)
    //RF5 - Shutter (Output)
    //RF6 - Battery Voltage (Analog Input)
    //RF7 - Bottom Switch (Input)
    LATF = 0b00000000;
    TRISF = 0b11000010;

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
    unsigned char TempID;

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
        MessageStream_WriteByte(Config.MyID);
        ChkSum += Config.MyID;
        MessageStream_WriteByte('O');
        ChkSum += 'O';
        MessageStream_WriteByte('R');
        ChkSum += 'R';
        MessageStream_WriteByte('B');
        ChkSum += 'B';
        MessageStream_WriteByte('I');
        ChkSum += 'I';
        MessageStream_WriteByte('T');
        ChkSum += 'T';
        MessageStream_WriteByte(ChkSum);
        TXBuffer[TX_Idx++] = '#';
        idx = TX_Idx;
        TX_Idx = 0;
        TX_bCount = idx;        
    } else if (MsgAddr == Config.MyID) {
        unsigned char CmdID = MessageStream_ReadByte();
        switch (CmdID) {
            case CMD_SET_ID:
                TempID = MessageStream_ReadByte();
                ChkSum += TempID;
                if (ChkSum == MessageStream_ReadByte()) {
                    Config.MyID = TempID;
                    addr = 0;
                    EEprom_write(&addr, (unsigned char *) &Config, sizeof (Config));
                    AckCmd(CmdID);
                } else {
                    NackCmd(CmdID, ChkSum);
                }
                break;
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

                MessageStream_WriteByte(UI_Location);
                ChkSum += UI_Location;

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
                    addr = 0;
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
    bPowerOff = 0;
    return retCode;
}

