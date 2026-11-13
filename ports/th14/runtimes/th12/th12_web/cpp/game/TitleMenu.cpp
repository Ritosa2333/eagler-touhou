#include "TitleMenu.hpp"
#include "PortTitle.hpp"
namespace th11 {
void TitleMenu::change(TitleScreen next){screen=next;substate=0;timer.set(0,&animations.rate);}
void TitleMenu::step(i32 next){substate=next;timer.set(0,&animations.rate);}
bool TitleMenu::available(i32 c,i32 p)const{
    if(c<0||c>=(kPortTitle==13?4:3)||p<0||p>=2)return false;
#if PORT_TITLE == 14
    // Retail4591d0 checks the four normal-difficulty clear flags in CR,
    // starting at manager+7e4 (record+7dc), rather than legacy ST bytes.
    const auto* record=scores.characters[c*2+p].data();
    for(u32 difficulty=0;difficulty<4;++difficulty){u32 flag;std::memcpy(&flag,record+0x7dc+difficulty*4,4);if(flag)return true;}
    return false;
#else
    return scores.settings[0x1c+c*2+p]&0x10;
#endif
}
bool TitleMenu::unlocked()const{for(i32 c=0;c<(kPortTitle==14?3:kPortTitle==13?4:2);++c)for(i32 p=0;p<(kPortTitle==13||kPortTitle==14?2:3);++p)if(available(c,p))return true;return false;}
u32 TitleMenu::create(i32 script,i32 slot,i32 file){
    if(slot<0)slot=script;
    if(file==3&&!text_resource){error="title text resource missing";return 0;}
    // Retail TH14 45966a/45969b load title and title_v in slots16/17.
    const u16 title_file=kPortTitle==14?16:24,variant_file=kPortTitle==14?17:25;
    // Retail458d70 copies the title template and updates it without replacing
    // its layer. In particular title script0 retains layer0; its children set
    // their own visible layers in their scripts.
    const u32 layer=kPortTitle==14&&file==0?u32(-1):22u;
    auto* vm=animations.create(file==1?variant:file==2?ascii:file==3?*text_resource:title,script,file==1?variant_file:file==2?2:file==3?0:title_file,layer);
    if(!vm){error="title animation creation failed: "+std::to_string(script)+" (ANM opcode "+std::to_string(animations.last_error)+")";return 0;}
    return handles[slot]=vm->id;
}
void TitleMenu::interrupt(u32 id,i32 label,bool immediate){
    auto* vm=animations.find(id);if(!vm)return;
    auto apply=[&](AnmVm& v){v.interrupt(i16(label));if(immediate&&v.update(animations)<0)error="title animation interrupt failed";};
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
void TitleMenu::disable_extra(){if(!unlocked())for(const auto script:{4,kPortTitle==13||kPortTitle==14?14:12})if(auto* vm=animations.find(child(0,script)))vm->interrupt(29);}
void TitleMenu::highlight(i32 slot,i32 base){family(slot,3,true);family(slot,cursor.selected+base);}
void TitleMenu::move(u32 keys,u32 negative,u32 positive){cursor.previous=cursor.selected;if(keys&negative)cursor.move(-1);if(keys&positive)cursor.move(1);}
bool TitleMenu::update(u32 pressed,u32 repeat){
    sounds.clear();text_requests.clear();music_request=-1;music_fade=-1;music_pause=false;fade_requested=false;volume_changed=false;replay_scan_requested=false;
    if((flags&1)&&++music_start_delay>=(kPortTitle==14?10:5)){music_request=0;scores.settings[0x26]=1;flags&=~1u;music_start_delay=0;}
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
#if PORT_TITLE == 14
    case TitleScreen::Manual:manual_th14();break;
#endif
    case TitleScreen::SpellPractice:if constexpr(kPortTitle==14)spell_stage_th14(pressed,repeat);else{error="spell practice unavailable";return false;}break;
    case TitleScreen::SpellRecords:if constexpr(kPortTitle==14)spell_select_th14(pressed,repeat);else{error="spell selection unavailable";return false;}break;
    case TitleScreen::SpellDifficulty:if constexpr(kPortTitle==14)spell_difficulty_th14(pressed,repeat);else{error="spell difficulty unavailable";return false;}break;
    case TitleScreen::Exit:case TitleScreen::Loading:break;
    default:error="title screen not restored: "+std::to_string(i32(screen));return false;
    }
    if constexpr(kPortTitle==13||kPortTitle==14)for(auto& sound:sounds){
        // Native TH13 menu sound calls use 7/9/10 for accept/cancel/move.
        if(sound==10)sound=7;else if(sound==11)sound=9;else if(sound==12)sound=10;
    }
    timer.tick();return error.empty();
}
void TitleMenu::main(u32 pressed,u32 repeat){
    if constexpr(kPortTitle==14){main_th14(pressed,repeat);return;}
    // TH13 44cdb9/44cdc5 start title scripts 89/94; 44ce69 waits
    // 180 frames before creating the menu. Handles retain their owner slots.
    constexpr bool th13=kPortTitle==13;
    constexpr i32 entrance=th13?180:70;
    switch(substate){
    case 0:
        cursor.count=8;if(!unlocked()&&cursor.disabled_count<16)cursor.disabled[cursor.disabled_count++]=1;
        if(selection.flags&0x10){cursor.select(2);selection.flags&=~0x10u;}
        step(1);
        if(flags&2){create(th13?89:79,79);create(th13?94:83,83);flags&=~2u;}
        else {
            if constexpr(th13){if(!exists(79))create(89,79);family(79,2,true);}
            else if(!exists(79)&&!exists(80))create(80);
            if(!exists(83)&&!exists(84))create(th13?95:84,84);
            timer.set(entrance,&animations.rate);
        }
        [[fallthrough]];
    case 1:
        if(timer.current==entrance){create(0);if(!exists(238))create(0,238,1);disable_extra();}
        if(timer.current>entrance+10){step(2);highlight(0,17);disable_extra();}break;
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
            if constexpr(th13)family(79,3,true);else close(79);change(TitleScreen::Difficulty);cursor.push();
            if(selection.last_difficulty>=4)selection.last_difficulty=selection.difficulty=1;
            cursor.select(selection.last_difficulty);selection.difficulty=selection.last_difficulty;break;
        case 1:
            selection.flags&=~0x10u;if constexpr(th13)family(79,3,true);else close(79);change(TitleScreen::Difficulty);cursor.push();
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
    if constexpr(kPortTitle==14){difficulty_th14(pressed,repeat);return;}
    constexpr bool th13=kPortTitle==13;
    const i32 group=(th13?123:117)+(selection.difficulty>=4?1:0);
    switch(substate){
    case 0:
        if constexpr(th13){if(!exists(193))create(19,193,2);}
        else if(!exists(92)){create(92);create(18,193,2);}
        selection.difficulty=selection.last_difficulty;cursor.count=selection.difficulty>=4?1:4;
        close(group);create(group);highlight(group,17);create(th13?101:96,96);step(1);
        [[fallthrough]];
    case 1:if(timer.current>6)step(2);break;
    case 2:
        if(selection.difficulty<4){move(pressed|repeat,16,32);if(cursor.previous!=cursor.selected){sounds.push_back(12);highlight(group);}}
        if(pressed&0x102){step(4);sounds.push_back(11);close(group);return;}
        if(pressed&0x80001){family(group,6);interrupt(child(group,selection.difficulty<4?cursor.selected+(th13?113:107):(th13?117:111)),2);step(3);sounds.push_back(10);}break;
    case 3:
        if(timer.current>=14){close(96);change(TitleScreen::Character);if(selection.difficulty<4)selection.difficulty=cursor.selected;
            selection.last_difficulty=selection.difficulty;cursor.push();cursor.wrap=1;cursor.count=th13?4:2;cursor.select(selection.last_character);selection.character=selection.last_character;}
        break;
    case 4:
        if(timer.current>=6){close(96);close(92);family(193,1);change(TitleScreen::Main);
            selection.last_difficulty=selection.difficulty=selection.difficulty<4?cursor.selected:selection.saved_difficulty;cursor.pop();}
        break;
    }
}
}
