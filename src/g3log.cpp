#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>

#include <g3log.hpp>
#include <benchmark_iterables.hpp>

/*----------------------------------------------------------------------------*/
g3log::~g3log() {}
/*----------------------------------------------------------------------------*/
bool g3log::create (int fixed_queues_bytes)
{
    m_worker = g3::LogWorker::createLogWorker();
    m_sink   = m_worker->addDefaultLogger ("g3", "./");
    g3::initializeLogging (m_worker.get());
    return true;
}
/*----------------------------------------------------------------------------*/
bool g3log::terminate()
{
    m_worker.reset();
    m_sink.reset();
    return true;
}
/*----------------------------------------------------------------------------*/
void g3log::destroy() {}
/*----------------------------------------------------------------------------*/
template <class T>
int g3log::run_logging (T& iterable)
{
    int success = 0;
    int i = 0;
    for (auto _ : iterable) {
        LOG (INFO) << STRING_TO_LOG << ++i;
        ++success;
    }
    return success;
}
INSTANTIATE_RUN_LOGGING_TEMPLATES (g3log)
/*----------------------------------------------------------------------------*/
char const* g3log::get_name() const
{
    return "g3log";
}
/*----------------------------------------------------------------------------*/
char const* g3log::get_description() const
{
    return "g3log async";
}
/*----------------------------------------------------------------------------*/
