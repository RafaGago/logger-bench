#include <stdio.h>

#include <bl/base/default_allocator.h>
#include <malc/malc.h>
#include <malc/destinations/file.h>

#include <malc.hpp>
#include <latency_measurements.hpp>

/*----------------------------------------------------------------------------*/
malc_base::malc_base()
{
    m_log     = nullptr;
    m_alloc   = get_default_alloc();
    m_created = false;
}
/*----------------------------------------------------------------------------*/
malc_base::~malc_base()
{
    destroy();
}
/*----------------------------------------------------------------------------*/
bool malc_base::create (int fixed_queues_bytes)
{
    bl_err err;
    m_log = (malc*) bl_alloc (&m_alloc, malc_get_size());
    if (!m_log) {
      fprintf (stderr, "Unable to allocate memory for the malc instance\n");
      return false;
    }
    err = malc_create (m_log, &m_alloc);
    if (err) {
      fprintf (stderr, "Error creating the malc instance\n");
      destroy();
      return false;
    }
    m_created = true;
    /* destination register */
    u32 file_id;
    err = malc_add_destination (m_log, &file_id, &malc_file_dst_tbl);
    if (err) {
      fprintf (stderr, "Error creating the file destination\n");
      destroy();
      return false;
    }
    /* logger startup */
    malc_cfg cfg;
    err = malc_get_cfg (m_log, &cfg);
    if (err) {
      fprintf (stderr, "bug when retrieving the logger configuration\n");
      destroy();
      return false;
    }
    cfg.consumer.start_own_thread = true;

    cfg.alloc.fixed_allocator_bytes     = 0;
    cfg.alloc.fixed_allocator_max_slots = 0;
    cfg.alloc.fixed_allocator_per_cpu   = 0;

    set_cfg (cfg, fixed_queues_bytes);
    err = malc_init (m_log, &cfg);
    if (err) {
        fprintf (stderr, "unable to start logger\n");
        destroy();
        return false;
    }
    return true;
}
/*----------------------------------------------------------------------------*/
bool malc_base::terminate()
{
    malc_terminate (m_log, false);
}
/*----------------------------------------------------------------------------*/
void malc_base::destroy()
{
    if (!m_log) {
        return;
    }
    if (m_created) {
        malc_destroy (m_log);
    }
    bl_dealloc (&m_alloc, m_log);
    m_log = nullptr;
}
/*----------------------------------------------------------------------------*/
int malc_base::enqueue_msgs (int count)
{
    bl_err err;
    int success = 0;
    for (int i = 0; i < count; ++i) {
        log_error (err, LOG_STRING " {}", i);
        success += (err == bl_ok);
    }
    return success;
}
/*----------------------------------------------------------------------------*/
void malc_base::fill_latencies(latency_measurements& lm, int count)
{
    bl_err err;
    for (int i = 0; i < count; ++i) {
        uint64_t start = ns_now();
        log_error (err, LOG_STRING " {}", i);
        lm.add_sample (ns_now() - start, err == bl_ok);
    }
}
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
bool malc_tls::prepare_thread(int fixed_queues_bytes)
{
    return malc_producer_thread_local_init (m_log, fixed_queues_bytes) == bl_ok;
}
/*----------------------------------------------------------------------------*/
void malc_tls::set_cfg (struct malc_cfg& cfg, int fixed_queues_bytes)
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
void malc_heap::set_cfg (struct malc_cfg& cfg, int fixed_queues_bytes)
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
void malc_fixed::set_cfg (struct malc_cfg& cfg, int fixed_queues_bytes)
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
void malc_fixed_cpu::set_cfg (struct malc_cfg& cfg, int fixed_queues_bytes)
{
    cfg.alloc.msg_allocator         = nullptr;
    cfg.alloc.fixed_allocator_bytes = fixed_queues_bytes / bl_get_cpu_count();
    cfg.alloc.fixed_allocator_max_slots = 2;
    cfg.alloc.fixed_allocator_per_cpu   = 0;
}
/*----------------------------------------------------------------------------*/
