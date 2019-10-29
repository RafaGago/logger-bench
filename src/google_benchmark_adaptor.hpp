#ifndef __LOGGER_BENCHMARK_ADAPTOR_HPP__
#define __LOGGER_BENCHMARK_ADAPTOR_HPP__

#include <atomic>
#include <thread>

#include <logger.hpp>

#include <benchmark/benchmark.h>
/*----------------------------------------------------------------------------*/
enum {
  unitialized,
  init_failed,
  init_ok,
};
#include <stdio.h>
/*----------------------------------------------------------------------------*/
auto run_google_benchmark = [](
    benchmark::State& state, logger* log, std::size_t qsize
    )
{
    static std::atomic<unsigned> runners (0);
    static std::atomic<unsigned> initstatus (unitialized);

    unsigned thread_count = state.threads;

    if (state.thread_index == 0) {
        /* the first thread initializes the instance */
        if (log->create (qsize)) {
            initstatus.store (init_ok);
        }
        else {
            state.SkipWithError ("unable to initialize");
            initstatus.store (init_failed);
        }
    }
    runners.fetch_add (1);
    while (runners.load() != thread_count) {
        std::this_thread::yield();
    }
    if (initstatus.load() == init_ok) {
        if (!log->prepare_thread (qsize / thread_count)) {
            state.SkipWithError ("failed to prepare thread");
        }
    }
    log->run_logging (state);
    state.SetItemsProcessed (state.iterations());
    /* only the fist thread destroys the instance */
    if (state.thread_index != 0) {
        runners.fetch_sub (1);
        while (runners.load() != 0) {
            std::this_thread::yield();
        }
        return;
    }
    /* only destroying if it could be initialized */
    if (initstatus.load() != init_ok) {
      return;
    }
    while (runners.load() > 1) {
      std::this_thread::yield();
    }
    log->terminate();
    log->destroy();

    initstatus.store (unitialized);
    runners.fetch_sub (1);
};
/*----------------------------------------------------------------------------*/

#endif /* __LOGGER_BENCHMARK_ADAPTOR_HPP__ */