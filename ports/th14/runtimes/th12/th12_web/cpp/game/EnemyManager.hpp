#pragma once
#include "EnemyFrame.hpp"
#include <memory>
#include <unordered_map>
namespace th11 {
struct EnemySpawn {
    Vec3 position{};i32 score=0,drop=0,health=0;u32 mirrored=0,world_coordinates=0;
    i32 integers[4]{};float floats[4]{},extra_floats[4]{};
#if PORT_TITLE == 14 || PORT_TITLE == 16
    u32 parent_instance_id=0;
#endif
};
static_assert(sizeof(EnemySpawn)==0x50+((kPortTitle==14||kPortTitle==16)?4:0));
class EnemyManager {
public:
    EnemyManager(EclProgram&,EnemyEnvironment&,EnemyCommandEnvironment&,EnemyFrameWorld&);
    ~EnemyManager();
    Enemy* spawn(const char*,const EnemySpawn&);
    // TH12's FUN_00412500(id) walks the enemy list at DAT_004b43dc+0x68 and returns 1 when any
    // enemy's +0x27b4 equals the id, else 0. The port keeps the same list and the same id, now in
    // EnemyState::instance_id.
    bool enemy_exists(i32 id)const noexcept;
    EnemyState* th16_boss(u32 slot)const noexcept;
    bool destroy(Enemy&);
    bool reset(EclProgram&);
#if PORT_TITLE == 14
    bool retire();
    bool initialize_empty(EclProgram&);
    bool finish_stage_owner();
    bool owner_present()const noexcept{return commands.th14_owner()==this;}
    const EclProgram* bound_timeline()const noexcept{return program;}
#endif
    bool update();
    bool erase_stage_enemies(bool matching_tag=false,u32 tag=0);
    bool cancel_circle(Vec3 center,float radius,bool reward,bool convert=false);
    bool cancel_beam(Vec3 center,float half_width,bool reward);
    i32 command(EnemyState&,EclContext&,EnemyGlobals&);
    EnemyLink* first=nullptr;EnemyLink* last=nullptr;
    // Retail TH14 4222f0 clears the manager, then reserves instance ID zero.
    u32 count=0,total_created=kPortTitle==14?1u:0u;Timer timer;
    // Retail TH14 manager +e4, initialized by 4224a8.
    u32 spawn_capacity=99999;
    i32 last_error=0;EnemyScriptError script_error;
#if PORT_TITLE == 16
    i32 th16_spawn_failure=0;char th16_failed_spawn[64]{};
    i32 th16_frame_failure[15]{};
#endif
private:
    struct Entry {Enemy enemy{};EnemyScriptServices script;Entry(EnemyEnvironment& w,EnemyCommandEnvironment& c):script(enemy.state,w,c){}};
    EclProgram* program;EnemyEnvironment& environment;EnemyCommandEnvironment& commands;EnemyFrameWorld& world;
    std::unordered_map<Enemy*,std::unique_ptr<Entry>> entries;
    u32 spawn_depth=0;
};
}
