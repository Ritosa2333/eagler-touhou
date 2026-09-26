// Diagnostic-only object: portable/build.mjs adds it solely for
// --multiplayer-fixtures. Every fixture changes an explicit initial condition
// at a fully confirmed boundary, then the real game tick/input/rollback code
// performs the behavior under test. No custom simulation or golden update.
#include "../../th10_web/cpp/platform/Application.hpp"
#include "../../th10_web/cpp/game/PlayerFrame.hpp"
#include "../../th10_web/cpp/game/Dialogue.hpp"

#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Multiplayer fixtures cannot enter an ordinary build
#endif

using namespace th10;
extern "C" browser::FileSystem* files_create();
extern "C" void files_destroy(browser::FileSystem*);
extern "C" int sdl_replay_seek_batch(browser::Application*);
extern "C" void sdl_audio_replay_seek_output(u32);
extern "C" u32 sdl_audio_replay_seek_tick();
extern "C" __attribute__((export_name("mp_fixture_audio_seek_output")))
void mp_fixture_audio_seek_output(u32 seeking){sdl_audio_replay_seek_output(seeking);}
extern "C" __attribute__((export_name("mp_fixture_audio_seek_tick")))
u32 mp_fixture_audio_seek_tick(){return sdl_audio_replay_seek_tick();}
extern "C" __attribute__((export_name("mp_fixture_replay_seek_batch")))
i32 mp_fixture_replay_seek_batch(browser::Application* app){
    // Exercise the exact browser seek operation, not an alternate simulator.
    return sdl_replay_seek_batch(app);
}

// Valid pre-existing local score files, emitted by the native codec before
// the application is created. This is not a Replay or gameplay-state injector.
extern "C" __attribute__((export_name("mp_fixture_score_history")))
u32 mp_fixture_score_history(u32 profile){
    if(profile<1||profile>2)return 0;
    auto* files=files_create();if(!files)return 0;
    bool success=false;
    {
        Rng random{};random.seed=u16(profile);
        browser::Scores records(*files,random,false);
        if(records.data){
            auto& data=*records.data;
            for(auto& character:data.characters){
                for(auto& difficulty:character.high_scores)difficulty[0].score=i32(profile)*5000000;
                const i32 plays=i32(profile)*20;std::memcpy(character.statistics,&plays,4);
                character.spells[0].attempts=i32(profile)*7;
            }
            std::memcpy(data.settings.last_name,profile==1?"ALICE   ":"BOB     ",9);
            data.settings.statistics[profile]=1;
            success=records.save()==0;
        }
    }
    files_destroy(files);return success?1:0;
}
extern "C" __attribute__((export_name("mp_fixture_save_replay")))
u32 mp_fixture_save_replay(browser::Application* app){
    if(!app||!app->multiplayer_active()||app->multiplayer_frame_open||
       app->state.netplay_runtime.HasRollbackRequest())return 0;
    const auto& runtime=app->state.netplay_runtime;
    if(runtime.LastSimulatedFrame()==Netplay::INVALID_FRAME||
       runtime.ConfirmedThroughAllRemotes()==Netplay::INVALID_FRAME||
       runtime.ConfirmedThroughAllRemotes()<runtime.LastSimulatedFrame())return 0;
    app->world->save_replay("th10_01.rpy","NATIVE");
    return app->world->error||app->error?0:1;
}

extern "C" __attribute__((export_name("mp_fixture_replay_probe")))
const float* mp_fixture_replay_probe(browser::Application* app){
    // Read-only observations for a keyboard-driving test robot. It receives
    // positions just as a human sees them; it cannot alter lives, invulnerability,
    // difficulty, ECL, collisions, stage completion or the Replay archive.
    // Layout: 16 header floats; 3*12 pilot floats; then 7 per bullet,
    // 8 per laser and 4 per regular item. Native pool limits bound the buffer.
    static float out[16+3*12+2001*7+512*8+150*4]{};
    std::fill(out,out+52,0);
    if(!app||!app->world)return out;
    const auto& world=*app->world;
    out[0]=1;out[1]=float(world.player_count);out[4]=float(world.state.game.stage);
    out[5]=float(world.state.game.stage_frames);out[13]=float(world.state.netplay_runtime.NextFrame());
    out[14]=float(world.state.netplay_runtime.Generation());out[15]=float(world.state.multiplayer_replay.Cursor());
    if(const auto* r=world.actors.results){out[6]=float(r->state);out[7]=float(r->menu.selected);
        out[8]=float(r->keyboard.selected);out[9]=float(r->name_length);out[10]=float(r->elapsed.current);out[11]=float(r->cleared);}
    for(u32 seat=0;seat<world.player_count;++seat){
        const auto& pilot=world.pilots[seat];if(!pilot.player)continue;
        const auto& p=*pilot.player;auto* dst=out+16+seat*12;
        dst[0]=p.position.x;dst[1]=p.position.y;dst[2]=float(p.state);
        dst[3]=float(pilot.game.lives);dst[4]=float(pilot.game.power);
        dst[5]=float(p.slow_speed)/100.f;dst[6]=p.hitbox_half_size.x;
        dst[7]=float(p.invulnerability.current);dst[8]=float(p.state_timer.current);
        dst[9]=float(p.fast_speed)/100.f;dst[10]=p.hitbox_half_size.y;
        dst[11]=float(world.cooperation.seats[seat].rescueTicks);
    }
    std::size_t at=52;u32 bullets=0,lasers=0,items=0;
    if(world.actors.bullets)for(const auto& b:world.actors.bullets->pool){
        if(!b.state)continue;
        out[at++]=b.motion.position.x;out[at++]=b.motion.position.y;
        out[at++]=b.motion.velocity.x;out[at++]=b.motion.velocity.y;
        out[at++]=b.cancel_size;out[at++]=b.hitbox_height;out[at++]=float(b.state);++bullets;
    }
    if(world.actors.lasers)for(const auto* laser=world.actors.lasers->sentinel.next;laser&&lasers<512;laser=laser->next){
        out[at++]=laser->position.x;out[at++]=laser->position.y;out[at++]=laser->angle;
        out[at++]=laser->length;out[at++]=laser->width;out[at++]=float(laser->state);
        out[at++]=laser->velocity.x;out[at++]=laser->velocity.y;++lasers;
    }
    if(world.actors.items)for(const auto& item:world.actors.items->regular){
        if(!item.state)continue;
        out[at++]=item.position.x;out[at++]=item.position.y;out[at++]=float(item.kind);
        out[at++]=float(item.state);++items;
    }
    out[2]=float(bullets);out[3]=float(lasers);out[12]=float(items);return out;
}
// Separate from the projectile probe so existing rollback observations retain
// their schema. The robot may read a visible dialogue/Boss, but only ordinary
// recorded keyboard input is allowed to advance either native owner.
extern "C" __attribute__((export_name("mp_fixture_replay_controls")))
const float* mp_fixture_replay_controls(browser::Application* app){
    static float out[16]{};std::fill(out,out+16,0);out[0]=1;
    if(!app||!app->world)return out;
    const auto& world=*app->world;
    if(world.actors.gui&&world.actors.gui->dialogue){
        const auto& dialogue=*world.actors.gui->dialogue;
        out[1]=1;out[2]=float(dialogue.script_time.current);
        out[3]=float(dialogue.wait.current);
        if(dialogue.instruction)out[4]=float(dialogue.instruction->opcode);
    }
    if(world.actors.enemies){
        out[11]=float(world.actors.enemies->count);
        for(const auto* boss:world.actors.enemies->bosses){
            if(!boss)continue;
            out[5]=1;out[6]=boss->state.current.position.x;
            out[7]=boss->state.current.position.y;
            out[8]=float(boss->state.health);out[9]=float(boss->state.maximum_health);
            out[10]=float(boss->state.lifetime.current);break;
        }
    }
    if(world.actors.session)out[12]=float(world.actors.session->session_flags);
    return out;
}
namespace {
void timer(Timer& value,u32& flags,i32 ticks,float& rate){
    value.rate=&rate;flags|=1;value.initialize(ticks);
}
void pose(browser::World& world,u32 seat,i32 x,i32 y,i32 state,i32 lives,i32 power){
    auto& pilot=world.pilots[seat];auto& player=*pilot.player;
    player.position={float(x),float(y),0};player.fixed_position={x*100,y*100};
    player.death_position=player.position;player.input_velocity={};player.velocity={};
    player.collision_bounds.minimum={float(x)-player.hitbox_half_size.x,float(y)-player.hitbox_half_size.y,0};
    player.collision_bounds.maximum={float(x)+player.hitbox_half_size.x,float(y)+player.hitbox_half_size.y,0};
    for(auto& point:player.position_history)point=player.fixed_position;
    player.state=state;pilot.game.lives=lives;pilot.game.power=std::int16_t(power);
    timer(player.state_timer,player.state_timer_flags,60,world.engine.speed);
    timer(player.invulnerability,player.invulnerability_flags,10000,world.engine.speed);
    world.configure_player(pilot);
    multiplayer::ReportNativeSeatOutcome(world.cooperation,u8(seat),
        state==3?multiplayer::LifeState::Spirit:multiplayer::LifeState::Alive,
        std::int16_t(lives),std::int16_t(power));
}
}

extern "C" __attribute__((export_name("mp_fixture_prepare")))
u32 mp_fixture_prepare(browser::Application* app,u32 kind){
    if(!app||!app->multiplayer_active()||app->multiplayer_frame_open||
       app->state.netplay_runtime.HasRollbackRequest()||
       app->value.screen!=app->value.pending_screen)return 0;
    auto& runtime=app->state.netplay_runtime;
    if(runtime.LastSimulatedFrame()==Netplay::INVALID_FRAME||
       runtime.ConfirmedThroughAllRemotes()<runtime.LastSimulatedFrame()||
       runtime.ConfirmedThroughAllRemotes()==Netplay::INVALID_FRAME)return 0;
    if(kind<1||kind>13)return 0;
    auto& world=*app->world;
    if(world.player_count!=2||!world.pilots[0].player||!world.pilots[1].player)return 0;
    if(kind==6&&(world.state.game.stage<1||world.state.game.stage>=6||!world.hud))return 0;
    // Retired confirmed history must not restore across a fixture setup.
    world.rollback.Clear();
    if(kind==7){
        // Real native projectile initialization and updates, not cosmetic dots.
        // Slow grid keeps the workload on screen during the measured window.
        for(u32 i=0;i<1200;++i){
            BulletEmitter emitter{};emitter.initialize();
            emitter.position={float(i%40)*8.f-156.f,float(i/40)*6.f+30.f,0};
            emitter.count=emitter.layers=1;emitter.pattern=2;emitter.sprite_type=std::int16_t(i%8);
            emitter.color=std::int16_t(i%8);emitter.angle=1.5707963705f;
            emitter.speed_start=emitter.speed_end=0.0625f;emitter.shoot_sound=-1;
            world.fire(emitter);
        }
        return world.error?0:1;
    }
    if(kind==6){
        browser::HudProgress progression(*world.hud);
        complete_stage(progression);
        return world.state.pending_screen==11?1:0;
    }
    if(kind==1){
        pose(world,0,100,400,1,2,80);pose(world,1,-100,400,1,0,80);
        auto& player=*world.pilots[1].player;
        timer(player.invulnerability,player.invulnerability_flags,0,world.engine.speed);
        // Native collision enters the native eight-tick deathbomb window.
        return world.collide_player(player.position,{4,4})==1?1:0;
    }
    if(kind==2){
        pose(world,0,0,400,1,3,80);pose(world,1,10,400,3,-1,0);
        world.cooperation.wipeTicks=0;world.cooperation.retryPending=false;
        return 1;
    }
    if(kind==3){
        pose(world,0,-15,400,1,2,0);pose(world,1,15,400,1,2,0);
        world.spawn_item({0,400,0},1,0xffffffff,0,0);
        return world.actors.items->regular[0].state?1:0;
    }
    if(kind==8){
        // Power-transfer integration fixture: P2 is the donor so the browser
        // test can drive it through the normal remote input lane.  The players
        // start inside the real twenty-pixel cooperation radius; no item or
        // cooperation event is fabricated here.
        pose(world,0,0,400,1,2,0);pose(world,1,10,400,1,2,40);
        return 1;
    }
    if(kind==9){
        // No-Bomb fixture: 0.95 Power is below TH10's native 1.00 Bomb cost.
        // Keep P2 alive so a normal Bomb edge can prove it is ignored without
        // any cooperation-layer substitute or resource debit.
        pose(world,0,0,400,1,2,80);pose(world,1,10,400,1,0,19);
        return 1;
    }
    if(kind==10){
        // Same resource boundary while inside the native eight-tick deathbomb
        // window.  The Bomb edge must not rescue the player; native death then
        // owns the ordinary Power loss/drop before multiplayer enters Spirit.
        pose(world,0,0,400,1,2,80);pose(world,1,10,400,1,0,19);
        auto& player=*world.pilots[1].player;
        timer(player.invulnerability,player.invulnerability_flags,0,world.engine.speed);
        return world.collide_player(player.position,{4,4})==1?1:0;
    }
    if(kind==11){
        // P1 final-death fixture matching the live report: one remaining stock,
        // then an intentional collision. Keep P2 well away so the collision
        // deterministically belongs to P1 on every endpoint.
        pose(world,0,0,400,1,0,80);pose(world,1,-100,400,1,2,80);
        auto& player=*world.pilots[0].player;
        timer(player.invulnerability,player.invulnerability_flags,0,world.engine.speed);
        return world.collide_player(player.position,{4,4})==1?1:0;
    }
    if(kind==12){
        // Rescue setup for the reported sequence: P1 is already a Spirit and
        // P2 has a spare life while standing inside the cooperation radius.
        pose(world,0,0,400,3,-1,0);pose(world,1,10,400,1,2,80);
        world.cooperation.wipeTicks=0;world.cooperation.retryPending=false;
        return 1;
    }
    if(kind==13){
        // Preserve the just-rescued P1 state and only remove spawn protection,
        // then collide that exact native Player. This catches lifecycle bugs in
        // revive_player() instead of replacing the revived player with a pose.
        auto& player=*world.pilots[0].player;
        if(player.state!=1||world.pilots[0].game.lives<0)return 0;
        timer(player.invulnerability,player.invulnerability_flags,0,world.engine.speed);
        return world.collide_player(player.position,{4,4})==1?1:0;
    }
    if(kind==4){
        StraightLaserParameters parameters{};
        parameters.position={100,150,0};parameters.angle=1.5707963705f;
        parameters.target_length=parameters.initial_length=80;
        parameters.maximum_distance=200;parameters.width=8;
        return world.create_laser(0,&parameters)>0?1:0;
    }
    pose(world,0,-30,400,3,-1,0);pose(world,1,30,400,3,-1,0);
    world.cooperation.wipeTicks=175;world.cooperation.retryPending=false;
    return 1;
}

extern "C" __attribute__((export_name("mp_fixture_status")))
const i32* mp_fixture_status(browser::Application* app){
    static i32 words[19]{};std::fill(words,words+19,0);
    if(!app||!app->world)return words;auto& world=*app->world;
    words[0]=1;words[1]=world.cooperation.wipeTicks;
    words[2]=world.cooperation.retryPending;
    for(u32 seat=0;seat<2;++seat){
        auto& value=world.cooperation.seats[seat];
        words[3+seat*4]=i32(value.lifeState);words[4+seat*4]=value.rescueTicks;
        words[5+seat*4]=value.waitingForFocusRelease;
        words[6+seat*4]=world.pilots[seat].player->state_timer.current;
    }
    words[11]=world.actors.lasers?world.actors.lasers->count:0;
    words[12]=world.actors.items?world.actors.items->active_count:0;
    if(world.actors.gui)words[13]=world.actors.gui->dialogue?1:0;
    words[14]=world.backgrounds.current?world.backgrounds.current->stage_number:0;
    words[15]=world.backgrounds.previous?world.backgrounds.previous->stage_number:0;
    words[16]=world.backgrounds.previous?world.backgrounds.previous->fade_timer.current:-1;
    for(const auto& entry:world.backgrounds.retired)words[17]+=entry.stage?1:0;
    words[18]=world.state.multiplayer_cheat_movement_used?1:0;
    return words;
}
