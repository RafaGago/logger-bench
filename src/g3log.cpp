#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>

#include <latency_measurements.hpp>
#include <timestamp_ns.hpp>

#include <g3log.hpp>

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
int g3log::enqueue_msgs (int count)
{
    for (int i = 0; i < count; ++i) {
        LOG (INFO) << STRING_TO_LOG << i;
    }
    return count;
}
/*----------------------------------------------------------------------------*/
void g3log::fill_latencies(latency_measurements& lm, int count)
{
    for (int i = 0; i < count; ++i) {
        uint64_t start = ns_now();
        LOG (INFO) << STRING_TO_LOG << i;
        lm.add_sample (ns_now() - start, true);
    }
}
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
