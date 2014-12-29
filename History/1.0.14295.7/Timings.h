void Wait_seconds(unsigned int time);
void Wait_ms(unsigned int time);
void Wait_seconds_nonblocking(unsigned int time);
void Wait_ms_nonblocking(unsigned int time);
void Wait_ticks(unsigned char time);
void Wait_us(unsigned int time);
double Now();

//Timing Variables
volatile bit bWaiting=0;
volatile unsigned long WaitUntil_Tick500Hz=0;
volatile bit bTick40Khz=0;
volatile bit bLock_Tick500Hz=0;
volatile unsigned long Tick500Hz=0;
