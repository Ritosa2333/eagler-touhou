#pragma once
#include "PlayerMotion.hpp"
#include "AnmManager.hpp"
namespace th15 {
class PlayerAnmHost final:public PlayerMotionVisuals {
    AnmManager& animations;i32 player_resource,effect_resource;
public:
    AnmVm root;u32 focus_handle=0,barrier_handle=0;std::array<std::array<u32,2>,8> option_handles{};
    PlayerAnmHost(AnmManager& manager,i32 player,i32 effects):animations(manager),player_resource(player),effect_resource(effects){}
    ~PlayerAnmHost(){animations.registry.destroy_tree(root);animations.retire(focus_handle);animations.retire(barrier_handle);for(auto& pair:option_handles)for(auto& handle:pair)animations.retire(handle);}
    bool initialize(){return pose(0);}
    bool focus_begin(bool enabled,float scale)override{
        if(!enabled){if(!animations.interrupt(focus_handle,1))return false;focus_handle=0;return true;}
        if(!focus_handle){focus_handle=animations.create(effect_resource,26,14,0);if(!focus_handle)return false;}
        if(auto* vm=animations.registry.find(focus_handle)){vm->visual.secondary_scale={scale,scale};vm->visual.flags|=8;}else focus_handle=0;return true;
    }
    bool pose(i32 script)override{return animations.bind_template(root,player_resource,script)&&animations.tick_instance(root)>=0;}
    void focus_position(const Vec3& position)override{if(auto* vm=animations.registry.find(focus_handle)){vm->presentation_motion=true;vm->visual.translation=position;}else focus_handle=0;}
    void option_position(u32 i,const Vec3& position)override{for(auto handle:option_handles[i])if(auto* vm=animations.registry.find(handle)){vm->presentation_motion=true;vm->visual.translation=position;}}
    bool option_remove(u32 i)override{for(auto handle:option_handles[i])if(!animations.interrupt(handle,1))return false;return true;}
    bool barrier_position(const Vec3& position)override{if(!animations.registry.find(barrier_handle)){barrier_handle=animations.create(effect_resource,27,14,0);if(!barrier_handle)return false;}if(auto* vm=animations.registry.find(barrier_handle)){vm->presentation_motion=true;vm->visual.translation=position;}return true;}
    bool barrier_remove()override{const bool result=animations.interrupt(barrier_handle,1);barrier_handle=0;return result;}
};
}
