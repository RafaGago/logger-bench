#ifndef __BENCHMARK_MALC__
#define __BENCHMARK_MALC__

struct malc;
struct malc_cfg;

#include <bl/base/allocator.h>
#include <logger_impl_helpers.hpp>
/*----------------------------------------------------------------------------*/
class malc_base : public logger_adaptor<malc_base> {
public:
    malc_base();
    virtual ~malc_base();
    virtual bool create (std::size_t fixed_queues_bytes);
    virtual void destroy();
    virtual bool terminate();
    virtual bool prepare_thread(std::size_t fixed_queues_bytes) { return true; }
    template <class T>
    std::size_t run_logging (T& iterable);

protected:
    inline struct malc* get_malc_logger_instance()
    {
        return m_log;
    }

    struct malc* m_log;
    alloc_tbl    m_alloc;
    bool         m_created;

    virtual void set_cfg (struct malc_cfg& cfg, std::size_t fixed_queues_bytes) = 0;
};
/*----------------------------------------------------------------------------*/
class malc_tls : public malc_base {
public:
    virtual char const* get_name() const;
    virtual char const* get_description() const;
    virtual bool prepare_thread(std::size_t fixed_queues_bytes);
protected:
    void set_cfg (struct malc_cfg& cfg, std::size_t fixed_queues_bytes);
};
/*----------------------------------------------------------------------------*/
class malc_heap : public malc_base {
public:
    virtual char const* get_name() const;
    virtual char const* get_description() const;
protected:
    void set_cfg (struct malc_cfg& cfg, std::size_t fixed_queues_bytes);
};
/*----------------------------------------------------------------------------*/
class malc_fixed : public malc_base {
public:
    virtual char const* get_name() const;
    virtual char const* get_description() const;
protected:
    void set_cfg (struct malc_cfg& cfg, std::size_t fixed_queues_bytes);
};
/*----------------------------------------------------------------------------*/
class malc_fixed_cpu : public malc_base {
public:
    virtual char const* get_name() const;
    virtual char const* get_description() const;
protected:
    void set_cfg (struct malc_cfg& cfg, std::size_t fixed_queues_bytes);
};
/*----------------------------------------------------------------------------*/
class malc_tls_heap : public malc_tls {
public:
    virtual char const* get_name() const;
    virtual char const* get_description() const;
protected:
    void set_cfg (struct malc_cfg& cfg, std::size_t fixed_queues_bytes);
};
/*----------------------------------------------------------------------------*/
class malc_fixed_heap : public malc_fixed {
public:
    virtual char const* get_name() const;
    virtual char const* get_description() const;
protected:
    void set_cfg (struct malc_cfg& cfg, std::size_t fixed_queues_bytes);
};
/*----------------------------------------------------------------------------*/
class malc_fixed_cpu_heap : public malc_fixed_cpu {
public:
    virtual char const* get_name() const;
    virtual char const* get_description() const;
protected:
    void set_cfg (struct malc_cfg& cfg, std::size_t fixed_queues_bytes);
};
/*----------------------------------------------------------------------------*/
#endif /*__BENCHMARK_MALC__*/