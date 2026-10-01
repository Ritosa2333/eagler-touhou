// Product acceptance setup/observation only; never linked by a normal Runtime.
#pragma once
#include "../../th08_web/cpp/platform/BrowserRuntime.hpp"
#include "../../th08_web/cpp/platform/PlatformDevices.hpp"
#include <algorithm>
#include <memory>
#include <cstdio>

namespace th08::multiplayer::fixture {
inline bool product_presentation_clock(BrowserRuntime& r){
    struct Clock:FrameClock {
        u32 logic=0,wall=0;
        u32 milliseconds()override{return logic;}
        u64 performance_counter()override{return u64(logic)*1000;}
        u32 presentation_milliseconds()override{return wall;}
    } clock;
    FrameStatistics stats(r.app.ascii,clock);
    stats.state.replay_fps=60;stats.state.total=120;stats.state.rendered=120;
    // A 240 Hz display keeps presenting during five seconds of input wait.
    // Then gameplay resumes and rewinds while presentation stays at 240 Hz.
    for(u32 frame=0;frame<=1680;++frame){
        clock.wall=frame*1000/240;
        clock.logic=frame<=1200?0:frame<=1440?(frame-1200)*1000/240:500;
        stats.presentation_frame();
        if(frame&&frame%120==0){
            float fps=0;
            if(std::sscanf(stats.state.fps_text,"%ffps",&fps)!=1||fps<238||fps>244)return false;
        }
    }
    const auto wall=file_device().milliseconds();
    return r.presentation_milliseconds()-wall<100&&stats.state.replay_fps==60&&
        stats.state.total==120&&stats.state.rendered==120&&stats.state.frames==0;
}
inline const float* product_hud(BrowserRuntime& r){
    static float out[160]{};std::fill(out,out+160,0.f);
    if(!r.app.in_game()||!r.app.game.ready())return out;
    auto& g=r.app.game;const auto& s=r.app.session;
    out[0]=1;out[1]=float(s.local_player);out[2]=float(s.player_count);
    out[3]=float(g.ascii.displayed_gauge(g.ascii_context));
    out[4]=float(g.ascii_context.gauge);out[5]=float(s.numbers.gauge);
    for(u32 seat=0;seat<s.player_count;++seat){
        const auto& c=g.cooperation.seats[seat];const auto& p=s.pilot_resources[seat];
        auto* lane=out+8+seat*12;
        lane[0]=float(p.gauge);lane[1]=p.lives;lane[2]=p.bombs;lane[3]=p.power;
        lane[4]=float(c.spirit);lane[5]=float(c.progress);lane[6]=float(c.target);
        lane[7]=float(c.power_taps);lane[8]=float(c.power_window);
        lane[9]=float(g.roster.eligible(seat));lane[10]=float(g.pilot(seat).status().life.state);
    }
    for(u32 i=0;i<16;++i){
        const auto& v=g.display.front[i];auto* lane=out+48+i*7;
        lane[0]=v.pos.x;lane[1]=v.pos.y;lane[2]=v.scale.x;lane[3]=v.scale.y;
        lane[4]=v.spriteSize.x;lane[5]=v.spriteSize.y;lane[6]=float(v.activeSpriteIndex);
    }
    return out;
}
inline bool product_gauge(BrowserRuntime& r,u32 viewer){
    auto& g=r.app.game;auto& s=r.app.session;
    if(!r.app.in_game()||!g.ready()||s.netplay.Configured()||viewer>=s.player_count)return false;
    s.numbers.gauge=4321; // obsolete shared field must never win the binding
    for(u32 seat=0;seat<s.player_count;++seat)s.pilot_resources[seat].gauge=seat==0?-9000:seat==1?7500:2500;
    // Swap only the view binding; even the local authoritative seat stays put.
    g.ascii.bind_multiplayer_gauge(s.pilot_resources[viewer].gauge,
        viewer?s.guest_thresholds[viewer-1]:s.thresholds);
    return true;
}
inline bool product_gauge_draw_pure(BrowserRuntime& r){
    auto& g=r.app.game;if(!r.app.in_game()||!g.ready())return false;
    const auto before=std::make_unique<AsciiState>(g.ascii.state);
    const auto random=r.app.session.random;
    g.ascii.draw_overlays(g.ascii_context);
    const auto& after=g.ascii.state;
    return !std::memcmp(&before->gauge,&after.gauge,sizeof(AnmVm))&&
        !std::memcmp(&before->cursor,&after.cursor,sizeof(AnmVm))&&
        !std::memcmp(&before->percentage,&after.percentage,sizeof(AnmVm))&&
        !std::memcmp(&before->human_icon,&after.human_icon,sizeof(AnmVm))&&
        !std::memcmp(&before->youkai_icon,&after.youkai_icon,sizeof(AnmVm))&&
        !std::memcmp(&random,&r.app.session.random,sizeof(random));
}
inline bool product_guest_target(BrowserRuntime& r){
    if(!r.app.in_game()||!r.app.game.ready()||r.app.session.player_count<2||
       r.app.session.netplay.Configured())return false;
    auto& g=r.app.game;
    auto& host=g.pilot(0).status();auto& guest=g.pilot(1).status();
    const auto host_reference=host.frame.target_reference;
    const auto guest_reference=guest.frame.target_reference;
    const auto host_present=host.input.enemy_present;
    const auto guest_present=guest.input.enemy_present;
    const auto guest_enemy=guest.input.enemy;
    const auto guest_origin=guest.enemy_origin;
    EclVm enemy{};enemy.position={112,176,0};enemy.resolved_position={120,184,0};
    host.frame.target_reference=nullptr;
    guest.frame.target_reference=&enemy;
    g.enemies.fixture_publish_player();
    const bool published=!host.input.enemy_present&&guest.input.enemy_present&&
        guest.input.enemy.x==120&&guest.input.enemy.y==184&&
        guest.enemy_origin.x==112&&guest.enemy_origin.y==176;
    guest.frame.target_reference=nullptr;
    g.enemies.fixture_publish_player();
    const bool cleared=!guest.input.enemy_present;
    host.frame.target_reference=host_reference;guest.frame.target_reference=guest_reference;
    host.input.enemy_present=host_present;guest.input.enemy_present=guest_present;
    guest.input.enemy=guest_enemy;guest.enemy_origin=guest_origin;
    return published&&cleared;
}
}
