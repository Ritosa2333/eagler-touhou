#if TH15_MULTIPLAYER
#include "MultiplayerHud.hpp"
#include <algorithm>
#include <cstdio>
namespace th15 {
namespace {template<typename... T>std::string format(const char* pattern,T... v){char text[128];std::snprintf(text,sizeof(text),pattern,v...);return text;}}
bool MultiplayerHud::fail(const std::string& value){if(error.empty())error=value.empty()?"Multiplayer HUD unavailable":value;return false;}
float MultiplayerHud::top(unsigned seat)const{return 88.f+float(seat)*(battle.mp_options.seat_count==3?86.f:102.f);}
float MultiplayerHud::team_top()const{return battle.mp_options.seat_count==3?340.f:276.f;}
void MultiplayerHud::prepare(AnmRenderer& renderer){
    auto& hidden=renderer.multiplayer_hidden_hud;hidden.clear();
    for(auto id:hud.life_icons)if(auto* vm=battle.animations.registry.find(id))hidden.push_back(vm);
    for(auto id:hud.bomb_icons)if(auto* vm=battle.animations.registry.find(id))hidden.push_back(vm);
    if(auto* root=battle.animations.registry.find(hud.root))for(auto* vm:battle.animations.registry.children(*root))
        if((vm->source_script>=8&&vm->source_script<=14)||(vm->source_script>=20&&vm->source_script<=29))hidden.push_back(vm);
        else if(vm->source_script==3)vm->visual.color=(vm->visual.color&0xff000000u)|0x505050u;
}
bool MultiplayerHud::queue_text(HudDrawServices& sink){
    if(!error.empty())return false;HudTextStyle style;style.font=1;
    const auto put=[&](float x,float y,const std::string& value){return sink.hud_text({{x,y,0},style,value})||fail("Multiplayer HUD text submission failed");};
    constexpr const char* names[]={"REIMU","MARISA","SANAE","REISEN"};
    for(unsigned s=0;s<battle.mp_options.seat_count;++s){const auto& p=*battle.pilots[s];const auto& stock=p.session;const float y=top(s);const u32 alpha=p.ghost?0x70000000:0xff000000;
        style={};style.font=1;style.color=alpha|(s==battle.mp_options.local_seat?0xffff80:0xffffff);
        if(!put(428,y,format("%uP %s%s",s+1,names[p.character],p.ghost?" OUT":s==battle.mp_options.local_seat?" *":"")))return false;
        const auto resource=[&](float row,const char* label,i32 amount,u32 color){
            style.font=1;style.alignment=1;style.color=alpha|0xffffff;if(!put(428,row,label))return false;
            const unsigned filled=unsigned(std::clamp(amount,0,8));style.font=2;style.color=alpha|color;if(!put(476,row,std::to_string(filled)))return false;
            for(unsigned i=0;i<8;++i){style.color=alpha|(i<filled?color:0x504050);if(!put(514+13.f*i,row,i<filled?"*":"-"))return false;}
            return true;
        };
        if(!resource(y+16,"LIFE",stock.extra_lives,0xff80c0)||!resource(y+38,"BOMB",stock.bombs,0x80ff80))return false;
        style.font=1;style.color=alpha|0xff8030;style.alignment=1;if(!put(428,y+60,"POWER"))return false;
        const i32 tier=s?p.owned_life_piece_tier:battle.score.life_piece_tier,threshold=battle.score.difficulty==4?(tier<5?5:99999999):3;
        style.alignment=2;style.color=alpha|0xffffff;
        if(!put(620,y+28,threshold<100?format("%d/%d",stock.life_pieces,threshold):"---/---")||!put(620,y+50,format("%d/5",stock.bomb_pieces)))return false;
        if(stock.power_step<=0)return fail("Multiplayer HUD power step is zero");
        style.font=4;style.color=alpha|0xff8030;
        if(!put(620,y+60,format("%d.%02d/%d.00",stock.power/stock.power_step,wrapping_mul(stock.power%stock.power_step,100)/stock.power_step,battle.score.max_power/stock.power_step)))return false;
    }
    style={};style.font=1;if(!put(428,team_top(),"TEAM"))return false;
    style.alignment=1;style.color=0xff40c0ff;if(!put(428,team_top()+18,"POINT"))return false;
    style.color=0xffffffff;if(!put(428,team_top()+40,"GRAZE"))return false;
    style.font=4;style.alignment=2;style.color=0xff40c0ff;
    const auto& score=battle.score;if(!put(620,team_top()+18,grouped_number(u32((score.point_value/100)-(score.point_value/100)%10))))return false;
    style.color=0xffffffff;if(!put(620,team_top()+40,grouped_number(u32(score.graze_chapter))))return false;
    style.font=1;return put(620,team_top()+62,"TOTAL "+grouped_number(u32(score.graze_total)));
}
}
#endif
