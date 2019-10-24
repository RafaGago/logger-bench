#ifndef __BENCHMARK_NANOLOG__
#define __BENCHMARK_NANOLOG__

#include <logger.hpp>

/*----------------------------------------------------------------------------*/
class nanolog2 : public logger {
public:
    virtual ~nanolog2() {};
    virtual char const* get_name() const;
    virtual char const* get_description() const;
    virtual bool create (int fixed_queues_bytes);
    virtual void destroy();
    virtual bool terminate();
    virtual bool prepare_thread(int fixed_queues_bytes);
    virtual int enqueue_msgs (int count);
    virtual void fill_latencies (latency_measurements& meas, int count);
};
/*----------------------------------------------------------------------------*/
#endif
