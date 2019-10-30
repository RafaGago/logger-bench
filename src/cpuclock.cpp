#include <thread>
#include <algorithm>

#include <cpuclock.hpp>
#include <chrono>

static double     cpuclockfreq = 0.;
static cpuclock_t overhead_ns = 0;

/*----------------------------------------------------------------------------*/
static cpuclock_t get_overhead()
{
    cpuclock_t min = cpuclock_t (-1ll);
    for (int i = 0; i < 500000; ++i) {
        cpuclock_t prev = cpuclock_get();
        cpuclock_t now  = cpuclock_get();
        min = std::min (now - prev, min);
    }
    return min; /*this value can be substracted from each latency measurement*/
}
/*----------------------------------------------------------------------------*/
void cpuclock_init (double freq)
{
    using namespace std::chrono;
    if (freq == 0.) {
        cpuclock_t overhead;
        auto cstart = steady_clock::now();
        auto start = cpuclock_get();
        std::this_thread::sleep_for (seconds (2));
        overhead = get_overhead();
        auto cend = steady_clock::now();
        auto end = cpuclock_get();
        double sec = duration<double>(cend - cstart).count();
        cpuclockfreq = ((double)(end - start)) / sec;
        overhead_ns = cpuclock_to_ns (overhead);
    }
    else {
        cpuclockfreq = freq;
        overhead_ns = cpuclock_to_ns (get_overhead());
    }
}
/*----------------------------------------------------------------------------*/
double cpuclock_get_freq()
{
    return cpuclockfreq;
}
/*----------------------------------------------------------------------------*/
cpuclock_t cpuclock_get_call_overhead_ns()
{
    return overhead_ns;
}
/*----------------------------------------------------------------------------*/
cpuclock_t cpuclock_to_ns (cpuclock_t v)
{
    return (cpuclock_t) ((((double) v) / cpuclockfreq) * 1000000000.);
}
/*----------------------------------------------------------------------------*/
