// Included by the Win32 host to exercise its real message/timer/presenter path.
struct RetryHostProbe {
    using Clock=std::chrono::steady_clock;
    Clock::time_point key{},audio{},device{},ready{},intro_first_paint{},intro_end{},first_paint{},first_done{};
    bool intro_expected=false,disable_intro=false;
    int mode=-1,stage=1,chapter=0,phase=0,iteration=-1,count=0;
    soew::PracticeSection section=soew::PracticeSection::Chapter;
    soew::PracticeSpec recipe()const{
        soew::PracticeSpec spec;
        spec.stage=stage;spec.rank=stage==5?4:1;spec.section=section;
        spec.chapter_id=section==soew::PracticeSection::Chapter?chapter:-1;
        spec.boss_phase=phase;
        if(phase>0)spec.dialogue=soew::PracticeDialogue::Skip;
        return spec;
    }
    std::uint64_t prior_playback_generation=0;
    int sample_count=20;
    bool waiting=false,seen=false,restored=false,require_checkpoint=false,force_exact=false,missed_budget=false;
    static double ms(Clock::time_point a, Clock::time_point b){return std::chrono::duration<double,std::milli>(b-a).count();}
    void finish_sample(){
        if(!waiting||!seen||first_paint==Clock::time_point{}||(mode!=0&&first_done==Clock::time_point{}))return;
        const auto end=mode==0?first_paint:std::max(first_paint,first_done);
        std::cout<<(iteration<0?"setup_ms=":"retry_ms=")<<ms(key,end)<<" iteration="<<iteration<<" mode="<<mode<<" stage="<<stage+1<<" section="<<static_cast<int>(section)
                 <<" chapter="<<recipe().chapter_id<<" phase="<<phase
                 <<" scene_and_dispatch="<<ms(key,audio)<<" audio_and_poll="<<ms(audio,device)<<" publish="<<ms(device,ready)
                 <<" ready_ms="<<ms(key,ready)
                 <<" intro_first_paint_ms="<<(intro_first_paint==Clock::time_point{}?-1.0:ms(key,intro_first_paint))
                 <<" intro_end_ms="<<(intro_end==Clock::time_point{}?-1.0:ms(key,intro_end))
                 <<" intro_gate_ms="<<(intro_end==Clock::time_point{}?-1.0:ms(ready,intro_end))
                 <<" post_intro_control_ms="<<(intro_end==Clock::time_point{}?-1.0:ms(intro_end,first_paint))
                 <<" control_paint="<<ms(ready,first_paint)<<" first_paint_ms="<<ms(key,first_paint)
                 <<" first_device_done_ms="<<(mode==0?-1.0:ms(key,first_done))
                 <<" checkpoint_restored="<<restored<<std::endl;
        if(intro_expected&&(intro_first_paint==Clock::time_point{}||intro_end==Clock::time_point{}||
                            intro_first_paint>=intro_end||intro_end>first_paint))missed_budget=true;
        if(iteration>=0&&((require_checkpoint&&(!restored||ms(key,end)>=500.0))||(force_exact&&restored)))missed_budget=true;
        waiting=false;++iteration;
    }
    void intro_finished(){if(waiting&&seen&&intro_expected)intro_end=Clock::now();}
    template<class Host> void presented(const Host &app){
        if(!waiting||!seen||!app.practice_session||app.practice_visible||!app.stage_scene)return;
        if(app.stage_intro.active()){
            if(intro_first_paint==Clock::time_point{})intro_first_paint=Clock::now();
            return;
        }
        if(app.stage_entry_release||app.stage_scene->paused||!app.practice_session->samples())return;
        if(first_paint==Clock::time_point{})first_paint=Clock::now();
        finish_sample();
    }
    template<class Host> void audio_pumped(const Host &app){
        if(!waiting||!seen||mode==0||first_done!=Clock::time_point{}||!app.practice_session||!app.audio.active()||!app.audio.completed_frames())return;
        if(app.audio.playback_generation()==prior_playback_generation||
           app.audio.positioned_measure()!=soew::practice_music_measure(app.practice_session->recipe))return;
        restored=app.audio.checkpoint_restored();
        first_done=Clock::now();
        finish_sample();
    }
    template<class Host> void start_sample(const Host &app){
        prior_playback_generation=app.audio.playback_generation();
        intro_first_paint={};intro_end={};intro_expected=false;
        first_paint={};first_done={};restored=false;seen=false;waiting=true;key=Clock::now();
    }
    template<class Host> void step(HWND hwnd,Host &app){
        auto tap=[&](int k){SendMessageW(hwnd,WM_KEYDOWN,k,0);SendMessageW(hwnd,WM_KEYUP,k,0);};
        if(!app.practice_session&&!app.practice_audio_load&&!app.practice_builder&&!app.practice_load){
            if(count){app.failed=true;std::cerr<<"retry_host_failure=preparation "<<app.practice_menu.warning<<std::endl;DestroyWindow(hwnd);return;}
            app.menu.music=mode;
            if(disable_intro)app.menu.stage_intro=false;
            auto &m=app.practice_menu;m.spec=recipe();
            app.practice_visible=true;m.selected=m.row_count()-2;m.allowed=true;
            start_sample(app);++count;tap(VK_RETURN);return;
        }
        if(waiting||!app.practice_session||app.practice_load||app.practice_audio_load||app.practice_builder||app.stage_entry_release||app.stage_scene->paused||!app.practice_session->samples())return;
        if(iteration==sample_count){
            if(missed_budget){app.failed=true;std::cerr<<"retry_host_failure=checkpoint_or_budget\n";}
            DestroyWindow(hwnd);return;
        }
        tap(VK_ESCAPE);InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);
        start_sample(app);tap('R');
    }
};
