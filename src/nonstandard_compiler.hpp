#ifndef __NONSTANDARD_COMPILER__
#define __NONSTANDARD_COMPILER__

#if defined(__GNUC__) || defined(__clang__)
    #define likely(x)   __builtin_expect (!!(x), 1)
    #define unlikely(x) __builtin_expect (!!(x), 0)
#else
    #define likely(x)   (x)
    #define unlikely(x) (x)
#endif

#if defined(__GNUC__)
  #define forceinline __attribute__((always_inline))
#elif defined(_MSC_VER) && !defined(__clang__)
  #define forceinline __forceinline
#else
  #define forceinline
#endif

#endif /* __NONSTANDARD_COMPILER__ */