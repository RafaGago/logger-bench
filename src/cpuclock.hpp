#ifndef __PERFCOUNTER_HPP__
#define __PERFCOUNTER_HPP__

/* using the implementation on google benchmark's src files */
#include <cycleclock.h>

typedef uint64_t cpuclock_t;
#include <chrono>
/*----------------------------------------------------------------------------*/
inline BENCHMARK_ALWAYS_INLINE cpuclock_t cpuclock_get()
{
    return (cpuclock_t) benchmark::cycleclock::Now();
}

/*----------------------------------------------------------------------------*/
extern void cpuclock_init (double freq = 0.);
/*----------------------------------------------------------------------------*/
extern double cpuclock_get_freq();
/*----------------------------------------------------------------------------*/
extern cpuclock_t cpuclock_get_call_overhead_ns();
/*----------------------------------------------------------------------------*/
extern cpuclock_t cpuclock_to_ns (cpuclock_t v);
/*----------------------------------------------------------------------------*/
#endif
