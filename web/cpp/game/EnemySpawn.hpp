#pragma once
#include "Types.hpp"
#include <array>
#include <string>
namespace th15 {
struct EnemySpawnRequest {
    std::string routine;Vec3 position{};
    i32 score=0,item=0,life=0;bool mirrored=false,persistent=false;
    std::array<i32,4> integers{};std::array<float,4> floats{},temporary{};u32 parent=0;
};
struct EnemySpawnHost {
    virtual ~EnemySpawnHost()=default;
    virtual bool spawn(const EnemySpawnRequest&)=0;
    virtual bool spawn_at_rate(const EnemySpawnRequest& request,float){return spawn(request);}
    virtual bool boss_exists(i32 slot)const noexcept=0;
    virtual bool clear_field(bool suppress_death_script){return false;}
    virtual bool switch_enemy_routine(u32 identifier,const std::string&){return false;}
    virtual const std::string* failure()const noexcept{return nullptr;}
};
}
