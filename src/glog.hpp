#ifndef __BENCHMARK_GLOG__
#define __BENCHMARK_GLOG__

#include <logger_impl_helpers.hpp>

/*----------------------------------------------------------------------------*/
class glog : public logger_adaptor<glog> {
public:
    glog();
    virtual ~glog() {};
    virtual char const* get_name() const;
    virtual char const* get_description() const;
    virtual bool create (std::size_t fixed_queues_bytes);
    virtual void destroy();
    virtual bool terminate();
    template <class T>
    std::size_t run_logging (T& iterable);
private:
    bool m_init;
};
/*----------------------------------------------------------------------------*/
#endif
