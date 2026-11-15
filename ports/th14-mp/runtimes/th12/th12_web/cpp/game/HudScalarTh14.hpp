#pragma once
#include "PortTitle.hpp"
#include "Timer.hpp"

namespace th11 {
// Scalar shared access keeps each retail word separate. Non-worker builds
// retain ordinary accesses; no lock or whole-HUD snapshot is introduced.
template<class T> inline T th14_hud_load(const T& source)noexcept{
#if PORT_TITLE==14 && defined(TH14_CPP_STAGE_WORKER)
    T value;__atomic_load(&source,&value,__ATOMIC_SEQ_CST);return value;
#else
    return source;
#endif
}
template<class T> inline void th14_hud_store(T& target,T value)noexcept{
#if PORT_TITLE==14 && defined(TH14_CPP_STAGE_WORKER)
    __atomic_store(&target,&value,__ATOMIC_SEQ_CST);
#else
    target=value;
#endif
}
#if PORT_TITLE==14
enum class HudPrepareWordTh14:u32 {Current,Previous,Fractional,Rate,Flags,Displayed,Seconds,PreviousSeconds};
template<class ReadScore,class Write>
inline void th14_prepare_hud_fields(Timer& timer,const float* rate,i32& displayed,
                                    i32* seconds,i32& previous_seconds,ReadScore&& score,Write&& write){
    using Word=HudPrepareWordTh14;
    const u32 flags=th14_hud_load(timer.flags);
    if(!(flags&1)){
        write(Word::Current,timer.current,i32(0));
        write(Word::Previous,timer.previous,i32(-999999));
        write(Word::Fractional,timer.fractional,0.f);
        write(Word::Rate,timer.rate,rate);
        write(Word::Flags,timer.flags,flags|1u);
    }
    write(Word::Current,timer.current,i32(0));
    write(Word::Fractional,timer.fractional,0.f);
    write(Word::Previous,timer.previous,i32(-1));
    write(Word::Displayed,displayed,score());
    if(seconds)write(Word::Seconds,*seconds,i32(-1));
    write(Word::PreviousSeconds,previous_seconds,i32(-1));
}
inline void th14_hud_elapsed_tick(Timer& timer)noexcept{
    th14_hud_store(timer.previous,th14_hud_load(timer.current));
    const float speed=th14_hud_load(*th14_hud_load(timer.rate));
    if(speed>0.99f&&speed<1.01f){
        th14_hud_store(timer.current,wrapping_add(th14_hud_load(timer.current),1));
        th14_hud_store(timer.fractional,float(double(th14_hud_load(timer.fractional))+1));
    }else{
        const float next=float(double(th14_hud_load(timer.fractional))+speed);
        th14_hud_store(timer.fractional,next);
        th14_hud_store(timer.current,truncate_int(th14_hud_load(timer.fractional)));
    }
}
enum class HudScoreWordTh14:u32 {Current,Displayed,Speed,High,Continues,HighContinues,Flags};
enum class HudScoreAccessTh14:u32 {
    CurrentInitial,DisplayedInitial,SpeedMinimum,SetMinimum,CurrentAfterMinimum,
    DisplayedRemaining,SpeedClamp,SetClamp,SpeedAdd,DisplayedAdd,SetDisplayed,
    DisplayedFinish,CurrentFinish,ZeroSpeed,DisplayedHigh,HighCompare,SetHigh,
    ContinuesHigh,SetHighContinues,FlagsRead,SetFlags
};
// Retail434360: preserve each conditional reload and the unsigned SHR before
// signed comparisons. Callback access tags are identities, never frame targets.
template<class Read,class Write> inline void th14_hud_score_update(Read&& read,Write&& write){
    using A=HudScoreAccessTh14;using W=HudScoreWordTh14;
    i32 current=read(A::CurrentInitial,W::Current);
    const i32 initial=read(A::DisplayedInitial,W::Displayed);
    if(current!=initial){
        i32 step=i32((u32(current)-u32(initial))>>5);
        if(step>=0x8d55e)step=0x8d55e;else if(!step)step=1;
        if(read(A::SpeedMinimum,W::Speed)<step){
            write(A::SetMinimum,W::Speed,step);
            current=read(A::CurrentAfterMinimum,W::Current);
        }
        const i32 remaining=signed_bits(u32(current)-u32(read(A::DisplayedRemaining,W::Displayed)));
        if(read(A::SpeedClamp,W::Speed)>remaining)write(A::SetClamp,W::Speed,remaining);
        const i32 speed=read(A::SpeedAdd,W::Speed);
        const i32 before=read(A::DisplayedAdd,W::Displayed);
        write(A::SetDisplayed,W::Displayed,wrapping_add(before,speed));
        const i32 after=read(A::DisplayedFinish,W::Displayed);
        if(after>=read(A::CurrentFinish,W::Current))write(A::ZeroSpeed,W::Speed,i32(0));
    }
    const i32 displayed=read(A::DisplayedHigh,W::Displayed);
    if(read(A::HighCompare,W::High)<displayed){
        write(A::SetHigh,W::High,displayed);
        write(A::SetHighContinues,W::HighContinues,read(A::ContinuesHigh,W::Continues));
        const u32 flags=u32(read(A::FlagsRead,W::Flags));
        write(A::SetFlags,W::Flags,signed_bits(flags|4u));
    }
}
#endif
}
