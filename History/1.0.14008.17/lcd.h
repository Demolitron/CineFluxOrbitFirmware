void LCD_PrintString(const unsigned char *dat);
void LCD_PrintChar(char dat);
void LCD_PrintLong(long dat, char places, char sign);
void LCD_PrintFloat(double dat, char whole, char fract, char sign);

void LCD_ClearDisplay(void);
void LCD_PowerUp(void);

void LCD_SetPosition(char Row, char Col);
void LCD_FunctionSet(char EightBit, char DualLine, char FiveByTenDots);
void LCD_CursorShift(char ShiftDisplay, char ShiftRight);
void LCD_DisplayOnOff(char DisplayOn, char CursorOn, char BlinkOn);
void LCD_EntryModeSet(char Increment, char ShiftOn);
void LCD_ReturnHome(void);

void putc(unsigned char dat);

unsigned char LCD_Line1[20];
unsigned char LCD_Line2[20];
unsigned char LCD_Row=0;
unsigned char LCD_Col=0;