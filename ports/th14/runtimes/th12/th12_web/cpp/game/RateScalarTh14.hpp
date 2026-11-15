#pragma once
#include "PortTitle.hpp"
#include <type_traits>

namespace th11 {
// Timers retain a pointer to the shared retail float, including while Main
// temporarily replaces it. Each access is a separate word, never a snapshot
// or a thread-local rate. Non-worker builds keep their ordinary float ABI.
inline float th14_rate_load(const float* source)noexcept{
#if PORT_TITLE==14 && defined(TH14_CPP_STAGE_WORKER)
    float value;__atomic_load(source,&value,__ATOMIC_SEQ_CST);return value;
#else
    return *source;
#endif
}
inline void th14_rate_store(float* target,float value)noexcept{
#if PORT_TITLE==14 && defined(TH14_CPP_STAGE_WORKER)
    __atomic_store(target,&value,__ATOMIC_SEQ_CST);
#else
    *target=value;
#endif
}
#if PORT_TITLE==14 && defined(TH14_CPP_STAGE_WORKER)
class AnmRateTh14 {
    float value;
public:
    constexpr AnmRateTh14(float initial=1)noexcept:value(initial){}
    AnmRateTh14(const AnmRateTh14& source)noexcept:value(float(source)){}
    operator float()const noexcept{return th14_rate_load(&value);}
    AnmRateTh14& operator=(float next)noexcept{th14_rate_store(&value,next);return *this;}
    AnmRateTh14& operator=(const AnmRateTh14& next)noexcept{return *this=float(next);}
    // Existing timer/knife pointers keep the exact same live storage. Their
    // indirect accesses must use the scalar helpers above, too.
    float* operator&()noexcept{return &value;}
    const float* operator&()const noexcept{return &value;}
};
static_assert(sizeof(AnmRateTh14)==sizeof(float)&&alignof(AnmRateTh14)==alignof(float));
static_assert(std::is_standard_layout_v<AnmRateTh14>);
#else
using AnmRateTh14=float;
#endif
}
