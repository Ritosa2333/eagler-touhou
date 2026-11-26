// 从原游戏共同初态和回放输入逐步执行已恢复的单机逻辑，并输出完整对照状态。
#include "EmitterTestHarness.hpp"
#include "core/runtime/GameplayFrameRuntime.hpp"
#include "core/runtime/GameplayFrameWorldView.hpp"
#include "core/frontend/ReplayGameplayInput.hpp"
#include "GameplayReplayTailProbe.hpp"
#include "core/entities/EnemyOpeningBehavior.hpp"
#include "core/entities/BasicProjectileBehavior.hpp"
#include "core/entities/EnemySecondBehavior.hpp"
#include "core/entities/EnemyThirdBehavior.hpp"
#include "core/entities/EnemyFourthBehavior.hpp"
#include "core/entities/EnemyFifthBehavior.hpp"
#include "core/entities/EnemySixthBehavior.hpp"
#include "core/entities/EnemySeventhBehavior.hpp"
#include "core/entities/EnemyEighthBehavior.hpp"
#include "core/entities/EnemyNinthBehavior.hpp"
#include "core/entities/EnemyTenthBehavior.hpp"
#include "core/entities/EnemyEleventhBehavior.hpp"
#include "core/entities/EnemyTwelfthBehavior.hpp"
#include "core/entities/EnemyThirteenthBehavior.hpp"
#include "core/entities/ProjectileMotionBehavior.hpp"
#include "core/player/PlayerMovement.hpp"
#include "core/player/PlayerLifecycle.hpp"
#include "core/player/PlayerCollision.hpp"
#include "core/entities/ProjectileCollisionRadii.hpp"
#include "core/player/ItemDrop.hpp"
#include "core/shared/EffectUpdate.hpp"
#include "core/shared/GameColor.hpp"
#include "core/player/PlayerOptions.hpp"
#include "core/player/ItemMotion.hpp"
#include "core/player/ItemPickup.hpp"
#include "core/player/OptionsMotion.hpp"
#include "core/player/PlayerShotUpdate.hpp"
#include "core/player/PlayerShotFire.hpp"
#include "core/player/PlayerShotHit.hpp"
#include "core/player/PlayerShotHitLiveModeServices.hpp"
#include "core/entities/EnemyDamage.hpp"
#include "core/entities/EnemyDeath.hpp"
#include "BossWorldProbeState.hpp"
#include "core/entities/BossAllocation.hpp"
#include "core/entities/BossPhase.hpp"
#include "core/entities/BossPresentation.hpp"
#include "core/stages/Stage1SecondTimeline.hpp"
#include "core/stages/Stage1ThirdTimeline.hpp"
#include "core/entities/BossProjectileBehavior.hpp"
#include "core/entities/BossEnemyBehavior.hpp"
#include "core/entities/BossTimer.hpp"
#include "core/entities/BossSprite.hpp"
#include "core/entities/BossVisualUpdate.hpp"
#include "core/entities/EnemyLaterBehavior.hpp"
#include "core/entities/LaterProjectileBehavior.hpp"
#include "ActionWorldProbeState.hpp"
#include "core/player/PlayerBomb.hpp"
#include "core/player/PlayerBombHitServices.hpp"
#include "core/player/PlayerBombHit.hpp"
#include "core/stages/Stage1BossTimeline.hpp"
#include "core/stages/Stage1Dialogue.hpp"
#include "core/entities/EnemyBossNextBehavior.hpp"
#include "core/entities/FinalBossProjectileBehavior.hpp"
#include "core/stages/StageClear.hpp"
#include "core/stages/StageClearPresentation.hpp"
#include "ResourceWorldProbeState.hpp"
#include "core/stages/StageResources.hpp"
#include "core/stages/StageBackground.hpp"
#include "core/stages/Stage2FirstTimeline.hpp"
#include "core/stages/Stage2FirstEnemyBehavior.hpp"
#include "core/stages/Stage2SecondTimeline.hpp"
#include "core/stages/Stage2MidbossEnemyBehavior.hpp"
#include "core/stages/Stage2MidbossProjectileBehavior.hpp"
#include "core/stages/Stage2ThirdTimeline.hpp"
#include "core/stages/Stage2ThirdEnemyBehavior.hpp"
#include "core/stages/Stage2ThirdProjectileBehavior.hpp"
#include "core/stages/Stage2Dialogue.hpp"
#include "core/stages/Stage2BossTimeline.hpp"
#include "core/stages/Stage2BossEnemyBehavior.hpp"
#include "core/stages/Stage2BossProjectileBehavior.hpp"
#include "core/stages/Stage3FirstTimeline.hpp"
#include "core/stages/Stage3FirstEnemyEarly.hpp"
#include "core/stages/Stage3FirstEnemyLate.hpp"
#include "core/stages/Stage3SecondTimeline.hpp"
#include "core/stages/Stage3Dialogue.hpp"
#include "core/stages/Stage3MidbossEnemyBehavior.hpp"
#include "core/stages/Stage3MidbossProjectileBehavior.hpp"
#include "core/stages/Stage3ThirdTimeline.hpp"
#include "core/stages/Stage3ThirdEnemyBehavior.hpp"
#include "core/stages/Stage3BossTimelineContext.hpp"
#include "core/stages/Stage3BossTimeline.hpp"
#include "core/stages/Stage3BossEnemyEarly.hpp"
#include "core/stages/Stage3BossProjectileEarly.hpp"
#include "core/stages/Stage3BossEnemyFinal.hpp"
#include "core/stages/Stage3BossProjectileFinal.hpp"
#include "core/stages/Stage4FirstTimeline.hpp"
#include "core/stages/Stage4FirstEnemyBehavior.hpp"
#include "core/stages/Stage4FirstProjectileBehavior.hpp"
#include "core/stages/Stage4PatternContext.hpp"
#include "core/stages/Stage4MidbossTimeline.hpp"
#include "core/stages/Stage4MidbossEnemy.hpp"
#include "core/stages/Stage4MidbossProjectile.hpp"
#include "core/stages/Stage4ThirdTimeline.hpp"
#include "core/stages/Stage4ThirdEnemyBehavior.hpp"
#include "core/stages/Stage4Dialogue.hpp"
#include "core/stages/Stage4BossTimeline.hpp"
#include "core/stages/Stage4BossEnemyEarly.hpp"
#include "core/stages/Stage4BossProjectileEarly.hpp"
#include "core/stages/Stage4BossEnemyFinal.hpp"
#include "core/stages/Stage4BossProjectileFinal.hpp"
#include "core/entities/ProjectilePresentation.hpp"
#include "core/shared/BackgroundMeshPresentation.hpp"
#include "core/stages/Stage5FirstTimeline.hpp"
#include "core/stages/Stage5FirstEnemies.hpp"
#include "core/stages/Stage5FirstProjectiles.hpp"
#include "core/stages/Stage5MidbossTimeline.hpp"
#include "core/stages/Stage5MidbossEnemy.hpp"
#include "core/stages/Stage5MidbossProjectile.hpp"
#include "core/stages/Stage5ThirdTimeline.hpp"
#include "core/stages/Stage5ThirdEnemies.hpp"
#include "core/stages/Stage5BossTimeline.hpp"
#include "core/stages/Stage5BossEarlyEnemy.hpp"
#include "core/stages/Stage5BossEarlyProjectile.hpp"
#include "core/stages/Stage5BossFinalEnemies.hpp"
#include "core/stages/Stage5BossFinalProjectiles.hpp"
#include "core/stages/Stage5BossFinalSecondary.hpp"
#include "core/stages/Stage6FirstTimeline.hpp"
#include "core/stages/Stage6FirstEnemies.hpp"
#include "core/stages/Stage6FirstProjectiles.hpp"
#include "core/stages/Stage5Dialogue.hpp"
#include "core/stages/Stage6Dialogue.hpp"
#include "core/stages/Stage6RuntimeContext.hpp"
#include "core/frontend/EndingFadePresentation.hpp"
#include "core/stages/Stage6MidbossTimeline.hpp"
#include "core/stages/Stage6MidbossEnemy.hpp"
#include "core/stages/Stage6MidbossProjectile.hpp"
#include "core/stages/Stage6BossTimeline.hpp"
#include "core/stages/Stage6BossEarlyEnemies.hpp"
#include "core/stages/Stage6BossEarlyProjectiles.hpp"
#include "core/stages/Stage6BossFinalEnemy.hpp"
#include "core/stages/Stage6BossFinalProjectile.hpp"
#include "core/stages/ExFirstTimeline.hpp"
#include "core/stages/ExFirstEnemies.hpp"
#include "core/stages/ExFirstProjectiles.hpp"
#include "core/stages/ExMidbossTimeline.hpp"
#include "core/stages/ExMidbossProjectile.hpp"
#include "core/stages/ExThirdTimeline.hpp"
#include "core/stages/ExThirdEnemies.hpp"
#include "core/stages/ExThirdProjectiles.hpp"
#include "core/frontend/ReplayPauseEntry.hpp"
#include "ReplayPauseProbeServices.hpp"
#include "core/runtime/GameplayPresentationDispatch.hpp"
#include "GameplayPresentationProbeServices.hpp"
#include "core/stages/ExBossEarlyTimeline.hpp"
#include "core/stages/ExBossEarlyTimelineRuntimeServices.hpp"
#include "core/stages/ExBossOpeningProjectile.hpp"
#include "core/stages/ExBossMiddleProjectile.hpp"
#include "core/stages/ExBossEarlyFinal.hpp"
#include "core/stages/ExBossMiddleTimeline.hpp"
#include "core/stages/ExBossMiddleTimelineRuntimeServices.hpp"
#include "ExDrawingWorldProbeState.hpp"
#include "core/stages/ExBossEnemies.hpp"
#include "core/stages/ExBossControlProjectiles.hpp"
#include "core/stages/ExBossControlRuntimeServices.hpp"
#include "core/stages/ExBossLateTimeline.hpp"
#include "ExBossLateTimelineProbeServices.hpp"
#include "ExTitleWorldProbeState.hpp"
#include "ExReplayTailWorldProbeState.hpp"
#include "ExTitleProbeServices.hpp"
#include "core/frontend/MenuTitleReturn.hpp"
#include "core/stages/ExBossHeavy.hpp"
#include "core/stages/ExBossHeavyRuntimeServices.hpp"
#include "core/stages/ExBossMiddleFinal.hpp"
#include "core/stages/ExBossLateOther.hpp"
#include "core/stages/OmniverseTimeline.hpp"
#include "core/stages/OmniverseEnemies.hpp"
#include "core/stages/OmniverseProjectiles.hpp"
#include "core/stages/OmniverseWorldRuntimeServices.hpp"
#include "OmniverseDrawingWorldProbeState.hpp"
#include "core/stages/ExDialogue.hpp"
#include "core/shared/BackgroundMoviePause.hpp"
#include "core/shared/MusicPresentation.hpp"
#include "core/stages/StageTitlePresentation.hpp"
#include "PresentationWorldProbeState.hpp"
#include <functional>
#include <vector>
#include <string>

namespace {
template<class Records> void read_records(Records& records) {
    for (auto& record : records) std::cin.read(reinterpret_cast<char*>(record.data.data()), record.data.size());
    if (!std::cin) throw std::runtime_error("Opening state truncated");
}
template<class Records> void write_records(const Records& records) {
    for (const auto& record : records) std::cout.write(reinterpret_cast<const char*>(record.data.data()), record.data.size());
}
struct State {
    kaikeidou::ObjectRecord<240> player;
    kaikeidou::EnemyPool enemies;
    kaikeidou::ProjectilePools projectiles;
    kaikeidou::ItemPool items;
    kaikeidou::EffectPool effects;
    kaikeidou::EffectShake shake;
    kaikeidou::OptionsPool options;
    kaikeidou::OptionsContext options_context;
    kaikeidou::PlayerShotPools shots;
    std::uint32_t fire_lock = 0;
    kaikeidou::BossWorldProbeState boss;
    kaikeidou::ActionWorldProbeState action;
    kaikeidou::DialogueState dialogue;
    kaikeidou::ResourceWorldProbeState background;
    kaikeidou::Stage3BossTimelineContext extended_patterns;
    kaikeidou::Stage4PatternContext stage4_patterns;
    kaikeidou::Stage6RuntimeContext stage6;
    kaikeidou::ObjectRecord<1200> ex_vectors;
    kaikeidou::ExDrawingWorldProbeState ex_drawing;
    kaikeidou::OmniverseDrawingWorldProbeState omniverse_drawing;
    kaikeidou::ExTitleWorldProbeState ex_title;
    kaikeidou::ExReplayTailWorldProbeState ex_replay_tail;
    kaikeidou::MusicPresentationContext music;
    kaikeidou::StageTitleContext title;
    kaikeidou::PresentationWorldProbeState presentation;
    bool presentation_enabled = false;
    bool resource_enabled = false;
    std::uint32_t resource_tick = 0;
    std::uint32_t music_events = 0, save_events = 0;
    std::array<std::uint32_t, 9> bindings{{1, 2, 3, 0, 5, 4, 11, 13, 6}};
    std::uint32_t display_depth = 32, display_renderer = 1;
    std::array<std::uint32_t, 16> input{};
    bool replay_playback = false;
    std::uint32_t replay_buttons = 0;
    std::uint32_t input_value(std::uint32_t binding) const {
        if (replay_playback) return (replay_buttons >> ((binding == 11 ? 7u : binding) & 31u)) & 1u;
        return binding < input.size() ? input[binding] : 0u;
    }
};
struct Services final : kaikeidou::ExFirstServices, kaikeidou::ExBossEnemyServices {
    kaikeidou::ProjectilePools& projectiles;
    kaikeidou::GameMath& math;
    Services(kaikeidou::ProjectilePools& pools, kaikeidou::GameMath& functions) : projectiles(pools), math(functions) {}
    std::function<void(std::uint32_t,std::uint32_t)> on_music;
    std::function<void()> on_title;
    std::function<void(std::uint32_t)> on_kill;
    void kill_enemy(std::uint32_t index) override { if(!on_kill)throw std::runtime_error("EX enemy death service missing");on_kill(index); }
    void music(std::uint32_t track, std::uint32_t mode) override { if(on_music)on_music(track,mode); }
    void title() override { if(on_title)on_title(); }
    void clear(std::int32_t mode) override { kaikeidou::clear_projectiles(projectiles, mode); }
    kaikeidou::binary_float::Number cosine(std::uint64_t argument) override { return math.cosine(argument); }
};
struct PlayerServices final : kaikeidou::LifecycleServices {
    State& state;
    kaikeidou::LifecycleContext& context;
    kaikeidou::GameMath& math;
    kaikeidou::RandomPrecision precision;
    PlayerServices(State& world, kaikeidou::LifecycleContext& lifecycle, kaikeidou::GameMath& functions, kaikeidou::RandomPrecision bits)
        : state(world), context(lifecycle), math(functions), precision(bits) {}
    void drop_items() override { kaikeidou::drop_death_items(state.items, state.player, context.character, math, precision); }
    std::uint32_t color(std::uint32_t red, std::uint32_t green, std::uint32_t blue) override {
        return kaikeidou::game_color(red, green, blue, state.display_depth);
    }
    void effect(std::uint32_t kind, std::uint32_t color, std::uint64_t x, std::uint64_t y) override {
        kaikeidou::allocate_effect(state.effects, state.projectiles.sound_triggers, {kind, color, x, y});
    }
    void reset_options(std::uint32_t first, std::uint32_t last) override {
        kaikeidou::reset_player_options(state.options, state.player, state.options_context, first, last);
    }
    void reset_speed(std::uint64_t multiplier) override {
        kaikeidou::reset_player_bomb_speed(state.player,context.character,multiplier,static_cast<unsigned>(precision));
    }
    std::function<void(std::uint32_t,std::uint32_t)> on_music;
    void original_action(std::uint32_t track, std::uint32_t mode) override {
        if(!on_music)throw std::runtime_error("Lifecycle music service missing");on_music(track,mode);
    }
};
struct CollisionServices final : kaikeidou::CollisionServices {
    std::uint32_t special_collision(kaikeidou::ObjectRecord<240>& player, kaikeidou::ProjectilePools& pools, kaikeidou::CollisionContext&) override {
        return kaikeidou::stage5_boss_secondary_contact(pools,player,24);
    }
};
struct ItemInput final : kaikeidou::ItemMotionInput {
    State& state;
    explicit ItemInput(State& world) : state(world) {}
    std::uint32_t read_counter(std::uint32_t binding) override {
        return state.input_value(binding);
    }
};
struct FireInput final : kaikeidou::PlayerShotFireInput {
    State& state;
    explicit FireInput(State& world) : state(world) {}
    std::uint32_t read_counter(std::uint32_t binding) override {
        return state.input_value(binding);
    }
};
struct SpriteServices final : kaikeidou::BossSpriteServices {
    // 图形实参已由独立原函数对照；此数值探针保留绘制期间的状态更新。
    void blend(std::uint32_t, std::uint32_t) override {}
    void sprite(std::uint64_t, std::uint64_t, std::uint64_t, std::uint64_t,
        std::uint32_t, std::uint32_t, std::uint32_t) override {}
    std::uint32_t sprite_handle(std::uint32_t) const override { return 0; }
};
struct MidbossInput final : kaikeidou::PlayerShotFireInput {
    State& state;
    explicit MidbossInput(State& world):state(world){}
    std::uint32_t read_counter(std::uint32_t binding) override {
        if(state.boss.phase.replay_variant && state.boss.phase.replay_mode){
            const auto bit=binding==11?7u:binding;
            return bit<32?(state.replay_buttons>>bit)&1u:0u;
        }
        return binding<state.input.size()?state.input[binding]:state.boss.phase.replay_variant?1u:0xffffffffu;
    }
};
struct MeshServices final : kaikeidou::BackgroundMeshPresentationServices {
    std::int32_t set_clip(std::int32_t, std::int32_t, std::int32_t, std::int32_t) override { return 0; }
    std::int32_t draw_triangles(const std::uint8_t*, std::uint32_t, std::uint32_t, std::uint32_t) override { return 0; }
};
struct EndingFadeServices final : kaikeidou::EndingFadePresentationServices {
    void blend(std::uint32_t, std::uint32_t) override {}
    std::uint32_t color(std::uint32_t red, std::uint32_t green, std::uint32_t blue) override { return (red<<16)|(green<<8)|blue; }
    void rectangle(std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t) override {}
};
std::int32_t presentation_result(std::uint32_t bits){std::int32_t value;std::memcpy(&value,&bits,4);return value;}
struct TitleResources final : kaikeidou::StageResourceServices {
    State& state;const kaikeidou::StageTimeline& timeline;
    TitleResources(State& world,const kaikeidou::StageTimeline& time):state(world),timeline(time){}
    std::uint32_t load_graph(std::string_view path,std::uint32_t flag) override {
        return state.presentation.consume(state.resource_tick,timeline.frame,2,7,{0,flag},&state.title,sizeof(state.title),path);
    }
    std::int32_t delete_graph(std::uint32_t handle,std::uint32_t flag) override {
        return presentation_result(state.presentation.consume(state.resource_tick,timeline.frame,2,8,{handle,flag},&state.title,sizeof(state.title)));
    }
    std::uint32_t platform_random(std::uint32_t) override {throw std::runtime_error("Unexpected title random service");}
    std::int32_t movie_seek(std::uint32_t,std::uint32_t) override {throw std::runtime_error("Unexpected title movie seek");}
    std::int32_t movie_play(std::uint32_t,std::uint32_t,std::uint32_t) override {throw std::runtime_error("Unexpected title movie playback");}
};
struct TitleGraphics final : kaikeidou::StageTitlePresentationServices {
    State& state;explicit TitleGraphics(State& world):state(world){}
    void blend(std::uint32_t,std::uint32_t) override {}
    std::uint32_t color(std::uint32_t red,std::uint32_t green,std::uint32_t blue) override {return kaikeidou::game_color(red,green,blue,state.display_depth);}
    void box(std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t) override {}
    void sprite(std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t,std::uint32_t,std::uint32_t,std::uint32_t) override {}
};
struct DeathServices final : kaikeidou::BossPhaseServices, kaikeidou::Stage6BossTimelineServices,
    kaikeidou::BossPresentationServices, kaikeidou::DialogueServices, kaikeidou::SaveIO,
    kaikeidou::StageBackgroundServices, kaikeidou::MusicPresentationServices {
    State& state;
    kaikeidou::LifecycleContext& lifecycle;
    kaikeidou::StageTimeline& timeline;
    kaikeidou::PatternParameters& parameters;
    kaikeidou::Random& random;
    kaikeidou::GameMath& math;
    kaikeidou::RandomPrecision precision;
    bool enabled;
    DeathServices(State& world, kaikeidou::LifecycleContext& life, kaikeidou::StageTimeline& time,
        kaikeidou::PatternParameters& pattern, kaikeidou::Random& rng, kaikeidou::GameMath& functions,
        kaikeidou::RandomPrecision bits, bool active)
        : state(world), lifecycle(life), timeline(time), parameters(pattern), random(rng), math(functions), precision(bits), enabled(active) {}
    void synchronize() {
        auto& b=state.boss;
        b.death.stage=lifecycle.stage; b.death.part=timeline.part; b.death.subphase=timeline.subphase;
        b.death.character=lifecycle.character; b.death.boss_state=lifecycle.boss_state;
        b.phase.hit_flag=lifecycle.hit_flag; b.phase.weapon=state.options_context.weapon;
        parameters.game_frame=timeline.frame;
    }
    void publish() {
        auto& b=state.boss;
        timeline.part=b.death.part; timeline.subphase=b.death.subphase; timeline.frame=parameters.game_frame;
        lifecycle.stage=b.death.stage; lifecycle.part=b.death.part; lifecycle.subphase=b.death.subphase; lifecycle.boss_state=b.death.boss_state;
        lifecycle.hit_flag=b.phase.hit_flag;
    }
    std::uint32_t color(std::uint32_t red, std::uint32_t green, std::uint32_t blue) override {
        return kaikeidou::game_color(red, green, blue, state.display_depth);
    }
    void resolve_enemy_death(std::uint32_t slot) override {
        synchronize();
        kaikeidou::resolve_enemy_death(state.enemies,slot,state.items,state.effects,state.shake,state.projectiles,
            state.player,parameters,state.boss.death,random,math,precision,*this);
        publish();
    }
    void advance_phase(std::uint32_t mode) override {
        if (!enabled) throw std::runtime_error("Boss phase requires extended world protocol");
        auto& b=state.boss;
        kaikeidou::advance_boss_phase(mode,state.enemies,state.player,state.items,state.effects,state.shake,state.projectiles,
            b.savedata,parameters,b.death,b.phase,random,math,precision,*this);
        publish();
    }
    std::int32_t allocate_boss(std::uint32_t kind) override {
        synchronize(); auto& b=state.boss;
        const auto slot=kaikeidou::allocate_boss(state.enemies,b.allocation,b.phase,b.death,b.protection,kind);
        publish(); return slot;
    }
    void configure_phase(std::uint32_t spell,std::uint32_t seconds,std::uint32_t hp,std::uint32_t index) override {
        synchronize(); auto& b=state.boss;
        kaikeidou::configure_boss_phase(state.enemies,b.allocation,b.phase,b.death,b.protection,b.presentation,
            parameters,*this,precision,spell,seconds,hp,index);
        publish();
    }
    void begin_boss_effect() override { kaikeidou::begin_boss_effect(state.boss.presentation); }
    void retire_boss(std::uint32_t mode) override {
        auto& b=state.boss;
        kaikeidou::retire_boss(mode,state.enemies,state.items,state.effects,state.shake,state.projectiles.sound_triggers,
            b.allocation,b.phase,b.presentation,*this);
    }
    void sync_background() {
        auto& context=state.background.context;
        context.stage=lifecycle.stage;context.part=timeline.part;context.character=lifecycle.character;
    }
    void restore_stage_music(std::uint32_t mode) override {
        ++state.music_events;
        if(state.resource_enabled){sync_background();kaikeidou::control_stage_movies(mode,state.background.context,*this);}
    }
    void pause_background() override {
        if(state.resource_enabled){sync_background();kaikeidou::pause_stage_background(state.background.context,*this);}
    }
    void prepare_spell_music() override { ++state.music_events;pause_background(); }
    void persist_spell_records(const kaikeidou::SaveData&) override { ++state.save_events; }
    void dialog(std::uint32_t mode) override {
        kaikeidou::initialize_dialogue(state.dialogue,mode);state.fire_lock=state.dialogue.fields.word(8);
    }
    void award_stage_clear() override {
        const kaikeidou::StageClearContext context{lifecycle.stage,lifecycle.difficulty,lifecycle.character,
            state.options_context.weapon,lifecycle.practice};
        kaikeidou::award_stage_clear(state.player,state.boss.savedata,context,*this);
    }
    void change_stage_resources(std::uint32_t stage) override {
        if(!state.resource_enabled)throw std::runtime_error("Stage resource integration requires captured platform handles and background state");
        lifecycle.stage=state.boss.death.stage;
        sync_background();
        kaikeidou::change_stage_resources(stage,state.background.context,state.background.mesh,state.player,random,precision,math,*this);
    }
    std::int32_t delete_graph(std::uint32_t handle,std::uint32_t flag) override {
        return static_cast<std::int32_t>(state.background.consume(state.resource_tick,timeline.frame,1,handle,flag));
    }
    std::uint32_t load_graph(std::string_view path,std::uint32_t flag) override {
        return state.background.consume(state.resource_tick,timeline.frame,2,0,flag,path);
    }
    std::uint32_t platform_random(std::uint32_t) override {throw std::runtime_error("Platform random input requires its original capture");}
    // 这些平台返回值在原调用点被丢弃；图形调用及顺序另有完整原函数对照。
    std::int32_t movie_seek(std::uint32_t,std::uint32_t) override {++state.music_events;return 0;}
    std::int32_t movie_play(std::uint32_t,std::uint32_t,std::uint32_t) override {++state.music_events;return 0;}
    std::int32_t movie_pause(std::uint32_t,std::uint32_t) override {++state.music_events;return 0;}
    std::int32_t set_viewport_matrix(const std::array<std::uint32_t,16>&) override {return 0;}
    bool open(std::string_view, bool writing) override { if(!writing)throw std::runtime_error("Unexpected save read");++state.save_events;return true; }
    std::size_t read(std::uint8_t*,std::size_t) override { throw std::runtime_error("Unexpected save read"); }
    void write(const std::uint8_t*,std::size_t) override {}
    void close() override {}
    void brightness(std::uint32_t,std::uint32_t,std::uint32_t) override {}
    void blend(std::uint32_t,std::uint32_t) override {}
    void sprite(std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t,std::uint32_t,std::uint32_t,std::uint32_t) override {}
    void text(std::uint32_t,std::uint32_t,std::string_view,std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t) override {}
    std::uint32_t music_platform(std::uint32_t api,const std::vector<std::uint32_t>& arguments,std::string_view path={}) {
        return state.presentation.consume(state.resource_tick,timeline.frame,1,api,arguments,&state.music,sizeof(state.music),path);
    }
    std::int32_t set_create_sound_data_type(std::uint32_t type) override {return presentation_result(music_platform(1,{type}));}
    std::int32_t stop_sound(std::uint32_t handle) override {return presentation_result(music_platform(2,{handle}));}
    std::int32_t delete_sound(std::uint32_t handle,std::uint32_t flag) override {return presentation_result(music_platform(3,{handle,flag}));}
    std::int32_t load_sound(std::string_view path,std::uint32_t buffers,std::uint32_t union_handle) override {return presentation_result(music_platform(4,{0,buffers,union_handle},path));}
    std::int32_t set_volume(std::uint32_t volume,std::uint32_t handle) override {return presentation_result(music_platform(5,{volume,handle}));}
    std::int32_t play_sound(std::uint32_t handle,std::uint32_t type,std::uint32_t top) override {return presentation_result(music_platform(6,{handle,type,top}));}
    void music(std::uint32_t track,std::uint32_t mode) override {
        ++state.music_events;
        if(!state.presentation_enabled)return;
        state.presentation.begin(state.resource_tick,timeline.frame,1,1,{track,mode},&state.music,sizeof(state.music));
        kaikeidou::switch_music(track,mode,state.music,state.stage6.outer_scene,lifecycle.scene,state.boss.savedata.options[1],static_cast<unsigned>(precision),*this);
        state.presentation.end(&state.music,sizeof(state.music));
    }
    std::uint32_t read_counter(std::uint32_t binding) override { return state.input.at(binding); }
};
struct EnemyServices final : kaikeidou::PlayerBombHitServices, kaikeidou::EnemyDamageServices {
    std::uint32_t overflow_row_word() override { throw std::runtime_error("MarisaC row[7] requires the original caller stack word"); }
    State& state;
    kaikeidou::PatternParameters& parameters;
    kaikeidou::Random& random;
    kaikeidou::GameMath& math;
    kaikeidou::RandomPrecision precision;
    DeathServices& presentation;
    kaikeidou::EnemyDamageContext& protection;
    kaikeidou::EnemyDeathContext& death;
    EnemyServices(State& world, kaikeidou::PatternParameters& pattern, kaikeidou::Random& rng,
        kaikeidou::GameMath& functions, kaikeidou::RandomPrecision bits, DeathServices& services)
        : state(world), parameters(pattern), random(rng), math(functions), precision(bits), presentation(services), protection(world.boss.protection), death(world.boss.death) {}
    void damage(std::uint32_t slot, std::uint32_t amount) override {
        kaikeidou::apply_enemy_damage(state.enemies, state.player, state.projectiles.sound_triggers, protection, slot, amount, *this);
    }
    void check_enemy_death(std::uint32_t slot) override {
        kaikeidou::resolve_enemy_death(state.enemies, slot, state.items, state.effects, state.shake, state.projectiles,
            state.player, parameters, death, random, math, precision, presentation);
    }
    void synchronize(const kaikeidou::LifecycleContext& lifecycle, const kaikeidou::StageTimeline& timeline) {
        death.stage = lifecycle.stage; death.part = timeline.part; death.subphase = timeline.subphase;
        death.character = lifecycle.character; death.boss_state = lifecycle.boss_state;
    }
};
template<class Action> void lifecycle_words(kaikeidou::LifecycleContext& context, Action action) {
    action(context.character); action(context.difficulty); action(context.practice); action(context.stage);
    action(context.part); action(context.subphase); action(context.boss_state); action(context.hit_flag);
    action(context.scene); action(context.scene_phase); action(context.scene_selection);
}
template<class Record> void read_raw(Record& record) {
    std::cin.read(reinterpret_cast<char*>(record.data.data()), record.data.size());
    if (!std::cin) throw std::runtime_error("World region truncated");
}
template<class Record> void write_raw(const Record& record) {
    std::cout.write(reinterpret_cast<const char*>(record.data.data()), record.data.size());
}
std::uint64_t read_quad() {
    const auto low = emitter_test::read(), high = emitter_test::read();
    return low | (static_cast<std::uint64_t>(high) << 32);
}
void write_quad(std::uint64_t value) {
    emitter_test::write(static_cast<std::uint32_t>(value)); emitter_test::write(static_cast<std::uint32_t>(value >> 32));
}
}
int main(int argc, char** argv) {
    std::uint32_t diagnostic_frame = 0;
    try {
    const bool eof_pair_world = argc == 2 && (std::string(argv[1]) == "--world-ex-eof-pair" || std::string(argv[1]) == "--world-corpus-eof-pair");
    const bool omniverse_world = argc == 2 && (std::string(argv[1]) == "--world-omniverse" || std::string(argv[1]) == "--world-omniverse-return");
    const bool ex_return_world = argc == 2 && (std::string(argv[1]) == "--world-ex-return" || std::string(argv[1]) == "--world-omniverse-return");
    const bool ex_world = omniverse_world || ex_return_world || (argc == 2 && (std::string(argv[1]) == "--world-ex-music" || std::string(argv[1]) == "--world-ex-eof" || std::string(argv[1]) == "--world-ex-eof-pair"));
    const bool corpus_eof_world = eof_pair_world || argc == 2 && (std::string(argv[1]) == "--world-corpus-eof" || std::string(argv[1]) == "--world-ex-eof");
    const bool presentation_world = ex_world || corpus_eof_world || (argc == 2 && std::string(argv[1]) == "--world-six-music");
    const bool full_six_world = presentation_world || (argc == 2 && std::string(argv[1]) == "--world-six");
    const bool extended_world = full_six_world || (argc == 2 && std::string(argv[1]) == "--world-stage3");
    const bool resource_world = extended_world || (argc == 2 && std::string(argv[1]) == "--world-resources");
    const bool dialogue_world = resource_world || (argc == 2 && std::string(argv[1]) == "--world-dialogue");
    const bool action_world = dialogue_world || (argc == 2 && std::string(argv[1]) == "--world-action");
    const bool replay_world = action_world || (argc == 2 && std::string(argv[1]) == "--world-replay");
    const bool boss_world = replay_world || (argc == 2 && std::string(argv[1]) == "--world-boss");
    const bool fire_world = boss_world || (argc == 2 && std::string(argv[1]) == "--world-fire");
    const bool shot_world = fire_world || (argc == 2 && std::string(argv[1]) == "--world-shots");
    const bool world = shot_world || (argc == 2 && std::string(argv[1]) == "--world");
    if (argc != 1 && !world) return 2;
    emitter_test::binary_streams();
    const auto end = emitter_test::read();
    const auto requested = emitter_test::read();
    std::vector<std::uint32_t> frames(requested);
    for (auto& frame : frames) frame = emitter_test::read();
    const auto difficulty = emitter_test::read();
    auto state = std::make_unique<State>();
    state->resource_enabled = resource_world;
    state->presentation_enabled = presentation_world;
    state->replay_playback = replay_world;
    std::cin.read(reinterpret_cast<char*>(state->player.data.data()), 240);
    read_records(state->enemies.enemies); read_records(state->projectiles.secondary); read_records(state->projectiles.bullets);
    for (auto& cursor : state->projectiles.cursors) cursor = emitter_test::read();
    for (auto& sound : state->projectiles.sound_triggers) sound = emitter_test::read();
    for (auto& input : state->input) input = emitter_test::read();
    kaikeidou::Random random(emitter_test::read());
    kaikeidou::LifecycleContext lifecycle;
    lifecycle.difficulty = difficulty;
    kaikeidou::PatternParameters parameters;
    parameters.difficulty = difficulty;
    if (world) {
        read_records(state->items.items); read_records(state->effects.effects); read_raw(state->shake);
        read_records(state->options.options);
        state->enemies.next_id = emitter_test::read();
        for (auto& value : parameters.integers) value = emitter_test::read();
        for (auto& value : parameters.reals) value = read_quad();
        lifecycle_words(lifecycle, [](std::uint32_t& value) { value = emitter_test::read(); });
        std::cin.read(reinterpret_cast<char*>(&lifecycle.flash), 1);
        state->display_depth = emitter_test::read(); state->display_renderer = emitter_test::read();
        state->options_context.weapon = emitter_test::read();
        for (auto& value : state->bindings) value = emitter_test::read();
        if (shot_world) { read_records(state->shots.common); read_records(state->shots.weapon); }
        if (fire_world) state->fire_lock = emitter_test::read();
        if (!std::cin || lifecycle.difficulty != difficulty) throw std::runtime_error("World metadata differs");
    }
    std::vector<std::array<std::uint32_t,16>> input_steps;
    std::vector<std::uint32_t> replay_steps;
    std::uint32_t replay_input_count=0,replay_runtime_mode=1,replay_eof_flags=0;
    if (boss_world) {
        state->boss.read(std::cin);
        if (action_world) state->action.read(std::cin);
        if (dialogue_world) {
            read_raw(state->dialogue.fields);state->dialogue.word_a7e7fc=emitter_test::read();state->dialogue.word_a78e30=emitter_test::read();
            if(state->dialogue.fields.word(8)!=state->fire_lock)throw std::runtime_error("Dialogue fire lock differs from its alias");
        }
        if (resource_world) {
            state->background.read(std::cin);
            if(extended_world) {
                for(auto& value:state->extended_patterns.reals_5_to_9)value=read_quad();
                for(auto& value:state->extended_patterns.reals_20_to_29)value=read_quad();
            }
            if(full_six_world) {
                for(auto& value:state->stage4_patterns.integers_20_to_39)value=emitter_test::read();
                read_raw(state->stage6.vectors);read_raw(state->stage6.effect_prefix);read_raw(state->stage6.ending);
                state->stage6.outer_scene=emitter_test::read();
            }
            if(presentation_world){
                read_raw(state->music.banner);state->music.current_track=emitter_test::read();state->music.audio_handle=emitter_test::read();
                state->title.active=emitter_test::read();state->title.graph_handle=emitter_test::read();
                state->title.age=emitter_test::read();state->title.opacity=emitter_test::read();
            }
            if(ex_world){read_raw(state->ex_vectors);state->ex_title.read_initial(std::cin);}
            state->background.read_calls(std::cin);
            if(presentation_world)state->presentation.read(std::cin);
        }
        state->boss.phase.replay_variant = emitter_test::read()!=0;
        state->boss.phase.replay_mode = replay_world ? 1u : 0u;
        state->boss.phase.weapon=state->options_context.weapon;
        state->boss.savedata.bindings=state->bindings;
        input_steps.resize(end);
        replay_steps.resize(replay_world ? end : 0);
        for (std::size_t i=0;i<input_steps.size();++i) {
            for (auto& value : input_steps[i]) value=emitter_test::read();
            if(replay_world) replay_steps[i]=emitter_test::read();
        }
        if(state->boss.death.boss_state!=lifecycle.boss_state || state->boss.phase.hit_flag!=lifecycle.hit_flag)
            throw std::runtime_error("Boss globals disagree with common state");
    }
    if(corpus_eof_world){
        if(emitter_test::read()!=0x31464f45u)throw std::runtime_error("EOF input marker differs");
        replay_input_count=emitter_test::read();replay_runtime_mode=emitter_test::read();replay_eof_flags=emitter_test::read();
        if(end+1u!=replay_input_count||replay_runtime_mode!=1||replay_eof_flags!=0)
            throw std::runtime_error("EOF playback initial metadata differs");
    }
    if(ex_world){
        state->ex_title.require_initial_aliases(state->stage6.outer_scene,lifecycle.scene_phase,
            state->boss.stage.enemy_prefix.word(0x5c),state->boss.savedata.options[1]);
        state->ex_title.read_requests(std::cin);
        if(ex_return_world)state->ex_replay_tail.read(std::cin);
        if(omniverse_world)state->omniverse_drawing.read(std::cin);else state->ex_drawing.read(std::cin);
    }
    state->options_context.character = lifecycle.character;
    kaikeidou::RecoveredGameMath math;
    kaikeidou::StageTimeline timeline;
    timeline.difficulty = difficulty;
    if(boss_world) timeline.frame=state->boss.game_frame;
    timeline.part = lifecycle.part; timeline.subphase = lifecycle.subphase;
    Services services(state->projectiles, math);
    CollisionServices collision_services;
    kaikeidou::CollisionContext collision;
    collision.projectile_radii = kaikeidou::projectile_collision_radii;
    const auto precision = kaikeidou::RandomPrecision::bits24;
    PlayerServices player_services(*state, lifecycle, math, precision);
    ItemInput item_input(*state);
    FireInput fire_input(*state);
    MidbossInput midboss_input(*state);
    SpriteServices sprite_services;
    MeshServices mesh_services;
    EndingFadeServices ending_fade_services;
    kaikeidou::PlayerShotFireContext firing;
    firing.options = state->options_context;
    firing.fire_binding = state->bindings[4]; firing.slow_binding = state->bindings[6]; firing.fire_lock = state->fire_lock;
    firing.replay_variant = boss_world && state->boss.phase.replay_variant;
    firing.replay_mode = replay_world ? 1u : 0u;
    kaikeidou::ItemPickupServices pickup_services;
    DeathServices death_services(*state,lifecycle,timeline,parameters,random,math,precision,boss_world);
    kaikeidou::ExBossEarlyTimelineRuntimeServices<DeathServices> ex_boss_services(death_services);
    kaikeidou::ExBossMiddleTimelineRuntimeServices<DeathServices> ex_middle_services(death_services);
    kaikeidou::ExBossControlRuntimeServices ex_control_services;
    kaikeidou::ExBossLateTimelineProbeServices<DeathServices> ex_late_services(death_services);
    kaikeidou::ExBossHeavyRuntimeServices<DeathServices> ex_heavy_services(death_services);
    kaikeidou::OmniverseWorldRuntimeServices omni_services(death_services,ex_late_services);
    kaikeidou::OmniverseBehaviorContext omni_context{state->effects,state->stage6.effect_prefix,state->shake,
        state->player,parameters,state->boss.stage,state->background.context};
    player_services.on_music=[&](std::uint32_t track,std::uint32_t mode){death_services.music(track,mode);};
    const auto publish_title_menu=[&](){
        auto& title=state->ex_title;
        state->stage6.outer_scene=title.menu.outer_scene;lifecycle.scene_phase=title.menu.phase;
        state->boss.stage.enemy_prefix.word(0x58,title.menu.phase);state->boss.stage.enemy_prefix.word(0x5c,title.menu.timer);
        state->boss.phase.replay_mode=title.title.replay_mode;
        state->replay_playback=title.title.replay_mode!=0;firing.replay_mode=title.title.replay_mode;
    };
    kaikeidou::ExTitleProbeServices<DeathServices> ex_title_services(death_services,publish_title_menu);
    if(ex_world){
        ex_boss_services.pixel=[&](std::uint32_t x,std::uint32_t y,std::uint32_t color){
            state->ex_drawing.consume(state->resource_tick,timeline.frame,lifecycle.stage,timeline.part,
                timeline.subphase,lifecycle.character,{x,y,color},0x7f);
        };
        ex_middle_services.line=[&](std::uint32_t x1,std::uint32_t y1,std::uint32_t x2,std::uint32_t y2,std::uint32_t color,std::uint32_t thickness){
            state->ex_drawing.consume(state->resource_tick,timeline.frame,lifecycle.stage,timeline.part,
                timeline.subphase,lifecycle.character,{x1,y1,x2,y2,color,thickness},0x7f);
        };
        ex_late_services.line=ex_middle_services.line;
        ex_control_services.circle=[&](std::uint32_t x,std::uint32_t y,std::uint32_t radius,std::uint32_t color,std::uint32_t filled){
            return state->ex_drawing.consume(state->resource_tick,timeline.frame,lifecycle.stage,timeline.part,
                timeline.subphase,lifecycle.character,{x,y,radius,color,filled},0x7f);
        };
        ex_heavy_services.circle=ex_control_services.circle;
        ex_heavy_services.line=[&](std::uint32_t x1,std::uint32_t y1,std::uint32_t x2,std::uint32_t y2,std::uint32_t color,std::uint32_t width){
            return state->ex_drawing.consume(state->resource_tick,timeline.frame,lifecycle.stage,timeline.part,
                timeline.subphase,lifecycle.character,{x1,y1,x2,y2,color,width},0x7f);
        };
        ex_late_services.return_title=[&](bool replay_variant){
            auto& title=state->ex_title;
            title.menu.outer_scene=state->stage6.outer_scene;title.menu.phase=lifecycle.scene_phase;
            title.menu.timer=state->boss.stage.enemy_prefix.word(0x5c);title.title.replay_mode=state->boss.phase.replay_mode;
            death_services.sync_background();
            kaikeidou::return_menu_title(title.menu,title.title,state->background.context,ex_title_services,replay_variant);
            publish_title_menu();
        };
        ex_late_services.title_visuals=[&](){
            auto& title=state->ex_title;
            title.begin(state->resource_tick,timeline.frame,omniverse_world?0x68a7d2u:0x5e2630u,state->boss.savedata.options[1],24);
            kaikeidou::initialize_menu_title(title.title,title.menu,state->music,lifecycle.scene,state->boss.savedata,24,ex_title_services);
            ex_title_services.finish_music();title.end(state->boss.savedata.options[1]);
        };
    }
    if(omniverse_world){
        if(lifecycle.stage!=7)throw std::runtime_error("Omniverse input stage differs");
        omni_services.line=[&](std::uint32_t x1,std::uint32_t y1,std::uint32_t x2,std::uint32_t y2,std::uint32_t color,std::uint32_t thickness){
            state->omniverse_drawing.consume(state->resource_tick,timeline.frame,lifecycle.stage,timeline.part,
                timeline.subphase,lifecycle.character,{x1,y1,x2,y2,color,thickness},0x7f);
        };
        omni_services.circle=[&](std::uint32_t x,std::uint32_t y,std::uint32_t radius,std::uint32_t color,std::uint32_t fill){
            state->omniverse_drawing.consume(state->resource_tick,timeline.frame,lifecycle.stage,timeline.part,
                timeline.subphase,lifecycle.character,{x,y,radius,color,fill},0x7f);
        };
    }
    TitleResources title_resources(*state,timeline);
    TitleGraphics title_graphics(*state);
    services.on_music=[&](std::uint32_t track,std::uint32_t mode){death_services.music(track,mode);};
    services.on_title=[&](){
        if(!presentation_world)return;
        state->presentation.begin(state->resource_tick,timeline.frame,2,2,{},&state->title,sizeof(state->title));
        kaikeidou::start_stage_title(state->title,lifecycle.stage,title_resources);
        state->presentation.end(&state->title,sizeof(state->title));
    };
    EnemyServices enemy_services(*state, parameters, random, math, precision, death_services);
    services.on_kill=[&](std::uint32_t index){
        death_services.synchronize();enemy_services.synchronize(lifecycle,timeline);
        enemy_services.check_enemy_death(index);
    };
    const auto draw_eof_presentation=[&](){
        kaikeidou::GameplayPresentationDispatchContext drawing;
        const auto refresh=[&](){
            drawing.tint=lifecycle.flash;drawing.scene=lifecycle.scene;drawing.practice=lifecycle.practice;
            drawing.slow_binding=state->bindings[6];drawing.player_x_bits=state->player.quad(0x90);
            drawing.player_y_bits=state->player.quad(0x98);
        };
        kaikeidou::GameplayPresentationProbeServices rendering(ending_fade_services);
        rendering.input=[&](std::uint32_t binding){return presentation_result(state->input_value(binding));};
        rendering.component=[&](kaikeidou::GameplayPresentationComponent component){
            using Component=kaikeidou::GameplayPresentationComponent;
            if(component==Component::routine_611db0){
                death_services.synchronize();death_services.sync_background();
                if(kaikeidou::background_mesh_presentation_enabled(state->boss.savedata.options[3],
                        lifecycle.stage,timeline.part,timeline.subphase,lifecycle.boss_state))
                    kaikeidou::present_background_meshes(state->background.mesh,lifecycle.scene,24,mesh_services);
            }else if(component==Component::routine_619b80){
                kaikeidou::draw_boss_sprites(state->enemies,state->boss.allocation,state->boss.death,
                    parameters,random,sprite_services,24);
            }else if(component==Component::routine_61d450){
                kaikeidou::update_projectile_presentation(state->projectiles,random,precision,true);
            }else if(component==Component::routine_644bb0){
                kaikeidou::DialogueContext context;
                context.character=lifecycle.character;context.stage=lifecycle.stage;context.scene=lifecycle.scene;
                context.subphase=timeline.subphase;context.game_frame=timeline.frame;
                context.confirm_binding=state->bindings[4];context.skip_binding=state->bindings[8];
                context.replay_variant=true;context.replay_mode=replay_runtime_mode;context.replay_buttons=state->replay_buttons;
                kaikeidou::update_ex_dialogue(state->dialogue,context,state->enemies,state->boss.stage.enemy_prefix,
                    state->boss.allocation,state->projectiles,state->player,death_services,24,parameters,state->items,random,math);
                lifecycle.subphase=timeline.subphase=context.subphase;timeline.frame=context.game_frame;
                state->fire_lock=state->dialogue.fields.word(8);state->replay_buttons=context.replay_buttons;
            }else if(component==Component::routine_613250){
                kaikeidou::present_stage_title(state->title,title_graphics,24);
                kaikeidou::update_stage_clear_presentation(state->player);
                kaikeidou::update_music_presentation(state->music,24);
            }
            refresh();
        };
        refresh();kaikeidou::dispatch_gameplay_presentation(drawing,state->stage6.ending,rendering);
    };
    kaikeidou::PlayerShotHitContext hit;
    const auto write_snapshot = [&](std::uint32_t sample_frame) {
        emitter_test::write(sample_frame);
        std::cout.write(reinterpret_cast<const char*>(state->player.data.data()), 240);
        write_records(state->enemies.enemies); write_records(state->projectiles.secondary); write_records(state->projectiles.bullets);
        for (auto cursor : state->projectiles.cursors) emitter_test::write(cursor);
        for (auto sound : state->projectiles.sound_triggers) emitter_test::write(sound);
        for (auto input : state->input) emitter_test::write(input);
        emitter_test::write(random.cursor());
        if (world) {
            write_records(state->items.items); write_records(state->effects.effects); write_raw(state->shake);
            write_records(state->options.options);
            emitter_test::write(state->enemies.next_id);
            for (auto value : parameters.integers) emitter_test::write(value);
            for (auto value : parameters.reals) write_quad(value);
            lifecycle_words(lifecycle, [](std::uint32_t value) { emitter_test::write(value); });
            std::cout.write(reinterpret_cast<const char*>(&lifecycle.flash), 1);
            emitter_test::write(state->display_depth); emitter_test::write(state->display_renderer);
            emitter_test::write(state->options_context.weapon);
            for (auto value : state->bindings) emitter_test::write(value);
            if (shot_world) { write_records(state->shots.common); write_records(state->shots.weapon); }
            if (fire_world) emitter_test::write(state->fire_lock);
            if (boss_world) { state->boss.game_frame=timeline.frame;state->boss.write(std::cout); }
            if (action_world) state->action.write(std::cout);
            if (dialogue_world) {
                write_raw(state->dialogue.fields);emitter_test::write(state->dialogue.word_a7e7fc);emitter_test::write(state->dialogue.word_a78e30);
            }
            if (resource_world) state->background.write(std::cout);
            if(extended_world) {
                for(auto value:state->extended_patterns.reals_5_to_9)write_quad(value);
                for(auto value:state->extended_patterns.reals_20_to_29)write_quad(value);
            }
            if(full_six_world) {
                for(auto value:state->stage4_patterns.integers_20_to_39)emitter_test::write(value);
                write_raw(state->stage6.vectors);write_raw(state->stage6.effect_prefix);write_raw(state->stage6.ending);
                emitter_test::write(state->stage6.outer_scene);
            }
            if(presentation_world){
                write_raw(state->music.banner);emitter_test::write(state->music.current_track);emitter_test::write(state->music.audio_handle);
                emitter_test::write(state->title.active);emitter_test::write(state->title.graph_handle);
                emitter_test::write(state->title.age);emitter_test::write(state->title.opacity);
            }
            if(ex_world)write_raw(state->ex_vectors);
        }
    };
    kaikeidou::DialogueContext shared_dialogue_context;
    kaikeidou::GameplayPresentationDispatchContext shared_gameplay_context;
    kaikeidou::ReplayPauseEntryContext shared_pause_context;
    kaikeidou::PlayerBombContext shared_bomb_context;
    std::uint32_t shared_practice_selected=0,shared_weapon=state->options_context.weapon;
    kaikeidou::GameStartEntryRefs shared_refs{state->player,state->enemies,state->projectiles,state->items,state->effects,state->shake,
        state->options,state->options_context,state->shots,state->action.bombs,firing,state->dialogue,shared_dialogue_context,lifecycle,
        parameters,state->boss.death,state->action.stage,state->extended_patterns,state->stage4_patterns,state->boss.allocation,
        state->boss.phase,state->boss.presentation,state->boss.stage,state->boss.protection,state->stage6,state->music,state->title,
        state->background.context,state->background.mesh,state->boss.savedata,random,state->ex_title.menu,shared_gameplay_context,
        shared_pause_context,shared_bomb_context,shared_practice_selected,shared_weapon};
    kaikeidou::GameplayFrameWorldView shared_world(shared_refs,state->ex_vectors,state->fire_lock,state->replay_buttons,
        state->replay_playback,state->bindings,state->input);
    auto* frame_world=&shared_world;
    std::vector<std::uint8_t> gameplay_replay_input(replay_steps.size());
    for(std::size_t i=0;i<replay_steps.size();++i)gameplay_replay_input[i]=static_cast<std::uint8_t>(replay_steps[i]);
    std::uint32_t gameplay_replay_buffer=0x20000000u,gameplay_replay_cursor=0,gameplay_replay_scratch=0,gameplay_replay_caption=0;
    std::uint32_t gameplay_replay_count=corpus_eof_world?replay_input_count:ex_return_world?static_cast<std::uint32_t>(state->ex_replay_tail.buffer.size()):omniverse_world?9000u:end+1u;
    auto& gameplay_buffer=ex_return_world?state->ex_replay_tail.buffer:gameplay_replay_input;
    if(ex_return_world){if(gameplay_buffer.size()<gameplay_replay_input.size()||!std::equal(gameplay_replay_input.begin(),gameplay_replay_input.end(),gameplay_buffer.begin()))throw std::runtime_error("Replay input bytes differ from retained full buffer");}
    kaikeidou::GameStartEntryMachineFrame gameplay_frame;
    gameplay_frame.stack_address=0x15000000u;gameplay_frame.stack.assign(256,0x5a);
    gameplay_frame.registers.esp=0x15000080u;gameplay_frame.registers.ebp=0x150000c0u;
    kaikeidou::GameStartEntryMemory gameplay_memory(random);
    gameplay_memory.bind(0xa8cd68,state->player.data);
    gameplay_memory.bind(0xa733dc,state->bindings);
    gameplay_memory.bind(0xb3e2e8,parameters.game_frame);
    gameplay_memory.bind(0xb43100,gameplay_replay_buffer);
    gameplay_memory.bind(0xb43104,gameplay_replay_scratch);
    gameplay_memory.bind(0xb43108,state->replay_buttons);
    kaikeidou::bind_replay_gameplay_input(gameplay_memory,firing,shared_dialogue_context);
    gameplay_memory.bind(0xb4310c,firing.replay_mode);
    gameplay_memory.bind(0xb43110,gameplay_replay_cursor);
    gameplay_memory.bind(0xb43a08,gameplay_replay_count);
    gameplay_memory.bind(0xb43a10,gameplay_replay_caption);
    gameplay_memory.bind(0xa78e2c,lifecycle.scene);
    gameplay_memory.bind(0xa73060,state->fire_lock);
    gameplay_memory.bind(0xa73060,firing.fire_lock);
    if(dialogue_world)gameplay_memory.bind(0xa73058,state->dialogue.fields.data);
    gameplay_memory.bind(0xb43c04,state->ex_title.title.accumulator);
    gameplay_memory.bind_bytes(gameplay_replay_buffer,gameplay_buffer.data(),gameplay_buffer.size());
    gameplay_memory.bind_bytes(gameplay_frame.stack_address,gameplay_frame.stack.data(),gameplay_frame.stack.size());
    gameplay_memory.seal();
    struct PhysicalMovementInput final:kaikeidou::PlayerShotFireInput {
        State& state;explicit PhysicalMovementInput(State& value):state(value){}
        std::uint32_t read_counter(std::uint32_t binding)override{return binding<state.input.size()?state.input[binding]:0xffffffffu;}
    } physical_movement(*state);
    kaikeidou::ReplayGameplayInputCounterAdapter<kaikeidou::GameStartEntryMemory> replay_counter(gameplay_memory,firing,physical_movement);
    const auto move_player=[&](){
        if(boss_world){kaikeidou::update_replay_gameplay_movement(gameplay_memory,physical_movement,firing.replay_variant,24);return;}
        auto& player=state->player;
        kaikeidou::PlayerMovement movement{player.word(0), player.word(4), player.word(0x88), player.quad(0x90), player.quad(0x98),
            player.quad(0xb0), player.quad(0xb8), player.quad(0xc0), player.quad(0xc8)};
        kaikeidou::MovementInput movement_input;
        if(replay_world) {
            const auto buttons=state->replay_buttons;
            movement_input.directions={{(buttons>>1)&1u,(buttons>>2)&1u,(buttons>>3)&1u,buttons&1u}};
            movement_input.slow=(buttons>>7)&1u;
        } else if(boss_world) {
            for(std::size_t i=0;i<4;++i) movement_input.directions[i]=state->input.at(state->bindings[i]);
            movement_input.slow=state->input.at(state->bindings[6]);
        }
        kaikeidou::update_movement(movement, movement_input, 24);
        player.word(0x88, movement.sprite); player.quad(0x90, movement.x); player.quad(0x98, movement.y);
    };
    const auto present_fps=[](){};
    using FrameCounter=decltype(replay_counter);
    using FrameMemory=decltype(gameplay_memory);
    using FrameSharedDialogue=decltype(shared_dialogue_context);
    using FrameSharedBomb=decltype(shared_bomb_context);
    using FrameMovement=decltype(move_player);
    using FrameFps=decltype(present_fps);
    using FrameType0=decltype(frame_world);
    using FrameType1=decltype(firing);
    using FrameType2=decltype(lifecycle);
    using FrameType3=decltype(timeline);
    using FrameType4=decltype(random);
    using FrameType5=decltype(precision);
    using FrameType6=decltype(player_services);
    using FrameType7=decltype(replay_world);
    using FrameType8=decltype(boss_world);
    using FrameType9=decltype(math);
    using FrameType10=decltype(fire_input);
    using FrameType11=decltype(parameters);
    using FrameType12=decltype(death_services);
    using FrameType13=decltype(resource_world);
    using FrameType14=decltype(services);
    using FrameType15=decltype(extended_world);
    using FrameType16=decltype(full_six_world);
    using FrameType17=decltype(ex_world);
    using FrameType18=decltype(ex_boss_services);
    using FrameType19=decltype(ex_middle_services);
    using FrameType20=decltype(ex_late_services);
    using FrameType21=decltype(omniverse_world);
    using FrameType22=decltype(omni_services);
    using FrameType23=decltype(enemy_services);
    using FrameType24=decltype(action_world);
    using FrameType25=decltype(dialogue_world);
    using FrameType26=decltype(ex_control_services);
    using FrameType27=decltype(omni_context);
    using FrameType28=decltype(ex_heavy_services);
    using FrameType29=decltype(midboss_input);
    using FrameType30=decltype(collision);
    using FrameType31=decltype(collision_services);
    using FrameType32=decltype(pickup_services);
    using FrameType33=decltype(item_input);
    using FrameType34=decltype(hit);
    using FrameType35=decltype(presentation_world);
    using FrameType36=decltype(title_resources);
    using FrameType37=decltype(mesh_services);
    using FrameType38=decltype(sprite_services);
    using FrameType39=decltype(title_graphics);
    using FrameType40=decltype(ending_fade_services);
    struct FrameContext {
        FrameMovement& move_player;
        FrameFps& present_fps;
        FrameCounter& replay_counter;
        FrameMemory& gameplay_memory;
        FrameSharedDialogue& dialogue_context;
        FrameSharedBomb& bomb_context;
        FrameType0& state;
        FrameType1& firing;
        FrameType2& lifecycle;
        FrameType3& timeline;
        FrameType4& random;
        FrameType5& precision;
        FrameType6& player_services;
        FrameType7& replay_world;
        FrameType8& boss_world;
        FrameType9& math;
        FrameType10& fire_input;
        FrameType11& parameters;
        FrameType12& death_services;
        FrameType13& resource_world;
        FrameType14& services;
        FrameType15& extended_world;
        FrameType16& full_six_world;
        FrameType17& ex_world;
        FrameType18& ex_boss_services;
        FrameType19& ex_middle_services;
        FrameType20& ex_late_services;
        FrameType21& omniverse_world;
        FrameType22& omni_services;
        FrameType23& enemy_services;
        FrameType24& action_world;
        FrameType25& dialogue_world;
        FrameType26& ex_control_services;
        FrameType27& omni_context;
        FrameType28& ex_heavy_services;
        FrameType29& midboss_input;
        FrameType30& collision;
        FrameType31& collision_services;
        FrameType32& pickup_services;
        FrameType33& item_input;
        FrameType34& hit;
        FrameType35& presentation_world;
        FrameType36& title_resources;
        FrameType37& mesh_services;
        FrameType38& sprite_services;
        FrameType39& title_graphics;
        FrameType40& ending_fade_services;
    } frame_context{move_player,present_fps,replay_counter,gameplay_memory,shared_dialogue_context,shared_bomb_context,frame_world,firing,lifecycle,timeline,random,precision,player_services,replay_world,boss_world,math,fire_input,parameters,death_services,resource_world,services,extended_world,full_six_world,ex_world,ex_boss_services,ex_middle_services,ex_late_services,omniverse_world,omni_services,enemy_services,action_world,dialogue_world,ex_control_services,omni_context,ex_heavy_services,midboss_input,collision,collision_services,pickup_services,item_input,hit,presentation_world,title_resources,mesh_services,sprite_services,title_graphics,ending_fade_services};
    kaikeidou::GameplayFramePresentationRegisters gameplay_presentation_registers;
    kaikeidou::GameplayFrameRuntime<FrameContext> gameplay_runtime(shared_refs,frame_context,gameplay_presentation_registers);
    std::size_t next = 0;
    for (std::uint32_t elapsed=0; elapsed<=end; ++elapsed) {
        const auto sample_frame=boss_world?elapsed:timeline.frame;
        diagnostic_frame = sample_frame;
        if(corpus_eof_world&&sample_frame==end){
            if(eof_pair_world)write_snapshot(sample_frame);
            if(lifecycle.scene!=0)throw std::runtime_error("EOF pause entry requires playing scene");
            state->boss.stage.enemy_prefix.word(0x2c,state->boss.stage.enemy_prefix.word(0x2c)+1u);
            state->projectiles.sound_triggers.fill(0);
            state->resource_tick=end+1u;death_services.synchronize();death_services.sync_background();
            kaikeidou::ReplayPauseEntryContext pause{state->bindings[7],lifecycle.scene,lifecycle.scene_phase,
                lifecycle.scene_selection,end,replay_input_count,replay_runtime_mode,replay_eof_flags};
            const auto publish=[&](){
                lifecycle.scene=pause.scene;lifecycle.scene_phase=pause.phase;lifecycle.scene_selection=pause.selection;
                state->boss.stage.enemy_prefix.word(0x58,pause.phase);state->boss.stage.enemy_prefix.word(0x60,pause.selection);
            };
            const auto collect=[&](){pause.scene=lifecycle.scene;pause.phase=lifecycle.scene_phase;pause.selection=lifecycle.scene_selection;};
            kaikeidou::ReplayPauseProbeServices<DeathServices> pause_services(death_services,publish,collect,draw_eof_presentation);
            if(!kaikeidou::enter_pause_from_scene_zero(pause,true,state->background.context,state->music,pause_services))
                throw std::runtime_error("Natural EOF gate did not enter pause");
            publish();replay_eof_flags=pause.eof_flags;
            if(ex_world)state->ex_title.title.eof_flags=pause.eof_flags;
            state->presentation.finish_tick(state->resource_tick);
            if(omniverse_world)state->omniverse_drawing.finish_tick(state->resource_tick);
            else if(ex_world)state->ex_drawing.finish_tick(state->resource_tick);
            if(ex_world)state->ex_title.finish_tick(state->resource_tick);
        }
        if (next < frames.size() && frames[next] == sample_frame) {
            write_snapshot(sample_frame);
            ++next;
        }
        if (sample_frame == end) break;
        if(boss_world)state->boss.stage.enemy_prefix.word(0x2c,state->boss.stage.enemy_prefix.word(0x2c)+1u);
        state->resource_tick=elapsed+1;
        if (boss_world) state->input=input_steps.at(elapsed);
        else if (fire_world) state->input.at(state->bindings[4]) = 1;
        if(replay_world) {
            state->replay_buttons=replay_steps.at(elapsed);
            firing.replay_buttons=state->replay_buttons;
            if(!action_world && (state->replay_buttons&0x10u)) throw std::runtime_error("Replay bombs require action world protocol");
        }
        state->projectiles.sound_triggers.fill(0);
        parameters.game_frame=timeline.frame;gameplay_replay_cursor=elapsed;
        const auto gameplay_outcome=gameplay_runtime.gameplay_body(gameplay_memory,gameplay_frame);
        if(gameplay_outcome.kind!=kaikeidou::OuterSceneCallKind::completed)throw std::runtime_error("Gameplay body transferred");
        const auto fade_before_presentation=gameplay_runtime.last_fade_before();
        if(presentation_world)state->presentation.finish_tick(state->resource_tick);
        if(omniverse_world)state->omniverse_drawing.finish_tick(state->resource_tick);
            else if(ex_world)state->ex_drawing.finish_tick(state->resource_tick);
        if(ex_world)state->ex_title.finish_tick(state->resource_tick);
        if(ex_return_world&&!state->replay_playback){
            if(fade_before_presentation==0||fade_before_presentation>=0x80000000u)
                throw std::runtime_error("EX closing replay tail requires its ending-fade ECX branch");
            kaikeidou::advance_gameplay_probe_replay_tail(state->ex_replay_tail,gameplay_runtime,gameplay_memory,
                gameplay_frame,state->resource_tick,timeline.frame);
        }
        ++timeline.frame;
        if(boss_world) {
            auto& b=state->boss;
            b.phase.hit_flag=lifecycle.hit_flag; b.death.boss_state=lifecycle.boss_state;
            b.stage.enemy_prefix.word(0x58,lifecycle.scene_phase);
            b.stage.enemy_prefix.word(0x60,lifecycle.scene_selection);
        }
    }
    if (next != frames.size()) return 3;
    if(resource_world && state->background.next_call!=state->background.calls.size())
        throw std::runtime_error("Captured resource calls remain unconsumed");
    if(presentation_world)state->presentation.finish();
    if(omniverse_world)state->omniverse_drawing.finish();else if(ex_world)state->ex_drawing.finish();
    if(ex_world)state->ex_title.finish();
    if(ex_return_world)state->ex_replay_tail.write(std::cout);
    if(corpus_eof_world){
        emitter_test::write(0x52464f45u);emitter_test::write(end);emitter_test::write(replay_input_count);
        emitter_test::write(replay_runtime_mode);emitter_test::write(replay_eof_flags);
    }
    } catch (const std::exception& error) {
        std::cout.flush();
        std::cerr << "frame " << diagnostic_frame << ": " << error.what() << '\n';
        return 4;
    }
}
