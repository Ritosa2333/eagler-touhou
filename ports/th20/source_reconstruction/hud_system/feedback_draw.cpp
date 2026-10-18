#include "hud.hpp"
#include "draw_support.hpp"
#include "../player_entity/owner.hpp"
#include "../platform_window/platform_window.hpp"
namespace th20::source::hud {
namespace n=recovered;namespace d=draw_detail;
void draw_feedback(player_entity::Feedback& feedback) { //4f8840
    const auto active=[&]{return feedback.timers[0].current>0;}; //4ff7e0 ->461070
    if(n::signed_bits(feedback.fields_30[0])>0 && n::signed_bits(feedback.fields_30[1])<n::signed_bits(feedback.fields_30[0]) && active()){
        feedback.fields_30[1]=feedback.fields_30[0];n::timer_set(feedback.timers[1],120);
    }
    if(feedback.fields_30[1]==0)return;
    auto& renderer=*text::renderer;const sprite::Vec3 position{56,72,0};renderer.color=0xffffff00;
    if(!active())renderer.color=feedback.timers[1].current%6<3?0xffffff00:0x80808080;
    const std::uint8_t alpha=feedback.enabled?0x40:0xff;d::color_alpha(renderer,alpha);d::shadow_alpha(renderer,alpha);
    d::font(renderer,6);d::layer(renderer,0);d::scale(renderer,1,1);d::alignment(renderer,2,2);
    renderer.write_integer(position,n::signed_bits(feedback.fields_30[1]));
    d::alignment(renderer,1,2);d::scale(renderer,.8f,.8f);
    renderer.write_ascii(position,n::signed_bits(feedback.fields_30[1])<2?"hit":"hits");
    d::scale(renderer,1,1);renderer.color=0xffffffff;d::color_alpha(renderer,255);d::shadow_alpha(renderer,255);d::layer(renderer,0);d::alignment(renderer,1,1);d::font(renderer,0);
}
int draw_player(){ //4b5790
    platform_window::select_viewport(program_entry::graphics_state,4);
    auto& player=*static_cast<player_entity::Player*>(game_session::context(0).objects_04[0]);draw_feedback(player.feedback);return 1;
}
}
