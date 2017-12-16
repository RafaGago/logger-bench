#ifndef __BENCHMARK_G3LOG__
#define __BENCHMARK_G3LOG__

#include <memory>
#include <logger.hpp>

/*----------------------------------------------------------------------------*/
namespace g3 {
    class LogWorker;
    class FileSink;
    template <class T> class SinkHandle;
}
/*----------------------------------------------------------------------------*/
class g3log : public logger {
public:
    virtual ~g3log();
    virtual char const* get_name() const;
    virtual char const* get_description() const;
    virtual bool create (int fixed_queues_bytes);
    virtual void destroy();
    virtual bool terminate();
    virtual int enqueue_msgs (int count);
    virtual void fill_latencies (latency_measurements& meas, int count);
private:
    std::shared_ptr<g3::LogWorker>                m_worker;
    std::shared_ptr<g3::SinkHandle<g3::FileSink>> m_sink;
};
/*----------------------------------------------------------------------------*/
#endif
