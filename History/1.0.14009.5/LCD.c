#include <htc.h>
#include "main.h"
#include "timings.h"
#include "lcd.h"

void LCD_PrintFloat(double dat, char whole, char fract, char sign) {
    long wholepart;
    long fraction;

    wholepart = (long) dat;
    if (sign) {
        if (wholepart == 0) {
            if (dat < 0) LCD_PrintChar('-');
            else LCD_PrintChar('+');
			LCD_PrintLong(0,whole,0);
        } else LCD_PrintLong(wholepart, whole, sign);
    } else LCD_PrintLong(wholepart, whole, sign);

    dat -= wholepart;
    if (dat < 0.0) dat = 0.0 - dat;
    dat *= 100000.0;
    fraction = (long) dat;

    if (fract) {
        LCD_PrintChar('.');
        if (fract == 1) {
            fraction /= 10000;
            LCD_PrintLong(fraction, 1, 0);
        } else if (fract == 2) {
            fraction /= 1000;
            LCD_PrintLong(fraction, 2, 0);
        } else if (fract == 3) {
            fraction /= 100;
            LCD_PrintLong(fraction, 3, 0);
        } else if (fract == 4) {
            fraction /= 10;
            LCD_PrintLong(fraction, 4, 0);
        } else if (fract == 5) {
            LCD_PrintLong(fraction, 5, 0);
        }
    }
}

void LCD_PrintLong(long dat, char places, char sign) {
    long temp;

    if (dat < 0) {
        LCD_PrintChar('-');
        dat = 0 - dat;
    } else {
        if (sign) LCD_PrintChar('+');
    }

    temp = dat / 1000000000;
    dat -= temp * 1000000000;
    temp += 48;
    if (places > 9) LCD_PrintChar(temp);

    temp = dat / 100000000;
    dat -= temp * 100000000;
    temp += 48;
    if (places > 8) LCD_PrintChar(temp);

    temp = dat / 10000000;
    dat -= temp * 10000000;
    temp += 48;
    if (places > 7) LCD_PrintChar(temp);

    temp = dat / 1000000;
    dat -= temp * 1000000;
    temp += 48;
    if (places > 6) LCD_PrintChar(temp);

    temp = dat / 100000;
    dat -= temp * 100000;
    temp += 48;
    if (places > 5) LCD_PrintChar(temp);

    temp = dat / 10000;
    dat -= temp * 10000;
    temp += 48;
    if (places > 4) LCD_PrintChar(temp);

    temp = dat / 1000;
    dat -= temp * 1000;
    temp += 48;
    if (places > 3) LCD_PrintChar(temp);

    temp = dat / 100;
    dat -= temp * 100;
    temp += 48;
    if (places > 2) LCD_PrintChar(temp);

    temp = dat / 10;
    dat -= temp * 10;
    temp += 48;
    if (places > 1) LCD_PrintChar(temp);

    temp = dat;
    temp += 48;
    LCD_PrintChar(temp);
}

void LCD_PowerUp() {

    //Turn on the back light
    LCD_BL = 1;

    //Pause 60ms
    Wait_ms(60);

    //8-Bit, 2 Line, 5x7 Dots
    LCD_FunctionSet(1, 1, 1);

    //Display ON, Cursor OFF, Blink OFF
    LCD_DisplayOnOff(1, 0, 0);

    //Clear the Display
    LCD_ClearDisplay();

    //Pause 60ms
    Wait_ms(60);

    //8-Bit, 2 Line, 5x7 Dots
    LCD_FunctionSet(1, 1, 1);

    //Display ON, Cursor OFF, Blink OFF
    LCD_DisplayOnOff(1, 0, 0);

    //Clear the Display
    LCD_ClearDisplay();

}

void LCD_ClearDisplay(void) {
    char idx;

    LCD_RS = 0;
    LCD_RW = 0;
    LCD_DB = 0b00000001;
    LCD_TOGGLE()
    //Wait 1.64ms for the clear to complete.
    Wait_us(1640);

    for (idx = 0; idx < 20; idx++) {
        LCD_Line1[idx] = 32;
        LCD_Line2[idx] = 32;
    }
    LCD_Row = 0;
    LCD_Col = 0;
}

void LCD_ReturnHome(void) {

    LCD_RS = 0;
    LCD_RW = 0;
    LCD_DB = 0b00000010;
    LCD_TOGGLE()
    //Wait 1.64ms for the return to complete.
    Wait_us(1640);
    LCD_Row = 0;
    LCD_Col = 0;
}

void LCD_EntryModeSet(char Increment, char ShiftOn) {

    unsigned char dat = 0b00000100;
    if (Increment) dat |= 0b00000010;
    if (ShiftOn) dat |= 0b00000001;

    LCD_E = 0;
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_DB = dat;
    LCD_TOGGLE()

    //Wait 75us
    Wait_us(75);
}

void LCD_DisplayOnOff(char DisplayOn, char CursorOn, char BlinkOn) {

    unsigned char dat = 0b00001000;
    if (DisplayOn) dat |= 0b00000100;
    if (CursorOn) dat |= 0b00000010;
    if (BlinkOn) dat |= 0b00000001;

    LCD_E = 0;
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_DB = dat;
    LCD_TOGGLE()
    //Wait 75us
    Wait_us(75);
}

void LCD_CursorShift(char ShiftDisplay, char ShiftRight) {

    unsigned char dat = 0b00010000;
    if (ShiftDisplay) dat |= 0b00001000;
    if (ShiftRight) dat |= 0b00000100;

    LCD_E = 0;
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_DB = dat;
    LCD_TOGGLE()
    //Wait 75us
    Wait_us(75);
}

void LCD_FunctionSet(char EightBit, char DualLine, char FiveByTenDots) {

    unsigned char dat = 0b00100000;
    if (EightBit) dat |= 0b00010000;
    if (DualLine) dat |= 0b00001000;
    if (FiveByTenDots) dat |= 0b00000100;

    LCD_E = 0;
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_DB = dat;
    LCD_TOGGLE()
    //Wait 75us
    Wait_us(75);
}

void LCD_SetPosition(char Row, char Col) {

    unsigned char dat = Col;
    if (Row == 1) dat += 0x40;
    dat |= 0b10000000;

    LCD_E = 0;
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_DB = dat;
    LCD_TOGGLE()
    //Wait 75us
    Wait_us(75);
    LCD_Row = Row;
    LCD_Col = Col;
}

void LCD_PrintChar(char dat) {

    LCD_E = 0;
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_DB = dat;
    LCD_TOGGLE()
    //Wait 75us
    Wait_us(75);
    if (LCD_Row) LCD_Line1[LCD_Col] = dat;
    else LCD_Line2[LCD_Col] = dat;
    LCD_Col++;
    if (LCD_Col > 19) LCD_Col = 19;
}

void LCD_PrintString(const unsigned char *dat) {
    unsigned char a;
    while (*dat) {
        a = *dat;
        LCD_PrintChar(a);
        dat++;
    }
}

void putc(unsigned char dat) {
    while (TXSTA1bits.TRMT == 0) NOP();
    TXREG = dat;
}
