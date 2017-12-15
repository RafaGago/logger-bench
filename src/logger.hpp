#ifndef __TEST_HPP__
#define __TEST_HPP__

#include <chrono>
#include <cstdint>

class latency_measurements;

#define LOG_STRING \
    "123567890 123567890 123567890 123567890 123567890 123567890 123567890: "

/*----------------------------------------------------------------------------*/
class logger {
public:
    virtual ~logger() {};

    virtual bool create(int fixed_queues_bytes) {}
    virtual void destroy() {}
    /* does thread initialization, mainly to be used by loggers that use TLS */
    virtual bool prepare_thread(int fixed_queues_bytes) { return true; }

    /* runs the function that makes an async logger to empty its queue, if there
     is not such function available destroy the logger here and ignore the
     destroy call */
    virtual bool terminate() {};

    /* returns number of successes */
    virtual int enqueue_msgs (int count) = 0;
    virtual void fill_latencies (latency_measurements& meas, int count) = 0;

    inline uint64_t ns_now()
    {
        using namespace std::chrono;
        return duration_cast<nanoseconds>(
            system_clock::now().time_since_epoch()
            ).count();
    }
};
/*----------------------------------------------------------------------------*/


#endif /* __TEST_HPP__ */