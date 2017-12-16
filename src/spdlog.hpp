#ifndef __BENCHMARK_SPDLOG__
#define __BENCHMARK_SPDLOG__

#include <memory>
#include <logger.hpp>

namespace spdlog {
    class logger;
}

/*----------------------------------------------------------------------------*/
class spdlog_base : public logger {
public:
    virtual ~spdlog_base() {};
    virtual void destroy();
    virtual bool terminate();
    virtual int enqueue_msgs (int count);
    virtual void fill_latencies (latency_measurements& meas, int count);
protected:
    std::shared_ptr<spdlog::logger> m_log;
};
/*----------------------------------------------------------------------------*/
class spdlog_sync : public spdlog_base {
public:
    virtual ~spdlog_sync() {};
    virtual char const* get_name() const;
    virtual char const* get_description() const;
    virtual bool create (int fixed_queues_bytes);
};
/*----------------------------------------------------------------------------*/
class spdlog_async : public spdlog_base {
public:
    virtual ~spdlog_async() {};
    virtual char const* get_name() const;
    virtual char const* get_description() const;
    virtual bool create (int fixed_queues_bytes);
};
/*----------------------------------------------------------------------------*/
#endif
