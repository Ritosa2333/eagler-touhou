#include "PlayerMotion.hpp"
#include "PresentationAudit.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "../multiplayer/PlayerPresentation.hpp"
#endif
namespace th08 {
void update_player_motion(PlayerMotionState& s,PlayerMotionInput& input,Timer& shooting,GameGauge& gauge,const ShotProfile& human,const ShotProfile& focused,const FrameTiming& timing,PlayerMotionActions& actions){
    s.movement.direction=player_direction(input.buttons);
    update_player_form(s.form,s.movement,s.options,input.character,input.buttons,input.bomb,input.bomb_type,input.always_hitbox!=0,actions);
    move_player(s.movement,human,focused,s.form.focused,input.character,input.buttons,input.minimum,input.extent,timing,&actions,false);
    PlayerOptionContext context{s.movement.position,s.movement.history[15],input.enemy,shooting,s.movement.direction,input.bomb,input.buttons,s.form.focused,input.enemy_present};
    PlayerOptions options(context);options.actions=&actions;
    for(auto& option:s.options)if(option.update!=PlayerOptionKind::None){options.update(option,option.update);actions.step_animation(option.animation);option.timer.tick(timing);}
    input.enemy_present=context.enemy_present;
    if((input.buttons&1)&&!input.gui_blocked&&!input.tampered)trigger_shooting(shooting);
    update_player_gauge(s.gauge,s.form,shooting,gauge,input.gui_blocked,input.bomb,s.movement.position,timing,actions);
    record_player_position(s.movement);
}
void draw_player_motion(PlayerMotionState& s,const Vec2& offset,bool game_over,PlayerMotionActions& actions,u8 proximity_alpha){
    if(!game_over){
        TH08_AUDIT_SCOPE(Player,&s.animation,s.animation.currentTimeInScript.current,0);
        s.animation.pos={Scalar::add(offset.x,s.movement.position.x),Scalar::add(offset.y,s.movement.position.y),.1f};
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        const ZunColor color1=s.animation.color1,color2=s.animation.color2;
        s.animation.color1.a=multiplayer::clamp_player_alpha(s.animation.color1.a,proximity_alpha);
        s.animation.color2.a=multiplayer::clamp_player_alpha(s.animation.color2.a,proximity_alpha);
#endif
        actions.draw_player(s.animation);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        s.animation.color1=color1;s.animation.color2=color2;
#endif
    }
    PlayerOptionContext context;PlayerOptions options(context);options.actions=&actions;
    for(u32 i=0;i<4;++i){auto& option=s.options[i];if(!option.draw)continue;
        TH08_AUDIT_SCOPE(PlayerOption,&option,option.animation.currentTimeInScript.current,i);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        const ZunColor color1=option.animation.color1,color2=option.animation.color2;
        option.animation.color1.a=multiplayer::clamp_player_alpha(option.animation.color1.a,proximity_alpha);
        option.animation.color2.a=multiplayer::clamp_player_alpha(option.animation.color2.a,proximity_alpha);
#endif
        options.draw(option,offset);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        option.animation.color1=color1;option.animation.color2=color2;
#endif
    }
}
}
