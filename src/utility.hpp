#ifndef __UTILITY_HPP__
#define __UTILITY_HPP__

static const double ns_sec = 1000000000.;
/*----------------------------------------------------------------------------*/
template <class T>
static void to_si_units (T& stream, double val, char const* magnitude = nullptr)
{
    /* an intelligent compiler can unroll this if necessary */
    static const char units[] = "GMk munp";
    double unit = 1000000000.;
    if (val == 0.) {

        if (magnitude) {
            stream << "0(" << magnitude << ")";
        }
        else {
            stream << "0";
        }
        return;
    }
    for (int i = 0; i < (sizeof units - 1); ++i) {
        double converted = val / unit;
        if (converted < 1. && (i < (sizeof units - 2))) {
            unit /= 1000.;
            continue;
        }
        stream << converted;
        if (magnitude) {
            stream << "(";
        }
        if (units[i] != ' ') {
            stream << units[i];
        }
        if (magnitude) {
            stream << magnitude << ")";
        }
        return;
    }
}
/*----------------------------------------------------------------------------*/
#endif /* __TEST_SUITE__ */