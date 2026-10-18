#pragma once
#include "../../cpp/game/HeadsUpDisplay.hpp"
namespace hud_test {
using namespace th09;
struct Fixture:ResourceReader,ResourceTextures,HudPresentation {
    std::vector<u8> bytes;Rng random;AnmExecutor executor{random};GameResources resources{*this,*this,executor};
    std::unique_ptr<HeadsUpDisplay> hud;HudFrame frame;std::vector<std::array<i32,2>> draws;std::vector<AttackColorVertex> vertices;u32 textures=0;
    bool read(const char*,std::vector<u8>& out)override{out=bytes;return true;}
    TextureAllocation create(const AnmTextureSource& s,const u8*,u32)override{return {++textures,s.width,s.height};}
    void destroy(u32)override{}
    void begin_hud(i32)override{draws.clear();vertices.clear();}
    void hud_animation(AnmVm& vm,bool rotated)override{for(i32 n=0;n<63;++n)if(&vm==&hud->animations[n]){draws.push_back({n,i32(rotated)});return;}for(i32 n=0;n<2;++n)if(&vm==&hud->portraits[n])draws.push_back({63+n,i32(rotated)});}
    void hud_triangle(const AttackColorVertex* p)override{vertices.insert(vertices.end(),p,p+3);}
    bool initialize(const u8* p,u32 size,i32 side,bool versus){bytes.assign(p,p+size);if(!resources.load(AnimationFile::front,"front.anm"))return false;frame.versus=versus;frame.geometry={u32(16+side*320),16,{-144,0},288};random={0x7513,0,0};hud=std::make_unique<HeadsUpDisplay>(resources,*this,side);hud->initialize(versus);return !executor.invalid;}
    void input(const u32* v){frame.health=i32(v[0]);frame.combo.hits=i32(v[1]);frame.combo.display_score=i32(v[2]);frame.score.displayed=v[3];frame.wins=i32(v[4]);frame.spell_level=i32(v[5]);frame.boss_level=i32(v[6]);std::memcpy(&frame.charge,v+7,4);std::memcpy(&frame.available,v+8,4);std::memcpy(&frame.score.lives,v+9,4);frame.combo.chain_time.reset(i32(v[10]));std::memcpy(&frame.combo.display_time.time,v+11,4);std::memcpy(&frame.player.x,v+12,4);std::memcpy(&frame.player.y,v+13,4);}
    void operation(i32 type,i32 value){switch(type){case 0:hud->ready();break;case 1:hud->begin_charge();break;case 2:hud->end_charge();break;case 3:hud->charge_level(value);break;case 4:hud->health_notice(value);break;case 5:hud->begin_survival(value);break;case 6:hud->survival_time(value);break;case 7:hud->survival_expired();break;case 8:hud->close();break;case 9:hud->open();break;case 10:hud->portrait(value&1,7+value,AnimationFile::front);break;}}
};
}
