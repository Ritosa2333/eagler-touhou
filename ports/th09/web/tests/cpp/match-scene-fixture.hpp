#pragma once
#include "../../cpp/game/MatchScene.hpp"
#include <vector>
namespace match_scene_test {
using namespace th09;
inline u32 float_bits(float f){u32 n;std::memcpy(&n,&f,4);return n;}
struct Fixture:MatchSceneServices,MatchRuleActions {
    EclWorldState world;MatchRules rules;MatchScene scene;
    std::vector<std::array<i32,8>> events;
    i32 dialogue_id=0,background[2][2]{},transition_code=0;u8 game_over=0,match_complete=0;
    Fixture():rules(world,*this),scene(rules,*this){}
    i32 index(const AnmVm& a){return i32(&a-scene.animations.data());}
    void start_animation(AnmVm& a,OverlayResource r,i32 script)override{events.push_back({0,index(a),i32(r),script});std::memset(&a,0,sizeof(a));a.scriptIndex=i16(script);a.flags=1;a.color1.d3dColor=0xffffffff;a.pos2={float(script*3-10),float(script*2+1),.25f};}
    void set_sprite(AnmVm& a,i32 sprite)override{events.push_back({1,index(a),sprite});a.activeSpriteIndex=i16(sprite);}
    void advance_animation(AnmVm& a)override{events.push_back({2,index(a)});a.intVar0=wrapping_add(a.intVar0,1);}
    void draw_animation(AnmVm& a)override{events.push_back({3,index(a)});}
    void begin_view(i32 n)override{events.push_back({4,n});}
    void outline(const AttackColorVertex* p,u32 n)override{events.push_back({5,p==scene.borders[0].data()?0:1,i32(n)});}
    void number(const Vec3& p,i32 width,i32 value)override{events.push_back({6,signed_bits(float_bits(p.x)),signed_bits(float_bits(p.y)),width,value});}
    void clock(const Vec3& p,i32 stage,i32 minutes,i32 seconds)override{events.push_back({7,signed_bits(float_bits(p.x)),stage,minutes,seconds});}
    i32 dialogue_state()const override{return dialogue_id;}
    void update_dialogue()override{events.push_back({8});}
    void draw_dialogue()override{events.push_back({9});}
    void begin_dialogue(i32 id,i32 flip)override{events.push_back({10,id,flip});dialogue_id=id;}
    void victory_dialogue(i32 side)override{events.push_back({11,side});dialogue_id=0;}
    void play_sound(i32 sound,i32 pan)override{events.push_back({12,sound,pan});}
    void reset_attack_timer(i32 side)override{events.push_back({13,side});}
    void clear_round_hazards(i32 side)override{events.push_back({14,side,0});events.push_back({14,side,1});}
    void flush_combo(i32 side)override{events.push_back({15,side});}
    void background_transition(i32 side,i32 state,i32 frames)override{background[side][0]=state;background[side][1]=frames;}
    void fade(i32 type,i32 duration,u32 color,i32 side)override{events.push_back({16,type,duration,i32(color),side});}
    void fade_hud(i32 side)override{events.push_back({17,side});}
    void restart_round()override{events.push_back({18});scene.reset_round();}
    void transition(SceneTransition t)override{switch(t){case SceneTransition::title:transition_code=1;break;case SceneTransition::next_stage:transition_code=3;break;case SceneTransition::ending:transition_code=9;break;case SceneTransition::game_over:game_over=1;break;case SceneTransition::match_complete:match_complete=1;break;}}
    void record_defeat(i32 n)override{events.push_back({19,n});}
    void reward_enemy(i32,i32)override{}
    void reward_notification(i32)override{}
};
struct Field {u32 original,offset,size;};
#define SCENE_FIELD(original,field) {original,offsetof(MatchScene,field),sizeof(MatchScene::field)}
inline const Field fields[]={SCENE_FIELD(0xe86c,borders),SCENE_FIELD(0xe934,flash_colors),SCENE_FIELD(0xe93c,flash_frames),SCENE_FIELD(0x10958,displayed_music),SCENE_FIELD(0x1095c,phase),SCENE_FIELD(0x10960,ending_frames),SCENE_FIELD(0x10964,winner),SCENE_FIELD(0x11e88,result),SCENE_FIELD(0x11ea4,retry),SCENE_FIELD(0x11ea8,rewards_blocked)};
#undef SCENE_FIELD
}
