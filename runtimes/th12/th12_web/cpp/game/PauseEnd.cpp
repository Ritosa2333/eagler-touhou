#include "PauseMenu.hpp"
#include <cstring>
namespace th11 {
#if PORT_TITLE == 16
bool PauseMenu::th16_score_begin(){
    if(practice){error="TH16 practice score consumer unavailable";return false;}
    const i32 rank=scores.th16_insert_score(selection,difficulty,u32(result_score),stage==7&&completed?9:stage,continues,timestamp,slowdown,th16_subseason);
    unranked=rank<0;if(unranked)return th16_end_choices();
    cursor.count=25;cursor.wrap=1;cursor.select_th16(rank);name_cursor.select_th16(0);name_cursor.count=91;name_cursor.wrap=1;
    entered_name=scores.th16_last_name;if(std::memcmp(entered_name.data(),"        ",8))name_cursor.move(-1);
    name_length=8;while(name_length>0&&entered_name[name_length-1]==' ')--name_length;
    th16_substate=15;timer.set(0,&animations.rate);visible(false);return true;
}
bool PauseMenu::th16_end_choices(){
    if(practice||completed){error="TH16 compact end choices unavailable";return false;}
    // Own4404ae normal-run branch.46f130 delivers3 immediately before7+selection.
    th16_substate=6;timer.set(0,&animations.rate);cursor.count=5;cursor.wrap=1;
    auto* vm=th16_overlay(front,160,5);if(!vm){error="TH16 end choices creation failed";return false;}menu_animation=vm->id;
    if(continues>0&&!cursor.disable_th16(2))return false;
    if(th16_continues_remaining<=0){if(!cursor.disable_th16(0))return false;cursor.select_th16(1);}else cursor.select_th16(0);
    vm->interrupt(3);if(vm->update(animations)<0){error="TH16 immediate end animation failed";return false;}
    if(!vm->child.previous)for(auto* n=vm->child.next;n;n=n->next){n->value->interrupt(3);if(n->value->update(animations)<0){error="TH16 immediate end child failed";return false;}}
    family(menu_animation,i32(i16(cursor.selected+7)));return true;
}
#endif
#if PORT_TITLE == 14
bool PauseMenu::create_deferred_backdrop_th14(){
 auto& queue=animations.deferred_captures;
 if(!create_backdrop(false))return false;
 auto* vm=animations.find(background_animation);
 if(!vm||vm->resource!=&text||!vm->sprite){error="TH14 deferred text52 sprite missing";return false;}
 const auto& sprite=*vm->sprite;
 DeferredCaptureRequestTh14 request;
 request.file_index=i32(vm->file_index);request.texture_index=i32(sprite.texture);
 request.source={32,16,384,448};
 request.destination={i32(sprite.x),i32(sprite.y),i32(sprite.width),i32(sprite.height)};
 queue.enqueue(request);return true;
}
#endif
void PauseMenu::visible(bool value){
    if(auto* vm=animations.find(menu_animation)){
        if(value)vm->flags|=2;else vm->flags&=~2u;
        if(!vm->child.previous)for(auto* n=vm->child.next;n;n=n->next){if(value)n->value->flags|=2;else n->value->flags&=~2u;}
    }
}
bool PauseMenu::begin_end(bool is_practice,bool cleared){
#if PORT_TITLE == 16
    if(is_practice||cleared){error="TH16 practice/clear end entry unavailable";return false;}
    // Own43f350 non-replay path. State and substate are separate words.
    practice=false;completed=false;transition(2);elapsed.set(0,&animations.rate);th16_substate=2;
    sounds.clear();sounds.push_back(14);action=PauseAction::None;return create_backdrop();
#else
    practice=is_practice;completed=cleared;transition(practice||completed?21:13);elapsed.set(0,&animations.rate);
    sounds.clear();action=PauseAction::None;
#if PORT_TITLE == 14
#if defined(TH14_CPP_STAGE_DECODE)
    //449e0a always for death;449f8a for normal/SpellPr completion.
    //StagePr (including30) defers this until44b22a after capture enqueue.
    const u32 run_flags=th14_live_run_flags?u32(*th14_live_run_flags):
        spell_practice_id>=0?0x20u:practice?0x10u:0u;
    if(!completed||(run_flags&0x30u)==0||(run_flags&0x30u)==0x20u)
        if(auto* control=animations.th14_control.get())control->flags|=0x10u;
#endif
    if(!completed)sounds.push_back(14);
    // Retail449f20 leaves capture absent for a completed SpellPr attempt;
    // 44b16c creates only the compact choices after its ten-frame delay.
    if(spell_practice_id>=0&&completed)return true;
#endif
#if PORT_TITLE == 14
    //449d30 death uses448bf0 ImmediatePause, in all run flag branches.
    //449f20 completion queues only flags0/normal Extra at entry. Completed
    //StagePractice waits for31's80-frame handler; SpellPractice has no copy.
    if(!completed){if(!create_backdrop())return false;}
    else if(!practice){if(!create_deferred_backdrop_th14())return false;}
#else
    if(!create_backdrop())return false;
#endif
    if constexpr(kPortTitle==14){
#if PORT_TITLE == 14
        if(spell_practice_id<0)scores.settings[0x37]=1;
#endif
    }else scores.settings[0x37]=1;return true;
#endif
}
void PauseMenu::show_end_choices(i32 script,i32 next,i32 count){
    const bool compact=count==3;
    if constexpr(kPortTitle==14){script=compact?144:139;count=compact?3:5;next=compact?22:14;}
    auto* vm=animations.create(front,script,kPortTitle==14?0:5,kPortTitle==14?0:29,true);if(!vm){error="game-over menu creation failed";return;}
    if constexpr(kPortTitle==14)vm->flags2&=~0x6000u;
    menu_animation=vm->id;state=next;cursor.count=count;cursor.wrap=1;cursor.disabled_count=0;
    i32 selected=0;
    if constexpr(kPortTitle==14){
        // Retail 44a3ee: saving a replay is unavailable after continuing;
        // 435cef initializes the remaining continues by difficulty.
        if(!compact){
            if(continues>0)cursor.disabled[cursor.disabled_count++]=2;
            constexpr i32 limits[]={3,3,4,5,0};
            if(difficulty<0||difficulty>=5||continues>=limits[difficulty]){cursor.disabled[cursor.disabled_count++]=0;selected=1;}
        }else if(practice&&!completed)selected=2;
    }
    cursor.select(selected);family(menu_animation,3);family(menu_animation,cursor.selected+7);
}
void PauseMenu::end_menu(u32 pressed,u32 repeat){
    const bool practice_states=state>=20;const i32 list=practice_states?23:16,name=practice_states?24:17;
    switch(state){
    case 12:case 20:
        if(timer.current>=10&&(pressed&0x80001)){sounds.push_back(10);family(menu_animation,2);transition(state+1);}break;
    case 13:case 21:
#if PORT_TITLE == 14
        if(practice&&completed&&spell_practice_id<0){
            if(timer.current<80)break;
            if(!create_deferred_backdrop_th14())return;
#if defined(TH14_CPP_STAGE_DECODE)
            if(auto* control=animations.th14_control.get())control->flags|=0x10u; //44b22a, after enqueue.
#endif
        }else
#endif
        if(timer.current<10)break;
        if(!practice){
            const i32 rank=scores.insert_score(selection,difficulty,result_score,stage==7&&completed?8:stage,continues,timestamp,slowdown);
            unranked=rank<0;timer.set(0,&animations.rate);
            if(!unranked){cursor.count=25;cursor.wrap=1;cursor.select(rank);name_begin();state=practice_states?25:18;visible(false);break;}
            show_end_choices(98,14,practice_states?3:4);
        }else{
#if PORT_TITLE == 14
            timer.set(0,&animations.rate);
#endif
            if(selection>=0&&selection<7&&difficulty>=0&&difficulty<5&&stage>=1&&stage<=7
#if PORT_TITLE == 14
               &&spell_practice_id<0
#endif
            ){
                auto* p=scores.characters[selection].data()+ScoreFile::practice_score_offset+(stage+difficulty*ScoreFile::practice_stage_stride)*8;i32 best;std::memcpy(&best,p,4);
                if(best<result_score)std::memcpy(p,&result_score,4);
            }
            show_end_choices(practice_states?(completed?103:104):98,practice_states?22:14,practice_states?3:4);
        }break;
    case 14:case 22:{
        const bool compact=state==22;const i32 save=compact?1:2,back=compact?0:1;
        move(pressed|repeat,7);
        if constexpr(kPortTitle==14)if(cursor.previous!=cursor.selected)sounds.back()=10;
        if(pressed&0x80001){
            choose(cursor.selected+(kPortTitle==14?134:94));sounds.push_back(kPortTitle==14?7:10);transition(compact?26:19);
            if(cursor.selected==save){transition(compact?23:16);visible(false);cursor.push();cursor.count=25;cursor.wrap=1;cursor.select(0);scan_requested=true;}
            else if constexpr(kPortTitle==14){if(!compact&&cursor.selected==3){transition(15);manual_requested=true;}else if(cursor.selected==back||cursor.selected==(compact?2:4))sounds.push_back(7);}
            else if(cursor.selected==back||cursor.selected==(compact?2:3))sounds.push_back(10);
        }
        if(pressed&0x102){sounds.push_back(kPortTitle==14?9:11);if(cursor.selected!=back){cursor.select(back);family(menu_animation,cursor.selected+7);}}
        break;
    }
    case 16:case 23:
        if(timer.current<10)break;
        cursor.previous=cursor.selected;if((pressed|repeat)&16)cursor.move(-1);if((pressed|repeat)&32)cursor.move(1);
        if(cursor.previous!=cursor.selected)sounds.push_back(12);
        if(pressed&0x80001){transition(name);recording_metadata_requested=true;name_begin();sounds.push_back(10);return;}
        if(pressed&0x102){transition(practice_states?22:14);visible(true);cursor.pop();cursor.count=practice_states?3:(kPortTitle==14?5:4);cursor.wrap=1;
            if constexpr(kPortTitle==14)if(!practice_states&&continues>0)cursor.disabled[cursor.disabled_count++]=2;
            family(menu_animation,cursor.selected+7);replay_files.fill(nullptr);sounds.push_back(11);}break;
    case 17:case 24:
        if(timer.current<10)break;
        name_move(pressed|repeat);
        if(pressed&0x80001){const i32 choice=name_cursor.selected,edit=name_confirm();if(edit<0)return;
            if(edit==1){sounds.push_back(44);save_requested=true;transition(list);std::memcpy(scores.settings.data()+12,entered_name.data(),9);return;}
            sounds.push_back(choice==89?11:10);if(choice==89)return;
        }
        if(pressed&0x102){sounds.push_back(11);if(name_length)entered_name[--name_length]=' ';else transition(list);}break;
    case 18:case 25:
        if(timer.current<10)break;
        if(!unranked)name_move(pressed|repeat);
        if(pressed&0x80001){
            if(!unranked){const i32 edit=name_confirm();if(edit<0)return;
                if(edit!=1){sounds.push_back(10);return;}
                scores.score_name(selection,difficulty,cursor.selected,entered_name.data());
            }
            show_end_choices(practice_states?103:98,practice_states?22:14,practice_states?3:4);visible(true);return;
        }
        if(pressed&0x102){if(cursor.selected<0){show_end_choices(practice_states?103:98,practice_states?22:14,practice_states?3:4);visible(true);return;}
            if(name_length){sounds.push_back(11);entered_name[--name_length]=' ';}
        }break;
    case 19:case 26:
        if(timer.current>=12){family(background_animation,1);family(menu_animation,1);transition(27);
            if(practice_states)action=cursor.selected==2?PauseAction::Restart:PauseAction::Title;
            else action=cursor.selected==0?(completed?PauseAction::Restart:PauseAction::Continue):cursor.selected==(kPortTitle==14?4:3)?PauseAction::Restart:PauseAction::Title;
        }break;
    case 15:
#if PORT_TITLE == 14
        if(manual_resource){manual_update(pressed,repeat);if(manual_finished){transition(14);visible(true);}}
        else if(pressed&0x102){manual_requested=false;transition(14);family(menu_animation,cursor.selected+7);sounds.push_back(9);}
#endif
        break;
    }
}
#if PORT_TITLE == 14
// Retail help manager 437c40. PNG delivery is a host boundary; the menu
// bindings, input delays, seven pages and entrance/exit ANMs remain in core.
void PauseMenu::begin_manual(){
    visible(false);manual_requested=false;manual_finished=false;manual_phase=manual_substate=0;
    manual_animations={};manual_cursor={};manual_image_requested=-1;manual_timer.set(0,&animations.rate);
}
bool PauseMenu::manual_create(i32 script){
    const Vec3 origin{manual_origin_x,0,0};const float rotation=0;
    auto* vm=animations.create(*manual_resource,script,19,u32(-1),true,false,&origin,nullptr,nullptr,false,&rotation);
    if(!vm){error="TH14 manual animation creation failed";return false;}
    vm->flags2&=~0x6000u;manual_animations[script]=vm->id;return true;
}
void PauseMenu::manual_choices(){
    for(i32 i=0;i<7;++i)if(manual_create(i))if(auto* vm=animations.find(manual_animations[i]))vm->interrupt(i16(i==manual_cursor.selected?2:3));
}
void PauseMenu::manual_update(u32 pressed,u32 repeat){
    auto label=[&](i32 script,i32 interrupt){family(manual_animations[script],interrupt);};
    auto reset_timer=[&]{manual_timer.set(0,&animations.rate);};
    auto request_page=[&]{manual_substate=2;reset_timer();manual_image_requested=manual_cursor.selected;for(i32 i=0;i<7;++i)label(i,1);};
    if(manual_phase==0)manual_phase=1;
    else if(manual_phase==2){if(manual_timer.current>=30)manual_finished=true;}
    else switch(manual_substate){
    case 0:
        manual_cursor.count=7;manual_cursor.wrap=1;manual_cursor.select(0);manual_choices();manual_substate=1;
        [[fallthrough]];
    case 1:
        if(manual_timer.current<20)break;
        manual_cursor.previous=manual_cursor.selected;
        if((pressed|repeat)&16)manual_cursor.move(-1);if((pressed|repeat)&32)manual_cursor.move(1);
        if(manual_cursor.previous!=manual_cursor.selected){sounds.push_back(10);for(i32 i=0;i<7;++i)label(i,i==manual_cursor.selected?2:3);}
        if(pressed&0x80001){sounds.push_back(7);request_page();}
        else if(pressed&0x102){sounds.push_back(9);for(i32 i=0;i<7;++i)label(i,1);manual_phase=2;manual_substate=0;reset_timer();}
        break;
    case 2:break; // The host installs help_01.png .. help_07.png, then sets 3.
    case 3:
        if(manual_create(9)){manual_substate=4;reset_timer();}
        [[fallthrough]];
    case 4:
        if(manual_timer.current<20)break;
        if((pressed&32)&&manual_cursor.selected<6){manual_substate=5;reset_timer();sounds.push_back(7);manual_cursor.move(1);label(9,7);}
        else if((pressed&16)&&manual_cursor.selected>0){manual_substate=5;reset_timer();sounds.push_back(7);manual_cursor.move(-1);label(9,8);}
        else if(pressed&0x80103){sounds.push_back(9);manual_substate=1;reset_timer();label(9,1);manual_choices();}
        break;
    case 5:if(manual_timer.current>=20)request_page();break;
    }
    manual_timer.tick();
}
#endif
}
