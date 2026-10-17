#pragma once
#include "EclResource.hpp"
#include "Timer.hpp"
namespace th11 {
enum class EclValueType:u8 {Untyped=0,Integer='i',Float='f'};
struct EclStack {
    u8 data[4096];i32 top=0,frame_base=0;
    i32 push(EclValueType,u32 bits)noexcept;
    bool pop(u32& bits,EclValueType& type)noexcept;
    bool pop_raw(u32& bits)noexcept;
    i32 enter_frame(i32 local_bytes)noexcept;
    void leave_frame()noexcept;
    template<class T>T local(i32 offset)const noexcept{T v;std::memcpy(&v,data+frame_base+offset,sizeof v);return v;}
};
static_assert(sizeof(EclStack)==0x1008);
struct EclGlobals {
    virtual ~EclGlobals()=default;
    virtual i32 integer(i32)=0;
    // The original x87 return retains exact integer globals until the caller
    // explicitly stores a float. In particular, RNG u32 values are not f32.
    virtual double floating(i32)=0;
    virtual i32* integer_reference(i32)=0;
    virtual float* float_reference(i32)=0;
};
struct EclOwner;struct EclServices;
struct EclContext {
    float time=0;const EclInstruction* instruction=nullptr;EclStack stack;
    i32 thread_id=-1;EclOwner* owner=nullptr;u32 state=0,difficulty=0xff,flags=0;
    i32 integer_argument(u32,EclGlobals&);
    double float_argument(u32,EclGlobals&);
    i32 resolve_integer(i32,EclGlobals&);
    double resolve_float(float,EclGlobals&);
    i32* integer_reference(u32,EclGlobals&);
    float* float_reference(u32,EclGlobals&);
    i32 update(float elapsed,EclServices&);
};
static_assert(offsetof(EclContext,owner)==0x1014);
static_assert(sizeof(EclContext)==0x1024);
inline float float_bits(u32 v)noexcept{float f;std::memcpy(&f,&v,4);return f;}
inline u32 float_bits(float v)noexcept{u32 b;std::memcpy(&b,&v,4);return b;}
}
