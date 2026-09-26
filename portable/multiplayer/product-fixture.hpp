// Product acceptance setup/observation only; never linked by a normal Runtime.
#pragma once
#include "../../th08_web/cpp/platform/BrowserRuntime.hpp"
#include <algorithm>
#include <memory>

namespace th08::multiplayer::fixture {
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
}
