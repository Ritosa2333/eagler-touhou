#include "GuardSelector.hpp"
namespace th075 {
GuardResponse select_guard_response(DefenseView& view,u32 attackFlags,ReplayStreams& replay){
    auto& input=view.input;const double relativeX=double(view.opponent.x)-double(view.motion.x);
    if(!(view.collisionFlags&0x10)||(attackFlags&0x10)||view.combat.recoveryTimers[3]>0)return GuardResponse::None;
    const auto autoDirection=[&]{input.horizontal=relativeX<=0?1:-1;};
    if(view.cpuControlled){
        if(view.training){
            switch(view.trainingGuard){
                case 0:return GuardResponse::None;
                case 1:autoDirection();input.vertical=0;break;
                case 2:autoDirection();input.vertical=1;break;
                case 3:autoDirection();input.vertical=(attackFlags&2)?0:1;break;
            }
        }else{
            u8 roll=0;
            if(replay.mode==ReplayMode::Playback)roll=replay.auxiliary(0);
            else{roll=view.difficulty==1?u8(replay.random.next()%10+9):view.difficulty==0?u8(replay.random.next()%10+5):16;roll=replay.auxiliary(roll);}
            if(roll<10)return GuardResponse::None;autoDirection();input.vertical=(attackFlags&2)?0:1;
        }
    }
    const bool guardingAction=view.combat.action>=150&&view.combat.action<=199;
    if(!guardingAction&&!view.guardEstablished){
        if(relativeX*double(input.horizontal)>0||input.horizontal==0)return GuardResponse::None;
    }
    if(view.motion.y<=0&&!(view.collisionFlags&4)){
        const bool crouch=input.vertical>0,spirit=view.combat.spirit>0;
        if(crouch){
            if(spirit)return (attackFlags&4)?GuardResponse::Crouching:GuardResponse::WrongCrouching;
            if(attackFlags&4)return GuardResponse::EmptyCrouching;
        }else{
            if(spirit)return (attackFlags&2)?GuardResponse::Standing:GuardResponse::WrongStanding;
            if(attackFlags&2)return GuardResponse::EmptyStanding;
        }
        return (attackFlags&0x200000)?GuardResponse::ChipHit:GuardResponse::None;
    }
    return (attackFlags&8)&&view.combat.spirit>0?GuardResponse::Air:GuardResponse::None;
}
}
