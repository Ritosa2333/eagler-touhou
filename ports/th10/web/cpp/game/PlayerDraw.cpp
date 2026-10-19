#include "PlayerDraw.hpp"
#include "HighRefresh.hpp"
#include <cmath>
namespace th10 {
namespace {float presentation_faith(const Timer& timer){if(!high_refresh::active||timer.previous<0||std::abs(timer.current-timer.previous)>2)return float(timer.current);return high_refresh::lerp_world(float(timer.previous),timer.fractional);}}
// The optional faith bar shares the player's fractional presentation position.
i32 Player::draw(PlayerDrawEnvironment& env){
    if(state==2)return 1;
    animation.position={Scalar::add(position.x,224),Scalar::add(position.y,16),position.z};env.draw_animation(animation);
    for(auto& option:options)if(option.on_draw)env.draw_option(option);
    if(env.controller_flags&&*env.controller_flags<0&&*env.enemies&&*env.gui&&!(*env.enemies)->bosses[0]&&!(*env.gui)->dialogue&&!*env.results_state&&env.game->faith_timer.current){
        const float faith=presentation_faith(env.game->faith_timer);
        const float x=(number(position.x)+number(225)-number(16)).to_float();
        ScreenRect rect{x,0,(number(faith)*number(.29230770468711853f)+number(x)).to_float(),0};
        rect.top=(number(position.y)+number(17)-number(24)).to_float();rect.bottom=Scalar::add(rect.top,2);env.rectangle(rect,0x80000000);
        rect.left=Scalar::sub(rect.left,1);rect.right=Scalar::sub(rect.right,1);rect.top=Scalar::sub(rect.top,1);rect.bottom=Scalar::sub(rect.bottom,1);env.rectangle(rect,0xffffffff);
    }
    return 1;
}
}
