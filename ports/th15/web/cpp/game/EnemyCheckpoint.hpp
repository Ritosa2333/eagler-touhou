#pragma once
#include "EnemyManager.hpp"
#include "AnmCheckpoint.hpp"
#include "AnmManager.hpp"
namespace th15 {
class EnemyCheckpoint {
    struct Entry {EnemyState state;EclThreadsSnapshot scripts;};
    EnemyManager& manager;AnmManager& animations;AnmCheckpoint& pool;
    std::array<i32,6> banks;std::vector<Entry> entries;
    std::array<i32,4> integer_registers{};std::array<float,8> float_registers{};
    std::array<i32,3> counters{};std::array<u32,3> boss_ids{};
    u32 next_id=1,total=0,manager_flags=0;i32 control=0,shot_damage=0,other_damage=0;Timer timer;bool available=false;
public:
    std::string error;
    EnemyCheckpoint(EnemyManager& m,AnmManager& a,AnmCheckpoint& p,const std::array<i32,6>& resources):manager(m),animations(a),pool(p),banks(resources){}
    bool capture();bool restore();bool ready()const noexcept{return available;}
    bool write_file(std::vector<u8>&);bool read_file(const u8*,u32,u32& consumed);
    u32 count()const noexcept{return entries.size();}
    const EnemyState* state(u32 i)const noexcept{return i<entries.size()?&entries[i].state:nullptr;}
};
}
