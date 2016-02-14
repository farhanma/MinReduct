#ifndef TIMER_H
#define TIMER_H

#include <stdlib.h>
#include <sys/time.h>

struct timeval timerStart;

void StartTimer()
{
    gettimeofday(&timerStart, NULL);
}

// time elapsed in ms
double GetTimer()
{
    struct timeval timerStop, timerElapsed;
    gettimeofday(&timerStop, NULL);
    timersub(&timerStop, &timerStart, &timerElapsed);
    return timerElapsed.tv_sec*1000.0+timerElapsed.tv_usec/1000.0;
}

#endif // TIMER_H
