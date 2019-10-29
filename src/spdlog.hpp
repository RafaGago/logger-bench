#ifndef __BENCHMARK_SPDLOG__
#define __BENCHMARK_SPDLOG__

#include <memory>
#include <logger_impl_helpers.hpp>

namespace spdlog {
    class logger;
}
/*----------------------------------------------------------------------------*/
class spdlog_base : public logger_adaptor<spdlog_base> {
public:
    virtual ~spdlog_base() {};
    virtual void destroy();
    virtual bool terminate();
    template <class T>
    std::size_t run_logging (T& iterable);
protected:
    std::shared_ptr<spdlog::logger> m_log;
};
/*----------------------------------------------------------------------------*/
class spdlog_sync : public spdlog_base {
public:
    virtual ~spdlog_sync() {};
    virtual char const* get_name() const;
    virtual char const* get_description() const;
    virtual bool create (std::size_t fixed_queues_bytes);
};
/*----------------------------------------------------------------------------*/
class spdlog_async : public spdlog_base {
public:
    virtual ~spdlog_async() {};
    virtual char const* get_name() const;
    virtual char const* get_description() const;
    virtual bool create (std::size_t fixed_queues_bytes);
};
/*----------------------------------------------------------------------------*/
#endif
