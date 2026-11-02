#include "GameBattle.hpp"
#include "AnmVisualState.hpp"
#include "StageAssets.hpp"
#include <algorithm>
namespace th17 {
struct GameBattle::Services {
    struct PlayerServices final:PlayerHost {
        GameBattle& g;explicit PlayerServices(GameBattle& value):g(value){}
        bool allow_bomb()override{return g.bomb&&g.bomb->allowed();}
        bool begin_bomb()override{return (g.bomb&&g.bomb->begin())||g.fail(g.bomb?g.bomb->error:"Bomb owner unavailable");}
        bool end_roaring(bool death)override{return g.items->end_roaring(death)||g.fail(g.items->error);}
        bool cancel_lasers_near(const Vec3& p,float r,i32 kind,bool honor)override{return g.laser_scene->cancel_circle(p,r,kind,honor)>=0||g.fail(g.laser_scene->error);}
        // PlayerLife's flag selects the unprotected native circle routine.
        bool cancel_bullets_near(const Vec3& p,float r,i32 kind,bool all)override{return g.bullet_scene->cancel_circle(p,r,kind,g.historical_web_replay?all:!all)||g.fail(g.bullet_scene->error);}
        bool cancel_lasers(i32 kind,bool honor)override{return g.laser_scene->cancel_all(kind,honor)||g.fail(g.laser_scene->error);}
        bool item(i32 kind,const Vec3& p,float angle,float speed)override{return g.spawn_item(kind,p,angle,speed);}
        bool options_changed()override{return g.player->configure_options();}
        bool game_over()override{return g.check(g.presentation.game_over(),"Game-over scene unavailable");}
        bool respawn_damage(const Vec3& p,float radius,float growth,i32 life,i32 amount)override{return g.player->damage.circle(p,radius,growth,life,amount)>0;}
        bool shot_audio(i32 id,ShotSoundAction action,float pan)override{return g.sound(id,pan,static_cast<PlayerShots::SoundAction>(action));}
        bool refresh(PlayerFrameContext& frame)override{return g.refresh_player(frame);}
        bool sound(i32 id,float pan,PlayerShots::SoundAction action)override{return g.sound(id,pan,action);}
        bool hit_sound(i32 id)override{return g.sound(id);}
        bool life_hud(i32 lives,i32 pieces)override{return g.life_hud(lives,pieces);}
        bool bomb_hud(i32 stock,i32 pieces)override{return g.bomb_hud(stock,pieces);}
    } player;
    struct Projectiles final:LaserSceneHost {
        GameBattle& g;explicit Projectiles(GameBattle& value):g(value){}
        const PlayerCollision& player_collision()const noexcept override{return g.player_collision();}
        const Vec3* stationary_laser_anchor()const noexcept override{if(g.historical_web_replay)return nullptr;const auto* boss=g.enemy_world.boss_at(0);return boss?&boss->motion.position:nullptr;}
        void hit()override{g.player->hit();if(!g.player->error.empty())g.fail(g.player->error);}
        void sound(i32 id)override{g.sound(id);}
        void laser_sound(i32 id,bool queued)override{g.sound(id,0,PlayerShots::SoundAction::play,queued);}
        void cancellation_effect(i32 script,const Vec3& p,const Vec3& velocity)override{
            const auto handle=g.animations.create(g.resources.bullet,script,-1,0,p);auto* vm=g.animations.registry.find(handle);if(!vm){g.fail(g.animations.error);return;}vm->interpolators.invalidate();vm->interpolators.position.begin(30,6,{0,0,0},{velocity.x,velocity.y,velocity.z});
        }
        void graze_spark(const Vec3& p)override{g.record_graze(p);}
        void graze()override{}
        void graze_resonance(float value)override{g.items->items.motion_scale=value;}
        void item(i32 kind,const Vec3& p,float angle,float speed)override{g.spawn_item(kind,p,angle,speed);}
        bool spawn_enemy(const EnemySpawnRequest& request)override{return g.spawn(request);}
        bool create_laser(const MovingLaserRequest& r)override{return g.laser_scene->create_moving(r)!=0||g.laser_scene->error.empty();}
        bool create_laser(const StationaryLaserRequest& r)override{return g.laser_scene->create_stationary(r)!=0||g.laser_scene->error.empty();}
        bool emit_laser_bullets(const BulletShooter& r)override{return g.bullet_scene->emit(r,g.bullet_scene->manager.minimum_distance_squared);}
        bool erase_emitting_laser()override{return g.fail("Laser erasure must be handled by its concrete owner");}
    } projectiles;
    struct Items final:ItemSpiritHost {
        GameBattle& g;explicit Items(GameBattle& value):g(value){}
        bool options_changed()override{return g.player->configure_options()||g.fail(g.player->options.error);}
        bool notice(i32 kind)override{return g.hud?(g.hud->result(0,kind)||g.fail(g.hud->error)):g.check(g.presentation.notice(kind),"Item notice unavailable");}
        bool sound(i32 id,bool queued)override{return g.sound(id,0,PlayerShots::SoundAction::play,queued);}
        bool popup(const Vec3& p,i32 amount,u32 color)override{return g.check(g.presentation.popup(p,amount,color),"Item popup unavailable");}
        bool life_hud(i32 lives,i32 pieces)override{return g.life_hud(lives,pieces);}
        bool bomb_hud(i32 stock,i32 pieces)override{return g.bomb_hud(stock,pieces);}
        bool trophy(i32 id)override{return g.check(g.presentation.trophy(id),"Trophy recording unavailable");}
        bool cancel_bullets(const Vec3& p,float radius,i32 reward)override{return g.bullet_scene->cancel_circle(p,radius,reward,!g.historical_web_replay)||g.fail(g.bullet_scene->error);}
        bool cancel_lasers(const Vec3& p,float radius,i32 reward,bool honor)override{return g.laser_scene->cancel_circle(p,radius,reward,honor)>=0||g.fail(g.laser_scene->error);}
        bool active_player_shot()const override{for(const auto& shot:g.player->shots.shots)if(shot.state!=0)return true;return false;}
    } items;
    struct Enemies final:EnemyManagerHost,EnemyCombatServices,EnemyDeathHost,BulletEmissionHost,LaserEmissionHost {
        GameBattle& g;explicit Enemies(GameBattle& value):g(value){}
        EnemyVisualHost* animations()override{return &g.enemy_visuals;}
        bool configure(EnemyRuntime& e)override{e.commands.bullets.host=this;e.commands.lasers.host=this;e.commands.lasers.scene=g.laser_scene.get();return true;}
        int after_script(EnemyRuntime& e,float)override{return enemy_update_rule(e.state,g.bullet_scene->manager,g.game_random)?0:-2;}
        int collide_and_damage(EnemyRuntime& e,float rate)override{g.player_collision();g.enemy_world.damage_disabled=g.dialogue_present()?1:0;return g.enemy_damage->update(e,rate);}
        bool update_distortion(EnemyState& e,float rate)override{
            if(!e.distortion.active)return true;if(!e.distortion_mesh.grid.radial(e.distortion,e.motion.position,rate,g.grid_viewport))return g.fail("Enemy distortion grid update failed");
            for(u32 i=0;i<e.distortion_mesh.strip_handles.size();i++){auto* vm=g.animations.registry.find(e.distortion_mesh.strip_handles[i]);if(!vm||!e.distortion_mesh.grid.strip(i,vm->geometry.screen_vertices))return g.fail("Enemy distortion strip unavailable");}return true;
        }
        bool destroy(EnemyState& e)override{return g.cleanup_enemy(e);}
        bool scene_death(EnemyRuntime& e,float rate)override{return g.enemy_death->execute(e,rate)!=-2;}
        bool pause_animation(u32 h,bool paused)override{return g.animations.pause(h,paused)||g.fail(g.animations.error);}
        bool sound(i32 id,const Vec3& p)override{return g.sound(id,p.x);}
        bool emit(const BulletShooter& r,float minimum)override{return g.bullet_scene->emit(r,minimum)||g.fail(g.bullet_scene->error);}
        bool emit(const MovingLaserRequest& r)override{return g.laser_scene->create_moving(r)!=0||g.laser_scene->error.empty();}
        bool emit(const StationaryLaserRequest& r)override{return g.laser_scene->create_stationary(r)!=0||g.laser_scene->error.empty();}
        bool emit(const CurveLaserRequest& r)override{return g.laser_scene->create_curved(r)!=0||g.laser_scene->error.empty();}
        bool emit(const SegmentedLaserRequest& r)override{return g.laser_scene->create_segmented(r)!=0||g.laser_scene->error.empty();}
        bool bomb_damage(const DamageQuery&,i32& amount)override{amount=0;return true;}
        bool additional_damage(EnemyState&,i32,i32& amount)override{amount=0;return true;}
        int enemy_death(EnemyRuntime& e,float rate)override{return g.enemy_death->execute(e,rate);}
        bool contact_override(EnemyState&,AnmVm*,i32&,bool& handled)override{handled=false;return true;}
        bool graze(const Vec3& p)override{return g.record_graze(p);}
        bool spawn_item(const ItemSpawnRequest& r)override{return g.spawn_item(r.type,r.position,r.angle,r.speed);}
        bool effect(const EnemyDeathEffect& r)override{return g.death_effect(r);}
        bool callback(EnemyRuntime&)override{return true;}
        bool nudge(i32 duration,i32 a,i32 b)override{return g.screen_effects.nudge({duration,a,b})||g.fail(g.screen_effects.error);}
        bool scene_message(i32 id)override{return g.scene_message(id);}
        bool message_complete(bool& done)override{return g.message_complete(done);}
        bool cancel_all_bullets(i32 kind)override{return g.bullet_scene->cancel_all(kind)||g.fail(g.bullet_scene->error);}
        bool cancel_all_lasers(i32 kind,bool honor)override{return g.laser_scene->cancel_all(kind,honor)||g.fail(g.laser_scene->error);}
        bool cancel_bullets_circle(const Vec3& p,float r,i32 kind,bool honor)override{return g.bullet_scene->cancel_circle(p,r,kind,honor||g.historical_web_replay)||g.fail(g.bullet_scene->error);}
        bool cancel_lasers_circle(const Vec3& p,float r,i32 kind,bool honor)override{return g.laser_scene->cancel_circle(p,r,kind,honor)>=0||g.fail(g.laser_scene->error);}
        bool clear_lasers_with_rewards()override{return (g.historical_web_replay?g.laser_scene->manager.cancel_all(1,false):g.laser_scene->manager.clear_with_rewards())||g.fail(g.laser_scene->manager.error);}
        bool begin_spell(const SpellStartRequest& r)override{return g.begin_spell(r);}
        bool finish_spell()override{return g.finish_spell();}
        bool chapter(i32 id)override{return g.check(g.presentation.chapter(id),"Chapter update unavailable");}
        bool boss_segment(i32 boss,i32 index,float value,u32 color)override{return g.hud?(g.hud->segment(boss,index,value,color)||g.fail(g.hud->error)):g.check(g.presentation.boss_segment(boss,index,value,color),"Boss life segment unavailable");}
        bool boss_segments(i32 count)override{return g.hud?(g.hud->segments(count)||g.fail(g.hud->error)):g.check(g.presentation.boss_segments(count),"Boss life display unavailable");}
        bool stage_logo()override{return g.hud?(g.hud->stage_logo(g.hud_context)||g.fail(g.hud->error)):g.check(g.presentation.stage_logo(),"Stage logo unavailable");}
        bool tracked_effect(i32 script,const Vec3& p)override{return g.effects.tracked(script,p)!=0||g.fail(g.effects.error);}
        bool effect(const EnemyEffectRequest& r)override{return g.scene_effect(r);}
        bool immediate_update(EnemyState& e,EnemyUpdateRule rule)override{const auto prior=e.update_rule;e.update_rule=rule;const bool ok=enemy_update_rule(e,g.bullet_scene->manager,g.game_random);e.update_rule=prior;return ok;}
        bool retire_distortion(EnemyState& e)override{return e.distortion_mesh.retire(g.animations)||g.fail(g.animations.error);}
        bool begin_distortion(EnemyState& e)override{return e.distortion_mesh.initialize(g.animations,g.resources.text,17)||g.fail(g.animations.error);}
        bool drop_items(EnemyState& e)override{return drop_enemy_items_now(e,g.game_random,*this);}
        bool background_interrupt(i32 label)override{return g.check(g.presentation.background_interrupt(label),"Background interrupt unavailable");}
        bool bullet_bounds(const Vec2& size)override{g.bullet_scene->manager.context.reflection_bounds=size;return true;}
    } enemies;
    struct Bombs final:BombHost {
        GameBattle& g;explicit Bombs(GameBattle& value):g(value){}
        bool sound(i32 id,bool queued)override{return g.sound(id,0,PlayerShots::SoundAction::play,queued);}
        bool bomb_hud(i32 stock,i32 pieces)override{return g.bomb_hud(stock,pieces);}
        bool screen_effect(const BombScreenEffect& r)override{if(r.kind==1)return g.screen_effects.nudge({r.strength,r.rise,r.hold})||g.fail(g.screen_effects.error);if(r.kind==8)return g.screen_effects.shake({r.strength,r.rise,r.hold,r.fall})||g.fail(g.screen_effects.error);return g.fail("Unknown bomb screen effect");}
        bool cancel_bullets(const Vec3& p,float radius,i32 reward,bool honor)override{return g.bullet_scene->cancel_circle(p,radius,reward,honor)||g.fail(g.bullet_scene->error);}
        bool cancel_lasers(const Vec3& p,float radius,i32 reward,bool honor)override{return g.laser_scene->cancel_circle(p,radius,reward,honor)>=0||g.fail(g.laser_scene->error);}
        bool cancel_bullets_rectangle(const Vec3& p,const Vec2& size,float angle,i32 reward)override{return g.bullet_scene->cancel_rectangle(p,size,angle,reward)||g.fail(g.bullet_scene->error);}
        bool cancel_lasers_rectangle(const Vec3& p,const Vec2& size,float angle,i32 reward,bool honor)override{return g.laser_scene->cancel_rectangle(p,size,angle,reward)>=0||g.fail(g.laser_scene->error);}
        bool nearest_target(const Vec3& p,float range,BombTarget& target)override{target={};float nearest=float(range*range);for(const auto* enemy:g.enemy_world.enemies)if(enemy&&!(enemy->flags&0xc000021)){const float x=float(p.x-enemy->motion.position.x),y=float(p.y-enemy->motion.position.y),distance=float(float(y*y)+float(x*x));if(distance<nearest){nearest=distance;target={i32(enemy->id),enemy->motion.position,enemy->flags};}}return true;}
    } bombs;
    struct Spells final:SpellCardServices {
        GameBattle& g;explicit Spells(GameBattle& value):g(value){}
        bool spell_background_visible(bool value)override{return g.check(g.presentation.spell_background_visible(value),"Spell background unavailable");}
        bool spell_prepare_hud(bool value)override{return g.hud?(g.hud->prepare_spell(value)||g.fail(g.hud->error)):g.check(g.presentation.spell_prepare_hud(value),"Spell HUD unavailable");}
        bool spell_title(AnmVm& vm,const std::string& name)override{return g.check(g.presentation.spell_title(vm,name),"Spell text unavailable");}
        bool spell_result(i32 bonus,bool failed)override{return g.hud?(g.hud->result(bonus,failed?1:0)||g.fail(g.hud->error)):g.check(g.presentation.spell_result(bonus,failed),"Spell result unavailable");}
        bool spell_sound(i32 id,bool queued)override{return g.sound(id,0,PlayerShots::SoundAction::play,queued);}
        bool spell_trophy(i32 id)override{return g.check(g.presentation.trophy(id),"Spell trophy unavailable");}
    } spells;
    struct Dialogues final:DialogueServices {
        GameBattle& g;explicit Dialogues(GameBattle& value):g(value){}
        bool clear_dialogue_field()override{return g.bullet_scene->cancel_all(0)&&g.laser_scene->cancel_all(0,false)&&g.enemies->clear_field(false);}
        bool dialogue_text(AnmVm& vm,u32 color,u32 shadow,i32 style,i32 x,i32 y,const std::string& value)override{return g.dialogue_host&&g.dialogue_host->dialogue_text(vm,color,shadow,style,x,y,value);}
        bool dialogue_sound(i32 id)override{return g.sound(id,0,PlayerShots::SoundAction::play,true);}
        bool dialogue_boss_music()override{return g.dialogue_host&&g.dialogue_host->dialogue_boss_music();}
        bool dialogue_music_fade(i32 seconds)override{return g.dialogue_host&&g.dialogue_host->dialogue_music_fade(seconds);}
        bool dialogue_next_stage()override{return g.dialogue_host&&g.dialogue_host->dialogue_next_stage();}
        bool dialogue_boss_name()override{return g.hud&&(g.hud->ensure_boss_name()||g.fail(g.hud->error));}
        bool dialogue_notice(u32 handle)override{if(!g.hud)return false;g.hud->notice_handle=handle;return true;}
    } dialogues;
    explicit Services(GameBattle& g):player(g),projectiles(g),items(g),enemies(g),bombs(g),spells(g),dialogues(g){}
};
GameBattle::GameBattle(AnmManager& a,AnmEnvironment& env,Rng& game,Rng& visual,BattleWorldServices& host,EclProgram& program,const ShtResource& sht,i32 ch,BattleResources ids):services(std::make_unique<Services>(*this)),animations(a),environment(env),game_random(game),visual_random(visual),presentation(host),resources(ids),enemy_visuals(a),effects(a,ids.effect),screen_effects(schedule,game,visual),character(ch){
    enemy_world.frame_rate=&input.rate;enemy_world.spell_status=&spell;enemy_world.random=&game;
    enemy_world.shared_counters={{&session.enemy_deaths,&session.roaring_breaks,&session.enemy_chain}};
    for(u32 i=0;i<ids.enemy.size();i++)enemy_visuals.map_resource(i,ids.enemy[i]);
    player=std::make_unique<Player>(sht,a,effects,game,visual,session,spell,services->player,ch,ids.player,ids.effect);
    player->damage.shared_score=&score.score;
    items=std::make_unique<ItemSpirits>(a,effects,game,visual,player->motion,player->frame.bounds,session,player->life.invulnerability,spell,player->damage,score,services->items,ch,ids.bullet,ids.animal);
    enemy_world.shared_roaring_flags=&items->roaring.state.flags;
    bomb_context=std::make_unique<BombContext>(BombContext{a,player->motion,player->life.invulnerability,session,spell,player->damage,game,services->bombs,items->roaring.state.flags,enemy_world.enemy_count,ids.player});
    if(ch==0)bomb=std::make_unique<BombReimu>(*bomb_context);else if(ch==1)bomb=std::make_unique<BombMarisa>(*bomb_context);else if(ch==2)bomb=std::make_unique<BombYoumu>(*bomb_context);
    player->damage.hit_callback=[this](DamageSource& source,const DamageQuery& query)->i32{if(source.callback_kind==4)return items->roaring.otter_hit();if(source.callback_kind==3&&bomb)return bomb->hit(source,query);return player->shots.hit(source,query);};
    bullet_scene=std::make_unique<BulletScene>(services->projectiles,game,visual);bullet_scene->resource=a.resource(ids.bullet);bullet_scene->environment=&env;bullet_scene->animation_objects=&a;bullet_scene->own_cancellation_animations(a,ids.bullet);
    laser_scene=std::make_unique<LaserScene>(services->projectiles,a,effects,game,visual,bullet_scene->manager.cancellation_rewards,ids.bullet);laser_scene->attach_bullets(*bullet_scene);
    combat=std::make_unique<EnemyCombat>(*player,services->enemies);enemy_damage=std::make_unique<EnemyDamage>(enemy_world,*combat,enemy_visuals);enemy_death=std::make_unique<EnemyDeath>(enemy_world,game,services->enemies);
    enemies=std::make_unique<EnemyManager>(program,enemy_world,game,visual,services->enemies);
    const FrameCallback::Function actions[]={[](void* p)->i32{return static_cast<GameBattle*>(p)->update_animations(true);},[](void* p)->i32{return static_cast<GameBattle*>(p)->update_player();},[](void* p)->i32{return static_cast<GameBattle*>(p)->update_bomb();},[](void* p)->i32{return static_cast<GameBattle*>(p)->update_enemies();},[](void* p)->i32{return static_cast<GameBattle*>(p)->update_lasers();},[](void* p)->i32{return static_cast<GameBattle*>(p)->update_bullets();},[](void* p)->i32{return static_cast<GameBattle*>(p)->update_items();},[](void* p)->i32{return static_cast<GameBattle*>(p)->update_spirits();},[](void* p)->i32{return static_cast<GameBattle*>(p)->update_animations(false);}};
    for(u32 i=0;i<callbacks.size();i++){auto& c=callbacks[i];c.owner=this;c.run=actions[i];c.enabled=true;schedule.add(c,FramePass::Update,update_priorities[i]);}
    // The original scene registers the spell timer at update priority 32.
    spell_callback.owner=this;spell_callback.run=[](void* p)->i32{return static_cast<GameBattle*>(p)->update_spell();};spell_callback.enabled=true;schedule.add(spell_callback,FramePass::Update,32);
    hud_callback.owner=this;hud_callback.run=[](void* p)->i32{return static_cast<GameBattle*>(p)->update_hud();};hud_callback.enabled=true;schedule.add(hud_callback,FramePass::Update,34);
}
GameBattle::~GameBattle(){active_schedule->remove(hud_callback);active_schedule->remove(spell_callback);for(auto& callback:callbacks)active_schedule->remove(callback);dialogue.reset();if(enemies)enemies->clear();}
bool GameBattle::replace_stage(StageAssets& next){
    if(!initialized||!error.empty()||next.character!=character||next.animal!=session.animal)return fail("Stage replacement differs from active run");
    if(spell.flags&1)return fail("Active spell must finish before normal stage replacement");
    dialogue.reset();if(enemies&&!enemies->clear())return fail(enemies->error);
    if(spell_card)for(auto& handle:spell_card->handles)if(!animations.retire(handle))return fail(animations.error);
    spell_card.reset();spell_visuals.reset();spell={};resources=next.battle_resources();stage=next.definition->stage;
    enemy_world.enemies.clear();enemy_world.enemy_count=0;enemy_world.boss=nullptr;enemy_world.boss_ids.fill(0);enemy_world.manager_flags=0;
    // Native 0x41e550 creates a zeroed enemy manager for each stage. These
    // three ECL counters belong to that manager, despite sharing the player
    // services that update them when a miss or Roaring break occurs.
    if(!historical_web_replay)session.enemy_deaths=session.roaring_breaks=session.enemy_chain=0;
    enemy_world.integer_registers.fill(0);enemy_world.float_registers.fill(0);enemy_world.current_chapter=enemy_world.requested_chapter=enemy_world.chapter_total=enemy_world.chapter_defeated=0;
    enemy_world.boss_seconds=-1;enemy_world.boss_hundredths=0;enemy_world.frame_rate=&input.rate;
    enemies=std::make_unique<EnemyManager>(next.scripts,enemy_world,game_random,visual_random,services->enemies);
    for(u32 i=0;i<resources.enemy.size();i++)enemy_visuals.map_resource(i,resources.enemy[i]);
    if(hud)hud->stage_resources(next.hud_resources(),score.score);
    dialogue_stage=nullptr;message_resource=nullptr;dialogue_host=nullptr;
    return refresh_world();
}
bool GameBattle::fail(const std::string& message){if(error.empty())error=message.empty()?"Battle integration failed":message;return false;}
bool GameBattle::check(bool ok,const char* message){return ok||fail(message);}
bool GameBattle::sound(i32 id,float pan,PlayerShots::SoundAction action,bool queued){return check(presentation.audio(id,pan,action,queued),"Battle audio unavailable");}
bool GameBattle::spawn_item(i32 kind,const Vec3& p,float angle,float speed){return items->spawn(kind,p,angle,speed)||fail(items->error);}
bool GameBattle::scene_effect(const EnemyEffectRequest& r){const auto h=animations.create(enemy_visuals.resource_id(r.resource),r.script,-1,r.ordering,r.position);if(!h)return fail(animations.error);effects.track(h);return true;}
bool GameBattle::death_effect(const EnemyDeathEffect& r){const auto h=animations.create(enemy_visuals.resource_id(r.resource),r.script,r.layer,0,r.position,r.rotation);if(!h)return fail(animations.error);effects.track(h);return true;}
bool GameBattle::cleanup_enemy(EnemyState& e){if(e.lifecycle_flags&4)return true;if((e.flags&0x800000)&&e.boss_slot>=0&&e.boss_slot<3)enemy_world.boss_ids[e.boss_slot]=0;if(!e.distortion_mesh.retire(animations))return fail(animations.error);for(auto& h:e.animation_handles)if(!animations.retire(h))return fail(animations.error);return true;}
const PlayerCollision& GameBattle::player_collision()noexcept{
    auto& c=player->frame.collision;c.position={player->motion.position.x,player->motion.position.y};c.state=player->life.state;c.invulnerability=player->life.invulnerability.current;c.radius=player->resource.header.hitbox;c.size_multiplier=player->motion.enlargement;c.enlarged=player->motion.behavior_flags&16;c.laser_half_size={player->frame.bounds.hit_half_size.x,player->frame.bounds.hit_half_size.y};c.bomb_active=input.hud_collect||dialogue_present();return c;
}
bool GameBattle::refresh_world(){
    enemy_world.mode=character;enemy_world.submode=session.animal;enemy_world.stage_number=stage;enemy_world.difficulty=score.difficulty;enemy_world.state=session.power;enemy_world.counter3=session.deaths;enemy_world.game_frame_flags=input.game_flags;
    enemy_world.player_position=player->motion.position;enemy_world.player_damage_state=player->life.state;enemy_world.player_behavior_flags=player->motion.behavior_flags;enemy_world.player_damage_scale=player->frame.bounds.damage_multiplier;enemy_world.game_state=session.bomb_state;
    enemy_world.roaring_kind=items->roaring.state.kind;enemy_world.spirit_count=items->spirits.count();enemy_world.graze_history=bullet_scene->manager.graze_history;enemy_world.damage_disabled=dialogue_present()?1:0;
    bullet_scene->manager.context.player=player->motion.position;bullet_scene->manager.context.rate=input.rate;bullet_scene->manager.cancellation_rewards.spell_active=spell.flags&1;
    return error.empty();
}
bool GameBattle::refresh_player(PlayerFrameContext& frame){
    // ANM vertices include the compositor's screen offset. Cull shots in the
    // matching content rectangle, rather than the fixture's zero-based space.
    player->shot_context.screen_origin=environment.playfield_origin;
    items->after_player();frame.game_flags=input.game_flags;frame.hud_available=input.hud_available;frame.hud_bomb_active=input.hud_collect||dialogue_present();frame.focus_allowed=input.focus_allowed&&enemy_world.enemy_count!=0;frame.enemy_present=true;frame.background_delta=input.background_delta;frame.roaring_kind=items->roaring.state.kind;
    targets.clear();targets.reserve(enemy_world.enemies.size());for(const auto* e:enemy_world.enemies)targets.push_back({e->motion.position,e->flags,e->id});frame.targets.clear();frame.targets.reserve(targets.size());for(auto& target:targets)frame.targets.push_back(&target);return refresh_world();
}
bool GameBattle::initialize(bool initialize_roaring_hud){if(initialized)return fail("Battle already initialized");if(!bullet_scene->resource||!animations.resource(resources.player)||!animations.resource(resources.effect)||!animations.resource(resources.animal))return fail("Battle animation bank unavailable");if(!player->initialize())return fail(player->error);if(!player->configure_options())return fail(player->options.error);if(initialize_roaring_hud&&!items->initialize())return fail(items->error);initialized=true;return refresh_world();}
bool GameBattle::life_hud(i32 lives,i32 pieces){return hud?(hud->lives(lives,pieces)||fail(hud->error)):check(presentation.life_hud(lives,pieces),"Life HUD unavailable");}
bool GameBattle::bomb_hud(i32 stock,i32 pieces){return hud?(hud->bombs(stock,pieces)||fail(hud->error)):check(presentation.bomb_hud(stock,pieces),"Bomb HUD unavailable");}
bool GameBattle::configure_hud(const HudStartContext& context,HudResources visuals,bool initialize_now){if(hud)return fail("HUD already configured");hud_context=context;hud=std::make_unique<Hud>(animations,enemy_world,session,player->motion,spell,visuals);hud->sound=[this](i32 id){return sound(id,0,PlayerShots::SoundAction::play,true);};enemy_world.boss_seconds=-1;return !initialize_now||hud->initialize(context)||fail(hud->error);}
bool GameBattle::configure_dialogue(const StageConfiguration& stage,const MessageResource& messages,DialogueServices& host,DialogueResources visuals){if(dialogue||!hud)return fail("Dialogue configuration requires an idle HUD");dialogue_stage=&stage;message_resource=&messages;dialogue_host=&host;dialogue_resources=visuals;hud->update_dialogue=[this](){return update_dialogue();};return true;}
bool GameBattle::scene_message(i32 id){if(id<0||!message_resource)return check(presentation.scene_message(id),"Dialogue scene command unavailable");dialogue.reset();dialogue=std::make_unique<Dialogue>(animations,services->dialogues,*dialogue_stage,dialogue_resources);return dialogue->initialize(*message_resource,id)||fail(dialogue->error);}
bool GameBattle::message_complete(bool& done){if(!message_resource)return check(presentation.message_complete(done),"Dialogue state unavailable");done=!dialogue||dialogue->complete_pulse!=0;return true;}
bool GameBattle::update_dialogue(){if(!dialogue)return true;const i32 result=dialogue->update({input.rate,input.held,input.pressed,input.shoot_frames,input.skip_frames});if(result<0)return fail(dialogue->error);if(result>0)dialogue.reset();return true;}
bool GameBattle::configure_spell(SpellRecords& records,SpellVisualResources visuals){if(spell.flags&1)return fail("Cannot reconfigure an active spell");spell_context.visuals=std::move(visuals);spell_visuals=std::make_unique<SpellAnmHost>(animations,enemy_world,services->spells);spell_card=std::make_unique<SpellCard>(spell,score.score,records,items->roaring.state.flags,*spell_visuals);return true;}
bool GameBattle::begin_spell(const SpellStartRequest& request){if(!spell_card)return fail("Spell owner unavailable");spell_context.stage=stage;spell_context.difficulty=score.difficulty;spell_context.character=character;spell_context.animal=session.animal;spell_context.replay=session.replay_state==1;spell_context.practice=(enemy_world.mode_flags>>4&3)==2;return spell_card->begin(request,spell_context)||fail(spell_card->error);}
bool GameBattle::finish_spell(){return spell_card?(spell_card->finish()||fail(spell_card->error)):fail("Spell owner unavailable");}
bool GameBattle::spawn(const EnemySpawnRequest& r){if(!initialized)return fail("Battle not initialized");if(!refresh_world())return false;enemies->rate=input.rate;enemies->background_delta=input.background_delta;const bool ok=enemies->spawn_at_rate(r,input.rate);return ok?error.empty():fail(enemies->error);}
bool GameBattle::attach(FrameScheduler& target){if(active_schedule==&target)return true;for(u32 i=0;i<callbacks.size();i++){auto& c=callbacks[i];const auto run=c.run;active_schedule->remove(c);c.run=run;if(target.add(c,FramePass::Update,update_priorities[i])<0)return fail("Battle scheduling failed");}const auto spell_run=spell_callback.run;active_schedule->remove(spell_callback);spell_callback.run=spell_run;if(target.add(spell_callback,FramePass::Update,32)<0)return fail("Spell scheduling failed");const auto hud_run=hud_callback.run;active_schedule->remove(hud_callback);hud_callback.run=hud_run;if(target.add(hud_callback,FramePass::Update,34)<0)return fail("HUD scheduling failed");if(!screen_effects.attach(target))return fail(screen_effects.error);active_schedule=&target;return true;}
bool GameBattle::detach(FrameScheduler& owner){return active_schedule!=&owner||attach(schedule);}
bool GameBattle::prepare_frame(const BattleFrame& frame){if(!initialized||!error.empty())return fail("Battle unavailable");input=frame;animations.rate=input.rate;environment.paused=input.animation_paused;items->set_practice(input.practice);screen_effects.collect();screen_effects.context={false,true,input.game_flags,input.rate,environment.resolution_scale};bomb_context->hud_available=input.hud_available;bomb_context->hud_collect=input.hud_collect||dialogue_present();const bool enabled[]={true,input.player_enabled,input.bomb_enabled,input.enemies_enabled,input.lasers_enabled,input.bullets_enabled,input.items_enabled,input.spirits_enabled,true};for(u32 i=0;i<callbacks.size();i++)callbacks[i].enabled=enabled[i];world_active(world_running);return refresh_world();}
void GameBattle::world_active(bool active)noexcept{
    world_running=active;const bool enabled[]={true,input.player_enabled,input.bomb_enabled,input.enemies_enabled,input.lasers_enabled,input.bullets_enabled,input.items_enabled,input.spirits_enabled,true};
    // Native retained-stage release disables bullets and scene combat, while
    // the player, bomb and item/animal owners continue through the transition.
    for(u32 i=1;i<8;i++)callbacks[i].enabled=(active||(!historical_web_replay&&(i==1||i==2||i==6||i==7)))&&enabled[i];spell_callback.enabled=active;
    // The retained native HUD callback stays registered while the next stage
    // waits: its stage-clear notice must reach the priority-16 release gate.
    hud_callback.enabled=true;
}
bool GameBattle::step(const BattleFrame& frame){return prepare_frame(frame)&&active_schedule->update()>=0&&error.empty();}
i32 GameBattle::update_animations(bool alternate){animations.rate=input.rate;if(!animations.update(alternate)){fail(animations.error);return 5;}return error.empty()?1:5;}
i32 GameBattle::update_player(){PlayerFrameContext frame;frame.held=input.held;frame.pressed=input.pressed;items->before_player();if(!refresh_player(frame)||!player->update(frame,input.rate)){fail(player->error);return 5;}items->after_player();return refresh_world()?1:5;}
i32 GameBattle::update_bomb(){if(!bomb||!bomb->update(input.rate)){fail(bomb?bomb->error:"Bomb owner unavailable");return 5;}return refresh_world()?1:5;}
i32 GameBattle::update_enemies(){refresh_world();enemies->rate=input.rate;enemies->background_delta=input.background_delta;const bool ok=enemies->update();player->motion.behavior_flags=enemy_world.player_behavior_flags;player->frame.bounds.damage_multiplier=enemy_world.player_damage_scale;if(!ok){fail(enemies->error);return 5;}return error.empty()?1:5;}
i32 GameBattle::update_lasers(){refresh_world();if(!laser_scene->update(input.rate,input.game_flags)){fail(laser_scene->error);return 5;}return error.empty()?1:5;}
i32 GameBattle::update_bullets(){if((input.game_flags>>2|input.game_flags)&1)return 1;refresh_world();bullet_scene->manager.world_paused=input.game_flags&2;if(!bullet_scene->update()){fail(bullet_scene->error);return 5;}return error.empty()?1:5;}
ItemSpiritFrame GameBattle::item_frame()const{return {input.rate,player->resource.header.attraction_speed,player->life.state,session.bomb_state,bomb?bomb->age.current:0,input.held,input.hud_collect||dialogue_present(),input.practice};}
i32 GameBattle::update_items(){if(((input.game_flags>>2|input.game_flags)&1)||(input.game_flags&0x400))return 1;if(!items->update_items(item_frame())){fail(items->error);return 5;}return refresh_world()?1:5;}
i32 GameBattle::update_spirits(){if(!items->update_spirits(item_frame())){fail(items->error);return 5;}return refresh_world()?1:5;}
i32 GameBattle::update_hud(){if(hud){hud->current_seconds=enemy_world.boss_seconds;hud->game_flags=input.game_flags;hud->dialogue=dialogue_present();if(!hud->update(input.rate)){fail(hud->error);return 5;}}return error.empty()?1:5;}
i32 GameBattle::update_spell(){if(spell_card&&!spell_card->update({input.rate,player->motion.position.y,session.bomb_state})){fail(spell_card->error);return 5;}return error.empty()?1:5;}
bool GameBattle::record_graze(const Vec3& p){graze_total=std::min(wrapping_add(graze_total,1),99999999);graze_chapter=std::min(wrapping_add(graze_chapter,1),99999999);const auto& player_position=player->motion.position;const Vec3 middle{float(float(p.x+player_position.x)*.5f),float(float(p.y+player_position.y)*.5f),0};if(!animations.create(resources.effect,24,-1,0,middle))return fail(animations.error);return check(presentation.popup(middle,graze_chapter,0xffc0c0ff),"Graze popup unavailable")&&sound(42);}
}
