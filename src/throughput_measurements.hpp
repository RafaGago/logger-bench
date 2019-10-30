#ifndef __THROUGHPUT_MEASUREMENTS__
#define __THROUGHPUT_MEASUREMENTS__

#include <cassert>
#include <cpuclock.hpp>
#include <nonstandard_compiler.hpp>

class throughput_iterator;
/*----------------------------------------------------------------------------*/
class throughput_measurements {
public:
    throughput_measurements()
    {
        reset();
    }

    void add_successes (uint32_t count)
    {
        m_successes += count;
    }

    bool prepare (int elements)
    {
        if (elements < 0) {
            return false;
        }
        reset();
        m_expected = elements;
        return true;
    }

    int get_faults() const
    {
        return m_expected - m_successes;
    }

    int get_expected() const
    {
        return m_expected;
    }

    cpuclock_t get_start() const
    {
        return m_start;
    }

    cpuclock_t get_end() const
    {
        return m_end;
    }

    inline forceinline throughput_iterator begin();
    inline forceinline throughput_iterator end();
private:
    friend class throughput_iterator;

    void reset()
    {
        m_successes = 0;
        m_expected = 0;
        m_start = 0;
        m_end = 0;
    }

    cpuclock_t m_start;
    cpuclock_t m_end;
    int        m_expected;
    uint64_t   m_successes;
};
/*----------------------------------------------------------------------------*/
class throughput_iterator
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
    friend class throughput_measurements;
    forceinline throughput_iterator() {}

    forceinline explicit throughput_iterator (throughput_measurements& tm) :
        m_throughput (&tm)
    {
        m_remaining = tm.get_expected();
        m_expected  = m_remaining;
    }
public:
    forceinline dummy operator*() const
    {
        return dummy();
    }

    forceinline throughput_iterator& operator++()
    {
        assert (m_remaining > 0);
        --m_remaining;
        return *this;
    }

    forceinline bool operator!= (throughput_iterator const&)
    {
        if (likely (m_remaining > 0)) {
            return true;
        }
        m_throughput->m_end = cpuclock_get();
        return false;
    }

private:
    uint64_t                 m_remaining;
    uint64_t                 m_expected;
    throughput_measurements* m_throughput;
};

inline forceinline throughput_iterator throughput_measurements::begin()
{
    return throughput_iterator (*this);
}
inline forceinline throughput_iterator throughput_measurements::end()
{
    this->m_start = cpuclock_get();
    return throughput_iterator();
}
/*----------------------------------------------------------------------------*/

#endif /*__THROUGHPUT_MEASUREMENTS__*/
