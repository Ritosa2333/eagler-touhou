#include "GameBattle.hpp"
#include "Localization.hpp"
#include "GameSession.hpp"
#include <algorithm>

namespace th11 {
GameBattle::GameBattle(GameSessionResources& r,AnmManager& a,GameEconomy& e,SpellRecords& records,SceneCompositor& scene,ClearRecords& clears)
    :resources(r),animations(a),economy(e),compositor(scene),completion(e,clears,*this,&a.rate),
     hud(a,r.core.front,r.core.ascii,e,enemy_commands,completion.state,*this),ascii(r.core.ascii),enemy_animations(a),
     enemies(resources.stage->timeline,enemy_environment,enemy_commands,*this),
     enemy_callbacks(enemy_environment,enemy_commands,*this),
     bullets(resources.core.bullet,a,*this,6),lasers(resources.core.bullet,a,*this,6),
     items(resources.core.bullet,a,*this,6),item_rewards(e,*this),spells(*this,e,*this,records,&a.rate) {
    enemy_commands.animations=&enemy_animations;
    enemy_commands.bullets=&bullets;enemy_commands.lasers=&lasers;
    enemy_commands.manager=&enemies;enemy_commands.cancellation=&cancellation;
    enemy_commands.spells=&spells;
    enemy_commands.shakes=&shakes;
    enemy_commands.dialogue=this;
    enemy_commands.deformations=this;
    enemy_commands.callbacks=&enemy_callbacks;
    // Native manager +0x40 is bullet.anm; ECL ANIM entries follow it.
    enemy_animations.resources[0]={&resources.core.bullet,6};
    enemy_animations.resources[1]={&resources.core.enemy,8};
    enemy_animations.resources[2]={&resources.stage->enemies,9};
    enemy_commands.animation_user=&enemy_animations;
    enemy_commands.animation_visibility=[](u32 id,bool visible,void* p){static_cast<EnemyAnimations*>(p)->visible(id,visible);};
    enemy_environment.animation_user=&enemy_animations;
    enemy_environment.animation_sprite=[](u32 id,void* p){return static_cast<EnemyAnimations*>(p)->sprite(id);};
    enemy_environment.shared_random=&animations.script_rng;
    enemy_environment.rate=animations.rate;
    communication.reset(&animations.rate);
    callback_shared_visual.initialize();
}

GameBattle::~GameBattle(){for(auto* node=enemies.first;node;node=node->next)release_deformation(node->value->state);if(dialogue)dialogue->clear();for(auto& vm:spell_backgrounds)animations.release_geometry(vm);}
bool GameBattle::create_deformation(EnemyState& e){auto effect=std::make_unique<ScreenDeformation>(animations);if(!effect->initialize(resources.core.text))return false;e.deformation=effect.release();return true;}
bool GameBattle::initialize(i32 character,i32 subtype,i32 difficulty) {
    if(character<0||character>1||subtype<0||subtype>2||difficulty<0||difficulty>4)return unavailable("invalid battle selection");
    initialized=false;last_error=0;frame=0;error.clear();events.clear();
    completion.mode.stage=i32(resources.stage_number);completion.mode.selection=character*3+subtype;
    if(replaying)spell_timing.records=replay_entry.spell_times;
    enemy_environment.character=character;enemy_environment.subtype=subtype;
    enemy_environment.difficulty=difficulty;enemy_environment.rank=economy.rank;
    enemy_environment.player_position={0,400,0};
    // ECL world-space spawns and enemy movement read camera 0 (4c3484),
    // while background ANM reads camera 2 (4c36b4).
    camera_position=compositor.play_camera.position;
    enemy_environment.camera_delta=compositor.play_camera.animation_delta;
    economy.difficulty=difficulty;
    animations.reference_positions[0]=compositor.world_camera.position;
    animations.reference_positions[1]=compositor.world_camera.reserved24;
    animations.camera_delta=compositor.world_camera.animation_delta;
    stage=std::make_unique<Stage>(animations,resources.stage->background,resources.core.text,compositor.clear_color,u16((resources.stage_number&1)+3));
    stage->active_camera=&compositor.world_camera;
#ifdef TH_ENABLE_THPRAC
    stage->practice_game_frame=&frame;stage->practice_replay_has_stage5=practice&&practice->enabled&&replaying&&practice_replay_has_stage5;
#endif
    if(!stage->initialize(resources.stage->scene,resources.stage_number,compositor.world_camera)){last_error=-1008;error=stage->error;return false;}
    enemy_commands.stage=stage.get();
    spells.selection=character*3+subtype;spells.stage=resources.stage_number;spells.replay=replaying;
    create_dialogue();
    player_input={};player_input.movement.character=character;player_input.movement.subtype=subtype;
    player_input.movement.enemy_manager=true;
    bomb.reset();
    player=std::make_unique<PlayerFrame>(resources.core.shots[character*3+subtype],resources.core.players[character],resources.core.bullet,animations,economy,*this,7,6);
    player->input=player_input;
#ifdef TH_ENABLE_THPRAC
    player->practice=practice;
    if(practice&&practice->active&&!replaying)player->motion.state.weapon_mode=practice->run.marisa_b_formation;
#endif
    if(!player->initialize()){last_error=-601;error="player initialization";return false;}
    // On the first stage, 420390 calls 42fec0 before the first player tick.
    // Lifecycle 0 is the respawn entrance, not a new game's starting state.
    if(!player->start_stage()){last_error=-601;error="initial player stage reset";return false;}
    if(replaying&&!player->restore_replay_position(replay_entry)){error="replay player entry";return false;}
    bomb=std::make_unique<BombController>(animations,resources.core.players[character],*player,*this,7,&resources.core.text);
    bomb->selection=character*3+subtype;
    synchronize_player();
    if(!spawn_root()){last_error=-1001;error="stage main: ECL opcode "+std::to_string(enemies.script_error.opcode);return false;}
    countdown_seconds=-1;
#ifdef TH_ENABLE_THPRAC
    if(practice&&practice->active){const int section=practice->run.section;hud.practice_skip_logo=section&&(section<10000||section>=20000||section%100!=1);}
#endif
    if(!hud.start_stage(resources.stage->logo,hud_input(),demo,true,completion.mode.control_mode,hud.score.continues)){error="HUD initialization";return false;}
    initialized=true;return true;
}
void GameBattle::create_dialogue(){
    dialogue=std::make_unique<Dialogue>(animations,resources.core.text,resources.core.players[enemy_environment.character],resources.core.front,resources.stage->enemies,resources.stage->logo,*this);
    dialogue->character=enemy_environment.character;dialogue->stage=resources.stage_number;
}
bool GameBattle::stage_result_animation(){
    auto* vm=animations.create(resources.core.front,72,5,22);
    result_animation=vm?vm->id:0;return vm!=nullptr;
}
bool GameBattle::next_stage(GameResources& source,u32 number){
    if(transitioning||number<2||number>6)return false;
    // 41fe10 retains the player, bullet/item/laser pools and HUD. Stage-local
    // scripts and animations are retired before replacing their owners.
    if(!resources.load_stage(source,number)){error=resources.error;return false;}
    if(dialogue)dialogue->clear();dialogue.reset();bomb.reset();
    if(!enemies.reset(resources.stage->timeline)){last_error=-1001;return false;}
    animations.retire_resource(resources.core.enemy);
    if(auto* old=resources.previous_stage.get()){
        animations.retire_resource(old->enemies);animations.retire_resource(old->logo);
    }
    for(auto id:spell_titles)enemy_animations.erase(id);
    enemy_animations.erase(spell_circle);spell_circle=0;
    for(auto& id:spell_titles)id=0;
    for(auto& vm:spell_backgrounds){animations.release_geometry(vm);vm.initialize();}
    spell_flags=0;spell_elapsed=spell_bonus=spell_id=0;
    spell_timing={};if(replaying)spell_timing.records=replay_entry.spell_times;
    enemy_commands.stage_section=0;enemy_commands.section_frame=0;
    for(auto& value:enemy_environment.shared_integers)value=0;
    shakes.effects.clear();shakes.offset={};
    items.reset();animations.rate=enemy_environment.rate=1;
    economy.communication=0;communication.reset(&animations.rate);
    outgoing_stage=std::move(stage);
    stage=std::make_unique<Stage>(animations,resources.stage->background,resources.core.text,compositor.clear_color,u16((number&1)+3));
    stage->active_camera=&compositor.world_camera;
#ifdef TH_ENABLE_THPRAC
    stage->practice_game_frame=&frame;stage->practice_replay_has_stage5=practice&&practice->enabled&&replaying&&practice_replay_has_stage5;
#endif
    if(!stage->initialize(resources.stage->scene,number,compositor.world_camera,false)){error=stage->error;return false;}
    enemy_commands.stage=stage.get();enemy_animations.resources[2]={&resources.stage->enemies,9};
    create_dialogue();spells.stage=number;
    bomb=std::make_unique<BombController>(animations,resources.core.players[enemy_environment.character],*player,*this,7,&resources.core.text);
    bomb->selection=enemy_environment.character*3+enemy_environment.subtype;
    completion.mode.stage=i32(number);completion.state.exit=StageExit::None;
    hud.prepare_stage();countdown_seconds=-1;
    transition_timer.set(0,&animations.rate);frame=0;
    transitioning=true;transition_started=false;stage_active=false;
    return true;
}
bool GameBattle::activate_stage(){
    bullets.reset();animations.retire_resource(resources.core.bullet);
    if(!player->start_stage())return false;
    if(replaying&&!player->restore_replay_position(replay_entry))return false;
    items.reset();
    if(!enemies.reset(resources.stage->timeline))return false;
    animations.retire_resource(resources.core.enemy);animations.retire_resource(resources.stage->enemies);
    lasers.clear();enemy_commands.section_frame=0;
    if(!spawn_root())return false;
    if(!compositor.stage_start())return false;
    if(!hud.start_stage(resources.stage->logo,hud_input(),false,false,completion.mode.control_mode)){error="HUD stage activation";return false;}
    stage_active=true;events.push_back({BattleEventKind::StageMusic,i32(resources.stage_number)});
    // 42068b restarts the new stage's game-control clock at activation.
    frame=0;transition_timer.set(0,&animations.rate);
    return true;
}
bool GameBattle::stage_control(){
    if(!transitioning)return true;
    if(!completion.result_delay_elapsed())return true;
    if(!transition_started){
        enemy_animations.interrupt(result_animation,1);
        completion.state.hud_flags&=~0x200u;completion.state.result_timer.set(0,&animations.rate);
        if(!stage->start_objects())return false;
        outgoing_stage->fade_out(fades);stage->resume_background();transition_started=true;
    }else if(transition_timer.current==30&&!stage_active){
        if(!activate_stage())return false;
    }
    if(outgoing_stage&&(outgoing_stage->state.draw_flags&8)){
        if(resources.previous_stage)animations.retire_resource(resources.previous_stage->background);
        outgoing_stage.reset();
    }
    transition_timer.tick();return true;
}
void GameBattle::set_input(const GameSessionInput& input) {
    player_input.movement.held=input.held;player_input.movement.pressed=input.pressed;
    player_input.movement.enemy_manager=true;player_input.movement.enemies=enemies.count!=0;
    // Legacy name: movement.bomb represents HUD +4438 (dialogue), not the
    // bomb key or the separate special-attack controller +3c.
    player_input.movement.bomb=dialogue&&dialogue->active;
    if(player)player->input=player_input;
}
void GameBattle::synchronize_player() {
    if(!player)return;
    player_input.movement.bomb=dialogue&&dialogue->active;
    player->input.movement.bomb=player_input.movement.bomb;
    player->input.special_available=player_input.special_available=bool(bomb);
    player->input.shooting_blocked=player_input.shooting_blocked=(completion.state.hud_flags&0x10)!=0;
    const auto position=player->motion.state.position;
    enemy_environment.player_position=position;enemy_environment.rank=economy.rank;
    enemy_environment.rate=animations.rate;
    bullets.player=position;LaserWorld::player=position;LaserWorld::rate=animations.rate;
    ShotWorld::enemies=enemies.first;
    player_state=player->state.life_state;player_flags=player->motion.state.flags;
    special_active=bomb?bomb->state.active:0;
    player->input.special_active=player_input.special_active=special_active;
    // 410690 tests the player's invincibility timer and state flag 2, not
    // the Bomb controller's secondary timer (used by Nitori's shield).
    special_ending=player->state.invincibility.current!=0||(player_flags&2)!=0;
    spells.bomb_active=special_active;spells.replay=player_input.replay;
    cancellation={spell_flags,spell_id};
    player->copy_item_state(items.player);
    items.player.force_attract=player_input.movement.bomb;
}
bool GameBattle::spawn_root() {
    // 0x420495/0x4205d2 pass "main" with a zeroed 0x50-byte spawn record.
    // MainSub00 is only the first wave and must not replace this controller.
    return enemies.spawn("main",EnemySpawn{})!=nullptr;
}
bool GameBattle::update(const std::function<bool()>& sample_input) {
    if(!initialized||last_error)return false;
    events.clear();dialogue_text_requests.clear();
    // Native tick priorities: overlay ANM 8, game globals 10, stage 12, shake/fade 14,
    // player 16, Bomb 17, enemies 18, lasers 19, bullets 20, items 21, spell 22,
    // dialogue/HUD 24, normal ANM 26. ECL-created shakes first run next tick.
    if(!animations.update(true)){last_error=-1005;return false;}
    if(!stage_control()){last_error=-1009;return false;}
    const bool control_running=!transitioning||transition_started;
#ifdef TH_ENABLE_THPRAC
    practice_time_lock=practice&&practice->enabled&&!replaying&&(practice->cheats&8);
    enemy_commands.practice_time_lock=spells.practice_time_lock=practice_time_lock;
    completion.mode.all_clear_bonus=practice&&practice->enabled&&practice->all_clear_bonus;
#endif
    // 420752: the stage-six introduction holds the music cursor for 300 ticks.
    if(control_running&&!demo&&resources.stage_number==6&&frame==300)events.push_back({BattleEventKind::MusicResume});
    if(control_running)hud.score.update(economy.score_units);
    if(control_running&&frame>=90&&player&&!(dialogue&&dialogue->active))communication.update(economy,player->motion.state.position.y);
    if(control_running)enemy_commands.section_frame=wrapping_add(enemy_commands.section_frame,1);
    // Original replay/input callbacks run after game control (priority 10),
    // which selects the incoming stage's stream at activation, before players.
    if(sample_input&&!sample_input())return false;
    // Equal-priority callbacks insert newest first (456b70/456c10).
    if((!transitioning||transition_started)&&!stage->update()){last_error=-1008;error=stage->error;return false;}
    if(outgoing_stage&&!outgoing_stage->update()){last_error=-1008;error=outgoing_stage->error;return false;}
    camera_position=compositor.play_camera.position;
    enemy_environment.camera_delta=compositor.play_camera.animation_delta;
    shakes.offset=compositor.world_camera.offset;
    shakes.update(animations.script_rng);
    compositor.world_camera.offset=shakes.offset;
    fades.update();
    popups.update(animations.rate);
    synchronize_player();
    if(player_live&&player){
        player->spell={spell_flags,spell_elapsed,spell_bonus};
        if(!player->update()){if(!last_error)last_error=-601;return false;}
        spell_flags=player->spell.flags;spell_bonus=player->spell.bonus;
    }
    // Bomb activation enables its callback itself (406510); it may run during
    // the outgoing-background overlap while new enemy callbacks are disabled.
    if(bomb){
        if(!bomb->update()){if(error.empty())error="unimplemented or failed Bomb branch "+std::to_string(bomb->selection);last_error=-608;return false;}
        spell_flags=player->spell.flags;spell_bonus=player->spell.bonus;
    }
    synchronize_player();
    target_locked=false;
    if(stage_active&&!enemies.update()){last_error=-1001;if(error.empty())error="ECL opcode "+std::to_string(enemies.script_error.opcode)+" at time "+std::to_string(enemies.script_error.time);return false;}
    animations.rate=enemy_environment.rate;
    synchronize_player();
    if(!lasers.update(false,false)){last_error=-1003;return false;}
    if(stage_active&&!bullets.update(false)){last_error=-1002;return false;}
    synchronize_player();
    if(!items.update()){last_error=-1004;return false;}
    if(spell_flags&1){Vec3 boss;if(!boss_position(boss)||!spells.update(player->motion.state.position.y,boss,special_active)){last_error=-1006;if(error.empty())error="spell update";return false;}}
    completion.update();
    if(!hud.update(hud_input())){last_error=-1010;error="HUD update";return false;}
    if(dialogue&&dialogue->active){const i32 result=dialogue->update(player_input.movement.held,player_input.movement.pressed);if(result<0){last_error=-1007;if(error.empty())error=dialogue->error+" MSG opcode "+std::to_string(dialogue->last_opcode);return false;}if(result){if(enemy_commands.stage_section)enemy_commands.section_frame=0;enemy_commands.stage_section=0;}else dialogue->state.elapsed.tick();}
    if(!hud.finish_update(hud_input())){last_error=-1010;error="HUD indicator update";return false;}
    if(!animations.update(false)){last_error=-1005;return false;}
    if(control_running)++frame;
    // Both VM registries have now consumed resource retirement markers.
    if(transitioning&&stage_active&&!outgoing_stage){
        auto* old=resources.previous_stage.get();
        if(!old||(!animations.references(old->background)&&!animations.references(old->logo)&&!animations.references(old->enemies))){resources.retire_previous();transitioning=false;}
    }
    return true;
}
bool GameBattle::draw(AnmRenderer& renderer,SceneDrawKind kind) {
    switch(kind){
    case SceneDrawKind::ScorePopups:return popups.draw(renderer,resources.core.ascii,player->motion.state.position);
    case SceneDrawKind::HudInner:return hud.draw_inner(renderer,hud_input())&&hud.queue_text(ascii,hud_input());
    case SceneDrawKind::HudOuter:return hud.draw_outer(renderer,hud_input());
    case SceneDrawKind::AsciiInner:return ascii.draw(renderer,1,compositor.full_camera,compositor.play_camera);
    case SceneDrawKind::AsciiOuter:return ascii.draw(renderer,0,compositor.full_camera,compositor.play_camera);
    case SceneDrawKind::StageBackground:
        if((!transitioning||transition_started)&&stage&&!stage->draw_background(renderer,compositor.world_camera,fades))return false;
        return !outgoing_stage||outgoing_stage->draw_background(renderer,compositor.world_camera,fades);
    case SceneDrawKind::StageForeground:
        if((!transitioning||transition_started)&&stage&&!stage->draw_foreground(renderer,compositor.world_camera))return false;
        return !outgoing_stage||outgoing_stage->draw_foreground(renderer,compositor.world_camera);
    case SceneDrawKind::Spell:
        if(spell_flags&1){
            for(auto& vm:spell_backgrounds)if(renderer.draw(vm)==-2)return false;
            if(auto* title=animations.find(spell_titles[2]))return spells.queue_text(ascii,title->color);
            spell_titles[2]=0;
        }
        return true;
    case SceneDrawKind::Player:return player&&player->draw(renderer);
    case SceneDrawKind::Items:return items.draw(renderer);
    case SceneDrawKind::Lasers:return lasers.draw(renderer,false);
    case SceneDrawKind::Bullets:
        if(!stage_active)return true;
        for(u32 group=0;group<6;++group)if(!bullets.draw_group(group,renderer))return false;
        return true;
    default:return false;
    }
}
HudInput GameBattle::hud_input()const{
    HudInput input;input.player_present=bool(player);
    if(player)input.player_position=player->motion.state.position;
    input.dialogue=dialogue&&dialogue->active;input.spell=(spell_flags&1)!=0;
    input.stage=i32(resources.stage_number);input.section=enemy_commands.stage_section;
    input.seconds=countdown_seconds;input.hundredths=countdown_hundredths;
    input.practice=completion.mode.practice;input.practice_high=practice_high;input.result_animation=result_animation;
    input.spell_frames=spell_timing.last_frames;input.spell_time=spell_timing.encoded;return input;
}
bool GameBattle::project_spawn(Vec3 position,Vec3& projected){
    // 412f7c selects camera 0, then projects with identity world transform.
    auto& camera=compositor.play_camera;camera.perspective();Matrix4 world;world.identity();
    projected=GraphicsMath::project(position,camera.viewport,camera.projection,camera.view,world);return true;
}
bool GameBattle::start_bomb(){
    if(!bomb||!bomb->supported())return unavailable("unreconstructed Bomb activation");
    const bool result=bomb->start()!=-2;
#ifdef TH_ENABLE_THPRAC
    if(result&&practice)++practice->tracker_bombs;
#endif
    return result;
}
bool GameBattle::callback_spawn(const EnemyState& e,Vec3 p){EnemySpawn spawn;spawn.position=p;spawn.health=spawn.score=10;spawn.drop=-2;std::memcpy(spawn.integers,e.integers,48);return enemies.spawn("MBossCard2_at2",spawn)!=nullptr;}
bool GameBattle::callback_move_player(Vec3 p){
    if(!player)return false;auto& motion=player->motion;
    motion.state.x=truncate_int(double(p.x)*128);motion.state.y=truncate_int(double(p.y)*128);
    motion.state.position.x=float(double(motion.state.x)*.0078125);motion.state.position.y=float(double(motion.state.y)*.0078125);
    for(auto& option:motion.options)option.snap=1;
    // Later enemies in this same traversal read the live player position.
    enemy_environment.player_position=motion.state.position;bullets.player=motion.state.position;
    return true;
}
bool GameBattle::callback_indicator(i16 interrupt,bool update){if(interrupt)callback_shared_visual.pending_interrupt=interrupt;return !update||callback_shared_visual.update(animations)!=-2;}
bool GameBattle::special_damage(const Vec3& p,const Vec2& size,i32& damage){return bomb&&bomb->damage(p,size,damage);}
bool GameBattle::bomb_cancel(Vec3 p,float radius,u32 rewards,bool convert){
    const BulletCancelContext context{player->spell.flags,spell_id};
    if(convert?!bullets.convert_circle(p,radius,rewards&1,context):!bullets.cancel_circle(p,radius,rewards&1,true,context))return false;
    return lasers.cancel_circle(p,radius,rewards,true)>=0;
}
bool GameBattle::bomb_cancel_beam(Vec3 p,bool reward){
    const BulletCancelContext context{player->spell.flags,spell_id};
    if(!bullets.cancel_circle(p,48,reward,true,context)||!bullets.cancel_beam(p,32,reward,context)||lasers.cancel_circle(p,48,reward,true)<0)return false;
    p.y=float(double(p.y)-224);return lasers.cancel_rectangle(p,{32,448,0},reward)>=0;
}
bool GameBattle::cancel_shot(Vec3 position,float angle) {
    if(!player)return false;
    player->shots.locked_target=EnemyFrameWorld::target;
    // A full native 256-shot pool silently discards the emission.
    return player->shots.spawn_converted_laser(position,angle)!=-2;
}
bool GameBattle::shot_damage(EnemyState& e,i32& out) {
    if(!player)return false;
    const auto& timer=player->state.state_timer;
    return player->shots.damage(e.current.position,e.hitbox,timer.previous!=timer.current,economy,out);
}
i32 GameBattle::apply_collision(PlayerCollisionResult result) {
    if(result.trigger_hit){
        if(!player){last_error=-602;return -2;}
        player->spell={spell_flags,spell_elapsed,spell_bonus};
        if(!player->hit()){last_error=-602;return -2;}
        spell_flags=player->spell.flags;spell_bonus=player->spell.bonus;
    }
    return i32(result.kind);
}
bool GameBattle::player_collision(EnemyState& e,float radius,i32& out) {
    if(!player)return false;
    out=apply_collision(player->collision().circle({e.current.position.x,e.current.position.y},radius));return out>=0;
}
i32 GameBattle::collision(const BulletState& b) {
    if(!player)return -2;
    auto shape=player->collision();const Vec2 p{b.position.x,b.position.y};
    return apply_collision(b.flags&0x10?shape.circle(p,b.hitbox.x):shape.rectangle(p,b.hitbox));
}
i32 GameBattle::collision(Vec3 p,float angle,float width,float length) {
    if(!player)return -2;const auto half=player->state.hit_half;
    return apply_collision(player->collision().laser({p.x,p.y},{half.x,half.y},angle,width,length));
}
i32 GameBattle::warning_collision(Vec3 p,float angle,float width,float length) {
    if(!player)return -2;const auto half=player->state.hit_half;
    return i32(player->collision().laser({p.x,p.y},{half.x,half.y},angle,width,length,true).kind);
}
void GameBattle::clear_shot_targets(Enemy* enemy) { if(player)player->shots.invalidate_target(enemy);ShotWorld::enemies=enemies.first; }
bool GameBattle::boss_position(Vec3& out) { if(!enemy_commands.bosses[0])return false;out=enemy_commands.bosses[0]->current.position;return true; }
bool GameBattle::visual(Vec3 p,i32 script) { p.x=float(double(p.x)+224);p.y=float(double(p.y)+16);return animations.create(resources.core.bullet,script,6,22,false,false,&p)!=nullptr; }
bool GameBattle::colored_effect(const Vec3& position,i32 script,u32 color){Vec3 p=position;p.x=float(double(p.x)+224);p.y=float(double(p.y)+16);auto* vm=animations.create(resources.core.bullet,script,6,22,false,false,&p);if(!vm)return false;vm->color=color;return true;}
bool GameBattle::point_reward(Vec3 p) { return items.spawn(8,p,0xffffffff,-1.5707963705062866f,.6f)==0; }
bool GameBattle::sound(i32 id,float x,bool positional) {events.push_back({BattleEventKind::Sound,id,0,{x,0,0},0,positional});return true;}
bool GameBattle::popup(Vec3 p,i32 value,u32 color) {popups.add(p,value,color,&animations.rate);events.push_back({BattleEventKind::Popup,value,0,p,color});return true;}
bool GameBattle::notify(i32 value) {events.push_back({BattleEventKind::Notification,value});return hud.notice(value);}
bool GameBattle::display_lives(i32 lives,i32 fragments) {hud.display_lives(lives,fragments);events.push_back({BattleEventKind::Lives,lives,fragments});return true;}
bool GameBattle::power_changed() { return player&&player->motion.rebuild_options(resources.core.shots[enemy_environment.character*3+enemy_environment.subtype].header,economy,enemy_environment.character*3+enemy_environment.subtype); }
bool GameBattle::record_death() {
#ifdef TH_ENABLE_THPRAC
    if(practice)++practice->tracker_misses;
#endif
    events.push_back({BattleEventKind::Death});return true;}
bool GameBattle::enemy_death() {
    // 0x431307 increments manager +0x10 and resets +0x18. This is a player
    // death counter visible to ECL, not the erase-all-enemies operation.
    enemy_environment.shared_integers[0]=wrapping_add(enemy_environment.shared_integers[0],1);
    enemy_environment.shared_integers[2]=0;return true;
}
bool GameBattle::game_over(bool replay) {game_over_requested=true;events.push_back({BattleEventKind::GameOver,i32(replay)});return true;}
bool GameBattle::register_graze() { if(economy.graze<99999999)++economy.graze;return true; }
bool GameBattle::communication_reward(i32 amount) { communication.reward(economy,amount,&animations.rate);return true; }
bool GameBattle::reward_graze() {constexpr i32 amounts[]={800,500,500,500,500};return communication_reward(amounts[economy.difficulty]);}
bool GameBattle::graze_reward() {constexpr i32 amounts[]={500,400,300,200,200};return communication_reward(amounts[economy.difficulty]);}
bool GameBattle::graze(EnemyState& e) {return register_graze()&&visual(e.current.position,156)&&reward_graze()&&sound(28,e.current.position.x,true);}
bool GameBattle::cancel_bullets(const Vec3* center,float radius,bool skip_delayed) {
    cancellation={spell_flags,spell_id};
    // Player respawn passes zero for item rewards and the final argument for
    // delayed-projectile filtering (43113f/43115e). Bombs use bomb_cancel.
    return center?bullets.cancel_circle(*center,radius,false,skip_delayed,cancellation):bullets.cancel_all(skip_delayed,cancellation);
}
bool GameBattle::spell_begin_visuals(i32 id,i32 timeout,const char* name){
    cancellation={spell_flags,spell_id};
    auto create=[&](AnmResource& resource,i32 script,u16 file,u32* result=nullptr){auto* vm=animations.create(resource,script,file,22,false,false);if(result)*result=vm?vm->id:0;return vm;};
    if(!create(resources.core.ascii,1,2,&spell_titles[0])||!create(resources.core.text,74,0,&spell_titles[1])||!create(resources.core.ascii,2,2,&spell_titles[2]))return false;
    dialogue_text_requests.push_back({spell_titles[1],0xffffff,0,0,0,Localization::SpellName(u32(id),name?name:"",spells.translation_rank),true});
    events.push_back({BattleEventKind::SpellTitle,i32(spell_titles[1]),id});
    if(!sound(14,0,false))return false;
    auto* circle=create(resources.core.bullet,139,6,&spell_circle);if(!circle)return false;
    spell_circle_position(spells.circle_position);
    for(i32 script:{137,138}){AnmVm* found=nullptr;for(auto* node=&circle->child;node;node=node->next)if(node->value->script_index==script){found=node->value;break;}if(!found)return false;found->integers[2]=timeout;}
    if(!create(resources.core.bullet,149,6))return false;
    i32 bg[2]{},effect=10;
    switch(resources.stage_number){
    case 1:bg[0]=7;bg[1]=8;effect=id<2?11:10;break;
    case 2:bg[0]=7;bg[1]=8;break;
    case 3:bg[0]=8;bg[1]=9;effect=11;break;
    case 4:bg[0]=12;bg[1]=13;effect=15;break;
    case 5:bg[0]=16;bg[1]=17;effect=id>=122?19:-1;break;
    case 6:if(enemy_commands.stage_section<24){bg[0]=21;bg[1]=22;effect=23;}else{bg[0]=18;bg[1]=17;effect=20;}break;
    case 7:if(enemy_commands.stage_section<24){bg[0]=11;bg[1]=12;effect=13;}else{bg[0]=7;bg[1]=8;}break;
    default:return false;
    }
    for(u32 i=0;i<2;++i){auto& vm=spell_backgrounds[i];animations.release_geometry(vm);if(!vm.bind_script(resources.stage->enemies,bg[i],9,&animations.rate)||vm.update(animations)<0)return false;}
    return effect<0||create(resources.stage->enemies,effect,9);
}
bool GameBattle::spell_update_visuals(){for(auto& vm:spell_backgrounds)if(vm.update(animations)<0)return false;return true;}
bool GameBattle::start_dialogue(i32 id){
    if(!dialogue||!dialogue->begin(resources.stage->messages[enemy_environment.character*3+enemy_environment.subtype],id)){if(dialogue)error=dialogue->error;return false;}
    // 41cfe0 uses 40b5c0; ECL 415ade then uses the distinct 40b3a0
    // rectangle conversion. Already-cancelled bullets are excluded.
    if(!bullets.cancel_all(false,cancellation)||!lasers.cancel_all(false,false)||!enemies.erase_stage_enemies())return false;
    const i32 section=wrapping_add(id,1);if(enemy_commands.stage_section!=section)enemy_commands.section_frame=0;enemy_commands.stage_section=section;
    return bullets.cancel_rectangle(false,cancellation)&&lasers.cancel_all(false,false)&&enemies.erase_stage_enemies();
}
}
