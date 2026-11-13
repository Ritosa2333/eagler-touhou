#pragma once
#include "EnemyState.hpp"
#include "AnmManager.hpp"
#include <array>
namespace th11 {
class EnemyAnimations {
public:
    explicit EnemyAnimations(AnmManager& manager):manager(manager){}
    struct Resource {AnmResource* file=nullptr;u16 id=0;};
    std::array<Resource,16> resources{};
    Resource stage_logo_resource;
#if PORT_TITLE == 16
    void* th16_registration_user=nullptr;
    bool (*th16_register_effect)(void*,u32)=nullptr;
#endif
    i32 command(EnemyState&,EclContext&,EnemyGlobals&);
#if PORT_TITLE == 14
    NativeStackTh14& native_stack()noexcept{return manager.th14_native_stack;}
#endif
    void visible(u32 id,bool value);
    void interrupt(u32 id,i16 value);
    // TH14 47f1b0 also executes each VM immediately, without unregistering it.
    bool interrupt_now(u32 id,i32 value);
    void erase(u32 id);
    void position(u32 id,Vec3 value,bool playfield);
    i32 sprite(u32 id);
    AnmVm* lookup(u32& id){return find(id);}
    bool rebind(u32& id,i32 script);
    bool update_direction(EnemyState&);
    bool update_positions(EnemyState&);
    bool th14_stage_logo(i32 runtime_mode,u32 run_flags);
    bool th16_stage_logo(i32 runtime_mode,u32 run_flags);
    u32 effect(i32 file,i32 script,u32 layer);
    u32 effect_at(i32 file,i32 script,u32 layer,Vec3 position,bool playfield);
    u32 th14_death_effect(const EnemyState&);
private:
    AnmManager& manager;
    AnmVm* find(u32& id);
    AnmVm* create(EnemyState&,u32 slot,i32 script,u32 layer);
    void size(EnemyState&,const AnmVm&);
};
}
