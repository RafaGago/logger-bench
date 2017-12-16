#ifndef __BENCHMARK_MAL__
#define __BENCHMARK_MAL__

namespace mal {
    class frontend;
    class cfg;
}

#include <logger.hpp>

/*----------------------------------------------------------------------------*/
class mal_base : public logger {
public:
    mal_base();
    virtual ~mal_base();
    virtual bool create (int fixed_queues_bytes);
    virtual void destroy();
    virtual int enqueue_msgs (int count);
    virtual bool terminate();
    virtual void fill_latencies (latency_measurements& meas, int count);
    virtual bool prepare_thread(int fixed_queues_bytes) { return true; }

protected:
    mal::frontend* m_log;
    virtual void set_cfg (mal::cfg& cfg, int fixed_queues_bytes) = 0;
};
/*----------------------------------------------------------------------------*/
class mal_heap : public mal_base {
public:
    virtual char const* get_name() const;
    virtual char const* get_description() const;
protected:
    void set_cfg (mal::cfg& cfg, int fixed_queues_bytes);
};
/*----------------------------------------------------------------------------*/
class mal_fixed : public mal_base {
public:
    virtual char const* get_name() const;
    virtual char const* get_description() const;
protected:
    void set_cfg (mal::cfg& cfg, int fixed_queues_bytes);
};
/*----------------------------------------------------------------------------*/
#endif
