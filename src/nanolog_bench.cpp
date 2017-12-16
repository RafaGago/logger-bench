#include <stdio.h>

#include <latency_measurements.hpp>
#include <timestamp_ns.hpp>

#include <NanoLog.hpp>
#include <NanoLog.cpp>
#include <nanolog_bench.hpp>
/*----------------------------------------------------------------------------*/
bool nanolog_bench::create (int fixed_queues_bytes)
{
    nanolog::initialize (nanolog::GuaranteedLogger(), "./", "nanolog", 1);
    return true;
}
/*----------------------------------------------------------------------------*/
bool nanolog_bench::terminate()
{
    /* workaround: this is relying on internals to shut it down, not on the
    public API */
    ::nanolog::nanologger.reset();
    return true;
}
/*----------------------------------------------------------------------------*/
void nanolog_bench::destroy() {}
/*----------------------------------------------------------------------------*/
int nanolog_bench::enqueue_msgs (int count)
{
    for (int i = 0; i < count; ++i) {
        LOG_INFO << LOG_STRING << i;
    }
    return count;
}
/*----------------------------------------------------------------------------*/
void nanolog_bench::fill_latencies(latency_measurements& lm, int count)
{
    for (int i = 0; i < count; ++i) {
        uint64_t start = ns_now();
        LOG_INFO << LOG_STRING << i;
        lm.add_sample (ns_now() - start, true);
    }
}
/*----------------------------------------------------------------------------*/
char const* nanolog_bench::get_name() const
{
    return "nanolog";
}
/*----------------------------------------------------------------------------*/
char const* nanolog_bench::get_description() const
{
    return "NanoLog GuaranteedLogger";
}
/*----------------------------------------------------------------------------*/
