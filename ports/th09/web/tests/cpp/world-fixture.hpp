#pragma once
#include "../../cpp/game/GameWorld.hpp"
#include <map>
namespace world_test {
using namespace th09;
inline u32 float_bits(float value){u32 bits;std::memcpy(&bits,&value,4);return bits;}
struct Fixture:ResourceReader,ResourceTextures,WorldPresentation {
    EclWorldState world;AnmExecutor animations{world.random};GameResources resources{*this,*this,animations};
    std::map<std::string,std::vector<u8>> files;std::unique_ptr<GameWorld> game;u32 texture_id=0;
    std::vector<u8> text_bytes;std::vector<std::array<i32,3>> events;
    bool read(const char* n,std::vector<u8>& out)override{auto i=files.find(n);if(i==files.end())return false;out=i->second;return true;}
    TextureAllocation create(const AnmTextureSource& s,const u8*,u32)override{return {++texture_id,s.width,s.height};}
    void destroy(u32)override{}
    void begin_field(i32)override{}
    void animation(AnmVm&,BattleSprite)override{}
    void colored(const AnmVm*,const AttackColorVertex*,u32,BattleGeometry,bool)override{}
    void textured(const AnmVm&,const AttackTextureVertex*,u32,BattleGeometry)override{}
    void text(AnmVm&,const char* value,u32 color,u32 shadow)override{events.push_back({0,signed_bits(color),signed_bits(shadow)});for(;*value;++value)text_bytes.push_back(u8(*value));text_bytes.push_back(0);}
    void sound(i32 id,i32 pan)override{events.push_back({1,id,pan});}
    void positioned_sound(i32 side,i32 id,float x)override{events.push_back({2,id,signed_bits(float_bits(x))});}
    void music(i32 track)override{events.push_back({3,track,0});}
    void fade_music()override{events.push_back({4,0,0});}
    void background(Background&,const Background&,i32,bool)override{}
    void score_popup(i32,const Vec3&,i32,u32)override{}
    void number(const Vec3&,i32,i32)override{}
    void clock(const Vec3&,i32,i32,i32)override{}
    void shake(i32,float,float)override{}
    void rectangle(float,float,float,float,u32,bool)override{}
    void encountered(i32 ch)override{events.push_back({5,ch,0});}
    void defeated(i32 ch)override{events.push_back({6,ch,0});}
    bool initialize(i32 left,i32 right,i32 seed,i32 mode,i32 controller){
        game.reset();animations.invalid=false;world.random={u16(seed),0,0};
        WorldConfiguration config;auto& s=config.selection;s.characters[0]=left;s.characters[1]=right;s.mode=GameMode(mode);s.difficulty=1;s.stage=0;s.round=0;s.selector=left;s.lives=2;
        config.controllers[1]=controller;game=std::make_unique<GameWorld>(world,resources,animations,*this);return game->initialize(config);
    }
    void clear(){events.clear();text_bytes.clear();}
    void* part(u32 side,u32 kind){return part_of(*game,side,kind);}
    void* part_of(GameWorld& selected,u32 side,u32 kind){auto* game=&selected;auto& f=game->battle->fields[side];auto& p=*f.player;switch(kind){case 0:return &p.motion;case 1:return &p.control;case 2:return &p.body;case 3:return &p.input;case 4:return &p.cpu;case 5:return &p.combo_state;case 6:return &world;case 7:return &f.script;case 8:return &p.hazards;case 9:return p.shots.areas.pool.data();case 10:return p.items.items.data();case 11:return &p.shots.target;case 12:return &p.secondary_target;case 13:return p.attack_levels;case 14:return &p.items.attraction;case 15:return &p.shots.beam_time;case 16:return game->battle->patterns.data();case 17:return game->scene.get();case 18:return game->dialogue.get();case 19:return &game->rules.progress;case 20:return game->rules.scores.data();case 21:return &game->configuration.selection;case 22:return game->backgrounds[side].get();case 23:return &game->backgrounds[side]->camera;case 24:return game->scene->animations.data();case 25:return game->dialogue->animations.data();case 26:return &game->huds[side]->wipe;case 27:return &game->huds[side]->blink;}return nullptr;}
};
}
