// timer.c — fixed-timestep timer (ticks & partialTicks)

#define _POSIX_C_SOURCE 200809L

#include "timer.h"

#if defined(_WIN32)
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#elif defined(__APPLE__)
  #include <mach/mach_time.h>
  #include <time.h>
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

long long getCurrentTimeInNanoseconds(void) {
#if defined(_WIN32)
    static LARGE_INTEGER freq = {0};
    LARGE_INTEGER now;
    if (freq.QuadPart == 0) QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&now);
    return (long long)((now.QuadPart * 1000000000LL) / freq.QuadPart);

#elif defined(__APPLE__)
    static mach_timebase_info_data_t tb = {0};
    if (tb.denom == 0) mach_timebase_info(&tb);
    uint64_t t = mach_absolute_time();
    return (long long)(t * (uint64_t)tb.numer / (uint64_t)tb.denom);

#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000000LL + (long long)ts.tv_nsec;
#endif
}

long long currentTimeMillis(void) {
#if defined(_WIN32)
    static LARGE_INTEGER freq = {0};
    LARGE_INTEGER now;
    if (freq.QuadPart == 0) QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&now);
    return (long long)((now.QuadPart * 1000LL) / freq.QuadPart);

#elif defined(__APPLE__)
    static mach_timebase_info_data_t tb = {0};
    if (tb.denom == 0) mach_timebase_info(&tb);
    uint64_t t = mach_absolute_time();
    uint64_t ns = t * (uint64_t)tb.numer / (uint64_t)tb.denom;
    return (long long)(ns / 1000000ULL);

#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000LL + (long long)(ts.tv_nsec / 1000000L);
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
