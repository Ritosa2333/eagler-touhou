#include "replay_menu.hpp"
#include "replay_slots.hpp"
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
using M=soew::ReplayMenu;
using A=soew::ReplayMenuAction;
using S=soew::ReplayMenuScreen;
using O=soew::ReplaySaveOrigin;
using E=soew::ReplayMenuEntry;
using K=soew::ReplayNameEditor;
using Slot=soew::ReplaySlotState;
unsigned checks=0;

void check(bool okay){
    ++checks;
    if(!okay)throw std::runtime_error("Replay menu check "+std::to_string(checks)+" failed");
}
struct Harness {
    M menu;
    soew::ReplayMenuCatalog catalog;
    soew::ReplayMenuCommand press(unsigned keys){menu.update(0,catalog);return menu.update(keys,catalog);}
    void key(unsigned keys){check(press(keys).action==A::None);}
    void naming_end(){
        check(menu.name_editor().selected_key()==0);
        key(M::up);key(M::left);key(M::confirm);
        check(menu.screen()==S::SaveSlots);
    }
};

void browser(){
    Harness h;auto &m=h.menu;auto &c=h.catalog;
    c.add_browse({false,1},false,false,false);
    c.add_browse({false,99},false,false,true);
    c.add_browse({false,12},false,true,true);
    c.add_browse({false,24},true,true,true); // Extra practice stays in Practice.
    m.begin_browse();check(m.active()&&m.screen()==S::Browse&&m.browse_column()==0&&m.browse_entry(c)==E{false,1});
    check(m.update(M::confirm,c).action==A::None&&m.screen()==S::Browse); // Held on modal entry.
    h.key(M::confirm);check(m.screen()==S::Browse&&m.error()=="INVALID REPLAY");
    check(m.update(M::down,c).action==A::None&&m.browse_index(0)==0);
    h.key(M::down|M::confirm);check(m.screen()==S::Browse&&m.browse_entry(c)==E{false,99}&&m.error().empty());
    h.key(M::right);check(m.browse_column()==1&&m.browse_entry(c)==E{false,12});
    h.key(M::right);check(m.browse_column()==2&&m.browse_entry(c)==E{false,24});
    h.key(M::right);check(m.browse_column()==0&&m.browse_entry(c)==E{false,99});
    check(h.press(M::toggle_recording|M::confirm).action==A::ToggleRecording&&m.screen()==S::Browse);
    check(m.update(M::toggle_recording,c).action==A::None);
    check(h.press(M::toggle_recording).action==A::ToggleRecording&&m.browse_entry(c)==E{false,99});
    h.key(M::confirm);check(m.screen()==S::Details&&m.selected_entry()==E{false,99});
    h.key(M::toggle_recording);check(m.screen()==S::Details);
    check(m.update(M::confirm,c).action==A::None&&m.screen()==S::Details);
    h.key(M::cancel|M::confirm);check(m.screen()==S::Browse&&m.browse_entry(c)==E{false,99});
    h.key(M::confirm);
    const auto play=h.press(M::confirm);check(play.action==A::Play&&play.entry==E{false,99}&&play.stage==-1&&m.screen()==S::Starting);
    check(h.press(M::cancel).action==A::CancelPlay&&m.screen()==S::Details);
    m.set_playback_stages((1u<<1)|(1u<<4));
    h.key(M::right);check(m.playback_stage()==1);
    h.key(M::down);check(m.playback_stage()==4);
    h.key(M::right);check(m.playback_stage()==-1);
    h.key(M::left);check(m.playback_stage()==4);
    check(h.press(M::confirm).stage==4&&m.screen()==S::Starting);
    check(m.complete_play(false,"SEEK MISMATCH").action==A::None&&m.screen()==S::Details&&m.error()=="SEEK MISMATCH");
    check(h.press(M::confirm).action==A::Play);
    check(m.complete_play(true).action==A::Started&&!m.active());
    check(m.complete_play(true).action==A::None);
    m.begin_browse();check(h.press(M::cancel).action==A::Back&&!m.active());
}

void legacy_browser(){
    Harness h;auto &m=h.menu;auto &c=h.catalog;
    for(std::size_t i=0;i<13;++i)c.add_browse({true,i},false,true,true);
    m.begin_browse();check(!m.browse_entry(c));
    h.key(M::confirm);check(m.screen()==S::Browse&&m.error().empty());
    h.key(M::right);check(m.browse_entry(c)==E{true,0});
    h.key(M::up);check(m.browse_index(1)==12&&m.browse_entry(c)==E{true,12});
    h.key(M::right);check(m.browse_column()==2&&!m.browse_entry(c));
    h.key(M::left);check(m.browse_index(1)==12&&m.browse_entry(c)==E{true,12});
    h.key(M::down);check(m.browse_entry(c)==E{true,0});
    h.key(M::confirm);check(m.screen()==S::Details);
    const auto play=h.press(M::confirm);check(play.action==A::Play&&play.entry==E{true,0});
    check(m.complete_play(true).action==A::Started);
    m.begin_browse();check(m.browse_column()==0&&m.browse_index(1)==0);
}

void protected_slots(){
    Harness h;auto &m=h.menu;auto &c=h.catalog;
    c.slots[0]=Slot::ReadyProtected;c.slots[1]=Slot::UnavailableProtected;
    c.add_browse({false,0},false,false,true);
    c.add_browse({false,1},false,false,false);
    c.add_browse({true,0},true,false,true);
    c.add_browse({true,1},true,false,false);
    m.begin_browse();h.key(M::confirm);check(m.screen()==S::Details);
    h.key(M::cancel);h.key(M::down);h.key(M::confirm);
    check(m.screen()==S::Browse&&m.error()=="INVALID REPLAY");
    h.key(M::left);h.key(M::down);h.key(M::confirm);
    check(m.browse_entry(c)==E{true,1}&&m.screen()==S::Browse&&m.error()=="INVALID REPLAY");
    h.key(M::up);h.key(M::confirm);
    check(m.selected_entry()==E{true,0}&&m.screen()==S::Details);
    m.begin_save(O::Pause);h.naming_end();h.key(M::confirm);
    check(m.screen()==S::SaveSlots&&m.error()=="REPLAY SLOT PROTECTED");
    h.key(M::down);h.key(M::confirm);
    check(m.screen()==S::SaveSlots&&m.error()=="REPLAY SLOT PROTECTED");
    h.key(M::down);check(h.press(M::confirm).action==A::Write);
}

void name_grid(){
    Harness h;auto &m=h.menu;
    m.begin_save(O::Pause);check(m.screen()==S::Name&&m.origin()==O::Pause&&m.name()=="PLAYER");
    check(K::columns==17&&K::rows==3&&K::capacity==8);
    check(!K::selectable(30)&&!K::selectable(33)&&!K::selectable(44)&&!K::selectable(46)&&K::selectable(K::space_key));
    check(K::character(0)=='A'&&K::character(25)=='Z'&&K::character(43)=='9'&&K::character(29)=='?');
    h.key(M::toggle_recording);check(m.name_editor().cursor()==0&&m.screen()==S::Name);
    h.key(M::confirm);check(m.name()=="A"&&m.name_editor().cursor()==1);
    h.key(M::erase);check(m.name()=="A"&&m.name_editor().cursor()==0); // Bomb clears current, then goes back.
    h.key(M::erase);check(m.name_editor().text().empty());
    h.key(M::up);h.key(M::left);h.key(M::left);check(m.name_editor().selected_key()==K::advance_key);
    h.key(M::confirm);check(m.name_editor().cursor()==1&&m.name()=="PLAYER");
    h.key(M::left);h.key(M::left);check(m.name_editor().selected_key()==K::space_key);
    h.key(M::confirm);check(m.name_editor().text()=="  "&&m.name()=="PLAYER");
    h.key(M::right);check(m.name_editor().selected_key()==K::back_key);
    h.key(M::confirm);h.key(M::confirm);check(m.name_editor().text().empty()&&m.name_editor().cursor()==0);
    h.key(M::right);h.key(M::right);h.key(M::right);check(m.name_editor().selected_key()==34);
    for(std::size_t i=1;i<=8;++i){
        h.key(M::confirm);check(m.name_editor().text().size()==i);
    }
    check(m.name()=="00000000"&&m.name_editor().selected_key()==K::end_key);
    h.key(M::right);check(m.name_editor().selected_key()==34);
    h.key(M::confirm);check(m.name()=="00000000"); // No ninth cell even when leaving END.
    h.key(M::left);h.key(M::left);h.key(M::left);check(m.name_editor().selected_key()==K::back_key);
    h.key(M::confirm);check(m.name()=="0000000"&&m.name_editor().cells().back()==' ');
    h.key(M::right);h.key(M::right);h.key(M::confirm);check(m.screen()==S::SaveSlots);
    h.key(M::cancel);check(m.screen()==S::Name&&m.name()=="0000000");
    check(h.press(M::cancel).action==A::Back&&!m.active()&&m.origin()==O::Pause);
    m.begin_save(O::GameOver,"foo/12!?ABCDEF");
    check(m.screen()==S::Name&&m.origin()==O::GameOver&&m.name()=="FOO12!?A"&&m.name_editor().selected_key()==K::end_key);
    check(h.press(M::cancel).action==A::Back&&!m.active());
}

void save_flows(){
    Harness h;auto &m=h.menu;auto &c=h.catalog;
    c.add_browse({true,13},false,false,true);c.slots[99]=Slot::Unavailable;
    m.begin_save(O::Clear);check(m.screen()==S::SaveChoice&&m.choice_yes());
    h.key(M::down);check(!m.choice_yes());
    check(h.press(M::confirm).action==A::Discard&&!m.active());
    m.begin_save(O::Clear);h.key(M::confirm);h.key(M::cancel);
    check(m.screen()==S::SaveChoice&&m.choice_yes());
    check(h.press(M::cancel).action==A::Back&&!m.active());
    m.begin_save(O::Clear,"reimu");h.key(M::confirm);h.naming_end();check(m.name()=="REIMU");
    check(soew::replay_menu_slot_pages==10);
    h.key(M::up);check(m.selected_entry()==E{false,99});
    h.key(M::confirm);check(m.screen()==S::Overwrite&&!m.choice_yes());
    h.key(M::confirm);check(m.screen()==S::SaveSlots&&m.selected_entry()==E{false,99});
    h.key(M::confirm);h.key(M::left);check(m.choice_yes());
    h.key(M::cancel|M::confirm);check(m.screen()==S::SaveSlots);
    h.key(M::confirm);check(!m.choice_yes());
    h.key(M::up);
    auto write=h.press(M::confirm);check(write.action==A::Write&&write.entry==E{false,99}&&write.replace&&m.screen()==S::Writing);
    check(m.complete_write(false).action==A::None&&m.screen()==S::SaveSlots&&!m.error().empty());
    check(m.name()=="REIMU"&&m.selected_entry()==E{false,99});
    check(m.update(M::confirm,c).action==A::None&&m.screen()==S::SaveSlots);
    h.key(M::confirm);check(!m.choice_yes()&&m.screen()==S::Overwrite);
    h.key(M::cancel);h.key(M::down);
    check(m.selected_entry()==E{false,0}&&m.error().empty());
    write=h.press(M::confirm);check(write.action==A::Write&&write.entry==E{false,0}&&!write.replace);
    const auto saved=m.complete_write(true);check(saved.action==A::Saved&&saved.entry==E{false,0}&&!m.active());
    check(m.complete_write(true).action==A::None);
    m.begin_save(O::GameOver);h.naming_end();check(m.name()=="PLAYER");
    write=h.press(M::confirm);check(write.action==A::Write&&!write.replace);
    check(m.complete_write(false).action==A::None);
    h.key(M::cancel);check(m.screen()==S::Name&&m.error().empty());
    check(h.press(M::cancel).action==A::Back&&m.origin()==O::GameOver);
}

void slowdown_sidecar(const std::filesystem::path &base){
    using namespace soew;
    using namespace soew::native_replay_detail;
    using namespace soew::replay_slots_detail;
    unsigned n=0;
    for(const auto &[len,expected]:std::array<std::pair<std::size_t,std::string_view>,5>{{
        {0,"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"},
        {55,"9f4390f8d30c2dd92ec9f095b65e2b9ae9b0a925a5258e241c9f1e910f734318"},
        {56,"b35439a4ac6f0948b6d6f9e3c6af0f5f590ce20f1bde7090ef7970686ec6738a"},
        {64,"ffe054fe7ae0cb6dc65c3af9b61d5209f439851db43d0ba5997337df154668eb"},
        {128,"6836cf13bac400e9105071cd6af47084dfacad4e5e302c94bfed24e013afb73e"}}}){
        const auto digest=replay_digest(Bytes(len,'a'));
        check(digest&&slowdown_path({},*digest)==".soew-replay-"+std::string(expected)+".slowdown");
    }
    struct Cleanup {
        fs::path path;
        ~Cleanup(){if(!path.empty()){std::error_code ignored;fs::remove_all(path,ignored);}}
    } root;
    const auto stamp=std::chrono::steady_clock::now().time_since_epoch().count();
    for(;n<64;++n){
        auto candidate=base/("soew-slowdown-test-"+std::to_string(stamp)+"-"+std::to_string(n));
        if(fs::create_directory(candidate)){root.path=std::move(candidate);break;}
    }
    check(!root.path.empty());
    const auto write=[](const fs::path &path,const Bytes &bytes){
        std::ofstream out(path,std::ios::binary|std::ios::trunc);
        out.write(reinterpret_cast<const char*>(bytes.data()),static_cast<std::streamsize>(bytes.size()));
        out.close();check(static_cast<bool>(out));
    };
    const auto rep_dir=root.path/"recorded",copy_dir=root.path/"copied";
    const ReplaySlotStore store(rep_dir),copied(copy_dir);
    const ReplaySlowdownStats measured{10000,25},zero{511,0};
    NativeReplay run;
    run.events={{NativeReplayEventKind::Tick,Shot,false,false,NativeReplayContext::Gameplay}};
    run.summary.emplace();run.summary->ticks=1;run.summary->stage_scores[0]=0;
    run.summary->stage_checks[0]=NativeReplayBoundary{1,0};
    check(copied.save(0,run)==ReplaySlotSaveResult::Saved);
    check(!copied.slowdown_stats(copied.inspect(0,false)));
    check(store.save(0,run,nullptr,measured)==ReplaySlotSaveResult::Saved);
    const auto rep=read_file(store.path(0));
    check(rep==read_file(copied.path(0))&&rep==encode(run)&&store.load(0)==run);
    const auto digest=replay_digest(rep);
    check(digest.has_value());
    const auto meta=slowdown_path(rep_dir,*digest);
    const auto meta_bytes=read_file(meta);
    const auto entry=store.inspect(0,false);
    check(store.slowdown_stats(entry)==measured&&store.legacy_entries().empty());
    check(!store.slowdown_stats(copied.inspect(0,false)));
    fs::copy_file(meta,copy_dir/meta.filename());
    check(copied.load(0)==run&&copied.slowdown_stats(copied.inspect(0,false))==measured);
    fs::copy_file(store.path(0),store.path(1));
    check(store.slowdown_stats(store.inspect(1,false))==measured);
    fs::rename(store.path(1),rep_dir/"renamed.rep");
    const auto loose=store.legacy_entries();
    check(loose.size()==1&&store.slowdown_stats(loose.front())==measured);
    fs::rename(rep_dir/"renamed.rep",store.path(2));
    check(store.slowdown_stats(store.inspect(2,false))==measured);

    // A valid REP with the same listing but different recorded input needs its own SHA.
    auto changed=run;changed.events[0].input=Bomb;
    const auto changed_rep=encode(changed);
    const auto changed_digest=replay_digest(changed_rep);
    check(changed_digest&&changed_digest!=digest&&listing(changed_rep)==listing(rep));
    check(store.save(0,changed,nullptr,zero)==ReplaySlotSaveResult::NeedsConfirmation);
    const auto wrong_meta=slowdown_path(rep_dir,*changed_digest);
    check(!fs::exists(wrong_meta));
    write(store.path(0),changed_rep);
    write(wrong_meta,meta_bytes);
    check(store.load(0)==changed&&!store.slowdown_stats(store.inspect(0,false))&&!store.slowdown_stats(entry));
    check(store.slowdown_stats(store.inspect(2,false))==measured);
    const auto confirmed=store.inspect(0);
    check(store.save(0,changed,confirmed,zero)==ReplaySlotSaveResult::Saved);
    check(store.slowdown_stats(store.inspect(0,false))==zero&&read_file(store.path(0))==changed_rep);
    check(store.slowdown_stats(confirmed)==zero);
    const auto before_replace=store.inspect(2);
    write(store.path(2),changed_rep);
    check(!store.slowdown_stats(before_replace));
    check(store.save(2,run,before_replace,measured)==ReplaySlotSaveResult::Changed);
    write(store.path(2),rep);

    const auto assert_unrecorded=[&](const Bytes &bad){
        write(meta,bad);
        check(!store.slowdown_stats(store.inspect(2,false))&&store.load(2)==run);
    };
    auto bad=meta_bytes;bad[52]^=1;assert_unrecorded(bad); // Valid-looking count, incorrect CRC.
    bad=meta_bytes;bad.pop_back();assert_unrecorded(bad);
    bad=meta_bytes;bad.resize(1024*1024);assert_unrecorded(bad);
    const auto reseal=[](Bytes &bytes){bytes.resize(60);put(bytes,slowdown_crc(bytes),4);};
    for(const auto at:{0,8,10,12}){bad=meta_bytes;bad[at]^=2;reseal(bad);assert_unrecorded(bad);}
    assert_unrecorded(encode_slowdown(*digest,{0,0}));
    assert_unrecorded(encode_slowdown(*digest,{100,101}));
    fs::remove(meta);
    check(!store.slowdown_stats(store.inspect(2,false)));
    fs::create_directory(meta); // Cannot atomically replace a directory with a sidecar.
    check(store.save(3,run,nullptr,measured)==ReplaySlotSaveResult::Saved);
    check(read_file(store.path(3))==rep&&store.load(3)==run&&!store.slowdown_stats(store.inspect(3,false)));
    for(const auto &item:fs::directory_iterator(rep_dir))check(item.path().extension()!=".tmp");
    auto invalid_stats=run;invalid_stats.name="INVALID";
    check(store.save(6,invalid_stats,nullptr,ReplaySlowdownStats{10,11})==ReplaySlotSaveResult::Saved);
    check(store.load(6)==invalid_stats&&!store.slowdown_stats(store.inspect(6,false)));
    auto invalid_rep=rep;invalid_rep[0]^=1;
    const auto good_entry=store.inspect(3,false);
    write(store.path(3),invalid_rep);
    check(store.inspect(3,false).status==ReplaySlotStatus::Corrupt&&!store.slowdown_stats(good_entry));

    auto legacy=run;legacy.summary.reset();legacy.events[0].context=NativeReplayContext::Legacy;
    write(store.path(7),encode(legacy,version));
    write(store.path(8),encode(run,v2_version));
    check(store.load(7)==legacy&&!store.slowdown_stats(store.inspect(7,false)));
    check(store.load(8)==run&&!store.slowdown_stats(store.inspect(8,false)));

    PracticeSpec spec;spec.stage=1;spec.section=PracticeSection::Chapter;spec.chapter_id=50;
    for(auto [slot,rule]:{std::pair{4u,PracticeStartRule::LegacyV4},std::pair{5u,PracticeStartRule::Calibrated}}){
        auto practice=run;practice.config=practice_replay_config(spec,2,false,false);
        practice.practice=NativeReplayPractice{spec,Language::English,1,rule};
        practice.summary->final_stage=1;
        std::swap(practice.summary->stage_scores[0],practice.summary->stage_scores[1]);
        std::swap(practice.summary->stage_checks[0],practice.summary->stage_checks[1]);
        check(store.save(slot,practice,nullptr,measured)==ReplaySlotSaveResult::Saved);
        check(copied.save(slot,practice)==ReplaySlotSaveResult::Saved);
        const auto bytes=read_file(store.path(slot));
        check(get(bytes,8,2)==slot&&bytes==read_file(copied.path(slot))&&store.load(slot)==practice);
        check(store.slowdown_stats(store.inspect(slot,false))==measured&&!copied.slowdown_stats(copied.inspect(slot,false)));
    }
}
}

int main(int argc,char **argv){
    try{
        browser();legacy_browser();protected_slots();name_grid();save_flows();
#ifdef _WIN32
        slowdown_sidecar(argc>1?std::filesystem::path(argv[1]):std::filesystem::current_path());
#else
        (void)argc;(void)argv;
#endif
    }
    catch(const std::exception &e){std::cerr<<e.what()<<'\n';return 1;}
    std::cout<<"Replay menu: "<<checks<<" checks\n";
}
