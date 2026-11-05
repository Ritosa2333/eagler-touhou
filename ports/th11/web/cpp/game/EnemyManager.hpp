#pragma once
#include "EnemyFrame.hpp"
#include <memory>
#include <unordered_map>
namespace th11 {
struct EnemySpawn {
    Vec3 position{};i32 score=0,drop=0,health=0;u32 mirrored=0,world_coordinates=0;
    i32 integers[4]{};float floats[4]{},extra_floats[4]{};
};
static_assert(sizeof(EnemySpawn)==0x50);
class EnemyManager {
public:
    EnemyManager(EclProgram&,EnemyEnvironment&,EnemyCommandEnvironment&,EnemyFrameWorld&);
    ~EnemyManager();
    Enemy* spawn(const char*,const EnemySpawn&);
    bool destroy(Enemy&);
    bool reset(EclProgram&);
    bool update();
    bool erase_stage_enemies(bool matching_tag=false,u32 tag=0);
    bool cancel_circle(Vec3 center,float radius,bool reward,bool convert=false);
    bool cancel_beam(Vec3 center,float half_width,bool reward);
    i32 command(EnemyState&,EclContext&,EnemyGlobals&);
    EnemyLink* first=nullptr;EnemyLink* last=nullptr;
    u32 count=0,total_created=0;Timer timer;
    i32 last_error=0;EnemyScriptError script_error;
private:
    struct Entry {Enemy enemy{};EnemyScriptServices script;Entry(EnemyEnvironment& w,EnemyCommandEnvironment& c):script(enemy.state,w,c){}};
    EclProgram* program;EnemyEnvironment& environment;EnemyCommandEnvironment& commands;EnemyFrameWorld& world;
    std::unordered_map<Enemy*,std::unique_ptr<Entry>> entries;
    u32 spawn_depth=0;
};
}
