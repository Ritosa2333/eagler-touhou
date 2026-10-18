#include "../th10_web/cpp/game/AnmRotation.hpp"
#include <array>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <vector>

using namespace th10;
extern "C" {
extern std::uint_fast8_t softfloat_exceptionFlags,softfloat_detectTininess;
}
namespace {
struct Result {u32 cosine,sine;std::uint_fast8_t flags;int error;};
u32 bits(float value){u32 result;std::memcpy(&result,&value,sizeof(result));return result;}
float value(u32 bits){float result;std::memcpy(&result,&bits,sizeof(result));return result;}
u32 random_bits(u32& state){state^=state<<13;state^=state>>17;state^=state<<5;return state;}
[[noreturn]] void fail(u32 input,unsigned precision,unsigned rounding,unsigned tininess,unsigned seed,const Result& expected,const Result& actual){
    std::fprintf(stderr,"ANM trig mismatch: input=%08x precision=%u rounding=%u tininess=%u seed=%u expected=(%08x,%08x,%u,%d) actual=(%08x,%08x,%u,%d)\n",input,precision,rounding,tininess,seed,expected.cosine,expected.sine,unsigned(expected.flags),expected.error,actual.cosine,actual.sine,unsigned(actual.flags),actual.error);
    std::abort();
}
Result evaluate(float radians,unsigned seed,int error,bool optimized){
    softfloat_exceptionFlags=seed;errno=error;float c,s;
    if(optimized)anm_rotation_values(radians,c,s);
    else{
        // Independent original-expression oracle, including its store points.
        c=cosine(number(radians)).to_float();s=sine(number(radians)).to_float();
    }
    return {bits(c),bits(s),softfloat_exceptionFlags,errno};
}
void check(u32 input,Precision precision,Rounding rounding,unsigned tininess,unsigned seed,int error){
    arithmetic_mode(precision,rounding);softfloat_detectTininess=tininess;
    const auto expected=evaluate(value(input),seed,error,false);
    const auto actual=evaluate(value(input),seed,error,true);
    if(expected.cosine!=actual.cosine||expected.sine!=actual.sine||expected.flags!=actual.flags||expected.error!=actual.error)
        fail(input,unsigned(precision),unsigned(rounding),tininess,seed,expected,actual);
}
}
int main(){
    // Signed zero, subnormal/normal boundaries, familiar rotations, extreme
    // finite values, infinities, and quiet/signaling NaNs with varied payloads.
    const std::array<u32,34> special={0,0x80000000,1,0x80000001,0x007fffff,0x807fffff,0x00800000,0x80800000,0x00800001,0x3e000000,0xbe000000,0x3f000000,0xbf000000,0x3f800000,0xbf800000,0x3fc90fdb,0xbfc90fdb,0x40490fdb,0xc0490fdb,0x40c90fdb,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc00000,0x7fc12345,0xffc12345,0x7f800001,0xff800001,0x33800000,0xb3800000,0x4b800000,0xcb800000,0x3f7fffff};
    std::vector<u32> inputs(special.begin(),special.end());u32 state=123456789;
    for(unsigned i=0;i<10000;++i)inputs.push_back(random_bits(state));
    for(unsigned i=0;i<2048;++i)inputs.push_back(bits(float(i+1)*.00390625f));
    constexpr std::array<Precision,3> precisions={Precision::Single,Precision::Double,Precision::Extended};
    unsigned checks=0;
    for(auto precision:precisions)for(unsigned rounding=0;rounding<4;++rounding)for(unsigned tininess=0;tininess<2;++tininess)
        for(unsigned seed:std::array<unsigned,4>{0,1,16,31})for(unsigned i=0;i<inputs.size();++i)for(unsigned repeat=0;repeat<2;++repeat){
            check(inputs[i],precision,static_cast<Rounding>(rounding),tininess,seed,repeat?EDOM:0);++checks;
        }
    // Revisit the same input while changing every arithmetic setting and
    // initial sticky flag pattern. No cached result or mode may leak across.
    for(unsigned i=0;i<4096;++i){
        check(bits(.125f),precisions[i%3],static_cast<Rounding>((i/3)%4),(i/12)%2,i%32,i%2?ERANGE:0);++checks;
    }
#if defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY) && defined(__wasm__)
    constexpr bool fast_input_enabled=true;
#else
    constexpr bool fast_input_enabled=false;
#endif
    std::printf("{\"suite\":\"anm-trig-input\",\"fast_input_enabled\":%s,\"scalar_checks\":%u,\"inputs\":%zu,\"precisions\":3,\"roundings\":4,\"tininess_modes\":2,\"scalar_bit_equal\":true,\"sticky_flags_equal\":true,\"errno_equal\":true}\n",fast_input_enabled?"true":"false",checks,inputs.size());
}
