#ifndef __BENCHMARK_MALC__
#define __BENCHMARK_MALC__

struct malc;
struct malc_cfg;

#include <logger.hpp>
#include <bl/base/allocator.h>

/*----------------------------------------------------------------------------*/
class malc_base : public logger {
public:
    malc_base();
    virtual ~malc_base();
    virtual bool create (int fixed_queues_bytes);
    virtual void destroy();
    virtual int enqueue_msgs (int count);
    virtual bool terminate();
    virtual void fill_latencies (latency_measurements& meas, int count);
    virtual bool prepare_thread(int fixed_queues_bytes) { return true; }

protected:
    inline struct malc* get_malc_logger_instance()
    {
        return m_log;
    }

    struct malc* m_log;
    alloc_tbl    m_alloc;
    bool         m_created;

    virtual void set_cfg (struct malc_cfg& cfg, int fixed_queues_bytes) = 0;
};
/*----------------------------------------------------------------------------*/
class malc_tls : public malc_base {
public:
    virtual bool prepare_thread(int fixed_queues_bytes);
protected:
    void set_cfg (struct malc_cfg& cfg, int fixed_queues_bytes);
};
/*----------------------------------------------------------------------------*/
class malc_heap : public malc_base {
protected:
    void set_cfg (struct malc_cfg& cfg, int fixed_queues_bytes);
};
/*----------------------------------------------------------------------------*/
class malc_fixed : public malc_base {
protected:
    void set_cfg (struct malc_cfg& cfg, int fixed_queues_bytes);
};
/*----------------------------------------------------------------------------*/
class malc_fixed_cpu : public malc_base {
protected:
    void set_cfg (struct malc_cfg& cfg, int fixed_queues_bytes);
};
/*----------------------------------------------------------------------------*/
#endif /*__BENCHMARK_MALC__*/