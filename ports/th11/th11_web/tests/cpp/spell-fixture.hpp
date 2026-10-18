#pragma once
#include "../../cpp/game/SpellController.hpp"
#include "../../cpp/game/EnemyFrame.hpp"
namespace th11::test {
struct SpellFixture:EnemyFrameWorld,SpellEffects {
    float rate=1;GameEconomy economy;SpellRecords records;SpellController controller;
    u32 stage_flags=1;Vec3 positioned{};std::vector<i32> events;
    SpellFixture():controller(*this,economy,*this,records,&rate){}
    void clear_shot_targets(Enemy*)override{}
    bool spell_begin_visuals(i32 id,i32 timeout,const char*)override{events.insert(events.end(),{0,id,timeout});return true;}
    bool spell_update_visuals()override{events.insert(events.end(),{1,0,0});return true;}
    void spell_background(bool hidden)override{stage_flags=hidden?stage_flags|1:stage_flags&~1u;}
    void spell_title_interrupt(i16 label)override{events.insert(events.end(),{2,label,0});}
    void spell_circle_position(Vec3 p)override{positioned=p;}
    void spell_circle_end()override{events.insert(events.end(),{3,0,0});}
    bool spell_result(bool captured,i32 bonus)override{events.insert(events.end(),{4,captured?0:1,bonus});return true;}
    bool spell_sound(i32 id)override{events.insert(events.end(),{5,id,0});return true;}
};
}
