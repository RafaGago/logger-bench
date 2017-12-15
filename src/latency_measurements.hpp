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
        m_expected = 0;
    }
    void add_sample (uint32_t ns, bool is_success)
    {
        m_results.push_back (ns);
        m_min = std::min (m_min, ns);
        m_max = std::max (m_max, ns);
    }
    bool prepare (int elements)
    {
        if (elements < 0 || (elements % 100) != 0) {
            return false;
        }
        m_results.clear();
        m_results.reserve (elements);
        m_min      = (uint32_t) -1;
        m_max      = 0;
        m_expected = elements;
    }
    bool finish()
    {
        if (m_results.size() != m_expected) {
            return false;
        }
        m_expected = 0;
        std::sort (m_results.begin(), m_results.end());
        return true;
    }
    uint32_t get_percentile_ns (int percentile) const
    {
        assert (m_results.size());
        unsigned idx = (m_results.size() / 100) * percentile;
        return m_results[idx];
    }
    uint32_t get_min_ns() const
    {
        return m_min;
    }
    uint32_t get_max_ns() const
    {
        return m_max;
    }
private:
    std::vector<uint32_t> m_results;
    uint32_t              m_min;
    uint32_t              m_max;
    int                   m_expected;
};
/*----------------------------------------------------------------------------*/

#endif /*__LATENCY_MEASUREMENTS__*/
