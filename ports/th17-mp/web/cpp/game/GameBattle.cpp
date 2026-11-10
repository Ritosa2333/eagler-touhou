#include "GameBattle.hpp"
#include "AnmVisualState.hpp"
#include "StageAssets.hpp"
#include <algorithm>
#include <optional>
#include <eagler/gameplay/CooperativeRules.hpp>
#if TH17_MULTIPLAYER
#include "../multiplayer/FrameImage.hpp"
#endif
namespace th17 {
struct GameBattle::Services {
    struct PlayerServices final:PlayerHost {
        GameBattle& g;u32 seat;
        explicit PlayerServices(GameBattle& value,u32 index=0):g(value),seat(index){}
        bool allow_bomb()override{const auto* bomb=g.pilot_bomb(seat);return bomb&&bomb->allowed();}
        bool begin_bomb()override{
            auto* bomb=g.pilot_bomb(seat);
            if(!bomb||!bomb->begin())return g.fail(bomb?bomb->error:"Bomb owner unavailable");
            g.protect_teammates(seat,120);
            return true;
        }
        bool end_roaring(bool death)override{return g.items->end_roaring(death)||g.fail(g.items->error);}
        bool cancel_lasers_near(const Vec3& p,float r,i32 kind,bool honor)override{return g.laser_scene->cancel_circle(p,r,kind,honor)>=0||g.fail(g.laser_scene->error);}
        // PlayerLife's flag selects the unprotected native circle routine.
        bool cancel_bullets_near(const Vec3& p,float r,i32 kind,bool all)override{return g.bullet_scene->cancel_circle(p,r,kind,g.historical_web_replay?all:!all)||g.fail(g.bullet_scene->error);}
        bool cancel_lasers(i32 kind,bool honor)override{return g.laser_scene->cancel_all(kind,honor)||g.fail(g.laser_scene->error);}
        bool item(i32 kind,const Vec3& p,float angle,float speed)override{return g.spawn_item(kind,p,angle,speed);}
        bool options_changed()override{return g.pilot(seat).configure_options();}
        bool game_over()override{return g.pilot_game_over(seat);}
        bool respawn_damage(const Vec3& p,float radius,float growth,i32 life,i32 amount)override{return g.pilot(seat).damage.circle(p,radius,growth,life,amount)>0;}
        bool shot_audio(i32 id,ShotSoundAction action,float pan)override{return g.sound(id,pan,static_cast<PlayerShots::SoundAction>(action));}
        bool refresh(PlayerFrameContext& frame)override{return seat?g.refresh_partner(g.pilot(seat),frame):g.refresh_player(frame);}
        bool sound(i32 id,float pan,PlayerShots::SoundAction action)override{return g.sound(id,pan,action);}
        bool hit_sound(i32 id)override{return g.sound(id);}
        bool life_hud(i32 lives,i32 pieces)override{return g.life_hud(lives,pieces,seat);}
        bool bomb_hud(i32 stock,i32 pieces)override{return g.bomb_hud(stock,pieces,seat);}
    } player;
    struct Projectiles final:LaserSceneHost {
        GameBattle& g;u32 seat=0;explicit Projectiles(GameBattle& value):g(value){}
        const PlayerCollision& player_collision()const noexcept override{return g.player_collision();}
        Vec3 aim_target(const Vec3& origin)const noexcept override{return g.partner_count?g.aim_target(origin):LaserSceneHost::aim_target(origin);}
        u32 contact_players()const noexcept override{return g.pilot_count();}
        const PlayerCollision* select_contact(u32 index)noexcept override{
            seat=index;
            if(!g.pilot_active(seat)||g.pilot(seat).life.state==2)return nullptr;
            return &g.player_collision(seat);
        }
        void finish_contacts()noexcept override{seat=0;}
        const Vec3* stationary_laser_anchor()const noexcept override{if(g.historical_web_replay)return nullptr;const auto* boss=g.enemy_world.boss_at(0);return boss?&boss->motion.position:nullptr;}
        void hit()override{auto& player=g.pilot(seat);player.hit();if(!player.error.empty())g.fail(player.error);}
        void sound(i32 id)override{g.sound(id);}
        void laser_sound(i32 id,bool queued)override{g.sound(id,0,PlayerShots::SoundAction::play,queued);}
        void cancellation_effect(i32 script,const Vec3& p,const Vec3& velocity)override{
            const auto handle=g.animations.create(g.resources.bullet,script,-1,0,p);auto* vm=g.animations.registry.find(handle);if(!vm){g.fail(g.animations.error);return;}vm->interpolators.invalidate();vm->interpolators.position.begin(30,6,{0,0,0},{velocity.x,velocity.y,velocity.z});
        }
        void graze_spark(const Vec3& p)override{g.record_graze(p,seat);}
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
        GameBattle& g;u32 seat;explicit Items(GameBattle& value,u32 index=0):g(value),seat(index){}
        bool options_changed()override{auto& pilot=g.pilot(seat);return pilot.configure_options()||g.fail(pilot.options.error);}
        bool notice(i32 kind)override{return g.hud?(g.hud->result(0,kind)||g.fail(g.hud->error)):g.check(g.presentation.notice(kind),"Item notice unavailable");}
        bool sound(i32 id,bool queued)override{return g.sound(id,0,PlayerShots::SoundAction::play,queued);}
        bool popup(const Vec3& p,i32 amount,u32 color)override{return g.check(g.presentation.popup(p,amount,color),"Item popup unavailable");}
        bool life_hud(i32 lives,i32 pieces)override{return g.life_hud(lives,pieces,seat);}
        bool bomb_hud(i32 stock,i32 pieces)override{return g.bomb_hud(stock,pieces,seat);}
        bool trophy(i32 id)override{return g.check(g.presentation.trophy(id),"Trophy recording unavailable");}
        bool cancel_bullets(const Vec3& p,float radius,i32 reward)override{return g.bullet_scene->cancel_circle(p,radius,reward,!g.historical_web_replay)||g.fail(g.bullet_scene->error);}
        bool cancel_lasers(const Vec3& p,float radius,i32 reward,bool honor)override{return g.laser_scene->cancel_circle(p,radius,reward,honor)>=0||g.fail(g.laser_scene->error);}
        bool active_player_shot()const override{
            for(u32 seat=0;seat<g.pilot_count();++seat)
                for(const auto& shot:g.pilot(seat).shots.shots)if(shot.state!=0)return true;
            return false;
        }
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
        i32 primary_damage(i32 amount)override{
            return g.partner_count&&(g.player->life.state==0||g.player->life.state==2)?amount/5:amount;
        }
        bool partner_damage(const DamageQuery& query,EnemyShotDamage& hit)override{
            for(u32 seat=1;seat<g.pilot_count();++seat){
                auto& player=g.pilot(seat);
                if(!g.pilot_active(seat))continue;
                const auto result=player.damage.query(query,player.life.age.current!=player.life.age.previous);
                if(!player.damage.error.empty())return g.fail(player.damage.error);
                const i32 amount=player.life.state==0||player.life.state==2?result.amount/5:result.amount;
                hit.amount=wrapping_add(hit.amount,amount);
                hit.direct|=result.direct;
                if(result.position_written)hit.position=result.position;
            }
            return true;
        }
        bool additional_damage(EnemyState&,i32,i32& amount)override{amount=0;return true;}
        i32 final_damage(EnemyState& enemy,i32 amount)override{return g.boss_damage(enemy,amount);}
        int enemy_death(EnemyRuntime& e,float rate)override{return g.enemy_death->execute(e,rate);}
        bool contact_override(EnemyState& enemy,AnmVm* visual,i32& result,bool& handled)override{
            handled=g.pilot_count()>1;
            if(!handled)return true;
            result=0;
            for(u32 seat=0;seat<g.pilot_count();++seat){
                if(!g.pilot_active(seat))continue;
                auto& player=g.pilot(seat);
                const auto contact=enemy_player_contact(enemy,visual,g.player_collision(seat),&player);
                if(!player.error.empty())return g.fail(player.error);
                if(contact==PlayerContact::hit)result=1;
                if(contact==PlayerContact::graze&&(enemy.flags&0x200)&&enemy.age_timer.current%6==0
                    &&!g.record_graze(player.motion.position,seat))return false;
            }
            return true;
        }
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
        GameBattle& g;u32 seat;explicit Bombs(GameBattle& value,u32 index=0):g(value),seat(index){}
        bool sound(i32 id,bool queued)override{return g.sound(id,0,PlayerShots::SoundAction::play,queued);}
        bool bomb_hud(i32 stock,i32 pieces)override{return g.bomb_hud(stock,pieces,seat);}
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
namespace {
std::unique_ptr<BombController> make_bomb(i32 character,BombContext& context){
    switch(character){
    case 0:return std::make_unique<BombReimu>(context);
    case 1:return std::make_unique<BombMarisa>(context);
    case 2:return std::make_unique<BombYoumu>(context);
    default:return {};
    }
}
}
struct GameBattle::Partner {
    Services::PlayerServices player_services;
    Services::Bombs bomb_services;
    Services::Items item_services;
    PlayerLifeSession session;
    Player player;
    BombContext bomb_context;
    std::unique_ptr<BombController> bomb;
    i32 life_piece_tier=0;
    ItemCollection collection;
    GameInput input;
    i32 character;
    bool retired=false;

    Partner(GameBattle& battle,u32 seat,const ShtResource& shots,i32 character,i32 animal,i32 bank):
        player_services(battle,seat),bomb_services(battle,seat),item_services(battle,seat),
        player(shots,battle.animations,battle.effects,battle.game_random,battle.visual_random,
               session,battle.spell,player_services,character,bank,battle.resources.effect),
        bomb_context{battle.animations,player.motion,player.life.invulnerability,session,battle.spell,
                     player.damage,battle.game_random,bomb_services,battle.items->roaring.state.flags,
                     battle.enemy_world.enemy_count,bank},
        bomb(make_bomb(character,bomb_context)),collection(session,battle.score,player.motion,item_services,character,&life_piece_tier),
        character(character){
        session.animal=animal;
        session.initial_bombs=session.bombs=animal==1?4:3;
        player.damage.shared_score=&battle.score.score;
        player.damage.hit_callback=[this](DamageSource& source,const DamageQuery& query){
            if(source.callback_kind==3)return bomb->hit(source,query);
            return player.shots.hit(source,query);
        };
    }
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
    bomb=make_bomb(ch,*bomb_context);
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
GameBattle::~GameBattle(){active_schedule->remove(hud_callback);active_schedule->remove(spell_callback);for(auto& callback:callbacks)active_schedule->remove(callback);dialogue.reset();if(enemies)enemies->clear();for(auto& partner:partners)partner.reset();}
Player& GameBattle::pilot(u32 seat) noexcept{return seat?partners[seat-1]->player:*player;}
u32 GameBattle::viable_pilots() const noexcept{
    u32 count=!primary_retired&&session.extra_lives>=0;
    for(u32 seat=0;seat<partner_count;++seat)
        count+=!partners[seat]->retired&&partners[seat]->session.extra_lives>=0;
    return count;
}
i32 GameBattle::boss_damage(EnemyState& enemy,i32 amount){
    if(!partner_count||amount<=0||!(enemy.flags&0x40800000u))return amount;
    const u32 count=viable_pilots(),numerator=count>=3?8:count==2?9:12;
    const u64 scaled=u64(u32(amount))*numerator+enemy.multiplayer_damage_remainder;
    enemy.multiplayer_damage_remainder=u32(scaled%12);
    return i32(scaled/12);
}
i32 GameBattle::life_piece_tier(u32 seat) const noexcept{return seat?partners[seat-1]->life_piece_tier:score.life_piece_tier;}
void GameBattle::restore_pilot_stock(u32 seat,i32 tier,bool retired) noexcept{
    if(seat){partners[seat-1]->life_piece_tier=tier;partners[seat-1]->retired=retired;}
    else {score.life_piece_tier=tier;primary_retired=retired;}
    if(retired)pilot(seat).life.state=2;
}
bool GameBattle::initialize_partner_hud(){
    for(u32 index=0;index<partner_count;++index){
        const auto& partner=*partners[index];
        if(!hud->initialize_partner(index+1,partner.character,partner.session))return fail(hud->error);
    }
    return true;
}
BombController* GameBattle::pilot_bomb(u32 seat) const noexcept{return seat?partners[seat-1]->bomb.get():bomb.get();}
bool GameBattle::add_coop_player(const ShtResource& shots,i32 character,i32 animal,i32 bank){
    if(!initialized||partner_count>=partners.size()||u32(character)>=3||u32(animal)>=3||!animations.resource(bank))
        return fail("Invalid TH17 partner loadout");
    auto partner=std::make_unique<Partner>(*this,partner_count+1,shots,character,animal,bank);
    if(stage==7)partner->session.power=score.max_power;
    partner->player.shot_context.cooperative=true;
    partner->player.visuals.presentation_seat=partner->player.shots.presentation_seat=u8(partner_count+2);
    if(!partner->player.initialize()||!partner->player.configure_options())return fail(partner->player.error);
    partners[partner_count++]=std::move(partner);
    if(partner_count==1){
        player->shot_context.cooperative=true;
        auto& visuals=player->visuals;
        visuals.presentation_seat=player->shots.presentation_seat=1;
        const auto tag_primary=[&](auto&& tag,AnmVm& vm)->void{
            vm.pilot_draw_owner=1;
            for(auto* child:animations.registry.children(vm))tag(tag,*child);
        };
        tag_primary(tag_primary,visuals.root);
        for(const auto& pair:visuals.option_handles)for(auto handle:pair)
            if(auto* vm=animations.registry.find(handle))tag_primary(tag_primary,*vm);
        enemy_world.multiplayer_target=[this](const Vec3& origin){return aim_target(origin);};
        bullet_scene->manager.context.multiplayer_target=&enemy_world.multiplayer_target;
    }
    return true;
}
bool GameBattle::pilot_game_over(u32 seat){
    if(!partner_count)return check(presentation.game_over(),"Game-over scene unavailable");
    if(!pilot_active(seat))return true;
    transfers[seat]={};
    if(seat)partners[seat-1]->retired=true;
    else primary_retired=true;
    const u32 seed=u32(game_random.seed)^(0x9e3779b9u*(seat+1));
    ghosts[seat]={seed?seed:1};
    auto& dead=pilot(seat);
    if(dead.session.bomb_state&&!pilot_bomb(seat)->stop())return fail(pilot_bomb(seat)->error);
    if(!dead.shots.clear_lasers())return fail(dead.shots.error);
    dead.motion.option_count=0;
    for(auto& option:dead.motion.options)option.active=0;
    for(auto& pair:dead.visuals.option_handles)
        for(auto& handle:pair)if(!animations.retire(handle))return fail(animations.error);
    for(auto& shot:dead.shots.shots){
        if(!shot.state)continue;
        if(!animations.retire(shot.animation))return fail(animations.error);
        shot.state=0;
        shot.damage_source=0;
    }
    for(auto& damage:dead.damage.sources)damage.flags=0;
    if(!dead.visuals.focus_begin(false,1)||!dead.visuals.barrier_remove())return fail(animations.error);
    for(u32 index=0;index<pilot_count();++index)
        if(index!=seat&&pilot_active(index))return true;
    return check(presentation.game_over(),"Game-over scene unavailable");
}
bool GameBattle::pilot_active(u32 seat) const noexcept{return seat?!partners[seat-1]->retired:!primary_retired;}
bool GameBattle::update_ghost(u32 seat){
    auto& ghost=ghosts[seat];
    if(ghost.clock++%90==0){
        const auto next=[&](){
            ghost.random^=ghost.random<<13;
            ghost.random^=ghost.random>>17;
            ghost.random^=ghost.random<<5;
            return ghost.random;
        };
        ghost.dx=i32(next()%97)-48;
        ghost.dy=i32(next()%81)-40;
    }
    auto& dead=pilot(seat);
    auto& motion=dead.motion;
    motion.position_fixed.x=std::clamp(motion.position_fixed.x+ghost.dx,-23552,23552);
    motion.position_fixed.y=std::clamp(motion.position_fixed.y+ghost.dy,4096,55296);
    motion.position={float(motion.position_fixed.x)/128,float(motion.position_fixed.y)/128,0};
    return animations.tick_instance(dead.visuals.root)>=0||fail(animations.error);
}
bool GameBattle::revive_pilot(u32 seat,i32 invulnerability){
    auto& target=pilot(seat);
    auto& stock=target.session;
    stock.extra_lives=std::max(0,stock.extra_lives);
    stock.power=score.max_power/2;
    stock.bombs=std::clamp(stock.initial_bombs,0,8);
    if(!pilot_bomb(seat)->stop())return fail(pilot_bomb(seat)->error);
    if(!target.visuals.pose(0))return fail(animations.error);
    if(!target.reset_for_stage())return fail(target.error);
    target.motion.snap_options();
    target.life.invulnerability.set(invulnerability);
    ghosts[seat]={};
    transfers[seat]={};
    if(seat)partners[seat-1]->retired=false;
    else primary_retired=false;
    return bomb_hud(stock.bombs,stock.bomb_pieces,seat);
}
bool GameBattle::update_transfers(){
    namespace coop=eagler::cooperative;
    if(!partner_count)return true;
    if(!world_running||input.game_flags&(4|0x70|0x4000|0x10000)||dialogue_present()){
        transfers={};return true;
    }
    for(u32 seat=0;seat<pilot_count();++seat){
        auto& taps=transfers[seat];
        auto& giver=pilot(seat);
        if(!pilot_active(seat)||giver.life.state!=1){taps={};continue;}
        const u32 held=seat?partners[seat-1]->input.held:input.held;
        const u32 pressed=seat?partners[seat-1]->input.pressed:input.pressed;
        const auto nearby=[&](u32 other){
            if(seat==other)return false;
            const auto& a=giver.motion.position;const auto& b=pilot(other).motion.position;
            const float dx=a.x-b.x,dy=a.y-b.y;
            return dx*dx+dy*dy<=400.f;
        };
        const auto send=[&](i32 kind,u32 other){
            const auto id=items->items.spawn(kind,giver.motion.position,0,0);
            if(!id)return false;
            auto& item=items->items.find(id)->state;
            item.gift_seat(other+1);item.state=3;item.attraction_speed=5;
            return true;
        };
        i32 life_receiver=-1;
        if((taps.life||(pressed&8))&&!(held&1)&&giver.session.extra_lives>0){
            for(u32 other=0;other<pilot_count();++other){
                if(!nearby(other))continue;
                const auto& target=pilot(other);
                const bool retired=!pilot_active(other);
                if(!retired&&(target.life.state!=1||target.session.extra_lives>=8))continue;
                if(life_receiver<0||retired>!pilot_active(u32(life_receiver))||
                   (retired==!pilot_active(u32(life_receiver))&&target.session.extra_lives<pilot(u32(life_receiver)).session.extra_lives))
                    life_receiver=i32(other);
            }
        }
        if(coop::AdvanceLifeTransfer(taps.life,(pressed&8)!=0,life_receiver>=0)){
            const u32 other=u32(life_receiver);
            const bool rescue=!pilot_active(other);
            if(rescue||send(i32(ItemKind::Life),other)){
                --giver.session.extra_lives;
                if(rescue&&(!revive_pilot(other,280)||!sound(17,0,PlayerShots::SoundAction::play,true)))return false;
                if(!life_hud(giver.session.extra_lives,giver.session.life_pieces,seat))return false;
            }else if(!items->items.error.empty())return fail(items->items.error);
        }
        const i32 step=giver.session.power_step;
        if(pressed&8)taps.power=0;
        const i32 power_receiver=(taps.power||(pressed&1))&&step>0&&giver.session.power-step>=step?coop::LowestPower(pilot_count(),
            [&](u32 other){const auto& target=pilot(other);return nearby(other)&&pilot_active(other)&&target.life.state==1&&target.session.power<score.max_power;},
            [&](u32 other){return pilot(other).session.power;},[&](u32 other){return pilot(other).session.power_step;}):-1;
        if(coop::AdvanceLifeTransfer(taps.power,(pressed&1)!=0,power_receiver>=0)){
            if(send(i32(ItemKind::LargePower),u32(power_receiver))){
                giver.session.power-=step;
                if(!giver.configure_options())return fail(giver.options.error);
            }else if(!items->items.error.empty())return fail(items->items.error);
        }
    }
    return true;
}
bool GameBattle::draw_transfer_hints(HudDrawServices& text){
    for(u32 seat=0;seat<pilot_count();++seat){
        const auto life=eagler::cooperative::TapCount(transfers[seat].life);
        const auto power=eagler::cooperative::TapCount(transfers[seat].power);
        if(life<3&&power<3)continue;
        const auto& position=pilot(seat).motion.position;
        HudTextDraw hint;
        hint.position={position.x+224,position.y-10,0};
        hint.style.font=1;hint.style.coordinate_space=2;hint.style.alignment=0;hint.style.shadow=true;
        hint.style.color=life>=3?0xffffa0c0:0xffffa070;
        hint.text=(life>=3?"L ":"P ")+std::to_string(life>=3?life:power)+"/5";
        if(!text.hud_text(hint))return false;
    }
    return true;
}
Vec3 GameBattle::aim_target(const Vec3& origin) noexcept{
    if(!partner_count)return player->motion.position;
    const Vec3* target=nullptr;
    float nearest=0;
    for(u32 seat=0;seat<pilot_count();++seat){
        const auto& p=pilot(seat);
        if(!pilot_active(seat)||p.session.extra_lives<0||p.life.state!=1)continue;
        const auto& position=p.motion.position;
        const float x=position.x-origin.x,y=position.y-origin.y,distance=x*x+y*y;
        if(!target||distance<nearest){target=&position;nearest=distance;}
    }
    return target?*target:Vec3{0,400,0};
}
void GameBattle::protect_teammates(u32 caster,i32 frames) noexcept{
    for(u32 seat=0;seat<pilot_count();++seat){
        if(seat==caster||!pilot_active(seat))continue;
        auto& timer=pilot(seat).life.invulnerability;
        if(timer.current<frames)timer.set(frames);
    }
}
bool GameBattle::reset_players(){
    transfers={};
    for(u32 seat=0;seat<pilot_count();++seat){
        if(seat)partners[seat-1]->input={};
        if(!pilot_active(seat)){
            if(!revive_pilot(seat,120))return false;
        }else if(!pilot(seat).reset_for_stage())return fail(pilot(seat).error);
    }
    return true;
}
bool GameBattle::finish_player_options(){
    for(u32 seat=0;seat<pilot_count();++seat)
        if(pilot_active(seat)&&!pilot(seat).finish_stage_options())return fail(pilot(seat).error);
    return true;
}
bool GameBattle::stop_bombs(){
    for(u32 seat=0;seat<pilot_count();++seat){
        auto* controller=pilot_bomb(seat);
        if(!controller||!controller->stop())return fail(controller?controller->error:"Bomb owner unavailable");
    }
    return true;
}
bool GameBattle::draw_players(AnmRenderer& renderer){
    for(u32 seat=0;seat<pilot_count();++seat){
        auto& p=pilot(seat);
        if(pilot_active(seat)){
            if(!p.draw(renderer))return fail(p.error);
            continue;
        }
        if(renderer.logical_only)continue;
        // Draw a copy so native death flags and the hitbox stay in the
        // authoritative player state, while the spirit remains visible.
        auto body=p.visuals.root;
        body.pilot_draw_owner=0;
        body.visual.flags=(body.visual.flags|3u)&~0x60000u;
        body.visual.render_flags=(body.visual.render_flags&~0x80060u)|0x40000u;
        const u32 alpha=renderer.hud_viewer==seat+1?112:88;
        body.visual.color=(body.visual.color&0xffffffu)|(alpha<<24);
        body.visual.translation=p.motion.position;
        if(renderer.draw(body)==-2)return fail(renderer.error);
    }
    return true;
}
void GameBattle::prepare_player_presentation(AnmRenderer& renderer){
    renderer.pilot_opacity.fill(255);
    if(!partner_count)return;
    const u32 local=renderer.hud_viewer-1;
    if(local>=pilot_count())return;
    const auto& origin=pilot(local).motion.position;
    for(u32 seat=0;seat<pilot_count();++seat){
        const auto& position=pilot(seat).motion.position;
        const float dx=position.x-origin.x,dy=position.y-origin.y;
        renderer.pilot_opacity[seat+1]=seat==local?255:dx*dx+dy*dy<2304?72:160;
    }
}
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
const PlayerCollision& GameBattle::player_collision(u32 seat)noexcept{
    auto& p=pilot(seat);auto& c=p.frame.collision;c.position={p.motion.position.x,p.motion.position.y};c.state=p.life.state;c.invulnerability=(p.session.practice_tools&PracticeInvincible)?1:p.life.invulnerability.current;c.radius=p.resource.header.hitbox;c.size_multiplier=p.motion.enlargement;c.enlarged=p.motion.behavior_flags&16;c.laser_half_size={p.frame.bounds.hit_half_size.x,p.frame.bounds.hit_half_size.y};c.bomb_active=input.hud_collect||dialogue_present();return c;
}
bool GameBattle::refresh_world(){
    enemy_world.cooperative=partner_count!=0;
    enemy_world.mode=character;enemy_world.submode=session.animal;enemy_world.stage_number=stage;enemy_world.difficulty=score.difficulty;enemy_world.state=session.power;enemy_world.counter3=session.deaths;enemy_world.game_frame_flags=input.game_flags;
    enemy_world.player_position=player->motion.position;enemy_world.player_damage_state=player->life.state;enemy_world.player_behavior_flags=player->motion.behavior_flags;enemy_world.player_damage_scale=player->frame.bounds.damage_multiplier;enemy_world.game_state=session.bomb_state;
    if(partner_count){
        enemy_world.state=score.max_power;
        for(u32 seat=0;seat<pilot_count();++seat)
            if(pilot_active(seat))enemy_world.state=std::min(enemy_world.state,pilot(seat).session.power);
    }
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
bool GameBattle::refresh_partner(Player& pilot,PlayerFrameContext& frame){
    pilot.shot_context.screen_origin=environment.playfield_origin;
    frame.game_flags=input.game_flags;
    frame.hud_available=input.hud_available;
    frame.hud_bomb_active=input.hud_collect||dialogue_present();
    frame.focus_allowed=input.focus_allowed&&enemy_world.enemy_count!=0;
    frame.enemy_present=true;
    frame.background_delta=input.background_delta;
    frame.roaring_kind=items->roaring.state.kind;
    frame.targets.clear();
    for(auto& target:targets)frame.targets.push_back(&target);
    return error.empty();
}
bool GameBattle::initialize(bool initialize_roaring_hud){if(initialized)return fail("Battle already initialized");if(!bullet_scene->resource||!animations.resource(resources.player)||!animations.resource(resources.effect)||!animations.resource(resources.animal))return fail("Battle animation bank unavailable");if(!player->initialize())return fail(player->error);if(!player->configure_options())return fail(player->options.error);if(initialize_roaring_hud&&!items->initialize())return fail(items->error);initialized=true;return refresh_world();}
bool GameBattle::life_hud(i32 lives,i32 pieces,u32 seat){return hud?(hud->lives(lives,pieces,seat)||fail(hud->error)):seat||check(presentation.life_hud(lives,pieces),"Life HUD unavailable");}
bool GameBattle::bomb_hud(i32 stock,i32 pieces,u32 seat){return hud?(hud->bombs(stock,pieces,seat)||fail(hud->error)):seat||check(presentation.bomb_hud(stock,pieces),"Bomb HUD unavailable");}
bool GameBattle::configure_hud(const HudStartContext& context,HudResources visuals,bool initialize_now){if(hud)return fail("HUD already configured");hud_context=context;hud=std::make_unique<Hud>(animations,enemy_world,session,player->motion,spell,visuals);hud->sound=[this](i32 id){return sound(id,0,PlayerShots::SoundAction::play,true);};enemy_world.boss_seconds=-1;return !initialize_now||hud->initialize(context)||fail(hud->error);}
bool GameBattle::configure_dialogue(const StageConfiguration& stage,const MessageResource& messages,DialogueServices& host,DialogueResources visuals){if(dialogue||!hud)return fail("Dialogue configuration requires an idle HUD");dialogue_stage=&stage;message_resource=&messages;dialogue_host=&host;dialogue_resources=visuals;hud->update_dialogue=[this](){return update_dialogue();};return true;}
bool GameBattle::scene_message(i32 id){if(id<0||!message_resource)return check(presentation.scene_message(id),"Dialogue scene command unavailable");if(!presentation.rollback_boundary())return false;dialogue.reset();dialogue=std::make_unique<Dialogue>(animations,services->dialogues,*dialogue_stage,dialogue_resources);return dialogue->initialize(*message_resource,id)||fail(dialogue->error);}
bool GameBattle::message_complete(bool& done){if(!message_resource)return check(presentation.message_complete(done),"Dialogue state unavailable");done=!dialogue||dialogue->complete_pulse!=0;return true;}
bool GameBattle::update_dialogue(){if(!dialogue)return true;const i32 result=dialogue->update({input.rate,input.held,input.pressed,input.shoot_frames,input.skip_frames});if(result<0)return fail(dialogue->error);if(result>0)dialogue.reset();return true;}
bool GameBattle::configure_spell(SpellRecords& records,SpellVisualResources visuals){if(spell.flags&1)return fail("Cannot reconfigure an active spell");spell_context.visuals=std::move(visuals);spell_visuals=std::make_unique<SpellAnmHost>(animations,enemy_world,services->spells);spell_card=std::make_unique<SpellCard>(spell,score.score,records,items->roaring.state.flags,*spell_visuals);return true;}
bool GameBattle::begin_spell(const SpellStartRequest& request){if(!spell_card)return fail("Spell owner unavailable");spell_context.stage=stage;spell_context.difficulty=score.difficulty;spell_context.character=character;spell_context.animal=session.animal;spell_context.replay=session.replay_state==1||enhanced_practice;spell_context.practice=(enemy_world.mode_flags>>4&3)==2;return spell_card->begin(request,spell_context)||fail(spell_card->error);}
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
i32 GameBattle::update_player(){
    PlayerFrameContext frame;frame.held=input.held;frame.pressed=input.pressed;items->before_player();
    if(!refresh_player(frame)){fail(player->error);return 5;}
    if(primary_retired){
        if(!update_ghost(0))return 5;
    }else if(!player->update(frame,input.rate)){fail(player->error);return 5;}
    items->after_player();
    for(u32 i=0;i<partner_count;++i){
        auto& partner=*partners[i];
        if(partner.retired){
            if(!update_ghost(i+1))return 5;
            continue;
        }
        partner.input.update(input.partners[i].held);
        partner.player.motion.touch=input.partners[i].touch;
        const auto timer=items->roaring.state.countdown.current;
        partner.session.roaring_flags=items->roaring.state.flags;
        partner.session.roaring_countdown=items->roaring.state.countdown;
        partner.bomb_context.hud_available=input.hud_available;
        partner.bomb_context.hud_collect=input.hud_collect||dialogue_present();
        PlayerFrameContext lane;lane.held=partner.input.held;lane.pressed=partner.input.pressed;
        if(!refresh_partner(partner.player,lane)||!partner.player.update(lane,input.rate)){fail(partner.player.error);return 5;}
        // A partner's miss can end the shared mode. Direct breaks already
        // update the owner, so avoid writing the partner's stale countdown.
        if(partner.session.roaring_countdown.current!=timer&&items->roaring.state.countdown.current==timer)
            items->roaring.state.countdown=partner.session.roaring_countdown;
    }
    items->before_player();
    return refresh_world()?1:5;
}
i32 GameBattle::update_bomb(){
    for(u32 seat=0;seat<pilot_count();++seat){
        auto* controller=pilot_bomb(seat);
        const bool active=pilot(seat).session.bomb_state==1;
        if(!controller||!controller->update(input.rate)){fail(controller?controller->error:"Bomb owner unavailable");return 5;}
        if(active)protect_teammates(seat,40);
    }
    return refresh_world()?1:5;
}
i32 GameBattle::update_enemies(){refresh_world();enemies->rate=input.rate;enemies->background_delta=input.background_delta;const bool ok=enemies->update();player->motion.behavior_flags=enemy_world.player_behavior_flags;player->frame.bounds.damage_multiplier=enemy_world.player_damage_scale;if(!ok){fail(enemies->error);return 5;}return error.empty()?1:5;}
i32 GameBattle::update_lasers(){refresh_world();if(!laser_scene->update(input.rate,input.game_flags)){fail(laser_scene->error);return 5;}return error.empty()?1:5;}
i32 GameBattle::update_bullets(){if((input.game_flags>>2|input.game_flags)&1)return 1;refresh_world();bullet_scene->manager.world_paused=input.game_flags&2;if(!bullet_scene->update()){fail(bullet_scene->error);return 5;}return error.empty()?1:5;}
ItemSpiritFrame GameBattle::item_frame()const{return {input.rate,player->resource.header.attraction_speed,player->life.state,session.bomb_state,bomb?bomb->age.current:0,input.held,input.hud_collect||dialogue_present(),input.practice};}
i32 GameBattle::update_items(){
    if(((input.game_flags>>2|input.game_flags)&1)||(input.game_flags&0x400))return 1;
    if(!update_transfers())return 5;
    std::array<std::optional<ItemFrameContext>,2> contexts;
    std::array<ItemFrameContext*,2> lanes{};
    const auto frame=item_frame();
    for(u32 i=0;i<partner_count;++i){
        auto& partner=*partners[i];
        auto& p=partner.player;
        contexts[i].emplace(ItemFrameContext{p.motion,p.frame.bounds,partner.collection,visual_random,frame.rate,
            p.resource.header.attraction_speed,partner.character,p.life.state,p.session.bomb_state,partner.bomb->age.current,
            partner.input.held,frame.hud_collect,!partner.retired});
        lanes[i]=&*contexts[i];
    }
    if(!items->update_items(frame,lanes,!primary_retired)){fail(items->error);return 5;}
    return refresh_world()?1:5;
}
i32 GameBattle::update_spirits(){
    auto frame=item_frame();
    if(!partner_count){
        if(!items->update_spirits(frame)){fail(items->error);return 5;}
    }else{
        AnimalSpiritTargets team;
        for(u32 seat=0;seat<pilot_count();++seat){
            const auto& p=pilot(seat);
            team[seat]={p.motion.position,pilot_active(seat)&&p.life.state==1};
        }
        for(u32 i=0;i<partner_count;++i)frame.held|=partners[i]->input.held;
        if(!items->update_spirits(frame,team)){fail(items->error);return 5;}
        const auto& roaring=items->roaring.state;
        for(u32 seat=0;seat<pilot_count();++seat){
            auto& shots=pilot(seat).shots;
            if(!shots.synchronize_roaring(roaring.flags,roaring.kind)){fail(shots.error);return 5;}
        }
    }
    return refresh_world()?1:5;
}
i32 GameBattle::update_hud(){if(hud){hud->current_seconds=enemy_world.boss_seconds;hud->game_flags=input.game_flags;hud->dialogue=dialogue_present();if(!hud->update(input.rate)){fail(hud->error);return 5;}}return error.empty()?1:5;}
#if TH17_MULTIPLAYER
void GameBattle::save_rollback(multiplayer::FrameImage& image){
    image.save(primary_retired,ghosts,transfers,spell_context,hud_context,input,world_running,session,spell,score,
        effects.handles,effects.cursor,effects.error,grid_viewport,graze_total,graze_chapter,error);
    image.restore_with([this]{dialogue.reset();targets.clear();services->projectiles.finish_contacts();return true;});
    struct PilotProjectiles {
        decltype(PlayerShots::shots) shots;
        decltype(DamageSources::sources) sources;
    };
    struct ProjectileSnapshot {
        std::vector<PilotProjectiles> pilots;
        // These fixed-size arrays own no resources. The next capture
        // overwrites them and the pool retains at most one spare.
        void reset(){}
    };
    auto& projectiles=image.reuse<ProjectileSnapshot>().pilots;
    projectiles.resize(pilot_count());
    const auto save_pilot=[&](Player& p,BombController& b,i32 ch,u32 seat){
        auto& v=p.visuals;auto& f=p.frame;auto& life=p.life;auto& shot=p.shot_context;auto& shooting=f.shooting;
        auto& damage=p.damage;
        auto& projectile=projectiles[seat];
        projectile.shots=p.shots.shots;
        projectile.sources=damage.sources;
        image.save(p.resource.header,p.motion,p.error,p.initialized,v.root,v.focus_handle,v.barrier_handle,
            v.option_handles,p.options.power_level,p.options.error);
        image.save(damage.cursor,damage.maximum_damage,damage.score,damage.shared_score,damage.uncap_score,
            damage.error,damage.hit_callback);
        image.save(p.shots.error,p.shots.rate,p.shots.reward_counter,p.shots.reward_auxiliary,
            shot.option_angles,shot.laser_power,shot.power,shot.power_step,shot.shoot_frame,shot.player_state,
            shot.roaring_kind,shot.roaring_flags,shot.enemy_scene,shot.bomb_active,shot.screen_origin,shot.background_delta);
        image.save(life.state,life.age,life.invulnerability,life.last_death_position,life.error,
            f.input_age,f.ready_age,f.bounds,f.collision,f.error,shooting.shot,shooting.continuous,shooting.charge,
            shooting.charge_origin,shooting.charge_handle,shooting.auxiliary,shooting.auxiliary_active,shooting.error);
        image.restore_with([&p,saved=&projectile]{
            p.shots.shots=saved->shots;
            p.damage.sources=saved->sources;
            p.frame.context={};p.frame.active_rate=nullptr;p.shot_context.targets.clear();p.visuals.hitbox_display.reset();
            return true;
        });
        auto& c=b.context;
        image.save(b.position,b.angle,b.age,b.main_handle,b.secondary_handle,b.aura_handle,b.failed_spell,b.error,
            b.score_popup,b.rate,c.hud_available,c.hud_collect,c.roaring_available,c.enemy_available);
        if(ch==0){
            auto& reimu=static_cast<BombReimu&>(b);
            std::optional<std::array<BombOrb,24>> orbs;
            if(reimu.orbs)orbs=*reimu.orbs;
            image.restore_with([&reimu,orbs]{
                if(orbs){if(!reimu.orbs)reimu.orbs=std::make_unique<std::array<BombOrb,24>>();*reimu.orbs=*orbs;}
                else reimu.orbs.reset();
                return true;
            });
        }else if(ch==2){
            auto& youmu=static_cast<BombYoumu&>(b);
            image.save(youmu.handles,youmu.sides,youmu.heights,youmu.timers,youmu.emitted,youmu.interval,youmu.spawn_age);
        }
    };
    save_pilot(*player,*bomb,character,0);
    for(u32 seat=0;seat<partner_count;++seat){
        auto& p=*partners[seat];
        image.save(p.session,p.life_piece_tier,p.collection.error,p.input,p.retired);
        save_pilot(p.player,*p.bomb,p.character,seat+1);
    }
    auto& h=*hud;auto& s=*spell_card;
    image.save(h.seats,h.partner_stock,h.life_handles,h.bomb_handles,h.clock_handles,h.gauges,h.phase_handles,
        h.bonus_digits,h.bonus_separators,h.result_banner,h.pickup_banner,h.result_panel,h.result_active,
        h.stage_clear_notice,h.chapter_result_notice,h.stage_clear_bonus,h.displayed_score,h.score_increment,h.chapter_result,
        h.base,h.indicator,h.difficulty_banner,h.difficulty_label,h.loadout_label,h.boss_name,h.notice_handle,
        h.flags,h.age,h.notice_age,h.phase_count,h.current_seconds,h.previous_seconds,h.dialogue,h.boss_hidden,h.game_flags,h.error);
    image.save(s.age,s.maximum_bonus,s.duration,s.active_frames,s.loadout,s.result_clock,s.name,s.error,s.anchor,s.handles,
        s.replay,s.practice,s.uncap_score,s.hide_master);
    screen_effects.save_rollback(image);
    items->save_rollback(image);
    enemies->save_rollback(image);
    bullet_scene->save_rollback(image);
    laser_scene->save_rollback(image);
}
#endif
i32 GameBattle::update_spell(){if(spell_card&&!spell_card->update({input.rate,player->motion.position.y,session.bomb_state,(session.practice_tools&PracticeTimeLock)!=0})){fail(spell_card->error);return 5;}return error.empty()?1:5;}
bool GameBattle::record_graze(const Vec3& p,u32 seat){graze_total=std::min(wrapping_add(graze_total,1),99999999);graze_chapter=std::min(wrapping_add(graze_chapter,1),99999999);const auto& player_position=pilot(seat).motion.position;const Vec3 middle{float(float(p.x+player_position.x)*.5f),float(float(p.y+player_position.y)*.5f),0};if(!animations.create(resources.effect,24,-1,0,middle))return fail(animations.error);return check(presentation.popup(middle,graze_chapter,0xffc0c0ff),"Graze popup unavailable")&&sound(42);}
}
