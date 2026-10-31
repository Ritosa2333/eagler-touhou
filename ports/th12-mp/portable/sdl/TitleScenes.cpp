#include "TitleScenes.hpp"
#include <algorithm>

namespace th12host {
const char name_characters[89]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=.,!?@:;[]()_/{}|~^#$%&*";
namespace {
constexpr u32 Confirm=0x80001,Cancel=0x102,Directions=0xf0,RetryKey=0x200000;
std::u32string ascii(const char* s){std::u32string out;for(;*s;++s)out+=char32_t(u8(*s));return out;}
const char32_t* const characters[3]{U"博麗 霊夢",U"霧雨 魔理沙",U"東風谷 早苗"};
// Title+ demo order (4205c0): demo2, demo1, demo0.
const char* const demos[3]{"demo2.rpy","demo1.rpy","demo0.rpy"};
// Read-only 4af208: each card's difficulty (113 cards).
i32 card_difficulty(i32 id){return id>=100?4:id<2?id+2:id>=54&&id<=56?id-53:id>=93&&id<=95?id-92:id<54?(id-2)%4:id<77?(id-57)%4:id<93?(id-77)%4:(id-96)%4;}
}
i32 spell_page_count(i32 difficulty){i32 n=0;for(i32 id=0;id<113;++id)if(card_difficulty(id)==difficulty)++n;return 1+(n+9)/10;}
std::vector<i32> spell_page_cards(i32 difficulty,i32 page){
    std::vector<i32> ids;if(page<=0)return ids;for(i32 id=0;id<113;++id)if(card_difficulty(id)==difficulty)ids.push_back(id);
    const size_t from=size_t(page-1)*10;if(from>=ids.size())return {};return std::vector<i32>(ids.begin()+i64(from),ids.begin()+i64(std::min(ids.size(),from+10)));
}

TitleScenes::TitleScenes(Profile& p,std::vector<MusicTrackInfo> t,std::string f):profile(p),tracks(std::move(t)),folder(std::move(f)){
    settings.bgm_volume=profile.bgm_volume();settings.se_volume=profile.se_volume();settings.joy_buttons=profile.joy_buttons();
    rescan_replays();open_title(true);
}
void TitleScenes::rescan_replays(){replay_files=list_replays(folder);if(state.scene==MenuScene::Replay||state.scene==MenuScene::ReplaySave)refresh_choices();}
void TitleScenes::refresh_choices(){
    auto& s=state;s.choices.clear();const auto add=[&](std::u32string label,i32 id,bool disabled=false){s.choices.push_back({std::move(label),id,disabled});};
    switch(s.scene){
    case MenuScene::Title:{
        const char* labels[8]{"Start","Extra Start","Practice Start","Replay","Player Data","Music Room","Option","Quit"};
        for(i32 i=0;i<8;++i)add(ascii(labels[i]),i,i==1&&!profile.any_extra());break;}
    case MenuScene::Difficulty:
        if(run.mode==ModeExtra)add(U"Extra",4);else{const char* d[4]{"Easy","Normal","Hard","Lunatic"};for(i32 i=0;i<4;++i)add(ascii(d[i]),i);}break;
    case MenuScene::Character:for(i32 i=0;i<3;++i)add(characters[i],i,run.mode==ModeExtra&&!profile.can_extra(i,0)&&!profile.can_extra(i,1));break;
    case MenuScene::Shot:for(i32 i=0;i<2;++i)add(i?U"B":U"A",i,run.mode==ModeExtra&&!profile.can_extra(run.character,i));break;
    case MenuScene::Practice:for(i32 i=1;i<=6;++i)add(ascii(("Stage "+std::to_string(i)).c_str()),i,!profile.can_practice(run.character,run.shot,run.difficulty,i));break;
    case MenuScene::Replay:for(size_t i=0;i<replay_files.size();++i)add(ascii(replay_files[i].file.c_str()),i32(i),!replay_files[i].info);break;
    case MenuScene::ReplayStage:for(i32 i=0;i<7;++i)add(i==6?U"Extra":ascii(("Stage "+std::to_string(i+1)).c_str()),i+1,!s.replay||!s.replay->stage(i+1));break;
    case MenuScene::PlayerData:for(i32 i=0;i<6;++i)add(characters[i/2],i);break;
    case MenuScene::MusicRoom:for(size_t i=0;i<tracks.size();++i)add(tracks[i].label,i32(i));break;
    case MenuScene::Options:for(const char* v:{"BGM Volume","SE Volume","Key Config","Default","Quit"})add(ascii(v),i32(s.choices.size()));break;
    case MenuScene::KeyConfig:for(const char* v:{"Shoot","Bomb","Focus","Pause","Skip","Default","Quit"})add(ascii(v),i32(s.choices.size()));break;
    case MenuScene::Quit:add(U"Return",0);add(U"Quit",1);break;
    // Pause ids: 0 resume, 1 title, 2 save-exit, 3 retry, 4 replay again.
    case MenuScene::Pause:
        add(U"Return to Game",0);add(U"Return to Title",1);
        if(s.context.run.mode==ModeReplay)add(U"Replay Again",4);else{add(U"Save Replay and Quit",2,!s.context.can_save_replay);add(U"Retry",3);}break;
    case MenuScene::PauseConfirm:add(U"Yes",0);add(U"No",1);break;
    // Continue ids: 0 continue, 1 title, 2 save replay, 3 retry.
    case MenuScene::Continue:add(U"Continue",0,!s.context.can_continue);add(U"Return to Title",1);add(U"Save Replay",2,!s.context.can_save_replay);add(U"Retry",3);break;
    case MenuScene::ResultMenu:add(U"Return to Title",1);add(U"Save Replay",2,!s.context.can_save_replay);add(U"Retry",3);break;
    // Name ids: 0..87 characters, 88 space, 89 delete, 90 end.
    case MenuScene::Name:
        if(!s.ranked&&s.name_kind==NameKind::Score)add(U"Next",90);
        else{for(i32 i=0;i<88;++i)add(std::u32string(1,char32_t(name_characters[i])),i);add(U"Space",88);add(U"Delete",89);add(U"End",90);}break;
    case MenuScene::ReplaySave:for(i32 i=0;i<25;++i)add(ascii(("No."+std::to_string(i+1)).c_str()),i+1);break;
    default:break;
    }
    if(!s.choices.empty())s.selection=std::clamp(s.selection,0,i32(s.choices.size())-1);
    if(s.scene==MenuScene::MusicRoom)scroll_music();
    s.page=s.selection/(s.scene==MenuScene::MusicRoom?8:25);
}
void TitleScenes::enter(MenuScene scene,i32 selection,std::function<void(MenuState&)> values){
    const MenuScene from=state.scene;const i32 serial=state.serial+1;pending.reset();
    MenuState next;next.scene=scene;next.from=from;next.selection=selection;next.serial=serial;next.joy_draft=settings.joy_buttons;
    if(values)values(next);
    state=std::move(next);refresh_choices();
}
void TitleScenes::open_title(bool intro){
    enter(MenuScene::Title,remember.title,[&](MenuState& s){s.intro=intro;s.age=intro?0:180;s.ready_at=191;});idle_frames=0;
    menu_music("th12_01");
}
void TitleScenes::open_pause(const MenuContext& c){enter(MenuScene::Pause,0,[&](MenuState& s){s.context=c;s.ready_at=11;});simple(MenuAction::Pause);sound(14);}
// upstream scoreRanking: native insertion puts an equal score ahead.
void TitleScenes::score_ranking(MenuState& s,const MenuContext& result,bool eligible)const{
    const i32 d=result.run.mode==ModeExtra?4:result.run.difficulty;
    const auto rows=profile.rankings(result.run.character,result.run.shot,d);
    s.rank=eligible?profile.ranking_place(result.run.character,result.run.shot,d,result.score()):-1;s.ranked=s.rank>=0;
    i32 at=0;for(i32 r=0;r<10;++r){
        if(r==s.rank){auto& row=s.ranking[size_t(r)];row={};row.score_stored=result.score_stored;row.continues=result.continues;row.name="        ";
            row.final_stage=result.clear?8:result.stage;row.timestamp=result.timestamp;row.slowdown=result.slowdown;}
        else s.ranking[size_t(r)]=rows[size_t(at++)];
    }
}
void TitleScenes::open_continue(const MenuContext& c){
    if(c.run.mode==ModePractice||c.run.mode==ModeReplay||c.run.mode==ModeDemo){MenuContext r=c;r.clear=false;finish(r);return;}
    const bool eligible=c.ranked&&c.continues==0;
    // 4337ef..433827: the score music, the ranking/name page, then continue.
    simple(MenuAction::StopEffects);menu_music("th10_17",true);
    MenuState s;score_ranking(s,c,eligible);
    // 433710 reveals the table after ten UI ticks and takes input after another ten.
    enter(MenuScene::Name,s.ranked?90:0,[&](MenuState& n){n.result=c;n.context=c;n.name=profile.last_name().substr(0,8);n.name_kind=NameKind::Score;n.name_context=NameContext::Pause;
        n.after_name=AfterName::Continue;n.rank=s.rank;n.ranked=s.ranked;n.ranking=s.ranking;n.reveal_at=10;n.ready_at=21;});
    result_action(c,s.ranked);
}
void TitleScenes::finish(const MenuContext& result){
    const i32 mode=result.multiplayer?ModeStory:result.run.mode;
    if(mode==ModeDemo||mode==ModeReplay){return_title();return;}
    if(mode==ModePractice){
        // Native 433870, Practice results.
        simple(MenuAction::StopEffects);menu_music("th10_17",true);open_result_menu(result);result_action(result,false);return;
    }
    if(mode==ModeExtra&&!result.clear){MenuContext c=result;c.can_continue=false;open_continue(c);return;}
    const bool extra_clear=mode==ModeExtra&&result.clear;
    if(extra_clear)simple(MenuAction::Pause); // 432850 freezes gameplay before the score music
    simple(MenuAction::StopEffects);menu_music("th10_17",true);
    const bool eligible=result.ranked&&result.continues==0;
    if(!eligible&&!result.clear){result_action(result,false);open_replay_save(result,profile.last_name(),NameContext::Title);return;}
    MenuState s;score_ranking(s,result,eligible);
    enter(MenuScene::Name,s.ranked?90:0,[&](MenuState& n){n.result=result;n.name=profile.last_name().substr(0,8);n.name_kind=NameKind::Score;
        n.name_context=extra_clear?NameContext::Pause:NameContext::Title;n.after_name=extra_clear?AfterName::ResultMenu:AfterName::ReplaySave;
        n.rank=s.rank;n.ranked=s.ranked;n.ranking=s.ranking;if(extra_clear){n.context=result;n.reveal_at=10;n.ready_at=21;}});
    result_action(result,s.ranked);
}
void TitleScenes::open_result_menu(const MenuContext& result,bool from_name,NameContext context){
    enter(MenuScene::ResultMenu,0,[&](MenuState& s){s.context=result;s.result=result;s.failed=!result.clear;s.name_context=context;
        s.ready_at=from_name?0:11;s.phase=from_name?MenuState::Phase::Active:MenuState::Phase::In;});
    if(from_name){MenuEvent e;e.type=MenuEvent::Selection;state.events.push_back(e);}
}
void TitleScenes::open_replay_save(const MenuContext& result,const std::string& name,NameContext context){
    if(!result.can_save_replay){return_title();return;}
    enter(MenuScene::ReplaySave,remember.replay_save,[&](MenuState& s){s.result=result;s.name=name;s.name_context=context;s.ready_at=context==NameContext::Pause?11:7;});
}
void TitleScenes::return_title(){simple(MenuAction::ReturnTitle);open_title(false);}
void TitleScenes::start(){MenuAction a;a.type=MenuAction::Start;a.run=run;enter(MenuScene::Playing,0,[](MenuState& s){s.ready_at=0;});emit(a);}
void TitleScenes::wait(i32 frames,std::function<void()> callback,const std::string& action){
    state.phase=MenuState::Phase::Out;state.transition_age=0;state.transition_action=action;pending=Pending{frames,std::move(callback)};
}
void TitleScenes::animate(MenuEvent::Type type){MenuEvent e;e.type=type;e.selection=state.selection;state.events.push_back(e);}
bool TitleScenes::select(i32 index,bool initial){
    auto& s=state;if(s.choices.empty())return false;index=std::clamp(index,0,i32(s.choices.size())-1);
    if(index==s.selection||(s.choices[size_t(index)].disabled&&s.scene!=MenuScene::Practice&&s.scene!=MenuScene::Replay))return false;
    const i32 previous=s.selection;s.selection=index;s.page=index/(s.scene==MenuScene::MusicRoom?8:25);
    if(s.scene==MenuScene::MusicRoom)scroll_music();
    MenuEvent e;e.type=MenuEvent::Selection;e.previous=previous;e.selection=index;e.reset_interrupt=3;e.selection_interrupt=(initial?17:7)+index;s.events.push_back(e);
    sound(10);return true;
}
void TitleScenes::move(i32 delta){
    auto& s=state;const i32 count=i32(s.choices.size());if(!count)return;i32 next=s.selection;
    if(s.scene==MenuScene::Replay){const i32 page=next/25;select(page*25+((next%25+delta+25)%25));return;}
    for(i32 i=0;i<count;++i){next=(next+delta+count)%count;if(!s.choices[size_t(next)].disabled||s.scene==MenuScene::Practice){select(next);break;}}
}
void TitleScenes::scroll_music(){
    auto& s=state;s.music_top=std::max(0,s.music_top);
    if(s.selection<s.music_top)s.music_top=s.selection;if(s.selection>=s.music_top+10)s.music_top=s.selection-9;
}
void TitleScenes::settings_changed(){MenuAction a;a.type=MenuAction::Settings;emit(a);}
bool TitleScenes::capture_button(i32 button){
    if(state.scene!=MenuScene::KeyConfig||state.capture_joy<0||button<0||button>31)return false;
    state.joy_draft[size_t(state.capture_joy)]=button;state.capture_joy=-1;return true;
}
void TitleScenes::tick(u32 held,u32 pulses){
    auto& s=state;++s.age;
    const u32 pressed=(held&~previous_held)|pulses;previous_held=held;u32 repeat=0;
    for(u32 bit=0;bit<32;++bit){const u32 mask=1u<<bit;if(held&mask){const i32 age=++hold_ages[bit];if(age>=26&&(age-26)%8==0)repeat|=mask;}else hold_ages[bit]=0;}
    if(pending){
        ++s.transition_age;
        const bool loading_fade=s.transition_action=="confirm"&&((s.scene==MenuScene::Shot&&run.mode!=ModePractice)||s.scene==MenuScene::Practice||s.scene==MenuScene::ReplayStage);
        if(loading_fade&&s.transition_age==(s.scene==MenuScene::ReplayStage?2:10)){MenuEvent e;e.type=MenuEvent::Fade;e.frames=32;s.events.push_back(e);simple(MenuAction::LoadingIndicator);}
        if(--pending->frames<=0){auto callback=std::move(pending->callback);pending.reset();callback();}return;
    }
    if(s.scene==MenuScene::Demo){
        // 422af3: any key, or a minute, ends the demo (fade from 3540).
        if((held&0x80103)||s.age>=3600)return_title();
        return;
    }
    // The title controller is absent during gameplay: Z and X belong to the game.
    if(s.scene==MenuScene::Playing||s.scene==MenuScene::Ending||s.scene==MenuScene::Closed)return;
#ifndef TH12_MULTIPLAYER
    if(s.scene==MenuScene::Title){
        idle_frames=(held&0xffff)?0:idle_frames+1;
        if(idle_frames>=1800){
            const std::string id=demos[demo_index];demo_index=(demo_index+1)%3;
            enter(MenuScene::Demo,0,[](MenuState& st){st.ready_at=0;});MenuAction a;a.type=MenuAction::Demo;a.run.replay_file=id;emit(a);return;
        }
    }
#endif
    if(!ready())return;
    if(s.capture_joy>=0){if(pressed&Cancel)s.capture_joy=-1;return;}
    if(s.phase==MenuState::Phase::In){s.phase=MenuState::Phase::Active;MenuEvent e;e.type=MenuEvent::Ready;e.selection=s.selection;e.reset_interrupt=3;e.selection_interrupt=17+s.selection;s.events.push_back(e);}
    if(s.scene==MenuScene::Pause&&(pressed&RetryKey)&&quick_retry())return;
    const u32 direction=(pressed|repeat)&Directions;
    if(s.scene==MenuScene::Name&&!s.ranked&&s.name_kind==NameKind::Score){
        // The unqualified score shows all ten rows, with no character grid.
    }else if(s.scene==MenuScene::Name){
        const i32 n=i32(s.choices.size());
        if(direction&16)select((s.selection-13+n)%n);if(direction&32)select((s.selection+13)%n);
        if(direction&64)select(s.selection/13*13+(s.selection%13+12)%13);if(direction&128)select(s.selection/13*13+(s.selection%13+1)%13);
    }else if(s.scene==MenuScene::PlayerData){
        const auto difficulty=[&](i32 delta){s.data_difficulty=(s.data_difficulty+delta+5)%5;s.data_page=0;animate(MenuEvent::DataHeader);sound(10);};
        if(direction&16)difficulty(-1);if(direction&32)difficulty(1);
        if(direction&64)move(-1);if(direction&128)move(1);
    }else{
        if(direction&16)move(-1);if(direction&32)move(1);
        if(direction&64)horizontal(-1);if(direction&128)horizontal(1);
    }
    if(pressed&Cancel)cancel();else if(pressed&Confirm)confirm();
}
void TitleScenes::horizontal(i32 delta){
    auto& s=state;
    if(s.scene==MenuScene::Options&&s.selection<2){i32& v=s.selection==0?settings.bgm_volume:settings.se_volume;v=std::clamp(v+delta*5,0,100);settings_changed();}
    else if(s.scene==MenuScene::Replay)select((s.selection+delta*25+75)%75);
    else if(s.scene==MenuScene::MusicRoom)move(delta);
}
void TitleScenes::resume(){animate(MenuEvent::Resume);wait(12,[this]{enter(MenuScene::Playing,0,[](MenuState& s){s.ready_at=0;});simple(MenuAction::Resume);});}
bool TitleScenes::quick_retry(){
    auto& s=state;if(s.scene!=MenuScene::Pause||s.context.run.mode==ModeReplay||!ready())return false;
    // 432c5f..432ccf bypasses confirmation (IRQ6 on front93); 4335fe waits 12 ticks.
    sound(7);animate(MenuEvent::RetryShortcut);const MenuContext c=s.context;wait(12,[this,c]{pause_action("retry",c);},"retry-shortcut");return true;
}
void TitleScenes::return_from_pause_confirm(){
    const MenuContext c=state.context;const i32 selection=state.pause_selection;animate(MenuEvent::ConfirmNo);
    wait(20,[this,c,selection]{enter(MenuScene::Pause,selection,[&](MenuState& s){s.context=c;s.ready_at=0;s.phase=MenuState::Phase::Active;});});
}
void TitleScenes::cancel(){
    auto& s=state;if(s.scene==MenuScene::Playing||s.scene==MenuScene::Ending||s.scene==MenuScene::Demo)return;
    sound(9);
    switch(s.scene){
    case MenuScene::Title:
#ifndef TH12_MULTIPLAYER
        if(s.selection==7)confirm();else select(7);
#endif
        break;
    case MenuScene::Difficulty:animate(MenuEvent::Cancel);wait(6,[this]{open_title();});break;
    case MenuScene::Character:animate(MenuEvent::Cancel);wait(6,[this]{enter(MenuScene::Difficulty,run.mode==ModeExtra?0:run.difficulty);});break;
    case MenuScene::Shot:animate(MenuEvent::Cancel);wait(6,[this]{enter(MenuScene::Character,run.character);});break;
    case MenuScene::Practice:animate(MenuEvent::Cancel);wait(6,[this]{enter(MenuScene::Shot,run.shot);},"cancel");break;
    case MenuScene::ReplayStage:enter(MenuScene::Replay,remember.replay);break;
    case MenuScene::Options:if(s.selection==4){animate(MenuEvent::Confirm);wait(10,[this]{return_title();});}else select(4);break;
    case MenuScene::KeyConfig:if(s.selection==6){animate(MenuEvent::Confirm);wait(10,[this]{enter(MenuScene::Options,2);});}else select(6);break;
    case MenuScene::Pause:resume();break;
    case MenuScene::PauseConfirm:if(s.selection==0)select(1);else return_from_pause_confirm();break;
    case MenuScene::Continue:case MenuScene::ResultMenu:return_title();break;
    case MenuScene::Name:
        if(!s.ranked&&s.name_kind==NameKind::Score)complete_name();
        else if(!s.name.empty())s.name.pop_back();
        else if(s.name_kind==NameKind::Replay){const MenuContext r=s.result;const std::string n=s.name;const NameContext c=s.name_context;
            enter(MenuScene::ReplaySave,s.slot-1,[&](MenuState& st){st.result=r;st.name=n;st.name_context=c;});}
        break;
    case MenuScene::ReplaySave:animate(MenuEvent::Cancel);wait(10,[this]{return_title();});break;
    default:return_title();
    }
}
void TitleScenes::confirm(){
    auto& s=state;if(s.scene==MenuScene::Playing||s.scene==MenuScene::Ending||s.scene==MenuScene::Demo)return;
    const MenuChoice* item=s.selection>=0&&size_t(s.selection)<s.choices.size()?&s.choices[size_t(s.selection)]:nullptr;
    if(item&&item->disabled){sound(17);return;}
#ifdef TH12_MULTIPLAYER
    // Room previews allow navigation and Options, as in TH15's room title.
    if(s.scene==MenuScene::Title&&s.selection!=6)return;
#endif
    // 449b61..449c72 changes the Music Room track without the menu OK sound;
    // finishing a replay name plays 18 instead.
    if(s.scene!=MenuScene::MusicRoom&&!(s.scene==MenuScene::Name&&s.name_kind==NameKind::Replay&&item&&item->id==90))sound(7);
    switch(s.scene){
    case MenuScene::Title:{
        remember.title=s.selection;MenuEvent e;e.type=MenuEvent::Confirm;e.selection=s.selection;s.events.push_back(e);const i32 selected=s.selection;
        wait(20,[this,selected]{
            if(selected==0||selected==2){run.mode=selected==2?ModePractice:ModeStory;enter(MenuScene::Difficulty,remember.difficulty);}
            else if(selected==1){run.mode=ModeExtra;run.difficulty=4;run.stage=7;enter(MenuScene::Difficulty,0);}
            else if(selected==3){rescan_replays();enter(MenuScene::Replay,remember.replay);}
            else if(selected==4)enter(MenuScene::PlayerData,0,[](MenuState& st){st.data_page=0;st.data_difficulty=1;});
            else if(selected==5)enter(MenuScene::MusicRoom,remember.music);
            else if(selected==6)enter(MenuScene::Options,0);
            else{enter(MenuScene::Closed,0,[](MenuState& st){st.ready_at=0;});simple(MenuAction::Quit);}
        });break;}
    case MenuScene::Difficulty:animate(MenuEvent::Confirm);run.difficulty=item->id;if(run.mode!=ModeExtra)remember.difficulty=s.selection;wait(14,[this]{enter(MenuScene::Character,remember.character);});break;
    case MenuScene::Character:animate(MenuEvent::Confirm);run.character=s.selection;remember.character=s.selection;
        wait(14,[this]{enter(MenuScene::Shot,remember.shot);if(!state.choices.empty()&&state.choices[size_t(state.selection)].disabled)move(1);});break;
    case MenuScene::Shot:animate(MenuEvent::Confirm);run.shot=s.selection;remember.shot=s.selection;
        if(run.mode==ModePractice)wait(10,[this]{enter(MenuScene::Practice,remember.practice,[](MenuState& st){st.ready_at=11;});});
        else{MenuAction a;a.type=MenuAction::FadeMusic;a.seconds=1;emit(a);run.stage=run.mode==ModeExtra?7:1;wait(40,[this]{start();});}break;
    case MenuScene::Practice:if(item){animate(MenuEvent::Confirm);MenuAction a;a.type=MenuAction::FadeMusic;a.seconds=6;emit(a);run.stage=item->id;remember.practice=s.selection;wait(40,[this]{start();});}break;
    case MenuScene::Replay:if(item){
        auto replay=std::make_shared<Replay>();std::vector<u8> bytes;std::string why;const auto& entry=replay_files[size_t(s.selection)];
        if(!host::read_file(folder+"/"+entry.file,bytes,64u*1024*1024)||!parse_replay(bytes,*replay,why)){host::log("replay %s: %s",entry.file.c_str(),why.c_str());sound(17);break;}
        remember.replay=s.selection;const i32 file_index=s.selection,first=replay->stages.front().number;
        enter(MenuScene::ReplayStage,first-1,[&](MenuState& st){st.replay=replay;st.file_index=file_index;st.ready_at=15;});}break;
    case MenuScene::ReplayStage:if(item){
        animate(MenuEvent::Confirm);MenuAction f;f.type=MenuAction::FadeMusic;f.seconds=6;emit(f);
        const std::string file=replay_files[size_t(s.file_index)].file;const i32 stage=item->id;
        wait(32,[this,file,stage]{enter(MenuScene::Playing,0,[](MenuState& st){st.ready_at=0;});MenuAction a;a.type=MenuAction::PlayReplay;a.run.replay_file=file;a.stage=stage;emit(a);});}break;
    case MenuScene::PlayerData:s.data_page=(s.data_page+1)%spell_page_count(s.data_difficulty);animate(MenuEvent::DataHeader);break;
    case MenuScene::MusicRoom:if(item){
        remember.music=s.selection;const std::string id=tracks[size_t(item->id)].id;const bool heard=id=="th12_01"||profile.music_heard(id);
        // 449b61..449bbe warns once before playing an unheard track.
        s.comment_track=id;++s.comment_serial;
        if(!heard&&!s.music_warning){s.music_warning=true;MenuAction a;a.type=MenuAction::MusicWarning;a.track=id;emit(a);break;}
        s.music_warning=false;menu_music(id,false,true);s.playing_track=id;}break;
    case MenuScene::Options:
        if(s.selection==2){animate(MenuEvent::Confirm);wait(10,[this]{enter(MenuScene::KeyConfig,0);});}
        else if(s.selection==3){settings.bgm_volume=100;settings.se_volume=80;settings_changed();}
        else if(s.selection==4){animate(MenuEvent::Confirm);wait(10,[this]{return_title();});}break;
    case MenuScene::KeyConfig:
        if(s.selection<5){s.capture_joy=s.selection;s.capture_ready=false;}
        else if(s.selection==5)s.joy_draft={0,1,2,4,3};
        else{settings.joy_buttons=s.joy_draft;settings_changed();animate(MenuEvent::Confirm);wait(10,[this]{enter(MenuScene::Options,2);});}break;
    case MenuScene::Quit:if(item&&item->id==1){enter(MenuScene::Closed,0,[](MenuState& st){st.ready_at=0;});simple(MenuAction::Quit);}else open_title();break;
    case MenuScene::Pause:if(item&&item->id==0)resume();
        else{const MenuContext c=s.context;const i32 sel=s.selection;const std::string action=!item?"title":item->id==3?"retry":item->id==4?"replay-again":item->id==2?"save-exit":"title";
            enter(MenuScene::PauseConfirm,1,[&](MenuState& st){st.context=c;st.pause_selection=sel;st.pause_action=action;st.ready_at=30;});}break;
    case MenuScene::PauseConfirm:if(item&&item->id==1)return_from_pause_confirm();else{const MenuContext c=s.context;pause_action(s.pause_action,c);}break;
    case MenuScene::Continue:
        if(item&&item->id==0){MenuAction a;a.type=MenuAction::Continue;a.run=s.context.run;enter(MenuScene::Playing,0,[](MenuState& st){st.ready_at=0;});emit(a);}
        else if(item&&item->id==3){const MenuContext c=s.context;pause_action("retry",c);}
        else if(item&&item->id==2){const MenuContext c=s.context;open_replay_save(c,profile.last_name(),NameContext::Pause);}
        else return_title();break;
    case MenuScene::ResultMenu:
        if(item&&item->id==3){const MenuContext c=s.context;pause_action("retry",c);}
        else if(item&&item->id==2){const MenuContext c=s.context;open_replay_save(c,profile.last_name(),s.name_context);}
        else return_title();break;
    case MenuScene::Name:
        if((!s.ranked&&s.name_kind==NameKind::Score)||(item&&item->id==90))complete_name();
        else if(item&&item->id==89){if(!s.name.empty())s.name.pop_back();}
        else if(item){if(s.name.size()>=8)s.name.resize(7);s.name+=item->id==88?' ':name_characters[item->id];if(s.name.size()>=8)select(90);}break;
    case MenuScene::ReplaySave:if(item){
        remember.replay_save=s.selection;const MenuContext r=s.result;const std::string n=s.name.empty()?profile.last_name():s.name;const NameContext c=s.name_context;const i32 slot=item->id;
        enter(MenuScene::Name,90,[&](MenuState& st){st.name_kind=NameKind::Replay;st.name_context=c;st.slot=slot;st.result=r;st.name=n.substr(0,8);st.ready_at=c==NameContext::Pause?11:7;});}break;
    default:break;
    }
}
void TitleScenes::pause_action(const std::string& action,const MenuContext& context){
    if(action=="replay-again"){
        MenuAction a;a.type=MenuAction::PlayReplay;a.run.replay_file=run.replay_file;a.stage=run.stage;enter(MenuScene::Playing,0,[](MenuState& s){s.ready_at=0;});emit(a);
    }else if(action=="retry"){
        run=context.run;run.stage=run.mode==ModePractice?context.run.stage:run.mode==ModeExtra?7:1;
        MenuAction a;a.type=MenuAction::Retry;a.run=run;enter(MenuScene::Playing,0,[](MenuState& s){s.ready_at=0;});emit(a);
    }else if(action=="save-exit")simple(MenuAction::SaveExit);
    else return_title();
}
void TitleScenes::complete_name(){
    const MenuState s=state;std::string name=s.name;if(name.find_first_not_of(' ')==std::string::npos)name="        ";
    if(s.name_kind==NameKind::Replay){
        sound(18);MenuAction a;a.type=MenuAction::SaveReplay;a.slot=s.slot;a.name=name;a.result=s.result;emit(a);
        enter(MenuScene::ReplaySave,s.slot-1,[&](MenuState& st){st.result=s.result;st.name=s.name;st.name_context=s.name_context;st.ready_at=s.name_context==NameContext::Pause?11:7;});return;
    }
    if(s.ranked){MenuAction a;a.type=MenuAction::SaveName;a.name=name;a.result=s.result;emit(a);}
    if(s.after_name==AfterName::Continue){
        const MenuContext c=s.context;const i32 selection=c.can_continue?0:1;
        enter(MenuScene::Continue,selection,[&](MenuState& st){st.context=c;st.result=s.result;st.name_context=s.name_context;st.ready_at=0;st.phase=MenuState::Phase::Active;});
        // Native creates front111 and sends IRQ3 then IRQ7 in this same tick.
        MenuEvent e;e.type=MenuEvent::Selection;e.previous=selection;e.selection=selection;e.selection_interrupt=7+selection;state.events.push_back(e);
    }else if(s.after_name==AfterName::ResultMenu)open_result_menu(s.result,true,s.name_context);
    else open_replay_save(s.result,s.name,NameContext::Title);
}
}
