#include <spdlog/spdlog.h>

#include <spdlog.hpp>
#include <latency_measurements.hpp>
#include <timestamp_ns.hpp>

/*----------------------------------------------------------------------------*/
bool spdlog_base::terminate()
{
    m_log->flush();
    return true;
}
/*----------------------------------------------------------------------------*/
void spdlog_base::destroy()
{
    spdlog::drop ("spdlog");
    m_log.reset();
}
/*----------------------------------------------------------------------------*/
int spdlog_base::enqueue_msgs (int count)
{
    for (int i = 0; i < count; ++i) {
        m_log->info (STRING_TO_LOG, i);
    }
    return count;
}
/*----------------------------------------------------------------------------*/
void spdlog_base::fill_latencies(latency_measurements& lm, int count)
{
    for (int i = 0; i < count; ++i) {
        uint64_t start = ns_now();
        m_log->info (STRING_TO_LOG, i);
        lm.add_sample (ns_now() - start, true);
    }
}
/*----------------------------------------------------------------------------*/
bool spdlog_sync::create (int fixed_queues_bytes)
{
    spdlog::set_sync_mode();
    m_log = spdlog::rotating_logger_mt ("spdlog", "./spdl" , 1024 * 1024, 1000);
    return (bool) m_log;
}
/*----------------------------------------------------------------------------*/
char const* spdlog_sync::get_name() const
{
    return "spdlog-sync";
}
/*----------------------------------------------------------------------------*/
char const* spdlog_sync::get_description() const
{
    return "spdlog synchronous mode";
}
/*----------------------------------------------------------------------------*/
bool spdlog_async::create (int fixed_queues_bytes)
{
    m_log = spdlog::rotating_logger_mt ("spdlog", "./spdl" , 1024 * 1024, 1000);
    return (bool) m_log;
}
/*----------------------------------------------------------------------------*/
char const* spdlog_async::get_name() const
{
    return "spdlog-async";
}
/*----------------------------------------------------------------------------*/
char const* spdlog_async::get_description() const
{
    return "spdlog asynchronous mode";
}
/*----------------------------------------------------------------------------*/
