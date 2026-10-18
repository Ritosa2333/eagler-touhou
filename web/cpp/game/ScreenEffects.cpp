#include "ScreenEffects.hpp"
#include "Presentation.hpp"
#include <algorithm>
namespace th08 {
namespace {Extended integer(i32 value){return Extended::from_int(value);}}
ScreenEffects::~ScreenEffects(){clear();}
void ScreenEffects::clear(){while(!active.empty())remove(&active.back()->state);}
void ScreenEffects::shake(float amplitude){
    for(float* offset:{&renderer.shake.x,&renderer.shake.y}){
        switch(random.bounded32(3)){case 0:*offset=0;break;case 1:*offset=amplitude;break;case 2:*offset=-amplitude;break;}
    }
}
JobResult ScreenEffects::calculate(ScreenEffectState& s){
    presentation_previous[&s]={s.alpha,s.timer.current,s.phase,s.a,s.type};
    auto& t=s.timer;auto& c=context;
    switch(s.type){
    case ScreenEffectType::FadeIn:
        if(s.duration)s.alpha=std::max(0,(number(255)-t.value()*number(255)/integer(s.duration)).truncate_int());
        if(t.current>=s.duration)return JobResult::Remove;t.tick(c.timing);break;
    case ScreenEffectType::FadeOut:case ScreenEffectType::ArcadeFadeOut:
        if(c.terminating)return JobResult::Remove;
        if(s.duration)s.alpha=std::max(0,(t.value()*number(255)/integer(s.duration)).truncate_int());
        if(t.current>=s.duration)return JobResult::Remove;
        if(!c.paused&&!c.retry)t.tick(c.timing);break;
    case ScreenEffectType::MenuFullFade:case ScreenEffectType::MenuArcadeFade:
        if(s.phase==0){if(s.duration&&t.current<=s.duration)s.alpha=(t.value()*number(128)/integer(s.duration)).truncate_int();}
        else {if(t.current>8)return JobResult::Remove;s.alpha=wrapping_sub(128,(t.value()*number(128)/number(8)).truncate_int());}
        t.tick(c.timing);break;
    case ScreenEffectType::Flash:
        if(c.terminating)return JobResult::Remove;
        if(t.current<s.duration){const u32 alpha=u32(s.b)>>24;s.alpha=std::max(0,wrapping_sub(i32(alpha),(t.value()*Extended::from_int64(alpha)/integer(s.duration)).truncate_int()));}
        else {s.alpha=0;s.a=wrapping_sub(s.a,1);if(s.a<=0)return JobResult::Remove;t.set(0);}
        t.tick(c.timing);break;
    case ScreenEffectType::Shake:{
        if(c.frozen||c.shake_disabled)return JobResult::Continue;if(c.terminating)return JobResult::Remove;
        t.tick(c.timing);if(t.current>=s.duration)return JobResult::Remove;
        float amplitude=(t.value()*integer(wrapping_sub(s.b,s.a))).to_float();
        amplitude=(number(amplitude)/integer(s.duration)).to_float();amplitude=(integer(s.a)+number(amplitude)).to_float();shake(amplitude);break;
    }
    case ScreenEffectType::EnvelopeShake:{
        if(c.frozen||c.shake_disabled)return JobResult::Continue;if(c.transition_state<=1)return JobResult::Remove;
        t.tick(c.timing);float amplitude;
        const i32 hold_end=wrapping_add(s.a,s.b),end=wrapping_add(hold_end,s.c);
        if(t.current<s.a)amplitude=(t.value()/integer(s.a)).to_float();
        else if(t.current<hold_end)amplitude=1;
        else if(t.current<end){const float total=Extended::from_int64(u32(end)).to_float();amplitude=((number(total)-t.value())/Extended::from_int64(u32(s.c))).to_float();}
        else return JobResult::Remove;
        amplitude=(integer(s.duration)*number(amplitude)).to_float();shake(amplitude);break;
    }
    }
    return JobResult::Continue;
}
JobResult ScreenEffects::draw(ScreenEffectState& s){
    bool full=false;
    switch(s.type){
    case ScreenEffectType::Shake:case ScreenEffectType::EnvelopeShake:return JobResult::Continue;
    case ScreenEffectType::FadeIn:case ScreenEffectType::FadeOut:{auto v=renderer.viewport;v.x=v.y=0;v.width=640;v.height=480;renderer.set_viewport(v);full=true;break;}
    case ScreenEffectType::MenuFullFade:full=true;break;
    default:break;
    }
    i32 draw_alpha=s.alpha;if(presentation::active){const auto found=presentation_previous.find(&s);if(found!=presentation_previous.end()){const auto& before=found->second;if(before.type==s.type&&before.phase==s.phase&&before.a==s.a&&s.timer.current>=before.timer)draw_alpha=i32(std::clamp(presentation::lerp_world(float(before.alpha),float(s.alpha)),0.0f,255.0f));}}
    const u32 color=(u32(draw_alpha)<<24)|(s.type==ScreenEffectType::Flash?u32(s.b)&0xffffff:u32(s.a));
    const u32 colors[4]{color,color,color,color};renderer.draw_rectangle(full?0:32,full?0:16,full?640:416,full?480:464,colors);return JobResult::Continue;
}
ScreenEffectState* ScreenEffects::create(ScreenEffectType type,i32 duration,i32 a,i32 b,i32 c,i32 priority){
    if(u32(type)>7)return nullptr;
    auto* instance=new Instance();instance->owner=this;auto& s=instance->state;s.type=type;s.duration=duration;s.a=a;s.b=b;s.c=c;
    auto* calc=Chain::create(calculate_callback);calc->argument=instance;calc->added=added_callback;calc->deleted=deleted_callback;
    ChainElement* draw=nullptr;if(type!=ScreenEffectType::Shake&&type!=ScreenEffectType::EnvelopeShake){draw=Chain::create(draw_callback);draw->argument=instance;}
    active.push_back(instance);chain.add(calc,3);if(draw)chain.add(draw,priority,true);s.calculation=calc;s.drawing=draw;return &s;
}
void ScreenEffects::remove(ScreenEffectState* state){if(state)chain.cut(state->calculation);}
JobResult ScreenEffects::calculate_callback(void* p){auto& i=*static_cast<Instance*>(p);return i.owner->calculate(i.state);}
JobResult ScreenEffects::draw_callback(void* p){auto& i=*static_cast<Instance*>(p);return i.owner->draw(i.state);}
i32 ScreenEffects::added_callback(void* p){static_cast<Instance*>(p)->state.timer.set(0);return 0;}
i32 ScreenEffects::deleted_callback(void* p){
    auto* i=static_cast<Instance*>(p);auto& owner=*i->owner;i->state.calculation->deleted=nullptr;
    owner.presentation_previous.erase(&i->state);
    owner.chain.cut(i->state.drawing);i->state.drawing=nullptr;
    owner.active.erase(std::remove(owner.active.begin(),owner.active.end(),i),owner.active.end());delete i;return 0;
}
}
