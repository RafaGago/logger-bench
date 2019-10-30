#ifndef __LATENCY_MEASUREMENTS__
#define __LATENCY_MEASUREMENTS__

#include <vector>
#include <atomic>
#include <cassert>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <cpuclock.hpp>
#include <nonstandard_compiler.hpp>

class latency_iterator;
/*----------------------------------------------------------------------------*/
class latency_measurements {
public:
    latency_measurements()
    {
        reset();
    }

    void add_sample (cpuclock_t s)
    {
        uint32_t v = s;
        assert (s == v); /* max 4.294967295 seconds/entry*/
        m_results.push_back (v);
    }

    void add_successes (std::size_t count)
    {
        m_success += count;
    }

    bool prepare (std::size_t elements)
    {
        if (elements < 0) {
            return false;
        }
        reset();
        m_results.reserve (elements);
        m_expected = elements;
        return true;
    }
    bool finish()
    {
        if (m_results.size() != m_expected) {
            assert (false);
            return false;
        }
        m_expected = 0;
        std::sort (m_results.begin(), m_results.end());
        return true;
    }
    /*lm is destroyed*/
    void join (latency_measurements& lm)
    {
        m_results.reserve (m_results.capacity() + lm.m_results.size());
        m_expected += lm.m_results.size();
        m_success  += lm.m_success;
        for (uint32_t v : lm.m_results) {
            add_sample (v);
        }
        lm.reset();
    }
    uint32_t get_percentile_ns (double percentile) const
    {
        assert (m_results.size());
        double step  = ((double) m_results.size()) / 100.;
        uint64_t idx = (uint64_t) std::round (step * percentile);
        /*improvement for small data sets: some type of interpolation*/
        return m_results[std::min (idx, m_results.size() - 1)];
    }

    std::size_t get_faults() const
    {
        return m_expected - m_success;
    }

    std::size_t get_expected() const
    {
        return m_expected;
    }

    inline forceinline latency_iterator begin();
    inline forceinline latency_iterator end();

private:
    void reset()
    {
        m_results.clear();
        m_results.reserve (0);
        m_min               = (uint32_t) -1;
        m_max               = 0;
        m_expected          = 0;
        m_success           = 0;
    }

    std::vector<uint32_t> m_results;
    uint32_t              m_min;
    uint32_t              m_max;
    std::size_t           m_expected;
    std::size_t           m_success;
};
/*----------------------------------------------------------------------------*/
class latency_iterator
{
/* wraps latency measurements in an iterator as Google Benchmark does */
public:
    struct dummy {};
    typedef std::forward_iterator_tag iterator_category;
    typedef dummy value_type;
    typedef dummy reference;
    typedef dummy pointer;
    typedef std::ptrdiff_t difference_type;

private:
    friend class latency_measurements;
    forceinline latency_iterator() {}

    forceinline explicit latency_iterator (latency_measurements& lm) :
        m_latency (&lm)
    {
        m_remaining = lm.get_expected();
        m_expected  = m_remaining;
    }
public:
    forceinline dummy operator*() const
    {
        return dummy();
    }

    forceinline latency_iterator& operator++()
    {
        assert (m_remaining > 0);
        --m_remaining;
        return *this;
    }

    forceinline bool operator!= (latency_iterator const&)
    {
        if (likely (m_remaining != m_expected)) {
            m_latency->add_sample (cpuclock_get() - m_prev_point);
            m_prev_point = cpuclock_get();
            return m_remaining != 0;
        }
        /*first iteration before even entering the loop*/
        m_prev_point = cpuclock_get();
        return true;
    }

private:
    latency_measurements* m_latency;
    std::size_t           m_remaining;
    std::size_t           m_expected;
    cpuclock_t            m_prev_point;
};
/*----------------------------------------------------------------------------*/
inline forceinline latency_iterator latency_measurements::begin()
{
    return latency_iterator (*this);
}
inline forceinline latency_iterator latency_measurements::end()
{
    return latency_iterator();
}
/*----------------------------------------------------------------------------*/
#endif /*__LATENCY_MEASUREMENTS__*/
