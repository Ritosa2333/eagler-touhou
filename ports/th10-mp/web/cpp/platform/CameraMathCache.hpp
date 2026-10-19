#pragma once
#include "../game/GraphicsMath.hpp"
#include <array>
#include <cerrno>
#include <cstdint>
#include <cstring>

extern "C" {
extern std::uint_fast8_t softfloat_roundingMode,extF80_roundingPrecision;
extern std::uint_fast8_t softfloat_detectTininess,softfloat_exceptionFlags;
}

namespace th10::browser {

// Only the two pure GraphicsMath calls are cached. Camera::configure_* keeps
// every flush, field read, transform submission, right-vector calculation,
// draw-offset copy and viewport operation in its original order.
//
// This service owns derived values, not game/resource pointers or journal
// state. GraphicsRenderer/GraphicsCamera wrappers are short-lived; their
// shared WASM instance uses camera_math_cache() below instead of member caches.
class CameraMathCache final {
public:
    static constexpr u32 capacity=4;
    static constexpr u32 max_key_words=14;
#if defined(__wasm__) && defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY)
    static constexpr bool supported=true;
#else
    static constexpr bool supported=false;
#endif
    enum class Function:u32 {None=0,LookAt=1,Perspective=2};
    struct FunctionCounters {
        u64 calls=0,hits=0,misses=0,evictions=0,bypasses=0;
        u64 verified=0,mismatches=0;
    };
    struct Counters {
        FunctionCounters look_at{},perspective{};
        u64 invalidations=0;
    };
    // Diagnostic copies are populated only by opt-in hit verification.
    // They are not renderer inputs. number==0 means no sample was checked.
    struct Verification {
        u64 number=0;
        Function function=Function::None;
        u32 key_words=0;
        std::array<u32,max_key_words> key{};
        Matrix4 cached{},original{};
        u32 cached_flags=0,original_flags=0;
        i32 cached_errno=0,original_errno=0;
        bool matched=false;
    };

    bool set_enabled(bool value)noexcept{enabled_=supported&&value;return enabled_;}
    bool enabled()const noexcept{return enabled_;}
    // Zero is the normal fast path. N verifies every Nth hit independently
    // for each function, by running the original with the same entry state.
    // Checked hits return the original result and side effects even on failure.
    void set_verify_every(u32 value)noexcept{verify_every_=value;}
    u32 verify_every()const noexcept{return verify_every_;}
    const Counters& counters()const noexcept{return counters_;}
    const Verification& last_verification()const noexcept{return last_verification_;}
    const Verification& first_mismatch()const noexcept{return first_mismatch_;}
    // Invalidation preserves enable/verification settings, per-call counters,
    // arithmetic modes, flags and errno. No lifecycle reset is needed for keys
    // made solely of values, but diagnostics can explicitly request cold data.
    void invalidate()noexcept{
        look_at_.invalidate();perspective_.invalidate();++counters_.invalidations;
    }

    void look_at(Matrix4& output,const Vec3& eye,const Vec3& target,
                 const Vec3& up)noexcept{
        auto& counters=counters_.look_at;++counters.calls;
        // Do not infer a value-only contract for overlapping output/input.
        // Keep the original reads/writes for every such raw-memory shape.
        if(!enabled_||overlap(output,eye)||overlap(output,target)||overlap(output,up)){
            ++counters.bypasses;GraphicsMath::look_at(output,eye,target,up);return;
        }
        Key<14> key;
        std::memcpy(key.data(),&eye,sizeof(eye));
        std::memcpy(key.data()+3,&target,sizeof(target));
        std::memcpy(key.data()+6,&up,sizeof(up));
        execution_state(key,9);
        evaluate(look_at_,key,output,counters,Function::LookAt,
                 [&](){GraphicsMath::look_at(output,eye,target,up);});
    }

    void perspective(Matrix4& output,float fov,float aspect,float near_plane,
                     float far_plane)noexcept{
        auto& counters=counters_.perspective;++counters.calls;
        if(!enabled_){
            ++counters.bypasses;
            GraphicsMath::perspective(output,fov,aspect,near_plane,far_plane);return;
        }
        Key<9> key;
        std::memcpy(key.data(),&fov,sizeof(fov));
        std::memcpy(key.data()+1,&aspect,sizeof(aspect));
        std::memcpy(key.data()+2,&near_plane,sizeof(near_plane));
        std::memcpy(key.data()+3,&far_plane,sizeof(far_plane));
        execution_state(key,4);
        evaluate(perspective_,key,output,counters,Function::Perspective,[&](){
            GraphicsMath::perspective(output,fov,aspect,near_plane,far_plane);
        });
    }

private:
    static_assert(sizeof(Vec3)==12&&sizeof(Matrix4)==64&&sizeof(int)==sizeof(u32));
    static_assert(sizeof(std::uint_fast8_t)<=sizeof(u32));
    template<std::size_t Words>using Key=std::array<u32,Words>;
    template<std::size_t Words>struct Entry {
        Key<Words> key{};
        Matrix4 output{};
        std::uint_fast8_t flags_after=0;
        int errno_after=0;
        bool valid=false;
    };
    template<std::size_t Words>struct Table {
        std::array<Entry<Words>,capacity> entries{};
        u32 next=0;
        void invalidate()noexcept{
            for(auto& entry:entries)entry.valid=false;next=0;
        }
        Entry<Words>* find(const Key<Words>& key)noexcept{
            for(auto& entry:entries)
                if(entry.valid&&!std::memcmp(entry.key.data(),key.data(),sizeof(key)))
                    return &entry;
            return nullptr;
        }
        Entry<Words>& replacement()noexcept{
            auto& result=entries[next];next=(next+1)%capacity;return result;
        }
    };
    Table<14> look_at_{};
    Table<9> perspective_{};
    Counters counters_{};
    Verification last_verification_{},first_mismatch_{};
    u64 verification_serial_=0;
    u32 verify_every_=0;
    bool enabled_=false;

    static bool overlap(const Matrix4& output,const Vec3& input)noexcept{
        const auto a=reinterpret_cast<std::uintptr_t>(&output);
        const auto b=reinterpret_cast<std::uintptr_t>(&input);
        // Subtraction avoids wrapping either interval's end address.
        return a<=b?b-a<sizeof(output):a-b<sizeof(input);
    }
    template<std::size_t Words>
    static void execution_state(Key<Words>& key,std::size_t offset)noexcept{
        key[offset]=softfloat_roundingMode;
        key[offset+1]=extF80_roundingPrecision;
        key[offset+2]=softfloat_detectTininess;
        key[offset+3]=softfloat_exceptionFlags;
        const int error=errno;
        std::memcpy(key.data()+offset+4,&error,sizeof(error));
    }
    template<std::size_t Words>
    void record_verification(const Key<Words>& key,const Entry<Words>& entry,
                             const Matrix4& output,Function function,
                             std::uint_fast8_t flags_after,int errno_after,
                             bool matched)noexcept{
        auto& sample=last_verification_;sample={};
        sample.number=++verification_serial_;sample.function=function;
        sample.key_words=Words;
        std::memcpy(sample.key.data(),key.data(),sizeof(key));
        std::memcpy(&sample.cached,&entry.output,sizeof(sample.cached));
        std::memcpy(&sample.original,&output,sizeof(sample.original));
        sample.cached_flags=entry.flags_after;sample.original_flags=flags_after;
        sample.cached_errno=entry.errno_after;sample.original_errno=errno_after;
        sample.matched=matched;
        if(!matched&&!first_mismatch_.number)first_mismatch_=sample;
    }
    template<std::size_t Words,class Original>
    void evaluate(Table<Words>& table,const Key<Words>& key,Matrix4& output,
                  FunctionCounters& counters,Function function,
                  Original original)noexcept{
        if(auto* entry=table.find(key)){
            ++counters.hits;
            if(verify_every_&&counters.hits%verify_every_==0){
                // Lookup/key construction contain no numeric operations and
                // do not change the original call's entry modes/flags/errno.
                original();
                const auto flags_after=softfloat_exceptionFlags;
                const int errno_after=errno;
                const bool matched=!std::memcmp(&output,&entry->output,sizeof(output))&&
                    flags_after==entry->flags_after&&errno_after==entry->errno_after;
                ++counters.verified;
                record_verification(key,*entry,output,function,flags_after,errno_after,matched);
                if(!matched){++counters.mismatches;entry->valid=false;}
                // Leave the reference outcome intact even if a future change
                // violates the cache contract. The mismatch remains observable.
                softfloat_exceptionFlags=flags_after;errno=errno_after;return;
            }
            std::memcpy(&output,&entry->output,sizeof(output));
            softfloat_exceptionFlags=entry->flags_after;errno=entry->errno_after;return;
        }
        ++counters.misses;
        original();
        const auto flags_after=softfloat_exceptionFlags;
        const int errno_after=errno;
        auto& entry=table.replacement();if(entry.valid)++counters.evictions;
        std::memcpy(entry.key.data(),key.data(),sizeof(key));
        std::memcpy(&entry.output,&output,sizeof(output));
        entry.flags_after=flags_after;entry.errno_after=errno_after;entry.valid=true;
        softfloat_exceptionFlags=flags_after;errno=errno_after;
    }
};

inline CameraMathCache& camera_math_cache()noexcept{
    // One fixed, non-journal service per WASM instance. No heap allocation,
    // owner registration, resource lifetime or transient wrapper participates.
    static CameraMathCache cache;
    return cache;
}

} // namespace th10::browser
