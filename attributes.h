
#pragma once

#if __GNUC__ || __clang__

#define builtin_unreachable() __builtin_unreachable()

#define attr_pure               __attribute__ ((pure)) // pure or const?
#define attr_deprecated(msg)    __attribute__((__deprecated__(msg)))
#define attr_checkignoredresult _Pragma("GCC diagnostic error \"-Wunused-result\"") \
                                __attribute__ ((warn_unused_result))
#define forceinline             __attribute__((always_inline))
#define noinline                __attribute__((__noinline__))

#elif _MSC_VER

#define builtin_unreachable() __assume(0)
#define constexpr const

#define attr_pure               __declspec(noalias)
#define attr_deprecated(msg)    __declspec(deprecated(msg))
#define attr_checkignoredresult _Check_return_
#define forceinline             __forceinline
#define noinline                __declspec(noinline)

#endif