#pragma once
#include "AnmManager.hpp"
#include "AnmRenderer.hpp"
#include <array>
namespace th17::presentation {
inline bool always_hitbox=false;
// Private stock animation instances keep this display option outside the
// game's registry, random streams, movement and recorded input.
class HitboxDisplay final:private AnmObjectHost {
    std::array<AnmVm,16> markers{};
    std::array<bool,16> active{};
    AnmEnvironment environment;Rng random;
    AnmManager* manager=nullptr;i32 bank=0,previous=-1;
    bool bind(unsigned slot,i32 script){
        auto* source=manager->script_template(bank,script);if(!source)return false;
        auto& vm=markers[slot];vm=*source;vm.environment=&environment;vm.object_host=this;
        vm.creation_parent=vm.rotation_parent=nullptr;vm.pending_interrupt=0;
        vm.slowdown=0;vm.reset_runtime_timers();active[slot]=true;return true;
    }
    AnmVm* spawn_child(AnmVm& parent,i32 script,u32)override{
        for(unsigned i=1;i<markers.size();++i)if(!active[i]){
            if(!bind(i,script))return nullptr;auto& vm=markers[i];
            vm.creation_parent=vm.rotation_parent=&parent;vm.visual.translation={};
            if(vm.tick(random,1)<0){active[i]=false;return nullptr;}return &vm;
        }return nullptr;
    }
    AnmVm* spawn_detached(AnmVm&,i32)override{return nullptr;}
    bool spawn_effect(AnmVm&,i32)override{return false;}
    bool release_tree(AnmVm& vm)override{for(unsigned i=0;i<markers.size();++i)if(&markers[i]==&vm)active[i]=false;return true;}
public:
    void reset(){previous=-1;active.fill(false);}
    bool draw(AnmManager& owner,i32 effect,AnmRenderer& renderer,const Vec3& position,float scale,i32 frame){
        manager=&owner;bank=effect;
        if(previous<0||frame<previous){
            reset();environment=owner.scene_environment();random={};
            if(!bind(0,26))return false;
            auto& root=markers[0];root.visual.render_flags|=0x40400;root.visual.layer=14;
            if(root.tick(random,1)<0)return false;previous=frame;
        }
        while(previous<frame){for(unsigned i=0;i<markers.size();++i)if(active[i]&&markers[i].tick(random,1)<0)return false;++previous;}
        markers[0].visual.translation=position;markers[0].visual.secondary_scale={scale,scale};markers[0].visual.flags|=12;
        for(unsigned i=0;i<markers.size();++i)if(active[i]&&renderer.draw(markers[i])==-2)return false;
        return true;
    }
};
}
