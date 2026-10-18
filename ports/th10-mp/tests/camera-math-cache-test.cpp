#include "../th10_web/cpp/platform/CameraMathCache.hpp"
#include "../th10_web/cpp/game/Arithmetic.hpp"
#include <array>
#include <cerrno>
#include <cstdio>
#include <cstdlib>

using namespace th10;
using Cache=th10::browser::CameraMathCache;
using Function=Cache::Function;

namespace {
struct Inputs {
    Vec3 eye{17.25f,-3.125f,53.75f},target{-2.25f,11.5f,31.125f},up{.1f,1,-.2f};
    std::array<float,4> perspective{0x1.0c1524p-1f,384.f/448.f,30,1800};
};
struct Mode {
    Precision precision=Precision::Single;
    Rounding rounding=Rounding::NearestEven;
    std::uint_fast8_t tininess=1,flags=0;
    int error=ERANGE;
};
struct State {
    unsigned flags=0,precision=0,rounding=0,tininess=0;
    int error=0;
};
struct GuardedMatrix {
    std::array<u32,4> prefix;
    Matrix4 matrix;
    std::array<u32,4> suffix;
};
static_assert(sizeof(GuardedMatrix)==96);
struct Result {GuardedMatrix output;State state;};
enum class Route {Bypass,Miss,Hit};
u32 comparisons=0,flag_transitions=0,errno_transitions=0;

void require(bool condition,const char* label){
    if(condition)return;
    std::fprintf(stderr,"camera math cache: %s (comparison %u)\n",label,comparisons);
    std::exit(1);
}
float value(u32 bits){float result;std::memcpy(&result,&bits,sizeof(result));return result;}
void enter(const Mode& mode){
    arithmetic_mode(mode.precision,mode.rounding);
    softfloat_detectTininess=mode.tininess;softfloat_exceptionFlags=mode.flags;
    errno=mode.error;
}
State observe(){
    return {unsigned(softfloat_exceptionFlags),unsigned(extF80_roundingPrecision),
            unsigned(softfloat_roundingMode),unsigned(softfloat_detectTininess),errno};
}
bool same_state(const State& a,const State& b){
    return a.flags==b.flags&&a.precision==b.precision&&a.rounding==b.rounding&&
           a.tininess==b.tininess&&a.error==b.error;
}
const Cache::FunctionCounters& counters(const Cache& cache,Function function){
    return function==Function::LookAt?cache.counters().look_at:cache.counters().perspective;
}
void invoke(Cache* cache,Function function,Matrix4& output,const Inputs& input){
    if(function==Function::LookAt){
        if(cache)cache->look_at(output,input.eye,input.target,input.up);
        else GraphicsMath::look_at(output,input.eye,input.target,input.up);
    }else{
        const auto& p=input.perspective;
        if(cache)cache->perspective(output,p[0],p[1],p[2],p[3]);
        else GraphicsMath::perspective(output,p[0],p[1],p[2],p[3]);
    }
}
Result evaluate(Cache* cache,Function function,const Inputs& input,const Mode& mode){
    Result result;std::memset(&result.output,0xa5,sizeof(result.output));
    enter(mode);invoke(cache,function,result.output.matrix,input);result.state=observe();
    return result;
}
void same_bytes(const void* expected,const void* actual,std::size_t bytes,
                const State& before,const State& after,const char* label){
    if(!same_state(before,after)||std::memcmp(expected,actual,bytes)){
        std::fprintf(stderr,
            "camera math mismatch: %s comparison=%u flags=%u/%u precision=%u/%u "
            "rounding=%u/%u tiny=%u/%u errno=%d/%d\n",label,comparisons,
            before.flags,after.flags,before.precision,after.precision,
            before.rounding,after.rounding,before.tininess,after.tininess,
            before.error,after.error);
        const auto* a=static_cast<const u8*>(expected);
        const auto* b=static_cast<const u8*>(actual);
        for(std::size_t i=0;i<bytes;++i)if(a[i]!=b[i]){
            std::fprintf(stderr,"first differing output byte=%zu expected=%u actual=%u\n",
                         i,unsigned(a[i]),unsigned(b[i]));break;
        }
        std::exit(1);
    }
    ++comparisons;
}
void same(const Result& expected,const Result& actual,const char* label){
    same_bytes(&expected.output,&actual.output,sizeof(expected.output),
               expected.state,actual.state,label);
}
void route(const Cache::FunctionCounters& before,const Cache::FunctionCounters& after,
           Route expected){
    require(after.calls==before.calls+1,"one call was not counted exactly once");
    require(after.hits==before.hits+(expected==Route::Hit?1u:0u),"hit admission mismatch");
    require(after.misses==before.misses+(expected==Route::Miss?1u:0u),"miss admission mismatch");
    require(after.bypasses==before.bypasses+(expected==Route::Bypass?1u:0u),"bypass admission mismatch");
    require(after.calls==after.hits+after.misses+after.bypasses,"call counters do not partition");
    require(after.evictions<=after.misses&&after.verified<=after.hits&&
            after.mismatches<=after.verified,"invalid cache statistics");
}
Result check(Cache& cache,Function function,const Inputs& input,const Mode& mode,
             Route expected,const char* label){
    const auto before=counters(cache,function);
    const auto reference=evaluate(nullptr,function,input,mode);
    const auto actual=evaluate(&cache,function,input,mode);
    same(reference,actual,label);route(before,counters(cache,function),expected);
    if(reference.state.flags!=mode.flags)++flag_transitions;
    if(reference.state.error!=mode.error)++errno_transitions;
    return actual;
}
Inputs fixture(u32 scenario){
    Inputs input;
    switch(scenario){
    case 1:
        input.eye={value(0x80000000u),0,0};input.target={0,0,0};input.up={0,0,0};
        input.perspective={value(0x80000000u),0,0,0};
        break;
    case 2:
        input.eye={value(1),value(0x80000001u),value(0x00800000u)};
        input.target={value(0x007fffffu),value(0x80800000u),value(2)};
        input.up={value(0x00800001u),value(0x007ffffeu),value(0x80000000u)};
        input.perspective={value(1),value(0x00800000u),value(1),value(0x007fffffu)};
        break;
    case 3:
        input.eye={value(0x7f800000u),value(0xff800000u),value(0x7fc12345u)};
        input.target.x=value(0x7f800001u);
        input.up.y=value(0xffc12345u);
        input.perspective={value(0x7f800000u),value(0x7f800001u),0,1};
        break;
    case 4:
        input.eye={value(0x7f7fffffu),value(0xff7fffffu),1};
        input.target={value(0xff7fffffu),value(0x7f7fffffu),-1};
        input.up={value(0x7f7fffffu),value(0x7f7fffffu),value(0x00800000u)};
        input.perspective={value(0x7fc54321u),value(0x80000000u),
                           value(0xff800000u),value(0x7f800000u)};
        break;
    case 5:
        input.eye={0,0,0};input.target={0,0,-1};input.up={0,1,0};
        input.perspective={3.1415927410125732f,-.75f,30,30};
        break;
    }
    return input;
}
void numeric_modes(){
    Cache cache;require(cache.set_enabled(true),"WASM MP cache did not enable");
    for(auto precision:{Precision::Single,Precision::Double,Precision::Extended})
        for(unsigned rounding=0;rounding<4;++rounding)
            for(unsigned tiny=0;tiny<2;++tiny)
                for(unsigned flags:{0u,1u,16u,31u})
                    for(int error:{0,ERANGE}){
                        const Mode mode{precision,static_cast<Rounding>(rounding),
                            static_cast<std::uint_fast8_t>(tiny),
                            static_cast<std::uint_fast8_t>(flags),error};
                        for(u32 scenario=0;scenario<6;++scenario){
                            const auto input=fixture(scenario);
                            cache.invalidate();
                            for(auto function:{Function::LookAt,Function::Perspective}){
                                check(cache,function,input,mode,Route::Miss,"numeric cold");
                                check(cache,function,input,mode,Route::Hit,"numeric warm");
                            }
                        }
                    }
    require(flag_transitions>0,"numeric cases did not exercise changed exception flags");
    // Some libm configurations preserve errno even for exceptional inputs.
    // The oracle checks the actual original contract in either configuration.
}
void change_input_word(Inputs& input,Function function,u32 word){
    float* location=nullptr;
    if(function==Function::Perspective){
        require(word<4,"invalid perspective word");location=&input.perspective[word];
    }else{
        require(word<9,"invalid look-at word");
        auto* vector=word<3?&input.eye:word<6?&input.target:&input.up;
        const auto component=word%3;
        location=component==0?&vector->x:component==1?&vector->y:&vector->z;
    }
    u32 bits;std::memcpy(&bits,location,sizeof(bits));bits^=1u;
    std::memcpy(location,&bits,sizeof(bits));
}
void exact_inputs_and_state(){
    Cache cache;cache.set_enabled(true);const Inputs base;const Mode mode;
    for(auto function:{Function::LookAt,Function::Perspective}){
        cache.invalidate();check(cache,function,base,mode,Route::Miss,"base input");
        // Address and values belonging to the OTHER function cannot invalidate.
        auto elsewhere=base;
        if(function==Function::LookAt)elsewhere.perspective={11,12,13,14};
        else{elsewhere.eye={11,12,13};elsewhere.target={14,15,16};elsewhere.up={17,18,19};}
        check(cache,function,elsewhere,mode,Route::Hit,"value identity across addresses");
        for(u32 word=0;word<(function==Function::LookAt?9u:4u);++word){
            cache.invalidate();check(cache,function,base,mode,Route::Miss,"key baseline");
            auto changed=base;change_input_word(changed,function,word);
            check(cache,function,changed,mode,Route::Miss,"input bit mutation");
            check(cache,function,changed,mode,Route::Hit,"input bit repeated");
            check(cache,function,base,mode,Route::Hit,"original key retained");
        }
        for(u32 field=0;field<5;++field){
            cache.invalidate();check(cache,function,base,mode,Route::Miss,"mode baseline");
            auto changed=mode;
            switch(field){
            case 0:changed.precision=Precision::Double;break;
            case 1:changed.rounding=Rounding::Down;break;
            case 2:changed.tininess=0;break;
            case 3:changed.flags=1;break;
            case 4:changed.error=EDOM;break;
            }
            check(cache,function,base,changed,Route::Miss,"execution-state mutation");
            check(cache,function,base,changed,Route::Hit,"execution-state repeated");
            check(cache,function,base,mode,Route::Hit,"original execution state retained");
        }
        for(auto bit_pair:{std::array<u32,2>{0,0x80000000u},
                           std::array<u32,2>{0x7fc00001u,0x7fc00002u}}){
            cache.invalidate();auto a=base,b=base;
            if(function==Function::LookAt){a.eye.x=value(bit_pair[0]);b.eye.x=value(bit_pair[1]);}
            else{a.perspective[0]=value(bit_pair[0]);b.perspective[0]=value(bit_pair[1]);}
            check(cache,function,a,mode,Route::Miss,"signed-zero/NaN first bits");
            check(cache,function,b,mode,Route::Miss,"signed-zero/NaN distinct bits");
            check(cache,function,a,mode,Route::Hit,"signed-zero/NaN first repeat");
            check(cache,function,b,mode,Route::Hit,"signed-zero/NaN second repeat");
        }
    }
}
Inputs distinct_input(u32 index){
    Inputs input;input.eye.x=100.f+float(index);
    input.perspective[0]=.2f+float(index)*.025f;return input;
}
void independent_capacity(){
    Cache cache;cache.set_enabled(true);const Mode mode;
    static_assert(Cache::capacity==4);
    // Four distinct values fit for EACH function, irrespective of entry hash
    // or replacement details. A fifth must cause at least one real eviction.
    for(auto function:{Function::LookAt,Function::Perspective})
        for(u32 i=0;i<Cache::capacity;++i)
            check(cache,function,distinct_input(i),mode,Route::Miss,"fill four slots");
    for(auto function:{Function::LookAt,Function::Perspective})
        for(u32 i=0;i<Cache::capacity;++i)
            check(cache,function,distinct_input(i),mode,Route::Hit,"four slots coexist");
    check(cache,Function::LookAt,distinct_input(4),mode,Route::Miss,"fifth look-at");
    require(cache.counters().look_at.evictions>0,"look-at capacity did not evict");
    for(u32 i=0;i<Cache::capacity;++i)
        check(cache,Function::Perspective,distinct_input(i),mode,Route::Hit,"separate perspective table");
    check(cache,Function::Perspective,distinct_input(4),mode,Route::Miss,"fifth perspective");
    require(cache.counters().perspective.evictions>0,"perspective capacity did not evict");
    for(auto function:{Function::LookAt,Function::Perspective}){
        const auto before=counters(cache,function);
        for(u32 i=0;i<=Cache::capacity;++i){
            const auto input=distinct_input(i);
            same(evaluate(nullptr,function,input,mode),evaluate(&cache,function,input,mode),
                 "revisit after bounded eviction");
        }
        require(counters(cache,function).misses>before.misses,"evicted value never missed");
    }
}
void controls_and_invalidation(){
    Cache cache;const Inputs input;const Mode mode;
    require(!cache.enabled()&&cache.verify_every()==0,"cache defaults changed");
    for(auto function:{Function::LookAt,Function::Perspective})
        check(cache,function,input,mode,Route::Bypass,"default-off original path");
    enter(mode);const auto state=observe();
    require(cache.set_enabled(true),"enable failed");cache.set_verify_every(7);cache.invalidate();
    require(same_state(state,observe()),"cache controls changed numeric state");
    require(cache.enabled()&&cache.verify_every()==7,"invalidation changed controls");
    cache.set_verify_every(0);
    for(auto function:{Function::LookAt,Function::Perspective})
        check(cache,function,input,mode,Route::Miss,"enabled cold");
    require(!cache.set_enabled(false),"disable failed");
    for(auto function:{Function::LookAt,Function::Perspective})
        check(cache,function,input,mode,Route::Bypass,"disabled warm original path");
    require(cache.set_enabled(true),"re-enable failed");
    for(auto function:{Function::LookAt,Function::Perspective})
        check(cache,function,input,mode,Route::Hit,"reenable exact retained entry");
    const auto invalidations=cache.counters().invalidations;
    cache.invalidate();require(cache.counters().invalidations==invalidations+1,"invalidation counter");
    for(auto function:{Function::LookAt,Function::Perspective})
        check(cache,function,input,mode,Route::Miss,"explicit invalidation is cold");
}
struct AliasResult {alignas(Matrix4) std::array<u8,160> bytes;State state;};
AliasResult evaluate_overlap(Cache* cache,const Inputs& input,const Mode& mode,
                             u32 which,u32 source_offset){
    AliasResult result;result.bytes.fill(0xa5);
    const Vec3* vectors[]{&input.eye,&input.target,&input.up};
    // Exercise the overlapping byte regions available to the native WASM
    // boundary. All typed addresses are naturally aligned and bounded.
    std::memcpy(result.bytes.data()+source_offset,vectors[which],sizeof(Vec3));
    vectors[which]=reinterpret_cast<const Vec3*>(result.bytes.data()+source_offset);
    auto& output=*reinterpret_cast<Matrix4*>(result.bytes.data()+32);
    enter(mode);
    if(cache)cache->look_at(output,*vectors[0],*vectors[1],*vectors[2]);
    else GraphicsMath::look_at(output,*vectors[0],*vectors[1],*vectors[2]);
    result.state=observe();return result;
}
void output_aliasing(){
    Cache cache;cache.set_enabled(true);const Inputs input;const Mode mode;
    check(cache,Function::LookAt,input,mode,Route::Miss,"warm alias candidate");
    // Output is [32,96). Prefix/suffix adjacency may hit; any overlap with
    // any one of the three input vectors must run the original, warm or cold.
    for(u32 which=0;which<3;++which)
        for(u32 source:{0u,20u,24u,32u,36u,84u,88u,96u,112u}){
            const bool overlaps=source<96&&source+sizeof(Vec3)>32;
            const auto before=cache.counters().look_at;
            const auto expected=evaluate_overlap(nullptr,input,mode,which,source);
            const auto actual=evaluate_overlap(&cache,input,mode,which,source);
            same_bytes(expected.bytes.data(),actual.bytes.data(),expected.bytes.size(),
                       expected.state,actual.state,"output/input overlap");
            route(before,cache.counters().look_at,overlaps?Route::Bypass:Route::Hit);
        }
    Cache cold;cold.set_enabled(true);
    const auto expected=evaluate_overlap(nullptr,input,mode,1,32);
    const auto actual=evaluate_overlap(&cold,input,mode,1,32);
    same_bytes(expected.bytes.data(),actual.bytes.data(),expected.bytes.size(),
               expected.state,actual.state,"cold overlap");
    require(cold.counters().look_at.bypasses==1&&cold.counters().look_at.misses==0,
            "cold overlap populated cache");
    check(cache,Function::LookAt,input,mode,Route::Hit,"overlap preserved warm entries");
}
Result evaluate_shared_inputs(Cache* cache,const Inputs& input,const Mode& mode,bool all){
    Result result;std::memset(&result.output,0xa5,sizeof(result.output));enter(mode);
    if(cache)cache->look_at(result.output.matrix,input.eye,input.eye,all?input.eye:input.up);
    else GraphicsMath::look_at(result.output.matrix,input.eye,input.eye,all?input.eye:input.up);
    result.state=observe();return result;
}
void input_aliasing(){
    Cache cache;cache.set_enabled(true);const Mode mode;
    for(bool all:{false,true}){
        Inputs input;input.target=input.eye;if(all)input.up=input.eye;
        cache.invalidate();check(cache,Function::LookAt,input,mode,Route::Miss,"shared-input values");
        const auto before=cache.counters().look_at;
        same(evaluate_shared_inputs(nullptr,input,mode,all),
             evaluate_shared_inputs(&cache,input,mode,all),"input/input aliases");
        route(before,cache.counters().look_at,Route::Hit);
    }
}
void live_verification(){
    Cache cache;cache.set_enabled(true);cache.set_verify_every(2);const Mode mode;
    for(auto function:{Function::LookAt,Function::Perspective}){
        const auto input=fixture(3);
        check(cache,function,input,mode,Route::Miss,"verification cold");
        const auto serial=cache.last_verification().number;
        check(cache,function,input,mode,Route::Hit,"unsampled hit");
        require(cache.last_verification().number==serial,"verification interval was ignored");
        const auto actual=check(cache,function,input,mode,Route::Hit,"sampled hit");
        const auto& sample=cache.last_verification();
        require(sample.number==serial+1&&sample.function==function&&sample.matched,
                "reference sample identity/result mismatch");
        require(sample.key_words==(function==Function::LookAt?14u:9u),
                "reference sample key size mismatch");
        require(!std::memcmp(&sample.cached,&sample.original,sizeof(Matrix4))&&
                !std::memcmp(&sample.original,&actual.output.matrix,sizeof(Matrix4))&&
                sample.cached_flags==sample.original_flags&&
                sample.original_flags==actual.state.flags&&
                sample.cached_errno==sample.original_errno&&
                sample.original_errno==actual.state.error,"reference sample lost outcome");
        require(counters(cache,function).verified==1&&counters(cache,function).mismatches==0,
                "reference sample counters mismatch");
        const auto verified=counters(cache,function).verified;
        cache.set_verify_every(0);
        check(cache,function,input,mode,Route::Hit,"verification disabled");
        require(counters(cache,function).verified==verified,"disabled verification still ran");
        cache.set_verify_every(2);
    }
    require(cache.first_mismatch().number==0,"unexpected first mismatch record");
}
} // namespace

int main(){
    require(Cache::supported,"compile this test for WASM with TH_ENABLE_MULTIPLAYER_GAMEPLAY");
    numeric_modes();
    exact_inputs_and_state();
    independent_capacity();
    controls_and_invalidation();
    output_aliasing();
    input_aliasing();
    live_verification();
    arithmetic_mode(Precision::Single,Rounding::NearestEven);
    std::printf("camera math cache: PASS %u comparisons; %u flag transitions; "
                "%u errno transitions; 24 arithmetic modes; 4 slots per function\n",
                comparisons,flag_transitions,errno_transitions);
    return 0;
}
