#include "GameBattle.hpp"
#include "BattleSceneBindings.hpp"
#include "StageAssets.hpp"
#include "BattleFramePolicy.hpp"
#include "EnemyCallbacks.hpp"
#include "RecordStore.hpp"
#include <algorithm>
namespace th15 {
struct GameBattle::Services {
    struct PlayerServices final:PlayerHost {
        GameBattle& g;explicit PlayerServices(GameBattle& g):g(g){}
        bool allow_bomb()override{return g.bomb&&g.bomb->allowed();}
        bool begin_bomb()override{if(!g.bomb||!g.bomb->begin())return false;g.enemy_world.counters[1]=g.bomb_enemies.uses;g.enemy_world.counters[2]=g.bomb_enemies.chain;g.session.enemy_chain=g.bomb_enemies.chain;return true;}
        bool cancel_lasers_near(const Vec3& p,float r,i32 kind,bool flag)override{return g.laser_scene->cancel_circle(p,r,kind,flag)>=0;}
        bool cancel_bullets_near(const Vec3& p,float r,i32 kind)override{return g.bullet_scene->cancel_circle(p,r,kind,false);}
        bool cancel_lasers(i32 kind,bool flag)override{return g.laser_scene->cancel_all(kind,flag);}
        bool item(i32 kind,const Vec3& p,float a,float s)override{return g.spawn_item(kind,p,a,s);}
        bool options_changed()override{return g.player->configure_options();}
        bool game_over()override{return g.check(g.game_over_begin?g.game_over_begin():g.presentation.game_over(),"Game-over service failed");}
        bool respawn_damage(const Vec3& p,float r,float growth,i32 frames,i32 value)override{return g.player->damage.circle(p,r,growth,frames,value)>0;}
        bool bomb_hud(i32 bombs,i32 pieces)override{return g.bomb_hud(bombs,pieces);}
        bool stop_sound(i32 id)override{return g.sound(id,0,PlayerShots::SoundAction::stop,true);}
        bool refresh(PlayerFrameContext& c)override{return g.refresh_player(c);}
        bool sound(i32 id,float pan,PlayerShots::SoundAction action)override{return g.sound(id,pan,action);}
        bool hit_sound(i32 id)override{return g.sound(id,0,PlayerShots::SoundAction::play,true);}
        bool life_hud(i32 lives,i32 pieces)override{return g.life_hud(lives,pieces);}
    } player;
    struct Projectiles final:LaserSceneHost {
        GameBattle& g;explicit Projectiles(GameBattle& g):g(g){}
        const PlayerCollision& player_collision()const noexcept override{return g.player_collision();}
        void hit()override{g.player->hit();if(!g.player->error.empty())g.fail(g.player->error);}
        void sound(i32 id)override{g.sound(id,0,PlayerShots::SoundAction::play);}
        void laser_sound(i32 id,bool queued)override{g.sound(id,0,PlayerShots::SoundAction::play,queued);}
        void cancellation_effect(i32 script,const Vec3& p,const Vec3& velocity)override{g.check(g.presentation.cancellation_effect(script,p,velocity),"Cancellation effect failed");}
        void graze_spark(const Vec3& p)override{g.record_graze(p);}
        void graze()override{g.graze_flash();}
        void graze_resonance(float value)override{g.graze_resonance(value);}
        void item(i32 kind,const Vec3& p,float a,float s)override{g.spawn_item(kind,p,a,s);}
        bool spawn_enemy(const EnemySpawnRequest& request)override{return g.spawn(request);}
        bool create_laser(const MovingLaserRequest& request)override{const auto id=g.laser_scene->create_moving(request);return id||g.laser_scene->error.empty()||g.fail(g.laser_scene->error);}
        bool create_laser(const StationaryLaserRequest& request)override{const auto id=g.laser_scene->create_stationary(request);return id||g.laser_scene->error.empty()||g.fail(g.laser_scene->error);}
        const Vec3* stationary_laser_anchor()const noexcept override{const auto* boss=g.enemy_world.boss_at(0);return boss?&boss->motion.position:nullptr;}
    } projectiles;
    struct Items final:ItemCollectionHost {
        GameBattle& g;explicit Items(GameBattle& g):g(g){}
        bool options_changed()override{return g.player->configure_options();}
        bool notice(i32 id)override{return g.scene?(g.scene->hud.notification(0,id)||g.fail(g.scene->hud.error)):g.check(g.presentation.notice(id),"Item notice failed");}
        bool sound(i32 id,bool queued)override{return g.sound(id,0,PlayerShots::SoundAction::play,queued);}
        bool popup(const Vec3& p,i32 value,u32 color)override{return g.popup(p,value,color);}
        bool life_hud(i32 lives,i32 pieces)override{return g.life_hud(lives,pieces);}
        bool bomb_hud(i32 bombs,i32 pieces)override{return g.bomb_hud(bombs,pieces);}
    } items;
    struct Enemies final:EnemyManagerHost,EnemyCombatServices,EnemyDeathHost {
        GameBattle& g;explicit Enemies(GameBattle& g):g(g){}
        EnemyVisualHost* animations()override{return &g.enemy_visuals;}
        LaserScene* lasers()override{return g.laser_scene.get();}
        BulletEmissionHost* bullets()override{return &g.bullet_scene->manager;}
        int after_script(EnemyRuntime& e,float rate)override{if(!enemy_update_rule(e.state,*g.bullet_scene,*g.laser_scene,g.player_collision(),g.dialogue_present())){e.error=g.bullet_scene->error.empty()?"Enemy special update rule failed":g.bullet_scene->error;return -2;}return g.presentation.enemy_callback(e,rate);}
        int collide_and_damage(EnemyRuntime& e,float rate)override{return g.enemy_damage->update(e,rate);}
        bool update_distortion(EnemyState& e,float rate)override{return g.scene?(e.distortion_mesh.update(g.animations,e.distortion,e.motion.position,rate,g.scene->screen_view)||g.fail("Enemy distortion grid update failed")):g.check(g.presentation.enemy_distortion(e,rate),"Enemy distortion failed");}
        bool destroy(EnemyState& e)override{return g.cleanup_enemy(e);}
        bool pause_animation(u32 handle,bool paused)override{return g.animations.pause(handle,paused);}
        bool sound(i32 id,const Vec3& p)override{return g.sound(id,p.x,PlayerShots::SoundAction::play);}
        bool cancel_bullets_circle(const Vec3& p,float r,i32 reward,bool honor)override{return g.bullet_scene->cancel_circle(p,r,reward,honor);}
        bool cancel_lasers_circle(const Vec3& p,float r,i32 reward,bool honor)override{return g.laser_scene->cancel_circle(p,r,reward,honor)>=0;}
        bool cancel_bullets_rectangle(const Vec3& p,const Vec2& size,float angle,i32 reward)override{return g.bullet_scene->cancel_rectangle(p,size,angle,reward);}
        bool background_fog(i32 duration,i32 mode,const StageFog& target)override{if(g.scene){g.scene->background.script.interpolate_fog(duration,mode,target);return true;}return g.check(g.presentation.background_fog(duration,mode,target),"Background fog service failed");}
        bool background_interrupt(i32 label)override{return g.scene?(g.scene->background.interrupt(label)||g.fail(g.scene->background.error)):g.check(g.presentation.background_interrupt(label),"Background interrupt service failed");}
        bool cancel_all_bullets(i32 reward)override{return g.bullet_scene->cancel_all(reward);}
        bool cancel_all_lasers(i32 reward,bool honor)override{return g.laser_scene->cancel_all(reward,honor);}
        bool clear_lasers_with_rewards()override{return g.laser_scene->manager.clear_with_rewards();}
        bool scene_message(i32 id)override{if(g.scene){g.scene->message_state(g.session,g.spell);return g.scene->messages.request(id)||g.fail(g.scene->messages.error);}return g.check(g.presentation.scene_message(id),"Scene message service failed");}
        bool message_complete(bool& complete)override{if(g.scene){complete=g.scene->messages.finished();return true;}return g.check(g.presentation.message_complete(complete),"Scene message status failed");}
        bool nudge(const ScreenNudgeSpec& s)override{return g.check(g.presentation.nudge(s),"Enemy screen nudge failed");}
        bool boss_segment(i32 boss,i32 index,float fraction,u32 color)override{return g.check(g.scene?g.scene->hud.boss.segment(boss,index,fraction,color):g.presentation.boss_segment(boss,index,fraction,color),"Boss health segment failed");}
        bool boss_segments(i32 count)override{if(g.scene){g.scene->hud.boss.displayed_segments=count;return true;}return g.check(g.presentation.boss_segments(count),"Boss health display failed");}
        bool stage_logo()override{return g.scene?(g.scene->hud.stage_logo(g.scene->scene_destination)||g.fail(g.scene->hud.error)):g.check(g.presentation.stage_logo(),"Stage logo failed");}
        bool drop_items(EnemyState& enemy)override{return drop_enemy_items_now(enemy,g.game_random,*this);}
        bool effect(const EnemyCommandHost::EffectRequest& e)override{const u32 handle=g.animations.create(g.enemy_visuals.resource_id(e.resource),e.script,-1,e.ordering,e.position,e.rotation);if(!handle)return g.fail(g.animations.error);g.effects.track(handle);return g.effects.error.empty()||g.fail(g.effects.error);}
        bool begin_distortion(EnemyState& e)override{return g.scene?(e.distortion_mesh.initialize(g.animations,g.scene->text_bank)||g.fail("Enemy distortion grid creation failed")):g.check(g.presentation.begin_enemy_distortion(e),"Enemy distortion creation failed");}
        bool retire_distortion(EnemyState& e)override{return g.scene?(e.distortion_mesh.retire(g.animations)||g.fail(g.animations.error)):g.check(g.presentation.retire_enemy_distortion(e),"Enemy distortion retirement failed");}
        bool begin_spell(const SpellStartRequest& request)override{if(g.scene&&g.scene->assets)g.spell_visuals=g.scene->assets->spell_resources(g.scene->progress.chapter);SpellStartContext c;c.stage=g.stage;c.difficulty=g.enemy_world.difficulty;c.character=g.character;c.bomb_state=g.session.bomb_state;c.replay=g.session.replay_state==1;c.visuals=g.spell_visuals;const bool ok=g.spell_card.begin(request,c);return ok||g.fail(g.spell_card.error);}
        bool finish_spell()override{g.score.score=g.player->damage.score;const bool ok=g.spell_card.finish();g.player->damage.score=g.score.score;return ok||g.fail(g.spell_card.error);}
        bool bomb_damage(const DamageQuery& q,i32& amount)override{return g.check(g.presentation.bomb_damage(q,amount),"Bomb damage service failed");}
        bool additional_damage(EnemyState& e,i32 incoming,i32& amount)override{const bool ok=e.damage_rule==EnemyDamageRule::animation_regions?g.combat->animation_region_damage(e,g.animations.registry.find(e.animation_handles[0]),incoming,amount):enemy_additional_damage(e,incoming,amount);if(!ok)return g.fail("Enemy additional damage rule failed");i32 extra=0;if(!g.check(g.presentation.enemy_additional_damage(e,incoming,extra),"Enemy additional damage failed"))return false;amount=wrapping_add(amount,extra);return true;}
        int enemy_death(EnemyRuntime& e,float rate)override{return g.enemy_death->execute(e,rate);}
        int scene_death(EnemyRuntime& e,float rate)override{return g.enemy_death->execute(e,rate);}
        bool contact_override(EnemyState& e,AnmVm* vm,i32& result,bool& handled)override{g.player_collision();return g.check(g.presentation.enemy_contact(e,vm,result,handled),"Enemy contact service failed");}
        bool graze(const Vec3& p)override{return g.record_graze(p);}
        bool spawn_item(const ItemSpawnRequest& r)override{return g.spawn_item(r.type,r.position,r.angle,r.speed);}
        bool effect(const EnemyDeathEffect& effect)override{return g.death_effect(effect);}
        bool callback(EnemyRuntime& e)override{return g.check(g.presentation.enemy_death_callback(e),"Enemy death callback failed");}
    } enemies;
    struct Bombs final:BombHost {
        GameBattle& g;explicit Bombs(GameBattle& g):g(g){}
        bool sound(i32 id,bool queued)override{return g.sound(id,0,PlayerShots::SoundAction::play,queued);}
        bool bomb_hud(i32 bombs,i32 pieces)override{return g.bomb_hud(bombs,pieces);}
        bool cancel_bullets(const Vec3& p,float r,i32 reward)override{return g.bullet_scene->cancel_circle(p,r,reward,false);}
        bool cancel_lasers(const Vec3& p,float r,i32 reward,bool honor)override{return g.laser_scene->cancel_circle(p,r,reward,honor)>=0;}
        bool cancel_bullets_rectangle(const Vec3& p,const Vec2& size,float angle,i32 reward)override{return g.bullet_scene->cancel_rectangle(p,size,angle,reward);}
        bool cancel_lasers_rectangle(const Vec3& p,const Vec2& size,float angle,i32 reward,bool honor)override{if(!honor)return g.fail("Unprotected laser rectangle service is not connected");return g.laser_scene->cancel_rectangle(p,size,angle,reward)>=0;}
        bool shake(const ScreenShakeSpec& s)override{return g.check(g.presentation.shake(s),"Screen shake failed");}
        bool nudge(const ScreenNudgeSpec& s)override{return g.check(g.presentation.nudge(s),"Screen nudge failed");}
    } bombs;
    struct Spells final:SpellCardServices {
        GameBattle& g;explicit Spells(GameBattle& owner):g(owner){}
        bool spell_background_visible(bool value)override{if(g.scene){g.scene->background.draw_objects=value;return true;}return g.presentation.spell_background_visible(value);}
        bool spell_prepare_hud(bool start)override{return g.scene?(g.scene->hud.prepare_spell(start)||g.fail(g.scene->hud.error)):g.presentation.spell_prepare_hud(start);}
        bool spell_title(AnmVm& vm,const std::string& value)override{return g.presentation.spell_title(vm,value);}
        bool spell_history_begin(i32 id,const std::string& title)override{return g.records?g.records->spell_begin(g.character,(g.session.mode_flags&0x300)==0,id,(g.session.mode_flags&0x30)==0x20,title):g.presentation.spell_history_begin(id,title);}
        bool spell_history_capture(i32 id)override{return g.records?g.records->spell_capture(g.character,(g.session.mode_flags&0x300)==0,id,(g.session.mode_flags&0x30)==0x20):g.presentation.spell_history_capture(id);}
        bool spell_result(i32 bonus,bool failed)override{return g.scene?(g.scene->hud.notification(bonus,failed?1:0)||g.fail(g.scene->hud.error)):g.presentation.spell_result(bonus,failed);}
        bool spell_sound(i32 id)override{return g.presentation.spell_sound(id);}
    } spells;
    explicit Services(GameBattle& g):player(g),projectiles(g),items(g),enemies(g),bombs(g),spells(g){}
};
GameBattle::GameBattle(AnmManager& a,AnmEnvironment& env,Rng& game,Rng& visual,BattleWorldServices& p,EclProgram& ecl,const ShtResource& sht,i32 ch,BattleResources ids):services(std::make_unique<Services>(*this)),animations(a),environment(env),game_random(game),visual_random(visual),presentation(p),program(ecl),resources(ids),character(ch),spell_animations(a,enemy_world,services->spells),spell_card(spell,score.score,spell_animations),enemy_visuals(a),effects(a,ids.effect){
    enemy_world.frame_rate=&input.rate;enemy_world.spell_status=&spell;spell_visuals.effect=ids.effect;
    enemy_visuals.map_resource(0,ids.bullet);enemy_visuals.map_resource(1,ids.effect);
    player=std::make_unique<Player>(sht,a,effects,game,visual,session,spell,services->player,ch,ids.player,ids.effect);
    collection=std::make_unique<ItemCollection>(session,score,player->motion,services->items,ch);items=std::make_unique<ItemManager>(a,effects,ids.bullet);
    items->sound=[this](i32 id){return sound(id,0,PlayerShots::SoundAction::play,true);};items->immediate_sound=[this](i32 id){return sound(id,0,PlayerShots::SoundAction::play);};
    bullet_scene=std::make_unique<BulletScene>(services->projectiles,game,visual);bullet_scene->resource=a.resource(ids.bullet);bullet_scene->environment=&env;bullet_scene->animation_objects=&a;
    bullet_scene->own_cancellation_animations(a,ids.bullet);
    laser_scene=std::make_unique<LaserScene>(services->projectiles,a,effects,game,visual,bullet_scene->manager.cancellation_rewards,ids.bullet);laser_scene->attach_bullets(*bullet_scene);
    combat=std::make_unique<EnemyCombat>(*player,services->enemies);enemy_damage=std::make_unique<EnemyDamage>(enemy_world,*combat,enemy_visuals);enemy_death=std::make_unique<EnemyDeath>(enemy_world,game,services->enemies);
    enemies=std::make_unique<EnemyManager>(ecl,enemy_world,game,visual,services->enemies);enemy_world.random=&game;
    bomb_context=std::make_unique<BombContext>(BombContext{a,player->motion,player->life,player->frame.bounds,player->resource.header,session,spell,bomb_enemies,services->bombs,ids.player});
    switch(ch){case 0:bomb=std::make_unique<BombReimu>(*bomb_context,player->damage,&enemy_world);break;case 1:bomb=std::make_unique<BombMarisa>(*bomb_context,player->damage);break;case 2:bomb=std::make_unique<BombSanae>(*bomb_context,effects,game,visual,player->damage);break;case 3:bomb=std::make_unique<BombReisen>(*bomb_context);break;default:error="Invalid battle character";break;}
    const FrameCallback::Function functions[]={
        [](void* p)->i32{return static_cast<GameBattle*>(p)->update_animations(true);},
        [](void* p)->i32{return static_cast<GameBattle*>(p)->update_player();},
        [](void* p)->i32{return static_cast<GameBattle*>(p)->update_bomb();},
        [](void* p)->i32{return static_cast<GameBattle*>(p)->update_enemies();},
        [](void* p)->i32{return static_cast<GameBattle*>(p)->update_lasers();},
        [](void* p)->i32{return static_cast<GameBattle*>(p)->update_bullets();},
        [](void* p)->i32{return static_cast<GameBattle*>(p)->update_spell();},
        [](void* p)->i32{return static_cast<GameBattle*>(p)->update_items();},
        [](void* p)->i32{return static_cast<GameBattle*>(p)->update_animations(false);}
    };
    for(u32 i=0;i<callbacks.size();i++){callbacks[i].owner=this;callbacks[i].run=functions[i];callbacks[i].enabled=true;schedule.add(callbacks[i],FramePass::Update,update_priorities[i]);}
}
GameBattle::~GameBattle(){for(auto& callback:callbacks)active_schedule->remove(callback);if(enemies)enemies->clear();}
bool GameBattle::attach(FrameScheduler& target){if(active_schedule==&target)return true;for(u32 i=0;i<callbacks.size();++i){auto& callback=callbacks[i];const auto run=callback.run;active_schedule->remove(callback);callback.run=run;if(target.add(callback,FramePass::Update,update_priorities[i])<0)return fail("Battle callback registration failed");}active_schedule=&target;return true;}
void GameBattle::bind_progress(SessionState& state,i32* requested_chapter)noexcept{progress=&state;enemy_world.frame_rate=&state.rate;enemy_world.scene_flags_owner=&state.scene_flags;enemy_world.chapter_request_owner=requested_chapter;refresh_world();}
// The first error survives later cleanup and reports the unavailable service.
bool GameBattle::fail(const std::string& message){if(error.empty())error=message.empty()?"Battle operation failed":message;return false;}
bool GameBattle::check(bool ok,const char* message){return ok||fail(message);}
bool GameBattle::life_hud(i32 lives,i32 pieces){return scene?(scene->hud.life(lives,pieces)||fail(scene->hud.error)):check(presentation.life_hud(lives,pieces),"Life HUD failed");}
bool GameBattle::bomb_hud(i32 stock,i32 pieces){return scene?(scene->hud.bombs(stock,pieces)||fail(scene->hud.error)):check(presentation.bomb_hud(stock,pieces),"Bomb HUD failed");}
bool GameBattle::record_graze(const Vec3& position){
    if(!scene)return check(presentation.graze_spark(position),"Graze presentation unavailable");
    score.graze_total=std::min(wrapping_add(score.graze_total,1),99999999);score.graze_chapter=std::min(wrapping_add(score.graze_chapter,1),99999999);
    score.point_value=std::min(wrapping_add(score.point_value,100),score.max_point_value);
    const auto& player_position=player->motion.position;const Vec3 middle{float(float(player_position.x+position.x)*.5f),float(float(player_position.y+position.y)*.5f),0};
    const auto handle=animations.create(resources.effect,24,-1,0,middle);if(!handle)return fail(animations.error);
    scene->popups.number(middle,score.graze_chapter,0xffc0c0ff);return sound(42,position.x,PlayerShots::SoundAction::play,true);
}
bool GameBattle::popup(const Vec3& position,i32 value,u32 color){if(scene){scene->popups.number(position,value,color);return true;}return check(presentation.popup(position,value,color),"Item popup failed");}
bool GameBattle::dialogue_present()const noexcept{return scene?scene->messages.active():presentation.dialogue_present();}
bool GameBattle::sound(i32 id,float pan,PlayerShots::SoundAction action,bool queued){return check(presentation.audio(id,pan,action,queued),"Battle audio failed");}
bool GameBattle::spawn_item(i32 kind,const Vec3& p,float angle,float speed){items->spawn(kind,p,angle,speed);return items->error.empty()||fail(items->error);}
bool GameBattle::death_effect(const EnemyDeathEffect& e){const u32 handle=animations.create(enemy_visuals.resource_id(e.resource),e.script,e.layer,2,e.position,e.rotation);if(!handle)return fail(animations.error);auto* vm=animations.registry.find(handle);if(!vm)return fail("Enemy death effect disappeared during creation");vm->visual.render_flags=(vm->visual.render_flags&~e.render_flags_clear)|e.render_flags_set;return true;}
bool GameBattle::cleanup_enemy(EnemyState& e){if(e.lifecycle_flags&4)return true;if((e.flags&0x800000)&&e.boss_slot>=0&&e.boss_slot<3)enemy_world.boss_ids[e.boss_slot]=0;if(!e.distortion_mesh.retire(animations))return fail(animations.error);for(auto& handle:e.animation_handles)if(!animations.retire(handle))return fail(animations.error);return true;}
const PlayerCollision& GameBattle::player_collision()noexcept{
    auto& collision=player->frame.collision;
    // Native contact predicates read these live fields at every query. Bombs,
    // messages and earlier contacts can change them after the player callback.
    collision.position={player->motion.position.x,player->motion.position.y};
    collision.state=player->life.state;collision.invulnerability=player->life.invulnerability.current;
    collision.radius=player->resource.header.hitbox;collision.enlarged=player->motion.behavior_flags&16;
    collision.size_multiplier=player->motion.enlargement;
    collision.laser_half_size={player->frame.bounds.hit_half_size.x,player->frame.bounds.hit_half_size.y};
    collision.bomb_active=input.hud_collect||dialogue_present();return collision;
}
bool GameBattle::refresh_world(){
    if(progress){stage=progress->stage;input.rate=progress->rate;input.game_flags=progress->scene_flags;enemy_world.mode=progress->character;enemy_world.submode=progress->subcharacter;enemy_world.difficulty=progress->difficulty;enemy_world.counter1=progress->spell_id;enemy_world.current_chapter=progress->chapter;enemy_world.spell_state=progress->transition;enemy_world.player_state=progress->new_run;enemy_world.scene_flags=progress->scene_flags;score.difficulty=progress->difficulty;}
    enemy_world.mode_flags=session.mode_flags;enemy_world.state=session.power;enemy_world.counter3=session.deaths;
    enemy_world.player_position=player->motion.position;enemy_world.player_damage_state=player->life.state;enemy_world.game_state=session.bomb_state;
    // ECL, player deaths and Bomb activation share the native manager counters.
    session.enemy_deaths=enemy_world.counters[0];session.enemy_chain=enemy_world.counters[2];
    bomb_enemies.uses=enemy_world.counters[1];bomb_enemies.chain=enemy_world.counters[2];
    bomb_enemies.available=enemy_world.enemy_count!=0;enemy_world.boss_damage_gate=enemy_world.counters[2];
    bullet_scene->manager.context.player=player->motion.position;bullet_scene->manager.context.rate=input.rate;bullet_scene->manager.cancellation_rewards.spell_active=spell.flags&1;
    player->shot_context.enemies=&enemy_world;player->shot_context.background_delta=input.background_delta;
    return error.empty();
}
bool GameBattle::refresh_player(PlayerFrameContext& c){enemy_world.counters[0]=session.enemy_deaths;enemy_world.counters[2]=session.enemy_chain;player->shot_context.screen_origin=environment.playfield_origin;c.hud_available=input.hud_available;c.hud_bomb_active=input.hud_collect||dialogue_present();c.enemy_present=true;c.enemies=&enemy_world;c.game_flags=input.game_flags;c.focus_allowed=scene?enemy_world.enemy_count!=0:input.focus_allowed;c.background_delta=input.background_delta;return refresh_world();}
bool GameBattle::initialize(bool configure_player){if(initialized)return fail("Battle already initialized");if(!error.empty())return false;if(!bullet_scene->resource||!animations.resource(resources.effect)||!animations.resource(resources.player))return fail("Battle animation resource unavailable");if(!player->initialize())return fail(player->error);if(configure_player&&!player->configure_options())return fail(player->options.error);player->damage.maximum_damage=player->resource.header.maximum_damage();initialized=true;return refresh_world();}
bool GameBattle::spawn(const EnemySpawnRequest& request){if(!initialized)return fail("Battle not initialized");if(!error.empty())return false;refresh_world();player->damage.score=score.score;enemies->rate=input.rate;enemies->background_delta=input.background_delta;const bool ok=enemies->spawn_at_rate(request,input.rate);score.score=player->damage.score;return ok?error.empty():fail(enemies->error);}
bool GameBattle::prepare_frame(const BattleFrame& frame){if(!initialized)return fail("Battle not initialized");if(!error.empty())return false;input=frame;animations.rate=input.rate;environment.paused=input.animation_paused;callbacks[1].enabled=input.player_enabled;callbacks[2].enabled=input.bomb_enabled;callbacks[3].enabled=input.enemies_enabled;callbacks[4].enabled=input.lasers_enabled;callbacks[6].enabled=input.spell_enabled;callbacks[5].enabled=input.bullets_enabled;callbacks[7].enabled=input.items_enabled;refresh_world();bomb_context->hud_available=input.hud_available;bomb_context->hud_collect=input.hud_collect||dialogue_present();return error.empty();}
bool GameBattle::step(const BattleFrame& frame){return prepare_frame(frame)&&active_schedule->update()>=0&&error.empty();}
i32 GameBattle::update_animations(bool alternate){refresh_world();if(!alternate&&!battle_animation_frame(input.game_flags))return 1;animations.rate=input.rate;if(!animations.update(alternate)){fail(animations.error);return 5;}return error.empty()?1:5;}
i32 GameBattle::update_player(){PlayerFrameContext c;c.held=input.held;c.pressed=input.pressed;if(!refresh_world()||!refresh_player(c))return 5;if(!player->update(c,input.rate)){fail(player->error);return 5;}return refresh_world()?1:5;}
i32 GameBattle::update_bomb(){refresh_world();if(!bomb->update(input.rate)){fail(bomb->error);return 5;}return refresh_world()?1:5;}
i32 GameBattle::update_enemies(){refresh_world();if(!battle_enemy_frame(input.game_flags))return 1;player->damage.score=score.score;enemies->rate=input.rate;enemies->background_delta=input.background_delta;const bool ok=enemies->update();score.score=player->damage.score;if(!ok){fail(enemies->error);return 5;}return error.empty()?1:5;}
i32 GameBattle::update_lasers(){refresh_world();if(!laser_scene->update(input.rate,input.game_flags)){fail(laser_scene->error);return 5;}return error.empty()?1:5;}
i32 GameBattle::update_spell(){refresh_world();if(!spell_card.update({input.rate,player->motion.position.y,session.bomb_state,enemy_world.practice&&enemy_world.practice->cheat(PracticeTime)})){fail(spell_card.error);return 5;}return refresh_world()?1:5;}
i32 GameBattle::update_bullets(){refresh_world();const auto mode=battle_bullet_frame(input.game_flags);if(mode==BulletFrameMode::Skipped)return 1;refresh_world();bullet_scene->manager.world_paused=mode==BulletFrameMode::VisualOnly;if(!bullet_scene->update()){fail(bullet_scene->error);return 5;}return error.empty()?1:5;}
i32 GameBattle::update_items(){refresh_world();ItemFrameContext c{player->motion,player->frame.bounds,score,*collection,visual_random};c.player_state=player->life.state;c.character=character;c.bomb_state=session.bomb_state;c.bomb_frame=bomb->age.current;c.hud_collect=input.hud_collect||dialogue_present();c.held=input.held;c.attraction_speed=player->resource.header.attraction_speed;c.rate=input.rate;if(!items->update(c)){fail(items->error);return 5;}return error.empty()?1:5;}
}

namespace th15 {
bool GameBattle::detach(FrameScheduler& owner){return active_schedule!=&owner||attach(schedule);}
bool GameBattle::release_stage_enemies(){if(enemies&&!enemies->clear())return fail(enemies->error);enemies.reset();return true;}
bool GameBattle::retire_spell_visuals(){for(u32 i=1;i<=3;i++)if(!animations.retire(spell_card.handles[i]))return fail(animations.error);return true;}
bool GameBattle::retire_bomb_visuals(){
 if(!bomb)return true;
 auto retire=[&](u32& handle){return animations.retire(handle)||fail(animations.error);};
 switch(character){
 case 0:return retire(static_cast<BombReimu&>(*bomb).aura);
 case 1:{auto& b=static_cast<BombMarisa&>(*bomb);return retire(b.beam)&&retire(b.aura);}
 case 2:{auto& b=static_cast<BombSanae&>(*bomb);return retire(b.field)&&retire(b.aura);}
 case 3:{auto& b=static_cast<BombReisen&>(*bomb);return retire(b.barrier)&&retire(b.aura);}
 }return fail("Invalid battle character during bomb retirement");
}
bool GameBattle::replace_stage(EclProgram& next,const std::array<i32,6>& banks,const SpellVisualResources& visuals){
 if(!initialized||!error.empty())return fail("Battle unavailable during stage replacement");
 if(spell.flags&1)return fail("Active spell must complete before normal stage replacement");
 if(enemies&&!enemies->clear())return fail(enemies->error);
 // The original spell destructor retires the three banner handles; finished
 // spell backgrounds and the boss ring have already taken their own path.
 if(!retire_spell_visuals()||!retire_bomb_visuals())return false;
 enemies=std::make_unique<EnemyManager>(next,enemy_world,game_random,visual_random,services->enemies);
 for(u32 i=0;i<banks.size();i++)enemy_visuals.map_resource(i,banks[i]);spell_visuals=visuals;
 spell={2,0,0};spell_card.clock={};spell_card.age.set(0);spell_card.identifier=spell_card.maximum_bonus=spell_card.duration=spell_card.active_frames=0;spell_card.name.clear();spell_card.error.clear();spell_card.anchor={};spell_card.handles.fill(0);spell_card.replay=false;
 session.bomb_state=0;
 switch(character){case 0:bomb=std::make_unique<BombReimu>(*bomb_context,player->damage,&enemy_world);break;case 1:bomb=std::make_unique<BombMarisa>(*bomb_context,player->damage);break;case 2:bomb=std::make_unique<BombSanae>(*bomb_context,effects,game_random,visual_random,player->damage);break;case 3:bomb=std::make_unique<BombReisen>(*bomb_context);break;}
 return refresh_world();
}
}
