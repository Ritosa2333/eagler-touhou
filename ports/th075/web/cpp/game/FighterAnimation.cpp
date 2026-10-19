#include "FighterAnimation.hpp"
namespace th075 {
static i16 increment(i16 value){const u16 bits=u16(value)+u16(1);i16 result;std::memcpy(&result,&bits,2);return result;}
i16 AnimationCursor::advance(Rebind bind,void* owner){
    elapsed=increment(elapsed);if(elapsed<duration)return 0;
    elapsed=0;index=increment(index);
    if(index<frameCount){bind(owner,*this);return index;}
    index=0;bind(owner,*this);return frameCount;
}
void AnimationCursor::reset(i16 value,Rebind bind,void* owner){set(value,0,0,bind,owner);}
void AnimationCursor::set(i16 a,i16 i,i16 e,Rebind bind,void* owner){action=a;index=i;elapsed=e;bind(owner,*this);}
void AnimationCursor::select_frame(i16 i,Rebind bind,void* owner){index=i;elapsed=0;bind(owner,*this);}
void FighterAnimation::bind(){
    frame=nullptr;cursor.frameCount=cursor.duration=0;
    if(!pattern||cursor.action<0||std::size_t(cursor.action)>=pattern->sequences.size())return;
    const auto& sequence=pattern->sequences[cursor.action];cursor.frameCount=i16(sequence.frames.size());
    if(!sequence.present||cursor.index<0||std::size_t(cursor.index)>=sequence.frames.size())return;
    frame=&sequence.frames[cursor.index];cursor.duration=i16(frame->duration);
}
void FighterAnimation::rebound(void* owner,AnimationCursor&){static_cast<FighterAnimation*>(owner)->bind();}
void FighterAnimation::reset(i16 action){cursor.reset(action,rebound,this);}
void FighterAnimation::set(i16 action,i16 index,i16 elapsed){cursor.set(action,index,elapsed,rebound,this);}
i16 FighterAnimation::advance(){return cursor.advance(rebound,this);}
}
