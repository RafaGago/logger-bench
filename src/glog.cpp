#include <glog/logging.h>

#include <glog.hpp>
#include <benchmark_iterables.hpp>

/*----------------------------------------------------------------------------*/
glog::glog()
{
    m_init = false;
}
/*----------------------------------------------------------------------------*/
bool glog::create (int fixed_queues_bytes)
{
    if (m_init) {
        return true;
    }
    FLAGS_log_dir         = "./";
    FLAGS_logtostderr     = false;
    FLAGS_alsologtostderr = false;
    FLAGS_stderrthreshold = 3;
    /*glog log names are minute-based, it needs a file size that isn't
      filled in a minute, otherwise it compains a lot.*/
    FLAGS_max_log_size =  1024 * 1024 * 1024;
    google::SetLogDestination (google::INFO, "");
    google::SetLogDestination (google::WARNING, "");
    google::SetLogDestination (google::ERROR, "");
    google::SetLogDestination (google::FATAL, "");
    google::SetLogFilenameExtension (".glog");
    google::InitGoogleLogging ("");
    m_init = true;
    return true;
}
/*----------------------------------------------------------------------------*/
bool glog::terminate()
{
    google::FlushLogFiles (google::INFO);
    return true;
}
/*----------------------------------------------------------------------------*/
void glog::destroy() {}
/*----------------------------------------------------------------------------*/
template <class T>
int glog::run_logging (T& iterable)
{
    int success = 0;
    int i = 0;
    for (auto _ : iterable) {
        LOG (INFO) << STRING_TO_LOG << ++i;
        ++success;
    }
    return success;
}
INSTANTIATE_RUN_LOGGING_TEMPLATES (glog)
/*----------------------------------------------------------------------------*/
char const* glog::get_name() const
{
    return "glog";
}
/*----------------------------------------------------------------------------*/
char const* glog::get_description() const
{
    return "Google log";
}
/*----------------------------------------------------------------------------*/
