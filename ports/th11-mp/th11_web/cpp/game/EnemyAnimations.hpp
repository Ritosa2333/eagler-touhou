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
    i32 command(EnemyState&,EclContext&,EnemyGlobals&);
    void visible(u32 id,bool value);
    void interrupt(u32 id,i16 value);
    void erase(u32 id);
    void position(u32 id,Vec3 value,bool playfield);
    i32 sprite(u32 id);
    AnmVm* lookup(u32& id){return find(id);}
    bool rebind(u32& id,i32 script);
    bool update_direction(EnemyState&);
    void update_positions(EnemyState&);
    u32 effect(i32 file,i32 script,u32 layer);
    u32 effect_at(i32 file,i32 script,u32 layer,Vec3 position,bool playfield);
private:
    AnmManager& manager;
    AnmVm* find(u32& id);
    AnmVm* create(EnemyState&,u32 slot,i32 script,u32 layer);
    void size(EnemyState&,const AnmVm&);
};
}
