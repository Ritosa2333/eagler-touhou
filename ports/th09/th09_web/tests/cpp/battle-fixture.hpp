#pragma once
#include "../../cpp/game/GameBattle.hpp"
#include <map>
namespace battle_test {
using namespace th09;
struct Fixture:ResourceReader,ResourceTextures,BattlePresentation,MatchRuleActions {
    EclWorldState world;AnmExecutor animations{world.random};GameResources resources{*this,*this,animations};MatchRules rules{world,*this};
    std::map<std::string,std::vector<u8>> files;std::unique_ptr<GameBattle> battle;std::vector<u32> snapshot;AnmVm portraits[2][2]{};u32 texture_id=0,draws=0,events=0;
    bool read(const char* n,std::vector<u8>& out)override{auto i=files.find(n);if(i==files.end())return false;out=i->second;return true;}
    TextureAllocation create(const AnmTextureSource& s,const u8*,u32)override{return {++texture_id,s.width,s.height};}
    void destroy(u32)override{}
    void sound(i32,i32)override{++events;}
    void play_sound(i32 id,i32 pan)override{sound(id,pan);}
    void positioned_sound(i32,i32,float)override{++events;}
    void begin_field(i32)override{}
    void animation(AnmVm&,BattleSprite)override{++draws;}
    void colored(const AnmVm*,const AttackColorVertex*,u32,BattleGeometry,bool)override{++draws;}
    void textured(const AnmVm&,const AttackTextureVertex*,u32,BattleGeometry)override{++draws;}
    void text(AnmVm&,const char*,u32,u32)override{++events;}
    void score_popup(i32,const Vec3&,i32,u32)override{++events;}
    void notice(i32,BattleNotice,i32)override{++events;}
    void damage_flash(i32,i32,u32)override{++events;}
    void end_round(i32)override{battle->state.scene_phase=1;battle->state.rewards_blocked=true;}
    void finish_match()override{battle->state.scene_phase=2;}
    void attack_position(i32,const Vec3&)override{}
    void boss_indicator(i32,i32,i32)override{}
    void boss_indicator_position(i32,i32,const Vec3&)override{}
    void background_setting(i32,i32)override{}
    void background_transition(i32,i32,i32)override{}
    void reset_background(i32)override{}
    void boss_background(i32)override{}
    void portrait(i32 side,u32 layer,i32 script)override{resources.start(character_animation(side),portraits[side][layer],script);}
    void reward_enemy(i32 side,i32 reward)override{battle->reward_enemy(side,reward);}
    void reward_notification(i32)override{++events;}
    bool initialize(i32 left,i32 right,i32 seed,i32 difficulty,i32 controller){
        battle.reset();animations.invalid=false;world.random={u16(seed),0,0};rules.initialize(difficulty,GameMode::versus,0);rules.scores={};
        const i32 chars[]{left,right};const bool alternate[]{false,false};if(!resources.load_common()||!resources.load_match(0,chars,alternate,false))return false;
        battle=std::make_unique<GameBattle>(world,rules,resources,animations,*this);battle->configure(left,right);
        if(!battle->initialize_field(0,controller,10)||!battle->initialize_field(1,controller,10)||!battle->initialize_shared())return false;
        for(auto& f:battle->fields){f.cpu_level=20;f.player->control.player_state=0;f.player->control.protection.reset();f.player->motion.position={0,384,.49f};}battle->sync_players();return true;
    }
    bool step(u16 left,u16 right,u32 mask){
        auto& b=*battle;b.fields[0].player->input.advance(left);b.fields[1].player->input.advance(right);b.sync_players();
        for(i32 s=0;s<2;++s)if(mask&1)if(!b.update_enemies(s))return false;
        for(i32 s=0;s<2;++s)if(mask&2)if(!b.update_bullets(s))return false;
        if(mask&4)if(!b.update_attacks())return false;
        for(i32 s=0;s<2;++s)if(mask&8)b.update_player(s);
        for(i32 s=0;s<2;++s)if(mask&16)b.update_controller(s);
        for(i32 s=0;s<3;++s)if(mask&32)b.update_effects(s);
        return b.error.empty()&&!animations.invalid;
    }
    void* part(u32 side,u32 kind){auto& f=battle->fields[side];auto& p=*f.player;switch(kind){case 0:return &p.motion;case 1:return &p.control;case 2:return &p.body;case 3:return &p.input;case 4:return &p.cpu;case 5:return &p.combo_state;case 6:return &world;case 7:return &f.script;case 8:return &p.hazards;case 9:return p.shots.areas.pool.data();case 10:return p.items.items.data();case 11:return &p.shots.target;case 12:return &p.secondary_target;case 13:return p.attack_levels;case 14:return &p.items.attraction;case 15:return &p.shots.beam_time;case 16:return battle->patterns.data();}return nullptr;}
    const u32* values(){snapshot.clear();snapshot.push_back(world.random.seed);snapshot.push_back(world.random.calls);snapshot.push_back(battle->state.flags);snapshot.push_back(battle->state.scene_phase);
        for(auto& f:battle->fields){auto& e=*f.enemies;auto& b=*f.bullets;snapshot.insert(snapshot.end(),{u32(e.alive),u32(e.normal_alive),u32(e.attack_alive),u32(e.spirit_alive),u32(e.timeline.time.current),u32(e.timeline.wait.current),u32(e.timeline.instruction_offset()),u32(e.pattern_index),u32(b.total),u32(b.first_count),u32(b.second_count),u32(f.spells),u32(f.bosses),u32(f.counters),u32(rules.scores[f.script.side].points)});
        }return snapshot.data();}
};
}
