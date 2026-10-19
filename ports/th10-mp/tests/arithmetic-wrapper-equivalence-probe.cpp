// Isolated Emscripten oracle. Link this object unchanged into two modules,
// each with exactly one Arithmetic.cpp object and the same production SoftFloat.o.
// No test shim, normalized production hash, or alternative arithmetic backend.
#include "../web/cpp/game/Arithmetic.hpp"
#include <emscripten/emscripten.h>
#include <array>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

#if !defined(__EMSCRIPTEN__) || !defined(__wasm32__)
#error "Use the real Emscripten wasm32 compiler for this independent oracle."
#endif

extern "C" {
extern std::uint_fast8_t softfloat_roundingMode, extF80_roundingPrecision;
extern std::uint_fast8_t softfloat_exceptionFlags, softfloat_detectTininess;
}

namespace {
using th10::Extended;
using th10::u8;
using th10::u16;
using th10::u32;
using th10::u64;
static_assert(std::is_standard_layout<Extended>::value);
static_assert(std::is_trivially_copyable<Extended>::value);
static_assert(sizeof(Extended)==16 && alignof(Extended)==8);
static_assert(offsetof(Extended,significand)==0 && offsetof(Extended,exponent)==8);
static_assert(offsetof(Extended,reserved16)==10 && offsetof(Extended,reserved32)==12);
static_assert(sizeof(std::uint_fast8_t)==sizeof(u8));
static_assert(sizeof(std::uintptr_t)==4);

constexpr u32 mode_count=24, pair_count=48, random_pair_count=256;
constexpr u32 directed_cases=pair_count*4*mode_count*8*3;
constexpr u32 random_cases=random_pair_count*4*mode_count;
constexpr u32 conversion_cases=96*mode_count;
constexpr u32 chain_cases=12*mode_count*4*3;
constexpr u32 total_cases=directed_cases+random_cases+conversion_cases+chain_cases;
constexpr u32 total_records=directed_cases+random_cases+conversion_cases*3+chain_cases*8;
constexpr u32 batch_capacity=256, records_capacity=batch_capacity*8;
static_assert(total_cases==140928 && total_records==169728);

// Every record is 16 u32 words, without C++ structure padding:
// case, step, event, operator, activeMode, initialFlags, initialErrno,
// exitFlags, exitPrecision, exitRounding, exitTininess, exitErrno,
// followed by the complete 16-byte result (scalar traces explicitly zero-fill).
// Events: 0 Extended binary; 1 from_f32; 2 from_f64; 3 to_f32; 4 to_f64.
struct Record {u32 words[16];};
static_assert(sizeof(Record)==64);
struct GuardedRecords {
    u32 prefix[4];
    Record values[records_capacity];
    u32 suffix[4];
};
GuardedRecords trace;
u32 status[12]{};
u32 info[24]{};
u32 description[32]{};
u32 layout[20]{};
volatile u32 stack_witness=0;

constexpr u32 f32_inputs[]={
    0x00000000u,0x80000000u,0x00000001u,0x80000001u,
    0x00000002u,0x80000002u,0x007fffffu,0x807fffffu,
    0x00800000u,0x80800000u,0x00800001u,0x80800001u,
    0x3f800000u,0xbf800000u,0x3f000000u,0xbf000000u,
    0x40000000u,0xc0000000u,0x40400000u,0xc0400000u,
    0x3f800001u,0xbf800001u,0x3f7fffffu,0xbf7fffffu,
    0x7f7fffffu,0xff7fffffu,0x7f800000u,0xff800000u,
    0x7fc00000u,0x7fc00001u,0x7fffffffu,0xffc12345u,
    0x7f800001u,0x7fa00001u,0xff800001u,0xffa54321u,
    0x33800000u,0x337fffffu,0x33800001u,0xb3800000u,
    0x00800002u,0x007ffffeu,0x4b000000u,0x4b000001u,
    0x4affffffu,0xcb000001u,0x3eaaaaabu,0xbeaaaaabu
};
constexpr u64 f64_inputs[]={
    0x0000000000000000ull,0x8000000000000000ull,
    0x0000000000000001ull,0x8000000000000001ull,
    0x0000000000000002ull,0x8000000000000002ull,
    0x000fffffffffffffull,0x800fffffffffffffull,
    0x0010000000000000ull,0x8010000000000000ull,
    0x0010000000000001ull,0x8010000000000001ull,
    0x3ff0000000000000ull,0xbff0000000000000ull,
    0x3fe0000000000000ull,0xbfe0000000000000ull,
    0x4000000000000000ull,0xc000000000000000ull,
    0x4008000000000000ull,0xc008000000000000ull,
    0x3ff0000000000001ull,0xbff0000000000001ull,
    0x3fefffffffffffffull,0xbfefffffffffffffull,
    0x7fefffffffffffffull,0xffefffffffffffffull,
    0x7ff0000000000000ull,0xfff0000000000000ull,
    0x7ff8000000000000ull,0x7ff8000000000001ull,
    0x7fffffffffffffffull,0xfff8123456789abcull,
    0x7ff0000000000001ull,0x7ff4000000000001ull,
    0xfff0000000000001ull,0xfff4123456789abcull,
    0x3ca0000000000000ull,0x3c9fffffffffffffull,
    0x3ca0000000000001ull,0xbca0000000000000ull,
    0x0010000000000002ull,0x000ffffffffffffeull,
    0x4340000000000000ull,0x4340000000000001ull,
    0x433fffffffffffffull,0xc340000000000001ull,
    0x3fd5555555555555ull,0xbfd5555555555555ull
};
static_assert(sizeof(f32_inputs)/sizeof(f32_inputs[0])==48);
static_assert(sizeof(f64_inputs)/sizeof(f64_inputs[0])==48);
constexpr u32 flag_seeds[]={0,1,2,4,8,16,21,31};
constexpr u32 chain_flags[]={0,1,16,31};
constexpr int error_seeds[]={0,EDOM,ERANGE};

Extended raw(u64 significand,u16 exponent) {
    Extended value;
    value.significand=significand;value.exponent=exponent;
    return value;
}
Extended power(int exponent,bool negative=false) {
    return raw(0x8000000000000000ull,
               static_cast<u16>((negative?0x8000u:0u)+16383+exponent));
}
Extended negate_bits(Extended value) {value.exponent^=0x8000u;return value;}

// Integer-only fixture construction. It neither calls a tested wrapper nor
// modifies the numerical environment. Noncanonical extended encodings are
// deliberately excluded from the hard equivalence corpus.
Extended f32_encoding(u32 bits) {
    const u32 exponent=(bits>>23)&255u, fraction=bits&0x7fffffu;
    const u16 sign=static_cast<u16>((bits>>16)&0x8000u);
    if(!exponent) {
        if(!fraction)return raw(0,sign);
        const u32 top=31u-static_cast<u32>(__builtin_clz(fraction));
        return raw(u64(fraction)<<(63-top),static_cast<u16>(sign+16383-149+top));
    }
    if(exponent==255)return raw(0x8000000000000000ull|(u64(fraction)<<40),sign|0x7fffu);
    return raw(u64(fraction|0x800000u)<<40,static_cast<u16>(sign+exponent+16256));
}
Extended f64_encoding(u64 bits) {
    const u32 exponent=static_cast<u32>((bits>>52)&2047u);
    const u64 fraction=bits&0xfffffffffffffull;
    const u16 sign=static_cast<u16>((bits>>48)&0x8000u);
    if(!exponent) {
        if(!fraction)return raw(0,sign);
        const u32 top=63u-static_cast<u32>(__builtin_clzll(fraction));
        return raw(fraction<<(63-top),static_cast<u16>(sign+16383-1074+top));
    }
    if(exponent==2047)return raw(0x8000000000000000ull|(fraction<<11),sign|0x7fffu);
    return raw(0x8000000000000000ull|(fraction<<11),static_cast<u16>(sign+exponent+15360));
}
struct Pair {Extended a,b;};
Pair directed_pair(u32 index) {
    const auto zero=raw(0,0),minus_zero=raw(0,0x8000),one=power(0),two=power(1);
    const auto three=raw(0xc000000000000000ull,0x4000);
    const auto min_ext=raw(1,0),normal_ext=raw(0x8000000000000000ull,1);
    const auto sub_ext=raw(0x7fffffffffffffffull,0),max_ext=raw(~u64(0),0x7ffe);
    const auto inf=raw(0x8000000000000000ull,0x7fff);
    const auto qnan=raw(0xc000000000000001ull,0x7fff);
    switch(index) {
    case 0:return {zero,minus_zero};
    case 1:return {minus_zero,zero};
    case 2:return {minus_zero,minus_zero};
    case 3:return {one,zero};
    case 4:return {negate_bits(one),minus_zero};
    case 5:return {one,one};
    case 6:return {negate_bits(one),one};
    case 7:return {one,negate_bits(one)};
    case 8:return {three,two};
    case 9:return {one,three};
    case 10:return {f32_encoding(0x7f7fffff),two};
    case 11:return {f32_encoding(0x00800000),power(-1)};
    case 12:return {f32_encoding(0x007fffff),f32_encoding(1)};
    case 13:return {f32_encoding(0x00800000),f32_encoding(0x80000001)};
    case 14:return {f32_encoding(1),f32_encoding(1)};
    case 15:return {f64_encoding(0x7fefffffffffffffull),two};
    case 16:return {f64_encoding(0x0010000000000000ull),power(-1)};
    case 17:return {f64_encoding(0x000fffffffffffffull),f64_encoding(1)};
    case 18:return {f64_encoding(1),three};
    case 19:return {max_ext,two};
    case 20:return {normal_ext,power(-1)};
    case 21:return {sub_ext,min_ext};
    case 22:return {min_ext,three};
    case 23:return {inf,negate_bits(inf)};
    case 24:return {inf,zero};
    case 25:return {negate_bits(inf),minus_zero};
    case 26:return {inf,two};
    case 27:return {qnan,one};
    case 28:return {raw(0xc123456789abcdefull,0x7fff),qnan};
    case 29:return {raw(0x8000000000000001ull,0x7fff),two};
    case 30:return {raw(0xa123456789abcdefull,0xffff),qnan};
    case 31:return {raw(0xc000000000000002ull,0xffff),raw(0xe000000000000000ull,0x7fff)};
    case 32:return {one,power(-24)};
    case 33:return {one,raw(~u64(0),16383-25)};
    case 34:return {one,raw(0x8000000000000001ull,16383-24)};
    case 35:return {negate_bits(one),power(-24,true)};
    case 36:return {one,power(-53)};
    case 37:return {one,raw(~u64(0),16383-54)};
    case 38:return {one,raw(0x8000000000000001ull,16383-53)};
    case 39:return {negate_bits(one),power(-53,true)};
    case 40:return {one,power(-64)};
    case 41:return {one,raw(~u64(0),16383-65)};
    case 42:return {one,raw(0x8000000000000001ull,16383-64)};
    case 43:return {negate_bits(one),power(-64,true)};
    case 44:return {f32_encoding(0x3f800001),one};
    case 45:return {power(10000),power(-10000)};
    case 46:return {negate_bits(sub_ext),normal_ext};
    default:return {f32_encoding(0x7f7fffff),f32_encoding(0xff7fffff)};
    }
}
u32 next(u32& state) {state^=state<<13;state^=state>>17;state^=state<<5;return state;}
Extended random_value(u32& state,u32 kind) {
    const u32 high=next(state),low=next(state);
    const u64 significand=(u64(high)<<32)|low;
    const u16 sign=static_cast<u16>(next(state)&0x8000u);
    if(kind<2) {
        const u32 fraction=next(state)&0x807fffffu;
        return f32_encoding(fraction|((1u+next(state)%254u)<<23));
    }
    if(kind<4)return raw(significand|0x8000000000000000ull,
                        static_cast<u16>(sign+1u+next(state)%32766u));
    if(kind==4)return raw(significand&0x7fffffffffffffffull,sign);
    return raw(significand|0x8000000000000000ull,sign|0x7fffu);
}
void reserves(Extended& value,u32 seed) {
    value.reserved16=seed%3==0?0:seed%3==1?0xa5a5u:0x5a5au;
    value.reserved32=seed%3==0?0:seed%3==1?0xa5a5a5a5u:0x5a5a5a5au;
}
struct Case {
    u32 group=0,scenario=0,operation=0,mode=0,flags=0;
    int error=0;
    Extended a,b,c;
    u64 bits=0;
};
Case make_case(u32 id) {
    Case value;u32 index=id;
    if(index<directed_cases) {
        value.scenario=index%48;index/=48;
        value.operation=index%4;index/=4;value.mode=index%24;index/=24;
        value.flags=flag_seeds[index%8];index/=8;value.error=error_seeds[index%3];
        const auto pair=directed_pair(value.scenario);value.a=pair.a;value.b=pair.b;
    } else if((index-=directed_cases)<random_cases) {
        value.group=1;value.scenario=index%256;index/=256;
        value.operation=index%4;value.mode=index/4;
        value.flags=value.scenario%32;value.error=error_seeds[(value.scenario/32)%3];
        u32 state=0xa341316cu^(value.scenario*0x9e3779b9u);
        value.a=random_value(state,value.scenario%6);
        value.b=random_value(state,(value.scenario+3)%6);
    } else if((index-=random_cases)<conversion_cases) {
        value.group=2;value.scenario=index%96;value.mode=index/96;
        value.flags=value.scenario%32;value.error=error_seeds[(value.scenario/32)%3];
        value.bits=value.scenario<48?u64(f32_inputs[value.scenario]):f64_inputs[value.scenario-48];
        value.a=value.scenario<48?f32_encoding(static_cast<u32>(value.bits)):f64_encoding(value.bits);
    } else {
        index-=conversion_cases;value.group=3;
        value.scenario=index%12;index/=12;value.mode=index%24;index/=24;
        value.flags=chain_flags[index%4];index/=4;value.error=error_seeds[index%3];
        constexpr u32 selected[]={8,44,20,19,29,3,5,36,12,38,9,2};
        const auto pair=directed_pair(selected[value.scenario]);value.a=pair.a;value.b=pair.b;
        value.c=power(value.scenario%3==0?-1:1);
        if(value.scenario==0) {
            value.a=f32_encoding(0x3fa00000);value.b=f32_encoding(0x40200000);value.c=power(-3);
        }
    }
    reserves(value.a,id);reserves(value.b,id+1);reserves(value.c,id+2);
    return value;
}
void mode(u32 index) {
    constexpr u32 precisions[]={32,64,80};
    th10::arithmetic_mode(static_cast<th10::Precision>(precisions[index/8]),
                         static_cast<th10::Rounding>((index/2)%4));
    softfloat_detectTininess=static_cast<std::uint_fast8_t>(index%2);
}
void enter(const Case& value) {
    mode(value.mode);softfloat_exceptionFlags=static_cast<std::uint_fast8_t>(value.flags);
    errno=value.error;
}
void fail(u32 code,u32 id) {
    if(!status[4]) {status[4]=code;status[5]=id;}
    if(code==1)++status[6];
    if(code==2)++status[7];
}
__attribute__((noinline)) void prime_stack(u32 seed) {
    // Legal writes only, after which the object dies. Different histories can
    // reveal unspecified padding copied by the unchanged SoftFloat C backend.
    volatile u32 words[64];
    for(u32 index=0;index<64;++index)words[index]=seed^(index*0x9e3779b9u);
    stack_witness=words[seed&63u];
}
Extended binary(u32 id,u32 operation,const Extended& a,const Extended& b) {
    u8 before_a[16],before_b[16];
    std::memcpy(before_a,&a,16);std::memcpy(before_b,&b,16);
    Extended result;
    switch(operation) {
    case 0:result=a+b;break;
    case 1:result=a-b;break;
    case 2:result=a*b;break;
    default:result=a/b;break;
    }
    if(std::memcmp(before_a,&a,16)||std::memcmp(before_b,&b,16))fail(1,id);
    return result;
}
void capture(u32 id,u32 step,u32 event,u32 operation,u32 active_mode,
             const Case& value,const void* result,u32 bytes) {
    const int error=errno;
    const u32 flags=softfloat_exceptionFlags,precision=extF80_roundingPrecision;
    const u32 rounding=softfloat_roundingMode,tininess=softfloat_detectTininess;
    if(status[3]>=records_capacity) {fail(3,id);return;}
    auto& out=trace.values[status[3]++];
    out=Record{};
    out.words[0]=id;out.words[1]=step;out.words[2]=event;out.words[3]=operation;
    out.words[4]=active_mode;out.words[5]=value.flags;out.words[6]=static_cast<u32>(value.error);
    out.words[7]=flags;out.words[8]=precision;out.words[9]=rounding;
    out.words[10]=tininess;out.words[11]=static_cast<u32>(error);
    std::memcpy(out.words+12,result,bytes);
}
void casts(u32 id,u32 first,u32 active_mode,const Case& value,const Extended& result) {
    const float f=result.to_float();
    capture(id,first,3,~u32(0),active_mode,value,&f,sizeof(f));
    const double d=result.to_double();
    capture(id,first+1,4,~u32(0),active_mode,value,&d,sizeof(d));
}
void run_case(u32 id,u32 poison) {
    auto value=make_case(id);prime_stack(poison^id);enter(value);
    if(value.group<2) {
        const auto result=binary(id,value.operation,value.a,value.b);
        capture(id,0,0,value.operation,value.mode,value,&result,sizeof(result));
    } else if(value.group==2) {
        Extended result;
        if(value.scenario<48) {
            const u32 bits=static_cast<u32>(value.bits);float f;std::memcpy(&f,&bits,sizeof(f));
            result=Extended::from_float(f);
            capture(id,0,1,~u32(0),value.mode,value,&result,sizeof(result));
        } else {
            double d;std::memcpy(&d,&value.bits,sizeof(d));
            result=Extended::from_double(d);
            capture(id,0,2,~u32(0),value.mode,value,&result,sizeof(result));
        }
        casts(id,1,value.mode,value,result);
    } else {
        auto result=value.a;u32 active_mode=value.mode;
        for(u32 step=0;step<6;++step) {
            if(value.scenario==7) {active_mode=(value.mode+step*5)%24;mode(active_mode);}
            const u32 operation=(value.scenario+step)%4;
            // The operands can be the same object, and assignment takes place
            // only after the true by-value production wrapper returns.
            const auto& rhs=value.scenario==6&&step%2==0?result:step%2?value.c:value.b;
            result=binary(id,operation,result,rhs);
            capture(id,step,0,operation,active_mode,value,&result,sizeof(result));
        }
        casts(id,6,active_mode,value,result);
    }
    ++status[9];
}
u32 address(const void* pointer) {return static_cast<u32>(reinterpret_cast<std::uintptr_t>(pointer));}
} // namespace

extern "C" EMSCRIPTEN_KEEPALIVE const u32* arithmetic_oracle_info() {
    const u32 values[24]={
        1,sizeof(Extended),alignof(Extended),offsetof(Extended,significand),
        offsetof(Extended,exponent),offsetof(Extended,reserved16),offsetof(Extended,reserved32),
        sizeof(std::uint_fast8_t),16,batch_capacity,records_capacity,total_cases,total_records,
        directed_cases,random_cases,conversion_cases,chain_cases,EDOM,ERANGE,
        address(trace.values),address(status),32,20,mode_count
    };
    std::memcpy(info,values,sizeof(info));return info;
}
extern "C" EMSCRIPTEN_KEEPALIVE const u32* arithmetic_oracle_layout() {
    std::memset(layout,0,sizeof(layout));
    const Extended empty;
    const auto exact=Extended::from_float(1.5f);
    auto input=raw(0xa000000000000000ull,0x3fff);
    input.reserved16=0xa55a;input.reserved32=0xdeadbeef;
    Case value;value.mode=22;value.flags=21;value.error=EDOM;enter(value);
    const auto negative=-input;
    const int error=errno;
    std::memcpy(layout,&empty,16);std::memcpy(layout+4,&exact,16);
    std::memcpy(layout+8,&negative,16);
    layout[12]=softfloat_exceptionFlags;layout[13]=extF80_roundingPrecision;
    layout[14]=softfloat_roundingMode;layout[15]=softfloat_detectTininess;
    layout[16]=static_cast<u32>(error);
    return layout;
}
extern "C" EMSCRIPTEN_KEEPALIVE const u32* arithmetic_oracle_describe(u32 id) {
    if(id>=total_cases)return nullptr;
    const auto value=make_case(id);std::memset(description,0,sizeof(description));
    description[0]=1;description[1]=id;description[2]=value.group;
    description[3]=value.scenario;description[4]=value.operation;description[5]=value.mode;
    description[6]=value.flags;description[7]=static_cast<u32>(value.error);
    std::memcpy(description+8,&value.a,16);std::memcpy(description+12,&value.b,16);
    std::memcpy(description+16,&value.c,16);std::memcpy(description+20,&value.bits,8);
    description[22]=value.group<2?1:value.group==2?3:8;
    return description;
}
extern "C" EMSCRIPTEN_KEEPALIVE const Record* arithmetic_oracle_run(u32 first,u32 count,u32 poison) {
    std::memset(status,0,sizeof(status));status[0]=1;status[1]=first;status[2]=count;
    status[5]=~u32(0);status[10]=poison;status[11]=total_cases;
    if(first>total_cases||count>batch_capacity||count>total_cases-first) {
        fail(4,first);return nullptr;
    }
    for(u32 index=0;index<4;++index) {
        trace.prefix[index]=0x13579bdfu+index;trace.suffix[index]=0xfdb97531u-index;
    }
    for(u32 index=0;index<count;++index)run_case(first+index,poison);
    for(u32 index=0;index<4;++index)
        if(trace.prefix[index]!=0x13579bdfu+index||trace.suffix[index]!=0xfdb97531u-index)
            fail(2,first);
    return trace.values;
}
