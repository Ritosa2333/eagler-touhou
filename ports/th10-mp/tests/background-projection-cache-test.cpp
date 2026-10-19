#include "../web/cpp/platform/BackgroundProjectionCache.hpp"
#include "../web/cpp/game/Arithmetic.hpp"
#include <array>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <memory>

using namespace th10;
using Cache=th10::browser::BackgroundProjectionCache;

namespace {
struct Fixture {
    std::array<Vec3,16> input{};
    Camera camera{};
    Matrix4 world{};
    u32 count=8;
};
struct Mode {
    Precision precision=Precision::Single;
    Rounding rounding=Rounding::NearestEven;
    std::uint_fast8_t tininess=1,flags=0;
    int error=ERANGE;
};
struct Result {
    // Prefix, suffix and unused points must remain untouched, including count 0.
    std::array<Vec3,18> output;
    unsigned flags,precision,rounding,tininess;
    int error;
};
u32 comparisons=0,flag_transitions=0;

void require(bool condition,const char* message){
    if(condition)return;
    std::fprintf(stderr,"background projection cache: %s\n",message);
    std::exit(1);
}
float value(u32 bits){
    float result;std::memcpy(&result,&bits,sizeof(result));return result;
}
Fixture ordinary(){
    Fixture result;
    result.world.identity();
    result.camera.view.identity();
    result.camera.projection.identity();
    result.camera.viewport={32,16,384,448,0,1};
    result.world.elements[3][0]=.25f;
    result.world.elements[3][1]=-.5f;
    result.world.elements[3][2]=.125f;
    result.camera.view.elements[3][2]=.0625f;
    result.camera.projection.elements[2][3]=.125f;
    for(u32 i=0;i<result.input.size();++i){
        result.input[i]={float(i32(i%4)*2-3)*.25f,
                         float(i32(i/4)*2-3)*.25f,float(i%5)*.125f};
    }
    return result;
}
Fixture fixture(u32 scenario){
    auto result=ordinary();
    switch(scenario){
    case 1:
        // Integer viewport conversions and depth subtraction raise real sticky
        // flags. A hit must reproduce them after the caller resets its flags.
        result.camera.viewport={0x1000001u,0xffffffffu,0x1000001u,0x80000001u,
                                value(1),1};
        break;
    case 2:
        result.input[0]={value(0x80000000u),value(1),value(0x80000001u)};
        result.input[1]={value(0x007fffffu),value(0x00800000u),value(0x80800000u)};
        result.world.elements[0][0]=value(0x00800000u);
        result.camera.viewport.near_depth=value(1);
        result.camera.viewport.far_depth=value(0x00800000u);
        break;
    case 3:
        result.input[0].x=value(0x7f800000u);
        result.input[1].y=value(0xff800000u);
        result.input[2].z=value(0x7fc12345u);
        result.input[3].x=value(0x7f800001u);
        result.camera.viewport.near_depth=value(0x7f800001u);
        break;
    case 4:
        result.world.elements[3][3]=0;
        result.camera.projection.elements[2][3]=0;
        result.camera.viewport.near_depth=-.25f;
        break;
    case 5:
        result.world.elements[0][0]=value(0x7f7fffffu);
        result.world.elements[1][1]=value(0xff7fffffu);
        result.camera.viewport.near_depth=value(0xff800000u);
        result.camera.viewport.far_depth=value(0x7f800000u);
        break;
    }
    return result;
}
Result evaluate(Cache* cache,const Fixture& input,const Mode& mode){
    arithmetic_mode(mode.precision,mode.rounding);
    softfloat_detectTininess=mode.tininess;
    softfloat_exceptionFlags=mode.flags;
    errno=mode.error;
    Result result;
    std::memset(result.output.data(),0xa5,sizeof(result.output));
    if(cache){
        cache->project_points(result.output.data()+1,input.input.data(),input.count,
                              input.camera,input.world);
    }else{
        GraphicsMath::project_array(result.output.data()+1,12,input.input.data(),12,
            input.count,&input.camera.viewport,&input.camera.projection,
            &input.camera.view,&input.world);
    }
    result.flags=softfloat_exceptionFlags;
    result.precision=extF80_roundingPrecision;
    result.rounding=softfloat_roundingMode;
    result.tininess=softfloat_detectTininess;
    result.error=errno;
    return result;
}
void same(const Result& expected,const Result& actual,const char* label){
    if(expected.flags!=actual.flags||expected.error!=actual.error||
       expected.precision!=actual.precision||expected.rounding!=actual.rounding||
       expected.tininess!=actual.tininess||
       std::memcmp(expected.output.data(),actual.output.data(),sizeof(expected.output))){
        std::fprintf(stderr,
            "background projection mismatch: check=%u %s flags=%u/%u "
            "precision=%u/%u rounding=%u/%u tiny=%u/%u errno=%d/%d\n",
            comparisons,label,expected.flags,actual.flags,expected.precision,
            actual.precision,expected.rounding,actual.rounding,expected.tininess,
            actual.tininess,expected.error,actual.error);
        const auto* a=reinterpret_cast<const u8*>(expected.output.data());
        const auto* b=reinterpret_cast<const u8*>(actual.output.data());
        for(std::size_t i=0;i<sizeof(expected.output);++i)if(a[i]!=b[i]){
            std::fprintf(stderr,"first differing output byte=%zu expected=%u actual=%u\n",
                         i,unsigned(a[i]),unsigned(b[i]));break;
        }
        std::exit(1);
    }
    ++comparisons;
}
void check(Cache& cache,const Fixture& input,const Mode& mode,bool hit){
    const auto before=cache.counters();
    const auto expected=evaluate(nullptr,input,mode);
    const auto actual=evaluate(&cache,input,mode);
    same(expected,actual,hit?"hit":"miss");
    require(cache.counters().hits==before.hits+(hit?1u:0u),"unexpected hit count");
    require(cache.counters().misses==before.misses+(hit?0u:1u),"unexpected miss count");
    if(expected.flags!=mode.flags)++flag_transitions;
}
void numeric_modes(){
    auto cache=std::make_unique<Cache>();
    for(auto precision:{Precision::Single,Precision::Double,Precision::Extended}){
        for(unsigned rounding=0;rounding<4;++rounding){
            for(unsigned tiny=0;tiny<2;++tiny){
                for(unsigned flags:{0u,1u,16u,31u}){
                    Mode mode{precision,static_cast<Rounding>(rounding),
                              static_cast<std::uint_fast8_t>(tiny),
                              static_cast<std::uint_fast8_t>(flags),ERANGE};
                    for(u32 scenario=0;scenario<6;++scenario){
                        const auto input=fixture(scenario);
                        check(*cache,input,mode,false);
                        // Re-enter with the same pre-flags, but different errno:
                        // the cache restores post-flags and preserves this errno.
                        mode.error=EDOM;
                        check(*cache,input,mode,true);
                        mode.error=ERANGE;
                    }
                }
            }
        }
    }
    require(flag_transitions>0,"numeric cases did not exercise post-flag restoration");
}
void change_word(Fixture& input,std::size_t index){
    u8* bytes=nullptr;
    if(index<24)bytes=reinterpret_cast<u8*>(input.input.data());
    else if((index-=24)<16)bytes=reinterpret_cast<u8*>(&input.world);
    else if((index-=16)<16)bytes=reinterpret_cast<u8*>(&input.camera.view);
    else if((index-=16)<16)bytes=reinterpret_cast<u8*>(&input.camera.projection);
    else{index-=16;require(index<6,"invalid key mutation");
         bytes=reinterpret_cast<u8*>(&input.camera.viewport);}
    u32 bits;std::memcpy(&bits,bytes+index*sizeof(bits),sizeof(bits));bits^=1u;
    std::memcpy(bytes+index*sizeof(bits),&bits,sizeof(bits));
}
void exact_keys(){
    auto cache=std::make_unique<Cache>();
    const auto base=ordinary();Mode mode;
    check(*cache,base,mode,false);
    // Values, not stack addresses or unrelated camera state, own this result.
    auto elsewhere=base;
    elsewhere.camera.position={111,222,333};
    elsewhere.camera.fog.near_distance=765;
    check(*cache,elsewhere,mode,true);
    for(std::size_t word=0;word<78;++word){
        auto changed=base;change_word(changed,word);
        check(*cache,changed,mode,false);
        check(*cache,changed,mode,true);
    }
}
void collisions_and_eviction(){
    auto cache=std::make_unique<Cache>();Mode mode;
    // Pigeonhole guarantee, independent of the cache's hash implementation.
    // Every first visit is distinct and must remain a miss even on collision.
    for(std::size_t i=0;i<=Cache::capacity;++i){
        auto input=ordinary();input.world.elements[3][0]=float(i)+.125f;
        check(*cache,input,mode,false);
    }
    require(cache->counters().evictions>0,"bounded cache did not evict a collision");
    const auto before=cache->counters();
    for(std::size_t i=0;i<=Cache::capacity;++i){
        auto input=ordinary();input.world.elements[3][0]=float(i)+.125f;
        const auto expected=evaluate(nullptr,input,mode);
        const auto actual=evaluate(cache.get(),input,mode);
        same(expected,actual,"revisit after collision");
    }
    require(cache->counters().misses>before.misses,"evicted input did not miss");
}
void fallback(){
    auto cache=std::make_unique<Cache>();Mode mode;
    auto input=fixture(1);
    check(*cache,input,mode,false);
    for(u32 count:{0u,1u,7u,9u,11u,16u}){
        input.count=count;
        for(unsigned repeat=0;repeat<2;++repeat){
            const auto before=cache->counters();
            const auto expected=evaluate(nullptr,input,mode);
            const auto actual=evaluate(cache.get(),input,mode);
            same(expected,actual,"non-eight-point fallback");
            require(cache->counters().hits==before.hits&&
                    cache->counters().misses==before.misses&&
                    cache->counters().evictions==before.evictions&&
                    cache->counters().fallbacks==before.fallbacks+1,
                    "fallback touched cache entries or counters");
        }
    }
    input.count=8;
    check(*cache,input,mode,true);
}
Result evaluate_alias(Cache* cache,const Fixture& input,const Mode& mode,i32 offset){
    arithmetic_mode(mode.precision,mode.rounding);
    softfloat_detectTininess=mode.tininess;
    softfloat_exceptionFlags=mode.flags;
    errno=mode.error;
    Result result;
    std::memset(result.output.data(),0xa5,sizeof(result.output));
    auto* source=result.output.data()+(offset<0?9:1);
    auto* destination=source+offset;
    std::memcpy(source,input.input.data(),8*sizeof(Vec3));
    if(cache){
        cache->project_points(destination,source,8,input.camera,input.world);
    }else{
        GraphicsMath::project_array(destination,12,source,12,8,&input.camera.viewport,
            &input.camera.projection,&input.camera.view,&input.world);
    }
    result.flags=softfloat_exceptionFlags;
    result.precision=extF80_roundingPrecision;
    result.rounding=softfloat_roundingMode;
    result.tininess=softfloat_detectTininess;
    result.error=errno;
    return result;
}
void aliasing(){
    auto cache=std::make_unique<Cache>();const auto input=fixture(1);Mode mode;
    check(*cache,input,mode,false);
    // Identical initial values: partial overlap must bypass the warm entry;
    // exact in-place and adjacent disjoint buffers can reuse it.
    for(i32 offset:{-8,-1,0,1,8}){
        const bool partial=offset==-1||offset==1;
        const auto before=cache->counters();
        const auto expected=evaluate_alias(nullptr,input,mode,offset);
        const auto actual=evaluate_alias(cache.get(),input,mode,offset);
        same(expected,actual,"buffer aliasing");
        require(cache->counters().hits==before.hits+(partial?0u:1u)&&
                cache->counters().misses==before.misses&&
                cache->counters().evictions==before.evictions&&
                cache->counters().fallbacks==before.fallbacks+(partial?1u:0u),
                "partial overlap/in-place admission mismatch");
    }
    check(*cache,input,mode,true);
    // A cold in-place calculation must retain the original input key too.
    auto inplace=std::make_unique<Cache>();
    const auto expected=evaluate_alias(nullptr,input,mode,0);
    const auto actual=evaluate_alias(inplace.get(),input,mode,0);
    same(expected,actual,"cold in-place");
    require(inplace->counters().misses==1&&inplace->counters().hits==0,
            "cold in-place did not populate cache");
    check(*inplace,input,mode,true);
}
} // namespace

int main(){
    numeric_modes();
    exact_keys();
    collisions_and_eviction();
    fallback();
    aliasing();
    std::printf("{\"suite\":\"background-projection-cache\",\"capacity\":%zu,"
                "\"differential_checks\":%u,\"post_flag_transitions_checked\":%u,"
                "\"vertices_bit_equal\":true,\"sticky_flags_modes_errno_equal\":true,"
                "\"all_graphic_key_words_checked\":78,\"collisions_and_eviction\":true,"
                "\"non_eight_point_fallback\":true,\"aliasing_checked\":true}\n",
                Cache::capacity,comparisons,flag_transitions);
}
