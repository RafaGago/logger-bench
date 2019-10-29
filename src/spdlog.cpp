#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include <spdlog.hpp>
#include <benchmark_iterables.hpp>

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
template <class T>
std::size_t spdlog_base::run_logging (T& iterable)
{
    std::size_t success = 0;
    int i = 0;
    for (auto _ : iterable) {
        m_log->info (STRING_TO_LOG, i);
        ++success;
    }
    return success;
}
INSTANTIATE_RUN_LOGGING_TEMPLATES (spdlog_base)
/*----------------------------------------------------------------------------*/
bool spdlog_sync::create (std::size_t fixed_queues_bytes)
{
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
bool spdlog_async::create (std::size_t fixed_queues_bytes)
{
    m_log = spdlog::rotating_logger_mt<spdlog::async_factory>(
        "spdlog", "./spdl" , 1024 * 1024, 1000
        );
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
