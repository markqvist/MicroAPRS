#ifndef UTIL_TIME_H
#define UTIL_TIME_H

#include <util/atomic.h>
#include "device.h"

#define DIV_ROUND(dividend, divisor)  (((dividend) + (divisor) / 2) / (divisor))
#define CLOCK_TICKS_PER_SEC CONFIG_AFSK_DAC_SAMPLERATE

typedef int32_t ticks_t;
typedef int32_t mtime_t;

volatile ticks_t _clock;

static inline ticks_t timer_clock(void) {
    ticks_t result;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        result = _clock;
    }

    return result;
}


inline ticks_t ms_to_ticks(mtime_t ms) {
    return ms * DIV_ROUND(CLOCK_TICKS_PER_SEC, 1000);
}

inline void cpu_relax(void) {
    // Do nothing!
}

static inline void delay_ms(unsigned long ms) {
    ticks_t start = timer_clock();
    unsigned long n_ticks = ms_to_ticks(ms);
    while (timer_clock() - start < n_ticks) {
        cpu_relax();
    }
}


#endif
