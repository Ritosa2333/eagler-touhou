#pragma once
#include "Types.hpp"
#include "PortTitle.hpp"
#include <type_traits>
namespace th11 {
#if PORT_TITLE==14 && defined(TH14_CPP_STAGE_WORKER)
// Native aligned words can be read by Main draw while the loader writes.
// Each expression keeps separate scalar loads/stores. In particular compound
// operations are not fetch_or/fetch_and: retail non-LOCK updates can lose a
// competing store. This wrapper does not make a multi-word snapshot or lock.
class AnmFlagWordTh14 {
    u32 value;
public:
    AnmFlagWordTh14()noexcept=default;
    constexpr AnmFlagWordTh14(u32 initial)noexcept:value(initial){}
    AnmFlagWordTh14(const AnmFlagWordTh14& source)noexcept:value(u32(source)){}
    operator u32()const noexcept{return __atomic_load_n(&value,__ATOMIC_SEQ_CST);}
    AnmFlagWordTh14& operator=(u32 next)noexcept{__atomic_store_n(&value,next,__ATOMIC_SEQ_CST);return *this;}
    AnmFlagWordTh14& operator=(const AnmFlagWordTh14& next)noexcept{return *this=u32(next);}
    AnmFlagWordTh14& operator|=(u32 bits)noexcept{return *this=u32(*this)|bits;}
    AnmFlagWordTh14& operator&=(u32 bits)noexcept{return *this=u32(*this)&bits;}
    AnmFlagWordTh14& operator^=(u32 bits)noexcept{return *this=u32(*this)^bits;}
};
static_assert(sizeof(AnmFlagWordTh14)==sizeof(u32)&&alignof(AnmFlagWordTh14)==alignof(u32));
static_assert(std::is_standard_layout_v<AnmFlagWordTh14> && std::is_trivially_destructible_v<AnmFlagWordTh14>);
#else
using AnmFlagWordTh14=u32;
#endif
}
