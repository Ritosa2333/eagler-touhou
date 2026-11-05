#include "BombCheckpoint.hpp"
namespace th15 {
bool BombCheckpoint::clone(u32& handle){if(handle)handle=animations.capture(handle,bomb.context.animations.registry);if(!animations.error.empty()){error=animations.error;return false;}return true;}
bool BombCheckpoint::restore_handle(u32& handle){if(handle)handle=animations.restore(handle,bomb.context.animations.registry);if(!animations.error.empty()){error=animations.error;return false;}return true;}
bool BombCheckpoint::retire(u32& handle){if(bomb.context.animations.retire(handle))return true;error=bomb.context.animations.error;return false;}
bool BombCheckpoint::capture(){
    error.clear();available=false;position=bomb.position;angle=bomb.angle;state=bomb.context.session.bomb_state;
    age.set(bomb.age.current);secondary_age.set(bomb.secondary_age.current);effective=bomb.effective_against_spell;
    first_handle=first?*first:0;aura_handle=aura;saved_charges=charges?*charges:0;
    if(state){
        if(reimu){if(!reimu->orbs_available){error="Active Reimu bomb has no orbs";return false;}orbs=reimu->orbs;if(!clone(aura_handle))return false;for(auto& orb:orbs){if(!clone(orb.animation))return false;orb.archived=true;}}
        else if(!clone(first_handle)||!clone(aura_handle))return false;
    }
    available=true;return true;
}
bool BombCheckpoint::restore(){
    error.clear();if(!available){error="No saved bomb chapter";return false;}
    const bool current_active=bomb.context.session.bomb_state!=0;
    // Marisa's original restore retires its two handles even when inactive.
    if(reimu&&current_active){for(auto& orb:reimu->orbs)if(!retire(orb.animation))return false;if(!retire(aura))return false;reimu->orbs_available=false;}
    else if(!reimu&&(current_active||kind==Kind::Marisa)){if(!retire(*first)||!retire(aura))return false;}
    bomb.position=position;bomb.angle=angle;bomb.context.session.bomb_state=state;
    bomb.age.set(age.current);bomb.secondary_age.set(secondary_age.current);bomb.effective_against_spell=effective;
    if(charges)*charges=saved_charges;if(first)*first=first_handle;aura=aura_handle;
    if(state){
        if(reimu){reimu->orbs=orbs;reimu->orbs_available=true;for(auto& orb:reimu->orbs){if(!restore_handle(orb.animation))return false;orb.archived=false;}if(!restore_handle(aura))return false;}
        else if(!restore_handle(*first)||!restore_handle(aura))return false;
    }
    return true;
}
}
