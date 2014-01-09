/* 
 * File:   Main.h
 * Author: THORAXIUM
 *
 * Created on January 1, 2014, 12:40 PM
 */
typedef union LONG {
    unsigned char ub[4];
    signed char b[4];
    unsigned int ui[2];
    signed int i[2];
    unsigned long ul;
    signed long l;
};

typedef union {
    unsigned char ub[4];
    signed char b[4];
    unsigned int ui[2];
    signed int i[2];
    unsigned long ul;
    signed long l;
    float fl;
    double dbl;
} MULTI;



void main(void);
void AckCmd(unsigned char CmdID);
void NackCmd(unsigned char CmdID, unsigned char Reason);
void MessageStream_WriteMULTI(MULTI dat, unsigned char CmdID);
void MessageStream_WriteByte(unsigned char dat);
unsigned char MessageStream_ReadByte();
void GoToSleep();
void SetupIO();
void interrupt FastTick(void);
void interrupt low_priority SlowTick();
void SetupHardware();
void Idle(void);
double GetCurrentLinearPosition();
double GetCurrentAngle();

const signed char enc_states[] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};



//Serial Variables
unsigned char ReadIdx = 0;
volatile unsigned char TX_Idx = 0;
volatile unsigned char TX_bCount = 0;
volatile unsigned char TXBuffer[255];
volatile bit RX_MsgComplete;
volatile bit RX_InProgress;
volatile unsigned char RX_Idx = 0;
volatile unsigned char RXBuffer[255];
volatile unsigned char MyID = 0xAA;
volatile bit isPendingExternalUIEvent;
volatile char PendingExternalUIEvent;
volatile bit ExtModeActive;



//UI Variables
volatile bit ClearUI_Event = 0;
volatile bit bLock_RotaryDetentIntervals = 0;
unsigned char RotaryDetentIntervals[16];
volatile bit ZeroRotaryDetentIntervals = 0;
volatile bit bLock_UI_Event = 0;
volatile char UI_Event;
#define USER_INPUT_CLICK 1
#define USER_INPUT_BACK 2
#define USER_INPUT_CANCEL 3
#define USER_INPUT_INC	4
#define USER_INPUT_DEC 5

#define RESET_DURATION 1200
#define CLICK_DURATION 3
#define BACK_DURATION 200
#define CANCEL_DURATION 400

//Battery Varaibles
volatile bit bLock_BatteryVoltage = 0;
unsigned int BatteryVoltage = 0;
volatile bit bLowVoltageTrip = 0;

//LCD Backlight Variables
volatile bit LCD_BacklightEnabled;

//Move Trajectory Variables
volatile bit bFollowMode = 0;
volatile bit bMove_InProgress = 0;
unsigned long Move_speedQ24 = 0;
union LONG Move_shifted_position;
union LONG Move_position[2];
unsigned long Move_AccelValueQ24 = 0;
unsigned long Move_CoastSpeedQ24 = 0;
unsigned long Move_DecelPosition = 0;
signed long Move_FinalPosition = 0;
signed long Move_Origin = 0;
volatile bit bMove_Neg = 0;


//Speed Mode Variables;
volatile bit bSpeedMode = 0;
unsigned long Speed_SetToThis;
volatile bit Speed_SetPending;
volatile bit Speed_IsAccelerating;


//Motor Position Variables
volatile bit bClear_MotorPosition = 0;
signed long Motor_Position = 0;
volatile bit bLock_Motor_Position = 0;

//PID Variables
volatile bit bIncrement_PID_Setpoint = 0;
volatile bit bDecrement_PID_Setpoint = 0;
volatile bit bLock_PID_SetPoint = 0;
//signed long PID_SetPoint=0;
volatile bit bMotionError = 0;
volatile bit bPowerOff = 0;

#define Conversion_Q24_500Hz    33554.4320
#define INV_Conversion_Q24_500Hz  0.0000298023224
#define Conversion_Q24_500Hz_500Hz  67.1088640


#define UI_SWITCH_PULLUP 	RBPU=0
#define UI_SWITCH_TRIS 		TRISBbits.TRISB2
#define UI_SWITCH 		(!PORTBbits.RB2)

#define UI_ENCODER_TRIS_A 	TRISBbits.TRISB3
#define UI_ENCODER_TRIS_B 	TRISBbits.TRISB4
#define UI_ENCODER_PULLUP_A 	RBPU=0
#define UI_ENCODER_PULLUP_B 	RBPU=0
#define UI_ENCODER 				((PORTB & 0b00011000)>>3)

#define MOTOR_ENCODER_TRIS_A 	TRISAbits.TRISA1
#define MOTOR_ENCODER_TRIS_B 	TRISAbits.TRISA0
#define MOTOR_ENCODER 			(PORTA & 0b00000011)

#define MOTOR_FORWARD_TRIS 	TRISCbits.TRISC0
#define MOTOR_FORWARD 		LATCbits.LATC0

#define MOTOR_REVERSE_TRIS 	TRISBbits.TRISB0
#define MOTOR_REVERSE 		LATBbits.LATB0

#define MOTOR_PWM_TRIS 	TRISCbits.TRISC2
#define MOTOR_PWM_H 	CCPR1L
#define MOTOR_PWM_L 	CCP1CON

#define LCD_DB  LATD
#define LCD_BL  LATBbits.LATB1
#define LCD_RS 	LATEbits.LATE5
#define LCD_RW	LATEbits.LATE6
#define LCD_E	LATEbits.LATE7
#define LCD_TOGGLE() LCD_E=1;NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();LCD_E=0;NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();

#define CMD_SET_PRESET  0x01
#define CMD_GET_PRESET  0x02

#define CMD_GETDISPLAY  0x10
#define CMD_UI_CLICK    0x11
#define CMD_UI_BACK     0x12
#define CMD_UI_CANCEL   0x13
#define CMD_UI_INC      0x14
#define CMD_UI_DEC      0x15
#define CMD_GET_POS     0x16
#define CMD_GET_SPEED   0x17
#define CMD_GET_BATTERY 0x18

#define CMD_PREP_MOVE   0x60
#define CMD_EXEC_MOVE   0x61
#define CMD_STOP        0x62
#define CMD_STATUS      0x63
#define CMD_PATH_INIT   0x64
#define CMD_PATH_ADD    0x65
#define CMD_PATH_RUN    0x66

const unsigned char ASCII_HEX_TO_BINARY[128] ={0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
    0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0};

const unsigned char BINARY_TO_ASCII_HEX[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

