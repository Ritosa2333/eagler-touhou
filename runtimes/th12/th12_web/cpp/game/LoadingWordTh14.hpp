#pragma once
#include "Types.hpp"
#include "PortTitle.hpp"
#include <type_traits>
namespace th11 {
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
// One native aligned word per access. Compound flag operations deliberately
// keep a separate load/store, preserving retail's non-LOCK lost-store cases.
template<class T> class LoadingWordTh14 {
    static_assert(std::is_integral_v<T> && sizeof(T)==4);
    T value;
public:
    LoadingWordTh14()noexcept=default;
    constexpr LoadingWordTh14(T initial)noexcept:value(initial){}
    LoadingWordTh14(const LoadingWordTh14& source)noexcept:value(T(source)){}
    operator T()const noexcept{return __atomic_load_n(&value,__ATOMIC_SEQ_CST);}
    LoadingWordTh14& operator=(T next)noexcept{__atomic_store_n(&value,next,__ATOMIC_SEQ_CST);return *this;}
    LoadingWordTh14& operator=(const LoadingWordTh14& next)noexcept{return *this=T(next);}
    LoadingWordTh14& operator|=(T bits)noexcept{return *this=T(*this)|bits;}
    LoadingWordTh14& operator&=(T bits)noexcept{return *this=T(*this)&bits;}
    LoadingWordTh14& operator^=(T bits)noexcept{return *this=T(*this)^bits;}
};
static_assert(sizeof(LoadingWordTh14<i32>)==4&&alignof(LoadingWordTh14<i32>)==4);
static_assert(sizeof(LoadingWordTh14<u32>)==4&&alignof(LoadingWordTh14<u32>)==4);
static_assert(std::is_standard_layout_v<LoadingWordTh14<i32>>&&std::is_trivially_destructible_v<LoadingWordTh14<i32>>);
#else
template<class T> using LoadingWordTh14=T;
#endif
}
