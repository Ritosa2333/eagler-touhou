#pragma once
// Internal typed connections between battle managers. Only GameBattle.cpp
// includes this file; all public platform interaction is BattlePresentation.
#include "GameBattle.hpp"
#include "EclGameOperations.hpp"
#include "EclSpecial.hpp"
#include "EnemyDeath.hpp"
#include "CharacterAttackMeter.hpp"
#include "EnemyDraw.hpp"
#include "BulletDraw.hpp"
#include <algorithm>
#include <cmath>
namespace th09 {
struct GameBattle::Services {
    GameBattle& g;DamageRules* active_damage=nullptr;
    struct Field final:PlayerActions,EnemyFrameActions,EnemyDeathActions,EclSceneActions,BulletFrameActions,
                       BulletEmissionActions,LaserEmissionActions,LaserCollisionActions,EnemyAnimationActions,
                       EclSpecialActions,AreaCollisionActions,EnemyDrawServices,BulletDrawServices {
        Services& root;i32 side;std::unique_ptr<EclGameOperations> commands;EclSpecial special{*this};BattleField* sprite_owner=nullptr;
        Field(Services& r,i32 s):root(r),side(s){}
        GameBattle& game(){return root.g;}const GameBattle& game()const{return root.g;}
        BattleField& field(){return root.g.fields[side];}const BattleField& field()const{return root.g.fields[side];}
        Player& player(){return *field().player;}const Player& player()const{return *field().player;}
        BattleField& other(){return root.g.fields[1-side];}const BattleField& other()const{return root.g.fields[1-side];}
        Combo combo_system(){return Combo(game().world,*this,game().state.timing,game().state.geometry[side],game().state.geometry[1-side].width,side);}
        void play_sound(i32 id,i32 pan)override{game().output.sound(id,pan);}
        void play_positioned_sound(i32 id,float x)override{game().output.positioned_sound(side,id,x);}
        void sound_pan(i32 id,i32 pan)override{play_sound(id,pan);}
        void start_animation(AnmVm& vm,i32 script)override{root.start(character_animation(side),vm,script,false);}
        bool advance_animation(AnmVm& vm)override{return game().resources.advance(vm);}
        void draw_animation(AnmVm& vm)override{game().output.animation(vm);}
        void draw_animation(AnmVm& vm,bool fading)override{game().output.animation(vm,fading?BattleSprite::automatic:BattleSprite::rotated);}
        void draw_body(AnmVm& vm)override{game().output.animation(vm,BattleSprite::unrotated);}
        void draw_item(AnmVm& vm)override{game().output.animation(vm,BattleSprite::unrotated);}
        void item_animation(AnmVm& vm,i32 script)override{root.start(AnimationFile::bullets,vm,script,false);}
        void body_interrupt(AnmVm& vm,i32 script)override{root.start(character_animation(side),vm,script,false);}
        void effect(i32 type,const Vec3& p)override{root.effect(side,type,p);}
        void effect(i32 type,const Vec3& p,i32 slot)override{root.effect(side,type,p,slot);}
        void effect(const EclEffectRequest& request)override;
        bool effect_active(i32 slot)override{auto* e=field().effects->get_slot(slot);return e&&e->active;}
        Vec3 effect_position(i32 slot)override{auto* e=field().effects->get_slot(slot);return e?e->position:Vec3{};}
        void begin_charge()override{game().output.notice(side,BattleNotice::begin_charge);}
        void end_charge()override{game().output.notice(side,BattleNotice::end_charge);}
        bool opponent_boss_available()override{return other().enemies->find_attack(3)==nullptr;}
        bool opponent_has_boss()const override{return other().enemies&&other().enemies->find_attack(3);}
        bool rewards_blocked()const override{return game().state.rewards_blocked;}
        i32 opposing_spirits()const override{return other().enemies?other().enemies->spirit_alive:0;}
        void charged_attack(i32 level,const std::string& name)override{game().begin_attack(1-side,level,level,0,name.c_str());}
        void attack(i32 variant,i32 level)override{game().begin_attack(1-side,variant,level,0,player().resource.spell_names[level].c_str());}
        void protection_effect(i32 type,const Vec3& p)override{effect(type,p);}
        void opponent_wins(i32 winner)override{game().output.end_round(winner);}
        void slotted_effect(i32 type,const Vec3& p,i32 slot,u32 color)override{root.effect(side,type,p,slot,color);}
        void critical_health()override{game().output.notice(side,BattleNotice::critical_health);}
        void damage_flash(i32 target,i32 frames,u32 color)override{game().output.damage_flash(target,frames,color);}
        void begin_focus(const Vec3&,u32,u32 character)override;
        void end_focus()override;
        void shield_position(const Vec3& p)override{if(field().shield)field().shield->position=p;}
        void remove_shield()override{if(field().shield)field().shield->flags0=1;field().shield=nullptr;}
        void charge_level(i32 level)override{game().output.notice(side,BattleNotice::charge_level,level);}
        void charge(float amount)override{player().charge(amount);}
        TransferParameters* create_transfer(i32 kind,const Vec3& p,const Vec3& to)override{return root.transfer(side,kind,p,to,0);}
        TransferParameters* delayed_transfer(i32 kind,const Vec3& p,const Vec3& to,float delay)override{return root.transfer(side,kind,p,to,delay);}
        void critical_opponent_time(i32 frames)override{game().output.notice(side,BattleNotice::begin_survival,frames);}
        void opponent_survival_display(i32 frames)override{game().output.notice(1-side,BattleNotice::survival_time,frames);}
        void opponent_survival_expired()override{game().output.notice(1-side,BattleNotice::survival_expired);}
        void add_score(i32 amount)override{game().rules.scores[side].add(amount);}
        void score_popup(const Vec3& p,i32 amount,u32 color)override{game().output.score_popup(side,p,amount,color);}
        void character_meter(ComboState& combo,const Vec3& p)override;
        void reset_chain()override{player().combo_state.reset_chain();}
        i32 chain_count()const override{return player().combo_state.kills;}
        void chain_kill(const Vec3& p,i32 a,i32 b,i32 c,i32 score)override{combo_system().kill(player().combo_state,p,a,b,c,score);}
        void combo(const Vec3& p,i32 a,i32 b,i32 c,i32 score)override{combo_system().add(player().combo_state,p,a,b,c,score);}
        void explosion(const Vec3& p,float size,float growth,i32 damage,i32 lifetime,i32 delay)override{player().shots.areas.circle(p,size,growth,damage,lifetime,delay,AttackAreaKind::reflect_damage);}
        void drop_item(i32 type,const Vec3& p)override{player().items.spawn(type,p,game().state.scene_phase!=0);}
        void attack_position(i32 s,const Vec3& p)override{game().output.attack_position(s,p);}
        void body_collision(const Vec3& p,const Vec3& size)override{player().hazards.box(p,size);}
        i32 shot_damage(const Vec3& p,const Vec3& extent,i32& direct,i32& token,i32& special_damage)override;
        void boss_position(i32 slot,const Vec3& p)override{game().output.boss_indicator_position(side,slot,p);}
        void boss_state(i32 slot,u32 value)override{game().output.boss_indicator(side,slot,i32(value));}
        void boss_indicator(i32 slot,i16 value)override{game().output.boss_indicator(side,slot,value);}
        void boss_indicator_position(i32 slot,const Vec3& p)override{boss_position(slot,p);}
        // ECL 128 increments this count but the shipped 1.50a program never
        // allocates an attached actor. Its 24 pointer slots remain null.
        void release_attached_effects(EclVm& e)override{e.status.attached_effect_count=0;}
        void update_attached_effects(EclVm&)override{}
        bool enemy_death(EclVm& e,i32 source)override{return EnemyDeath::apply(e,source,game().state.geometry[side],game().state.geometry[1-side].width,*this);}
        void finish_match()override{game().output.finish_match();}
        void scene_setting(i32 value)override{game().output.background_setting(side,value);}
        void add_script_extra_time(i32 value)override{game().state.script_extra_time=wrapping_add(game().state.script_extra_time,value);}
        void end_attack(u32 kind)override{field().attacks->notify_pattern(kind);}
        bool start_animation(EclVm& e,u32 slot,bool character_resource,i32 script)override;
        bool prepare(Bullet& b,u32 index,i32 type,i32 color,u32 flags)override{return field().bullet_visuals->prepare(b,index,type,color,flags);}
        bool advance_animation(Bullet& b,BulletAnimation kind)override{return field().bullet_visuals->advance(b,u32(&b-field().bullets->pool.data()),kind);}
        AreaCollisionContext area_context();
        i32 probe_attacks(Bullet& b)override{auto c=area_context();return player().shots.areas.probe(b.position,b.hitbox,player().motion.graze_bounds,&b,c);}
        i32 collide_attacks(Bullet& b)override{auto c=area_context();return player().shots.areas.cancel(b.position,&b,c);}
        void collide_player(Bullet& b)override{player().hazards.box(b.position,b.hitbox,&b);}
        void change_type(Bullet& b,i32 type,i32 color)override{if(!field().bullet_visuals->change_type(b,u32(&b-field().bullets->pool.data()),type,color))game().fail("Bullet appearance");}
        bool emit_children(const BulletEmission& b)override{root.emit(side,false,b);return game().error.empty();}
        bool emit_capture_bullets(const BulletEmission& b)override{root.emit(side,false,b);return game().error.empty();}
        bool emit(const BulletEmission& b)override{root.emit(side,true,b);return game().error.empty();}
        void clear(i32)override{} // The original ECL clear helper is empty.
        Laser* emit_laser(const BulletEmission& b)override{return field().lasers->create(b,player().motion.position,field().bullets->cancel_frames);}
        void add_laser_collision(const Vec2& p,const Vec2& size,const Vec3& pivot,float angle)override{player().hazards.rotated_box({p.x,p.y,0},{size.x,size.y,0},pivot,angle);}
        void queue_attack(i32 kind,i32 source,const Vec3& p,const Vec3* extra)override{root.spawn_attack(kind,source,p,extra,nullptr);}
        Bullet* bullets(i32 s,u32& count)override{sprite_owner=&game().fields[s];count=BulletManager::update_count;return sprite_owner->bullets->pool.data();}
        void set_sprite(Bullet& b,i32 sprite)override;
        void draw_strip(AnmVm& vm,const EnemyTrailVertex* p,u32 n)override;
        void begin_field()override{game().output.begin_field(side);}
        void upper_effects()override{field().effects->draw(1);}
    };
    struct Effects final:EffectServices {
        Services& root;explicit Effects(Services& r):EffectServices(r.g.world.random),root(r){}
        void sync();
        void start_animation(AnmVm& vm,i32 side,bool character,i32 script)override{root.start(character?character_animation(side):AnimationFile::bullets,vm,script,false);}
        bool advance_animation(AnmVm& vm)override{return root.g.resources.advance(vm);}
        void draw_animation(AnmVm& vm)override{root.g.output.animation(vm);}
        void play_sound(i32 id,i32 pan)override{root.g.output.sound(id,pan);}
        void emit_bullets(i32 side,const BulletEmission& b)override{root.emit(side,false,b);}
        EclVm* spawn_spirit(i32 side,i32 script,const Vec3& p)override{return root.g.fields[side].enemies->create({script,p,20,-2,1000,0,0});}
        void fire_shots(i32 side,u32 set,i32 time)override;
        void begin_layer(i32 side,i32)override{root.g.output.begin_field(side);}
        void draw_color_fan(AnmVm& vm,const AttackColorVertex* p,u32 n)override{root.g.output.colored(&vm,p,n,BattleGeometry::fan);}
        void draw_color_strip(AnmVm& vm,const AttackColorVertex* p,u32 n)override{root.g.output.colored(&vm,p,n,BattleGeometry::strip);}
        void draw_texture_strip(AnmVm& vm,const AttackTextureVertex* p,u32 n)override{root.g.output.textured(vm,p,n,BattleGeometry::strip);}
        void draw_additive_lines(const AttackColorVertex* p,u32 n)override{root.g.output.colored(nullptr,p,n,BattleGeometry::lines,true);}
    } effects{*this};
    struct Attacks final:CharacterAttackServices {
        Services& root;explicit Attacks(Services& r):CharacterAttackServices(r.g.world.random),root(r){}
        void sync();
        void play_sound(i32 id,i32 pan)override{root.g.output.sound(id,pan);}
        void advance_animation(AnmVm& vm)override{root.g.resources.advance(vm);}
        bool start_animation(AttackActor& a,u32 slot,i32 side,AttackAnimationResource resource,i32 script)override{return root.start(resource==AttackAnimationResource::shared_effects?AnimationFile::bullets:character_animation(side),a.animations[slot],script,false);}
        // Character attacks query cancellation areas and proximity. Ordinary
        // player shots must not be consumed by this query.
        i32 probe_cancellation(i32 side,const Vec3& p,const Vec3& extent)override{auto& field=root.g.fields[side];auto context=root.fields[side]->area_context();return field.player->shots.areas.probe(p,extent,field.player->motion.graze_bounds,nullptr,context);}
        void collide_player(i32 side,const Vec3& p,float radius)override{root.g.fields[side].player->hazards.circle(p,radius);}
        void collide_player_box(i32 side,const Vec3& p,const Vec3& extent)override{root.g.fields[side].player->hazards.box(p,extent);}
        void spawn_enemy(i32 side,const EnemySpawn& request,const EclLocals& locals)override{root.g.fields[side].enemies->create(request,&locals);}
        void effect(i32 side,i32 kind,const Vec3& p)override{root.effect(side,kind,p);}
        Bullet* emit_bullets(i32 side,bool secondary,const BulletEmission& b)override{return root.emit(side,secondary,b);}
        BulletManager& bullet_manager(i32 side)override{return *root.g.fields[side].bullets;}
        void scale_player_effect(i32 side,float amount)override{auto& scale=root.g.fields[side].player->motion.effect_scale;scale.x*=amount;scale.y*=amount;}
        void begin_attack_layer(i32 side)override{root.g.output.begin_field(side);}
        void draw_animation(AnmVm& vm)override{root.g.output.animation(vm);}
        void draw_color_fan(const AnmVm& vm,const AttackColorVertex* p,u32 n)override{root.g.output.colored(&vm,p,n,BattleGeometry::fan);}
        void draw_texture_fan(const AnmVm& vm,const AttackTextureVertex* p,u32 n)override{root.g.output.textured(vm,p,n,BattleGeometry::fan);}
        void draw_additive_lines(const AttackColorVertex* p,u32 n)override{root.g.output.colored(nullptr,p,n,BattleGeometry::lines,true);}
        AttackActor* spawn_attack(i32 kind,i32 side,const Vec3& p,const Vec3* extra,const AttackActor* parent)override{return root.spawn_attack(kind,side,p,extra,parent);}
    } attacks{*this};
    struct Controller final:AttackControllerServices {
        Services& root;i32 side;Controller(Services& r,i32 s):root(r),side(s){}
        void pull();void push();
        EclVm* boss(i32 side)override{return root.g.fields[side].enemies->find_attack(3);}
        void spawn_attack_enemy(i32 target,i32 script,i32 life,i32 score)override;
        void start_animation(AnmVm& vm,i32 script)override{root.start(AnimationFile::front,vm,script,false);}
        void set_sprite(AnmVm& vm,bool text,i32 sprite)override{root.sprite(text?AnimationFile::text:AnimationFile::front,vm,sprite);}
        bool advance_animation(AnmVm& vm)override{return root.g.resources.advance(vm);}
        void draw_animation(AnmVm& vm)override{root.g.output.animation(vm,BattleSprite::unrotated);}
        void draw_text(AnmVm& vm,const char* text,u32 color,u32 shadow)override{root.g.output.text(vm,text,color,shadow);}
        void play_sound(i32 id,i32 pan)override{root.g.output.sound(id,pan);}
        void background_transition(i32 target,i32 state,i32 frames)override{root.g.output.background_transition(target,state,frames);}
        void reset_background(i32 target)override{root.g.output.reset_background(target);}
        void boss_background(i32 target)override{root.g.output.boss_background(target);}
        void portrait(i32 target,u32 layer,i32 script)override{root.g.output.portrait(target,layer,script);}
        void begin_draw(i32 target)override{root.g.output.begin_field(target);}
    };
    std::array<std::unique_ptr<Field>,2> fields;
    std::array<std::unique_ptr<Controller>,2> controllers;
    explicit Services(GameBattle& game):g(game){for(i32 side=0;side<2;++side){fields[side]=std::make_unique<Field>(*this,side);controllers[side]=std::make_unique<Controller>(*this,side);}}
    bool start(AnimationFile file,AnmVm& vm,i32 script,bool reset=true){return g.resources.start(file,vm,script,reset)||g.fail("Animation start");}
    void sprite(AnimationFile file,AnmVm& vm,i32 index){if(!g.resources.sprite(file,vm,index))g.fail("Animation sprite");}
    EffectActor* effect(i32 side,i32 kind,const Vec3&,i32 slot=-1,u32 color=0xffffffff);
    TransferParameters* transfer(i32 source,i32 kind,const Vec3&,const Vec3&,float delay);
    Bullet* emit(i32 side,bool secondary,const BulletEmission&);
    AttackActor* spawn_attack(i32 kind,i32 source,const Vec3& p,const Vec3* extra,const AttackActor* parent){attacks.sync();return g.attack_queue?g.attack_queue->create(kind,source,p,extra,parent):nullptr;}
};
}
