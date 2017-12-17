#ifndef __LATENCY_MEASUREMENTS__
#define __LATENCY_MEASUREMENTS__

#include <vector>
#include <cstdint>
#include <cassert>
#include <algorithm>

/*----------------------------------------------------------------------------*/
class latency_measurements {
public:
    latency_measurements()
    {
        reset();
    }
    void add_sample (uint32_t ns, bool is_success)
    {
        m_results.push_back (ns);
        m_min = std::min (m_min, ns);
        m_max = std::max (m_max, ns);
        m_success += (is_success == true);
    }
    bool prepare (int elements)
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
            add_sample (v, false);
        }
        lm.reset();
    }
    uint32_t get_percentile_ns (double percentile) const
    {
        assert (m_results.size());
        double step  = ((double) m_results.size()) / 100.;
        uint64_t idx = (uint64_t) round (step * percentile);
        /*improvement for small data sets: some type of interpolation*/
        return m_results[std::min (idx, m_results.size() - 1)];
    }
    uint32_t get_min_ns() const
    {
        return m_min;
    }
    uint32_t get_max_ns() const
    {
        return m_max;
    }
    uint64_t get_successes() const
    {
        return m_success;
    }
private:
    void reset()
    {
        m_results.clear();
        m_results.reserve (0);
        m_min      = (uint32_t) -1;
        m_max      = 0;
        m_expected = 0;
        m_success  = 0;
    }

    std::vector<uint32_t> m_results;
    uint32_t              m_min;
    uint32_t              m_max;
    int                   m_expected;
    uint64_t              m_success;
};
/*----------------------------------------------------------------------------*/

#endif /*__LATENCY_MEASUREMENTS__*/
