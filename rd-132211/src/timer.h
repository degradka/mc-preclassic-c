// timer.h — fixed-timestep timer (ticks & partialTicks)

#ifndef TIMER_H
#define TIMER_H

typedef struct {
    float      ticksPerSecond;
    long long  lastTime;
    float      timeScale;
    float      fps;
    float      passedTime;
    int        ticks;
    float      partialTicks;
} Timer;

void       Timer_init(Timer* timer, float ticksPerSecond);
void       Timer_advanceTime(Timer* timer);
long long  currentTimeMillis(void);
long long  getCurrentTimeInNanoseconds(void);

#endif /* TIMER_H */