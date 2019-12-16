#include <stdio.h>

#include <bl/base/default_allocator.h>
#include <bl/base/thread.h>

#include <malc.hpp>
#include <benchmark_iterables.hpp>

/*----------------------------------------------------------------------------*/
bool malc_base::create (std::size_t fixed_queues_bytes)
{
    try {
        m_log.construct();

        m_log.add_destination<malcpp::file_dst>();
        auto cfg = m_log.get_cfg();

        cfg.consumer.start_own_thread = true;

        cfg.alloc.fixed_allocator_bytes     = 0;
        cfg.alloc.fixed_allocator_max_slots = 0;
        cfg.alloc.fixed_allocator_per_cpu   = 0;

        cfg.producer.timestamp = true;

        set_cfg (cfg, fixed_queues_bytes);

        m_log.init (cfg);
        return true;
    }
    catch (...) {
        return false;
    }
}
/*----------------------------------------------------------------------------*/
void malc_base::destroy()
{
    m_log.destroy();
}
/*----------------------------------------------------------------------------*/
bool malc_base::terminate()
{
    m_log.terminate (false);
}
/*----------------------------------------------------------------------------*/
template <class T>
std::size_t malc_base::run_logging (T& iterable)
{
    bl_err err;
    std::size_t success = 0;
    int i = 0;
    for (auto _ : iterable) {
        log_error_i (m_log, STRING_TO_LOG " {}", ++i);
        ++success;
    }
    return success;
}
INSTANTIATE_RUN_LOGGING_TEMPLATES (malc_base)
/*----------------------------------------------------------------------------*/
char const* malc_tls::get_name() const
{
    return "malc-tls";
}
/*----------------------------------------------------------------------------*/
char const* malc_tls::get_description() const
{
    return "mal C using thread local storage only";
}
/*----------------------------------------------------------------------------*/
bool malc_tls::prepare_thread (std::size_t fixed_queues_bytes)
{
    try {
        m_log.producer_thread_local_init (fixed_queues_bytes);
        return true;
    }
    catch (...) {
        return false;
    }
}
/*----------------------------------------------------------------------------*/
void malc_tls::set_cfg (malcpp::cfg& cfg, std::size_t fixed_queues_bytes)
{
    cfg.alloc.msg_allocator = nullptr;
}
/*----------------------------------------------------------------------------*/
char const* malc_heap::get_name() const
{
    return "malc-heap";
}
/*----------------------------------------------------------------------------*/
char const* malc_heap::get_description() const
{
    return "mal C using the default heap only";
}
/*----------------------------------------------------------------------------*/
void malc_heap::set_cfg (malcpp::cfg& cfg, std::size_t fixed_queues_bytes)
{}
/*----------------------------------------------------------------------------*/
char const* malc_fixed::get_name() const
{
    return "malc-fixed";
}
/*----------------------------------------------------------------------------*/
char const* malc_fixed::get_description() const
{
    return "mal C using the fixed size memory pool";
}
/*----------------------------------------------------------------------------*/
void malc_fixed::set_cfg (malcpp::cfg& cfg, std::size_t fixed_queues_bytes)
{
    cfg.alloc.msg_allocator             = nullptr;
    cfg.alloc.fixed_allocator_bytes     = fixed_queues_bytes;
    cfg.alloc.fixed_allocator_max_slots = 2;
    cfg.alloc.fixed_allocator_per_cpu   = 0;
}
/*----------------------------------------------------------------------------*/
char const* malc_fixed_cpu::get_name() const
{
    return "malc-fixed-cpu";
}
/*----------------------------------------------------------------------------*/
char const* malc_fixed_cpu::get_description() const
{
    return "mal C using one fixed size memory pool for each CPU";
}
/*----------------------------------------------------------------------------*/
void malc_fixed_cpu::set_cfg (malcpp::cfg& cfg, std::size_t fixed_queues_bytes)
{
    cfg.alloc.msg_allocator         = nullptr;
    cfg.alloc.fixed_allocator_bytes = fixed_queues_bytes / bl_get_cpu_count();
    cfg.alloc.fixed_allocator_max_slots = 2;
    cfg.alloc.fixed_allocator_per_cpu   = 1;
}
/*----------------------------------------------------------------------------*/
void malc_tls_heap::set_cfg (malcpp::cfg& cfg, std::size_t fixed_queues_bytes)
{}
/*----------------------------------------------------------------------------*/
char const* malc_tls_heap::get_name() const
{
    return "malc-tls-heap";
}
/*----------------------------------------------------------------------------*/
char const* malc_tls_heap::get_description() const
{
    return "as malc-tls but using the as heap fallback";
}
/*----------------------------------------------------------------------------*/
void malc_fixed_heap::set_cfg (malcpp::cfg& cfg, std::size_t fixed_queues_bytes)
{
    auto alloc = cfg.alloc.msg_allocator;
    malc_fixed::set_cfg (cfg, fixed_queues_bytes);
    cfg.alloc.msg_allocator = alloc;
}
/*----------------------------------------------------------------------------*/
char const* malc_fixed_heap::get_name() const
{
    return "malc-fixed-heap";
}
/*----------------------------------------------------------------------------*/
char const* malc_fixed_heap::get_description() const
{
    return "as malc-fixed but using the as heap fallback";
}
/*----------------------------------------------------------------------------*/
void malc_fixed_cpu_heap::set_cfg(
    malcpp::cfg& cfg, std::size_t fixed_queues_bytes
    )
{
    auto alloc = cfg.alloc.msg_allocator;
    malc_fixed_cpu::set_cfg (cfg, fixed_queues_bytes);
    cfg.alloc.msg_allocator = alloc;
}
/*----------------------------------------------------------------------------*/
char const* malc_fixed_cpu_heap::get_name() const
{
    return "malc-fixed-cpu-heap";
}
/*----------------------------------------------------------------------------*/
char const* malc_fixed_cpu_heap::get_description() const
{
    return "as malc-fixed-cpu but using the as heap fallback";
}
/*----------------------------------------------------------------------------*/
