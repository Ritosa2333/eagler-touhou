#pragma once
#include "SpellCard.hpp"
#include "AnmManager.hpp"
#include "EnemyState.hpp"
namespace th15 {
struct SpellCardServices {
    virtual ~SpellCardServices()=default;
    virtual bool spell_background_visible(bool){return false;}
    virtual bool spell_prepare_hud(bool){return false;}
    virtual bool spell_title(AnmVm&,const std::string&){return false;}
    virtual bool spell_history_begin(i32,const std::string&){return false;}
    virtual bool spell_history_capture(i32){return false;}
    virtual bool spell_result(i32,bool){return false;}
    virtual bool spell_sound(i32){return false;}
};
// Actual ANM objects and child scripts are used for spell presentation. Text,
// records and scene flags connect through the session's named services.
class SpellAnmHost final:public SpellCardHost {
    AnmManager& animations;EnemyWorldState& enemies;SpellCardServices& services;
public:
    SpellAnmHost(AnmManager& a,EnemyWorldState& e,SpellCardServices& s):animations(a),enemies(e),services(s){}
    bool background_visible(bool v)override{return services.spell_background_visible(v);}
    bool prepare_hud(bool start)override{return services.spell_prepare_hud(start);}
    u32 create_visual(i32 resource,i32 script)override{return animations.create(resource,script,-1,0);}
    bool interrupt(u32 handle,i32 label)override{return animations.interrupt(handle,label);}
    bool retire(u32& handle)override{return animations.retire(handle);}
    bool text(u32 handle,const std::string& value)override{auto* vm=animations.registry.find(handle);return vm&&services.spell_title(*vm,value);}
    bool child_integer(u32 handle,i32 script,i32 slot,i32 value)override{auto* parent=animations.registry.find(handle);if(!parent||slot<0||slot>=4)return false;auto* child=animations.registry.find_child_script(*parent,script,0);if(!child)return false;child->variables.integers[slot]=value;return true;}
    bool position(u32 handle,const Vec3& p)override{if(auto* vm=animations.registry.find(handle))vm->visual.translation=p;return true;}
    bool boss_position(Vec3& p)override{auto* boss=enemies.boss_at(0);if(!boss)return false;p=boss->motion.position;return true;}
    bool history_begin(i32 id,const std::string& title)override{return services.spell_history_begin(id,title);}
    bool history_capture(i32 id)override{return services.spell_history_capture(id);}
    bool result(i32 bonus,bool failed)override{return services.spell_result(bonus,failed);}
    bool sound(i32 id)override{return services.spell_sound(id);}
};
}
