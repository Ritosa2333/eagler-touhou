#include "Hud.hpp"
#include <algorithm>
#include <cstdio>
namespace th17 {
namespace {
template<typename...T>std::string format(const char* pattern,T... values){char text[256];std::snprintf(text,sizeof(text),pattern,values...);return text;}
u32 alpha(u32 color,u32 source){return (color&0xffffffu)|(source&0xff000000u);}
}
std::string grouped_number(u32 value){if(value<1000)return format("%d",signed_bits(value));if(value<1000000)return format("%d,%.3d",value/1000,value%1000);if(value<1000000000)return format("%d,%.3d,%.3d",value/1000000,(value/1000)%1000,value%1000);return format("%d,%.3d,%.3d,%.3d",value/1000000000,(value/1000000)%1000,(value/1000)%1000,value%1000);}
std::string score_number(u32 value,i32 last_digit){if(value<100)return format("%d",signed_bits(value*10+u32(last_digit)));if(value<100000)return format("%d,%.2d%d",value/100,value%100,last_digit);if(value<100000000)return format("%d,%.3d,%.2d%d",(value/100000)%1000,(value/100)%1000,value%100,last_digit);return format("%d,%.3d,%.3d,%.2d%d",(value/100000000)%1000,(value/100000)%1000,(value/100)%1000,value%100,last_digit);}
void Hud::update_score(i32 score,HudScoreState& progress)noexcept{
    if(u32(score)<u32(displayed_score)){displayed_score=score;score_increment=0;}
    if(u32(score)>u32(displayed_score)){const u32 difference=u32(score)-u32(displayed_score);const u32 change=std::clamp(difference>>5,1u,578910u);score_increment=i32(std::min(difference,std::max(u32(score_increment),change)));displayed_score=wrapping_add(displayed_score,score_increment);if(score==displayed_score)score_increment=0;}
    if(u32(progress.high_score)<u32(displayed_score)){progress.mode_flags|=4;progress.high_score_extra=progress.continues;progress.high_score=displayed_score;}
}
bool Hud::draw(const HudDrawContext& context,HudDrawServices& services,u32 viewer,const std::array<HudPilotStock,3>& pilots,bool captions){
    // Result panel retirement affects the animation timeline even on a frame
    // that only advances simulation. The numeric captions are transient.
    const auto* panel=result_active?find(result_panel):nullptr;
    if(result_active&&!context.spell_available){
        if(!check(animations.retire(result_panel),"HUD result retirement failed"))return false;
        panel=nullptr;
    }
    if(!panel)result_active=0;
    if(!captions)return true;
    HudTextStyle style;const auto text=[&](float x,float y,const std::string& value){return check(services.hud_text({{x,y,0},style,value}),"HUD text submission failed");};const auto* root=animations.registry.find(life_handles[0]);if(!check(root!=nullptr,"HUD stock animation unavailable"))return false;
    if(auto* vm=find(stage_clear_notice)){style.color=alpha(style.color,vm->visual.color);style.font=4;style.coordinate_space=2;style.alignment=style.vertical_alignment=0;if(!text(224,200,grouped_number(u32(stage_clear_bonus))))return false;style={};}
    if(auto* vm=find(chapter_result_notice)){if(vm->visual.flags&2){style.color=alpha(style.color,vm->visual.color);style.font=2;style.coordinate_space=2;style.alignment=2;style.vertical_alignment=0;if(!text(300,226,format("%d",chapter_result.grazes))||!text(308,246,format("%3.1f%%",double(chapter_result.percentage)))||!text(300,266,format("%3d",chapter_result.deaths))||!text(300,286,format("%6d",chapter_result.score)))return false;style.color=0xff8080ff;if(!text(300,296,format("  +%d",wrapping_mul(chapter_result.score/50000,10))))return false;style={};}}
    if(panel){
        style.color=alpha(style.color,panel->visual.color);style.font=4;style.coordinate_space=2;if(!text(224,144,format("%3d.",std::min(999,context.spell_frames/60))))return false;style.scale={.6f,.6f};if(!text(268,150,format("%.2ds",wrapping_mul(context.spell_frames%60,100)/60)))return false;style.color=alpha(0xff808080,panel->visual.color);style.scale={1,1};const i32 encoded=context.spell_best_time,a=((encoded/100)%1000+934)%1000,b=(encoded%100+67)%100;const bool valid=encoded/100000-22==a+b;if(!text(224,160,format("%3d.",valid?a:999)))return false;style.scale={.6f,.6f};if(!text(268,166,format("%.2ds",valid?b:99)))return false;style={};
    }
    const auto& score=context.score;const auto& progress=context.progress;style.color=alpha(0xff808080,root->visual.color);style.font=4;style.alignment=2;if(!text(620,42,score_number(u32(progress.high_score),progress.high_score_extra)))return false;style.color=alpha(0xffffffff,root->visual.color);if(!text(620,64,score_number(u32(displayed_score),progress.continues)))return false;
    for(u32 seat=0;seat<seats;++seat){
        const auto& pilot=pilots[seat];const auto* icon=animations.registry.find(stock_handles(seat,false)[0]);
        if(!check(pilot.session&&icon,"HUD pilot stock unavailable"))return false;
        const auto& stock=*pilot.session;const auto tier=pilot.life_piece_tier;
        if(!check(tier>=0&&(score.difficulty!=4||tier<=9),"HUD life-piece tier outside TH17 range"))return false;
        if(seats>1){
            style={};style.font=1;style.color=alpha(0xffffffff,icon->visual.color);
            if(stock.challenge&&!text(432,life_row(seat)+3,"MISS"))return false;
            style.color=alpha(seat==viewer?0xffffff80:0xffffffff,icon->visual.color);
            if(!text(491,life_row(seat)+3,format("%uP",seat+1)))return false;
        }
        style={};style.font=4;style.color=alpha(0xffffffff,icon->visual.color);style.scale={.6f,.6f};
        const float life=life_row(seat)+24.f,bomb=bomb_row(seat)+24.f;
        if(stock.challenge){
            style.scale={1,1};style.alignment=2;
            if(!text(620,life-8.f,grouped_number(u32(stock.deaths))))return false;
            style.scale={.6f,.6f};style.alignment=1;
        }else if(score.difficulty==4&&tier==9){
            if(!text(569,life,"---/---"))return false;
        }else if(!text(576,life,format("%3d",stock.life_pieces))||!text(597,life,"/3"))return false;
        if(!text(576,bomb,format("%3d",stock.bomb_pieces))||!text(597,bomb,"/3"))return false;
        if(!check(stock.power_step>0,"HUD power step must be positive"))return false;
        const float power=power_row(seat);
        style.color=alpha(0xffff8030,icon->visual.color);style.scale={1,1};if(!text(540,power,format("%d.",stock.power/stock.power_step)))return false;
        style.scale={.6f,.6f};if(!text(560,power+7,format("%.2d",wrapping_mul(stock.power%stock.power_step,100)/stock.power_step)))return false;
        style.scale={1,1};if(!text(574,power,format("/%d.",score.max_power/stock.power_step)))return false;
        style.scale={.6f,.6f};if(!text(606,power+7,"00"))return false;
    }
    style.color=alpha(0xff40c0ff,root->visual.color);style.scale={1,1};style.alignment=2;
    if(!text(620,point_row(),grouped_number(u32(score.point_value/100-(score.point_value/100)%10))))return false;
    style.color=alpha(0xffffffff,root->visual.color);if(!text(620,graze_row(),grouped_number(u32(context.graze_total))))return false;style={};
    if(current_seconds>=0&&enemies.boss_at(0)&&!(enemies.manager_flags&1)&&!dialogue&&!(game_flags&0x10000)){
        const auto* vm=animations.registry.find(clock_handles[0]);if(!check(vm!=nullptr,"HUD clock animation unavailable"))return false;const float x=float(vm->variables.position.x+16.f),y=float(vm->variables.position.y-7.f);style.color=vm->visual.color;style.font=4;style.coordinate_space=2;if(!text(x,y,"."))return false;style.scale={.6f,.6f};if(!text(float(x+8.f),float(y+6.f),format("%.2d",enemies.boss_hundredths)))return false;
    }
    return true;
}
}
