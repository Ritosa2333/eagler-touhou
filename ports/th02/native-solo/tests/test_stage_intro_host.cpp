#define wmain soew_application_wmain
#include "main.cpp"
#undef wmain
#include <source_location>

namespace {
using namespace soew;
unsigned checks=0;
void verify(bool value,std::source_location where=std::source_location::current()) {
    ++checks;
    if(!value)throw std::runtime_error("host intro check failed at line "+std::to_string(where.line()));
}
void timer(HWND hwnd,App &app,bool intro=false) {
    auto &clock=intro?app.stage_intro.clock:app.stage_clock;
    clock.defer_batch(host_ms()-17);
    SendMessageW(hwnd,WM_TIMER,2,0);
}
void intro_run(HWND hwnd,App &app,bool enabled) {
    verify(app.stage_intro.active()==enabled);
    if(!enabled)return;
    const auto digest=replay_state_digest(*app.stage_scene);
    const auto cursor=app.replay_cursor;
    const auto events=app.recorder?app.recorder->replay().events.size():0;
    SendMessageW(hwnd,WM_KEYDOWN,'Z',0);
    SendMessageW(hwnd,WM_KEYDOWN,VK_ESCAPE,0);
    SendMessageW(hwnd,WM_KEYUP,'Z',0);
    verify(!app.stage_scene->paused&&!app.run_pause_menu);
    app.stage_scene->paused=true;
    timer(hwnd,app,true);
    verify(app.stage_intro.elapsed==0);
    app.stage_scene->paused=false;
    app.timed_test=false;
    app.focused=false;
    timer(hwnd,app,true);
    verify(app.stage_intro.elapsed==0);
    app.focused=true;
    app.timed_test=true;
    for(unsigned i=0;i<StageIntro::total_frames;++i){
        timer(hwnd,app,true);
        verify(app.stage_intro.elapsed==i+1);
        verify(replay_state_digest(*app.stage_scene)==digest&&app.replay_cursor==cursor);
        if(app.recorder)verify(app.recorder->replay().events.size()==events);
    }
    verify(!app.stage_intro.active()&&app.stage_entry_release);
}
void chinese_dialogue_probe(const fs::path &root){
    const auto pack=ChineseDisplayPack::load(root,[&](std::string_view payload){verify_chinese_manifest(root,payload);});
    if(!pack)return;
    const auto blank=[] {return Frame{640,400,std::vector<std::uint32_t>(640*400)};};
    auto scene=load_replay_scene(root,NativeReplayConfig{});
    constexpr std::array counts={48u,70u,22u,104u,114u,62u,51u,94u,12u,300u};
    for(auto lang:{Language::ChineseSimplified,Language::ChineseTraditional}){
        std::set<std::wstring> scalars;
        for(std::size_t asset=0;asset<counts.size();++asset)for(unsigned row=0;row<counts[asset];++row){
            auto value=wide_utf8(expand_continues(pack->line(lang,locale_text_assets[asset],row),12));
            std::wstring_view chars(value);
            while(!chars.empty()){
                const auto n=display_scalar_units(chars);scalars.emplace(chars.substr(0,n));chars.remove_prefix(n);
            }
        }
        for(const auto &scalar:scalars){
            if(!chinese_font(lang).supports(scalar)){
                std::wcerr<<L"Missing Chinese glyph: "<<scalar<<L" (U+"<<std::hex<<unsigned(scalar.front())<<L")\n";
                verify(false);
            }
            verify(true);
        }
        for(unsigned stage=0;stage<6;++stage){
            scene.stage=static_cast<int>(stage);scene.dialogue=scene.load_dialogue(stage);
            scene.dialogue.phase=StageDialogue::Phase::Text;scene.dialogue.current=0;scene.dialogue.reveal.cursor=1;
            auto half=blank();compose_dialogue_text(half,scene,lang,&*pack);
            verify(half.pixels==blank().pixels); // first double-width glyph cannot be split
            scene.dialogue.reveal.cursor=72;
            auto full=blank();compose_dialogue_text(full,scene,lang,&*pack);
            verify(full.pixels!=half.pixels);
        }
        scene.stage=3;scene.dialogue=scene.load_dialogue(3);
        for(unsigned count:{0u,1u,12u}){
            scene.dialogue=scene.load_dialogue(3);
            scene.dialogue.continues_used=count;scene.dialogue.current=scene.dialogue.pre_count;scene.dialogue.start_postboss();
            verify(scene.dialogue.end_box==(count?StageDialogue::stage4_no_continue_start:scene.dialogue.total_count));
            // Sample the authored count line directly, including zero even though it is not visited on the no-continue route.
            scene.dialogue.phase=StageDialogue::Phase::KeyDelay;scene.dialogue.reveal.cursor=72;
            scene.dialogue.current=42;
            const auto expected=expand_continues(pack->line(lang,"STAGE3.TXT",84),count);
            auto frame=blank(),reference=blank();compose_dialogue_text(frame,scene,lang,&*pack);
            display_text(reference,wide_utf8(expected),112,336,0xffffff,lang,288);
            verify(std::equal(frame.pixels.begin()+336*640,frame.pixels.begin()+352*640,reference.pixels.begin()+336*640));
        }
    }
}
struct Recorded { NativeReplay run; std::uint64_t before_last=0; };
Recorded record(HWND hwnd,App &app,bool intro,bool practice) {
    app.menu.stage_intro=intro;
    if(practice){
        PracticeSpec spec;spec.stage=0;spec.section=PracticeSection::StageStart;
        PracticeBuilder builder(app.assets_root,spec,{},spec.seed,true);
        verify(builder.status()==PracticeBuilder::Status::Ready);
        app.stage_scene.emplace(std::move(*builder.release()));
        app.practice_session.emplace(spec,PracticeCheckpoint{},spec.seed);
        auto config=practice_replay_config(spec,app.menu.music,true,app.menu.autofire);
        app.recorder.emplace(config,NativeReplayPractice{spec,app.stage_scene->language,practice_replay_start_digest(*app.stage_scene)});
        arm_stage_intro(app,true);
    }else{
        app.settings_enabled=true;
        launch_stage(app,0);
        app.settings_enabled=false;
    }
    verify(app.recorder.has_value());
    intro_run(hwnd,app,intro);
    Recorded result;
    for(unsigned i=0;i<20;++i){
        if(i==19)result.before_last=replay_state_digest(*app.stage_scene);
        timer(hwnd,app);
        verify(app.recorder.has_value()&&app.recorder->replay().events.size()==i+1);
    }
    verify(app.recorder->replay().events.front().input==0);
    auto summary=practice?practice_replay_summary(*app.stage_scene,*app.practice_session):app.replay_session->stop(*app.stage_scene);
    verify(app.recorder->finalize(summary));
    result.run=app.recorder->replay();
    app.stage_scene.reset();app.practice_session.reset();app.recorder.reset();app.replay_session.reset();
    return result;
}
void playback(HWND hwnd,App &app,const Recorded &recorded,bool intro,unsigned version) {
    auto run=recorded.run;
    if(version==1){run.summary.reset();run.played_at=0;run.name.clear();for(auto &event:run.events)event.context=NativeReplayContext::Legacy;}
    run=native_replay_detail::decode(native_replay_detail::encode(run,version));
    auto scene=run.practice?load_practice_replay_scene(app.assets_root,run):std::make_unique<EncounterScene>(load_replay_scene(app.assets_root,run.config));
    ReplaySession session(run);
    app.menu.stage_intro=intro;
    publish_replay(app,run,std::move(scene),std::move(session),0);
    intro_run(hwnd,app,intro);
    for(unsigned i=0;i<19;++i){timer(hwnd,app);verify(app.replay_cursor==i+1);}
    verify(replay_state_digest(*app.stage_scene)==recorded.before_last);
    timer(hwnd,app);
    verify(!app.replay_playback&&app.notice.find(L"Replay finished")!=std::wstring::npos);
}
}
int main(int argc,char **argv){
    try{
        if(argc!=2)throw std::runtime_error("Expected staged assets directory");
        chinese_dialogue_probe(argv[1]);
        App app;app.assets_root=argv[1];app.menu.language=Language::English;app.stage_intro_art=Frame{};
        app.menu.reduced=true;app.menu.music=0;
        WNDCLASSW wc{};wc.lpfnWndProc=window_proc;wc.hInstance=GetModuleHandleW(nullptr);wc.lpszClassName=L"StageIntroHostTest";
        verify(RegisterClassW(&wc)!=0);
        HWND hwnd=CreateWindowW(wc.lpszClassName,L"Intro host test",WS_OVERLAPPEDWINDOW,0,0,640,400,nullptr,nullptr,wc.hInstance,&app);
        verify(hwnd!=nullptr);ShowWindow(hwnd,SW_SHOW);SetForegroundWindow(hwnd);SetFocus(hwnd);
        app.focused=true;app.timed_test=true;app.test_started=host_ms();
        for(bool practice:{false,true}){
            std::optional<Recorded> baseline;
            for(bool intro:{false,true}){
                const auto captured=record(hwnd,app,intro,practice);
                if(baseline){
                    verify(captured.run.events==baseline->run.events);
                    verify(captured.run.summary==baseline->run.summary);
                    verify(captured.before_last==baseline->before_last);
                }else baseline=captured;
                for(unsigned version=practice?4:1;version<=(practice?4:3);++version)playback(hwnd,app,captured,!intro,version);
                std::cout<<(practice?"practice":"story")<<" record_intro="<<intro<<" playback_intro="<<!intro<<" versions="<<(practice?"4":"1-3")<<" samples=20\n";
            }
        }
        DestroyWindow(hwnd);
        std::cout<<"stage_intro_host_checks="<<checks<<" failures=0\n";
        return 0;
    }catch(const std::exception &e){std::cerr<<e.what()<<'\n';return 1;}
}
