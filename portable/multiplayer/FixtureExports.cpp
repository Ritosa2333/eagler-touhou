// Diagnostic-only object: portable/build.mjs adds it solely for
// --multiplayer-fixtures. Every fixture changes an explicit initial condition
// at a fully confirmed boundary, then the real game tick/input/rollback code
// performs the behavior under test. No custom simulation or golden update.
#include "../../th10_web/cpp/platform/Application.hpp"
#include "../../th10_web/cpp/game/PlayerFrame.hpp"

#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Multiplayer fixtures cannot enter an ordinary build
#endif

using namespace th10;
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
    if(kind<1||kind>7)return 0;
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
    static i32 words[18]{};std::fill(words,words+18,0);
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
    return words;
}
