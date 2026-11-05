#include "AnmFile.hpp"
#include "AnmEnvironment.hpp"
namespace th10 {
// 0x404f30. Embedded animations reset position before binding a script.
void initialize_embedded_animation(AnmFile& file,AnmVm& vm,i32 script,AnmEnvironment& env,u32& started){
    vm.initialize();vm.position=vm.script_position=vm.child_position={};vm.text_settings[0]=vm.text_settings[1]=16;vm.script_index=static_cast<std::int16_t>(script);
    file.initialize_script(vm,script,env,started);
}
// 0x43e7e0. Pool VMs are already initialized; retain their local state.
void AnmFile::bind_script(AnmVm& vm,i32 index,AnmEnvironment& env,u32& started_scripts){
    if(!scripts[index]||unavailable){std::memset(&vm,0,sizeof(vm));return;}
    vm.script_index=static_cast<std::int16_t>(index);vm.file_index=file_index;vm.animation_file=this;
    vm.flags&=~0x600u;vm.script_begin=vm.instruction=scripts[index];
    if(!(vm.script_timer_flags&1)){vm.script_timer.rate=env.rate;vm.script_timer_flags|=1;}
    vm.script_timer.initialize(-1);vm.flags&=~1u;vm.update(env);++started_scripts;
}
// 0x449870. Prepare a pooled animation without resetting its local variables.
void AnmFile::prepare_script(AnmVm& vm,i32 index,AnmEnvironment& env,u32& started_scripts){
    vm.position=vm.script_position=vm.child_position={};vm.flags|=0x40000000;
    vm.script_index=static_cast<std::int16_t>(index);vm.text_settings[0]=vm.text_settings[1]=0x10;
    bind_script(vm,index,env,started_scripts);
}
// 0x43e710. A missing script clears every byte, including sprite_index.
void AnmFile::initialize_script(AnmVm& vm,i32 index,AnmEnvironment& env,u32& started_scripts){
    if(!scripts[index]||unavailable){std::memset(&vm,0,sizeof(vm));return;}
    vm.initialize();vm.script_index=static_cast<std::int16_t>(index);vm.file_index=file_index;vm.animation_file=this;
    vm.flags&=~0x600u;vm.script_begin=vm.instruction=scripts[index];
    vm.script_timer.rate=env.rate;vm.script_timer_flags|=1;vm.script_timer.initialize(-1);vm.flags&=~1u;
    vm.update(env);++started_scripts;
}
// 0x43e8b0. Rebinding retains the VM's position, local variables and high flags.
void AnmFile::start_script(AnmVm& vm,i32 index,AnmEnvironment& env,u32& started_scripts){
    auto* script=scripts[index];
    if(!script||unavailable)return;
    vm.script_index=static_cast<std::int16_t>(index);vm.animation_file=this;
    if(vm.flags&0x200){vm.flags=(vm.flags|8)^0x200;vm.scale.x=Scalar::mul(vm.scale.x,-1.0f);}
    vm.flags=(vm.flags&0xffff0000u)|7;vm.color=0xffffffff;
    vm.script_timer.reset();
    vm.position_interpolation.duration=vm.color_interpolation.duration=vm.alpha_interpolation.duration=0;
    vm.rotation_interpolation.duration=vm.scale_interpolation.duration=vm.color2_interpolation.duration=vm.alpha2_interpolation.duration=0;
    vm.file_index=file_index;vm.animation_file=this;
    vm.flags&=~0x600u;vm.script_begin=script;vm.instruction=script;
    if(!(vm.script_timer_flags&1)){vm.script_timer.rate=env.rate;vm.script_timer_flags|=1;}
    vm.script_timer.initialize(-1);vm.flags&=~1u;
    vm.update(env);++started_scripts;
}
}
