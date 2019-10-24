#ifndef __BENCHMARK_NANOLOG_IYENGAR__
#define __BENCHMARK_NANOLOG_IYENGAR__

#include <logger.hpp>

/*----------------------------------------------------------------------------*/
class nanolog_iyengar : public logger {
public:
    virtual ~nanolog_iyengar() {};
    virtual char const* get_name() const;
    virtual char const* get_description() const;
    virtual bool create (int fixed_queues_bytes);
    virtual void destroy();
    virtual bool terminate();
    virtual int enqueue_msgs (int count);
    virtual void fill_latencies (latency_measurements& meas, int count);
};
/*----------------------------------------------------------------------------*/
#endif
