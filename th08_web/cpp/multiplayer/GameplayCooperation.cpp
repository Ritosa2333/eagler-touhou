#include "../game/GameplayScene.hpp"
#include "../game/InputController.hpp"
#include "CooperativeResources.hpp"

namespace th08 {
void GameplayScene::enter_spirit(u32 seat){
    if(seat>=session.player_count||cooperation.seats[seat].spirit)return;
    auto& simulation=pilot(seat);
    const i8 dx=session.random.next16()&1?1:-1,dy=session.random.next16()&1?1:-1;
    if(!multiplayer::enter_spirit(cooperation,u8(seat),dx,dy))return;
    simulation.enter_spirit();
    roster.seats[seat].available=false;items.set_player_available(seat,false);
    // Spirit stock is not playable. Rescue and next-stage return initialize
    // their respective Bomb stock when the player becomes playable again.
    session.pilot_values[seat].set_bombs(Scalar::truncate(simulation.profile(false).initial_bombs));
    pilot_services(seat).sync_values();
}
void GameplayScene::update_cooperation(){
    if(paused||retrying||time_stopped||globals.paused||(globals.game_flags&0x460))return;
    multiplayer::CooperativeFrameInput input{};
    for(u32 seat=0;seat<session.player_count;++seat){
        const auto& pilot_state=pilot(seat).status();const auto& position=pilot_state.motion.movement.position;
        auto& lane=input.seats[seat];
        lane.x=Scalar::truncate(Scalar::mul(position.x,100.f));
        lane.y=Scalar::truncate(Scalar::mul(position.y,100.f));
        lane.lives=Scalar::truncate(session.pilot_resources[seat].lives);
        lane.power=Scalar::truncate(session.pilot_resources[seat].power);
        lane.available=roster.eligible(seat);
        const bool live=pilot_state.life.state==0||pilot_state.life.state==3;
        lane.can_give=lane.available&&live&&!pilot_state.input.gui_blocked;
        lane.can_receive=lane.available&&live;
        lane.focus=(pilot_state.input.buttons&InputButton::Focus)!=0;
        lane.shoot=(pilot_state.input.buttons&InputButton::Shoot)!=0;
        lane.shoot_pressed=lane.shoot&&(pilot_state.bomb_input.previous_buttons&InputButton::Shoot)==0;
    }
    const auto allocate=[](void* raw,std::uint8_t giver,std::uint8_t target) noexcept {
        auto& scene=*static_cast<GameplayScene*>(raw);
        return scene.items.spawn_for_player(scene.pilot(giver).status().motion.movement.position,5,1,target);
    };
    const auto allocate_power=[](void* raw,std::uint8_t giver,std::uint8_t target) noexcept {
        auto& scene=*static_cast<GameplayScene*>(raw);
        return scene.items.spawn_power_gift(scene.pilot(giver).status().motion.movement.position,target);
    };
    const auto result=multiplayer::advance(cooperation,input,allocate,allocate_power,this);
    for(u32 i=0;i<result.count;++i){
        const auto& event=result.events[i];
        if(event.kind==multiplayer::CooperativeEventKind::Retry){
            // Multiplayer has no Continue flow.  The cooperation policy still
            // emits one terminal wipe event after 180 ticks, but presentation
            // skips the native retry menu and goes straight to Game Results.
            globals.stage_completion=0;menus.context.show_retry=0;retrying=false;
            menus.context.supervisor_state=6;
            continue;
        }
        const u32 giver=event.giver;
        if(event.kind==multiplayer::CooperativeEventKind::PowerItems){
            if(!session.pilot_values[giver].add_power(-20)){failed=true;return;}
            pilot(giver).status().context.hud_flags|=0x20u;
            pilot_services(giver).sync_values();
            continue;
        }
        if(!session.pilot_values[giver].add_lives(-1)){failed=true;return;}
        auto& donor=pilot(giver).status();donor.context.hud_flags=(donor.context.hud_flags&~3u)|2;
        pilot_services(giver).sync_values();
        if(event.kind==multiplayer::CooperativeEventKind::Revive){
            const u32 target=u32(event.target);
            auto& recipient=pilot(target);
            session.pilot_values[target].set_bombs(0);
            session.pilot_values[target].set_power(64);
            if(Scalar::truncate(session.pilot_resources[target].lives)<8)session.pilot_values[target].add_lives(1);
            recipient.revive_spirit();
            roster.seats[target].available=true;items.set_player_available(target,true);
            pilot_services(target).sync_values();
            recipient.status().context.hud_flags=(recipient.status().context.hud_flags&~0x3fu)|0x2au;
        }
    }
}
void GameplayScene::reset_team_after_continue(){
    multiplayer::reset(cooperation,u8(session.player_count));
    for(u32 seat=0;seat<session.player_count;++seat){
        multiplayer::begin_base_life(session.pilot_resources[seat],float(session.config.lives),0);
        auto& simulation=pilot(seat);
        if(simulation.status().life.state==4)simulation.revive_spirit();
        simulation.place_multiplayer_spawn(seat,session.player_count);
        roster.seats[seat].available=true;items.set_player_available(seat,true);
        pilot_services(seat).sync_values();
        simulation.status().context.hud_flags=(simulation.status().context.hud_flags&~0x3fu)|0x2au;
    }
}
}
