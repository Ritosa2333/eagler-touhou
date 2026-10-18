#pragma once
#include "../game/GraphicsMath.hpp"
#include <array>
#include <cstdint>
#include <cstring>

extern "C" {
extern std::uint_fast8_t softfloat_roundingMode,extF80_roundingPrecision;
extern std::uint_fast8_t softfloat_detectTininess,softfloat_exceptionFlags;
}

namespace th10::browser {

// Derived values only: no game pointers, resource ownership or journal state.
// A hit still performs the original projection's sticky-flag transition.
class BackgroundProjectionCache final {
public:
    static constexpr u32 point_count=8;
    static constexpr std::size_t capacity=256;
    struct Counters {u64 hits=0,misses=0,evictions=0,fallbacks=0;};

    const Counters& counters()const noexcept{return counters_;}

    void project_points(Vec3* output,const Vec3* input,u32 count,
                        const Camera& camera,const Matrix4& world)noexcept{
        if(count!=point_count||partial_overlap(output,input)){
            ++counters_.fallbacks;
            GraphicsMath::project_array(output,12,input,12,count,&camera.viewport,
                                        &camera.projection,&camera.view,&world);
            return;
        }
        // Copy before projection so exact in-place writes cannot change the key.
        const auto key=make_key(input,camera,world);
        auto& entry=entries_[hash_key(key)&(capacity-1)];
        if(entry.valid&&!std::memcmp(entry.key.data(),key.data(),sizeof(key))){
            ++counters_.hits;
            std::memcpy(output,entry.output.data(),sizeof(entry.output));
            softfloat_exceptionFlags=entry.flags_after;
            return;
        }
        ++counters_.misses;
        if(entry.valid)++counters_.evictions;
        GraphicsMath::project_array(output,12,input,12,count,&camera.viewport,
                                    &camera.projection,&camera.view,&world);
        const auto flags_after=softfloat_exceptionFlags;
        std::memcpy(entry.key.data(),key.data(),sizeof(key));
        std::memcpy(entry.output.data(),output,sizeof(entry.output));
        entry.flags_after=flags_after;
        entry.valid=true;
    }

private:
    static_assert(sizeof(Vec3)==12&&sizeof(Matrix4)==64&&sizeof(CameraViewport)==24);
    static_assert(sizeof(std::uint_fast8_t)<=sizeof(u32));
    static_assert((capacity&(capacity-1))==0);
    static constexpr std::size_t input_words=point_count*3,matrix_words=16;
    static constexpr std::size_t world_offset=input_words;
    static constexpr std::size_t view_offset=world_offset+matrix_words;
    static constexpr std::size_t projection_offset=view_offset+matrix_words;
    static constexpr std::size_t viewport_offset=projection_offset+matrix_words;
    static constexpr std::size_t mode_offset=viewport_offset+6;
    // Word storage has no struct padding and preserves signed zeros/NaN payloads.
    using Key=std::array<u32,mode_offset+4>;
    static_assert(sizeof(Key)==(mode_offset+4)*sizeof(u32));
    struct Entry {
        Key key{};
        std::array<Vec3,point_count> output{};
        std::uint_fast8_t flags_after=0;
        bool valid=false;
    };
    std::array<Entry,capacity> entries_{};
    Counters counters_{};

    static bool partial_overlap(const Vec3* output,const Vec3* input)noexcept{
        const auto a=reinterpret_cast<std::uintptr_t>(output);
        const auto b=reinterpret_cast<std::uintptr_t>(input);
        const auto distance=a>b?a-b:b-a;
        // The original reads and writes one point at a time. Partial overlap
        // can therefore alter later inputs; its result is not a value-only key.
        // Compare distances rather than forming possibly overflowing end pointers.
        return distance!=0&&distance<point_count*sizeof(Vec3);
    }

    static Key make_key(const Vec3* input,const Camera& camera,
                        const Matrix4& world)noexcept{
        Key key;
        std::memcpy(key.data(),input,point_count*sizeof(Vec3));
        std::memcpy(key.data()+world_offset,&world,sizeof(world));
        std::memcpy(key.data()+view_offset,&camera.view,sizeof(camera.view));
        std::memcpy(key.data()+projection_offset,&camera.projection,sizeof(camera.projection));
        std::memcpy(key.data()+viewport_offset,&camera.viewport,sizeof(camera.viewport));
        key[mode_offset]=softfloat_roundingMode;
        key[mode_offset+1]=extF80_roundingPrecision;
        key[mode_offset+2]=softfloat_detectTininess;
        key[mode_offset+3]=softfloat_exceptionFlags;
        return key;
    }

    static u32 hash_key(const Key& key)noexcept{
        u32 hash=2166136261u;
        for(const auto word:key){hash^=word;hash*=16777619u;}
        // Fold high float/exponent bits into the slot index. Hashes only choose
        // a slot; every hit is decided by the full exact key comparison above.
        hash^=hash>>16;hash*=0x7feb352du;
        hash^=hash>>15;hash*=0x846ca68bu;
        return hash^(hash>>16);
    }
};

} // namespace th10::browser
