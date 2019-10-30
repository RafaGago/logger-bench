#ifndef __TEST_SUITE__
#define __TEST_SUITE__

#include <cstdint>
#include <iostream>
#include <cassert>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <array>
#include <thread>
#include <atomic>
#include <chrono>

#include <logger.hpp>
#include <test_results.hpp>
#include <latency_measurements.hpp>
#include <throughput_measurements.hpp>
#include <cpuclock.hpp>

static int max_threads = 16;

/*----------------------------------------------------------------------------*/
#if __linux__
#define GNU_SOURCE
#include <pthread.h>
#include <unistd.h>
/*----------------------------------------------------------------------------*/
static void set_thread_cpu (unsigned i)
{
    /*Linux only, will require ifdefs*/
    cpu_set_t cpu;
    CPU_ZERO (&cpu);
    CPU_SET (i % sysconf (_SC_NPROCESSORS_ONLN), &cpu);
    pthread_setaffinity_np (pthread_self(), sizeof cpu, &cpu);
}
#else
/*Note that for an implementation without thread affinity just a stub
"set_thread_cpu" implementation will do.*/
#error "Implement setting a thread to a CPU for this platform"
#endif
/*----------------------------------------------------------------------------*/
static const int thread_count[] = { 1, 2, 4, 8, max_threads };
static const int thread_count_idxs =
    sizeof thread_count / sizeof thread_count[0];
/*----------------------------------------------------------------------------*/
typedef std::vector<std::unique_ptr<logger>> logvector;
/*----------------------------------------------------------------------------*/
static bool run_throughput(
    test_result&          tr,
    logger&               l,
    std::size_t           msgs,
    int                   thread_count,
    std::size_t           mem_bytes
    )
{
    using namespace std;
    using namespace std::chrono;
    tr.thread_count = thread_count;

    unique_ptr<thread[]> threads (new thread[thread_count]);
    unique_ptr<throughput_measurements[]> results(
        new throughput_measurements[thread_count]
        );
    atomic<int> initialized (0);
    cpuclock_t total_end;

    auto msgs_thr = msgs / thread_count;

    if (!l.create (mem_bytes)) {
        cerr << "logger creation failed: " << l.get_name() << "\n";
        return false;
    }
    if (thread_count > 1) {
        /*set this thread to a free CPU when the number of CPUs is greater than
        the thread count */
        set_thread_cpu (thread_count);
        for (unsigned i = 0; i < thread_count; ++i) {
            throughput_measurements& tm = results[i];
            tm.prepare (msgs_thr);
            threads[i] = thread ([=, &l, &initialized, &tm]()
            {
                set_thread_cpu (i);
                l.prepare_thread (mem_bytes / thread_count);
                initialized.fetch_add (1, memory_order_relaxed);
                while (initialized.load (memory_order_relaxed) >= 0) {
                    this_thread::yield();
                }
                l.run_logging (tm);
                std::atomic_thread_fence (std::memory_order_relaxed);
            });
        }
        while (initialized.load (memory_order_relaxed) != thread_count) {
            this_thread::yield();
        }
        initialized.store (-1, memory_order_relaxed);
        for (unsigned i = 0; i < thread_count; ++i) {
            threads[i].join();
        }
    }
    else {
        set_thread_cpu (0);
        results[0].prepare (msgs_thr);
        l.prepare_thread (mem_bytes);
        l.run_logging (results[0]);
    }
    l.terminate();
    total_end = cpuclock_get();
    l.destroy();

    auto min_start = results[0].get_start();
    auto max_end   = results[0].get_end();
    tr.messages = msgs;
    tr.throughput_faults = 0;
    for (unsigned i = 0; i < thread_count; ++i) {
        tr.throughput_faults += results[i].get_faults();
        min_start = std::min (min_start, results[i].get_start());
        max_end   = std::max (max_end, results[i].get_end());
    }
    tr.producer_ns = cpuclock_to_ns (max_end - min_start);
    tr.total_ns    = cpuclock_to_ns (total_end - min_start);
    return true;
}
/*----------------------------------------------------------------------------*/
static bool run_latency(
    test_result&          tr,
    logger&               l,
    std::size_t           msgs,
    int                   thread_count,
    std::size_t           mem_bytes
    )
{
    using namespace std;
    tr.thread_count = thread_count;

    unique_ptr<thread[]> threads (new thread[thread_count]);
    unique_ptr<latency_measurements[]> results(
        new latency_measurements[thread_count]
        );
    atomic<int> initialized (0);

    auto msgs_thr = msgs / thread_count;

    if (!l.create (mem_bytes)) {
        cerr << "logger creation failed: " << l.get_name() << "\n";
        return false;
    }
    if (thread_count > 1) {
        /*set this thread to a free CPU when the number of CPUs is greater than
        the thread count */
        set_thread_cpu (thread_count);
        for (unsigned i = 0; i < thread_count; ++i) {
            latency_measurements& lm = results[i];
            lm.prepare (msgs_thr);
            threads[i] = thread ([=, &l, &initialized, &lm]()
            {
                set_thread_cpu (i);
                l.prepare_thread (mem_bytes / thread_count);
                initialized.fetch_add (1, memory_order_relaxed);
                while (initialized.load (memory_order_relaxed) >= 0) {
                    this_thread::yield();
                }
                l.run_logging (lm);
            });
        }
        while (initialized.load (memory_order_relaxed) != thread_count) {
            this_thread::yield();
        }
        initialized.store (-1, memory_order_relaxed);
        for (unsigned i = 0; i < thread_count; ++i) {
            threads[i].join();
        }
    }
    else {
        set_thread_cpu (0);
        results[0].prepare (msgs_thr);
        l.prepare_thread (mem_bytes);
        l.run_logging (results[0]);
    }
    l.terminate();
    l.destroy();
    latency_measurements &lm = results[0];
    for (int i = 1; i < thread_count; ++i) {
        lm.join (results[i]);
    }
    lm.finish();
    tr.latency_faults = lm.get_faults();
    tr.latency_ns_50  = cpuclock_to_ns (lm.get_percentile_ns (50.));
    tr.latency_ns_75  = cpuclock_to_ns (lm.get_percentile_ns (75.));
    tr.latency_ns_85  = cpuclock_to_ns (lm.get_percentile_ns (85.));
    tr.latency_ns_90  = cpuclock_to_ns (lm.get_percentile_ns (90.));
    tr.latency_ns_95  = cpuclock_to_ns (lm.get_percentile_ns (95.));
    tr.latency_ns_97  = cpuclock_to_ns (lm.get_percentile_ns (97.));
    tr.latency_ns_99  = cpuclock_to_ns (lm.get_percentile_ns (99.));
    tr.latency_ns_999 = cpuclock_to_ns (lm.get_percentile_ns (99.9));
    tr.latency_ns_min = cpuclock_to_ns (lm.get_percentile_ns (0.));
    tr.latency_ns_max = cpuclock_to_ns (lm.get_percentile_ns (100.));
    return true;
}
/*----------------------------------------------------------------------------*/
static int run_tests(
  test_results& results,
  std::size_t   msgs,
  int           iterations,
  std::size_t   mem_bytes,
  logvector&    loggers
  )
{
    using namespace std;
    test_results res;
    res.init (loggers.size(), thread_count_idxs, iterations);

    for (int it = 0; it < iterations; ++it) {
      cout << "iteration: " << it << "\n";
      for (int l = 0; l < loggers.size(); ++l) {
          cout << "--" << loggers[l]->get_name() << "--\n";
          for (int t = 0; t < thread_count_idxs; ++t) {
              int threads = thread_count[t];
              test_result* tr = res.at (l, t, it);
              assert (tr);
              memset (tr, 0, sizeof *tr);
              bool success = run_throughput(
                  *tr, *loggers[l], msgs, threads, mem_bytes
                  );
              if (!success) {
                return 1;
              }
              success = run_latency(
                  *tr, *loggers[l], msgs, threads, mem_bytes
                  );
              if (!success) {
                return 1;
              }
              tr->apply_clock_correction (cpuclock_get_call_overhead_ns());
              tr->to_stream (cout);
              cout << "\n";
          }
      }
    }
    results = std::move (res);
    return 0;
}
/*----------------------------------------------------------------------------*/
#endif /* __TEST_SUITE__ */
