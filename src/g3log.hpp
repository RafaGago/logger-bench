#ifndef __BENCHMARK_G3LOG__
#define __BENCHMARK_G3LOG__

#include <memory>
#include <logger_impl_helpers.hpp>

/*----------------------------------------------------------------------------*/
namespace g3 {
    class LogWorker;
    class FileSink;
    template <class T> class SinkHandle;
}
/*----------------------------------------------------------------------------*/
class g3log : public logger_adaptor<g3log> {
public:
    virtual ~g3log();
    virtual char const* get_name() const;
    virtual char const* get_description() const;
    virtual bool create (int fixed_queues_bytes);
    virtual void destroy();
    virtual bool terminate();
    template <class T>
    int run_logging (T& iterable);
private:
    std::shared_ptr<g3::LogWorker>                m_worker;
    std::shared_ptr<g3::SinkHandle<g3::FileSink>> m_sink;
};
/*----------------------------------------------------------------------------*/
#endif
