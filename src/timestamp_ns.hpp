#ifndef __TIMESTAMP_NS__
#define __TIMESTAMP_NS__

/*----------------------------------------------------------------------------*/
static inline uint64_t ns_now()
{
    using namespace std::chrono;
    return duration_cast<nanoseconds>(
        system_clock::now().time_since_epoch()
        ).count();
}
/*----------------------------------------------------------------------------*/

#endif
