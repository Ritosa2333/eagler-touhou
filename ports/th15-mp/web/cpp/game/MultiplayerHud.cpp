#if TH15_MULTIPLAYER
#include "MultiplayerHud.hpp"
#include "AnmCoordinates.hpp"
#include "MultiplayerTransferFeedback.hpp"
#include "MultiplayerEconomy.hpp"
#include <algorithm>
#include <cstdio>
namespace th15 {
namespace {template<typename... T>std::string format(const char* pattern,T... v){char text[128];std::snprintf(text,sizeof(text),pattern,v...);return text;}}
bool MultiplayerHud::fail(const std::string& value){if(error.empty())error=value.empty()?"Multiplayer HUD unavailable":value;return false;}
float MultiplayerHud::top(unsigned seat)const{return 96.f+float(seat)*(battle.mp_options.seat_count==3?78.f:120.f);}
float MultiplayerHud::team_top()const{return battle.mp_options.seat_count==3?330.f:342.f;}
u32 MultiplayerHud::widget(i32 script,float original_y,float y){
    auto* root=battle.animations.registry.find(hud.root);if(!root){fail("Multiplayer HUD root unavailable");return 0;}
    const auto handle=battle.animations.create_overlay(battle.animations.resource_id(root->resource),script);
    auto* vm=battle.animations.registry.find(handle);if(!vm){fail(battle.animations.error);return 0;}
    widgets.push_back(handle);
    // ANM coordinates use either the native 640 or 1280 pixel basis. Keep
    // its flags, atlas, dimensions and animation intact; translate only Y.
    const u32 mode=vm->visual.render_flags>>20&7;
    const float factor=mode==1||mode==3?battle.environment.resolution_scale:mode==2||mode==4?battle.environment.resolution_scale*.5f:1.f;
    if(factor<=0){fail("Multiplayer HUD resolution is zero");return 0;}
    vm->visual.translation.y=(y-original_y)*battle.environment.resolution_scale/factor;
    return handle;
}
MultiplayerHud::MultiplayerHud(GameBattle& b,GameHud& h):battle(b),hud(h){
    // Initialize the native item script with private presentation RNG/context.
    // Never spawn a gameplay item or advance either authoritative RNG stream.
    if(!battle.animations.bind_template(transfer_power_icon,battle.resources.bullet,114)){fail(battle.animations.error);return;}
    auto icon_environment=battle.environment;auto icon_random=battle.visual_random;
    transfer_power_icon.environment=&icon_environment;transfer_power_icon.object_host=nullptr;
    const int icon_result=transfer_power_icon.tick(icon_random,1);
    transfer_power_icon.environment=&battle.environment;
    if(icon_result<0||!transfer_power_icon.sprite){fail("Transfer Power icon initialization failed");return;}
    for(auto& values:displayed)values.fill(-999);
    for(unsigned seat=0;seat<battle.mp_options.seat_count;++seat){const float y=top(seat);
        const bool compact=battle.mp_options.seat_count==3;
        for(unsigned i=0;i<8;++i){lives[seat][i]=widget(30+i,96,y);bombs[seat][i]=widget(38+i,134,y+(compact?26:38));}
        // Original labels, fragment bars and the original separators.
        for(const auto& entry:std::array<std::pair<i32,float>,11>{{{8,96},{9,112},{10,134},{11,150},{12,182},{20,128},{21,128},{22,166},{23,166},{24,198},{25,198}}}){
            // 3P omits only fragment text, retaining native stock icons and
            // their partial-fragment fill. Close the vacated fragment rows.
            if(compact&&(entry.first==9||entry.first==11))continue;
            const float row=compact?(entry.second==128?116:entry.second==134?122:entry.second==166?142:entry.second==182?150:entry.second==198?166:entry.second):entry.second;
            widget(entry.first,entry.second,y+row-96);
        }
    }
    for(const auto& entry:std::array<std::pair<i32,float>,6>{{{13,204},{14,226},{26,220},{27,220},{28,242},{29,242}}})
        widget(entry.first,entry.second,team_top()+entry.second-204);
}
MultiplayerHud::~MultiplayerHud(){for(auto handle:widgets)battle.animations.retire(handle);}
bool MultiplayerHud::draw_transfer_feedback(AnmRenderer& renderer,HudDrawServices& sink){
    if(!error.empty())return false;
    for(unsigned seat=0;seat<battle.mp_options.seat_count;++seat){const auto& pilot=*battle.pilots[seat];
        const auto feedback=multiplayer_transfer_feedback(pilot.alive(),pilot.life_hold,battle.mp_life_receiver(seat)>=0,multiplayer_can_give_power(pilot.session.power,pilot.session.power_step)?pilot.power_taps:0);
        if(feedback.kind==MultiplayerTransferFeedback::none)continue;
        const auto p=pilot.player->motion.position;HudTextStyle style;style.font=2;style.coordinate_space=2;
        if(feedback.kind==MultiplayerTransferFeedback::life){
            const auto value=format("L %u/5",feedback.value);
            if(!sink.hud_text({{p.x+224-float(value.size())*3.5f,p.y-8,0},style,value}))return fail("Transfer life progress text failed");
        }else{
            auto icon=transfer_power_icon;icon.visual.translation={p.x-16,p.y-24,0};
            if(renderer.draw(icon)==-2)return fail(renderer.error);
            if(!sink.hud_text({{p.x+225,p.y-5,0},style,format("%u",feedback.value)}))return fail("Transfer Power progress text failed");
        }
    }
    return true;
}
bool MultiplayerHud::prepare(AnmRenderer& renderer){
    if(!error.empty())return false;
    auto& hidden=renderer.multiplayer_hidden_hud;hidden.clear();
    for(auto id:hud.life_icons)if(auto* vm=battle.animations.registry.find(id))hidden.push_back(vm);
    for(auto id:hud.bomb_icons)if(auto* vm=battle.animations.registry.find(id))hidden.push_back(vm);
    if(auto* root=battle.animations.registry.find(hud.root))for(auto* vm:battle.animations.registry.children(*root))
        if((vm->source_script>=8&&vm->source_script<=14)||(vm->source_script>=20&&vm->source_script<=29))hidden.push_back(vm);
    for(unsigned seat=0;seat<battle.mp_options.seat_count;++seat){const auto& p=*battle.pilots[seat];const auto& stock=p.session;
        const i32 tier=seat?p.owned_life_piece_tier:battle.score.life_piece_tier,threshold=battle.score.difficulty==4?(tier<5?5:99999999):3;
        const std::array<i32,5> next{std::clamp(stock.extra_lives,0,8),stock.life_pieces,std::clamp(stock.bombs,0,8),stock.bomb_pieces,threshold};
        for(unsigned kind=0;kind<2;++kind)if(next[kind*2]!=displayed[seat][kind*2]||next[kind*2+1]!=displayed[seat][kind*2+1]||(!kind&&next[4]!=displayed[seat][4])){
            const auto count=unsigned(next[kind*2]);const auto pieces=next[kind*2+1];auto& icons=kind?bombs[seat]:lives[seat];
            for(unsigned i=0;i<8;++i){const i32 label=i<count?2:i==count?7+(kind?pieces:wrapping_mul(pieces,5)/threshold):3;
                if(!battle.animations.interrupt(icons[i],label,true))return fail(battle.animations.error);
            }
        }
        displayed[seat]=next;
    }
    return true;
}
bool MultiplayerHud::queue_text(HudDrawServices& sink){
    if(!error.empty())return false;HudTextStyle style;
    const auto put=[&](float x,float y,const std::string& value){return sink.hud_text({{x,y,0},style,value})||fail("Multiplayer HUD text submission failed");};
    for(unsigned seat=0;seat<battle.mp_options.seat_count;++seat){const auto& p=*battle.pilots[seat];const auto& stock=p.session;const float dy=top(seat)-96;
        style={};style.font=1;style.color=seat==battle.mp_options.local_seat?0xffffff80:0xffffffff;
        if(!put(battle.mp_options.seat_count==3?488.f:436.f,(battle.mp_options.seat_count==3?96.f:84.f)+dy,format("%uP",seat+1)))return false;
        const i32 tier=seat?p.owned_life_piece_tier:battle.score.life_piece_tier,threshold=battle.score.difficulty==4?(tier<5?5:99999999):3;
        // Match GameHudDraw's native fragment and split Power typography.
        style={};style.font=4;style.scale={.6f,.6f};
        if(battle.mp_options.seat_count!=3){
            if(threshold<999999){if(!put(576,120+dy,format("%3d",stock.life_pieces))||!put(597,120+dy,format("/%d",threshold)))return false;}
            else if(!put(569,120+dy,"---/---"))return false;
            if(!put(576,158+dy,format("%3d",stock.bomb_pieces))||!put(597,158+dy,"/5"))return false;
        }
        if(stock.power_step<=0)return fail("Multiplayer HUD power step is zero");
        const float power_y=(battle.mp_options.seat_count==3?150.f:182.f)+dy;
        style.color=0xffff8030;style.scale={1,1};if(!put(540,power_y,format("%d.",stock.power/stock.power_step)))return false;
        style.scale={.6f,.6f};if(!put(560,power_y+7,format("%.2d",wrapping_mul(stock.power%stock.power_step,100)/stock.power_step)))return false;
        style.scale={1,1};if(!put(574,power_y,format("/%d.",battle.score.max_power/stock.power_step)))return false;
        style.scale={.6f,.6f};if(!put(606,power_y+7,"00"))return false;
    }
    style={};style.font=4;style.alignment=2;style.color=0xff40c0ff;
    const auto& score=battle.score;if(!put(620,team_top(),grouped_number(u32((score.point_value/100)-(score.point_value/100)%10))))return false;
    style.color=0xffffffff;if(!put(620,team_top()+22,grouped_number(u32(score.graze_chapter))))return false;
    style.scale={.6f,.6f};return put(620,team_top()+39,grouped_number(u32(score.graze_total)));
}
}
#endif
