#pragma once
#include "EclResource.hpp"
#include "Timer.hpp"
#if PORT_TITLE == 16
#include "Interpolation.hpp"
#endif
namespace th11 {
enum class EclValueType:u8 {Untyped=0,Integer='i',Float='f'};
// The original reads stack-relative values straight out of the ECL context and
// never bounds them, which is only safe there because the context sits inside a
// larger allocation. In the port the stack is a real 4096-byte array, so an offset
// past it is an out-of-bounds access, and a wasm trap kills the whole session
// (stage 4 died this way at frame 1502 on an i32.load at stack.data + index).
//
// Clamp instead of trapping, and count it, so the deviation stays measurable rather
// than silent: ecl_stack_clamp_first records the largest position reached, which is
// the number needed to size the stack the original actually uses.
i32 ecl_stack_clamp(i32 position,u32 size)noexcept;
extern i32 ecl_stack_clamp_count,ecl_stack_clamp_first;
// The instruction that most recently drove a stack access, recorded by every accessor
// (both paths, so the value is never stale) so the clamp can name its caller instead of
// leaving it a guess. This is how the stage 4 out-of-bounds access was traced to TH11's
// 0x1b4 shadowing TH12's: the clamp reported a position of 0x40800000, the bit pattern
// of 4.0f, and the recording named opcode 0x1b4 argument 0.
extern const EclInstruction* ecl_last_integer_access;
extern i32 ecl_last_integer_argument,ecl_clamp_opcode,ecl_clamp_argument;
struct EclStack {
    u8 data[4096];i32 top=0,frame_base=0;
    i32 push(EclValueType,u32 bits)noexcept;
    bool pop(u32& bits,EclValueType& type)noexcept;
    bool pop_raw(u32& bits)noexcept;
    i32 enter_frame(i32 local_bytes)noexcept;
    void leave_frame()noexcept;
    template<class T>T local(i32 offset)const noexcept{T v;std::memcpy(&v,data+ecl_stack_clamp(frame_base+offset,sizeof v),sizeof v);return v;}
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
#if PORT_TITLE == 16
    struct ScalarSlot {FloatInterpolator value;float current=0;const EclInstruction* target=nullptr;};
    ScalarSlot th16_scalars[8]{};
    float th16_scalar_rate=1;
#endif
    i32 integer_argument(u32,EclGlobals&);
    double float_argument(u32,EclGlobals&);
    i32 resolve_integer(i32,EclGlobals&);
    double resolve_float(float,EclGlobals&);
    i32* integer_reference(u32,EclGlobals&);
    float* float_reference(u32,EclGlobals&);
    i32 update(float elapsed,EclServices&);
};
static_assert(offsetof(EclContext,owner)==0x1014);
#if PORT_TITLE == 16
static_assert(sizeof(EclContext)==0x1024+8*52+4);
#else
static_assert(sizeof(EclContext)==0x1024);
#endif
inline float float_bits(u32 v)noexcept{float f;std::memcpy(&f,&v,4);return f;}
inline u32 float_bits(float v)noexcept{u32 b;std::memcpy(&b,&v,4);return b;}
}
