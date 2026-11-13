#pragma once
#include <cstdint>
#include <cstring>
namespace th11 {
// Explicit data for the reference Win32 call stack. These are virtual ABI
// addresses, not pointers into Wasm memory or values derived from game RNG.
struct NativeStackTh14 {
    using Word=std::uint32_t;
    static constexpr Word reference_geometry_frame=0x001b0000u-0x137cu;
    // Standalone hosts get the CRT's normalized fallback input. The Web host
    // supplies an independent process nonce before the first game callback.
    Word cookie=0xbb40e64fu,frame=reference_geometry_frame,scratch=0,damage_scratch=0;
    Word controller_scratch=0;bool controller_scratch_known=true;
    // 425793 -> 42df70 -> 473af0 -> 409bd0 leaves its vector Z at the
    // stack word later read by a constructor's first-frame 45286f call.
    Word spawn_scratch=0,spawn_scratch_mask=0,enemy_spawn_depth=0,ecl_animation_factory_depth=0;
    bool spawn_scratch_known=false;
    struct CounterScope {
        Word* counter;
        explicit CounterScope(Word* value)noexcept:counter(value){if(counter)++*counter;}
        ~CounterScope(){if(counter)--*counter;}
        CounterScope(const CounterScope&)=delete;
        CounterScope& operator=(const CounterScope&)=delete;
    };
    void ecl_factory_interpolation(Word value,Word mask)noexcept{
        spawn_scratch=(spawn_scratch&~mask)|(value&mask);spawn_scratch_mask|=mask;
        spawn_scratch_known=spawn_scratch_mask==~Word(0);
    }
    Word beam_fallback_scratch()const noexcept{return enemy_spawn_depth==1?((spawn_scratch&spawn_scratch_mask)|(scratch&~spawn_scratch_mask)):scratch;}
    static bool preinitialized_cookie(Word value)noexcept{return value!=0xbb40e64eu&&(value&0xffff0000u);}
    void configure(Word opaque_cookie,Word geometry_frame=reference_geometry_frame)noexcept{
        cookie=opaque_cookie;frame=geometry_frame;scratch=0;damage_scratch=0;controller_scratch=0;controller_scratch_known=true;spawn_scratch=spawn_scratch_mask=0;enemy_spawn_depth=ecl_animation_factory_depth=0;spawn_scratch_known=false;
    }
    // Reference collector entry alignment4: its C-28 word is reused by
    // 414020's two-dimensional polar temporary. Calls and math write it.
    void damage_rectangles_call()noexcept{damage_scratch=0x00451548u;}
    void damage_circle_rectangle(float absolute_x)noexcept{std::memcpy(&damage_scratch,&absolute_x,4);}
    void damage_hit_wrapper(float midpoint_y)noexcept{std::memcpy(&damage_scratch,&midpoint_y,4);}
    float damage_polar_z()const noexcept{float z;std::memcpy(&z,&damage_scratch,4);return z;}
    void damage_polar_store(float z)noexcept{std::memcpy(&damage_scratch,&z,4);}
    // Reference overlay47e7a0 leaves ESI as the null list cursor. A normal
    // shake46cecf instead leaves the script-RNG this pointer through the
    // pinned RtlLeaveCriticalSection prologue;4138ca reuses that saved word.
    void controller_overlay_tail()noexcept{controller_scratch=0;controller_scratch_known=true;}
    void controller_shake_rng_leave()noexcept{controller_scratch=0x004db508u;controller_scratch_known=true;}
    // Phased46cf60 adds a saved EDI: the reused word is the pinned
    // RtlLeaveCriticalSection saved EBX, the last32-bit draw's low half.
    void controller_phased_rng_leave(Word sample)noexcept{controller_scratch=sample&0xffffu;controller_scratch_known=true;}
    // The retiring callback node's opaque native allocation address remains
    // unresolved. Preserve the old numeric fallback and expose that gap.
    void controller_opaque_leave()noexcept{controller_scratch=0;controller_scratch_known=false;}
    float controller_polar_z()const noexcept{float z;std::memcpy(&z,&controller_scratch,4);return z;}
    void beam_effect_published(Word handle)noexcept{scratch=handle;}
    void rectangle_intersection_y(float y)noexcept{std::memcpy(&scratch,&y,4);}
    // The pinned reference dsound.dll pushes STRSAFE_MAX_CCH at+25bab.
    void reference_pan_call()noexcept{scratch=0x7ffffffeu;}
    void geometry_mode1_call()noexcept{scratch=cookie^frame;}
    static Word fallback_bits(Word bits)noexcept{
        // 45286f..4528a6: addss8; comiss0; store; jbe; optional zero.
        // Explicitly retain the quieted NaN payload across Wasm engines.
        if((bits&0x7f800000u)==0x7f800000u&&(bits&0x007fffffu))return bits|0x00400000u;
        float value;std::memcpy(&value,&bits,4);value=value+8.f;
        if(0.f>value)value=0.f;
        std::memcpy(&bits,&value,4);return bits;
    }
};
}
