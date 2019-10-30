#include <iostream>
#include <algorithm>
#include <string>

#include <stdio.h>
#include <logger.hpp>
#include <test_suite.hpp>
#include <benchmark/benchmark.h>
#include <google_benchmark_adaptor.hpp>
#include <cpuclock.hpp>

#ifdef HAS_MALC
#include <malc.hpp>
#endif

#ifdef HAS_MAL
#include <mal.hpp>
#endif

#ifdef HAS_NANOLOG_IYENGAR
#include <nanolog_iyengar.hpp>
#endif

#ifdef HAS_SPDLOG
#include <spdlog.hpp>
#endif

#ifdef HAS_GLOG
#include <glog.hpp>
#endif

#ifdef HAS_G3LOG
#include <g3log.hpp>
#endif

#ifdef HAS_NANOLOG
#include <nanolog.hpp>
#endif

typedef std::vector<std::unique_ptr<logger>> logvector;
/*----------------------------------------------------------------------------*/
static logvector init_logvector()
{
    logvector ret;
#ifdef HAS_MALC
    ret.push_back (std::unique_ptr<logger> (new malc_tls()));
    ret.push_back (std::unique_ptr<logger> (new malc_heap()));
    ret.push_back (std::unique_ptr<logger> (new malc_fixed()));
    ret.push_back (std::unique_ptr<logger> (new malc_fixed_cpu()));
    ret.push_back (std::unique_ptr<logger> (new malc_tls_heap()));
    ret.push_back (std::unique_ptr<logger> (new malc_fixed_heap()));
    ret.push_back (std::unique_ptr<logger> (new malc_fixed_cpu_heap()));
#endif
#ifdef HAS_MAL
    ret.push_back (std::unique_ptr<logger> (new mal_heap()));
    ret.push_back (std::unique_ptr<logger> (new mal_fixed()));
#endif
#ifdef HAS_NANOLOG_IYENGAR
    ret.push_back (std::unique_ptr<logger> (new nanolog_iyengar()));
#endif
#ifdef HAS_SPDLOG
    ret.push_back (std::unique_ptr<logger> (new spdlog_async()));
    ret.push_back (std::unique_ptr<logger> (new spdlog_sync()));
#endif
#ifdef HAS_GLOG
    ret.push_back (std::unique_ptr<logger> (new glog()));
#endif
#ifdef HAS_G3LOG
    ret.push_back (std::unique_ptr<logger> (new g3log()));
#endif
#ifdef HAS_NANOLOG
    ret.push_back (std::unique_ptr<logger> (new nanolog()));
#endif
    return std::move (ret);
}
/*----------------------------------------------------------------------------*/
static void print_own_usage (logvector const& loggers)
{
    using namespace std;
    cout << "usage: own <iterations> <logger-type 1> .. [<logger-type N>]\n";
    cout << "  Where \"logger-type\" can be any of the values below:\n";
    cout << "    [all]: All the available loggers.\n";
    for (auto const& l: loggers) {
        cout << "    [" << l->get_name() << "]: " ;
        cout << l->get_description() << "\n";
    }
}
//------------------------------------------------------------------------------
static bool is_help (std::string str)
{
    return str.compare ("help") == 0
        || str.compare ("--help") == 0
        || str.compare ("-h") == 0;
}
/*----------------------------------------------------------------------------*/
static bool strtosize_t(
    std::size_t& v, char const* str, char const* errmsg_prefix
    )
{
    unsigned long l;
    try {
        l = std::stoull (str);
        v = (std::size_t) l;
        if (v != l) {
            std::cerr
                << errmsg_prefix
                << ", value out of range: \""
                << str
                << "\"\n";
            return false;
        }
        return true;
    }
    catch (...) {
        std::cerr << errmsg_prefix << ": \"" << str << "\"\n";
        return false;
    }
}
/*----------------------------------------------------------------------------*/
#define LITLEN(x) (sizeof x - 1)
#define ENDSWITHLIT(str, lit) (str.rfind(lit) == str.size() - LITLEN(lit))
/*----------------------------------------------------------------------------*/
static bool parse_qbytes (std::size_t& v, char const* str)
{
    std::string s(str);
    std::transform (s.begin(), s.end(), s.begin(), ::tolower);

    unsigned long long factor = 1;
    if (ENDSWITHLIT (s, "kb")) {
        factor = 1024;
    }
    if (ENDSWITHLIT (s, "mb")) {
        factor = 1024 * 1024;
    }
    if (ENDSWITHLIT (s, "gb")) {
        factor = 1024 * 1024 * 1024;
    }
    if (factor != 1) {
        s.erase (s.size() - LITLEN("xb"));
    }
    static char const errmsg[] =
        "invalid recommended logger memory usage";

    if (!strtosize_t(v, s.c_str(), errmsg)) {
        return false;
    }
    unsigned long long m = ((unsigned long long) v) * factor;
    v = (std::size_t) m;
    if (v != m) {
        std::cerr
            << errmsg
            << ", value out of range: \""
            << str
            << "\"\n";
        return false;
    }
    return true;
}
/*----------------------------------------------------------------------------*/
static int parse_args(
    logvector&   loggers,
    std::size_t& iterations,
    std::size_t  msgs,
    int          argc,
    char**       argv
    )
{
    using namespace std;
    for (int i = 1; i < argc; ++i) {
        if (is_help (argv[i])) {
            print_own_usage (loggers);
            return -1;
        }
    }
    int currarg = 1;
    if (argc < currarg + 1) {
        cerr << "no iteration count specified\n";
        print_own_usage (loggers);
        return 1;
    }
    if (!strtosize_t (iterations, argv[currarg], "invalid iteration count")) {
        return 1;
    }
    if (msgs < max_threads) {
        cerr << "the message count must be bigger than "<<  max_threads
             << "\n";
        return 1;
    }
    ++currarg;
    if (argc < currarg + 1) {
        cerr << "no logger type specified\n";
        print_own_usage (loggers);
        return 1;
    }
    bool has_all = (argc == currarg + 1);
    has_all = has_all && std::string (argv[currarg]).compare ("all") == 0;
    std::vector<bool> enabled (loggers.size(), has_all);
    if (!has_all) {
        for (int narg = currarg; narg < argc; ++narg) {
            std::string type (argv[narg]);
            int i = 0;
            if (type.compare ("all") == 0) {
                cerr << "\"all\" logger must be the only type specified\n" ;
                return 1;
            }
            for (; i < loggers.size(); ++i) {
                if (type.compare (loggers[i]->get_name()) == 0) {
                    enabled[i] = true;
                    break;
                }
            }
            if (i == loggers.size()) {
                cerr << "invalid logger type: " << type << "\n" ;
                return 1;
            }
        }
        logvector enabled_lv;
        for (int i = 0; i < loggers.size(); ++i) {
            if (enabled[i]) {
                enabled_lv.push_back (std::move (loggers[i]));
            }
        }
        loggers = std::move (enabled_lv);
    }
    return 0;
}
/*----------------------------------------------------------------------------*/
int own_subcommand (int argc, char** argv, int qsize, std::size_t messages)
{
    std::size_t iterations = 0;
    logvector loggers = init_logvector();
    int r = parse_args (loggers, iterations, messages, argc, argv);
    if (r != 0) {
      return (r < 0) ? 0 : r;
    }

    std::cout << "estimating cpu clock frequency ... " << std::flush;
    cpuclock_init();
    to_si_units (std::cout, cpuclock_get_freq(), "Hz");
    std::cout << ", call overhead: ";
    to_si_units(
        std::cout,
        cpuclock_get_call_overhead_ns() / ns_sec,
        "s");
    std::cout << "\n";

    test_results results;
    return run_tests (results, messages, iterations, qsize, loggers);
}
/*----------------------------------------------------------------------------*/
int gbench_subcommand (int argc, char** argv, int qsize, std::size_t messages)
{
    if (messages != 0 && messages < max_threads) {
        std::cerr
            << "the message count has to be 0 or bigger than "<<  max_threads
            << "\n";
        return 1;
    }
    ::benchmark::Initialize (&argc, argv);
    if (::benchmark::ReportUnrecognizedArguments(argc, argv)) {
      return 1;
    }
    logvector loggers = init_logvector();
    for (std::unique_ptr<logger>& log: loggers) {
        for (int i = 0; i < thread_count_idxs; ++i) {
            auto b = ::benchmark::RegisterBenchmark(
                log->get_name(),
                run_google_benchmark,
                &(*log),
                qsize
                );
            b->Threads (thread_count[i]);
            if (messages) {
                b->Iterations (messages / thread_count[i]);
            }
            b->ComputeStatistics(
                "max", [](const std::vector<double>& v) -> double
            {
                return *std::max_element (v.begin(), v.end());
            });
            b->ComputeStatistics(
                "99%", [](const std::vector<double>& v) -> double
            {
                std::vector<double> cp = v;
                std::sort (cp.begin(), cp.end());
                return cp.size() ?
                    cp[(unsigned long) (((double) cp.size()) * 0.99)] : 0.;
            });
            b->ComputeStatistics(
                "50%", [](const std::vector<double>& v) -> double
            {
                std::vector<double> cp = v;
                std::sort (cp.begin(), cp.end());
                return cp.size() ?
                    cp[(unsigned long) (((double) cp.size()) * 0.5)] : 0.;
            });
        }
    }
    ::benchmark::RunSpecifiedBenchmarks();
}
/*----------------------------------------------------------------------------*/
void print_main_help (void)
{
    using namespace std;
    cout << "usage: logger-bench <logger-mem-usage> <message-count> <subcommand> ...\n";
    cout << "  Where:\n";
    cout << "    -logger-mem-usage: recommended memory usage in bytes to be \n";
    cout << "      used by each logger. Accepts kB, MB and GB suffixes. \n";
    cout << "\n";
    cout << "    -message-count: the message count for each single test. If\n";
    cout << "      left to zero the \"google\" subcommand will try to find a\n";
    cout << "      message count automatically.\n";
    cout << "\n";
    cout << "    -subcommand: benchmarking implementation. Each of them have\n";
    cout << "      their own arguments and help. Valid values are:\n";
    cout << "        [own]: Own benchmarking implementation.\n";
    cout << "        [google]: Google benchmark implementation.\n";
}
/*----------------------------------------------------------------------------*/
int main (int argc, char* argv[])
{
    using namespace std;
    static const int args = 3;
    if (argc < args + 1) {
        std::cerr
            << "Expecting "
            << args
            << " arguments. Got: "
            << argc - 1
            << "\n";
        print_main_help();
        return 1;
    }
    for (int i = 1; i < args + 1; ++i) {
        if (is_help (argv[i])) {
            print_main_help();
            return 0;
        }
    }
    std::size_t qbytes;
    if (!parse_qbytes (qbytes, argv[1])) {
        return 1;
    }
    std::size_t msgs;
    if (!strtosize_t (msgs, argv[2], "invalid message count")) {
        return 1;
    }
    std::string ltype (argv[3]);
    if (ltype.compare("own") == 0) {
        return own_subcommand (argc - args, argv + args, qbytes, msgs);
    }
    else if (ltype.compare("google") == 0) {
        return gbench_subcommand (argc - args, argv + args, qbytes, msgs);
    }
    else {
        std::cerr
            << "invalid subcommand. Valid values: [own, google], got: \""
            << ltype
            << "\"\n";
        return 1;
    }
}
/*----------------------------------------------------------------------------*/
