#include "EnemyAnimation.hpp"
#include "EclVm.hpp"
namespace th09 {
bool EnemyAnimation::command(EclVm& vm,const EclInstruction& i,EnemyAnimationActions& actions){
    const auto iv=[&](u32 n){return vm.integer(i,n);};
    switch(i.opcode){
    case 54:case 58:{const bool alternate=i.opcode==58;const i32 script=iv(0);
        if(!actions.start_animation(vm,0,alternate,script))return false;
        vm.behavior_flags=(vm.behavior_flags&0x7fffffffu)|(alternate?0x80000000u:0);break;
    }
    case 55:case 56:case 59:case 60:{
        i16 values[6];if(i.opcode==55||i.opcode==59){const i32 base=iv(0);for(u32 n=0;n<6;++n)values[n]=i16(wrapping_add(base,i32(n)));}
        else for(i32 n=5;n>=0;--n)values[n]=i16(iv(u32(n)));
        idle=values[0];left=values[1];right=values[2];stop_left=values[3];stop_right=values[4];death=values[5];pose=-1;
        vm.behavior_flags=(vm.behavior_flags&0x7fffffffu)|(i.opcode>=59?0x80000000u:0);break;
    }
    case 57:case 61:{
        if(i.opcode==61)vm.behavior_flags|=0x80000000u;
        if(i.variable_mask&1)iv(0);
        if(iv(1)>=0){const i32 script=iv(1),slot=iv(0);if(slot<0||slot>=2)return false;
            if(!actions.start_animation(vm,u32(slot)+1,(vm.behavior_flags&0x80000000u)!=0,script))return false;
        }else {const i32 slot=iv(0);if(slot<0||slot>=2)return false;layers[slot+1].scriptIndex=-1;}
        if(i.opcode==57)vm.behavior_flags&=0x7fffffffu;break;
    }
    case 62:if(!actions.start_animation(vm,0,(vm.behavior_flags&0x80000000u)!=0,death))return false;break;
    default:return false;
    }
    return !vm.invalid;
}
bool EnemyAnimation::update_pose(EclVm& vm,EnemyAnimationActions& actions){
    if(vm.values.life<=0||left<0)return true;
    const float vx=vm.behavior_flags&0x8000?-vm.velocity.x:vm.velocity.x;
    const i8 next=vx<-.01f?1:vx>.01f?2:0;if(pose==next)return true;
    const i32 script=next==1?left:next==2?right:pose==-1?idle:pose==1?stop_left:stop_right;
    if(!actions.start_animation(vm,0,(vm.behavior_flags&0x80000000u)!=0,script))return false;pose=next;return true;
}
}
