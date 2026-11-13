#pragma once
#include "SceneCameraReadJobTh14.hpp"
namespace th11 {
// Retail40e363/40e7ef copy75 words before446400. Match the existing
// update publication's mapping and scalar shared-memory access policy.
// This does not make the entire camera atomic or fix other camera writers.
inline void th14_publish_draw_camera_words(SceneCamera& target,const SceneCamera& source)noexcept{
    for(u32 word=0;word<75;++word){
        const u32 offset=SceneCameraReadJobTh14::source_offset(word);
        u32 bits;std::memcpy(&bits,reinterpret_cast<const u8*>(&source)+offset,4);
        __atomic_store_n(reinterpret_cast<u32*>(reinterpret_cast<u8*>(&target)+offset),bits,__ATOMIC_SEQ_CST);
    }
}
}
