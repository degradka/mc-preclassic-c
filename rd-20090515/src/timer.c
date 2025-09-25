// timer.c — fixed-timestep timer (ticks & partialTicks)

#include "timer.h"
#include <stdlib.h>

#ifdef _WIN32
  #include <windows.h>
#else
  #include <time.h>
#endif

#define NS_PER_SECOND        1000000000LL
#define MAX_NS_PER_UPDATE    1000000000LL
#define MAX_TICKS_PER_UPDATE 100

void Timer_init(Timer* timer, float ticksPerSecond) {
    timer->ticksPerSecond = ticksPerSecond;
    timer->timeScale      = 1.0f;
    timer->fps            = 0.0f;
    timer->passedTime     = 0.0f;
    timer->ticks          = 0;
    timer->partialTicks   = 0.0f;
    timer->lastTime       = getCurrentTimeInNanoseconds();
}

long long getCurrentTimeInNanoseconds() {
#ifdef _WIN32
    LARGE_INTEGER freq, now;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&now);
    return now.QuadPart * NS_PER_SECOND / freq.QuadPart;
#else
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec * NS_PER_SECOND + now.tv_nsec;
#endif
}

long long currentTimeMillis() {
#ifdef _WIN32
    LARGE_INTEGER freq, now;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&now);
    return (now.QuadPart * 1000) / freq.QuadPart;
#else
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return tp.tv_sec * 1000 + tp.tv_nsec / 1000000;
#endif
}

void Timer_advanceTime(Timer* timer) {
    long long now      = getCurrentTimeInNanoseconds();
    long long passedNs = now - timer->lastTime;
    timer->lastTime    = now;

    if (passedNs <= 0)                 passedNs = 1;
    if (passedNs > MAX_NS_PER_UPDATE)  passedNs = MAX_NS_PER_UPDATE;

    timer->fps = (float)((double)NS_PER_SECOND / (double)passedNs);

    timer->passedTime += (float)((double)passedNs * timer->timeScale * timer->ticksPerSecond / (double)NS_PER_SECOND);
    timer->ticks = (int)timer->passedTime;
    if (timer->ticks > MAX_TICKS_PER_UPDATE) timer->ticks = MAX_TICKS_PER_UPDATE;

    timer->passedTime -= timer->ticks;
    timer->partialTicks = timer->passedTime;
}
