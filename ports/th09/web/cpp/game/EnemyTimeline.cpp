#include "EnemyTimeline.hpp"
#include "Binary.hpp"
namespace th09 {
bool EnemyTimeline::start(EclProgram& resource,u32 index,i32 mirror){
    auto p=resource.timeline(index);invalid=!p;completed=!p;program=&resource;
    time.reset();wait.reset();mirrored=mirror;
    if(!p)return false;
    cursor=u32(reinterpret_cast<const u8*>(p)-resource.data());end=cursor+resource.timeline_size(index);return true;
}
i32 EnemyTimeline::step(const FrameTiming& timing,float frame_step,u8 difficulty_mask,i32 dialogue,Rng& random,EnemyTimelineActions& actions){
    if(invalid)return -1;if(completed)return 1;
    if(dialogue>=0||dialogue==-2)return 0;
    if(wait.value()>0){wait.decrement(1,timing);return 0;}
    bool stalled=false;
    while(!stalled){
        if(cursor>end||end-cursor<8){invalid=true;return -1;}
        const u8* ins=program->data()+cursor;const i32 at=signed_bits(read32(ins));
        if(at<0){completed=true;time.reset();return 1;}
        const u32 size=ins[6],op=read16(ins+4);
        if(size<8||size>end-cursor){invalid=true;return -1;}
        if(time.current<at)break;
        if(time.current==at&&(ins[7]&difficulty_mask)){
            const u32 required=op==2||op==4||op==11||op==12?36:
                op==0||op==1||op==15||op==17?32:op==3||op==5?28:
                op==8?16:op==10||op==13||op==14?12:8;
            if(size<required){invalid=true;return -1;}
            const auto word=[&](u32 i){return read32(ins+8+i*4);};
            const auto integer=[&](u32 i){return signed_bits(word(i));};
            const auto real=[&](u32 i){const u32 b=word(i);float f;std::memcpy(&f,&b,4);return f;};
            EnemySpawn request;request.script=size>=12?i16(word(0)):0;request.mirrored=mirrored;
            switch(op){
            case 0:case 1:case 15:case 17:{
                request.position={real(1),real(2),0};request.life=integer(3);request.item=i8(word(4));request.score=integer(5);
                EclVm* enemy=actions.spawn(request);if(op==17){if(!enemy){invalid=true;return -1;}enemy->values.flags|=0x200;}break;}
            case 2:case 4:
                if(op==4)request.mirrored=mirrored=1;
                request.position={float(random.range(float(real(2)-real(1)))+double(real(1))),real(3),0};
                request.life=integer(4);request.item=i8(word(5));request.score=integer(6);actions.spawn(request);break;
            case 3:case 5:
                if(op==5)request.mirrored=mirrored=1;
                request.position={float(random.range(384)),real(1),0};request.life=integer(2);request.item=i8(word(3));request.score=integer(4);actions.spawn(request);break;
            case 8:{auto enemy=actions.boss(integer(0));if(!enemy){invalid=true;return -1;}enemy->pending_interrupt=i16(word(1));break;}
            case 10:{auto enemy=actions.boss(integer(0));stalled=enemy&&(enemy->behavior_flags&1);break;}
            case 11:case 12:{
                if(op==12)request.mirrored=mirrored=1;
                request.position={real(1),real(2),0};request.life=integer(3);request.item=-1;request.score=integer(6);
                auto enemy=actions.spawn(request);if(!enemy){invalid=true;return -1;}
                enemy->values.drop_count=integer(4);enemy->values.drop_item=integer(5);break;}
            case 13:{i32* events=actions.timeline_events();stalled=true;for(u32 n=0;n<4;++n)if(events[n]==integer(0)){events[n]=-1;stalled=false;}break;}
            case 14:{i32* events=actions.timeline_events();for(u32 n=0;n<4;++n)if(events[n]<0)events[n]=integer(0);break;}
            case 16:actions.finish_match();break;
            default:break;
            }
        }
        if(stalled)time.decrement(1,timing);else cursor+=size;
    }
    time.advance(frame_step,timing.rate,timing.force_step?32:0);return 0;
}
}
