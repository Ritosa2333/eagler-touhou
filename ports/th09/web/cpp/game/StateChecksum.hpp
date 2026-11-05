#pragma once
#include "Types.hpp"
#include <type_traits>
namespace th09 {
// In-process restore oracle. Resource addresses and padding are deliberately
// retained, so this is NOT a portable network/desync hash.
struct PlayerFrameContext;struct CpuContext;struct FrameTiming;
struct StateChecksum {
    u32 value=2166136261u;
    void Bytes(const void* p,std::size_t n){const auto* b=static_cast<const u8*>(p);while(n--)value=(value^*b++)*16777619u;}
    void One(const PlayerFrameContext&);
    void One(const CpuContext&);
    void One(const FrameTiming&);
    template<class T>void One(const T& v){static_assert(std::is_trivially_copyable_v<T>);Bytes(&v,sizeof(v));}
    template<class... T>void Add(const T&... fields){(One(fields),...);}
};
}
