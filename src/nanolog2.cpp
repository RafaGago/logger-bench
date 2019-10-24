#include <stdio.h>

#include <latency_measurements.hpp>
#include <timestamp_ns.hpp>

#include <nanolog2.hpp>
#include <NanoLogCpp17.h>

static const char logfile[] = "./logger-bench-nanolog2";

/*----------------------------------------------------------------------------*/
bool nanolog2::create (int fixed_queues_bytes)
{
    NanoLog::setLogFile (logfile);
    NanoLog::setLogLevel (NanoLog::DEBUG);
    return true;
}
/*----------------------------------------------------------------------------*/
bool nanolog2::terminate()
{
    NanoLog::sync();
    remove (logfile);
    return true;
}
/*----------------------------------------------------------------------------*/
void nanolog2::destroy() {}
/*----------------------------------------------------------------------------*/
bool nanolog2::prepare_thread(int fixed_queues_bytes)
{
    /*NOTICE, no way to decide the buffer */
    NanoLog::preallocate();
    return true;
}
/*----------------------------------------------------------------------------*/
int nanolog2::enqueue_msgs (int count)
{
    for (int i = 0; i < count; ++i) {
        NANO_LOG (NanoLog::NOTICE, "%s %i", STRING_TO_LOG, i);
    }
    return count;
}
/*----------------------------------------------------------------------------*/
void nanolog2::fill_latencies(latency_measurements& lm, int count)
{
    for (int i = 0; i < count; ++i) {
        uint64_t start = ns_now();
        NANO_LOG (NanoLog::NOTICE, "%s %i", STRING_TO_LOG, i);
        lm.add_sample (ns_now() - start, true);
    }
}
/*----------------------------------------------------------------------------*/
char const* nanolog2::get_name() const
{
    return "nanolog2";
}
/*----------------------------------------------------------------------------*/
char const* nanolog2::get_description() const
{
    return "NanoLog2 C++17";
}
/*----------------------------------------------------------------------------*/
