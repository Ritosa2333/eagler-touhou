#pragma once
#include "AnmVm.hpp"
#include <array>
#include <algorithm>
namespace th15::multiplayer {class BattleFrameState;}
namespace th15::presentation {
inline bool always_hitbox=false;
// Run the stock focus scripts on private VMs. This host owns only their child
// relationship, never the game's animation registry, effects or random source.
class HitboxDisplay final:private AnmObjectHost {
    friend class th15::multiplayer::BattleFrameState;
    std::array<AnmVm,2> markers;
    const AnmVm* child_template=nullptr;
    AnmEnvironment environment;Rng random;
    float previous=-1,rate=1;bool child_active=false;
    void bind(AnmVm& marker,const AnmVm& source){
        marker=source;marker.environment=&environment;marker.object_host=this;
        marker.rotation_parent=marker.creation_parent=nullptr;
        marker.pending_interrupt=0;marker.slowdown=0;marker.reset_runtime_timers();
    }
    AnmVm* spawn_child(AnmVm& parent,i32 script,u32)override{
        if(!child_template||script!=child_template->source_script||child_active)return nullptr;
        auto& child=markers[0];bind(child,*child_template);child.visual.translation={};
        child.creation_parent=child.rotation_parent=&parent;child_active=true;
        return child.tick(random,rate)<0?nullptr:&child;
    }
    AnmVm* spawn_detached(AnmVm&,i32)override{return nullptr;}
    bool spawn_effect(AnmVm&,i32)override{return false;}
    bool release_tree(AnmVm&)override{reset();return true;}
public:
    void reset(){previous=-1;child_active=false;}
    std::array<AnmVm,2>& sample(const AnmVm& child,const AnmVm& root,const Vec3& position,float scale,float frame,u64 owner,float initial_rate=1){
        if(previous>=0&&root.environment){
            const auto local_random=environment.game_rng;
            environment=*root.environment;environment.game_rng=local_random;
        }
        if(previous<0||frame<previous){
            environment=root.environment?*root.environment:AnmEnvironment{};
            random={};child_template=&child;child_active=false;rate=initial_rate;
            bind(markers[1],root);markers[1].visual.render_flags|=0x40400;
            markers[1].visual.layer=14;markers[1].variables.rotation.z=0;
            markers[1].tick(random,rate);previous=frame;
        }else{
            for(float remaining=std::max(0.f,frame-previous);remaining>0;remaining-=rate){
                rate=std::min(1.f,remaining);
                if(child_active)markers[0].tick(random,rate);
                markers[1].tick(random,rate);
            }
            previous=frame;
        }
        for(unsigned i=0;i<2;++i){auto& marker=markers[i];
            marker.visual.translation=i==0?Vec3{}:position;
            marker.visual.secondary_scale=i==0?Vec2{1,1}:Vec2{scale,scale};marker.visual.flags|=12;
            marker.presentation_motion=true;marker.presentation_generation=owner;marker.presentation_part=0x686200+i;
        }
        return markers;
    }
};
}
