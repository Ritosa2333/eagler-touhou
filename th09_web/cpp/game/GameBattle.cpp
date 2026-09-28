#include "GameBattleServices.hpp"
namespace th09 {
GameBattle::GameBattle(EclWorldState& w,MatchRules& r,GameResources& res,AnmExecutor& a,BattlePresentation& out):
    world(w),rules(r),resources(res),animations(a),output(out),behaviors(character_attack_behaviors()){
    services=std::make_unique<Services>(*this);
}
GameBattle::~GameBattle()=default;
bool GameBattle::fail(const char* reason){if(error.empty())error=reason;return false;}
void GameBattle::configure(i32 left,i32 right){fields[0].script.side=0;fields[1].script.side=1;fields[0].script.character=left;fields[1].script.character=right;}
bool GameBattle::initialize_field(i32 side,i32 controller,i32 health){
    if(side<0||side>1)return fail("Battle side");auto& f=fields[side];auto& a=*services->fields[side];
    if(f.player)return fail("Battle field already initialized");const auto* profile=character_resources(u32(f.script.character));const auto* opposing=character_resources(u32(fields[1-side].script.character));
    auto* bullets=resources.animation(AnimationFile::bullets);if(!profile||!opposing||!bullets)return fail("Battle resources");
    std::vector<u8> data;if(!resources.read(profile->shots,data))return fail("Shot resource read");
    f.player=std::make_unique<Player>(world,a);if(!f.player->initialize(data.data(),data.size(),side,f.script.character,controller))return fail("Shot resource load");
    f.player->reset_round(health,world.difficulty,rules.progress.round);f.script.player=f.player->motion.position;
    for(u32 n=0;n<2;++n)f.script.attack_levels[n]=f.player->attack_levels[n];
    f.bullets=std::make_unique<BulletManager>();f.bullets->cancel_frames=5;f.bullet_visuals=std::make_unique<BulletVisuals>(*bullets,animations);if(!f.bullet_visuals->initialize())return fail("Bullet templates");
    f.lasers=std::make_unique<LaserManager>(*bullets,animations);f.effects=std::make_unique<EffectManager>(services->effects,side,256,7);
    f.enemies=std::make_unique<EnemyManager>(world,f.script,fields[1-side].script);auto& m=*f.enemies;
    if(!resources.read("enemy.ecl",data)||!m.common_program.load(data.data(),data.size()))return fail("Common enemy program");
    if(!resources.read(opposing->enemies,data)||!m.character_program.load(data.data(),data.size()))return fail("Character enemy program");
    a.commands=std::make_unique<EclGameOperations>(m,a);m.bindings={&a.special,a.commands.get(),&a,&a,&a};m.player=&f.enemy_player;m.frame_actions=&a;
    m.timing=state.timing;m.frame_step=state.ecl_step;m.difficulty_mask=state.difficulty_mask;
    if(side==0)for(auto& pattern:patterns){pattern=u8(world.random.bounded32(m.common_program.timeline_count()));pattern|=u8(world.random.bounded32(2)<<7);}
    // Initialization selects the first timeline without applying its mirror bit.
    if(!m.timeline.start(m.common_program,patterns[m.pattern_index++]&127,0))return fail("Initial enemy timeline");
    m.timeline.wait.reset(200);m.capture_lifetime=f.script.character==5?200:160;
    f.attacks=std::make_unique<AttackController>(side,*services->controllers[side]);
    sync_players();return error.empty();
}
bool GameBattle::initialize_shared(){
    if(!fields[0].enemies||!fields[1].enemies)return fail("Both battle fields required");
    cross_effects=std::make_unique<EffectManager>(services->effects,2,800,1);attack_queue=std::make_unique<AttackQueue>(behaviors,services->attacks);
    for(i32 side=0;side<2;++side){const auto* profile=character_resources(fields[side].script.character);attack_queue->limits[side]=profile->attack_limit+profile->attack_limit_per_difficulty*world.difficulty;}
    sync_players();return error.empty();
}
void GameBattle::sync_players(){
    animations.timing=state.timing;
    for(i32 side=0;side<2;++side){auto& f=fields[side];if(!f.player)continue;auto& p=*f.player;
        f.script.player=p.motion.position;for(u32 n=0;n<2;++n)f.script.attack_levels[n]=p.attack_levels[n];
        f.enemy_player.state=p.control.player_state;f.enemy_player.focus=f.focus_aura!=nullptr;f.enemy_player.protection=p.control.protection;
        f.enemy_player.nearest_position=p.shots.target;f.enemy_player.target=f.targeted_enemy;
        if(f.capture_effect)f.enemy_player.capture={f.capture_effect->radius,f.capture_effect->angle,f.capture_effect->spread};else f.enemy_player.capture={};
    }
    services->effects.sync();services->attacks.sync();
}
bool GameBattle::update_enemies(i32 side){
    auto& f=fields[side];auto& m=*f.enemies;sync_players();m.timing=state.timing;m.frame_step=state.ecl_step;m.difficulty_mask=state.difficulty_mask;
    EnemyFrameSettings settings;settings.game_flags=state.flags;settings.timeline_blocked=state.scene_phase;settings.dialogue=state.dialogue;settings.patterns=patterns;
    if(!m.step_frame(settings))return fail("Enemy frame");
    f.player->control.player_state=f.enemy_player.state;f.player->control.protection=f.enemy_player.protection;f.player->shots.target=f.enemy_player.nearest_position;f.targeted_enemy=f.enemy_player.target;
    return error.empty();
}
bool GameBattle::update_bullets(i32 side){
    auto& f=fields[side];auto& a=*services->fields[side];sync_players();
    // The original manager checks the global pause once, before processing
    // bullets. A combo may start a spell cut-in inside that loop; its lasers
    // and end-of-frame timers still finish this tick before the pause begins.
    const u32 entry_flags=state.flags;
    if(!f.bullets->update_bullets(state.timing,f.player->motion.position,f.script.flags,entry_flags,a)||!f.lasers->update(state.timing,f.script.flags,entry_flags,a))return fail("Bullet/laser frame");
    f.bullets->finish_frame(state.timing,f.script.flags,entry_flags);return error.empty();
}
bool GameBattle::update_attacks(){services->attacks.sync();if(!attack_queue->update(state.timing,state.flags,fields[0].script.flags))return fail("Character attack frame");return error.empty();}
void GameBattle::update_player(i32 side){
    auto& f=fields[side];auto& other=fields[1-side];PlayerFrameContext c;c.timing=state.timing;c.limits=state.limits;
    std::copy_n(state.geometry,2,c.geometry);c.game_flags=state.flags;c.field_flags=f.script.flags;c.dialogue=state.dialogue>=0||state.dialogue==-2;
    c.automatic_focus=state.automatic_focus[side];c.extra_mode=rules.mode==GameMode::extra;c.hide_players=state.hide_players;c.scene_state=state.scene_phase;c.ending_frames=state.ending_frames;
    c.opponent_level=other.cpu_level;c.opponent_survival_time=other.player->cpu.survival.current;
    auto& cpu=c.computer;cpu.level=f.cpu_level;cpu.opponent_pending=other.player->combo_state.pending;cpu.spirit_count=f.enemies->spirit_alive;cpu.normal_enemies=f.enemies->alive;cpu.extra_damage=state.extra_damage;
    if(auto e=f.enemies->priority_target){cpu.has_priority=true;cpu.priority_target=e->values.position;cpu.target_flags=e->values.flags;}
    if(auto e=f.enemies->first_target){cpu.has_first=true;cpu.first_target=e->values.position;}
    DamageRules damage{rules.progress.reward_stage,state.extra_damage,rules.progress.reward_accumulator,state.rewards_blocked};services->active_damage=&damage;
    f.player->update(c,damage);services->active_damage=nullptr;
    rules.progress.reward_stage=damage.damage;rules.progress.reward_accumulator=damage.rank_charge;state.extra_damage=damage.extra_damage;sync_players();
}
bool GameBattle::begin_attack(i32 side,i32 variant,i32 level,i32 parameter,const char* name){
    if(side<0||side>1||!fields[side].attacks)return false;auto& a=*services->controllers[side];a.pull();const bool accepted=fields[side].attacks->begin(variant,level,parameter,name);a.push();return accepted;
}
void GameBattle::update_controller(i32 side){auto& a=*services->controllers[side];a.pull();fields[side].attacks->update();a.push();}
void GameBattle::update_effects(i32 side){services->effects.sync();(side==2?cross_effects.get():fields[side].effects.get())->update(state.flags,fields[0].script.flags);}
void GameBattle::flush_combo(i32 side){if(fields[side].player)services->fields[side]->combo_system().flush(fields[side].player->combo_state);}
void GameBattle::clear_hazards(i32 side){auto& a=fields[side].player->shots.areas;a.circle({0,224,0},500,0,0,30000,0,AttackAreaKind::cancel);a.circle({0,224,0},500,0,200,30000,0,AttackAreaKind::special_damage);}
void GameBattle::reset_player(i32 side,i32 health){fields[side].player->reset_round(health,world.difficulty,rules.progress.round);fields[side].focus_aura=fields[side].capture_effect=nullptr;sync_players();}
void GameBattle::reset_field(i32 side,i32 health){
    auto& f=fields[side];f.script.flags&=~1u;reset_player(side,health);
    for(auto& b:f.bullets->pool)b=Bullet{};f.bullets->pool[BulletManager::first_capacity].state=f.bullets->pool[BulletManager::update_count].state=6;f.bullets->cancel_frames=5;
    for(auto& instance:f.bullet_visuals->instances){if(f.bullet_visuals->checkpoint)f.bullet_visuals->checkpoint->BeforeBytes(&instance,sizeof(instance));std::memset(instance.data(),0,sizeof(instance));}
    std::memset(f.lasers->pool.data(),0,sizeof(f.lasers->pool));f.effects->clear();f.shield=nullptr;
    auto& m=*f.enemies;for(u32 n=0;n<EnemyManager::capacity;++n){auto& e=m.enemies[n];if(m.checkpoint)m.checkpoint->BeforeEnemy(e);for(auto& a:e.asynchronous)a.reset();e.active=&e.primary;e.active_slot=-1;e.behavior_flags&=~1u;}
    if(side==0)for(auto& pattern:patterns){pattern=u8(world.random.bounded32(m.common_program.timeline_count()));pattern|=u8(world.random.bounded32(2)<<7);}
    m.pattern_index=0;const i32 mirrored=m.timeline.mirrored;m.timeline.start(m.common_program,patterns[0]&127,mirrored);m.timeline.wait.reset(200);
}
void GameBattle::reward_enemy(i32 side,i32 reward){
    // The point-item enemy carries flag 0x2000: it is the CPU priority target
    // and uses the dedicated death sequence. Both sides receive one copy.
    auto* e=fields[side].enemies->create({19,{0,-32,0},830,-2,10000,0,0});
    if(e){e->values.item_reward=reward;e->values.flags|=0x2000;}
}
void GameBattle::draw_enemies(i32 side,bool upper){output.begin_field(side);auto& a=*services->fields[side];a.EnemyDrawServices::geometry=state.geometry[side];EnemyDraw::layers(*fields[side].enemies,upper?2:0,upper?4:2,a);}
void GameBattle::draw_player(i32 side,bool fading){output.begin_field(side);fields[side].player->draw(fading);}
void GameBattle::draw_attacks(bool cross){draw_character_attacks(*attack_queue,services->attacks,cross);}
void GameBattle::draw_effects(i32 side,i32 layer){(side==2?cross_effects.get():fields[side].effects.get())->draw(layer);}
void GameBattle::draw_bullets(i32 side){auto& f=fields[side];auto& a=*services->fields[side];a.BulletDrawServices::geometry=state.geometry[side];BulletDraw::draw(*f.bullets,*f.bullet_visuals,*f.lasers,a);}
void GameBattle::draw_controller(i32 side){services->controllers[side]->pull();fields[side].attacks->draw();}

void GameBattle::Services::Effects::sync(){
    auto& g=root.g;timing=g.state.timing;rank=g.world.rank;rewards_blocked=g.state.rewards_blocked;
    for(i32 side=0;side<2;++side){geometry[side]=g.state.geometry[side];auto& f=g.fields[side];characters[side]=f.script.character;spirit_counts[side]=f.enemies?f.enemies->spirit_alive:0;
        if(f.player){players[side]=f.player->motion.position;player_angles[side]=f.player->motion.angle;player_steps[side]=f.player->motion.step.x;}}
}
void GameBattle::Services::Effects::fire_shots(i32 side,u32 set,i32 time){auto& p=*root.g.fields[side].player;p.shots.player_position=p.motion.position;p.shots.player_scale=p.motion.base_scale;p.shots.fire(p.resource,set,time);p.motion.base_scale=p.shots.player_scale;}
void GameBattle::Services::Attacks::sync(){
    auto& g=root.g;timing=g.state.timing;rank=g.world.rank;difficulty=g.world.difficulty;
    for(i32 side=0;side<2;++side){geometry[side]=g.state.geometry[side];auto& f=g.fields[side];if(f.player){players[side]=f.player->motion.position;std::copy_n(f.player->attack_levels,2,attack_levels[side]);}}
}
EffectActor* GameBattle::Services::effect(i32 side,i32 kind,const Vec3& p,i32 slot,u32 color){
    effects.sync();effects.coordinate_side=side;auto* manager=side==2?g.cross_effects.get():g.fields[side].effects.get();if(!manager)return nullptr;
    return slot>=0?manager->slotted(EffectKind(kind),p,u32(slot),color):manager->create(EffectKind(kind),p,{},1,color);
}
TransferParameters* GameBattle::Services::transfer(i32 source,i32 kind,const Vec3& p,const Vec3& to,float delay){
    if(!g.cross_effects)return nullptr;effects.sync();effects.coordinate_side=1-source;
    auto* e=g.cross_effects->create(EffectKind(kind),p,to);if(!e)return nullptr;e->rotation=delay;return &e->transfer;
}
Bullet* GameBattle::Services::emit(i32 side,bool secondary,const BulletEmission& e){
    auto& f=g.fields[side];const auto& p=f.player->motion.position;const float dx=p.x-e.position.x,dy=p.y-e.position.y;
    const float aim=dx==0&&dy==0?1.5707963705062866f:float(std::atan2(double(dy),double(dx)));Bullet* last=nullptr;
    for(i32 layer=0;layer<e.layers;++layer)for(i32 index=0;index<e.count;++index){last=f.bullets->create(e,index,layer,aim,secondary,g.state.timing,g.world.random,p,*fields[side]);if(!last){g.fail("Bullet emission");return nullptr;}}
    if(e.flags&0x200)g.output.positioned_sound(side,e.sound,e.position.x);return last;
}
void GameBattle::Services::Field::effect(const EclEffectRequest& r){
    root.effects.sync();root.effects.coordinate_side=side;auto* e=field().effects->create(EffectKind(r.type),r.position,{},r.count,r.color);if(e&&r.explicit_velocity)e->velocity=r.velocity;
}
void GameBattle::Services::Field::begin_focus(const Vec3& p,u32,u32 character){field().focus_aura=root.effect(side,7,p,side);field().capture_effect=root.effect(side,character_resources(character)->focus_effect,p,side+2);}
void GameBattle::Services::Field::end_focus(){if(field().focus_aura&&field().focus_aura->animation)field().focus_aura->animation->pendingInterrupt=1;field().focus_aura=nullptr;if(field().capture_effect)field().capture_effect->active=0;field().capture_effect=nullptr;}
void GameBattle::Services::Field::character_meter(ComboState& state,const Vec3& p){CharacterAttackMeter::consume(state,field().script.character,game().world.rank,side,p,*this);}
i32 GameBattle::Services::Field::shot_damage(const Vec3& p,const Vec3& extent,i32& direct,i32& token,i32& special_damage){u32 id=u32(token);const i32 result=player().shots.hit(p,extent,player().control.protection,direct,&id,special_damage);token=i32(id);return result;}
bool GameBattle::Services::Field::start_animation(EclVm& e,u32 slot,bool character,i32 script){return slot<3&&root.start(character?character_animation(1-side):AnimationFile::enemies,e.animation.layers[slot],script,false);}
AreaCollisionContext GameBattle::Services::Field::area_context(){AreaCollisionContext c{game().world.random,*this};std::copy_n(game().state.geometry,2,c.geometry);c.side=side;c.difficulty=game().world.difficulty;c.combo_count=player().combo_state.kills;c.transfer_kind=side;return c;}
void GameBattle::Services::Field::set_sprite(Bullet& b,i32 sprite){
    // Eiki obtains a complete field span before visiting its bullets.
    if(!sprite_owner){game().fail("Bullet owner");return;}const auto index=u32(&b-sprite_owner->bullets->pool.data());
    if(!sprite_owner->bullet_visuals->set_sprite(b,index,sprite))game().fail("Bullet sprite");
}
void GameBattle::Services::Field::draw_strip(AnmVm& vm,const EnemyTrailVertex* vertices,u32 count){
    game().output.textured(vm,vertices,count,BattleGeometry::strip);
}
void GameBattle::Services::Controller::pull(){
    auto& g=root.g;timing=g.state.timing;game_flags=g.state.flags;reward_time=g.rules.progress.reward_accumulator;std::copy_n(g.state.geometry,2,geometry);
    for(i32 s=0;s<2;++s){auto& f=g.fields[s];if(!f.player)continue;auto& p=players[s];std::copy_n(f.player->attack_levels,2,p.levels);p.spell_count=f.spells;p.boss_count=f.bosses;p.boss_counters=f.counters;p.portrait_script=character_resources(f.script.character)->attack_portrait;}
}
void GameBattle::Services::Controller::push(){
    auto& g=root.g;g.state.flags=game_flags;g.rules.progress.reward_accumulator=reward_time;if(root.active_damage)root.active_damage->rank_charge=reward_time;
    for(i32 s=0;s<2;++s){auto& f=g.fields[s];if(!f.player)continue;const auto& p=players[s];std::copy_n(p.levels,2,f.player->attack_levels);std::copy_n(p.levels,2,f.script.attack_levels);std::copy_n(p.levels,2,g.rules.attack_levels[s]);f.spells=p.spell_count;f.bosses=p.boss_count;f.counters=p.boss_counters;}
}
void GameBattle::Services::Controller::spawn_attack_enemy(i32 side,i32 script,i32 life,i32 score){
    // ECL executes synchronously and can immediately announce this attack.
    push();root.g.sync_players();root.g.fields[side].enemies->create({script,{},life,-2,score,0,1});pull();
}
}
