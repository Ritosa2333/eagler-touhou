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
#if PORT_TITLE == 14
#include "StagedLoaderPrototype.hpp"
#include "SceneCameraReadJobTh14.hpp"
#include "LoadingTh14.hpp"
#endif
#if PORT_TITLE == 16
#include "EffectTh16Slots.hpp"
#endif
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
#include "PublishedOwnerTh14.hpp"
#endif
#include <memory>
#include <vector>
#include <functional>

namespace th11 {
struct GameSessionInput;
class ScoreFile;
enum class BattleEventKind {Sound,Popup,Notification,Lives,Death,GameOver,SpellTitle,SpellResult,BossMusic,MusicFade,StopSound,StageMusic,MusicResume,Bombs,ComboPopup,SoundPan,MusicStop
#if PORT_TITLE == 14
    ,PauseResume
#endif
};
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
    GameSessionResources& resources;
    AnmManager& animations;
#if PORT_TITLE == 16
    Th16EffectSlots th16_effect_slots;
#endif
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
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
    PublishedOwnerTh14<BombController> bomb;
    std::atomic<bool> th14_next_tail_prepared{false};
    bool prepare_next_tail_owners(i32 character,i32 subtype);
#else
    std::unique_ptr<BombController> bomb;
#endif
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
#if PORT_TITLE == 16
    u32 th16_spell_background=0;
#endif
#if PORT_TITLE == 14
    u32 th14_dialogue_skip_frames=0,th14_dialogue_fire_frames=0;
    u32 th14_spell_background=0;
    bool th14_spell_result_requested=false;
    i32 th14_current_music=-1;
    u32 th14_audio_flags=0;
    ScoreFile* th14_scores=nullptr;
#endif
    u32 frame=0;
#if PORT_TITLE == 14
    // Native controller+10 is rate-scaled; frame counts physical updates
    // for Replay recording and remains independent of global slowdown.
    Timer th14_control_clock;
    // Main-only outcome of this update, independent of rate-scaled time.
    // Native436cc3 counts play time only after the Control early-return gates.
    bool th14_control_counted=false;
#endif
    i32 control_frame()const noexcept{
#if PORT_TITLE == 14
#if defined(TH14_CPP_STAGE_DECODE)
        return __atomic_load_n(&th14_control_clock.current,__ATOMIC_SEQ_CST);
#else
        return th14_control_clock.current;
#endif
#else
        return wrapping_add(i32(frame),1);
#endif
    }
    // Diagnostic: every unit of damage the player's shots have dealt this stage, so a run's kill rate can be read
    // off directly instead of inferred from when enemies happen to disappear.
    u32 damage_total=0;
    bool initialized=false,player_live=true,game_over_requested=false;
    i32 practice_high=0;
    bool replaying=false,demo=false;
    ReplayStageState replay_entry;
    i32 last_error=0;
    std::string error;
    // Optional state observer at native priority 16, before the player callback.
    // Aligns whole-session traces without changing gameplay.
    void (*before_player_update)(const GameBattle&)=nullptr;

    GameBattle(GameSessionResources&,AnmManager&,GameEconomy&,SpellRecords&,SceneCompositor&,ClearRecords&);
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE)
    GameBattle(GameSessionResources&,AnmManager&,GameEconomy&,SpellRecords&,SceneCompositor&,ClearRecords&,bool defer_initial_pools);
    void initialize_initial_pools();
    bool construct_initial_player(i32 character,i32 subtype);
    bool initial_player_waiting_for_resources=false;
#endif
    ~GameBattle()override;
    bool initialize(i32 character,i32 subtype,i32 difficulty);
#if PORT_TITLE == 14
    bool prepare_initial_player(i32 character,i32 subtype);
    bool prepare_initial_scene(bool complete);
    bool prepare_initial_tail_owners(i32 character,i32 subtype);
#endif
    bool next_stage(GameResources&,u32);
#if PORT_TITLE == 14
    u32 pending_stage=0;
    prototype::StagedLoader stage_loader;
    LoadingTh14 local_stage_loading;
    LoadingTh14& stage_loading=resources.loading_owner?*resources.loading_owner:local_stage_loading;
    std::function<void()> initial_control_activation;
    GameResources* next_stage_source=nullptr;
    std::function<bool()> next_stage_initializer;
    std::function<prototype::Work()> next_stage_initializer_step;
    bool stage_initialization_started=false,stage_initialization_finished=false;
    SceneCameraReadJobTh14 stage_camera_read_job;
    std::unique_ptr<Stage> incoming_scene_owner;
    std::function<prototype::Work(u32)> stage_camera_read_boundary;
    bool next_stage(GameResources&,u32,std::function<bool()>);
    bool next_stage(GameResources&,u32,std::function<bool()>,std::function<prototype::Work()>);
    bool pump_stage_loading(u64 now,i32 first_capture_index,u32 supervisor_flags);
    bool poll_stage_initialization(u64 now);
    bool poll_stage_scene(u64 now);
    bool retire_stage_owner();
    bool initialize_stage_work();
    prototype::Work advance_stage_work();
    prototype::Work prepare_stage_hud();
    prototype::Work prepare_stage_scene();
    bool publish_stage_owner();
    prototype::Work prepare_stage_timeline();
    bool finish_stage_loading();
    bool enter_stage_transition(u32);
#endif
    bool update(const std::function<bool()>& sample_input={}
#if PORT_TITLE == 14
        ,bool overlay_already_updated=false
        ,const std::function<bool()>& before_normal_animation_update={}
        ,const AnmManager::VmBoundaryCallbackTh14& normal_vm_boundary={}
        ,const StageObjectVmBoundaryTh14& stage_object_boundary={}
        ,const StageCameraWordBoundaryTh14& camera_word_boundary={}
        ,const std::function<void()>& control_update={}
#endif
    );
    bool draw(AnmRenderer&,SceneDrawKind
#if PORT_TITLE == 14
        ,const std::function<bool()>& camera_published={}
#endif
);
    bool spawn_root();
    void set_input(const GameSessionInput&);
    void synchronize_player();
    bool spell_begin_visuals(i32,i32,const char*)override;
    bool spell_update_visuals()override;
    void spell_background(bool visible)override{if(stage)stage->state.draw_flags=visible?stage->state.draw_flags|1:stage->state.draw_flags&~1u;}
    void spell_title_interrupt(i16 label)override{for(auto id:spell_titles)enemy_animations.interrupt(id,label);}
    void spell_circle_position(Vec3 p)override{if constexpr(kPortTitle==14||kPortTitle==16){if(auto* vm=animations.find(spell_circle))vm->position=p;}else enemy_animations.position(spell_circle,p,true);}
    void spell_circle_end()override{enemy_animations.erase(spell_circle);spell_circle=0;cancellation={spell_flags,spell_id};}
    bool spell_result(bool captured,i32 bonus)override{events.push_back({BattleEventKind::SpellResult,captured?0:1,bonus});return hud.notice(captured?0:1,bonus);}
    bool spell_sound(i32 id)override{return sound(id,0,false);}
    bool spell_timer_interrupt(i16)override;
    void spell_background_end()override;
    i32& shared_spell_counter()override{return enemy_environment.shared_integers[2];}
    bool start_dialogue(i32)override;
    bool dialogue_close()override;
    bool dialogue_waiting()const override{return dialogue&&dialogue->active&&!dialogue->state.release_signal;}
    bool dialogue_text(const DialogueText& request)override{
#if PORT_TITLE == 14
        if(resources.effects)if(auto* device=resources.effects->dialogue_text_device())return device->dialogue_text(request);
#endif
        dialogue_text_requests.push_back(request);return true;
    }
    bool dialogue_sound(i32 id)override{return sound(id,0,false);}
    bool hud_sound(i32 id)override{return sound(id,0,false);}
    bool dialogue_music()override{events.push_back({BattleEventKind::BossMusic,i32(resources.stage_number)});return true;}
#if PORT_TITLE == 14
    bool dialogue_boss_name()override{return hud.boss_name_th14(hud_input());}
#endif
    bool dialogue_fade(float seconds)override{events.push_back({BattleEventKind::MusicFade,0,0,{seconds,0,0}});return true;}
    bool dialogue_stage_complete()override{
#if PORT_TITLE == 14
        completion.mode.continues=hud.score.continues;
#endif
        return completion.complete();
    }
    bool stage_result_animation()override;
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
    void start_ending_control()override{
        if(auto* control=animations.th14_control.get())control->flags|=0x4000u;
    }
#endif
    void recall_player_options()override{if(player){
#if PORT_TITLE == 14
        // Retail4358e5 changes this flag in the same call that sends label3.
        player->th14_scale.flags|=2;
#endif
        player->motion.recall_options(true);
    }}
    void request_stage_exit(StageExit exit)override{
#if PORT_TITLE == 14
        // Stage completion and Player GameOver both enter448ef0. Reuse the
        // menu delivery callback without declaring a gameplay GameOver.
        if(exit==StageExit::ReplayEnd&&on_game_over&&!on_game_over(true)){
            last_error=-1011;if(error.empty())error="replay completion menu entry failed";
        }
#endif
    }
    void start_ending_fade()override{fades.start(5,200,0,kPortTitle==14?73:67,&animations.rate);}

    bool shot_damage(EnemyState&,i32&) override;
#if PORT_TITLE == 14
    bool th14_probe_damage(EnemyState&,i32&,bool&) override;
#endif
#if PORT_TITLE == 16
    bool th16_shot_score(i32 amount)override{return item_rewards.th16_add_score(amount);}
    bool th16_beam_pan(i32 pan)override{events.push_back({BattleEventKind::SoundPan,20,pan});return true;}
    bool th16_beam_stop()override{events.push_back({BattleEventKind::StopSound,20});return true;}
    AnmVm* th16_freeze_effect(Vec3 position,float angle)override{return animations.create(resources.core.bullet,152,7,0xffffffff,false,false,&position,nullptr,nullptr,false,&angle,0x2400);}
    bool th16_register_hit_effect(u32 id)override{
        struct Lookup:Th16EffectLookup{AnmManager& animations;explicit Lookup(AnmManager& a):animations(a){}bool alive(u32 id)override{return animations.find(id)!=nullptr;}} lookup(animations);
        const i32 slot=th16_effect_slots.choose(lookup);if(slot>=0)th16_effect_slots.ids[u32(slot)]=id;return true;
    }
    bool th16_player_death_effect(Vec3 position)override{
        struct Lookup:Th16EffectLookup{AnmManager& animations;explicit Lookup(AnmManager& a):animations(a){}bool alive(u32 id)override{return animations.find(id)!=nullptr;}} lookup(animations);
        const i32 slot=th16_effect_slots.choose(lookup);if(slot<0)return true;const float rotation=0;
        auto* vm=animations.create(resources.core.effect,28,8,0xffffffff,false,false,&position,nullptr,nullptr,false,&rotation,0x2400);if(!vm)return false;th16_effect_slots.ids[u32(slot)]=vm->id;return true;
    }
#endif
    bool player_collision(EnemyState&,float,i32&) override;
    bool tick_callback(EnemyState& e,i32& out) override {
#if PORT_TITLE == 14
        return enemy_callbacks.invoke_th14(e,i32(e.tick_callback),out);
#else
        return enemy_callbacks.invoke_tick(e,e.tick_callback,out);
#endif
    }
    bool damage_callback(EnemyState& e,i32& out) override {
#if PORT_TITLE == 14
        return e.damage_callback==EnemyDamage::Arc&&enemy_callbacks.damage_th14(e,out);
#else
        return e.damage_callback==EnemyDamage::Arc&&enemy_callbacks.damage_arc(e,out);
#endif
    }
    bool collision_callback(EnemyState& e) override {return e.collision_callback==EnemyCollision::Arc&&enemy_callbacks.collision_arc(e);}
    EnemyLink* callback_enemies()override{return enemies.first;}
    bool callback_spawn(const EnemyState&,Vec3)override;
    bool callback_move_player(Vec3)override;
    bool callback_indicator(i16,bool)override;
#if PORT_TITLE == 14
    AnmVm* callback_th14_visual()override{return &compositor.copies[3];}
    bool th14_spawn_enemy(const char* name,const EnemySpawn& value)override{return enemies.spawn(name,value)!=nullptr;}
    bool callback_th14_player_scale(float value)override{return player&&player->set_th14_scale(value);}
    bool th14_hit_item(i32 type,Vec3 p,float angle,float speed)override{return items.spawn(type,p,0xffffffff,angle,speed)==0;}
    // Native VisualManager+10 owns effect.anm; +14 owns bullet.anm.
    // 452183 passes layer -1 so effect149 retains its template layer/children.
    bool th14_hit_effect(Vec3 p,float angle)override{return animations.create(resources.core.effect,149,8,-1,false,false,&p,nullptr,nullptr,false,&angle)!=nullptr;}
    bool th14_beam_pan(i32 pan)override{
        animations.th14_native_stack.reference_pan_call();
        events.push_back({BattleEventKind::SoundPan,20,pan});return true;
    }
    bool th14_beam_stop()override{events.push_back({BattleEventKind::StopSound,20});return true;}
    bool th14_knife_slowdown(Enemy& e,float delta)override{shot_th14_sakuya_slowdown(e.state,delta,spell_flags,spell_id);return true;}
    bool th14_beam_hit_effect(Vec3 p,float angle,Vec3 travel)override{
        auto* vm=animations.create(resources.core.players[enemy_environment.character],8,kPortTitle==14?9:7,22,false,false,&p);if(!vm)return false;
        animations.th14_native_stack.beam_effect_published(vm->id);
        vm->flags|=4;vm->rotation.z=angle;auto& v=vm->position_interpolation;
        v.duration=20;v.mode=InterpolationMode::Decelerate2;v.timer.set(0,&animations.rate);
        const auto start=animations.default_tangent;std::memcpy(v.start,&start,12);std::memcpy(v.end,&travel,12);
        std::memcpy(v.initial_tangent,&start,12);std::memcpy(v.final_tangent,&start,12);return true;
    }
    bool th14_plain_hit_effect(Vec3 p,float angle,u32 color)override{
        auto* vm=animations.create(resources.core.effect,150,8,-1,false,false,&p,nullptr,nullptr,false,&angle);
        if(!vm)return false;vm->color=color;return true;
    }
    bool th14_bomb_dialogue()const override{return dialogue&&dialogue->active;}
#if PORT_TITLE == 14
    bool th14_damage_dialogue()const override{return dialogue&&dialogue->active;}
#endif
    bool th14_reimu_bomb_hit_effect(Vec3 p,float angle)override{
        return animations.create(resources.core.effect,58,8,0,false,false,&p,nullptr,nullptr,false,&angle)!=nullptr;
    }
    bool th14_bomb_circle(Vec3 p,float radius,float growth,i32 duration,i32 damage)override{
        return player&&player->shots.th14_bomb_circle(p,radius,growth,duration,damage);
    }
    bool th14_bomb_item(i32 type,Vec3 p,float angle,float speed)override{return items.spawn(type,p,0xffffffff,angle,speed)==0;}
    bool th14_bomb_rectangle(Vec3 p,Vec2 size,float angle,i32 duration,i32 damage)override{return player&&player->shots.th14_bomb_rectangle(p,size,angle,duration,damage);}
    bool th14_bomb_cancel_rectangle(Vec3 p,Vec2 size,float angle,u32 rewards)override{
        return bullets.cancel_rectangle_th14(p,size,angle,rewards)&&lasers.cancel_rectangle_th14(p,size,angle,rewards)>=0;
    }
    bool th14_reimu_cancel_rectangle(Vec3 p,Vec2 size,float angle,u32 rewards)override{
        return bullets.cancel_rectangle_th14(p,size,angle,rewards)&&lasers.cancel_rectangle_th14(p,size,angle,rewards,true)>=0;
    }
    bool th14_bomb_shake(i32 amplitude,i32 ramp,i32 hold,i32 fall)override{shakes.start_th14_hold(amplitude,ramp,hold,fall,&animations.rate);return true;}
    bool th14_bomb_shake_short(i32 duration,i32 from,i32 to)override{shakes.start(duration,from,to,&animations.rate);return true;}
    EnemyLink* th14_bomb_enemies()const override{return enemies.first;}
    bool callback_th14_damage(const EnemyState& e,Vec3 p,const Vec2* size,float radius,float angle,i32& out)override{
        if(!player)return false;const auto& timer=player->state.state_timer;
        return player->shots.damage_th14(p,size,radius,angle,timer.previous!=timer.current,economy,out,u32(e.instance_id),u32(e.instance_id),e.script_owner);
    }
#endif
    AnmVm* callback_animation(u32& id)override{return enemy_animations.lookup(id);}
    bool callback_damage(Vec3 p,Vec2 size,i32& out)override{if(!player)return false;const auto& timer=player->state.state_timer;return player->shots.damage(p,size,timer.previous!=timer.current,economy,out);}
    bool callback_collision(Vec3 p,float angle,float width,float length)override{return collision(p,angle,width,length)>=0;}
    bool graze(EnemyState&) override;
    bool sound(i32 id,float x) override { return sound(id,x,true); }
    bool add_score(i32 value) override { economy.add_score(value); return true; }
    bool score_popup(Vec3 p,i32 value)override{return popup(p,value,0xffffffff);}
    bool drop_items(EnemyState& e) override { items.player.power=economy.power;items.player.max_power=economy.max_power;return items.drop(e.drops,e.current.position); }
#if PORT_TITLE == 16
    bool drop_items_now_th16(EnemyState& e)override{return items.drop_th16(e.drops,e.current.position);}
#endif
    bool season_damage_drop(EnemyState&)override;
    bool cancel_reward(Vec3 p) override { return point_reward(p); }
    bool create_deformation(EnemyState&) override;
    bool deform(EnemyState& e) override { e.deformation->update(e,spell_id);return true; }
    bool release_deformation(EnemyState& e) override { delete e.deformation;e.deformation=nullptr;return true; }
    void clear_shot_targets(Enemy*) override;
    bool project_spawn(Vec3,Vec3&) override;

    bool effect(const Vec3& p,i32 script) override { return visual(p,script); }
    bool colored_effect(const Vec3&,i32,u32) override;
    i32 collision(const BulletState&) override;
#if PORT_TITLE == 14
    bool th14_bullet_hit_effect(const Vec3&,i32,const Vec3&)override;
#endif
    bool th16_hit_effect(const Vec3&,i32,const Vec3&)override;
    bool th16_graze(const Vec3&)override;
    i32 th14_probe_collision(const BulletState&)override;
    bool register_graze() override;
    bool reward_graze() override;
    bool cancel_reward(const Vec3& p) override { return point_reward(p); }
    bool th14_cancel_item(const Vec3& p) override {
#if PORT_TITLE == 14
        return items.cancel_reward_th14(p);
#else
        return false;
#endif
    }
    bool th14_cancel_reward(const Vec3& p,u32 rewards,u32 count)override{
#if PORT_TITLE == 14
        return items.cancel_reward_th14(p,rewards,count,spell_flags);
#else
        return false;
#endif
    }
    bool cancel_enemies(const Vec3& p,float radius,bool reward) override { return enemies.cancel_circle(p,radius,reward); }
    bool special_damage(const Vec3&,const Vec2&,i32&) override;
    bool convert_enemies(const Vec3& p,float radius,bool reward) override { return enemies.cancel_circle(p,radius,reward,true); }

    bool sound(i32,float,bool) override;
    i32 collision(Vec3,float,float,float) override;
    i32 warning_collision(Vec3,float,float,float) override;
    bool cancel_effect(Vec3 p,i32 script) override { return visual(p,script); }
#if PORT_TITLE == 16
    bool th16_line_graze(Vec3 p)override{return th16_graze(p);}
    bool th16_line_reward(Vec3 p,u32 rewards)override;
    bool th16_line_cut_effect(Vec3 p,i32 script)override{
        const float rotation=0;auto* vm=animations.create(resources.core.bullet,script,7,0xffffffff,false,false,&p,nullptr,nullptr,false,&rotation,0x400);
        return vm&&th16_register_hit_effect(vm->id);
    }
#endif
#if PORT_TITLE == 14
    bool th14_curve_reward(Vec3 p,u32 rewards)override{
        if(!rewards)return true;
        const float left=p.x+32.f,right=p.x-32.f,top=p.y+32.f,bottom=p.y-32.f;
        if(left<=-192.f||right>=192.f||top<=0.f||bottom>=448.f)return true;
        ++bullets.th14_reward_count;return th14_cancel_reward(p,rewards,bullets.th14_reward_count);
    }
    bool th14_curve_graze(Vec3)override;
#endif
    bool cancel_shot(Vec3,float) override;
    bool graze_effect(Vec3 p) override { return visual(p,156); }
    bool graze_reward() override;
    bool graze_register() override { return register_graze(); }
    bool boss_position(Vec3&) override;

    bool effect(Vec3 p,i32 script) override { return visual(p,script); }
    bool collect(ItemState& item,bool& convert) override { return item_rewards.collect(item,convert,&items.player); }
#if PORT_TITLE == 16
    bool item_th16_collect(ItemState&)override;
    bool item_th16_spawn_notice(Vec3,i32)override;
#endif
#if PORT_TITLE == 14
    i32& item_th14_drop_counter()override{return economy.th14_special_drop_count;}
    Th14ItemCombo& item_th14_combo_state()override{return economy.th14_combo;}
    bool item_th14_combo_settle()override{return item_rewards.th14_combo_settle(dialogue&&dialogue->active);}
    bool th14_combo_drop(i32 type,Vec3 p,i32 delay)override{return items.spawn_ordinary_th14(type,p,-1.5707963705062866f,2.2f,delay)==0;}
    bool th14_combo_popup(Vec3 p,i32 value,u32 color,float multiplier)override{
        popups.add_bonus(p,value,color,multiplier,&animations.rate);
        events.push_back({BattleEventKind::ComboPopup,value,signed_bits(float_bits(multiplier)),p,color});
        return true;
    }
    bool item_th14_spawn_notice(Vec3 p,i32 type)override{
        return animations.create(resources.core.effect,100,8,0,false,false,&p)&&sound(type==4||type==5?74:48,0,false);
    }
#endif
    bool rank_delta(i32 value) override { economy.add_rank(value); return true; }
    bool popup(Vec3,i32,u32) override;
    bool notify(i32) override;
    bool power_changed() override;
    bool lives_changed(i32 lives,i32 fragments) override { return display_lives(lives,fragments); }
#if PORT_TITLE == 14 || PORT_TITLE == 16
    bool bombs_changed(i32 count,i32 fragments)override{hud.display_bombs(count,fragments);events.push_back({BattleEventKind::Bombs,count,fragments});return true;}
#endif

    bool player_sound(i32 id) override { return sound(id,0,false); }
#if PORT_TITLE == 16
    u32 player_movement_effect(i32 script)override{const Vec3 origin{};const float rotation=0;
        auto* vm=animations.create(resources.core.effect,script,8,14,false,false,&origin,nullptr,nullptr,false,&rotation,0x400);return vm?vm->id:0;}
    bool player_effect(Vec3 p,i32 script)override{const float rotation=0;return animations.create(resources.core.effect,script,8,0xffffffff,false,false,&p,nullptr,nullptr,false,&rotation,0x2400)!=nullptr;}
#endif
#if PORT_TITLE == 14
    // 44f60d/44f7df retain the effect27/28 template layer (18).
    bool player_effect(Vec3 p,i32 script)override{return animations.create(resources.core.effect,script,8,u32(-1),false,false,&p)!=nullptr;}
    u32 player_focus()override{auto* vm=animations.create(resources.core.effect,26,8,14,false,false,nullptr,nullptr,nullptr,true);return vm?vm->id:0;}
#endif
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
    bool th14_bomb_freeze(bool)override;
    bool th14_bomb_cancel(Vec3,float,u32)override;
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
#if PORT_TITLE == 14
    std::function<bool(bool)> on_game_over;
#endif
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
