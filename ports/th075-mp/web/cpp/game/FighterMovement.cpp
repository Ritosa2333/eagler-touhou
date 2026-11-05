#include "FighterMovement.hpp"
namespace th075 {
static i32 relative_direction(i8 facing,i32 direction){const u32 bits=u32(i32(facing))*u32(direction);i32 result;std::memcpy(&result,&bits,4);return result;}
void FighterMovement::select(FighterMotion& motion,const FighterMotion& opponent,const ControlCounts& input,FighterAnimation& animation){
    auto& action=animation.cursor.action;
    const auto change=[&](i16 next){if(action!=next)animation.reset(next);};
    if(motion.y<=0)usedAirDashes=usedAirJumps=0;
    if(!(motion.y<=0)||action==6||action==7||action==8)return;
    if(input.vertical<0){
        motion.update_facing(opponent);const bool dash=input.buttons[3]!=0;
        if(input.horizontal==0)change(dash?204:6);
        else if(relative_direction(motion.facing,input.horizontal)>0)change(dash?205:7);
        else if(relative_direction(motion.facing,input.horizontal)<0)change(dash?206:8);
        return;
    }
    if(input.vertical==0){
        if(input.horizontal==0){
            if(action==1||action==2)animation.reset(3);
            else if(action!=0&&action!=3&&action!=10)animation.reset(0);
        }else{
            motion.update_facing(opponent);const bool forward=relative_direction(motion.facing,input.horizontal)>0,dash=input.buttons[3]!=0;
            change(forward?(dash?200:4):(dash?201:5));
        }
    }else if(action==1||action==2){if(action!=1)animation.reset(2);}
    else animation.reset(1);
    motion.update_facing(opponent);
}
void FighterMovement::aerial_recovery(float height,i8 facing,i16 health,u32 frameFlags,const ControlCounts& input,bool cpuControlled,bool training,bool manualRecovery,ReplayRandom& random,FighterAnimation& animation){
    if((frameFlags&0x100000)||health<=0)return;
    if(recoveryDelay<1){
        if(!cpuControlled||!training||manualRecovery){
            bool button=false;for(const auto value:input.buttons)button|=value!=0;
            if(input.horizontal!=0&&button)animation.reset(relative_direction(facing,input.horizontal)>0?180:181);
        }else animation.reset((random.next()&1)?180:181);
    }else if(height>0)--recoveryDelay;
}
}
