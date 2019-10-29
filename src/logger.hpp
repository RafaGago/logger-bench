#ifndef __TEST_HPP__
#define __TEST_HPP__

#include <chrono>
#include <cstdint>

class latency_measurements;
class throughput_measurements;
namespace benchmark { class State; }

#define STRING_TO_LOG \
    "123567890 123567890 123567890 123567890 123567890 123567890 123567890" \
    "123567890 123567890 123567890 123567890 123567890 123567890 123567890: "

/*----------------------------------------------------------------------------*/
class logger {
public:
    virtual ~logger() {};

    /*get the logger name on the command line*/
    virtual char const* get_name() const = 0;
    /*get the logger description on the help of the command line*/
    virtual char const* get_description() const = 0;
    /*create a logger instance (if not a singleton)*/
    virtual bool create(int fixed_queues_bytes) = 0;
    /*destroy a logger instance (if not a singleton). See terminate.*/
    virtual void destroy() = 0;
    /* runs the function that makes an async logger to empty its queue. It is
    used to measure time to disk. If the function that destroys the logger is
    the one that empties the queue it has to be run here and the destroy call
    be implemented as a no-op  */
    virtual bool terminate() = 0;
    /* does thread initialization, mainly to be used by loggers that use TLS.
    Time measurements won't start until all threads return from this function */
    virtual bool prepare_thread(int fixed_queues_bytes) { return true; }
    /* Write (Enqueue) the messages by iterating the iterable objects in a
    loop. */
    virtual void run_logging (benchmark::State& bs) = 0;
    virtual void run_logging (latency_measurements& lm) = 0;
    virtual void run_logging (throughput_measurements& lm) = 0;
};

#endif /* __TEST_HPP__ */
