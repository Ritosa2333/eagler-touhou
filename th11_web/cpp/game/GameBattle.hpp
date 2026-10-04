#pragma once
#include "Replay.hpp"
#include "ScorePopups.hpp"
#include "SpellTiming.hpp"
#include "GameSessionResources.hpp"
#include "EnemyManager.hpp"
#include "EnemyAnimations.hpp"
#include "BulletManager.hpp"
#include "LaserManager.hpp"
#include "ItemManager.hpp"
#include "ItemRewards.hpp"
#include "AnmRenderer.hpp"
#include "PlayerFrame.hpp"
#include "BombController.hpp"
#include "Communication.hpp"
#include "SceneSchedule.hpp"
#include "SpellController.hpp"
#include "ScreenShake.hpp"
#include "Dialogue.hpp"
#include "ScreenDeformation.hpp"
#include "EnemyCallbacks.hpp"
#include "Stage.hpp"
#include "ScreenFade.hpp"
#include "SceneCompositor.hpp"
#include "StageCompletion.hpp"
#include "Hud.hpp"
#include <memory>
#include <vector>
#include <functional>

namespace th11 {
struct GameSessionInput;
enum class BattleEventKind {Sound,Popup,Notification,Lives,Death,GameOver,SpellTitle,SpellResult,BossMusic,MusicFade,StopSound,StageMusic,MusicResume};
struct BattleEvent {
    BattleEventKind kind;
    i32 value=0,extra=0;
    Vec3 position{};
    u32 color=0;
    bool positional=false;
};

// Live connections between the reconstructed managers. Presentation events are
// retained for the platform consumer; unavailable game logic fails explicitly.
class GameBattle final : public EnemyFrameWorld, public BulletWorld,
                         public LaserWorld, public ItemWorld,
                         public PlayerFrameWorld, public ItemRewardEffects, public SpellEffects,
                         public DialogueEffects, public DialogueControl, public DeformationControl, public BombWorld, public EnemyCallbackWorld, public StageCompletionEffects, public HudEffects {
public:
    PracticeState* practice=nullptr;
    bool practice_replay_has_stage5=false;
    GameSessionResources& resources;
    AnmManager& animations;
    GameEconomy& economy;
    SceneCompositor& compositor;
    std::unique_ptr<Stage> stage;
    std::unique_ptr<Stage> outgoing_stage;
    StageCompletion completion;
    Timer transition_timer;
    bool transitioning=false,transition_started=false,stage_active=true;
    u32 result_animation=0;
    ScreenFades fades;
    EnemyEnvironment enemy_environment;
    EnemyCommandEnvironment enemy_commands;
    Hud hud;
    AsciiText ascii;
    ScorePopups popups;
    SpellTiming spell_timing;
    EnemyAnimations enemy_animations;
    EnemyManager enemies;
    EnemyCallbacks enemy_callbacks;
    BulletManager bullets;
    LaserManager lasers;
    ItemManager items;
    ItemRewards item_rewards;
    std::unique_ptr<PlayerFrame> player;
    std::unique_ptr<BombController> bomb;
    PlayerFrameInput player_input;
    BulletCancelContext cancellation;
    std::vector<BattleEvent> events;
    Communication communication;
    SpellController spells;
    ScreenShakes shakes;
    std::unique_ptr<Dialogue> dialogue;
    std::vector<DialogueText> dialogue_text_requests;
    AnmVm spell_backgrounds[2]{};
    AnmVm callback_shared_visual{};
    u32 spell_titles[3]{},spell_circle=0;
    u32 frame=0;
    bool initialized=false,player_live=true,game_over_requested=false;
    i32 practice_high=0;
    bool replaying=false,demo=false;
    ReplayStageState replay_entry;
    i32 last_error=0;
    std::string error;

    GameBattle(GameSessionResources&,AnmManager&,GameEconomy&,SpellRecords&,SceneCompositor&,ClearRecords&);
    ~GameBattle()override;
    bool initialize(i32 character,i32 subtype,i32 difficulty);
    bool next_stage(GameResources&,u32);
    bool update(const std::function<bool()>& sample_input={});
    bool draw(AnmRenderer&,SceneDrawKind);
    bool spawn_root();
    void set_input(const GameSessionInput&);
    void synchronize_player();
    bool spell_begin_visuals(i32,i32,const char*)override;
    bool spell_update_visuals()override;
    void spell_background(bool visible)override{if(stage)stage->state.draw_flags=visible?stage->state.draw_flags|1:stage->state.draw_flags&~1u;}
    void spell_title_interrupt(i16 label)override{for(auto id:spell_titles)enemy_animations.interrupt(id,label);}
    void spell_circle_position(Vec3 p)override{enemy_animations.position(spell_circle,p,true);}
    void spell_circle_end()override{enemy_animations.erase(spell_circle);spell_circle=0;cancellation={spell_flags,spell_id};}
    bool spell_result(bool captured,i32 bonus)override{events.push_back({BattleEventKind::SpellResult,captured?0:1,bonus});return hud.notice(captured?0:1,bonus);}
    bool spell_sound(i32 id)override{return sound(id,0,false);}
    i32& shared_spell_counter()override{return enemy_environment.shared_integers[2];}
    bool start_dialogue(i32)override;
    bool dialogue_waiting()const override{return dialogue&&dialogue->active&&!dialogue->state.release_signal;}
    bool dialogue_text(const DialogueText& request)override{dialogue_text_requests.push_back(request);return true;}
    bool dialogue_sound(i32 id)override{return sound(id,0,false);}
    bool hud_sound(i32 id)override{return sound(id,0,false);}
    bool dialogue_music()override{events.push_back({BattleEventKind::BossMusic,i32(resources.stage_number)});return true;}
    bool dialogue_fade(float seconds)override{events.push_back({BattleEventKind::MusicFade,0,0,{seconds,0,0}});return true;}
    bool dialogue_stage_complete()override{return completion.complete();}
    bool stage_result_animation()override;
    void recall_player_options()override{if(player)player->motion.recall_options(true);}
    void request_stage_exit(StageExit)override{}
    void start_ending_fade()override{fades.start(5,200,0,67,&animations.rate);}

    bool shot_damage(EnemyState&,i32&) override;
    bool player_collision(EnemyState&,float,i32&) override;
    bool tick_callback(EnemyState& e,i32& out) override {return enemy_callbacks.invoke_tick(e,e.tick_callback,out);}
    bool damage_callback(EnemyState& e,i32& out) override {return e.damage_callback==EnemyDamage::Arc&&enemy_callbacks.damage_arc(e,out);}
    bool collision_callback(EnemyState& e) override {return e.collision_callback==EnemyCollision::Arc&&enemy_callbacks.collision_arc(e);}
    EnemyLink* callback_enemies()override{return enemies.first;}
    bool callback_spawn(const EnemyState&,Vec3)override;
    bool callback_move_player(Vec3)override;
    bool callback_indicator(i16,bool)override;
    AnmVm* callback_animation(u32& id)override{return enemy_animations.lookup(id);}
    bool callback_damage(Vec3 p,Vec2 size,i32& out)override{if(!player)return false;const auto& timer=player->state.state_timer;return player->shots.damage(p,size,timer.previous!=timer.current,economy,out);}
    bool callback_collision(Vec3 p,float angle,float width,float length)override{return collision(p,angle,width,length)>=0;}
    bool graze(EnemyState&) override;
    bool sound(i32 id,float x) override { return sound(id,x,true); }
    bool add_score(i32 value) override { economy.add_score(value); return true; }
    bool score_popup(Vec3 p,i32 value)override{return popup(p,value,0xffffffff);}
    bool drop_items(EnemyState& e) override { items.player.power=economy.power;items.player.max_power=economy.max_power;return items.drop(e.drops,e.current.position); }
    bool cancel_reward(Vec3 p) override { return point_reward(p); }
    bool create_deformation(EnemyState&) override;
    bool deform(EnemyState& e) override { e.deformation->update(e,spell_id);return true; }
    bool release_deformation(EnemyState& e) override { delete e.deformation;e.deformation=nullptr;return true; }
    void clear_shot_targets(Enemy*) override;
    bool project_spawn(Vec3,Vec3&) override;

    bool effect(const Vec3& p,i32 script) override { return visual(p,script); }
    bool colored_effect(const Vec3&,i32,u32) override;
    i32 collision(const BulletState&) override;
    bool register_graze() override;
    bool reward_graze() override;
    bool cancel_reward(const Vec3& p) override { return point_reward(p); }
    bool cancel_enemies(const Vec3& p,float radius,bool reward) override { return enemies.cancel_circle(p,radius,reward); }
    bool special_damage(const Vec3&,const Vec2&,i32&) override;
    bool convert_enemies(const Vec3& p,float radius,bool reward) override { return enemies.cancel_circle(p,radius,reward,true); }

    bool sound(i32,float,bool) override;
    i32 collision(Vec3,float,float,float) override;
    i32 warning_collision(Vec3,float,float,float) override;
    bool cancel_effect(Vec3 p,i32 script) override { return visual(p,script); }
    bool cancel_shot(Vec3,float) override;
    bool graze_effect(Vec3 p) override { return visual(p,156); }
    bool graze_reward() override;
    bool graze_register() override { return register_graze(); }
    bool boss_position(Vec3&) override;

    bool effect(Vec3 p,i32 script) override { return visual(p,script); }
    bool collect(ItemState& item,bool& convert) override { return item_rewards.collect(item,convert); }
    bool rank_delta(i32 value) override { economy.add_rank(value); return true; }
    bool popup(Vec3,i32,u32) override;
    bool notify(i32) override;
    bool power_changed() override;
    bool lives_changed(i32 lives,i32 fragments) override { return display_lives(lives,fragments); }

    bool player_sound(i32 id) override { return sound(id,0,false); }
    bool attract_items() override { items.attract_all(); return true; }
    bool cancel_bullets(const Vec3*,float,bool) override;
    bool cancel_lasers(const Vec3* center,float radius,bool reward,bool skip) override { return center?lasers.cancel_circle(*center,radius,reward,skip)>=0:lasers.cancel_all(reward,skip); }
    bool start_bomb() override;
    bool bomb_sound(i32 id,float x,bool positional)override{return sound(id,x,positional);}
    bool bomb_stop_sound(i32 id)override{events.push_back({BattleEventKind::StopSound,id});return true;}
    bool bomb_cancel(Vec3,float,u32,bool)override;
    i32& bomb_count()override{return enemy_environment.shared_integers[1];}
    bool bomb_background_color(u32 color)override{if(!stage)return false;stage->state.tint=color;return true;}
    bool bomb_refund_power()override{bool changed=false;return item_rewards.add_power(10,changed)&&power_changed();}
    bool bomb_cancel_beam(Vec3,bool)override;
    bool cancel_enemy_beam(const Vec3& p,float width,bool reward)override{return enemies.cancel_beam(p,width,reward);}
    bool spawn_item(i32 type,Vec3 p,u32 color,float angle,float speed) override {
        // Death subtracts power inside the player callback before spawning
        // drops. The frame-start item snapshot is stale at this boundary.
        items.player.power=economy.power;items.player.max_power=economy.max_power;
        return items.spawn(type,p,color,angle,speed)==0;
    }
    bool display_lives(i32,i32) override;
    bool record_death() override;
    bool enemy_death() override;
    bool game_over(bool) override;
private:
    bool stage_control();
    bool activate_stage();
    HudInput hud_input()const;
    void create_dialogue();
    bool visual(Vec3,i32);
    bool point_reward(Vec3);
    bool communication_reward(i32);
    i32 apply_collision(PlayerCollisionResult);
    bool unavailable(const char* operation) { if(error.empty())error=std::string("unimplemented: ")+operation;last_error=-2;return false; }
};
}
