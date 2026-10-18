#include "GuiFrame.hpp"
#include "HighRefresh.hpp"
#include <cmath>
namespace th10 {
namespace {
float presentation_timer(const Timer& timer){
    if(!high_refresh::active||timer.previous<0||std::abs(timer.current-timer.previous)>2)return float(timer.current);
    return high_refresh::lerp_world(float(timer.previous),timer.fractional);
}
}
// 0x415800. Health and faith bars use separate shadow and foreground rectangles;
// the shadow's stored right edge is rounded before subtracting one pixel.
i32 Gui::draw(GuiDrawEnvironment& env){
    for(u32 i=0;i<10;++i){env.draw_animation(high_score_digits[i]);env.draw_animation(score_digits[i]);}
    for(auto& vm:life_icons)env.draw_animation(vm);
    for(auto& vm:power_digits)env.draw_animation(vm);
    for(auto& vm:faith_digits)env.draw_animation(vm);
    if(env.game->faith_timer.current){
        const float faith=presentation_timer(env.game->faith_timer);
        ScreenRect rect{51,461,(number(faith)*number(.29230770468711853f)+number(51)).to_float(),463};
        env.rectangle(rect,faith_digits[0].color&0xff000000);
        rect.right=Scalar::sub(rect.right,1);rect.left=50;rect.top=460;rect.bottom=462;env.rectangle(rect,(faith_digits[0].color&0xff000000)|0xffffff);
    }
    const float presented_boss_health=env.presentation_boss_health(displayed_boss_health);
    if(presented_boss_health>0){
        ScreenRect rect{41,23,(number(presented_boss_health)*number(342)+number(41)).to_float(),25};env.rectangle(rect,0xff000000);
        rect.right=Scalar::sub(rect.right,1);rect.left=40;rect.top=22;rect.bottom=24;env.rectangle(rect,0xffffffff);
        for(const auto& bar:boss_health)if(bar.amount!=0){const float amount=bar.amount<presented_boss_health?bar.amount:presented_boss_health;rect={40,22,(number(amount)*number(342)+number(40)).to_float(),24};env.rectangle(rect,static_cast<u32>(bar.style));}
    }
    if(*env.enemies){auto* boss=(*env.enemies)->bosses[0];if(boss&&!(boss->state.flags&0x11))env.draw_animation(enemy_marker);if(*env.enemies&&countdown>=0&&(*env.enemies)->bosses[0]&&!dialogue)for(auto& vm:countdown_digits)env.draw_animation(vm);}
    return 1;
}
}
