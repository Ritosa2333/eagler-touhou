#include "TitleMenus.hpp"
#include <algorithm>
namespace th09 {
namespace {
#include "TitleData.inc"
void move_cursor(AnmVm& a,float y){a.interpCurrentTimers[0].reset();a.interpEndTimers[0].reset(8);a.interpModes[0]=4;a.posInitial=a.pos;a.posFinal={a.pos.x,y,0};}
void fade_color(AnmVm& a,u32 rgb){a.interpCurrentTimers[1].reset();a.interpEndTimers[1].reset(8);a.interpModes[1]=4;a.color1Initial.d3dColor=(a.color1Initial.d3dColor&0xff000000)|(a.color1.d3dColor&0xffffff);a.color1Final.d3dColor=(a.color1Final.d3dColor&0xff000000)|(rgb&0xffffff);}
}
void TitleMenus::select_character(i32 selected,i32 base,i32 count,bool reverse){
    for(i32 n=0;n<count;++n){auto& a=animations[base+n*2];resources.sprite(AnimationFile::menu,a,a.baseSpriteIndex+1);a.pendingInterrupt=reverse?15:8;if(!reverse)resources.advance(a);}
    auto& a=animations[base+selected*2];resources.sprite(AnimationFile::menu,a,a.baseSpriteIndex);a.pendingInterrupt=reverse?14:7;if(!reverse)resources.advance(a);
}
void TitleMenus::skip_locked(i32 side,const std::array<u8,16>& order,i32 count,i32 direction){
    auto& unlocked=state.screen==TitleScreen::story_character?settings.story_unlocked:state.screen==TitleScreen::extra_character?settings.extra_unlocked:settings.versus_unlocked;
    auto& selected=state.character_selection[side];for(i32 n=0;n<count&&!unlocked[order[selected]];++n){selected+=direction;if(selected<0)selected+=count;if(selected>=count)selected-=count;}
}
void TitleMenus::character_list(const std::array<u8,16>& order,i32 count){
    const auto& unlocked=state.screen==TitleScreen::story_character?settings.story_unlocked:state.screen==TitleScreen::extra_character?settings.extra_unlocked:settings.versus_unlocked;i32 visible=0;
    for(i32 i=0;i<count;++i)if(unlocked[order[i]]){auto& a=animations[191+visible++];resources.sprite(AnimationFile::menu,a,235+order[i]);a.flag1=true;}
    for(i32 i=visible;i<16;++i)animations[191+i].flag1=false;
}
void TitleMenus::portraits(i32 side,i32 selected,i32 other,const std::array<u8,16>& order){
    const auto& unlocked=state.screen==TitleScreen::story_character?settings.story_unlocked:state.screen==TitleScreen::extra_character?settings.extra_unlocked:settings.versus_unlocked;
    i32 selected_row=0,other_row=0;for(i32 n=0;n<16&&order[n]!=selected;++n)selected_row+=unlocked[order[n]]!=0;for(i32 n=0;n<16&&order[n]!=other;++n)other_row+=unlocked[order[n]]!=0;
    move_cursor(animations[189+side],128.f+float(selected_row)*16.f);
    i32 row=0;for(i32 n=0;n<16;++n)if(settings.versus_unlocked[order[n]]){fade_color(animations[191+row],row==selected_row||row==other_row?0xffffffff:0xffa0a0a0);++row;}
}
void TitleMenus::portrait_color(i32 side,i32 character,u8 value){for(i32 base:{60,92}){auto& a=animations[base+character*2+side];a.color1.r=a.color1.g=a.color1.b=value;}}
void TitleMenus::life_icons(){
    for(i32 side=0;side<2;++side){const i32 base=141+side*6;for(i32 n=0;n<6;++n)animations[base+n].pendingInterrupt=9;
        for(i32 n=0;n<5;++n){const i32 health=state.health[side]-n*2;resources.sprite(AnimationFile::menu,animations[base+1+n],health<=0?197:health==1?196:195);}}
}
void TitleMenus::launch(bool versus,i32 stage){
    WorldConfiguration config;config.selection.mode=versus?GameMode::versus:state.screen==TitleScreen::story_character?GameMode::story:GameMode::extra;
    config.selection.characters[0]=settings.characters[0];config.selection.characters[1]=versus?settings.characters[1]:0;config.selection.difficulty=settings.difficulty;config.selection.selector=stage;config.selection.stage=0;
    config.selection.unlocked=settings.versus_unlocked;config.selection.lives=2;
    config.controllers[0]=versus&&(settings.versus==2||settings.versus==3);config.controllers[1]=!versus||settings.versus==1||settings.versus==3;
    for(i32 n=0;n<2;++n){config.health[n]=state.health[n];settings.health[n]=state.health[n];config.alternate[n]=settings.alternate[n];}output.title_music(-1);output.title_launch(config);leaving=true;
}
bool TitleMenus::character(){
    auto& s=state;const auto& in=input[2];
    if(!s.state){if(!s.load_frame){interrupt(13);advance();character_list(story_order,14);animations[189].flag1=true;animations[190].flag1=false;
        s.character_selection[0]=0;while(s.character_selection[0]<13&&story_order[s.character_selection[0]]!=settings.characters[0])++s.character_selection[0];skip_locked(0,story_order,14,1);
        const i32 selected=story_order[s.character_selection[0]];portrait_color(1,selected,255);portrait_color(1,story_order[s.character_selection[1]],255);select_character(selected,92,14);select_character(selected,60,14);
        s.confirmed[0]=0;s.confirmed[1]=1;s.state=s.frames=0;s.description=-1;s.layout_changed=1;portraits(0,selected,selected,story_order);s.health_adjustment=0;s.health[0]=s.health[1]=10;
        if(in.held&4){s.health_adjustment=1;life_icons();}animations[153].pendingInterrupt=output.title_clear_count(selected,settings.difficulty)?9:10;
    }if(s.load_frame==8)s.state=1;tick();return true;}
    if(s.state==1){
        if(!s.confirmed[0]){
            if(!(in.held&4)){const i32 direction=navigate(s.character_selection[0],14,in,true);if(direction){skip_locked(0,story_order,14,direction);const i32 selected=story_order[s.character_selection[0]];animations[153].pendingInterrupt=output.title_clear_count(selected,settings.difficulty)?9:10;select_character(selected,92,14,direction<0);select_character(selected,60,14,direction<0);portraits(0,selected,selected,story_order);}}
            else if(s.health_adjustment){const auto keys=in.pressed|in.repeat;if(keys&0x40){if(s.health[0]>1)--s.health[0];life_icons();}if(keys&0x80){if(s.health[0]<10)++s.health[0];life_icons();}}
        }
        const i32 selected=story_order[s.character_selection[0]];
        if((in.pressed&0x1001)&&!s.confirmed[0]){output.title_sound(10);s.confirmed[0]=1;portrait_color(0,selected,128);settings.alternate[0]=bool(in.held&4);}
        if(s.confirmed[0]&&s.confirmed[1]){settings.characters[0]=selected;settings.characters[1]=0;settings.alternate[1]=bool(in.held&0x4000);launch(false);return false;}
        if((in.pressed&10)&&!s.confirmed[0]){output.title_sound(11);settings.characters[0]=selected;change(s.screen==TitleScreen::story_character?TitleScreen::story_difficulty:TitleScreen::extra_difficulty);return true;}
    }
    tick();return true;
}
bool TitleMenus::versus_character(){
    auto& s=state;const bool paired=settings.versus==0||settings.versus==4;const auto& order=paired?paired_order:versus_order;
    if(!s.state){if(!s.load_frame){interrupt(13);advance();character_list(order,16);animations[189].flag1=true;animations[190].flag1=paired;
        for(i32 n=0;n<2;++n){s.character_selection[n]=0;while(s.character_selection[n]<15&&order[s.character_selection[n]]!=settings.characters[n])++s.character_selection[n];skip_locked(n,order,16,1);}
        const i32 left=order[s.character_selection[0]],right=order[s.character_selection[1]];if(!paired){portrait_color(1,left,255);portrait_color(1,right,255);}
        select_character(left,92,16);select_character(right,93,16);select_character(left,60,16);select_character(right,61,16);s.layout_changed=1;s.confirmed[0]=s.confirmed[1]=0;s.state=s.frames=0;s.description=-1;
        s.health[0]=settings.health[0];s.health[1]=settings.health[1];life_icons();portraits(0,left,paired?right:left,order);if(paired)portraits(1,right,left,order);
    }if(s.load_frame==8)s.state=1;tick();return true;}
    if(s.state!=1){tick();return true;}
    for(i32 side=0;side<2;++side){if(s.confirmed[side]||(!paired&&side==1&&!s.confirmed[0]))continue;const auto& in=input[paired?side:2];i32 direction=0;
        if(!(in.held&4))direction=navigate(s.character_selection[side],16,in,true);
        else{const auto keys=in.pressed|in.repeat;if(keys&0x40){if(s.health[side]>1)--s.health[side];life_icons();}if(keys&0x80){if(s.health[side]<10)++s.health[side];life_icons();}}
        if(direction){skip_locked(side,order,16,direction);const i32 character=order[s.character_selection[side]],other=order[s.character_selection[1-side]];const bool reverse=side?direction>=0:direction<0;select_character(character,92+side,16,reverse);select_character(character,60+side,16,reverse);portraits(side,character,!paired&&side==0?character:other,order);}
        if(paired&&(in.held&0x100)){auto& cursor=s.character_selection[side];cursor=(cursor+(random.next32()&15))%16;skip_locked(side,order,16,1);const i32 character=order[cursor];select_character(character,92+side,16);select_character(character,60+side,16);portraits(side,character,order[s.character_selection[1-side]],order);}
    }
    const i32 selected[2]={order[s.character_selection[0]],order[s.character_selection[1]]};
    const bool left_was_confirmed=s.confirmed[0];
    for(i32 side=0;side<2;++side){if(s.confirmed[side]||(!paired&&side==1&&!left_was_confirmed))continue;const auto& in=input[paired?side:2];if(in.pressed&0x1001){output.title_sound(10);s.confirmed[side]=1;portrait_color(side,selected[side],128);settings.alternate[side]=bool(in.held&4);if(s.confirmed[1-side]&&selected[0]==selected[1])settings.alternate[side]=!settings.alternate[1-side];if(!paired&&side==0){animations[190].flag1=true;portraits(1,selected[1],selected[0],order);}}}
    if(s.confirmed[0]&&s.confirmed[1]){settings.characters[0]=selected[0];settings.characters[1]=selected[1];settings.health[0]=s.health[0];settings.health[1]=s.health[1];output.title_sound(10);change(TitleScreen::versus_stage);return versus_stage();}
    bool back=false;
    if(paired){back=(!(s.confirmed[0])&&(input[0].pressed&10))||(!(s.confirmed[1])&&(input[1].pressed&10));if(!back)for(i32 side=0;side<2;++side)if(s.confirmed[side]&&(input[side].pressed&10)){s.confirmed[side]=0;output.title_sound(11);portrait_color(side,selected[side],255);}}
    else if(input[2].pressed&10){if(!s.confirmed[0])back=true;else if(s.confirmed[1]){s.confirmed[1]=0;output.title_sound(11);portrait_color(1,selected[1],255);}else{s.confirmed[0]=0;output.title_sound(11);portrait_color(0,selected[0],255);animations[190].flag1=false;portraits(0,selected[0],selected[0],order);}}
    if(back){output.title_sound(11);settings.characters[0]=selected[0];settings.characters[1]=selected[1];change(TitleScreen::versus_difficulty);return true;}tick();return true;
}
bool TitleMenus::versus_stage(){
    auto& s=state;const auto& in=input[2];
    const auto show=[&](){const i32 selected=stages[s.selection];i32 row=0;for(i32 n=0;n<16&&stage_order[n]!=selected;++n)row+=n>13||settings.versus_unlocked[stage_order[n]];for(i32 side=0;side<2;++side)move_cursor(animations[189+side],128.f+float(row)*16);i32 visible=0;for(i32 n=0;n<16;++n)if(n>13||settings.versus_unlocked[stage_order[n]]){fade_color(animations[207+visible],visible==row?0xffffffff:0xffa0a0a0);++visible;}};
    if(!s.state){if(!s.load_frame){interrupt(26);advance();i32 count=0;for(u8 item:stage_order)if(item>13||settings.versus_unlocked[item]){auto& a=animations[207+count];resources.sprite(AnimationFile::menu,a,251+item);a.flag1=true;stages[count++]=i8(item);}s.selection=count-2;for(i32 n=count;n<16;++n){animations[207+n].flag1=false;stages[n]=-1;}animations[189].flag1=animations[190].flag1=true;s.state=s.frames=0;s.description=-1;s.layout_changed=1;show();}if(s.load_frame==8)s.state=1;tick();return true;}
    if(s.state==1){const i32 direction=navigate(s.selection,16,in,true);if(direction){for(i32 n=0;n<16&&stages[s.selection]<0;++n)s.selection=(s.selection+direction+16)%16;show();}
        if(in.held&0x100){s.selection=(s.selection+(random.next32()&15))%16;show();}
        if(in.pressed&0x1001){output.title_sound(10);const i32 stage=stages[s.selection];launch(true,stage==14?-1:stage==15?-2:stage);return false;}
        if(in.pressed&10){output.title_sound(11);change(TitleScreen::versus_character);return versus_character();}
    }tick();return true;
}
}
