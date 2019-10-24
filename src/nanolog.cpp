#include <stdio.h>

#include <latency_measurements.hpp>
#include <timestamp_ns.hpp>

#include <nanolog.hpp>
#include <NanoLogCpp17.h>

static const char logfile[] = "./logger-bench-nanolog";

/*----------------------------------------------------------------------------*/
bool nanolog::create (int fixed_queues_bytes)
{
    NanoLog::setLogFile (logfile);
    NanoLog::setLogLevel (NanoLog::DEBUG);
    return true;
}
/*----------------------------------------------------------------------------*/
bool nanolog::terminate()
{
    NanoLog::sync();
    remove (logfile);
    return true;
}
/*----------------------------------------------------------------------------*/
void nanolog::destroy() {}
/*----------------------------------------------------------------------------*/
bool nanolog::prepare_thread(int fixed_queues_bytes)
{
    /*NOTICE, no way to pass memory size restructions*/
    NanoLog::preallocate();
    return true;
}
/*----------------------------------------------------------------------------*/
int nanolog::enqueue_msgs (int count)
{
    for (int i = 0; i < count; ++i) {
        NANO_LOG (NanoLog::NOTICE, "%s %i", STRING_TO_LOG, i);
    }
    return count;
}
/*----------------------------------------------------------------------------*/
void nanolog::fill_latencies(latency_measurements& lm, int count)
{
    for (int i = 0; i < count; ++i) {
        uint64_t start = ns_now();
        NANO_LOG (NanoLog::NOTICE, "%s %i", STRING_TO_LOG, i);
        lm.add_sample (ns_now() - start, true);
    }
}
/*----------------------------------------------------------------------------*/
char const* nanolog::get_name() const
{
    return "nanolog";
}
/*----------------------------------------------------------------------------*/
char const* nanolog::get_description() const
{
    return "NanoLog C++17 ((C) Standford University)";
}
/*----------------------------------------------------------------------------*/
