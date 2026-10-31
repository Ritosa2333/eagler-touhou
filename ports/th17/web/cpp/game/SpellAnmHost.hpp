#pragma once
#include "SpellCard.hpp"
#include "AnmManager.hpp"
#include "EnemyState.hpp"
namespace th17 {
struct SpellCardServices {
    virtual ~SpellCardServices()=default;
    virtual bool spell_background_visible(bool)=0;virtual bool spell_prepare_hud(bool)=0;
    virtual bool spell_title(AnmVm&,const std::string&)=0;virtual bool spell_result(i32,bool)=0;
    virtual bool spell_sound(i32,bool queued)=0;virtual bool spell_trophy(i32)=0;
};
class SpellAnmHost:public SpellCardHost {
    AnmManager& animations;EnemyWorldState& enemies;SpellCardServices& services;
public:
    SpellAnmHost(AnmManager& a,EnemyWorldState& e,SpellCardServices& s):animations(a),enemies(e),services(s){}
    bool background_visible(bool value)override{return services.spell_background_visible(value);}
    bool prepare_hud(bool value)override{return services.spell_prepare_hud(value);}
    u32 create_visual(i32 resource,i32 script)override{return animations.create(resource,script,-1,0);}
    bool interrupt(u32 handle,i32 label)override{return animations.interrupt(handle,label);}
    bool retire(u32& handle)override{return animations.retire(handle);}
    bool text(u32 handle,const std::string& value)override{auto* vm=animations.registry.find(handle);return vm&&services.spell_title(*vm,value);}
    bool child_integer(u32 handle,i32 script,i32 slot,i32 value)override{auto* parent=animations.registry.find(handle);if(!parent||slot<0||slot>=4)return false;auto* child=animations.registry.find_child_script(*parent,script,0);if(!child)return false;child->variables.integers[slot]=value;return true;}
    bool position(u32 handle,const Vec3& value)override{if(auto* vm=animations.registry.find(handle))vm->visual.translation=value;return true;}
    bool boss_position(Vec3& value)override{auto* boss=enemies.boss_at(0);if(!boss)return false;value=boss->motion.position;return true;}
    bool result(i32 bonus,bool failed)override{return services.spell_result(bonus,failed);}
    bool sound(i32 id,bool queued)override{return services.spell_sound(id,queued);}
    bool trophy(i32 id)override{return services.spell_trophy(id);}
};
}
