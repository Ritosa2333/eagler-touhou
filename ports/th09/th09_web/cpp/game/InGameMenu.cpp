#include "InGameMenu.hpp"
namespace th09 {
void InGameMenus::start(AnmVm& a,i32 script){resources.start(AnimationFile::ascii,a,script,false);}
void InGameMenus::select(AnmVm& a,bool selected,u32 color){a.color1.d3dColor=i32(selected?color:0xff505050);a.pos2=selected?Vec3{-4,-4,0}:Vec3{};}
void InGameMenus::update_pause(const InputFrame& input){
    auto& m=pause;const u16 keys=input.pressed;
    const auto close=[&](i32 state){services.menu_sound(10);m.state=state;for(u32 n=0;n<7;++n)if(m.animations[n].visible)m.animations[n].pendingInterrupt=2;m.frames=0;};
    if((keys&8)&&m.state!=3){close(3);m.animations[7].pendingInterrupt=1;}
    if((keys&0x200)&&m.state!=8)close(8);
    if(m.state==0){for(u32 n=0;n<7;++n)start(m.animations[n],i32(n)+1);interrupt(m,0,3,1);resources.sprite(AnimationFile::ascii,m.animations[6],services.difficulty+125);m.state=1;m.frames=0;}
    const i32 state=m.state;
    if(state==1||state==2){
        select(m.animations[1],state==1);select(m.animations[2],state==2);
        if(m.frames>3){
            if(keys&0x10){m.state=state==1?2:1;services.menu_sound(0);}if(keys&0x20){m.state=state==1?2:1;services.menu_sound(0);}
            if(keys&0x1001){services.menu_sound(10);interrupt(m,0,3,2);m.frames=0;if(state==1){m.state=3;m.animations[7].pendingInterrupt=1;}else{interrupt(m,3,3,1);m.state=5;}}
        }
    }else if(state==3&&m.frames>=20){m.state=0;hide(m,7);services.menu_action(InGameAction::resume);}
    else if(state>=4&&state<=7){
        const bool yes=state==4||state==6;select(m.animations[4],yes,0xffff8080);select(m.animations[5],!yes,0xffff8080);
        if(m.frames>3){
            if(keys&0x30){m.state=yes?(state==4?5:7):(state==5?4:6);services.menu_sound(0);}
            if(keys&0x1001){services.menu_sound(10);if(yes){interrupt(m,3,3,2);m.frames=0;m.state=m.state==4?8:9;}
                else {interrupt(m,0,3,1);interrupt(m,3,3,2);if(m.state==5)m.state=2;m.frames=0;}}
        }
    }else if(state==8&&m.frames>=20){m.state=0;services.menu_action(InGameAction::title);}
    else if(state==9&&m.frames>=20){
        if((services.game_flags&1)||services.difficulty==4){services.menu_action(InGameAction::replay_retry);return;}
        m.state=0;services.menu_action(InGameAction::retry);
    }
    advance(m,7,services.capture_enabled);
}
bool InGameMenus::update_game_over(const InputFrame& input){
    auto& m=game_over;const u16 keys=input.pressed;
    if(services.game_flags&8){services.menu_action(InGameAction::title);return true;}
    if(services.continues>2){services.menu_action(InGameAction::save_score);return true;}
    if(m.state==0){const bool extra=services.mode==GameMode::extra;const u32 count=extra?3:5;for(u32 n=0;n<count;++n)start(m.animations[n],i32(n)+(extra?17:8));interrupt(m,0,count,1);if(!extra)resources.sprite(AnimationFile::ascii,m.animations[4],120-services.continues);m.state=1;m.frames=0;}
    const i32 state=m.state;
    if(state==1||state==2){
        select(m.animations[1],state==2,0xffff8080);select(m.animations[2],state==1,0xffff8080);
        if(m.frames>3){if(keys&0x30){m.state=state==1?2:1;services.menu_sound(0);}if(keys&0x1001){services.menu_sound(10);interrupt(m,0,5,1);m.state=state==1?4:3;m.frames=0;}}
    }else if(state==3&&m.frames>19){
        m.state=0;m.frames=0;
        if(services.mode==GameMode::extra)services.menu_action(InGameAction::restart_extra);
        else {hide(m,5);++services.continues;services.menu_action(InGameAction::continue_match);}return true;
    }else if(state==4&&m.frames>19){m.state=0;m.frames=0;hide(m,5);services.menu_action(InGameAction::save_score);return true;}
    advance(m,5,services.capture_enabled);return false;
}
bool InGameMenus::update_match_end(const InputFrame& input){
    auto& m=match_end;const u16 keys=input.pressed;
    if(services.game_flags&8){services.menu_action(InGameAction::title);return true;}
    if(m.state==0){for(u32 n=0;n<3;++n){start(m.animations[n],i32(n)+13);m.animations[n].pendingInterrupt=1;}m.state=1;m.frames=0;}
    const i32 state=m.state;
    if(state>=1&&state<=3){
        for(i32 n=0;n<3;++n)select(m.animations[n],n==state-1);
        if(m.frames>=4){if(keys&0x10){m.state=state==1?3:state-1;services.menu_sound(0);}if(keys&0x20){m.state=state==3?1:state+1;services.menu_sound(0);}if(keys&0x1001){services.menu_sound(10);interrupt(m,0,3,2);m.state=state+3;m.frames=0;}}
    }else if(state>=4&&state<=6&&m.frames>19){m.state=0;services.menu_action(state==4?InGameAction::retry:state==5?InGameAction::title:InGameAction::save_score);return true;}
    advance(m,3,false);return false;
}
void InGameMenus::draw_game_over(){services.menu_view();if(services.capture_enabled&&(game_over.state||game_over.frames>2))services.menu_sprite(game_over.animations[5]);for(u32 n=0;n<5;++n)if(game_over.animations[n].visible)services.menu_sprite(game_over.animations[n]);}
}
