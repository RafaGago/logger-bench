#include <iostream>

#include <stdio.h>
#include <logger.hpp>
#include <test_suite.hpp>

#ifdef HAS_MALC
#include <malc.hpp>
#endif

#ifdef HAS_MAL
#include <mal.hpp>
#endif

#ifdef HAS_NANOLOG
#include <nanolog_bench.hpp>
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
#endif
#ifdef HAS_MAL
    ret.push_back (std::unique_ptr<logger> (new mal_heap()));
    ret.push_back (std::unique_ptr<logger> (new mal_fixed()));
#endif
#ifdef HAS_NANOLOG
    ret.push_back (std::unique_ptr<logger> (new nanolog_bench()));
#endif
#ifdef HAS_SPDLOG
    ret.push_back (std::unique_ptr<logger> (new spdlog_async()));
    ret.push_back (std::unique_ptr<logger> (new spdlog_sync()));
#endif
#ifdef HAS_GLOG
    ret.push_back (std::unique_ptr<logger> (new glog()));
#endif
    return std::move (ret);
}
/*----------------------------------------------------------------------------*/
static void print_usage (logvector const& loggers)
{
    using namespace std;
    cout << "usage: logger-bench <iterations> <messages> <type 1> .. [<type N>]\n";
    cout << "  Where type is the logger type. Available options:\n";
    cout << "    [all]: All the available loggers.\n";
    for (auto const& l: loggers) {
        cout << "    [" << l->get_name() << "]: " ;
        cout << l->get_description() << "\n";
    }
}
//------------------------------------------------------------------------------
static bool try_handle_help (std::string str, logvector const& loggers)
{
    if (str.compare ("help") == 0
        || str.compare ("--help") == 0
        || str.compare ("-h") == 0
        ) {
        print_usage (loggers);
        return true;
    }
    return false;
}
/*----------------------------------------------------------------------------*/
static int parse_args(
    logvector& loggers, int& msgs, int& iterations, int argc, char const* argv[]
    )
{
    using namespace std;
    if (argc < 2) {
        cerr << "no message count specified\n\n";
        print_usage (loggers);
        return 1;
    }
    if (try_handle_help (argv[1], loggers)) {
        return -1;
    }
    char* end;
    iterations = strtol (argv[1], &end, 10);
    if (argv[1] == end) {
        cerr << "invalid itereration count: " << argv[1] << "\n\n" ;
        return 1;
    }
    if (iterations <= 0) {
        cerr << "the number of iterations must be bigger than 0\n\n" ;
        return 1;
    }
    if (argc < 3) {
        cerr << "no iteration count specified\n\n";
        print_usage (loggers);
        return 1;
    }
    if (try_handle_help (argv[2], loggers)) {
        return -1;
    }
    msgs = strtol (argv[2], &end, 10);
    if (argv[1] == end) {
        cerr << "invalid message count: " << argv[2] << "\n\n" ;
        return 1;
    }
    if (msgs <= 0) {
        cerr << "the message count must be bigger than 0\n\n" ;
        return 1;
    }
    if (argc < 4) {
        cerr << "no logger type specified\n\n";
        print_usage (loggers);
        return 1;
    }
    bool has_all = argc == 4 && std::string (argv[3]).compare ("all") == 0;
    std::vector<bool> enabled (loggers.size(), has_all);
    if (!has_all) {
        for (int arg = 3; arg < argc; ++arg) {
            std::string type (argv[arg]);
            if (try_handle_help (type, loggers)) {
               return -1;
            }
            int i = 0;
            if (type.compare ("all") == 0) {
                cerr << "\"all\" logger must be the only type specified\n\n" ;
                return 1;
            }
            for (; i < loggers.size(); ++i) {
                if (type.compare (loggers[i]->get_name()) == 0) {
                    enabled[i] = true;
                    break;
                }
            }
            if (i == loggers.size()) {
                cerr << "invalid logger type: " << type << "\n\n" ;
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
int main (int argc, char const* argv[])
{
    static const int mem = 16 * 1024 * 1024;
    int msgs       = 0;
    int iterations = 0;
    logvector loggers = init_logvector();
    int r = parse_args (loggers, msgs, iterations, argc, argv);
    if (r != 0) {
      return (r < 0) ? 0 : r;
    }
    test_results results;
    return run_tests (results, msgs, iterations, mem, loggers);
}
/*----------------------------------------------------------------------------*/
