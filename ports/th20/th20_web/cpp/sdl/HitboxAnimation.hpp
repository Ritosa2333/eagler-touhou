#pragma once
#include "../../../source_reconstruction/sprite_renderer/binding.hpp"
#include "../../../source_reconstruction/sprite_renderer/anm_vm.hpp"
#include <array>
namespace th20::web {
// effect.anm 18/19 use the native scale/alpha/rotation interpolators, followed
// by one angular-velocity command at time 18. Advance only these private VMs.
class HitboxAnimation {
    std::array<source::sprite::Animation,2> markers{};
    float previous=-1,age=0;
public:
    void reset(){previous=-1;}
    const std::array<source::sprite::Animation,2>& sample(source::sprite::AnimationFile& file,float frame){
        using namespace source;
        if(previous<0||frame<previous){
            for(unsigned i=0;i<2;++i){markers[i]={};sprite::select_animation_template(file,markers[i],i?19:18);sprite::set_animation_layer(markers[i],18);}
            age=0;previous=frame-*sprite::anm_environment::timer_rate();
        }
        if(frame>previous){
            // Native animation helpers consume the current simulation rate.
            // At most one update per elapsed native step, regardless of redraws.
            const float step=*sprite::anm_environment::timer_rate();
            if(step>0)for(float remaining=frame-previous;remaining>0.00001f;remaining-=step){
                for(unsigned i=0;i<2;++i){auto& marker=markers[i];
                    if(age>=18){marker.base.vector_44.z=i?.05235988f:-.10471976f;marker.base.flags[1]|=0x80000u;marker.base.flags[2]|=0x100u;}
                    sprite::update_animation_motion(marker);sprite::update_animation_interpolations(marker);
                }
                age+=step;
            }
            previous=frame;
        }
        return markers;
    }
};
}
