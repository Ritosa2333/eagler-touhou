#include "EnemySystem.hpp"
#include "Presentation.hpp"
namespace th08 {
EnemySystem::EnemySystem(EclProgram& program,EclExecutor& executor,const FrameTiming& timing,Rng& random,GameGlobals& numbers,GameValues& values,GameRank& rank,GameGauge& gauge,PlayerSimulation& player,EffectSystem& effects,ItemSystem& items,BulletManagerState& bullets,AsciiManager& ascii,AsciiContext& ascii_context,AnmRenderer& renderer,EnemySystemActions& actions)
    :population(executor,program),globals(executor.game_state()),numbers(numbers),values(values),player(player),effects(effects),items(items),projectiles(bullets),ascii(ascii),ascii_context(ascii_context),actions(actions),simulation(state,input,population,program,executor,timing,random,numbers,values,rank,gauge,player.status().shots.regions,player.status().frame,*this),drawing(renderer){globals.values=&numbers;globals.ascii=&ascii.state;globals.scene_actions=this;}
void EnemySystem::read_player(){
    auto& p=player.status();globals.player=p.motion.movement.position;globals.player_state=p.life.state;globals.player_state_timer=p.life.timer;
    globals.paused=p.context.pause!=0;globals.game_flags=p.context.game_flags;globals.youkai=p.motion.form.youkai;globals.shot=p.context.character;
    input.focused=p.motion.form.focused;input.familiar={p.motion.form.transition,p.motion.gauge.idle,p.item_gauge_lock,p.context.character,u8(p.bomb.active!=0),0,u8(globals.spell_flags&1)};
    input.spell_bomb_damage=(globals.spell_flags&128)!=0;population.replay_flags|=p.context.replay_flags;
}
void EnemySystem::reset(){
    population.reset(time_item_threshold);state=EnemySimulationState{};input=EnemySimulationInput{};failed=false;
    for(auto& boss:globals.boss_slots)boss=nullptr;for(auto& signal:globals.timeline_signals)signal=-1;
    globals.stop_spawn=0;globals.gui_blocks_spawn=false;globals.stage_completion=0;globals.frame_count=0;simulation.bind();
}
void EnemySystem::publish_player(){
    if(native_scene)native_scene->animations.timing=native_scene->timing;
    auto& p=player.status();p.life.state=globals.player_state;p.life.timer=globals.player_state_timer;
    if(globals.gui)std::memcpy(&p.context.hud_flags,&globals.gui->flags,4);
    p.context.pause=globals.paused;p.context.game_flags=globals.game_flags;p.context.time_spell=u8(globals.spell_flags&1);
    p.motion.form.transition=input.familiar.form_transition;p.motion.gauge.idle=input.familiar.gauge_idle;p.item_gauge_lock=input.familiar.item_gauge_lock;
    const auto publish_target=[](PlayerSimulationState& state){
        const auto* target=static_cast<const EclVm*>(state.frame.target_reference);
        state.input.enemy_present=target!=nullptr;
        if(target){state.input.enemy=target->resolved_position;state.enemy_origin=target->position;}
    };
    publish_target(p);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // Enemy damage selects a separate target for every pilot. Their option
    // motion (including Yukari's Ran) reads the published input on the next
    // player tick, so leaving this seat-zero-only prevents guest lock-on.
    if(roster)for(u32 seat=1;seat<roster->count;++seat)if(roster->seats[seat].player){
        auto& other=roster->seats[seat].player->status();
        if(!roster->eligible(seat))other.frame.target_reference=nullptr;
        publish_target(other);
    }
#endif
    p.context.gauge=numbers.gauge;p.context.power=Scalar::truncate(numbers.power);p.context.bombs=Scalar::truncate(numbers.bombs);p.context.lives=Scalar::truncate(numbers.lives);p.context.time_orbs=numbers.time_orbs;p.context.last_spell_requirement=numbers.last_spell_requirement;
    population.replay_flags|=p.context.replay_flags;p.context.replay_flags=population.replay_flags;
}
void EnemySystem::effect(i32 kind,const Vec3& p,i32 count,u32 color){publish_player();effects.spawn(kind,p,count,color);failed|=effects.invalid;}
void EnemySystem::parameter_effect(i32 kind,const Vec3& p,const Vec3& parameters,i32 count,u32 color){publish_player();effects.spawn(kind,p,count,color,&parameters);failed|=effects.invalid;}
EffectState* EnemySystem::attached_effect(i32 kind,const Vec3& p,i32 count,u32 color,bool overlay){publish_player();auto* effect=overlay?effects.overlay(kind,p,count,color):effects.spawn(kind,p,count,color);failed|=effects.invalid;return effect;}
bool EnemySystem::clear_projectiles(i32 mode){publish_player();PlayerCollision::BarrierBatch barriers(&player.collision());failed|=!th08::cancel_projectiles(projectiles,mode,player.status().cancel_item,this);return !failed;}
void EnemySystem::clear_projectiles_near(const Vec3& p,float radius){th08::cancel_projectiles_near(projectiles,p,radius,*this);}
void EnemySystem::item(const Vec3& p,i32 kind,i32 mode){publish_player();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    items.spawn_enemy_drop(p,kind,mode);
#else
    items.spawn(p,kind,mode);
#endif
    failed|=items.invalid();}
AnmVm* EnemySystem::overlay(i32 kind,const Vec3& p,i32 count,u32 color){publish_player();auto* result=effects.overlay(kind,p,count,color);failed|=effects.invalid;return result;}
i32 EnemySystem::graze(const Vec3& p,const Vec3& size){
    publish_player();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(roster){i32 result=0;for(u32 seat=0;seat<roster->count;++seat)if(roster->eligible(seat)){
        auto& pilot=*roster->seats[seat].player;result=std::max(result,pilot.collision().graze(p,size));failed|=pilot.invalid();
    }read_collision();return result;}
#endif
    const i32 result=player.collision().graze(p,size);read_collision();return result;
}
i32 EnemySystem::hit(const Vec3& p,const Vec3& size){publish_player();const i32 result=player.collision().bullet(p,size,false);read_collision();return result;}
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
bool EnemySystem::participant(u32 seat,EnemyDamageParticipant& out){
    if(!roster||!roster->eligible(seat))return false;
    auto& state=roster->seats[seat].player->status();
    out={state.motion.movement.position,&state.frame,roster->seats[seat].gauge,state.context.character,u8(state.bomb.active!=0),state.motion.form.focused,state.motion.form.youkai};
    return true;
}
i32 EnemySystem::participant_damage(u32 seat,const Vec3& p,const Vec3& size,i32& count,i32& bomb){
    if(!roster||!roster->eligible(seat))return 0;
    auto& pilot=*roster->seats[seat].player;
    const i32 result=pilot.damage(p,size,count,&bomb);
    failed|=pilot.invalid();
    return result;
}
i32 EnemySystem::hit(const Vec3& p,const Vec3& size,bool familiar){
    if(!roster)return hit(p,size);
    i32 result=0;
    for(u32 seat=0;seat<roster->count;++seat){
        if(!roster->eligible(seat))continue;
        auto& pilot=*roster->seats[seat].player;
        const u8 character=pilot.status().context.character;
        if(familiar&&(character==0||character==4))continue;
        result=std::max(result,pilot.collision().bullet(p,size,false));
        failed|=pilot.invalid();
    }
    // EnemySystem::update publishes its cached seat-zero life state after the
    // enemy pass. Refresh that cache after contact, as the single-player path
    // does, or a lethal contact only plays the hit animation for P1.
    read_collision();
    return result;
}
#endif
i32 EnemySystem::damage(const Vec3& p,const Vec3& size,i32& count,i32& bomb){publish_player();const i32 result=player.damage(p,size,count,&bomb);read_collision();failed|=player.invalid();return result;}
i32 EnemySystem::barrier(BulletState& b){
    publish_player();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(roster){for(u32 seat=0;seat<roster->count;++seat)if(roster->eligible(seat)){
        auto& pilot=*roster->seats[seat].player;
        const i32 result=pilot.collision().barrier({b.position.x,b.position.y});
        if(result){read_collision();return result;}
    }read_collision();return 0;}
#endif
    const i32 result=player.collision().barrier({b.position.x,b.position.y});read_collision();return result;
}
bool EnemySystem::cancel_projectiles(i32 maximum,bool reward,i32& score){publish_player();PlayerCollision::BarrierBatch barriers(&player.collision());const bool result=cancel_projectiles_for_score(projectiles,maximum,reward,player.status().cancel_item,*this,score);failed|=!result;return result&&!failed;}
EnemySpawnResult EnemySystem::spawn(const TimelineSpawn& request){read_player();population.initial_time_items=time_item_threshold;auto* enemy=population.spawn(request);publish_player();failed|=enemy->invalid;return {enemy,population.spawn_failed};}
JobResult EnemySystem::update(){
    if(failed)return JobResult::Error;drawing.snapshot(state.layers);read_player();population.initial_time_items=time_item_threshold;
    const auto result=simulation.update();publish_player();return failed?JobResult::Error:result;
}
bool EnemySystem::draw(i32 first,i32 last){const bool ok=drawing.draw(state.layers,first,last,ascii_context.arcade_origin);if(!presentation::render_only)failed|=!ok;return presentation::render_only?true:!failed;}
void EnemySystem::screen(i32 type,i32 duration,i32 a,i32 b,i32 c,i32 priority){
    if(!native_scene){failed=true;return;}publish_player();native_scene->screen.context.timing=native_scene->timing;native_scene->screen.create(ScreenEffectType(type),duration,a,b,c,priority);
}
bool EnemySystem::spell_background(i32 variant,const Vec3& p){
    if(!native_scene){failed=true;return false;}publish_player();if(variant<0)native_scene->spell_background.end();else failed|=!native_scene->spell_background.begin(u32(variant),p);return !failed;
}
void EnemySystem::background_interrupt(i16 interrupt){if(!native_scene){failed=true;return;}for(i32 i=0;i<2;i++)native_scene->background.spell_vms[i].pendingInterrupt=interrupt;}
void EnemySystem::tint(u32 color){
    if(!native_scene){failed=true;return;}auto& current=native_scene->background.tint_color;
    if(!current.a){current.d3dColor=i32(color);return;}u32 blended=0;for(u32 shift=0;shift<32;shift+=8)blended|=((((color>>shift)&255)+((u32(current.d3dColor)>>shift)&255))/2)<<shift;current.d3dColor=i32(blended);
}
void EnemySystem::laser(const Vec2& center,const Vec2& size,const Vec3& origin,float angle,bool graze){
    publish_player();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(roster){for(u32 seat=0;seat<roster->count;++seat)if(roster->eligible(seat)){
        auto& pilot=*roster->seats[seat].player;pilot.collision().laser(center,size,origin,angle,graze);failed|=pilot.invalid();
    }read_collision();return;}
#endif
    player.collision().laser(center,size,origin,angle,graze);read_collision();failed|=player.invalid();
}
bool EnemySystem::spell_announcement(i32 portrait,const char* name,i32 style){
    if(!native_scene){failed=true;return false;}publish_player();auto& p=native_scene->spell_presentation;p.context.game_flags=globals.game_flags;p.context.current_spell=globals.current_spell;p.context.hud_redraw=native_scene->hud_redraw;
    failed|=!p.enemy(portrait,name,style);native_scene->hud_redraw=p.context.hud_redraw;return !failed;
}
}
