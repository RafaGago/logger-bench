#include <stdio.h>

#include <mal_log/mal_log.hpp>

#include <mal.hpp>
#include <benchmark_iterables.hpp>
/*----------------------------------------------------------------------------*/
mal_base::mal_base()
{
    m_log = nullptr;
}
/*----------------------------------------------------------------------------*/
mal_base::~mal_base()
{
    destroy();
}
/*----------------------------------------------------------------------------*/
bool mal_base::create (std::size_t fixed_queues_bytes)
{
    using namespace mal;
    m_log = new mal::frontend();
    if (!m_log) {
      fprintf (stderr, "Unable to allocate memory for the mal instance\n");
      return false;
    }
    auto mal_cfg = m_log->get_cfg();
    mal_cfg.file.out_folder          = "./";
    mal_cfg.file.aprox_size          = 4 * 1024 * 1024;
    mal_cfg.file.rotation.file_count = 0;
    mal_cfg.file.rotation.delayed_file_count = 0;
    mal_cfg.queue.bounded_q_block_size   = 0;
    mal_cfg.queue.can_use_heap_q         = false;
    mal_cfg.queue.bounded_q_blocking_sev = sev::off;
    set_cfg (mal_cfg, fixed_queues_bytes);
    return m_log->init_backend (mal_cfg) == frontend::init_ok;
}
/*----------------------------------------------------------------------------*/
bool mal_base::terminate()
{
    m_log->on_termination();
    return true;
}
/*----------------------------------------------------------------------------*/
void mal_base::destroy()
{
    if (!m_log) {
        return;
    }
    delete m_log;
    m_log = nullptr;
}
/*----------------------------------------------------------------------------*/
template <class T>
std::size_t mal_base::run_logging (T& iterable)
{
    std::size_t success = 0;
    int i = 0;
    for (auto _ : iterable) {
        bool s = log_error_i (*m_log, STRING_TO_LOG " {}", ++i);
        success += (s == true);
    }
    return success;
}
INSTANTIATE_RUN_LOGGING_TEMPLATES (mal_base)
/*----------------------------------------------------------------------------*/
char const* mal_heap::get_name() const
{
    return "mal-heap";
}
/*----------------------------------------------------------------------------*/
char const* mal_heap::get_description() const
{
    return "mal using the heap only";
}
/*----------------------------------------------------------------------------*/
void mal_heap::set_cfg (mal::cfg& cfg, std::size_t fixed_queues_bytes)
{
    cfg.queue.can_use_heap_q = true;
}
/*----------------------------------------------------------------------------*/
char const* mal_fixed::get_name() const
{
    return "mal-fixed";
}
/*----------------------------------------------------------------------------*/
char const* mal_fixed::get_description() const
{
    return "mal using the fixed size memory pool";
}
/*----------------------------------------------------------------------------*/
void mal_fixed::set_cfg (mal::cfg& cfg, std::size_t fixed_queues_bytes)
{
    cfg.queue.bounded_q_block_size = fixed_queues_bytes;
}
/*----------------------------------------------------------------------------*/
