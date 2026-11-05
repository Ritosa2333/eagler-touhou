#pragma once
#include "AnmRegistry.hpp"
#include <functional>
namespace th15 {
// Resource templates and live animations share the same typed instruction
// engine. Graphics storage belongs to the portable renderer, not this manager.
class AnmManager final:public AnmObjectHost {
    struct Bank {AnmResource data;std::vector<AnmVm> templates;};
    std::unordered_map<i32,std::unique_ptr<Bank>> banks;
    std::vector<std::unique_ptr<Bank>> retired_banks;
    Rng& random;AnmEnvironment& environment;
    AnmVm* instantiate(AnmResource&,i32);
    bool initial_tick(AnmVm&);
public:
    AnmRegistry registry;std::string error;float rate=1;
    std::unordered_map<i32,std::string> resource_names;
    const std::unordered_map<i32,i32>* checkpoint_banks=nullptr;
    i32 checkpoint_bank(i32 id)const noexcept{if(!checkpoint_banks)return id;auto at=checkpoint_banks->find(id);return at==checkpoint_banks->end()?-1:at->second;}
    bool name_resource(i32 id,const std::string& name){if(!resource(id)||name.empty()||name.size()>1024)return false;resource_names[id]=name;return true;}
    std::function<bool(AnmVm&,i32)> effect;
    std::function<i32(AnmVm&,float)> effect_update;
    std::function<bool(AnmVm&,i32,float)> effect_interrupt;
    AnmManager(Rng& random,AnmEnvironment& environment):random(random),environment(environment){}
    AnmEnvironment& scene_environment()noexcept{return environment;}
    void set_paused(bool paused)noexcept{environment.paused=paused;}
    bool load(i32 id,const u8*,u32 size);
    void unload(i32 id);void collect_resources();
    u32 resource_count()const noexcept{return banks.size();}
    u32 retired_resource_count()const noexcept{return retired_banks.size();}
    std::function<void(const AnmResource&)> resource_release;
    AnmResource* resource(i32 id)const noexcept;
    i32 resource_id(const AnmResource* value)const noexcept;
    bool sprite_fallback(i32 id)noexcept{auto* bank=resource(id);if(!bank)return false;environment.fallback_sprite_resource=bank;return true;}
    AnmVm* script_template(i32 id,u32 script)const noexcept;
    bool bind_template(AnmVm&,i32 id,i32 script);
    u32 create(i32 resource,i32 script,i32 layer=-1,u32 ordering=2,const Vec3& translation={},float rotation_z=0);
    u32 create_overlay(i32 resource,i32 script);
    AnmVm* spawn_child(AnmVm&,i32 script,u32 ordering)override;
    AnmVm* spawn_detached(AnmVm&,i32 script)override;
    bool spawn_effect(AnmVm&,i32 script)override;
    bool release_tree(AnmVm& vm)override{return registry.destroy_tree(vm);}
    i32 update_effect(AnmVm&,float rate)override;
    bool interrupt_effect(AnmVm&,i32 label,float rate)override;
    bool rebind(u32& handle,i32 script);
    bool retire(u32& handle);
    void retire_resource(i32 id)noexcept{if(auto* bank=resource(id))registry.retire_resource(bank);}
    bool pause(u32 handle,bool paused=true);
    bool interrupt(u32 handle,i32 label,bool immediate=false);
    bool update(bool alternate);
    int tick_instance(AnmVm&);
};
}
