// 已恢复六面、EX和Omniverse的共同逻辑/展示帧；外层负责输入发布、Replay尾及帧递增。
#pragma once
#include "core/frontend/ReplayGameplayInputCounterAdapter.hpp"
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
#include "core/player/PlayerBomb.hpp"
#include "core/player/PlayerBombHitServices.hpp"
#include "core/player/PlayerBombHit.hpp"
#include "core/stages/Stage1BossTimeline.hpp"
#include "core/stages/Stage1Dialogue.hpp"
#include "core/entities/EnemyBossNextBehavior.hpp"
#include "core/entities/FinalBossProjectileBehavior.hpp"
#include "core/stages/StageClear.hpp"
#include "core/stages/StageClearPresentation.hpp"
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
#include "core/runtime/GameplayPresentationDispatch.hpp"
#include "core/stages/ExBossEarlyTimeline.hpp"
#include "core/stages/ExBossOpeningProjectile.hpp"
#include "core/stages/ExBossMiddleProjectile.hpp"
#include "core/stages/ExBossEarlyFinal.hpp"
#include "core/stages/ExBossMiddleTimeline.hpp"
#include "core/stages/ExBossEnemies.hpp"
#include "core/stages/ExBossControlProjectiles.hpp"
#include "core/stages/ExBossLateTimeline.hpp"
#include "core/frontend/MenuTitleReturn.hpp"
#include "core/stages/ExBossHeavy.hpp"
#include "core/stages/ExBossMiddleFinal.hpp"
#include "core/stages/ExBossLateOther.hpp"
#include "core/stages/OmniverseTimeline.hpp"
#include "core/stages/OmniverseEnemies.hpp"
#include "core/stages/OmniverseProjectiles.hpp"
#include "core/stages/ExDialogue.hpp"
#include "core/shared/BackgroundMoviePause.hpp"
#include "core/shared/MusicPresentation.hpp"
#include "core/stages/StageTitlePresentation.hpp"
#include <stdexcept>
#include <string>
namespace kaikeidou {
template<class C>auto begin_pixel_frame(C& c,int)->decltype(c.render_gameplay(),void()){c.render_gameplay();}
template<class C>void begin_pixel_frame(C&,...){ }
template<class C>auto end_pixel_frame(C& c,int)->decltype(c.finish_gameplay_render(),void()){c.finish_gameplay_render();}
template<class C>void end_pixel_frame(C&,...){ }

// Context以引用提供下列状态和服务，与菜单/暂停使用相同生产对象。
template<class Context>std::uint32_t execute_gameplay_frame(Context& frame_context){
    auto& state=frame_context.state;
    auto& firing=frame_context.firing;
    auto& lifecycle=frame_context.lifecycle;
    auto& timeline=frame_context.timeline;
    auto& random=frame_context.random;
    auto& precision=frame_context.precision;
    auto& player_services=frame_context.player_services;
    auto& replay_world=frame_context.replay_world;
    auto& boss_world=frame_context.boss_world;
    auto& math=frame_context.math;
    auto& fire_input=frame_context.fire_input;
    auto& parameters=frame_context.parameters;
    auto& death_services=frame_context.death_services;
    auto& resource_world=frame_context.resource_world;
    auto& services=frame_context.services;
    auto& extended_world=frame_context.extended_world;
    auto& full_six_world=frame_context.full_six_world;
    auto& ex_world=frame_context.ex_world;
    auto& ex_boss_services=frame_context.ex_boss_services;
    auto& ex_middle_services=frame_context.ex_middle_services;
    auto& ex_late_services=frame_context.ex_late_services;
    auto& omniverse_world=frame_context.omniverse_world;
    auto& omni_services=frame_context.omni_services;
    auto& enemy_services=frame_context.enemy_services;
    auto& action_world=frame_context.action_world;
    auto& dialogue_world=frame_context.dialogue_world;
    auto& ex_control_services=frame_context.ex_control_services;
    auto& omni_context=frame_context.omni_context;
    auto& ex_heavy_services=frame_context.ex_heavy_services;
    auto& midboss_input=frame_context.midboss_input;
    auto& collision=frame_context.collision;
    auto& collision_services=frame_context.collision_services;
    auto& pickup_services=frame_context.pickup_services;
    auto& item_input=frame_context.item_input;
    auto& hit=frame_context.hit;
    auto& presentation_world=frame_context.presentation_world;
    auto& title_resources=frame_context.title_resources;
    auto& mesh_services=frame_context.mesh_services;
    auto& sprite_services=frame_context.sprite_services;
    auto& title_graphics=frame_context.title_graphics;
    auto& ending_fade_services=frame_context.ending_fade_services;
        firing.fire_lock=state->fire_lock;
        auto& player = state->player;
        lifecycle.part = timeline.part; lifecycle.subphase = timeline.subphase;
        lifecycle.secondary_first_field_28 = state->projectiles.secondary[0].quad(0x28);
        kaikeidou::update_player_lifecycle(player, lifecycle, random, precision, player_services);
        state->projectiles.secondary[0].quad(0x28, lifecycle.secondary_first_field_28);
        frame_context.move_player();
        kaikeidou::OptionsMotionInput options_input;
        options_input.bindings = {{state->bindings[0], state->bindings[1], state->bindings[3], state->bindings[6]}};
        options_input.logical_input = state->input;
        if(replay_world) for(std::uint32_t i=0;i<options_input.logical_input.size();++i)
            options_input.logical_input[i]=state->input_value(i);
        kaikeidou::update_player_options(state->options, player, state->options_context, options_input, static_cast<unsigned>(precision), math);
        kaikeidou::update_player_shots(state->shots, state->options, state->enemies, state->options_context, precision, math,
            boss_world ? &state->boss.stage.enemy_prefix : nullptr,&player);
        frame_context.replay_counter.begin_fire();
        kaikeidou::fire_player_shots(state->shots, player, state->options, state->enemies, firing, frame_context.replay_counter, state->projectiles.sound_triggers, math, precision);
        frame_context.gameplay_memory.reconcile();
        if(action_world) {
            auto& bomb_context=frame_context.bomb_context;
            bomb_context.bomb_binding=state->bindings[5];bomb_context.screen_tint=lifecycle.flash;
            frame_context.replay_counter.begin_bomb();
            kaikeidou::update_player_bombs(player,state->action.bombs,bomb_context,firing,state->boss.phase,
                state->boss.protection,state->enemies,state->shake,random,frame_context.replay_counter,math,static_cast<unsigned>(precision));
            frame_context.gameplay_memory.reconcile();
            lifecycle.flash=bomb_context.screen_tint;
        }
        parameters.game_frame = timeline.frame;
        death_services.synchronize();
        if (resource_world && lifecycle.stage==1 && timeline.part==0)
            kaikeidou::stage2_first_segment(timeline,state->enemies,parameters,state->background.context,services,math,static_cast<unsigned>(precision));
        else if (resource_world && lifecycle.stage==1 && timeline.part==1)
            kaikeidou::stage2_second_segment(timeline,state->enemies,state->projectiles,state->effects,
                parameters,state->boss.phase,state->boss.stage,random,precision,death_services);
        else if (resource_world && lifecycle.stage==1 && timeline.part==2)
            kaikeidou::stage2_third_segment(timeline,state->enemies,state->projectiles,parameters,random,precision);
        else if (resource_world && lifecycle.stage==1 && timeline.part==3)
            kaikeidou::stage2_boss_segment(timeline,state->enemies,state->projectiles,state->effects,player,parameters,
                state->boss.phase,state->boss.stage,state->action.stage,state->boss.death,state->background.context,
                precision,death_services,death_services);
        else if (resource_world && lifecycle.stage==2 && timeline.part==0)
            kaikeidou::stage3_first_segment(timeline,state->enemies,state->projectiles,parameters,
                state->boss.stage,random,math,precision,services);
        else if (resource_world && lifecycle.stage==2 && timeline.part==1)
            kaikeidou::stage3_second_segment(timeline,state->enemies,state->projectiles,state->effects,
                parameters,state->boss.phase,state->boss.stage,state->action.stage,death_services);
        else if (resource_world && lifecycle.stage==2 && timeline.part==2)
            kaikeidou::stage3_third_segment(timeline,state->enemies,state->projectiles,parameters,random,math,precision);
        else if (resource_world && lifecycle.stage==2 && timeline.part==3) {
            if(!extended_world)throw std::runtime_error("Stage3 boss requires the extended pattern capture");
            kaikeidou::stage3_boss_segment(timeline,state->enemies,state->projectiles,state->effects,player,parameters,
                state->boss.phase,state->boss.stage,state->action.stage,state->extended_patterns,state->boss.death,
                state->background.context,random,precision,math,death_services,death_services);
        }
        else if (full_six_world && lifecycle.stage==3 && timeline.part==0)
            kaikeidou::stage4_first_segment(timeline,state->enemies,state->projectiles,parameters,
                state->boss.stage,random,math,precision,services);
        else if (full_six_world && lifecycle.stage==3 && timeline.part==1)
            kaikeidou::stage4_midboss_segment(timeline,state->enemies,state->projectiles,state->effects,
                parameters,state->boss.phase,state->boss.stage,state->boss.allocation,state->action.stage,
                state->stage4_patterns,lifecycle.character,death_services,static_cast<unsigned>(precision));
        else if (full_six_world && lifecycle.stage==3 && timeline.part==2)
            kaikeidou::stage4_third_segment(timeline,state->enemies,state->projectiles,parameters,random,math,precision);
        else if (full_six_world && lifecycle.stage==3 && timeline.part==3)
            kaikeidou::stage4_boss_segment(timeline,state->enemies,state->projectiles,state->effects,player,parameters,
                state->boss.phase,state->boss.allocation,state->boss.stage,state->action.stage,state->extended_patterns,
                state->stage4_patterns,state->boss.death,state->background.context,lifecycle.character,
                random,precision,math,death_services,death_services);
        else if (full_six_world && lifecycle.stage==4 && timeline.part==0)
            kaikeidou::stage5_first_segment(timeline,state->enemies,state->projectiles,parameters,
                state->boss.stage,state->background.context,player,random,math,precision,services);
        else if (full_six_world && lifecycle.stage==4 && timeline.part==1)
            kaikeidou::stage5_midboss_segment(timeline,state->enemies,state->projectiles,state->effects,
                parameters,state->boss.phase,state->boss.stage,state->boss.allocation,state->boss.presentation,
                state->action.stage,state->background.context,math,death_services,static_cast<unsigned>(precision));
        else if (full_six_world && lifecycle.stage==4 && timeline.part==2)
            kaikeidou::stage5_third_segment(timeline,state->enemies,state->projectiles,parameters,
                state->boss.stage,state->background.context,random,math,precision);
        else if (full_six_world && lifecycle.stage==4 && timeline.part==3)
            kaikeidou::stage5_boss_segment(timeline,state->enemies,state->projectiles,state->effects,player,
                parameters,state->boss.phase,state->boss.allocation,state->boss.presentation,state->boss.protection,
                state->boss.stage,state->action.stage,state->extended_patterns,state->boss.death,
                state->background.context,random,precision,math,death_services,death_services);
        else if (full_six_world && lifecycle.stage==5 && timeline.part==0)
            kaikeidou::stage6_first_segment(timeline,state->enemies,state->projectiles,parameters,
                state->boss.stage,state->action.stage,random,services,precision);
        else if (full_six_world && lifecycle.stage==5 && timeline.part==1)
            kaikeidou::stage6_midboss_segment(timeline,state->enemies,state->projectiles,state->effects,player,
                parameters,state->boss.phase,state->boss.stage,state->boss.allocation,state->boss.protection,
                lifecycle.character,random,precision,math,death_services);
        else if (full_six_world && lifecycle.stage==5 && (timeline.part==2||timeline.part==3))
            kaikeidou::stage6_boss_segment(timeline,state->enemies,state->projectiles,state->effects,state->items,player,
                parameters,state->boss.phase,state->boss.allocation,state->boss.protection,state->boss.stage,
                state->action.stage,state->extended_patterns,state->stage6,state->boss.death,state->background.context,
                lifecycle.character,random,precision,math,death_services);
        else if (full_six_world && lifecycle.stage==6 && timeline.part==0)
            kaikeidou::ex_first_segment(timeline,state->enemies,state->projectiles,parameters,
                state->boss.stage,state->action.stage,random,precision,math,services,fire_input,
                {{state->bindings[0],state->bindings[1],state->bindings[2],state->bindings[3],state->bindings[6]}},firing);
        else if (full_six_world && lifecycle.stage==6 && timeline.part==1)
            kaikeidou::ex_midboss_segment(timeline,state->enemies,state->projectiles,state->effects,player,
                parameters,state->boss.phase,state->boss.stage,random,precision,math,death_services);
        else if (full_six_world && lifecycle.stage==6 && timeline.part==2)
            kaikeidou::ex_third_segment(timeline,state->enemies,state->projectiles,parameters,
                state->boss.stage,state->action.stage,random,precision,math);
        else if (full_six_world && lifecycle.stage==6 && timeline.part==3 && timeline.subphase<=8)
            kaikeidou::ex_boss_early_segment(timeline,state->enemies,state->projectiles,state->effects,player,
                parameters,state->boss.phase,state->boss.allocation,state->boss.protection,state->boss.stage,
                state->action.stage,state->extended_patterns,state->boss.death,lifecycle.character,random,precision,math,
                ex_boss_services,fire_input,{{state->bindings[0],state->bindings[1],state->bindings[2],state->bindings[3],state->bindings[6]}},firing);
        else if (full_six_world && lifecycle.stage==6 && timeline.part==3 && timeline.subphase<=15){
            if(!ex_world)throw std::runtime_error("EX middle timeline requires 71-region drawing protocol");
            kaikeidou::ex_boss_middle_segment(timeline,state->enemies,state->projectiles,state->effects,player,
                parameters,state->boss.phase,state->boss.allocation,state->boss.protection,state->boss.stage,
                state->action.stage,state->extended_patterns,state->boss.death,lifecycle.character,random,precision,math,
                ex_middle_services,state->ex_vectors,state->stage6.effect_prefix);
        }
        else if (full_six_world && lifecycle.stage==6 && timeline.part==3 && timeline.subphase<=25){
            if(!ex_world)throw std::runtime_error("EX late timeline requires 71-region drawing protocol");
            kaikeidou::ex_boss_late_segment(timeline,state->enemies,state->projectiles,state->effects,player,
                parameters,state->boss.phase,state->boss.allocation,state->boss.protection,state->boss.stage,
                state->action.stage,state->extended_patterns,state->boss.death,lifecycle.character,random,precision,math,
                ex_late_services,state->items,state->boss.presentation,state->background.context,state->shake,state->stage6,
                fire_input,{{state->bindings[0],state->bindings[1],state->bindings[2],state->bindings[3],state->bindings[6]}},firing);
        }
        else if (omniverse_world && lifecycle.stage==7){
            kaikeidou::OmniverseTimelineState omni_state{timeline,state->enemies,state->projectiles,state->effects,player,
                state->options,state->shots,state->items,parameters,state->boss.phase,state->boss.allocation,
                state->boss.protection,state->boss.stage,state->action.stage,state->boss.presentation,lifecycle,state->stage6,state->shake};
            kaikeidou::update_omniverse_timeline(omni_state,random,precision,math,omni_services,fire_input,state->bindings,firing);
            lifecycle.scene_phase=state->boss.stage.enemy_prefix.word(0x58);
            lifecycle.scene_selection=state->boss.stage.enemy_prefix.word(0x60);
        }
        else if (lifecycle.stage!=0) throw std::runtime_error("Stage timeline remains to be recovered");
        else if (timeline.part==0) kaikeidou::stage1_first_segment(timeline, state->enemies, services, static_cast<unsigned>(precision));
        else if (boss_world && timeline.part==1) kaikeidou::stage1_second_segment(timeline,state->enemies,state->projectiles,
            state->effects,parameters,state->boss.phase,state->boss.stage,random,math,precision,death_services);
        else if (action_world && timeline.part==2) kaikeidou::stage1_third_segment(timeline,state->enemies,parameters,
            state->action.stage,state->boss.stage.enemy_prefix,services,static_cast<unsigned>(precision));
        else if (dialogue_world && timeline.part==3) kaikeidou::stage1_boss_segment(timeline,state->enemies,state->projectiles,
            state->effects,player,parameters,state->boss.phase,state->boss.stage,state->action.stage,state->boss.death,
            random,math,precision,death_services);
        else throw std::runtime_error("Stage segment remains to be recovered");
        lifecycle.part=timeline.part; lifecycle.subphase=timeline.subphase;
        death_services.synchronize();enemy_services.synchronize(lifecycle,timeline);
        firing.fire_lock=state->fire_lock;
        for (auto& enemy : state->enemies.enemies) if (enemy.word(0) == 1) {
            if (enemy.word(8) == 0) {}
            else if (enemy.word(8) == 1)
                kaikeidou::update_opening_enemy(enemy, state->projectiles, player.quad(0x90), player.quad(0x98), parameters, random, precision, math);
            else if (enemy.word(8) == 2)
                kaikeidou::update_second_enemy(enemy, state->projectiles, player.quad(0x90), player.quad(0x98), parameters, random, precision, math);
            else if (enemy.word(8) == 3)
                kaikeidou::update_third_enemy(enemy, state->projectiles, player.quad(0x90), player.quad(0x98), parameters, random, precision, math);
            else if (enemy.word(8) == 4)
                kaikeidou::update_fourth_enemy(enemy, state->projectiles, player.quad(0x90), player.quad(0x98), parameters, random, precision, math);
            else if (enemy.word(8) == 5)
                kaikeidou::update_fifth_enemy(enemy, static_cast<unsigned>(precision), math);
            else if (enemy.word(8) == 6)
                kaikeidou::update_sixth_enemy(enemy, state->projectiles, player.quad(0x90), player.quad(0x98), parameters, random, precision, math);
            else if (enemy.word(8) == 7)
                kaikeidou::update_seventh_enemy(enemy, state->projectiles, player.quad(0x90), player.quad(0x98), parameters, random, precision, math);
            else if (enemy.word(8) == 8)
                kaikeidou::update_eighth_enemy(enemy, state->projectiles, player.quad(0x90), player.quad(0x98), parameters, random, precision, math);
            else if (enemy.word(8) == 9)
                kaikeidou::update_ninth_enemy(enemy, state->projectiles, player.quad(0x90), player.quad(0x98), parameters, random, precision, math);
            else if (enemy.word(8) == 10)
                kaikeidou::update_tenth_enemy(enemy, state->projectiles, player.quad(0x90), player.quad(0x98), parameters, random, precision, math);
            else if (enemy.word(8) == 11)
                kaikeidou::update_eleventh_enemy(enemy, state->projectiles, player.quad(0x90), player.quad(0x98), parameters, random, precision, math);
            else if (enemy.word(8) == 12)
                kaikeidou::update_twelfth_enemy(enemy, state->projectiles, player.quad(0x90), player.quad(0x98), parameters, random, precision, math);
            else if (enemy.word(8) == 13)
                kaikeidou::update_thirteenth_enemy(enemy, state->projectiles, player.quad(0x90), player.quad(0x98), parameters, random, precision, math);
            else if(boss_world && enemy.word(8)<=17) kaikeidou::update_boss_enemy(enemy.word(8),enemy,state->projectiles,
                player.quad(0x90),player.quad(0x98),parameters,state->boss.stage,random,precision,math);
            else if(action_world && enemy.word(8)<=21) kaikeidou::update_later_enemy(enemy.word(8),enemy,state->projectiles,
                player.quad(0x90),player.quad(0x98),parameters,random,precision,math);
            else if(dialogue_world && enemy.word(8)==22)kaikeidou::update_twenty_second_enemy(enemy,precision);
            else if(dialogue_world && enemy.word(8)==23)kaikeidou::update_twenty_third_enemy(enemy,state->projectiles,state->effects,
                player.quad(0x90),player.quad(0x98),parameters,random,precision,math,death_services);
            else if(resource_world && enemy.word(8)>=24 && enemy.word(8)<=28)
                kaikeidou::update_stage2_first_enemy(enemy.word(8),enemy,state->projectiles,
                    player.quad(0x90),player.quad(0x98),parameters,random,precision,math);
            else if(resource_world && enemy.word(8)==29)
                kaikeidou::update_stage2_midboss_enemy(enemy,state->projectiles,player.quad(0x90),player.quad(0x98),
                    parameters,state->boss.stage,random,precision,math);
            else if(resource_world && enemy.word(8)>=30 && enemy.word(8)<=35)
                kaikeidou::update_stage2_third_enemy(enemy.word(8),enemy,state->projectiles,player.quad(0x90),player.quad(0x98),
                    parameters,random,precision,math);
            else if(resource_world && enemy.word(8)>=36 && enemy.word(8)<=42) {
                const auto index=static_cast<std::size_t>(&enemy-state->enemies.enemies.data());
                auto* next_record=index+1==state->enemies.enemies.size()?player.data.data():state->enemies.enemies[index+1].data.data();
                kaikeidou::update_stage2_boss_enemy(enemy.word(8),enemy,state->projectiles,state->effects,
                    player.quad(0x90),player.quad(0x98),parameters,state->boss.stage,state->action.stage,
                    random,precision,math,death_services,next_record);
            }
            else if(resource_world && enemy.word(8)>=43 && enemy.word(8)<=46)
                kaikeidou::update_stage3_first_enemy_early(enemy.word(8),enemy,state->projectiles,
                    player.quad(0x90),player.quad(0x98),parameters,random,precision,math);
            else if(resource_world && enemy.word(8)>=47 && enemy.word(8)<=50)
                kaikeidou::update_stage3_first_enemy_late(enemy.word(8),enemy,state->projectiles,
                    player.quad(0x90),player.quad(0x98),parameters,random,precision,math);
            else if(resource_world && enemy.word(8)>=51 && enemy.word(8)<=52)
                kaikeidou::update_stage3_midboss_enemy(enemy.word(8),enemy,state->projectiles,
                    player.quad(0x90),player.quad(0x98),parameters,random,precision,math);
            else if(resource_world && enemy.word(8)>=53 && enemy.word(8)<=54)
                kaikeidou::update_stage3_third_enemy(enemy.word(8),enemy,state->projectiles,
                    player.quad(0x90),player.quad(0x98),parameters,random,precision,math);
            else if(resource_world && enemy.word(8)>=55 && enemy.word(8)<=58)
                kaikeidou::update_stage3_boss_enemy_early(enemy.word(8),enemy,state->projectiles,
                    player.quad(0x90),player.quad(0x98),parameters,random,precision,math);
            else if(resource_world && enemy.word(8)==59)
                kaikeidou::update_stage3_boss_enemy_final(enemy,state->projectiles,state->effects,
                    player.quad(0x90),player.quad(0x98),parameters,state->action.stage,state->extended_patterns,
                    random,precision,math,death_services);
            else if(full_six_world && enemy.word(8)>=60 && enemy.word(8)<=67)
                kaikeidou::update_stage4_first_enemy(enemy.word(8),enemy,state->projectiles,
                    player.quad(0x90),player.quad(0x98),parameters,state->boss.stage,random,precision,math);
            else if(full_six_world && enemy.word(8)==68)
                kaikeidou::update_stage4_midboss_enemy(enemy,state->projectiles,player.quad(0x90),player.quad(0x98),
                    parameters,state->action.stage,state->stage4_patterns,random,precision,math);
            else if(full_six_world && enemy.word(8)>=69 && enemy.word(8)<=72)
                kaikeidou::update_stage4_third_enemy(enemy.word(8),enemy,state->projectiles,
                    player.quad(0x90),player.quad(0x98),parameters,random,precision,math);
            else if(full_six_world && enemy.word(8)>=73 && enemy.word(8)<=76)
                kaikeidou::update_stage4_boss_enemy_early(enemy.word(8),enemy,state->projectiles,
                    player.quad(0x90),player.quad(0x98),parameters,state->boss.stage,state->action.stage,
                    state->stage4_patterns,state->boss.death.pattern_integer_5,state->boss.allocation,random,precision,math);
            else if(full_six_world && enemy.word(8)>=77 && enemy.word(8)<=78)
                kaikeidou::update_stage4_boss_enemy_final(enemy.word(8),enemy,state->projectiles,
                    player.quad(0x90),player.quad(0x98),parameters,random,precision,math);
            else if(full_six_world && enemy.word(8)>=79 && enemy.word(8)<=84)
                kaikeidou::update_stage5_first_enemy(enemy.word(8),enemy,state->projectiles,player,
                    parameters,state->background.context,random,precision,math);
            else if(full_six_world && enemy.word(8)>=85 && enemy.word(8)<=86)
                kaikeidou::update_stage5_midboss_enemy(enemy.word(8),enemy,state->projectiles,state->effects,player,
                    parameters,state->background.context,random,precision,math,death_services);
            else if(full_six_world && enemy.word(8)>=87 && enemy.word(8)<=89)
                kaikeidou::update_stage5_third_enemy(enemy.word(8),enemy,state->projectiles,player,
                    parameters,state->background.context,random,precision,math);
            else if(full_six_world && enemy.word(8)>=90 && enemy.word(8)<=93)
                kaikeidou::update_stage5_boss_early_enemy(enemy.word(8),enemy,state->projectiles,player,
                    parameters,random,precision,math);
            else if(full_six_world && enemy.word(8)>=94 && enemy.word(8)<=98)
                kaikeidou::update_stage5_boss_final_enemy(enemy.word(8),enemy,state->projectiles,player,
                    parameters,state->boss.stage,random,precision,math);
            else if(full_six_world && enemy.word(8)>=99 && enemy.word(8)<=103)
                kaikeidou::update_stage6_first_enemy(enemy.word(8),enemy,state->projectiles,player,
                    parameters,state->boss.stage,random,precision,math);
            else if(full_six_world && enemy.word(8)>=106 && enemy.word(8)<=108)
                kaikeidou::update_stage6_midboss_enemy(enemy.word(8),enemy,state->projectiles,player,parameters,
                    state->boss.stage,state->boss.allocation,state->boss.protection,state->bindings[6],
                    midboss_input,random,precision,math);
            else if(full_six_world && enemy.word(8)>=109 && enemy.word(8)<=113)
                kaikeidou::update_stage6_boss_early_enemy(enemy.word(8),enemy,state->projectiles,state->effects,player,
                    parameters,random,precision,math,death_services);
            else if(full_six_world && (enemy.word(8)==114||enemy.word(8)==116||enemy.word(8)==117||enemy.word(8)==138))
                kaikeidou::update_stage6_boss_final_enemy(enemy.word(8),enemy,
                    static_cast<std::uint32_t>(&enemy-state->enemies.enemies.data()),state->projectiles,player,
                    parameters,random,precision,math,enemy_services);
            else if(full_six_world && enemy.word(8)>=118 && enemy.word(8)<=124)
                kaikeidou::update_ex_first_enemy(enemy.word(8),enemy,
                    static_cast<std::uint32_t>(&enemy-state->enemies.enemies.data()),state->projectiles,player,
                    parameters,random,precision,math,services);
            else if(full_six_world && enemy.word(8)>=125 && enemy.word(8)<=135)
                kaikeidou::update_ex_third_enemy(enemy.word(8),enemy,state->projectiles,state->effects,player,
                    parameters,state->boss.stage,random,precision,math,death_services);
            else if(full_six_world && (enemy.word(8)==136||enemy.word(8)==137))
                kaikeidou::update_ex_boss_enemy(enemy.word(8),static_cast<std::uint32_t>(&enemy-state->enemies.enemies.data()),
                    state->enemies,state->projectiles,player,parameters,state->boss.stage,state->boss.protection,
                    random,precision,math,services);
            else if(omniverse_world && enemy.word(8)>=139 && enemy.word(8)<=150)
                kaikeidou::update_omniverse_enemy(state->enemies,static_cast<std::uint32_t>(&enemy-state->enemies.enemies.data()),
                    enemy.word(8),state->projectiles,omni_context,random,precision,math,omni_services);
            else throw std::runtime_error("Enemy behavior " + std::to_string(enemy.word(8)) + " remains to be recovered");
            kaikeidou::update_enemy_animation(enemy, precision);
        }
        kaikeidou::update_projectiles(state->projectiles, [&](std::uint32_t behavior, std::uint32_t index) {
            if (behavior <= 3) kaikeidou::update_basic_projectile(state->projectiles.bullets.at(index), behavior, static_cast<unsigned>(precision));
            else if(behavior<=11) kaikeidou::update_projectile_motion(state->projectiles.bullets.at(index), behavior, static_cast<unsigned>(precision));
            else if(behavior<=14) kaikeidou::update_boss_projectile_motion(state->projectiles.bullets.at(index),behavior,state->enemies,state->boss.phase,parameters,static_cast<unsigned>(precision));
            else if(action_world && behavior<=16) kaikeidou::update_later_projectile_motion(state->projectiles.bullets.at(index),behavior,static_cast<unsigned>(precision),math);
            else if(resource_world && behavior>=22 && behavior<=24)
                kaikeidou::update_stage2_midboss_projectile(state->projectiles,index,behavior,state->enemies,state->boss.phase,
                    state->boss.stage.enemy_prefix,parameters,math,precision);
            else if(resource_world && behavior>=26 && behavior<=28)
                kaikeidou::update_stage2_third_projectile(state->projectiles,index,behavior,math,precision);
            else if(resource_world && behavior>=29 && behavior<=36)
                kaikeidou::update_stage2_boss_projectile(state->projectiles,index,behavior,parameters,state->boss.stage,math,precision);
            else if(resource_world && behavior>=37 && behavior<=41)
                kaikeidou::update_stage3_midboss_projectile(state->projectiles,index,behavior,
                    player.quad(0x90),player.quad(0x98),parameters,random,precision,math);
            else if(resource_world && behavior>=42 && behavior<=44)
                kaikeidou::update_stage3_boss_projectile_early(state->projectiles,index,behavior,
                    player.quad(0x90),player.quad(0x98),parameters,random,precision,math);
            else if(resource_world && behavior>=45 && behavior<=50)
                kaikeidou::update_stage3_boss_projectile_final(state->projectiles,index,behavior,
                    player.quad(0x90),player.quad(0x98),parameters,state->boss.stage,state->action.stage,
                    state->extended_patterns,random,precision,math);
            else if(full_six_world && behavior==51)
                kaikeidou::update_stage4_first_projectile(state->projectiles,index,behavior,
                    player.quad(0x90),player.quad(0x98),parameters,random,precision,math);
            else if(full_six_world && behavior==52)
                kaikeidou::update_stage4_midboss_projectile(state->projectiles,index,parameters,precision);
            else if(full_six_world && behavior>=54 && behavior<=56)
                kaikeidou::update_stage4_boss_projectile_early(state->projectiles,index,behavior,
                    player.quad(0x90),player.quad(0x98),parameters,state->boss.stage,state->action.stage,
                    state->stage4_patterns,state->boss.death.pattern_integer_5,random,precision,math);
            else if(full_six_world && behavior>=57 && behavior<=64)
                kaikeidou::update_stage4_boss_projectile_final(state->projectiles,index,behavior,
                    player.quad(0x90),player.quad(0x98),parameters,state->boss.stage,state->action.stage,
                    state->extended_patterns,state->stage4_patterns,state->boss.death.pattern_integer_5,random,precision,math);
            else if(full_six_world && behavior>=68 && behavior<=70)
                kaikeidou::update_stage5_first_projectile(behavior,state->projectiles,index,
                    state->background.context,timeline.frame,precision,math);
            else if(full_six_world && behavior>=71 && behavior<=72)
                kaikeidou::update_stage5_midboss_projectile(state->projectiles,index,behavior,player,
                    parameters,state->background.context,random,precision,math);
            else if(full_six_world && behavior>=73 && behavior<=78)
                kaikeidou::update_stage5_boss_early_projectile(state->projectiles,index,behavior,state->enemies,
                    state->boss.phase,state->boss.stage,player,parameters,random,precision,math);
            else if(full_six_world && (behavior==79||behavior==81||behavior==86))
                kaikeidou::update_stage5_boss_final_projectile(state->projectiles,index,behavior,math,precision);
            else if(full_six_world && (behavior==80||behavior==82||behavior==83||behavior==84))
                kaikeidou::update_stage5_boss_final_secondary(state->projectiles,index,behavior,state->enemies,
                    state->boss.phase,state->boss.stage,player,parameters,random,precision,math,true);
            else if(full_six_world && behavior==85)
                kaikeidou::update_stage6_first_projectile(state->projectiles,index,behavior,math,precision);
            else if(full_six_world && (behavior==87||behavior==88||(behavior>=90&&behavior<=92)))
                kaikeidou::update_stage6_midboss_projectile(state->projectiles,index,behavior,player,
                    parameters,random,precision,math);
            else if(full_six_world && behavior>=93 && behavior<=99)
                kaikeidou::update_stage6_boss_early_projectile(state->projectiles,index,behavior,player,
                    parameters,state->boss.death.pattern_integer_5,random,precision,math);
            else if(full_six_world && (behavior==100||(behavior>=102&&behavior<=106)))
                kaikeidou::update_stage6_boss_final_projectile(state->projectiles,index,behavior,player,
                    parameters,random,precision,math);
            else if(full_six_world && behavior==108)
                kaikeidou::update_ex_first_projectile(state->projectiles,index,behavior,math,precision);
            else if(full_six_world && behavior>=109 && behavior<=112)
                kaikeidou::update_ex_midboss_projectile(state->projectiles,index,behavior,player,
                    parameters,state->shake,random,precision,math);
            else if(full_six_world && behavior>=113 && behavior<=116)
                kaikeidou::update_ex_third_projectile(state->projectiles,index,behavior,state->enemies,
                    state->boss.stage,player,parameters,state->action.stage,random,precision,math);
            else if(full_six_world && behavior>=117 && behavior<=120)
                kaikeidou::update_ex_boss_opening_projectile(state->projectiles,index,behavior,parameters,precision,math);
            else if(full_six_world && behavior>=128 && behavior<=139)
                kaikeidou::update_ex_boss_middle_projectile(state->projectiles,index,behavior,state->effects,player,
                    parameters,state->boss.stage,state->boss.death,state->action.stage,random,precision,math,death_services);
            else if(full_six_world && ((behavior>=140 && behavior<=146)||behavior==149||behavior==151))
                kaikeidou::update_ex_boss_early_final_projectile(state->projectiles,index,behavior,state->enemies,
                    state->effects,state->shake,player,parameters,state->boss.phase,state->boss.stage,
                    state->action.stage,random,precision,math,death_services);
            else if(full_six_world && (behavior==121||behavior==122||behavior==147||behavior==148||behavior==150||
                    (behavior>=152&&behavior<=159)||(behavior>=163&&behavior<=166)))
                kaikeidou::update_ex_boss_middle_final_projectile(state->projectiles,index,behavior,state->enemies,player,
                    parameters,state->boss.phase,state->boss.stage,random,precision,math);
            else if(full_six_world && behavior==190)
                kaikeidou::update_ex_boss_enemy_projectile(state->projectiles,index,precision,math);
            else if(full_six_world && behavior>=170 && behavior<=176)
                kaikeidou::update_ex_boss_heavy_projectile(state->projectiles,index,behavior,player,parameters,
                    state->effects,random,precision,math,ex_heavy_services);
            else if(full_six_world && (behavior==189||behavior==191))
                kaikeidou::update_ex_boss_control_projectile(state->projectiles,index,behavior,player,
                    random,precision,math,ex_control_services);
            else if(full_six_world && ((behavior>=123&&behavior<=127)||(behavior>=167&&behavior<=169)||
                    (behavior>=177&&behavior<=188)||behavior==192||behavior==193))
                kaikeidou::update_ex_boss_late_other_projectile(state->projectiles,index,behavior,player,
                    parameters,state->enemies,state->boss.phase,state->boss.stage,state->action.stage,
                    state->extended_patterns,random,precision,math);
            else if(omniverse_world && behavior>=194 && behavior<=226)
                kaikeidou::update_omniverse_projectile(state->projectiles,index,behavior,state->enemies,omni_context,random,precision,math,omni_services);
            else if(dialogue_world)kaikeidou::update_final_boss_projectile(state->projectiles,index,behavior,player,state->enemies,
                state->boss.phase,state->boss.stage.enemy_prefix,parameters,random,math,precision);
            else throw std::runtime_error("Projectile behavior " + std::to_string(behavior) + " remains to be recovered");
        });
        death_services.synchronize();
        enemy_services.synchronize(lifecycle, timeline);
        if(action_world) kaikeidou::hit_player_bombs(state->action.bombs,player,firing,state->enemies,state->projectiles,enemy_services,math,static_cast<unsigned>(precision));
        hit.fire = firing; hit.mode_a7ea98 = lifecycle.boss_state;
        kaikeidou::PlayerShotHitLiveModeServices live_hit_services(enemy_services,hit,state->boss.death.boss_state);
        kaikeidou::hit_player_shots(state->shots, state->options, state->enemies, player, hit, fire_input, live_hit_services, precision,
            boss_world ? &state->boss.stage.enemy_prefix : nullptr);
        firing.replay_buttons = hit.fire.replay_buttons;
        state->replay_buttons = firing.replay_buttons;
        collision.stage_b3e2ec = lifecycle.stage; collision.part_a77390 = timeline.part; collision.phase_a733c0 = timeline.subphase;
        collision.option_a7ebf8 = state->boss.savedata.options[0];
        collision.mode_a7e618 = lifecycle.practice; collision.character_a74c0c = lifecycle.character; collision.mode_a7ea98 = lifecycle.boss_state;
        collision.scene_a78e2c = lifecycle.scene; collision.scene_step_a87f48 = lifecycle.scene_phase; collision.scene_choice_a87f50 = lifecycle.scene_selection;
        kaikeidou::update_player_collision(player, state->enemies, state->projectiles, collision, collision_services, static_cast<unsigned>(precision));
        lifecycle.scene = collision.scene_a78e2c; lifecycle.scene_phase = collision.scene_step_a87f48; lifecycle.scene_selection = collision.scene_choice_a87f50;
        kaikeidou::ItemPickupContext pickup{lifecycle.character, state->options_context.weapon, parameters.integers[1], parameters.integers[3],
            state->projectiles.secondary[0].quad(0x28)};
        kaikeidou::update_item_pickup(state->items, player, state->options, pickup, state->projectiles.sound_triggers, precision, pickup_services);
        parameters.integers[1] = pickup.pattern_integer_1; parameters.integers[3] = pickup.pattern_integer_3;
        state->projectiles.secondary[0].quad(0x28, pickup.secondary_first_field_28);
        kaikeidou::update_effect_frame(state->effects, state->shake, random, precision);
        if(boss_world) {
            death_services.synchronize(); auto& b=state->boss;
            kaikeidou::update_boss_timer(b.phase,b.death,b.protection,b.presentation,death_services,static_cast<unsigned>(precision));
            death_services.publish();
        }
        kaikeidou::update_item_motion(state->items, player, {state->bindings[6]}, item_input, math, precision);
        if(presentation_world)kaikeidou::update_stage_title(state->title,lifecycle.stage,title_resources);
        if(resource_world) {
            death_services.synchronize();death_services.sync_background();
            kaikeidou::update_stage_background(state->background.context,state->boss.death,timeline.frame,player,random,precision,math,death_services);
        }
        begin_pixel_frame(frame_context,0);
        if(boss_world) {
            death_services.synchronize(); auto& b=state->boss;
            if(resource_world && kaikeidou::background_mesh_presentation_enabled(b.savedata.options[3],
                    lifecycle.stage,timeline.part,timeline.subphase,lifecycle.boss_state))
                kaikeidou::present_background_meshes(state->background.mesh,lifecycle.scene,static_cast<unsigned>(precision),mesh_services);
            if(lifecycle.scene==0) kaikeidou::update_boss_visuals(state->enemies,b.phase,b.presentation,static_cast<unsigned>(precision));
            if(lifecycle.scene!=1||lifecycle.practice!=0)
                kaikeidou::draw_boss_sprites(state->enemies,b.allocation,b.death,parameters,random,sprite_services,static_cast<unsigned>(precision));
            if(lifecycle.scene!=1||lifecycle.practice!=0)
                kaikeidou::update_projectile_presentation(state->projectiles,random,precision,true);
        }
        if(dialogue_world) {
            auto& context=frame_context.dialogue_context;
            context.character=lifecycle.character;context.stage=lifecycle.stage;context.scene=lifecycle.scene;
            context.subphase=timeline.subphase;context.game_frame=timeline.frame;
            context.confirm_binding=state->bindings[4];context.skip_binding=state->bindings[8];
            context.replay_variant=state->boss.phase.replay_variant;context.replay_mode=state->boss.phase.replay_mode;
            context.replay_buttons=state->replay_buttons;
            kaikeidou::update_ex_dialogue(state->dialogue,context,state->enemies,state->boss.stage.enemy_prefix,
                state->boss.allocation,state->projectiles,player,death_services,static_cast<unsigned>(precision),parameters,state->items,random,math);
            lifecycle.subphase=timeline.subphase=context.subphase;timeline.frame=context.game_frame;
            state->fire_lock=state->dialogue.fields.word(8);state->replay_buttons=context.replay_buttons;
        }
        if(presentation_world)kaikeidou::present_stage_title(state->title,title_graphics,static_cast<unsigned>(precision));
        if(dialogue_world)kaikeidou::update_stage_clear_presentation(player);
        if(presentation_world)kaikeidou::update_music_presentation(state->music,static_cast<unsigned>(precision));
        frame_context.present_fps();
        const auto fade_before_presentation=state->stage6.ending.word(0x20);
        if(full_six_world)kaikeidou::present_ending_fade(state->stage6.ending,ending_fade_services);
    end_pixel_frame(frame_context,0);
    return fade_before_presentation;
}
}
