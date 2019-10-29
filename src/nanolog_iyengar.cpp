#include <stdio.h>

#include <NanoLog.hpp>
#include <NanoLog.cpp>

#include <nanolog_iyengar.hpp>
#include <benchmark_iterables.hpp>
/*----------------------------------------------------------------------------*/
bool nanolog_iyengar::create (int fixed_queues_bytes)
{
    nanolog::initialize (nanolog::GuaranteedLogger(), "./", "nanolog", 1);
    return true;
}
/*----------------------------------------------------------------------------*/
bool nanolog_iyengar::terminate()
{
    /* workaround: this is relying on internals to shut it down, not on the
    public API */
    ::nanolog::nanologger.reset();
    return true;
}
/*----------------------------------------------------------------------------*/
void nanolog_iyengar::destroy() {}
/*----------------------------------------------------------------------------*/
template <class T>
int nanolog_iyengar::run_logging (T& iterable)
{
    int success = 0;
    int i = 0;
    for (auto _ : iterable) {
        LOG_INFO << STRING_TO_LOG << ++i;
        ++success;
    }
    return success;
}
INSTANTIATE_RUN_LOGGING_TEMPLATES (nanolog_iyengar)
/*----------------------------------------------------------------------------*/
char const* nanolog_iyengar::get_name() const
{
    return "nanolog-iyengar";
}
/*----------------------------------------------------------------------------*/
char const* nanolog_iyengar::get_description() const
{
    return "Iyengar111's NanoLog GuaranteedLogger";
}
/*----------------------------------------------------------------------------*/
