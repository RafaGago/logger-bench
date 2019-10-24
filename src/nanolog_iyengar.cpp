#include <stdio.h>

#include <latency_measurements.hpp>
#include <timestamp_ns.hpp>

#include <NanoLog.hpp>
#include <NanoLog.cpp>
#include <nanolog_iyengar.hpp>
/*----------------------------------------------------------------------------*/
bool nanolog_iyengar::create (int fixed_queues_bytes)
{
    nanolog::initialize (nanolog::GuaranteedLogger(), "./", "nanolog", 1);
    return true;
}
/*----------------------------------------------------------------------------*/
bool nanolog_iyengar::terminate()
{
    /* workaround: this is relying on internals to shut it down, not on the
    public API */
    ::nanolog::nanologger.reset();
    return true;
}
/*----------------------------------------------------------------------------*/
void nanolog_iyengar::destroy() {}
/*----------------------------------------------------------------------------*/
int nanolog_iyengar::enqueue_msgs (int count)
{
    for (int i = 0; i < count; ++i) {
        LOG_INFO << STRING_TO_LOG << i;
    }
    return count;
}
/*----------------------------------------------------------------------------*/
void nanolog_iyengar::fill_latencies(latency_measurements& lm, int count)
{
    for (int i = 0; i < count; ++i) {
        uint64_t start = ns_now();
        LOG_INFO << STRING_TO_LOG << i;
        lm.add_sample (ns_now() - start, true);
    }
}
/*----------------------------------------------------------------------------*/
char const* nanolog_iyengar::get_name() const
{
    return "nanolog-iyengar";
}
/*----------------------------------------------------------------------------*/
char const* nanolog_iyengar::get_description() const
{
    return "Iyengar111's NanoLog GuaranteedLogger";
}
/*----------------------------------------------------------------------------*/
