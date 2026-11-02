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
    if(score!=displayed_score){u32 change=u32(wrapping_sub(score,displayed_score))>>5;change=std::min(578910u,std::max(1u,change));if(score_increment<i32(change))score_increment=i32(change);const i32 difference=wrapping_sub(score,displayed_score);if(difference<score_increment)score_increment=difference;displayed_score=wrapping_add(displayed_score,score_increment);if(score<=displayed_score)score_increment=0;}
    if(progress.high_score<displayed_score){progress.mode_flags|=4;progress.high_score_extra=progress.continues;progress.high_score=displayed_score;}
}
bool Hud::draw(const HudDrawContext& context,HudDrawServices& services){
    HudTextStyle style;const auto text=[&](float x,float y,const std::string& value){return check(services.hud_text({{x,y,0},style,value}),"HUD text submission failed");};const auto* root=animations.registry.find(life_handles[0]);if(!check(root!=nullptr,"HUD stock animation unavailable"))return false;
    if(auto* vm=find(stage_clear_notice)){style.color=alpha(style.color,vm->visual.color);style.font=4;style.coordinate_space=2;style.alignment=style.vertical_alignment=0;if(!text(224,200,grouped_number(u32(stage_clear_bonus))))return false;style={};}
    if(auto* vm=find(chapter_result_notice)){if(vm->visual.flags&2){style.color=alpha(style.color,vm->visual.color);style.font=2;style.coordinate_space=2;style.alignment=2;style.vertical_alignment=0;if(!text(300,226,format("%d",chapter_result.grazes))||!text(308,246,format("%3.1f%%",double(chapter_result.percentage)))||!text(300,266,format("%3d",chapter_result.deaths))||!text(300,286,format("%6d",chapter_result.score)))return false;style.color=0xff8080ff;if(!text(300,296,format("  +%d",wrapping_mul(chapter_result.score/50000,10))))return false;style={};}}
    if(result_active){auto* vm=find(result_panel);if(!context.spell_available){result_active=0;if(!check(animations.retire(result_panel),"HUD result retirement failed"))return false;}else if(!vm)result_active=0;else{
        style.color=alpha(style.color,vm->visual.color);style.font=4;style.coordinate_space=2;if(!text(224,144,format("%3d.",std::min(999,context.spell_frames/60))))return false;style.scale={.6f,.6f};if(!text(268,150,format("%.2ds",wrapping_mul(context.spell_frames%60,100)/60)))return false;style.color=alpha(0xff808080,vm->visual.color);style.scale={1,1};const i32 encoded=context.spell_best_time,a=((encoded/100)%1000+934)%1000,b=(encoded%100+67)%100;const bool valid=encoded/100000-22==a+b;if(!text(224,160,format("%3d.",valid?a:999)))return false;style.scale={.6f,.6f};if(!text(268,166,format("%.2ds",valid?b:99)))return false;style={};
    }}
    const auto& score=context.score;const auto& progress=context.progress;style.color=alpha(0xff808080,root->visual.color);style.font=4;style.alignment=2;if(!text(620,42,score_number(u32(progress.high_score),progress.high_score_extra)))return false;style.color=alpha(0xffffffff,root->visual.color);if(!text(620,64,score_number(u32(displayed_score),progress.continues)))return false;
    if(!check(score.life_piece_tier>=0&&(score.difficulty!=4||score.life_piece_tier<=9),"HUD life-piece tier outside TH17 range"))return false;
    style.alignment=style.vertical_alignment=1;style.scale={.6f,.6f};const i32 required=score.difficulty==4&&score.life_piece_tier==9?99999999:3;const float piece_x=required<100?576.f:569.f;if(required<999999){if(!text(piece_x,120,format("%3d",session.life_pieces))||!text(piece_x+21,120,format("/%d",required)))return false;}else if(!text(piece_x,120,"---/---"))return false;
    if(!text(576,158,format("%3d",session.bomb_pieces))||!text(597,158,"/3"))return false;
    if(!check(session.power_step>0,"HUD power step must be positive"))return false;style.color=alpha(0xffff8030,root->visual.color);style.scale={1,1};if(!text(540,182,format("%d.",session.power/session.power_step)))return false;style.scale={.6f,.6f};if(!text(560,189,format("%.2d",wrapping_mul(session.power%session.power_step,100)/session.power_step)))return false;style.scale={1,1};if(!text(574,182,format("/%d.",score.max_power/session.power_step)))return false;style.scale={.6f,.6f};if(!text(606,189,"00"))return false;
    style.color=alpha(0xff40c0ff,root->visual.color);style.scale={1,1};style.alignment=2;if(!text(620,204,grouped_number(u32(score.point_value/100-(score.point_value/100)%10))))return false;style.color=alpha(0xffffffff,root->visual.color);if(!text(620,226,grouped_number(u32(context.graze_total))))return false;style={};
    if(current_seconds>=0&&enemies.boss_at(0)&&!(enemies.manager_flags&1)&&!dialogue&&!(game_flags&0x10000)){
        const auto* vm=animations.registry.find(clock_handles[0]);if(!check(vm!=nullptr,"HUD clock animation unavailable"))return false;const float x=float(vm->variables.position.x+16.f),y=float(vm->variables.position.y-7.f);style.color=vm->visual.color;style.font=4;style.coordinate_space=2;if(!text(x,y,"."))return false;style.scale={.6f,.6f};if(!text(float(x+8.f),float(y+6.f),format("%.2d",enemies.boss_hundredths)))return false;
    }
    return true;
}
}
