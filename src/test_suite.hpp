#ifndef __TEST_SUITE__
#define __TEST_SUITE__

#include <cstdint>
#include <iostream>
#include <cassert>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>

#include <test_results.hpp>
#include <latency_measurements.hpp>
#include <timestamp_ns.hpp>

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
#error "Unsupported platform"
#endif
/*----------------------------------------------------------------------------*/
static const int thread_count[] = { 1, 2, 4, 8, 16 };
static const int thread_count_idxs =
    sizeof thread_count / sizeof thread_count[0];
/*----------------------------------------------------------------------------*/
typedef std::vector<std::unique_ptr<logger>> logvector;
/*----------------------------------------------------------------------------*/
struct throughput_result {
    int                                   faults;
    std::chrono::system_clock::time_point start; /*what a beautiful name*/
    std::chrono::system_clock::time_point end;
};
/*----------------------------------------------------------------------------*/
static bool run_throughput(
    test_result&          tr,
    logger&               l,
    int                   msgs,
    int                   thread_count,
    std::size_t           mem_bytes
    )
{
    using namespace std;
    using namespace std::chrono;
    tr.thread_count = thread_count;

    unique_ptr<thread[]> threads (new thread[thread_count]);
    unique_ptr<throughput_result[]> results(
        new throughput_result[thread_count]
        );
    atomic<int> initialized (0);
    system_clock::time_point total_end;

    auto msgs_thr = msgs / thread_count;

    if (!l.create (mem_bytes)) {
        cerr << "logger creation failed: " << l.get_name() << "\n";
        return false;
    }

    if (thread_count > 1) {
      for (unsigned i = 0; i < thread_count; ++i) {
          throughput_result& thread_results = results[i];
          threads[i] = thread ([=, &l, &initialized, &thread_results]()
          {
              set_thread_cpu (i);
              l.prepare_thread (mem_bytes / thread_count);
              initialized.fetch_add (1, memory_order_relaxed);
              while (initialized.load (memory_order_relaxed) >= 0);
              auto start  = std::chrono::system_clock::now();
              int successes = l.enqueue_msgs (msgs_thr);
              auto end = std::chrono::system_clock::now();
              std::atomic_thread_fence (std::memory_order_relaxed);
              thread_results.faults = msgs_thr - successes;
              thread_results.start  = start;
              thread_results.end    = end;
          });
      }
      while (initialized.load (memory_order_relaxed) != thread_count); /* spin */
      initialized.store (-1, memory_order_relaxed);
      for (unsigned i = 0; i < thread_count; ++i) {
          threads[i].join();
      }
      l.terminate();
      total_end = std::chrono::system_clock::now();
    }
    else {
        set_thread_cpu (0);
        l.prepare_thread (mem_bytes);
        results[0].start  = std::chrono::system_clock::now();
        results[0].faults = msgs_thr - l.enqueue_msgs (msgs_thr);
        results[0].end    = std::chrono::system_clock::now();
        l.terminate();
        total_end = std::chrono::system_clock::now();
    }
    l.destroy();

    std::chrono::system_clock::time_point min_start = results[0].start;
    std::chrono::system_clock::time_point max_end   = results[0].end;
    tr.throughput_faults = 0;
    for (unsigned i = 0; i < thread_count; ++i) {
        tr.throughput_faults += results[i].faults;
        min_start = std::min (min_start, results[i].start);
        max_end   = std::max (max_end, results[i].end);
    }
    uint64_t start_ns = duration_cast<nanoseconds>(
        min_start.time_since_epoch()
        ).count();
    uint64_t prod_end_ns = duration_cast<nanoseconds>(
        max_end.time_since_epoch()
        ).count();
    uint64_t total_end_ns = duration_cast<nanoseconds>(
        total_end.time_since_epoch()
        ).count();
    tr.producer_ns = prod_end_ns - start_ns;
    tr.total_ns    = total_end_ns - start_ns;
    return true;
}
/*----------------------------------------------------------------------------*/
static bool run_latency(
    test_result&          tr,
    logger&               l,
    int                   msgs,
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
      for (unsigned i = 0; i < thread_count; ++i) {
          latency_measurements& lm = results[i];
          lm.prepare (msgs_thr);
          threads[i] = thread ([=, &l, &initialized, &lm]()
          {
              set_thread_cpu (i);
              l.prepare_thread (mem_bytes / thread_count);
              initialized.fetch_add (1, memory_order_relaxed);
              while (initialized.load (memory_order_relaxed) >= 0);
              l.fill_latencies (lm, msgs_thr);
          });
      }
      while (initialized.load (memory_order_relaxed) != thread_count); /* spin */
      initialized.store (-1, memory_order_relaxed);
      for (unsigned i = 0; i < thread_count; ++i) {
          threads[i].join();
      }
      l.terminate();
    }
    else {
        set_thread_cpu (0);
        results[0].prepare (msgs_thr);
        l.prepare_thread (mem_bytes);
        l.fill_latencies (results[0], msgs_thr);
        l.terminate();
    }
    l.destroy();
    for (int i = 1; i < thread_count; ++i) {
        results[0].join (results[i]);
    }
    results[0].finish();
    tr.latency_faults = msgs - results[0].get_successes();
    tr.latency_ns_50  = results[0].get_percentile_ns (50);
    tr.latency_ns_75  = results[0].get_percentile_ns (75);
    tr.latency_ns_85  = results[0].get_percentile_ns (85);
    tr.latency_ns_90  = results[0].get_percentile_ns (90);
    tr.latency_ns_95  = results[0].get_percentile_ns (95);
    tr.latency_ns_97  = results[0].get_percentile_ns (97);
    tr.latency_ns_99  = results[0].get_percentile_ns (99);
    tr.latency_ns_min = results[0].get_min_ns();
    tr.latency_ns_max = results[0].get_max_ns();
    return true;
}
/*----------------------------------------------------------------------------*/
static int run_tests(
  test_results& results,
  int           msgs,
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
          for (int t = 0; t < thread_count_idxs; ++t) {
              int threads = thread_count[t];
              cout << loggers[l]->get_name() << " threads: " << threads;
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
              float rate = (double) msgs / (double) tr->producer_ns;
              rate *= 1000000;
              float trate = (double) msgs / (double) tr->total_ns;
              trate *= 1000000;
              cout << ", Kmsgs/s: " << rate;
              cout << ", faults: "  << tr->throughput_faults;
              cout << ", l97(ns):"  << tr->latency_ns_97;
              cout << ", l99(ns):"  << tr->latency_ns_99;
              cout << ", lmax(ns):" << tr->latency_ns_max;
              cout << "\n";
          }
      }
    }
    results = std::move (res);
    return 0;
}
/*----------------------------------------------------------------------------*/
#endif /* __TEST_SUITE__ */
