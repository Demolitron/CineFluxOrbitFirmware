#include "main.h"
#include "timings.h"

void Wait_seconds_nonblocking(unsigned int time) {
    unsigned int msWait = time * 1000;
    Wait_ms_nonblocking(msWait);
}

void Wait_seconds(unsigned int time) {
    unsigned int msWait = time * 1000;
    Wait_ms(msWait);
}

void Wait_ms_nonblocking(unsigned int time) {
    time >>= 1;
    if (time == 0) time = 1;

    time++;

    bLock_Tick500Hz = 1;
    WaitUntil_Tick500Hz = Tick500Hz;
    bLock_Tick500Hz = 0;

    bWaiting = 0;
    WaitUntil_Tick500Hz += time;
    bWaiting = 1;
}

void Wait_ms(unsigned int time) {
    Wait_ms_nonblocking(time);
    while (bWaiting) Idle();
}

void Wait_us(unsigned int time) {
    unsigned char tickcount;
    if (time > 3180) time = 3180;
    time /= 25;
    if (time == 0) time = 1;
    tickcount = (unsigned char) time;
    Wait_ticks(tickcount);
}

//Waits for the specified number of 40kHz ticks (1 tick = 0.025ms or 25us)

void Wait_ticks(unsigned char Ticks) {
    static bit LastFlop = 0;
    unsigned char remaining = Ticks + 1;
    while (remaining) {
        while (LastFlop == bTick40Khz);
        remaining--;
        LastFlop = !LastFlop;
    }
}

//Returns the current tme in Milliseconds since startup.

double Now() {
    double ret;
    bLock_Tick500Hz = 1;
    ret = Tick500Hz;
    bLock_Tick500Hz = 0;
    return (ret * 2);
}
