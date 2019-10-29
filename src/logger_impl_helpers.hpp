#ifndef __BENCHMARK_RUN_LOGGING_HELPERS_HPP__
#define __BENCHMARK_RUN_LOGGING_HELPERS_HPP__

#include <logger.hpp>
#include <benchmark_iterables.hpp>

/* A class, meant to be inhertited by logger implementations, that just
implements all the virtual "run_logging" functions on "logger" as a call to a
non-virtual "run_logging" function template.*/
/*----------------------------------------------------------------------------*/
template <class derived>
class logger_adaptor : public logger
{
public:
    virtual void run_logging (benchmark::State& bs)
    {
        auto successes =  child().run_logging (bs);
        bs.counters["faults"] = bs.iterations() - successes;
    }

    virtual void run_logging (latency_measurements& lm)
    {
        lm.add_successes (child().run_logging (lm));
    }

    virtual void run_logging (throughput_measurements& tm)
    {
        tm.add_successes (child().run_logging (tm));
    }
private:
    inline derived& child()
    {
        return static_cast <derived&> (*this);
    }
};
/*----------------------------------------------------------------------------*/
#define INSTANTIATE_RUN_LOGGING_TEMPLATE(classname, type)\
template std::size_t classname::run_logging<type> (type&);\
/*----------------------------------------------------------------------------*/
#define INSTANTIATE_RUN_LOGGING_TEMPLATES(classname) \
INSTANTIATE_RUN_LOGGING_TEMPLATE (classname, benchmark::State) \
INSTANTIATE_RUN_LOGGING_TEMPLATE (classname, latency_measurements) \
INSTANTIATE_RUN_LOGGING_TEMPLATE (classname, throughput_measurements)
/*----------------------------------------------------------------------------*/
#endif
