#include "TitlePresentation.hpp"
#include <algorithm>

namespace th12host {
namespace {
bool snow_scene(MenuScene s){return s==MenuScene::Difficulty||s==MenuScene::Character||s==MenuScene::Shot||s==MenuScene::Practice||s==MenuScene::MusicRoom||
    s==MenuScene::Replay||s==MenuScene::ReplayStage||s==MenuScene::PlayerData||s==MenuScene::Name||s==MenuScene::ReplaySave;}
bool popup_scene(MenuScene s){return s==MenuScene::Pause||s==MenuScene::PauseConfirm||s==MenuScene::Continue||s==MenuScene::ResultMenu;}
// The in-game name and save pages, drawn over the paused field.
bool paused_page(const MenuState& s){return (s.scene==MenuScene::Name||s.scene==MenuScene::ReplaySave)&&s.name_context==NameContext::Pause;}
bool popup_page(const MenuState& s){return popup_scene(s.scene)||paused_page(s);}
std::optional<i32> menu_root(MenuScene s){switch(s){case MenuScene::Title:return 0;case MenuScene::Options:return 1;case MenuScene::KeyConfig:return 2;case MenuScene::Difficulty:return 131;case MenuScene::Character:return 143;default:return std::nullopt;}}
std::optional<i32> heading_of(MenuScene s){switch(s){case MenuScene::Difficulty:return 110;case MenuScene::Character:return 111;case MenuScene::Shot:return 112;
    case MenuScene::Replay:case MenuScene::ReplayStage:return 113;case MenuScene::PlayerData:return 114;case MenuScene::MusicRoom:return 115;case MenuScene::ReplaySave:return 117;case MenuScene::Practice:return 118;default:return std::nullopt;}}
std::u32string ascii(const std::string& s){std::u32string out;for(char c:s)out+=char32_t(u8(c));return out;}
std::string pad(i64 v,size_t digits){std::string s=std::to_string(v<0?-v:v);if(s.size()<digits)s=std::string(digits-s.size(),'0')+s;return v<0?"-"+s:s;}
std::string width(i64 v,size_t n){std::string s=std::to_string(v);return s.size()<n?std::string(n-s.size(),' ')+s:s;}
std::string fit(const std::string& s,size_t n){std::string t=s.substr(0,n);t.resize(n,' ');return t;}
std::string fixed1(double v){char b[32];std::snprintf(b,sizeof b,"%.1f",v);return b;}
TextDescriptor ascii_text(const std::string& text,float x,float y,u32 color=0xffffffff){TextDescriptor t;t.text=text;t.font=0;t.x=x;t.y=y;t.color=color;t.draw_flag=1;return t;}
float move_y(float from,float to,i32 age,i32 frames=10){return from+(to-from)*std::min(1.f,std::max(0.f,float(age)/float(frames)));}
constexpr u32 Selected=0xffffff00,Inactive=0xff808080;
// 4aee20 / 4aee38 / 4aee60 / 4aee88
const char* const loadouts[6]{"ReimuA ","ReimuB ","MarisaA","MarisaB","SanaeA ","SanaeB "};
const char* const difficulty_labels[5]{"Easy   ","Normal ","Hard   ","Lunatic","Extra  "};
const char* const stage_labels[9]{"test   ","Stage 1","Stage 2","Stage 3","Stage 4","Stage 5","Stage 6","Extra  ","Clear  "};
const char* const final_stage_labels[9]{"tst","St1","St2","St3","St4","St5","St6","Ex ","All"};
const char* label(const char* const* table,i32 n,i32 i,const char* none){return i>=0&&i<n?table[i]:none;}
// 4a1e78: a TitleInf replay row (25 fixed slots, then user files).
std::string replay_row(const ReplayEntry* entry,i32 slot,bool user,const std::string* name=nullptr,const char* final_label=nullptr){
    if(!entry||!entry->info)return user?"User  -------- --/--/-- --:-- ------- ------- --- ---%":"No."+pad(slot,2)+" -------- --/--/-- --:-- ------- ------- --- ---%";
    const auto& r=*entry->info;const auto t=host::local_time(r.timestamp);
    std::string prefix=user?(entry->file.size()>=11?entry->file.substr(7,4):std::string("User"))+"  ":"No."+pad(slot,2)+" ";
    return prefix+fit(name?*name:r.name,8)+" "+pad(t.year%100,2)+"/"+pad(t.month,2)+"/"+pad(t.day,2)+" "+pad(t.hour,2)+":"+pad(t.minute,2)+" "+
        label(loadouts,6,r.character*2+r.shot,"-------")+" "+label(difficulty_labels,5,r.difficulty,"-------")+" "+(final_label?final_label:label(final_stage_labels,9,r.final_stage,"---"))+" "+fixed1(r.slowdown)+"%";
}
// 43230f..4324f5: the in-game save page's shorter row.
std::string pause_replay_row(const ReplayEntry* entry,i32 slot,bool naming=false){
    if(!entry||!entry->info)return "No."+pad(slot,2)+" -------- --/--/-- ------- - St-";
    const auto& r=*entry->info;const auto t=host::local_time(r.timestamp);const char diff[5]{'E','N','H','L','X'};
    return "No."+pad(slot,2)+" "+(naming?std::string(8,' '):fit(r.name,8))+" "+pad(t.year%100,2)+"/"+pad(t.month,2)+"/"+pad(t.day,2)+" "+
        label(loadouts,6,r.character*2+r.shot,"-------")+" "+std::string(1,r.difficulty>=0&&r.difficulty<5?diff[r.difficulty]:'-')+" "+label(final_stage_labels,9,r.final_stage,"St-");
}
// The run being saved, as a list row (449099: eight spaces and "All").
ReplayEntry result_entry(const MenuContext& r){
    ReplayEntry e;ReplayInfo i;i.timestamp=r.timestamp;i.score_stored=r.score_stored;i.slowdown=r.slowdown;i.character=r.run.character;i.shot=r.run.shot;
    i.difficulty=r.run.mode==ModeExtra?4:r.run.difficulty;i.final_stage=r.clear?8:r.stage;i.continues=r.continues;e.info=i;return e;
}
// 4a2014: a Score Ranking row.
std::string score_row(const Profile::RankingRow& r,i32 index){
    const std::string rank=width(index+1,2),name=fit(r.name,8),score=width(r.score_stored,9)+std::to_string(r.continues);
    if(!r.timestamp)return rank+"  "+name+"  "+score+"  ----/--/-- --:--  Stage -  ---%";
    const auto t=host::local_time(r.timestamp);const i32 stage=r.final_stage==8?8:std::clamp(r.final_stage,1,7);
    return rank+"  "+name+"  "+score+"  "+pad(t.year,4)+"/"+pad(t.month,2)+"/"+pad(t.day,2)+" "+pad(t.hour,2)+":"+pad(t.minute,2)+"  "+stage_labels[stage]+"  "+[&]{std::string v=fixed1(r.slowdown);return v.size()<3?std::string(3-v.size(),' ')+v:v;}()+"%";
}
void score_rows(const std::array<Profile::RankingRow,10>& rows,i32 selected,bool name_entry,bool ranked,std::vector<TextDescriptor>& out){
    for(i32 i=0;i<10;++i){const u32 tint=u32(255-i*16);
        const u32 color=name_entry&&ranked?(i==selected?0xffffffff:0xff404040):(0xff0000ffu|tint<<16|tint<<8);
        out.push_back(ascii_text(score_row(rows[size_t(i)],i),48,float(160+i*18),color));}
}
// 43250c / 432580: the in-game ranking.
void pause_score_rows(const std::array<Profile::RankingRow,10>& rows,i32 selected,bool ranked,std::vector<TextDescriptor>& out){
    out.push_back(ascii_text("            Score Ranking!!",48,64));
    for(i32 i=0;i<10;++i){const auto& r=rows[size_t(i)];
        std::string text=width(i+1,2)+" "+fit(r.name,8)+" "+pad(r.score_stored,9)+std::to_string(r.continues)+" ";
        if(r.timestamp){const auto t=host::local_time(r.timestamp);text+=pad(t.year%100,2)+"/"+pad(t.month,2)+"/"+pad(t.day,2)+" "+stage_labels[r.final_stage==8?8:std::clamp(r.final_stage,1,7)];}
        else text+="--/--/-- Stage -";
        out.push_back(ascii_text(text,48,float(96+i*18),ranked&&i==selected?Selected:Inactive));}
}
// The 91-cell name grid; cells 88..90 are font 0's space, delete and end glyphs.
void name_grid(float x,float y,i32 selection,std::vector<TextDescriptor>& out){
    for(i32 i=0;i<91;++i){const std::string cell=i<88?std::string(1,name_characters[i]):std::string(1,char(i==88?0x81:i==89?0x7f:0x80));
        out.push_back(ascii_text(cell,x+float(i%13*18),y+float(i/13*16),i==selection?Selected:Inactive));}
}
}

std::vector<MusicTrackInfo> parse_music_comments(const std::vector<u8>& bytes){
    // prepare-menu-assets.py music_comments: '@file', 'No.' label, eight lines ('♪' title first).
    const std::u32string text=decode_cp932(std::string(bytes.begin(),bytes.end()));std::vector<std::u32string> lines;std::u32string line;
    for(char32_t c:text){if(c==U'\n'){if(!line.empty()&&line.back()==U'\r')line.pop_back();lines.push_back(line);line.clear();}else line+=c;}
    if(!line.empty())lines.push_back(line);
    std::vector<MusicTrackInfo> out;
    for(size_t at=0;at<lines.size();){
        const auto& l=lines[at++];if(l.empty()||l[0]!=U'@')continue;if(at+9>lines.size())break;
        MusicTrackInfo t;std::string file;for(size_t i=1;i<l.size();++i)file+=char(l[i]&0x7f);
        const auto slash=file.find_last_of('/');t.id=file.substr(slash==std::string::npos?0:slash+1);const auto dot=t.id.find('.');if(dot!=std::string::npos)t.id.erase(dot);
        t.label=lines[at];t.lines.assign(lines.begin()+i64(at+1),lines.begin()+i64(at+9));at+=9;
        if(t.label.rfind(U"No.",0)!=0||t.lines[0].empty()||t.lines[0][0]!=U'♪')continue;
        t.title=t.lines[0].substr(1);out.push_back(std::move(t));
    }
    return out;
}

TitlePresentation::TitlePresentation(Scene& s,Banks& b,Profile& p,std::vector<std::u32string> w):scene(s),banks(b),profile(p),warning(std::move(w)){
    sampler.files=[this](i32 bank){return banks.get(bank);};
    // ascii 20 snow and the menus share their own display random stream.
    sampler.display_random32=[this]{const u32 a=((seed^0x9630)-0x6553)&65535,b=((a<<2)|(a>>14))&65535,c=((b^0x9630)-0x6553)&65535;seed=((c<<2)|(c>>14))&65535;return (a<<16)|c;};
}
bool TitlePresentation::popup(const MenuState& s)const{return popup_page(s);}
MenuAnimation* TitlePresentation::bind(const std::string& key,i32 script,i32 bank){
    remove(key);
    auto vm=std::make_unique<MenuAnimation>(sampler,banks.get(bank),bank,script);auto* raw=vm.get();bindings.emplace_back(key,std::move(vm));return raw;
}
void TitlePresentation::remove(const std::string& key){
    const auto it=std::find_if(bindings.begin(),bindings.end(),[&](const auto& b){return b.first==key;});if(it==bindings.end())return;
    if(!it->second->retiring)it->second->interrupt(1);it->second->retiring=true;retiring.push_back({std::move(it->second),0});bindings.erase(it);
}
void TitlePresentation::preserve_pane(const std::string& key){
    // Map semantics: remove(key), set(key, pane) at the end, delete 'pane'.
    if(!has("pane"))return;remove(key);
    const auto it=std::find_if(bindings.begin(),bindings.end(),[](const auto& b){return b.first=="pane";});
    auto vm=std::move(it->second);bindings.erase(it);bindings.emplace_back(key,std::move(vm));
}
void TitlePresentation::select_backdrop(){
    if(!has("background"))bind("background",99);
    remove("title-intro");remove("version");
    if(!has("select-background"))bind("select-background",107);
}
void TitlePresentation::set_heading(std::optional<i32> script){
    if(heading==script&&has("heading"))return;remove("heading");heading=script;if(script)bind("heading",*script);
}
void TitlePresentation::enter(const MenuState& state,const TitleScenes& scenes){
    const MenuScene previous=current;const bool had=have_scene;serial=state.serial;current=state.scene;have_scene=true;initial_selection=false;pending_popup.reset();
    const auto& run=scenes.run;
    if(snow_scene(state.scene)&&!paused_page(state)){if(!has("snow"))bind("snow",20,BankAscii);}else remove("snow");
    const bool replay_pause=state.context.run.mode==ModeReplay;
    if(state.scene==MenuScene::PauseConfirm&&get("popup")){get("popup")->interrupt(6,true,false,(replay_pause?94:90)+state.pause_selection);return;}
    if(had&&previous==MenuScene::PauseConfirm&&state.scene==MenuScene::Pause&&get("popup")){get("popup")->interrupt(7+state.selection);initial_selection=true;return;}
    if(had&&previous==MenuScene::Name&&(state.scene==MenuScene::Continue||state.scene==MenuScene::ResultMenu)&&has("popup-backdrop")){
        auto* vm=bind("popup",state.scene==MenuScene::Continue?111:state.failed?117:116,BankFront);vm->interrupt(3,true,true);vm->interrupt(7+state.selection,true,true);initial_selection=true;return;}
    if(popup_page(state)&&state.name_context==NameContext::Pause&&has("popup-backdrop")&&had&&
        (previous==MenuScene::Name||previous==MenuScene::Continue||previous==MenuScene::ResultMenu||previous==MenuScene::ReplaySave)){remove("popup");return;}
    if(had&&previous==MenuScene::Difficulty&&state.scene==MenuScene::Character)preserve_pane("difficulty-card");
    else if(had&&previous==MenuScene::Character&&state.scene==MenuScene::Shot)preserve_pane("character-portrait");
    else if(had&&previous==MenuScene::Shot&&state.scene==MenuScene::Practice)preserve_pane("practice-shot");
    else remove("pane");
    if(had&&previous==MenuScene::Practice&&state.scene!=MenuScene::Practice)remove("practice-shot");
    std::vector<std::string> stale;
    for(const auto& [key,vm]:bindings)if(key.rfind("popup",0)==0||key.rfind("track-",0)==0||key.rfind("comment-",0)==0||key.rfind("data-",0)==0||key=="practice-highlight")stale.push_back(key);
    for(const auto& key:stale)remove(key);
    if(state.scene==MenuScene::Title){
        set_heading(std::nullopt);remove("difficulty-card");remove("character-portrait");
        if(!has("background"))bind("background",99);
        intro=state.intro;
        if(state.intro){remove("select-background");if(!has("title-intro"))bind("title-intro",106);}
        else{
            if(!(had&&(previous==MenuScene::Options||previous==MenuScene::KeyConfig)))get("background")->interrupt(2,true,true);
            if(!has("title-intro")&&!has("select-background"))bind("select-background",107);
            bind("pane",0);position_menu_rows(state);bind("version",0,BankTitleV);
        }
    }else if(paused_page(state)){
        set_heading(std::nullopt);bind("popup-backdrop",75,BankText);capture_pause=true;
    }else if(popup_scene(state.scene)){
        const i32 script=state.scene==MenuScene::Continue?111:state.scene==MenuScene::ResultMenu?(state.failed?117:116):replay_pause?103:102;
        MenuAnimation* vm=state.scene==MenuScene::ResultMenu&&state.phase==MenuState::Phase::In?nullptr:bind("popup",script,BankFront);
        if(!vm)pending_popup=script;
        bind("popup-backdrop",75,BankText);capture_pause=true;
        if(vm)vm->interrupt(3,true,true);
    }else if(state.scene==MenuScene::Playing||state.scene==MenuScene::Demo||state.scene==MenuScene::Ending||state.scene==MenuScene::Closed){
        set_heading(std::nullopt);remove("difficulty-card");remove("character-portrait");
    }else{
        if(state.scene!=MenuScene::Options&&state.scene!=MenuScene::KeyConfig)select_backdrop();intro=false;
        if(state.scene==MenuScene::Difficulty){remove("difficulty-card");remove("character-portrait");}
        if(state.scene==MenuScene::Character)remove("character-portrait");
        std::optional<i32> root=state.scene==MenuScene::Difficulty&&run.mode==2?std::optional<i32>(132):menu_root(state.scene);
        if(!root&&state.scene==MenuScene::Shot)root=175+run.character+(run.mode==2?3:0);
        if(root)bind("pane",*root);
        set_heading(state.scene==MenuScene::Name?std::optional<i32>(state.name_kind==NameKind::Replay?117:116):heading_of(state.scene));
        if(state.scene==MenuScene::Difficulty||state.scene==MenuScene::Character||state.scene==MenuScene::Shot){
            MenuEvent e;e.type=MenuEvent::Ready;e.selection_interrupt=17+state.selection;select(state,e);initial_selection=true;}
        if(state.scene==MenuScene::Practice)bind("practice-highlight",120);
        if(state.scene==MenuScene::MusicRoom){
            // 449360 binds heading 115, tracks 211+ and text 41..48 only.
            for(i32 i=0;i<i32(state.choices.size());++i){auto* vm=bind("track-"+std::to_string(i),211+i);vm->set_text(211+i,{track_title(scenes,i),0xffffff,512,18});vm->interrupt(i==state.selection?2:3);}
            for(i32 i=0;i<8;++i)bind("comment-"+std::to_string(i),41+i,BankText);
            music_selection=state.selection;music_comment_key.clear();comment_update.reset();
        }
        if(state.scene==MenuScene::PlayerData)update_data(state,true);
        // 44812a..4481b7: the run's character, shot and difficulty headers.
        if(state.scene==MenuScene::Name&&state.name_kind!=NameKind::Replay){const auto& r=state.result.run;
            for(i32 id:{193+r.character,196+r.shot,198+(r.mode==ModeExtra?4:r.difficulty)})bind("data-header-"+std::to_string(id),id);}
    }
}
void TitlePresentation::select(const MenuState& state,const MenuEvent& event){
    if(auto* popup=get("popup")){
        if(state.scene==MenuScene::PauseConfirm)popup->interrupt(15+state.selection);
        else{
            // 432b45 queues only 7+row; the other rows take their fallback IRQ 3.
            const bool replay=state.context.run.mode==ModeReplay;const i32 row=replay&&state.selection==2?3:state.selection;
            popup->interrupt((event.type==MenuEvent::Ready?17:7)+row);
            for(i32 i=0;i<i32(state.choices.size());++i)if(state.choices[size_t(i)].disabled)
                popup->interrupt(14,true,false,state.scene==MenuScene::Continue?107+i:state.scene==MenuScene::ResultMenu?113+i:replay?94+(i==2?3:i):90+i);
        }
        return;
    }
    auto* pane=get("pane");if(!pane)return;
    pane->interrupt(event.reset_interrupt,true,true);pane->interrupt(event.selection_interrupt);
    position_menu_rows(state);
}
void TitlePresentation::position_menu_rows(const MenuState& state){
    auto* pane=get("pane");if(!pane)return;const MenuScene s=state.scene;
    if(s!=MenuScene::Title&&s!=MenuScene::Options&&s!=MenuScene::KeyConfig)return;
    const i32 count=i32(state.choices.size()),first=s==MenuScene::Title?3:s==MenuScene::Options?19:45,other=s==MenuScene::Title?11:s==MenuScene::Options?24:52;
    // 43ffcb..4401a7 runs the title positioning IRQ before queuing the lock IRQ 29.
    for(i32 i=0;i<count;++i)if(i!=state.selection){const i32 code=i<state.selection?30:31;const bool immediate=s==MenuScene::Title;
        pane->interrupt(code,true,immediate,first+i);pane->interrupt(code,true,immediate,other+i);}
    for(i32 i=0;i<count;++i)if(state.choices[size_t(i)].disabled){const i32 code=s==MenuScene::Title&&i==1?29:27;pane->interrupt(code,true,false,first+i);pane->interrupt(code,true,false,other+i);}
}
void TitlePresentation::confirm_animation(const MenuState& state){
    auto* vm=get("pane");if(!vm)return;const i32 sel=state.selection;
    switch(state.scene){
    case MenuScene::Title:
        vm->interrupt(6);
        if(sel<=5){if(auto* b=get("background"))b->interrupt(3);remove("title-intro");remove("version");if(!has("select-background"))bind("select-background",107);}break;
    case MenuScene::Difficulty:{const bool extra=!state.choices.empty()&&state.choices[0].id==4;vm->interrupt(6);vm->interrupt(2,true,false,extra?125:121+sel);break;}
    case MenuScene::Character:
        for(i32 script:{140+sel,133+sel,95,96})vm->interrupt(6,true,false,script);
        for(i32 i=0;i<3;++i){if(i!=sel)vm->interrupt(1,true,false,133+i);vm->interrupt(1,true,false,136+i);}break;
    case MenuScene::Shot:{const i32 group=run.character+(run.mode==2?3:0);vm->interrupt(6,true,false,144+group*2+sel);break;}
    default:vm->interrupt(6);break;
    }
}
std::u32string TitlePresentation::track_title(const TitleScenes& scenes,i32 index)const{
    const auto& t=scenes.music()[size_t(index)];const bool heard=t.id=="th12_01"||profile.music_heard(t.id);
    return heard?t.label:ascii("No."+std::to_string(index+1)+" ??????");
}
void TitlePresentation::update_music(const MenuState& state,const TitleScenes& scenes){
    const i32 top=state.music_top;
    for(i32 i=0;i<i32(state.choices.size());++i){
        auto* vm=get("track-"+std::to_string(i));if(!vm)continue;const i32 script=211+i;
        vm->set_visible(script,i>=top&&i<top+10);vm->set_position(script,float(i==state.selection?60:64),float(96+(i-top)*20),4);
        vm->set_text(script,{track_title(scenes,i),0xffffff,512,18});
        if(music_selection!=state.selection)vm->interrupt(i==state.selection?2:3);
    }
    music_selection=state.selection;
    const std::string id=!state.comment_track.empty()?state.comment_track:!state.playing_track.empty()?state.playing_track:"th12_01";
    const std::string key=id+":"+std::to_string(state.comment_serial);
    if(music_comment_key!=key){
        music_comment_key=key;comment_lines.clear();u32 color=0xffffff;
        if(state.music_warning){comment_lines=warning;color=0xff8080;}
        else for(const auto& t:scenes.music())if(t.id==id){comment_lines=t.lines;break;}
        comment_update=CommentUpdate{&comment_lines,color,0};
        for(i32 i=0;i<8;++i)if(auto* vm=get("comment-"+std::to_string(i)))vm->interrupt(3);
    }
    if(comment_update){
        auto& u=*comment_update;
        if(u.age%2==0&&u.age<16){const i32 i=u.age/2;if(auto* vm=get("comment-"+std::to_string(i))){
            vm->set_text(41+i,{size_t(i)<u.lines->size()?(*u.lines)[size_t(i)]:std::u32string(),u.color,512,17});vm->interrupt(2);}}
        if(++u.age>=16)comment_update.reset();
    }
}
void TitlePresentation::tick(MenuState& state,const TitleScenes& scenes){
    run=scenes.run;
    if(serial!=state.serial)enter(state,scenes);
    if(state.scene==MenuScene::Title&&intro&&state.age==180){bind("pane",0);position_menu_rows(state);bind("version",0,BankTitleV);}
    if((state.scene==MenuScene::Pause||state.scene==MenuScene::Continue||state.scene==MenuScene::ResultMenu)&&state.age==10&&state.phase==MenuState::Phase::In){
        if(pending_popup){bind("popup",*pending_popup,BankFront)->interrupt(3,true,true);pending_popup.reset();}
        MenuEvent e;e.type=MenuEvent::Selection;e.selection_interrupt=7+state.selection;select(state,e);initial_selection=true;
    }
    if(state.scene==MenuScene::PauseConfirm&&state.age==20)if(auto* p=get("popup"))p->interrupt(14);
    for(const auto& e:state.events){
        if((e.type==MenuEvent::Ready&&!initial_selection)||e.type==MenuEvent::Selection)select(state,e);
        else if(e.type==MenuEvent::Confirm)confirm_animation(state);
        else if(e.type==MenuEvent::Cancel&&state.scene==MenuScene::Difficulty){if(auto* vm=get("pane")){vm->interrupt(1);vm->retiring=true;}}
        else if(e.type==MenuEvent::Resume){if(auto* p=get("popup"))p->interrupt(1);if(auto* b=get("popup-backdrop"))b->interrupt(1);}
        else if(e.type==MenuEvent::RetryShortcut){if(auto* p=get("popup"))p->interrupt(6,true,false,93);}
        else if(e.type==MenuEvent::ConfirmNo){if(auto* p=get("popup"))p->interrupt(6,true,false,101);}
    }
    state.events.clear();
    const auto& run=scenes.run;auto* pane=get("pane");
    if(pane&&state.scene==MenuScene::Difficulty)for(i32 d=0;d<5;++d)pane->set_visible(170+d,profile.cleared(run.character,run.shot,d));
    if(pane&&state.scene==MenuScene::Character)for(i32 c=0;c<3;++c)pane->set_visible(136+c,profile.cleared(c,0,run.difficulty)||profile.cleared(c,1,run.difficulty));
    if(pane&&state.scene==MenuScene::Shot)for(i32 s=0;s<2;++s)pane->set_visible(168+s,profile.cleared(run.character,s,run.difficulty));
    if(pane&&state.scene==MenuScene::Options)for(i32 row=0;row<2;++row){
        const std::string v=pad(row?scenes.settings.se_volume:scenes.settings.bgm_volume,3);
        for(i32 i=0;i<3;++i){const i32 digit=v[size_t(i)]-'0';pane->set_sprite(29+row*8+i,44+digit);pane->set_sprite(33+row*8+i,55+digit);}
    }
    if(pane&&state.scene==MenuScene::KeyConfig)for(i32 row=0;row<5;++row){
        const i32 value=std::max(0,state.joy_draft[size_t(row)]);
        for(i32 i=0;i<2;++i){const i32 digit=i?value%10:value/10;pane->set_sprite(59+row*2+i,44+digit);pane->set_sprite(69+row*2+i,55+digit);}
    }
    if(state.scene==MenuScene::MusicRoom)update_music(state,scenes);
    if(state.scene==MenuScene::PlayerData)update_data(state);
    for(auto& [key,vm]:bindings)vm->tick();
    for(auto& r:retiring){r.vm->tick();++r.age;}
    retiring.erase(std::remove_if(retiring.begin(),retiring.end(),[](Retiring& r){return !r.vm->alive()||r.age>=120;}),retiring.end());
}
void TitlePresentation::update_data(const MenuState& state,bool force){
    const i32 c=state.selection/2,shot=state.selection%2,d=state.data_difficulty,page=state.data_page;
    const std::string key=std::to_string(c)+":"+std::to_string(shot)+":"+std::to_string(d)+":"+std::to_string(page);
    if(!force&&key==data_key)return;data_key=key;
    const std::string header=std::to_string(c)+":"+std::to_string(shot)+":"+std::to_string(d);
    if(force||header!=data_header_key){
        std::vector<std::string> stale;for(const auto& [k,vm]:bindings)if(k.rfind("data-header-",0)==0)stale.push_back(k);for(const auto& k:stale)remove(k);
        // 4471c3..44739f: one character, shot and difficulty header, then the frame.
        for(i32 id:{193+c,196+shot,198+d,206,207,208,209,203,204,205,210})bind("data-header-"+std::to_string(id),id);
        data_header_key=header;}
    std::vector<std::string> keep;
    if(page>0){
        // 447b20: "No.%3d %s %4d/%4d", the title padded to 42 bytes; captures
        // of this loadout tint it, all-player attempts reveal it.
        const auto history=profile.spell_history();const auto cards=spell_page_cards(d,page);
        for(size_t row=0;row<cards.size();++row){
            const i32 card=cards[row],player=c*2+shot,attempts=history[size_t(card*14+player*2)],captures=history[size_t(card*14+player*2+1)],any=history[size_t(card*14+12)];
            std::string title=profile.spell_title(card,player);if(title.empty())title=profile.spell_title(card,6);
            for(i32 p=0;p<6&&title.empty();++p)title=profile.spell_title(card,p);
            std::string bytes=any>0?title+std::string(title.size()<42?42-title.size():0,' '):std::string();
            std::u32string text=ascii("No."+width(card+1,3)+" ");
            text+=any>0?decode_cp932(bytes):std::u32string(21,U'？');
            text+=ascii(" "+width(captures,4)+"/"+width(attempts,4));
            const u32 color=any>0?(captures>0?0xffff80:0xefefef):0x808080;const i32 script=25+i32(row);
            const std::string name="data-row-"+std::to_string(script);keep.push_back(name);
            MenuAnimation* vm=get(name);if(!vm)vm=bind(name,script,BankText);vm->set_text(script,{text,color,512,17});
        }
    }
    std::vector<std::string> stale;for(const auto& [k,vm]:bindings)if(k.rfind("data-row-",0)==0&&std::find(keep.begin(),keep.end(),k)==keep.end())stale.push_back(k);
    for(const auto& k:stale)remove(k);
}
void TitlePresentation::texts(const MenuState& state,const TitleScenes& scenes,std::vector<TextDescriptor>& out)const{
    const auto& run=scenes.run;const auto& files=scenes.replays();
    const auto entry=[&](i32 i)->const ReplayEntry*{return i>=0&&size_t(i)<files.size()?&files[size_t(i)]:nullptr;};
    switch(state.scene){
    case MenuScene::Practice:{
        if(state.phase==MenuState::Phase::In||(state.phase==MenuState::Phase::Out&&state.transition_action=="cancel"))break;
        // nativePracticeDescriptors: six rows of 'Stage N  best score'.
        const bool confirming=state.phase==MenuState::Phase::Out;const i32 age=confirming?state.transition_age:std::max(0,state.age-state.ready_at+1);
        if(age>=10||confirming)for(i32 i=0;i<6;++i){
            const bool unlocked=size_t(i)<state.choices.size()&&!state.choices[size_t(i)].disabled;
            const u32 color=i!=state.selection?Inactive:!unlocked?0xffdfdfdf:confirming&&age%4>=2?0xff000000:Selected;
            out.push_back(ascii_text(std::string(stage_labels[i+1])+"  "+(unlocked?pad(profile.practice_score(run.character,run.shot,run.difficulty,i+1),8)+"0":"---------"),296,float(216+i*18),color));
        }
        break;}
    case MenuScene::Replay:{
        const i32 page=state.selection/25,row=state.selection%25;
        for(i32 i=0;i<25;++i)out.push_back(ascii_text(replay_row(entry(page*25+i),i+1,page>0),58,float(80+i*15),i==row?Selected:Inactive));
        break;}
    case MenuScene::ReplayStage:{
        if(!state.replay)break;const auto& r=*state.replay;
        out.push_back(ascii_text(replay_row(entry(state.file_index),state.file_index%25+1,state.file_index>=25),58,move_y(float(80+(state.file_index%25)*15),80,state.age)));
        if(state.age<10)break;
        // 446930: the next recorded stage's starting score, else the final one.
        for(i32 n=1;n<=7;++n){
            const ReplayStage* stage=r.stage(n);const ReplayStage* next=n<6?r.stage(n+1):nullptr;
            const i32 score=next?next->initial.score:r.info.score_stored,digit=next?next->initial.globals[0]:r.info.continues;
            out.push_back(ascii_text(std::string(stage_labels[n])+"  "+(stage?pad(score,8)+std::to_string(digit):"---------"),220,float(128+(n-1)*18),state.selection==n-1?Selected:Inactive));
        }
        break;}
    case MenuScene::ReplaySave:
        for(i32 i=0;i<25;++i){
            if(state.name_context==NameContext::Pause)out.push_back(ascii_text(pause_replay_row(entry(i),i+1),48,float(64+i*15),i==state.selection?Selected:Inactive));
            else out.push_back(ascii_text(replay_row(entry(i),i+1,false),58,float(80+i*15),i==state.selection?Selected:Inactive));
        }
        break;
    case MenuScene::PlayerData:{
        const i32 c=state.selection/2,shot=state.selection%2,d=state.data_difficulty;
        if(state.data_page==0)score_rows(profile.rankings(c,shot,d),-1,false,true,out);
        // 447fd1 / 44804d / 448082: totals, play time from 60 Hz frames.
        const auto t=profile.totals(c,shot,d);const u32 seconds=t.play_frames/60;
        out.push_back(ascii_text("    "+width(t.play_count,5),328,378));
        out.push_back(ascii_text(width(seconds/3600,3)+":"+pad(seconds/60%60,2)+":"+pad(seconds%60,2),328,396));
        out.push_back(ascii_text("    "+width(t.clear_count,5),328,414));
        break;}
    case MenuScene::Name:{
        const bool pause=state.name_context==NameContext::Pause;
        if(state.name_kind==NameKind::Replay){
            const auto e=result_entry(state.result);const std::string blank(8,' ');const i32 cursor=i32(std::min<size_t>(7,state.name.size()));
            if(pause){
                out.push_back(ascii_text(pause_replay_row(&e,state.slot,true),48,move_y(float(64+(state.slot-1)*15),224,state.age),Selected));
                if(state.age<10)break;
                out.push_back(ascii_text(fit(state.name,8),102,224));out.push_back(ascii_text("_",float(102+9*cursor),224,Selected));name_grid(112,320,state.selection,out);
            }else{
                out.push_back(ascii_text(replay_row(&e,state.slot,false,&blank,"All"),58,move_y(float(80+(state.slot-1)*15),240,state.age)));
                if(state.age<10)break;
                out.push_back(ascii_text(fit(state.name,8),112,240));out.push_back(ascii_text("_",float(112+9*cursor),240,Selected));name_grid(212,360,state.selection,out);
            }
        }else{
            if(state.age<state.reveal_at)break;
            if(pause)pause_score_rows(state.ranking,state.rank,state.ranked,out);else score_rows(state.ranking,state.rank,true,state.ranked,out);
            // 448848 / 432163: the name over its ranking row, then the grid.
            if(!state.ranked)break;
            const float x=pause?75:84,y=float((pause?96:160)+18*std::max(0,state.rank));const i32 length=i32(std::min<size_t>(state.name.size(),8));
            out.push_back(ascii_text(fit(state.name,8),x,y));out.push_back(ascii_text("_",x+float(9*std::min(length,7)),y,Selected));
            name_grid(pause?112:212,pause?320:360,state.selection,out);
        }
        break;}
    default:break;
    }
}
void TitlePresentation::render(const MenuState& state,const TitleScenes& scenes,float fps){
    std::vector<Appearance> poses;std::vector<TextDescriptor> text;
    const bool pop=popup_page(state);
    if(pop){if(auto* p=get("popup"))p->poses(poses);}
    else if(state.scene!=MenuScene::Closed){for(const auto& [key,vm]:bindings)vm->poses(poses);for(const auto& r:retiring)r.vm->poses(poses);}
    texts(state,scenes,text);
    if(pop){std::vector<Appearance> backdrop;if(auto* b=get("popup-backdrop"))b->poses(backdrop);scene.render_pause_backdrop(backdrop,capture_pause);capture_pause=false;}
    scene.render_menu(poses,0,0,pop,0,nullptr,{.035f,.035f,.055f,1},&text,fps);
    const bool fade=state.phase==MenuState::Phase::Out&&state.transition_action!="cancel"&&(state.scene==MenuScene::Practice||state.scene==MenuScene::ReplayStage||(state.scene==MenuScene::Shot&&scenes.run.mode!=ModePractice));
    const i32 fade_at=state.scene==MenuScene::ReplayStage?2:10;
    if(fade&&state.transition_age>=fade_at)scene.render_menu({},0,0,true,std::min(1.f,float(state.transition_age-fade_at)/32.f));
}
}
