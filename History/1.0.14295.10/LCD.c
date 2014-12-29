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
    //Clear the Display
    LCD_ClearDisplay();
}

void LCD_ClearDisplay(void) {
    char idx;
    for (idx = 0; idx < 20; idx++) {
        LCD_Line1[idx] = 32;
        LCD_Line2[idx] = 32;
    }
    LCD_Row = 0;
    LCD_Col = 0;
}

void LCD_ReturnHome(void) {
    LCD_Row = 0;
    LCD_Col = 0;
}


void LCD_SetPosition(char Row, char Col) {
    LCD_Row = Row;
    LCD_Col = Col;
}

void LCD_PrintChar(char dat) {
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
