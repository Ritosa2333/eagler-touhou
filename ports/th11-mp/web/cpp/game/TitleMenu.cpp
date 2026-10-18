#include "TitleMenu.hpp"
namespace th11 {
void TitleMenu::change(TitleScreen next){screen=next;substate=0;timer.set(0,&animations.rate);}
void TitleMenu::step(i32 next){substate=next;timer.set(0,&animations.rate);}
bool TitleMenu::available(i32 c,i32 p)const{return c>=0&&c<2&&p>=0&&p<3&&(scores.settings[0x1c+c*3+p]&0x10);}
bool TitleMenu::unlocked()const{for(i32 c=0;c<2;++c)for(i32 p=0;p<3;++p)if(available(c,p))return true;return false;}
u32 TitleMenu::create(i32 script,i32 slot,i32 file){
    if(slot<0)slot=script;
    if(file==3&&!text_resource){error="title text resource missing";return 0;}
    auto* vm=animations.create(file==1?variant:file==2?ascii:file==3?*text_resource:title,script,file==1?25:file==2?2:file==3?0:24,22);
    if(!vm){error="title animation creation failed: "+std::to_string(script);return 0;}
    return handles[slot]=vm->id;
}
void TitleMenu::interrupt(u32 id,i32 label,bool immediate){
    auto* vm=animations.find(id);if(!vm)return;
    auto apply=[&](AnmVm& v){v.pending_interrupt=i16(label);if(immediate&&v.update(animations)<0)error="title animation interrupt failed";};
    apply(*vm);
    if(!vm->child.previous)for(auto* node=vm->child.next;node;node=node->next)apply(*node->value);
}
void TitleMenu::close(i32 slot){family(slot,1);handles[slot]=0;}
u32 TitleMenu::child(i32 slot,i32 script){
    auto* vm=animations.find(handles[slot]);if(!vm){handles[slot]=0;return 0;}
    for(auto* node=&vm->child;node;node=node->next)if(node->value->script_index==script)return node->value->id;
    return 0;
}
void TitleMenu::hide(u32 id){auto* vm=animations.find(id);if(!vm)return;vm->flags&=~2u;if(!vm->child.previous)for(auto* n=vm->child.next;n;n=n->next)n->value->flags&=~2u;}
void TitleMenu::disable_extra(){if(!unlocked())for(const auto script:{4,12})if(auto* vm=animations.find(child(0,script)))vm->pending_interrupt=29;}
void TitleMenu::highlight(i32 slot,i32 base){family(slot,3,true);family(slot,cursor.selected+base);}
void TitleMenu::move(u32 keys,u32 negative,u32 positive){cursor.previous=cursor.selected;if(keys&negative)cursor.move(-1);if(keys&positive)cursor.move(1);}
bool TitleMenu::update(u32 pressed,u32 repeat){
    sounds.clear();text_requests.clear();music_request=-1;music_pause=false;fade_requested=false;volume_changed=false;replay_scan_requested=false;
    if((flags&1)&&++music_start_delay>4){music_request=0;scores.settings[0x26]=1;flags&=~1u;music_start_delay=0;}
    switch(screen){
    case TitleScreen::Main:main(pressed,repeat);break;
    case TitleScreen::Difficulty:difficulty(pressed,repeat);break;
    case TitleScreen::Character:character(pressed,repeat);break;
    case TitleScreen::Partner:partner(pressed,repeat);break;
    case TitleScreen::Practice:practice(pressed,repeat);break;
    case TitleScreen::Options:options(pressed,repeat);break;
    case TitleScreen::Keys:keys(pressed,repeat);break;
    case TitleScreen::Music:music_room(pressed,repeat);break;
    case TitleScreen::Replays:replays(pressed,repeat);break;
    case TitleScreen::Records:records(pressed,repeat);break;
    case TitleScreen::Results:results(pressed,repeat);break;
    case TitleScreen::ReplaySave:replay_save(pressed,repeat);break;
    case TitleScreen::Exit:case TitleScreen::Loading:break;
    default:error="title screen not restored: "+std::to_string(i32(screen));return false;
    }
    timer.tick();return error.empty();
}
void TitleMenu::main(u32 pressed,u32 repeat){
    switch(substate){
    case 0:
        cursor.count=8;if(!unlocked()&&cursor.disabled_count<16)cursor.disabled[cursor.disabled_count++]=1;
        if(selection.flags&0x10){cursor.select(2);selection.flags&=~0x10u;}
        step(1);
        if(flags&2){create(79);create(83);flags&=~2u;}
        else {if(!exists(79)&&!exists(80))create(80);if(!exists(83)&&!exists(84))create(84);timer.set(70,&animations.rate);}
        [[fallthrough]];
    case 1:
        if(timer.current==70){create(0);if(!exists(238))create(0,238,1);disable_extra();}
        if(timer.current>80){step(2);highlight(0,17);disable_extra();}break;
    case 2:
        move(pressed|repeat,16,32);
        if(cursor.previous!=cursor.selected){sounds.push_back(12);highlight(0);disable_extra();}
        if(pressed&0x102){
            if(cursor.selected==7){sounds.push_back(11);step(4);return;}
            sounds.push_back(11);cursor.select(7);highlight(0);disable_extra();
        }
        if(pressed&0x80001){
            family(0,6);
            if(cursor.selected<=5){sounds.push_back(10);close(83);close(84);family(238,1);step(4);if(cursor.selected==5)sounds.push_back(10);}
            else {sounds.push_back(cursor.selected==7?11:10);step(4);}
        }break;
    case 4:
        if(timer.current<20)break;
        switch(cursor.selected){
        case 0:case 2:
            if(cursor.selected==2)selection.flags|=0x10;else selection.flags&=~0x10u;
            close(79);change(TitleScreen::Difficulty);cursor.push();
            if(selection.last_difficulty>=4)selection.last_difficulty=selection.difficulty=1;
            cursor.select(selection.last_difficulty);selection.difficulty=selection.last_difficulty;break;
        case 1:
            selection.flags&=~0x10u;close(79);change(TitleScreen::Difficulty);cursor.push();
            selection.saved_difficulty=selection.difficulty;selection.last_difficulty=selection.difficulty=4;cursor.select(0);break;
        case 3:close(79);change(TitleScreen::Replays);cursor.push();break;
        case 4:close(79);change(TitleScreen::Records);cursor.push();break;
        case 5:close(79);change(TitleScreen::Music);cursor.push();break;
        case 6:change(TitleScreen::Options);cursor.push();break;
        case 7:change(TitleScreen::Exit);break;
        }break;
    }
}
void TitleMenu::difficulty(u32 pressed,u32 repeat){
    const i32 group=selection.difficulty>=4?118:117;
    switch(substate){
    case 0:
        if(!exists(92)){create(92);create(18,193,2);}
        selection.difficulty=selection.last_difficulty;cursor.count=selection.difficulty>=4?1:4;
        close(group);create(group);highlight(group,17);create(96);step(1);
        [[fallthrough]];
    case 1:if(timer.current>6)step(2);break;
    case 2:
        if(selection.difficulty<4){move(pressed|repeat,16,32);if(cursor.previous!=cursor.selected){sounds.push_back(12);highlight(group);}}
        if(pressed&0x102){step(4);sounds.push_back(11);close(group);return;}
        if(pressed&0x80001){family(group,6);interrupt(child(group,selection.difficulty<4?cursor.selected+107:111),2);step(3);sounds.push_back(10);}break;
    case 3:
        if(timer.current>=14){close(96);change(TitleScreen::Character);if(selection.difficulty<4)selection.difficulty=cursor.selected;
            selection.last_difficulty=selection.difficulty;cursor.push();cursor.wrap=1;cursor.count=2;cursor.select(selection.last_character);selection.character=selection.last_character;}
        break;
    case 4:
        if(timer.current>=6){close(96);close(92);family(193,1);change(TitleScreen::Main);
            selection.last_difficulty=selection.difficulty=selection.difficulty<4?cursor.selected:selection.saved_difficulty;cursor.pop();}
        break;
    }
}
}
