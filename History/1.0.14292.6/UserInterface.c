#include <htc.h>
#include "main.h"
#include "UserInterface.h"
#include "LCD.h"
#include "Settings.h"

signed char ShowVoltage() {    
    double Batt;
    LCD_ClearDisplay();
    bLock_BatteryVoltage = 1;
    Batt = BatteryVoltage;
    bLock_BatteryVoltage = 0;
    Batt *= Config.Volts_per_Count;
    LCD_PrintString("BATTERY VOLTAGE:\0");
    LCD_SetPosition(1, 0);
    LCD_PrintFloat(Batt, 2, 2, 0);
    LCD_PrintString(" VOLTS\0");
    return GetClick();
}

signed char GetClick(void) {
    signed char ret = 0;
    while (1) {
        ret = GetInput();
        if (ret == USER_INPUT_CLICK) return 0;
        if (ret == USER_INPUT_BACK) return -1;
        if (ret == USER_INPUT_CANCEL) return -2;
        if (ret < 0) return ret;
    }
    return 0;
}

signed char GetInput(void) {
    signed char ret = 0;
    ClearUI_Event = 1;
    while (ClearUI_Event) Idle();

    while (ret == 0) {
        bLock_UI_Event = 1;
        ret = UI_Event;
        bLock_UI_Event = 0;
        Idle();
    }
    return ret;
}

signed char GetInput_nonblocking(void) {
    signed char ret = 0;
    while (ClearUI_Event) NOP();
    bLock_UI_Event = 1;
    ret = UI_Event;
    bLock_UI_Event = 0;
    if (ret) {
        ClearUI_Event = 1;
    }
    return ret;
}

signed char GetYesNo(const char *prompt, signed char Initial) {
    signed char answer;
    signed char inp;
    LCD_ClearDisplay();
    LCD_PrintString(prompt);

    answer = Initial;
    while (1) {
        LCD_SetPosition(1, 0);
        if (answer) LCD_PrintString("<YES>\0");
        else LCD_PrintString("<NO> \0");
        inp = GetInput();
        if (inp < 0) return inp;

        switch (inp) {
            case USER_INPUT_CLICK: //The User Pressed the button to select something...
                return (answer);
                
            case USER_INPUT_CANCEL:
                return -2;
                
            case USER_INPUT_BACK:
                return -1;
                
            case USER_INPUT_INC: //Rot +
            case USER_INPUT_DEC: //Rot -
                answer = !answer;
                break;
            default:
                break;
        }
    }
    return (0);
}

signed char GetFloat(const char *prompt, const char *unit, double* Value, double Min, double Max, double Precision) {
    double Output;
    char WholePlaces;
    char FractionalPlaces;
    signed char ret = 0;


    if ((Max >= 10000) || (Min <= -10000)) WholePlaces = 5;
    else if (Max >= 1000 | Min <= -1000) WholePlaces = 4;
    else if (Max >= 100 | Min <= -100) WholePlaces = 3;
    else if (Max >= 10 | Min <= -10) WholePlaces = 2;
    else WholePlaces = 2;

    if (Precision >= 1.0) FractionalPlaces = 0;
    else if (Precision >= 0.1) FractionalPlaces = 1;
    else if (Precision >= 0.01) FractionalPlaces = 2;
    else if (Precision >= 0.001) FractionalPlaces = 3;
    else if (Precision >= 0.0001) FractionalPlaces = 4;


    Output = *Value;
    LCD_ClearDisplay();
    LCD_PrintString(prompt);

    LCD_SetPosition(1, 0);
    LCD_PrintFloat(Output, WholePlaces, FractionalPlaces, 1);
    LCD_PrintString(unit);

    while (1) {
        LCD_SetPosition(1, 0);
        LCD_PrintFloat(Output, WholePlaces, FractionalPlaces, 1);
        ret = GetInput();
        if (ret < 0) return ret;
        switch (ret) {
            case USER_INPUT_CLICK: //user has pressed the switch to accept.                     
                *Value = Output;
                return (0);
            case USER_INPUT_CANCEL:
                return -2;
                
            case USER_INPUT_BACK:
                return -1;
                
            case USER_INPUT_INC: //Rot+
                Output += GetRotaryMultiplier() * Precision;
                if (Output > Max) Output = Max;
                break;
            case USER_INPUT_DEC:
                Output -= GetRotaryMultiplier() * Precision;
                if (Output < Min) Output = Min;
                break;
        }
    }
    return (0);
}

unsigned char GetRotaryMultiplier() {
    char idx = 0;
    unsigned int uiAccum = 0;

    if (ZeroRotaryDetentIntervals) {
        ZeroRotaryDetentIntervals = 0;
        bLock_RotaryDetentIntervals = 1;
        {
            for (idx = 0; idx < 16; idx++) RotaryDetentIntervals[idx] = 0xFF;
        }
        bLock_RotaryDetentIntervals = 0;
        return 1;
    }
    uiAccum = 0;
    bLock_RotaryDetentIntervals = 1;
    {
        for (idx = 0; idx < 16; idx++) uiAccum += RotaryDetentIntervals[idx];
    }
    bLock_RotaryDetentIntervals = 0;

    uiAccum >>= 4;

    if (uiAccum < 12) return 50;
    if (uiAccum < 30) return 10;
    if (uiAccum < 50) return 5;
    return 1;
}

signed char DisplayMenu(const char *Menu[], char ItemCount, char Initial) {
    char SecondLine = 0;
    signed char SelectedItem = 0;
    signed char ret = 0;

    SelectedItem = Initial;

    while (1) {
        LCD_ClearDisplay();
        if (SecondLine) {
            LCD_SetPosition(0, 1);
            if (SelectedItem == 0) LCD_PrintString(Menu[ItemCount]);
            else LCD_PrintString(Menu[SelectedItem - 1]);
            LCD_SetPosition(1, 0);
            LCD_PrintChar('*');
            LCD_PrintString(Menu[SelectedItem]);
            LCD_SetPosition(1, 0);
        } else {
            LCD_PrintChar('*');
            LCD_PrintString(Menu[SelectedItem]);
            LCD_SetPosition(1, 1);
            if (SelectedItem == ItemCount) LCD_PrintString(Menu[0]);
            else LCD_PrintString(Menu[SelectedItem + 1]);
            LCD_SetPosition(0, 0);
        }

        ret = GetInput();
        if (ret < 0) {
            SelectedItem = ret;
            goto exit;
        }

        switch (ret) {
            case USER_INPUT_CLICK:
                goto exit;
            case USER_INPUT_BACK:
                SelectedItem = -1;
                goto exit;
            case USER_INPUT_CANCEL:
                SelectedItem = -2;
                goto exit;
            case USER_INPUT_INC:
                SecondLine = 1;
                if (SelectedItem == ItemCount) SelectedItem = ItemCount;
                else SelectedItem++;
                break;
            case USER_INPUT_DEC:
                SecondLine = 0;
                if (SelectedItem == 0) SelectedItem = 0;
                else SelectedItem--;
                break;
        }
    }
exit:
    return (SelectedItem);
}

signed char DisplayChoices(const char *Menu[], char ItemCount, const char *Prompt, char Initial) {
    char SelectedItem = 0;
    signed char ret = 0;
    SelectedItem = Initial;
    while (1) {
        LCD_ClearDisplay();
        LCD_PrintString(Prompt);
        LCD_SetPosition(1, 0);
        LCD_PrintChar('*');
        LCD_PrintString(Menu[SelectedItem]);

        ret = GetInput();
        if (ret < 0) return ret;

        switch (ret) {
            case USER_INPUT_CLICK:
                return (SelectedItem);
            case USER_INPUT_BACK:
                return (-1);
            case USER_INPUT_CANCEL:
                return (-2);
            case USER_INPUT_INC:
                if (SelectedItem == ItemCount) SelectedItem = ItemCount;
                else SelectedItem++;
                break;
            case USER_INPUT_DEC:
                if (SelectedItem == 0) SelectedItem = 0;
                else SelectedItem--;
                break;
        }
    }
    return (0);
}

void ConvertTimeToDiscrete(double Time, char *Hours, char *Minutes, char *Seconds, int *Milliseconds) {
    double dat = Time;
    double temp;
    double holder;

    //Determine the whole hours...
    temp = dat * 0.000277778;
    *Hours = (char) temp;

    //Deduct the hours from the time
    holder = *Hours;
    holder *= 3600;
    dat -= holder;

    //Determine the Whole Minutes
    temp = dat * 0.016666667;
    *Minutes = (char) temp;
    holder = *Minutes;
    holder *= 60;

    //Deduct the Minutes...
    dat -= holder;

    //Determine the Whole Seconds
    *Seconds = (char) dat;

    //Deduct the Seconds...
    dat -= *Seconds;

    //The remaining time is in milliseconds...
    dat *= 1000;
    *Milliseconds = (int) dat;
}

signed char GetTime(const char *prompt, double *Time, double Min, double Max, char Precision) {
    char Seconds, Minutes, Hours;
    int Milliseconds;
    double Tm = *Time;
    signed char ret = 0;

    ConvertTimeToDiscrete(Tm, &Hours, &Minutes, &Seconds, &Milliseconds);
    if (!(Precision & 0b1000)) Hours = 0;
    if (!(Precision & 0b0100)) Minutes = 0;
    if (!(Precision & 0b0010)) Seconds = 0;
    if (!(Precision & 0b0001)) Milliseconds = 0;
    Tm = ConvertTimeFromDiscrete(Hours, Minutes, Seconds, Milliseconds);
    if (Tm < Min) Tm = Min;
    if (Tm > Max) Tm = Max;

labelHours:
    if (!(Precision & 0b1000)) goto labelMinutes;
    LCD_ClearDisplay();
    LCD_PrintString(prompt);
    LCD_PrintString("<H>");
    LCD_SetPosition(1, 0);
    LCD_PrintString("                   \0");
    while (1) {
        LCD_SetPosition(1, 0);
        PrintTime(Tm, Precision, 0b1000);
        ret = GetInput();
        if (ret < 0) return ret;
        switch (ret) {
            case USER_INPUT_CLICK: //user has pressed the switch to accept.
                goto labelMinutes;
            case USER_INPUT_BACK: //user has pressed back
                return (-1);
            case USER_INPUT_CANCEL: //user has pressed Cancel
                return (-2);
            case USER_INPUT_INC: //Rot+
                Tm += 3600;
                if (Tm > Max) Tm = Max;
                break;
            case USER_INPUT_DEC:
                Tm -= 3600;
                if (Tm < Min) Tm = Min;
                break;
        }
    }
labelMinutes:
    if (!(Precision & 0b0100)) goto labelSeconds;
    LCD_ClearDisplay();
    LCD_PrintString(prompt);
    LCD_PrintString("<M>");
    LCD_SetPosition(1, 0);
    LCD_PrintString("                   \0");
    while (1) {
        LCD_SetPosition(1, 0);
        PrintTime(Tm, Precision, 0b0100);
        ret = GetInput();
        if (ret < 0) return ret;
        switch (ret) {
            case USER_INPUT_CLICK: //user has pressed the switch to accept.
                goto labelSeconds;
            case USER_INPUT_BACK: //user has pressed back
                goto labelHours;
            case USER_INPUT_CANCEL: //user has pressed Cancel
                return (-2);
            case USER_INPUT_INC: //Rot+
                Tm += 60;
                if (Tm > Max) Tm = Max;
                break;
            case USER_INPUT_DEC:
                Tm -= 60;
                if (Tm < Min) Tm = Min;
                break;
        }
    }
labelSeconds:
    if (!(Precision & 0b0010)) goto labelMilliseconds;
    LCD_ClearDisplay();
    LCD_PrintString(prompt);
    LCD_PrintString("<S>");
    LCD_SetPosition(1, 0);
    LCD_PrintString("                   \0");
    while (1) {
        LCD_SetPosition(1, 0);
        PrintTime(Tm, Precision, 0b0010);
        ret = GetInput();
        if (ret < 0) return ret;
        switch (ret) {
            case USER_INPUT_CLICK: //user has pressed the switch to accept.
                goto labelMilliseconds;
            case USER_INPUT_BACK: //user has pressed back
                goto labelMinutes;
            case USER_INPUT_CANCEL: //user has pressed Cancel
                return (-2);
            case USER_INPUT_INC: //Rot+
                Tm += 1;
                if (Tm > Max) Tm = Max;
                break;
            case USER_INPUT_DEC:
                Tm -= 1;
                if (Tm < Min) Tm = Min;
                break;
        }
    }
labelMilliseconds:
    if (!(Precision & 0b0001)) goto labelFinished;
    LCD_ClearDisplay();
    LCD_PrintString(prompt);
    LCD_PrintString("<ms>");
    LCD_SetPosition(1, 0);
    LCD_PrintString("                   \0");
    while (1) {
        LCD_SetPosition(1, 0);
        PrintTime(Tm, Precision, 0b0001);
        ret = GetInput();
        if (ret < 0) return ret;
        switch (ret) {
            case USER_INPUT_CLICK: //user has pressed the switch to accept.
                goto labelFinished;
            case USER_INPUT_BACK: //user has pressed back
                goto labelSeconds;
            case USER_INPUT_CANCEL: //user has pressed Cancel
                return (-2);
            case USER_INPUT_INC: //Rot+
                Tm += 0.1;
                if (Tm > Max) Tm = Max;
                break;
            case USER_INPUT_DEC:
                Tm -= 0.1;
                if (Tm < Min) Tm = Min;
                break;
        }
    }
labelFinished:
    *Time = Tm;
    return 0;
}

double ConvertTimeFromDiscrete(char Hours, char Minutes, char Seconds, int Milliseconds) {
    double ret;
    double dTemp;
    long temp;

    temp = Hours;
    temp *= 3600;
    ret += temp;

    temp = Minutes;
    temp *= 60;
    ret += temp;

    ret += Seconds;

    dTemp = Milliseconds;
    dTemp *= 0.001;
    ret += dTemp;

    return (ret);
}

void PrintTime(double dat, unsigned char precision, unsigned char EditChevron) {
    char Hours, Minutes, Seconds;
    int Milliseconds;

    ConvertTimeToDiscrete(dat, &Hours, &Minutes, &Seconds, &Milliseconds);

    //Change Milliseconds to 10ths of a second.
    Milliseconds /= 100;

    //Time provided in the dat variable which is in milliseconds...
    if (precision & 0b1000) {
        if (EditChevron & 0b1000) LCD_PrintChar('<');
        LCD_PrintLong((int) Hours, 2, 0);
        if (EditChevron & 0b1000) LCD_PrintChar('>');
    }

    if (precision & 0b0100) {
        if (precision & 0b00001000) LCD_PrintChar(':');
        if (EditChevron & 0b0100) LCD_PrintChar('<');
        LCD_PrintLong((int) Minutes, 2, 0);
        if (EditChevron & 0b0100) LCD_PrintChar('>');
    }

    if (precision & 0b0010) {
        if (precision & 0b00000100) LCD_PrintChar(':');
        if (EditChevron & 0b0010) LCD_PrintChar('<');
        LCD_PrintLong((int) Seconds, 2, 0);
        if (EditChevron & 0b0010) LCD_PrintChar('>');
    }

    if (precision & 0b0001) {
        if (precision & 0b00000010) LCD_PrintChar('.');
        if (EditChevron & 0b0001) LCD_PrintChar('<');
        LCD_PrintLong((int) Milliseconds, 1, 0);
        if (EditChevron & 0b0001) LCD_PrintChar('>');
    }
}

double RoundTimeUp(double Time, unsigned char precision) {
    char Hours, Minutes, Seconds;
    int Milliseconds;

    ConvertTimeToDiscrete(Time, &Hours, &Minutes, &Seconds, &Milliseconds);
    if (!(precision & 0b0001)) {
        if (Milliseconds) Seconds++;
        Milliseconds = 0;
    }
    if (!(precision & 0b0010)) {
        if (Seconds) Minutes++;
        Seconds = 0;
    }
    if (!(precision & 0b0100)) {
        if (Minutes) Hours++;
        Minutes = 0;
    }
    return ConvertTimeFromDiscrete(Hours, Minutes, Seconds, Milliseconds);
}

double RoundTimeDown(double Time, unsigned char precision) {
    char Hours, Minutes, Seconds;
    int Milliseconds;

    ConvertTimeToDiscrete(Time, &Hours, &Minutes, &Seconds, &Milliseconds);
    if (!(precision & 0b0001)) {
        Milliseconds = 0;
    }
    if (!(precision & 0b0010)) {
        Seconds = 0;
    }
    if (!(precision & 0b0100)) {
        Minutes = 0;
    }
    return ConvertTimeFromDiscrete(Hours, Minutes, Seconds, Milliseconds);
}

signed char GetPresetNumber() {
    signed char inp;
    char pnum;
    signed char ptype;
    LCD_ClearDisplay();
    LCD_PrintString("PRESET SLOT:\0");
    LCD_SetPosition(1, 0);
    LCD_PrintString("PRESET 01\0");
    pnum = 1;

    while (1) {
        LCD_SetPosition(1, 7);
        LCD_PrintLong((int) pnum, 2, 0);

        ptype = GetPresetType(pnum);

        switch (ptype) {
            case 0:
                LCD_PrintString("-EMPTY    \0");
                break;
            case 1:
                LCD_PrintString("-WAYPOINT \0");
                break;
            case 2:
                LCD_PrintString("-ORBIT    \0");
                break;
        }

        inp = GetInput();
        if (inp<0) return inp;
        
        switch (inp) {
            case USER_INPUT_DEC:
                if (pnum > 1) pnum--;
                break;
            case USER_INPUT_INC:
                if (pnum < 5) pnum++;
                break;
            case USER_INPUT_CLICK:
                return (pnum);
            case USER_INPUT_BACK:
                return (-1);
            case USER_INPUT_CANCEL:
                return (-2);
        }
    }
    return (-2);
}

