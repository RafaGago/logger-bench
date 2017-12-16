#ifndef __BENCHMARK_GLOG__
#define __BENCHMARK_GLOG__

#include <logger.hpp>

/*----------------------------------------------------------------------------*/
class glog : public logger {
public:
    glog();
    virtual ~glog() {};
    virtual char const* get_name() const;
    virtual char const* get_description() const;
    virtual bool create (int fixed_queues_bytes);
    virtual void destroy();
    virtual bool terminate();
    virtual int enqueue_msgs (int count);
    virtual void fill_latencies (latency_measurements& meas, int count);
private:
    bool m_init;
};
/*----------------------------------------------------------------------------*/
#endif
