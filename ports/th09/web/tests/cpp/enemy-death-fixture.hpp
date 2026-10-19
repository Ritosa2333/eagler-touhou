#pragma once
#include "../../cpp/game/EnemyDeath.hpp"
#include "../../cpp/game/ChargeGauge.hpp"
#include <vector>
namespace enemy_death_test {
using namespace th09;
struct Fixture:EnemyDeathActions {
    EclVm enemy;EclWorldState world;EclPlayfieldState field;PlayfieldGeometry geometry;float opposing_width=384;
    i32 count=0,opponent_count=0;ChargeGauge gauge;TransferParameters transfer;
    std::vector<std::array<u32,12>> events;
    Fixture(){enemy.values.world=&world;enemy.values.field=&field;}
    static u32 bits(float x){u32 n;std::memcpy(&n,&x,4);return n;}
    void reset_chain()override{events.push_back({0});count=0;}
    i32 chain_count()const override{return count;}
    void chain_kill(const Vec3& p,i32 normal,i32 spirit,i32 special,i32 score)override{events.push_back({1,bits(p.x),bits(p.y),bits(p.z),u32(normal),u32(spirit),u32(special),u32(score)});++count;}
    void play_positioned_sound(i32 id,float x)override{events.push_back({2,u32(id),bits(x)});}
    void effect(i32 type,const Vec3& p)override{events.push_back({3,u32(type),bits(p.x),bits(p.y),bits(p.z)});}
    void explosion(const Vec3& p,float size,float speed,i32 kind,i32 sprite,i32 color)override{events.push_back({4,bits(p.x),bits(p.y),bits(p.z),bits(size),bits(speed),u32(kind),u32(sprite),u32(color)});}
    void drop_item(i32 type,const Vec3& p)override{events.push_back({5,u32(type),bits(p.x),bits(p.y),bits(p.z)});}
    void charge(float amount)override{events.push_back({6,bits(amount)});const i32 level=gauge.add(amount,field.character);if(level>=0)events.push_back({7,u32(level)});}
    i32 opposing_spirits()const override{return opponent_count;}
    TransferParameters* create_transfer(i32 effect,const Vec3& p,const Vec3& control)override{events.push_back({8,u32(effect),bits(p.x),bits(p.y),bits(p.z),bits(control.x),bits(control.y),bits(control.z)});return &transfer;}
};
}
