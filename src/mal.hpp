#ifndef __BENCHMARK_MAL__
#define __BENCHMARK_MAL__

namespace mal {
    class frontend;
    class cfg;
}

#include <logger_impl_helpers.hpp>
/*----------------------------------------------------------------------------*/
class mal_base : public logger_adaptor<mal_base> {
public:
    mal_base();
    virtual ~mal_base();
    virtual bool create (std::size_t fixed_queues_bytes);
    virtual void destroy();
    virtual bool terminate();
    template<class T>
    std::size_t run_logging (T& iterable);
protected:
    mal::frontend* m_log;
    virtual void set_cfg (mal::cfg& cfg, std::size_t fixed_queues_bytes) = 0;
};
/*----------------------------------------------------------------------------*/
class mal_heap : public mal_base {
public:
    virtual char const* get_name() const;
    virtual char const* get_description() const;
protected:
    void set_cfg (mal::cfg& cfg, std::size_t fixed_queues_bytes);
};
/*----------------------------------------------------------------------------*/
class mal_fixed : public mal_base {
public:
    virtual char const* get_name() const;
    virtual char const* get_description() const;
protected:
    void set_cfg (mal::cfg& cfg, std::size_t fixed_queues_bytes);
};
/*----------------------------------------------------------------------------*/
#endif
