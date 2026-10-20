#pragma once
#include "FighterPattern.hpp"
namespace th075 {
struct AnimationCursor {
    i16 action=0,index=0,elapsed=0,frameCount=0,duration=0;
    using Rebind=void(*)(void*,AnimationCursor&);
    i16 advance(Rebind,void*);
    void reset(i16 action,Rebind,void*);
    void set(i16 action,i16 index,i16 elapsed,Rebind,void*);
    void select_frame(i16 index,Rebind,void*);
};
class FighterAnimation {
public:
    const FighterPattern* pattern=nullptr;const PatternFrame* frame=nullptr;
    AnimationCursor cursor{};
    bool valid()const{return frame!=nullptr;}
    void reset(i16 action);
    void set(i16 action,i16 index,i16 elapsed=0);
    i16 advance();
private:
    void bind();static void rebound(void*,AnimationCursor&);
};
}
