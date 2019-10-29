#ifndef __BENCHMARK_NANOLOG_IYENGAR__
#define __BENCHMARK_NANOLOG_IYENGAR__

#include <logger_impl_helpers.hpp>
/*----------------------------------------------------------------------------*/
class nanolog_iyengar : public logger_adaptor<nanolog_iyengar> {
public:
    virtual ~nanolog_iyengar() {};
    virtual char const* get_name() const;
    virtual char const* get_description() const;
    virtual bool create (std::size_t fixed_queues_bytes);
    virtual void destroy();
    virtual bool terminate();
    template <class T>
    std::size_t run_logging (T& iterable);
};
/*----------------------------------------------------------------------------*/
#endif
