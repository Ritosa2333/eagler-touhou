#include "AttackQueue.hpp"
namespace th09 {
AttackQueue::AttackQueue(const std::array<AttackBehavior,27>& b,AttackServices& a):behaviors(b),actions(a){for(auto& list:draw_lists)list.reserve(capacity);}
AttackActor* AttackQueue::create(i32 kind,i32 side,const Vec3& position,const Vec3* extra,const AttackActor* parent){
    if(side<0||side>1||kind<0||u32(kind)>=behaviors.size()||!behaviors[kind].initialize||!behaviors[kind].update){invalid=true;return nullptr;}
    i32 count=0;for(u32 n=0;n<capacity;++n)if(actors[n].active&&actors[n].source_side==side)++count;
    if(limits[side]<=count){before_write(actors[capacity]);return &actors[capacity];}
    actions.play_sound(45,side?500:-500);
    for(u32 n=0;n<capacity;++n){
        auto& actor=actors[n];if(actor.active)continue;
        before_write(actor);actor=AttackActor{};actor.active=1;actor.position=position;actor.source_side=side;actor.destination_side=1-side;
        actor.behavior=&behaviors[kind];actor.extra_position=extra;actor.parent=parent;actor.time.reset();
        if(!actor.behavior->initialize||actor.behavior->initialize(actor,actions))actor.active=0;
        actor.extra_position=nullptr;actor.parent=nullptr;return &actor;
    }
    before_write(actors[capacity]);return &actors[capacity];
}
void AttackQueue::dispose(AttackActor& a){
    before_write(a);
    if(a.behavior&&a.behavior->dispose)a.behavior->dispose(a,actions);
    a.animations.clear();a.state.reset();
}
bool AttackQueue::update(const FrameTiming& timing,u32 game_flags,u32 first_playfield_flags){
    if((game_flags&0x1800)||(first_playfield_flags&1))return true;
    for(auto& list:draw_lists)list.clear();counts[0]=counts[1]=0;
    for(u32 n=0;n<capacity;++n){
        auto& actor=actors[n];if(!actor.active)continue;
        if(!actor.behavior||!actor.behavior->update)return false;
        if(actor.behavior->update(actor,actions)){dispose(actor);actor.active=0;continue;}
        for(auto& animation:actor.animations)actions.advance_animation(animation);
        if(actor.layer<0||u32(actor.layer)>=draw_lists.size()||actor.source_side<0||actor.source_side>1)return false;
        draw_lists[actor.layer].push_back(&actor);++counts[actor.source_side];actor.time.tick(timing);
    }
    return true;
}
void AttackQueue::clear(){for(u32 n=0;n<capacity;++n){auto& actor=actors[n];if(!actor.active)continue;dispose(actor);actor=AttackActor{};}}
}
