#ifndef __TEST_RESULTS__
#define __TEST_RESULTS__

#include <cstdint>
#include <cstddef>
#include <cstring>

#include <utility.hpp>
/*----------------------------------------------------------------------------*/
class test_result {
public:
    int         thread_count;
    std::size_t throughput_faults;
    std::size_t latency_faults;
    std::size_t messages;
    uint64_t    producer_ns;
    uint64_t    total_ns;
    uint32_t    latency_ns_50;
    uint32_t    latency_ns_75;
    uint32_t    latency_ns_85;
    uint32_t    latency_ns_90;
    uint32_t    latency_ns_95;
    uint32_t    latency_ns_97;
    uint32_t    latency_ns_99;
    uint32_t    latency_ns_999;
    uint32_t    latency_ns_min;
    uint32_t    latency_ns_max;

    template <class T>
    void to_stream (T& stream)
    {
        double r;
        stream << "thr: " << thread_count;
        stream << ", faults: ";
        to_si_units (stream, throughput_faults);
        stream << ", rate: ";
        r  = (double) messages;
        r /= (double) producer_ns;
        r *= ns_sec;
        to_si_units (stream, r, "msgs/s");
        stream << ", lat 50%: ";
        to_si_units (stream, ((double) latency_ns_50) / ns_sec, "s");
        stream << ", lat 99.9%: ";
        to_si_units (stream, ((double) latency_ns_999) / ns_sec, "s");
        stream << ", lat max: ";
        to_si_units (stream, ((double) latency_ns_max) / ns_sec, "s");
    }

    void apply_clock_correction (uint32_t ns)
    {
        clock_correct (producer_ns , ns);
        clock_correct (total_ns , ns);
        clock_correct (latency_ns_50 , ns);
        clock_correct (latency_ns_75 , ns);
        clock_correct (latency_ns_85 , ns);
        clock_correct (latency_ns_90 , ns);
        clock_correct (latency_ns_95 , ns);
        clock_correct (latency_ns_97 , ns);
        clock_correct (latency_ns_99 , ns);
        clock_correct (latency_ns_999 , ns);
        clock_correct (latency_ns_min , ns);
        clock_correct (latency_ns_max , ns);
    }
private:
    template <class T>
    static inline void clock_correct (T& v, uint32_t ns)
    {
        auto old = v;
        v -= ns;
        assert (old > v);
    }
};
/*----------------------------------------------------------------------------*/
/* A 3d dynamic array with validation (wheel reinvention)*/
/*----------------------------------------------------------------------------*/
class test_results {
public:
    test_results()
    {
        m_results = nullptr;
        deinit();
    }
    test_results (test_results&& mv)
    {
        memcpy (this, &mv, sizeof *this);
        mv.m_results = nullptr;
        mv.deinit();
    }
    ~test_results()
    {
        deinit();
    }
    void init(
        std::size_t logger_count,
        std::size_t thread_count_idxs,
        std::size_t iterations
        )
    {
        m_iterations        = iterations;
        m_logger_count      = logger_count;
        m_thread_count_idxs = thread_count_idxs;
        m_results =
            new test_result[logger_count * thread_count_idxs * iterations];
    }
    test_results& operator= (test_results&& mv)
    {
      if (&mv == this) {
          return *this;
      }
      deinit();
      memcpy (this, &mv, sizeof *this);
      mv.m_results = nullptr;
      mv.deinit();
      return *this;
    }
    test_result* at(
        int logger_idx, int thread_count_idx, int iteration
        )
    {
        if (thread_count_idx >= m_thread_count_idxs ||
          logger_idx >= m_logger_count ||
          iteration >= m_iterations
          ) {
            assert (false);
            return nullptr;
        }
        if (!m_results) {
            return nullptr;
        }
        test_result* addr = m_results;
        addr += logger_idx * (m_thread_count_idxs * m_iterations);
        addr += thread_count_idx * (m_iterations);
        addr += iteration;
        return addr;
    }

private:
    void deinit()
    {
        if (m_results) {
            delete [] m_results;
            m_results = nullptr;
        }
        m_iterations   = 0;
        m_logger_count = 0;
        m_thread_count_idxs = 0;
    }
    test_result* m_results;
    std::size_t  m_iterations;
    std::size_t  m_logger_count;
    std::size_t  m_thread_count_idxs;
};
/*----------------------------------------------------------------------------*/

#endif