#include "TitleMenu.hpp"
#include "SpellMenuTh14.hpp"
#include <cstdio>
namespace th11 {
#if PORT_TITLE == 14
namespace {
// Clear counts are read directly from the retail TH14 CR record.
bool cleared(const ScoreFile& scores,i32 character,i32 partner,i32 difficulty){
    if(character<0||character>=3||partner<0||partner>=2||difficulty<0||difficulty>=5)return false;
    u32 count=0;std::memcpy(&count,scores.characters[character*2+partner].data()+ScoreFile::clear_count_offset+difficulty*4,4);return count!=0;
}
}
// Native 45a0f0: ten rows, a 180-frame entrance, and relative row interrupts.
// Slots are the native script-indexed handles (+2c8), including variant slot 306.
void TitleMenu::main_th14(u32 pressed,u32 repeat){
    auto arrange=[&]{
        for(i32 row=0;row<10;++row)if(row!=cursor.selected){const i32 label=row<cursor.selected?30:31;
            interrupt(child(0,row+3),label,true);interrupt(child(0,row+13),label,true);
        }
        disable_extra();
    };
    switch(substate){
    case 0:
        cursor.count=10;cursor.wrap=1;if(!unlocked()&&cursor.disabled_count<16)cursor.disabled[cursor.disabled_count++]=1;
        if(selection.flags&32)cursor.select(3);else if(selection.flags&16)cursor.select(2);
        step(1);
        if(flags&2){create(86);create(91);flags&=~2u;}
        else{
            if(!exists(86)){create(86);family(86,2,true);}
            if(!exists(91)){create(91);family(91,2,true);}
            family(86,2,true);timer.set(180,&animations.rate);
        }
        [[fallthrough]];
    case 1:
        if(timer.current==180){create(0);if(!exists(306))create(0,306,1);arrange();}
        if(timer.current>190){step(2);highlight(0,17);arrange();}break;
    case 2:
        move(pressed|repeat,16,32);
        if(cursor.previous!=cursor.selected){sounds.push_back(12);highlight(0);arrange();}
        if(pressed&0x102){
            sounds.push_back(11);
            if(cursor.selected==9){step(4);return;}
            cursor.select(9);highlight(0);arrange();
        }
        if(pressed&0x80001){
            family(0,6);sounds.push_back(cursor.selected==9?11:10);
            if(cursor.selected!=7&&cursor.selected!=9){close(91);family(306,1);}
            step(4);
        }break;
    case 4:
        if(timer.current<20)break;
        switch(cursor.selected){
        case 0:case 2:
            selection.flags=(selection.flags&~48u)|(cursor.selected==2?16u:0u);family(86,3,true);
            change(TitleScreen::Difficulty);cursor.push();
            if(selection.last_difficulty>=4)selection.last_difficulty=selection.difficulty=1;
            cursor.select(selection.last_difficulty);selection.difficulty=selection.last_difficulty;break;
        case 1:
            selection.flags&=~48u;family(86,3,true);change(TitleScreen::Difficulty);cursor.push();
            selection.saved_difficulty=selection.difficulty;selection.last_difficulty=selection.difficulty=4;cursor.select(0);break;
        case 3:
            selection.flags=(selection.flags&~48u)|32;family(86,3,true);change(TitleScreen::SpellPractice);cursor.push();secondary.count=6;secondary.wrap=1;cursor.select(0);break;
        case 4:family(86,3,true);change(TitleScreen::Replays);cursor.push();break;
        case 5:family(86,3,true);change(TitleScreen::Records);cursor.push();break;
        case 6:family(86,3,true);change(TitleScreen::Music);cursor.push();break;
        case 7:change(TitleScreen::Options);cursor.push();break;
        case 8:family(86,3,true);change(TitleScreen::Manual);cursor.push();break;
        case 9:change(TitleScreen::Exit);break;
        }break;
    }
}
void TitleMenu::manual_th14(){
    // Full461d10 uses a separate help manager; title input does not control
    // its cursor. Its priority10 callback supplies the finished boundary.
    switch(substate){
    case 0:
        if(!exists(261))create(19,261,2);
        create(103);manual_requested=true;manual_finished=false;manual_origin_x=128.f;step(1);break;
    case 1:
        if(manual_finished){close(261);close(103);change(TitleScreen::Main);manual_release_requested=true;}
        break;
    }
}
// Retail 45d5d0: difficulty is clamped, with both horizontal and vertical keys.
void TitleMenu::difficulty_th14(u32 pressed,u32 repeat){
    const i32 group=114+(selection.difficulty>=4);
    switch(substate){
    case 0:
        if(!exists(261))create(19,261,2);
        selection.difficulty=selection.last_difficulty;cursor.count=selection.difficulty>=4?1:4;cursor.wrap=0;
        close(group);create(group);highlight(group,13);create(93);step(1);
        for(i32 d=selection.difficulty>=4?4:0;d<(selection.difficulty>=4?5:4);++d){
            bool all=true;for(i32 c=0;c<3;++c)for(i32 p=0;p<2;++p)all=all&&cleared(scores,c,p,d);
            if(!all)hide(child(group,133+d));
        }
        [[fallthrough]];
    case 1:if(timer.current>6)step(2);break;
    case 2:
        if(selection.difficulty<4){move(pressed|repeat,16|64,32|128);if(cursor.previous!=cursor.selected){sounds.push_back(12);highlight(group);}}
        if(pressed&0x102){step(4);sounds.push_back(11);close(group);return;}
        if(pressed&0x80001){family(group,6);interrupt(child(group,selection.difficulty<4?104+cursor.selected:108),2);step(3);sounds.push_back(10);}break;
    case 3:
        if(timer.current>=14){close(93);change(TitleScreen::Character);if(selection.difficulty<4)selection.difficulty=cursor.selected;
            selection.last_difficulty=selection.difficulty;cursor.push();cursor.wrap=1;cursor.count=3;
            cursor.select(selection.last_character);selection.character=selection.last_character;}
        break;
    case 4:
        if(timer.current>=6){close(93);close(261);change(TitleScreen::Main);
            selection.last_difficulty=selection.difficulty=selection.difficulty<4?cursor.selected:selection.saved_difficulty;cursor.pop();}
        break;
    }
}
// Retail 45dba0: three characters, with two independent directional transitions.
void TitleMenu::character_th14(u32 pressed,u32 repeat){
    const i32 group=selection.difficulty==4?129:125;
    switch(substate){
    case 0:
        cursor.count=3;
        if(selection.difficulty==4){
            for(i32 c=0;c<3;++c)if(!available(c,0)&&!available(c,1)&&cursor.disabled_count<16)cursor.disabled[cursor.disabled_count++]=c;
            if(!available(cursor.selected,0)&&!available(cursor.selected,1))for(i32 c=0;c<3;++c)if(available(c,0)||available(c,1)){cursor.select(c);break;}
        }
        create(94);if(!exists(group)){close(group);create(group);highlight(group);}step(1);
        for(i32 c=0;c<3;++c)if(!cleared(scores,c,0,selection.difficulty)||!cleared(scores,c,1,selection.difficulty))hide(child(group,130+c));
        selection.last_partner=0;
        [[fallthrough]];
    case 1:if(timer.current>6)step(2);break;
    case 2:
        cursor.previous=cursor.selected;
        if((pressed|repeat)&64){sounds.push_back(12);family(group,cursor.selected+25,true);cursor.move(-1);family(group,cursor.selected+13);}
        if((pressed|repeat)&128){sounds.push_back(12);family(group,cursor.selected+19,true);cursor.move(1);family(group,cursor.selected+7);}
        if(pressed&0x102){step(4);sounds.push_back(11);return;}
        if(pressed&0x80001){for(const auto script:{116+cursor.selected,140+cursor.selected,82,83})interrupt(child(group,script),6);sounds.push_back(10);step(3);}break;
    case 3:
        if(timer.current>=14){change(TitleScreen::Partner);selection.character=selection.last_character=cursor.selected;
            cursor.push();cursor.wrap=1;cursor.count=2;cursor.select(selection.last_partner);close(group);close(94);}
        break;
    case 4:
        if(timer.current>=6){close(group);close(94);change(TitleScreen::Difficulty);selection.character=selection.last_character=cursor.selected;cursor.pop();}break;
    }
}
// Retail 45e1c0: two equipment choices for each of the three characters.
void TitleMenu::partner_th14(u32 pressed,u32 repeat){
    const i32 group=126+selection.character;
    switch(substate){
    case 0:
        cursor.count=2;
        if(selection.difficulty==4)for(i32 p=0;p<2;++p)if(!available(selection.character,p)){
            if(cursor.selected==p)cursor.select(1-p);if(cursor.disabled_count<16)cursor.disabled[cursor.disabled_count++]=p;
        }
        close(group);create(group);highlight(group);step(1);create(95);
        for(i32 p=0;p<2;++p)if(!cleared(scores,selection.character,p,selection.difficulty))hide(child(group,138+p));
        [[fallthrough]];
    case 1:if(timer.current>6)step(2);break;
    case 2:
        cursor.previous=cursor.selected;
        if((pressed|repeat)&64){sounds.push_back(12);family(group,cursor.selected+25,true);cursor.move(-1);family(group,cursor.selected+13);}
        if((pressed|repeat)&128){sounds.push_back(12);family(group,cursor.selected+19,true);cursor.move(1);family(group,cursor.selected+7);}
        if(pressed&0x102){step(4);sounds.push_back(11);return;}
        if(pressed&0x80001){for(const auto script:{119+cursor.selected,146+selection.character*2+cursor.selected,82,83})interrupt(child(group,script),6);
            sounds.push_back(10);step(3);sounds.push_back(50);}
        break;
    case 3:
        if(timer.current==10){
            if(!(selection.flags&48))fade_requested=true;
            else{selection.last_partner=selection.partner;selection.partner=cursor.selected;cursor.push();close(95);change(TitleScreen::Practice);}
        }
        if(timer.current>=40){selection.partner=selection.last_partner=cursor.selected;cursor.push();change(TitleScreen::Exit);
            selection.stage=selection.difficulty<4?1:7;start_requested=true;}
        break;
    case 4:
        if(timer.current>=6){selection.partner=selection.last_partner=cursor.selected;close(group);close(95);change(TitleScreen::Character);cursor.pop();}break;
    }
}
// Retail462c00 and463020: stage and six-loadout selectors precede spell choice.
void TitleMenu::spell_character_th14(u32 pressed,u32 repeat){
    secondary.previous=secondary.selected;
    if((pressed|repeat)&64){sounds.push_back(12);family(259,secondary.selected+25,true);secondary.move(-1);family(259,secondary.selected+13,true);}
    if((pressed|repeat)&128){sounds.push_back(12);family(259,secondary.selected+19,true);secondary.move(1);family(259,secondary.selected+7,true);}
    if(secondary.previous!=secondary.selected&&(screen==TitleScreen::SpellRecords||screen==TitleScreen::SpellDifficulty)){spell_choices_th14();if(screen==TitleScreen::SpellDifficulty)spell_highlight_th14();}
}
void TitleMenu::spell_stage_th14(u32 pressed,u32 repeat){
    auto enter_spells=[&]{close(102);selection.spell_stage=cursor.selected;change(TitleScreen::SpellRecords);cursor.push();cursor.select(0);};
    switch(substate){
    case 0:
        if(!exists(261))create(19,261,2);cursor.count=7;
        if(!exists(259))create(259);if(!exists(198))create(198);step(1);
        if(selection.resume_spell_stage>=0){cursor.select(selection.resume_spell_stage);selection.resume_spell_stage=-1;secondary.count=6;secondary.select(selection.character*2+selection.partner);
            family(198,3,true);family(198,cursor.selected+7,true);family(198,6,true);family(259,3,true);family(259,secondary.selected+7);enter_spells();return;}
        create(102);[[fallthrough]];
    case 1:
        if(timer.current>10){step(2);highlight(198);family(259,3,true);family(259,secondary.selected+7);}break;
    case 2:
        move(pressed|repeat,16,32);if(cursor.previous!=cursor.selected){sounds.push_back(12);highlight(198);}
        spell_character_th14(pressed,repeat);
        if(pressed&0x102){step(4);sounds.push_back(11);selection.last_practice=cursor.selected;return;}
        if(pressed&0x80001){family(198,6);step(3);sounds.push_back(10);selection.last_practice=cursor.selected;}break;
    case 3:if(timer.current>=20)enter_spells();break;
    case 4:
        if(timer.current>=6){close(102);close(198);close(259);change(TitleScreen::Main);cursor.pop();close(261);}break;
    }
}
void TitleMenu::spell_hide_th14(i32 slot){
    auto* vm=animations.find(handles[slot]);if(!vm)return;vm->flags2|=0x20;
    if(!vm->child.previous)for(auto* n=vm->child.next;n;n=n->next)n->value->flags2|=0x20;
}
bool TitleMenu::spell_seen_th14(i32 stage,i32 row)const{
    for(i32 id:th14_spell_choices[stage][row]){if(id<0)break;u32 seen;std::memcpy(&seen,scores.characters[6].data()+ScoreFile::spell_offset+id*ScoreFile::spell_stride+0x88,4);if(seen)return true;}return false;
}
void TitleMenu::spell_choices_th14(){
    const i32 stage=selection.spell_stage,row=screen==TitleScreen::SpellRecords?cursor.selected:selection.spell_row;
    spell_choices.fill(-2);for(i32 n=240;n<247;++n)spell_hide_th14(n);
    for(i32 n=0;n<5;++n){if(!exists(262+n)){create(193+n,262+n);if(auto* vm=animations.find(handles[262+n])){vm->flags2|=0x200;vm->position={};}}family(262+n,2,true);text_requests.push_back({handles[262+n],0x808080,0,0,0," Nothing ... ",false,true});}
    if(stage!=6)for(i32 n=240;n<244;++n){create(n);family(n,3,true);}
    i32 last=0;
    for(i32 id:th14_spell_choices[stage][row]){
        if(id<0)break;const i32 difficulty=th14_spell_difficulty(id),slot=stage==6?0:difficulty;last=std::max(last,slot);
        if(stage==6)create(244);
        const auto* aggregate=scores.characters[6].data()+ScoreFile::spell_offset+id*ScoreFile::spell_stride;
        const auto* own=scores.characters[secondary.selected].data()+ScoreFile::spell_offset+id*ScoreFile::spell_stride;
        u32 seen,captures;std::memcpy(&seen,aggregate+0x88,4);std::memcpy(&captures,own+0x84,4);char line[512];u32 color;
        if(!seen){spell_choices[slot]=-1;std::string unknown;for(i32 n=0;n<11;++n)unknown+="\x81\x48";std::snprintf(line,sizeof(line)," No.%3d  %s",id+1,unknown.c_str());color=0xb0b0b0;}
        else{spell_choices[slot]=id;size_t length=0;while(length<128&&aggregate[length])++length;std::string name(reinterpret_cast<const char*>(aggregate),length);if(name.size()<42)name.resize(42,' ');std::snprintf(line,sizeof(line)," No.%3d  %s",id+1,name.c_str());color=captures?0xffff80:0xefefef;}
        text_requests.push_back({handles[262+slot],color,0,0,0,line,false,true});family(262+slot,2,true);
    }
    // Native463600 resets text rows to interrupt3 after refreshing their texture.
    for(i32 n=0;n<(stage==6?2:5);++n)family(262+n,3,true);
    for(i32 n=last+1;n<5;++n){spell_hide_th14(262+n);handles[262+n]=0;}
}
void TitleMenu::spell_select_th14(u32 pressed,u32 repeat){
    switch(substate){
    case 0:
        cursor.count=th14_spell_counts[selection.spell_stage];if(!exists(96))create(96);if(!exists(199))create(199);step(1);
        for(i32 n=0;n<13-cursor.count;++n)family(199,39-n,true);
        if(selection.resume_spell_row>=0){cursor.select(selection.resume_spell_row);selection.resume_spell_row=-1;spell_choices_th14();family(199,3,true);family(199,cursor.selected+7,true);family(199,6,true);selection.spell_row=cursor.selected;change(TitleScreen::SpellDifficulty);cursor.push();return;}
        spell_choices_th14();
        [[fallthrough]];
    case 1:
        if(timer.current>10){step(2);highlight(199);}break;
    case 2:
        move(pressed|repeat,16,32);if(cursor.previous!=cursor.selected){sounds.push_back(12);highlight(199);spell_choices_th14();}
        spell_character_th14(pressed,repeat);
        if(pressed&0x102){step(4);sounds.push_back(11);return;}
        if(pressed&0x80001){if(!spell_seen_th14(selection.spell_stage,cursor.selected)){sounds.push_back(16);return;}family(199,6);step(3);sounds.push_back(10);}break;
    case 3:
        if(timer.current>=14){selection.spell_row=cursor.selected;change(TitleScreen::SpellDifficulty);cursor.push();}break;
    case 4:
        if(timer.current>=6){for(i32 n=262;n<267;++n)close(n);for(i32 n=240;n<247;++n)spell_hide_th14(n);close(96);close(199);close(198);change(TitleScreen::SpellPractice);cursor.pop();}break;
    }
}
void TitleMenu::spell_highlight_th14(){
    for(i32 n=0;n<5;++n){family(262+n,n==cursor.selected?2:3);const i32 id=spell_choices[n];if(id>=0)family(240+th14_spell_difficulty(id),n==cursor.selected?2:3,true);}
}
void TitleMenu::spell_difficulty_th14(u32 pressed,u32 repeat){
    switch(substate){
    case 0:
        cursor.count=5;for(i32 n=0;n<5;++n)if(spell_choices[n]<0&&cursor.disabled_count<16)cursor.disabled[cursor.disabled_count++]=n;
        // Retail41e820 skips disabled selections, like the later cursor helper.
        cursor.select_th16(0);if(selection.resume_spell_difficulty>=0){cursor.select_th16(selection.resume_spell_difficulty);selection.resume_spell_difficulty=-1;}spell_highlight_th14();step(1);[[fallthrough]];
    case 1:if(timer.current>10)step(2);break;
    case 2:
        move(pressed|repeat,16,32);if(cursor.previous!=cursor.selected){sounds.push_back(12);spell_highlight_th14();}spell_character_th14(pressed,repeat);
        if(pressed&0x102){step(4);sounds.push_back(11);return;}
        if(pressed&0x80001){family(262+cursor.selected,6);step(3);sounds.push_back(10);music_fade=0;sounds.push_back(50);}break;
    case 3:
        if(timer.current==10)fade_requested=true;
        if(timer.current>=40){const i32 id=spell_choices[cursor.selected];if(id<0||id>=120){error="invalid spell practice choice";return;}selection.spell_id=id;selection.stage=selection.spell_stage+1;selection.character=secondary.selected/2;selection.partner=secondary.selected%2;selection.difficulty=th14_spell_difficulty(id);selection.resume_spell_stage=selection.spell_stage;selection.resume_spell_row=selection.spell_row;selection.resume_spell_difficulty=cursor.selected;cursor.push();change(TitleScreen::Exit);start_requested=true;}break;
    case 4:if(timer.current>=6){close(199);change(TitleScreen::SpellRecords);cursor.pop();}break;
    }
}
#endif
}
