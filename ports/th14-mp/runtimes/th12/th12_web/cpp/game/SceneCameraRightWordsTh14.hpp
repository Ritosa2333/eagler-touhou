#pragma once
#include "SceneCamera.hpp"
#include <cstdint>
#include <cstring>
namespace th11 {
inline void th14_publish_camera_right_word(float& target,float value)noexcept{
    u32 bits;std::memcpy(&bits,&value,4);
    __atomic_store_n(reinterpret_cast<u32*>(&target),bits,__ATOMIC_SEQ_CST);
}
inline void th14_publish_camera_right_xy(Vec3& target,const Vec3& value)noexcept{
    std::uint64_t bits;std::memcpy(&bits,&value,8);
    if((reinterpret_cast<std::uintptr_t>(&target)&7u)==0){
        __atomic_store_n(reinterpret_cast<std::uint64_t*>(&target),bits,__ATOMIC_SEQ_CST);
    }else{
        // Other, nonshared cameras retain their unaligned qword publication.
        // The shared world-camera owner enforces alignment independently.
        __atomic_signal_fence(__ATOMIC_SEQ_CST);
        std::memcpy(&target,&bits,8);
        __atomic_signal_fence(__ATOMIC_SEQ_CST);
    }
}
}
