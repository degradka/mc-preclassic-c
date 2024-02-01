#ifndef TIMER_H
#define TIMER_H

typedef struct {
    float ticksPerSecond;
    long long lastTime; // Use long long for holding larger time values
    float timeScale;
    float fps;
    float passedTime;
    int ticks;
    float partialTicks;
} Timer;

void Timer_init(Timer* timer, float ticksPerSecond);
void Timer_advanceTime(Timer* timer);

#endif /* TIMER_H */