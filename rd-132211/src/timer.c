#include "timer.h"
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

#define NS_PER_SECOND 1000000000LL
#define MAX_NS_PER_UPDATE 1000000000LL
#define MAX_TICKS_PER_UPDATE 100

void Timer_init(Timer* timer, float ticksPerSecond) {
    timer->ticksPerSecond = ticksPerSecond;
    timer->lastTime = 0;
    timer->timeScale = 1.0F;
    timer->fps = 0.0F;
    timer->passedTime = 0.0F;
    timer->ticks = 0;
    timer->partialTicks = 0.0F;
}

long long getCurrentTimeInNanoseconds() {
#ifdef _WIN32
    LARGE_INTEGER frequency, currentTime;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&currentTime);
    return currentTime.QuadPart * NS_PER_SECOND / frequency.QuadPart;
#else
    struct timespec currentTime;
    clock_gettime(CLOCK_MONOTONIC, &currentTime);
    return currentTime.tv_sec * NS_PER_SECOND + currentTime.tv_nsec;
#endif
}

void Timer_advanceTime(Timer* timer) {
    long long now = getCurrentTimeInNanoseconds();
    long long passedNs = now - timer->lastTime;

    // Store nano time of this update
    timer->lastTime = now;

    // Maximum and minimum
    passedNs = passedNs > 0 ? passedNs : 0;
    passedNs = passedNs < MAX_NS_PER_UPDATE ? passedNs : MAX_NS_PER_UPDATE;

    // Calculate fps
    timer->fps = (float)(NS_PER_SECOND / passedNs);

    // Calculate passed time and ticks
    timer->passedTime += passedNs * timer->timeScale * timer->ticksPerSecond / NS_PER_SECOND;
    timer->ticks = (int)timer->passedTime;

    // Maximum ticks per update
    timer->ticks = timer->ticks < MAX_TICKS_PER_UPDATE ? timer->ticks : MAX_TICKS_PER_UPDATE;

    // Calculate the overflow of the current tick
    timer->passedTime -= timer->ticks;
    timer->partialTicks = timer->passedTime;
}