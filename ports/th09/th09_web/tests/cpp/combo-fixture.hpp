#pragma once
#include "../../cpp/game/Combo.hpp"
#include <array>
#include <vector>
namespace combo_test {
using namespace th09;
struct Fixture:ComboActions {
    ComboState state;EclWorldState world;FrameTiming timing;PlayfieldGeometry geometry;float opposing_width=384;i32 side=0,spirits=0;bool blocked=false,boss=false;
    std::vector<std::array<u32,12>> events;std::array<TransferParameters,4096> transfers;u32 next_transfer=0;
    static u32 bits(float f){u32 b;std::memcpy(&b,&f,4);return b;}
    bool rewards_blocked()const override{return blocked;}
    bool opponent_has_boss()const override{return boss;}
    i32 opposing_spirits()const override{return spirits;}
    void attack(i32 type,i32 level)override{events.push_back({0,u32(type),u32(level)});}
    void add_score(i32 score)override{events.push_back({1,u32(score)});}
    void score_popup(const Vec3& p,i32 score,u32 color)override{events.push_back({2,bits(p.x),bits(p.y),bits(p.z),u32(score),color});}
    void character_meter(ComboState& s,const Vec3&)override{events.push_back({3,u32(s.normal_attack),u32(s.spirit_attack),u32(s.character_attack)});}
    TransferParameters* create_transfer(i32 effect,const Vec3& p,const Vec3& control)override{
        events.push_back({4,u32(effect),bits(p.x),bits(p.y),bits(p.z),bits(control.x),bits(control.y),bits(control.z)});
        if(next_transfer>=transfers.size())return nullptr;auto& t=transfers[next_transfer++];t={};return &t;
    }
    Combo system(){return Combo(world,*this,timing,geometry,opposing_width,side);}
};
}
