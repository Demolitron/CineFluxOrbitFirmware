#define YES 1
#define NO 0

signed char ShowVoltage();
signed char GetClick(void);
signed char GetYesNo(const char *prompt, signed char Initial);
signed char GetFloat(const char *prompt, const char *unit, double* Value, double Min, double Max, double Precision);
signed char GetInput(void);
signed char GetInput_nonblocking(void);
signed char DisplayMenu(const char *Menu[], char ItemCount, char Initial);
signed char DisplayChoices(const char *Menu[], char ItemCount, const char *Prompt, char Initial);
signed char GetPresetNumber();

signed char GetTime(const char *prompt, double *Time, double Min, double Max, char Precision);
void PrintTime(double dat, unsigned char precision, unsigned char EditChevron);
void ConvertTimeToDiscrete(double Time, char *Hours, char *Minutes, char *Seconds, int *Milliseconds);
double ConvertTimeFromDiscrete(char Hours, char Minutes, char Seconds, int Milliseconds);
double RoundTimeUp(double Time,unsigned char precision);
double RoundTimeDown(double Time,unsigned char precision);

unsigned char GetRotaryMultiplier();
