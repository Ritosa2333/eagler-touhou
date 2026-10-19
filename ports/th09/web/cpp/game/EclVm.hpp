#pragma once
#include "EclProgram.hpp"
#include "EclVariables.hpp"
#include "Timer.hpp"
#include "EnemyMotion.hpp"
#include "EclEmitter.hpp"
#include "EclLasers.hpp"
#include "EnemyAnimation.hpp"
#include "EnemyStatus.hpp"
#include "EnemyTrail.hpp"
#include <memory>
namespace th09 {
struct EclInterpolation {
    u32 active=0;Timer time{0,0,0};i32 duration=0,curve=0,easing=0;float values[4]{},target=0;
};
static_assert(sizeof(EclInterpolation)==48);
struct EclFrame {
    EclProgram* program=nullptr;
    EclInstruction* instruction=nullptr;Timer timer,wait;
    EclLocals locals;EclInterpolation interpolations[8]{};
    i32 subroutine=-1,native_callback=-1;const EclInstruction* native_instruction=nullptr;
};
struct EclContext:EclFrame {
    EclFrame stack[16];i32 depth=0;bool returned=false;
};
struct EclVm {
    EclVariables values;
    EclProgram* program=nullptr;EclContext primary;EclContext* active=nullptr;
    std::unique_ptr<EclContext> asynchronous[4];u32 generations[4]{};
    i32 active_slot=-1,scratch_depth=0;u32 behavior_flags=0,difficulty_flags=0;
    i16 pending_interrupt=-1,interrupt_subroutines[32]{};
    Vec3 position_offset,velocity;
    EnemyMotion movement;
    EclEmitter emitter;
    EclLasers lasers;
    EnemyAnimation animation;
    EnemyStatus status;
    EnemyTrail trail;
    bool finished=false,invalid=false; i32 failed_opcode=-1;
    EclContext& context() noexcept{return active?*active:primary;}
    u32 raw(const EclInstruction&,u32 argument) const noexcept;
    float raw_float(const EclInstruction&,u32 argument) const noexcept;
    i32 integer(const EclInstruction&,u32 argument) noexcept;
    double wide(const EclInstruction&,u32 argument) noexcept;
    float real(const EclInstruction& i,u32 argument) noexcept{return float(wide(i,argument));}
    i32* integer_target(EclInstruction&,u32 argument) noexcept;
    float* float_target(EclInstruction&,u32 argument) noexcept;
    void bind_context() noexcept{values.locals=&context().locals;}
};
struct EclNativeServices {
    virtual ~EclNativeServices()=default;
    virtual bool execute(EclVm&,i32 script_callback,const EclInstruction&)=0;
};
struct EclGameCommands {
    virtual ~EclGameCommands()=default;
    virtual bool execute(EclVm&,EclInstruction&)=0;
};
struct EclBindings {
    EclNativeServices* native=nullptr;EclGameCommands* game=nullptr;BulletEmissionActions* bullets=nullptr;LaserEmissionActions* lasers=nullptr;EnemyAnimationActions* animations=nullptr;
};
class EclExecutor {
    enum class Flow {next,branch,returned,failed};
    FrameTiming timing;float frame_step=1;u32 difficulty_mask=1;
    EclNativeServices* native=nullptr;EclGameCommands* commands=nullptr;
    BulletEmissionActions* emissions=nullptr;
    LaserEmissionActions* laser_emissions=nullptr;
    EnemyAnimationActions* animations=nullptr;
    bool initialize(EclVm&,EclContext&,i32 subroutine,EclProgram* source=nullptr);
    bool jump(EclVm&,EclInstruction&,i32 time,i32 offset,bool full_reset);
    bool call(EclVm&,EclInstruction&,i32 subroutine);
    bool interrupt(EclVm&,EclInstruction&);
    Flow dispatch(EclVm&,EclInstruction&);
    bool interpolate(EclVm&);
    bool step_context(EclVm&);
public:
    EclExecutor(const FrameTiming& t,float step,u32 mask,const EclBindings& bindings={}):timing(t),frame_step(step),difficulty_mask(mask),native(bindings.native),commands(bindings.game),emissions(bindings.bullets),laser_emissions(bindings.lasers),animations(bindings.animations){}
    bool start(EclVm&,EclProgram&,i32 subroutine);
    bool call_from_other(EclVm& vm,i32 subroutine){return vm.context().instruction&&call(vm,*vm.context().instruction,subroutine);}
    bool enter_main(EclVm& vm,i32 subroutine){return initialize(vm,vm.primary,subroutine);}
    bool step(EclVm&);
};
}
