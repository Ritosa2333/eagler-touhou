#include "replay_session.hpp"
#include "encounter_info.hpp"
#include "opening.hpp"
#include "frame_pacing.hpp"
#include <iostream>
#include <source_location>

namespace {
using namespace soew;
using Status=ReplayStepStatus;
using Kind=NativeReplayEventKind;
using Context=NativeReplayContext;
using Phase=StageTransition::Phase;
namespace fs=std::filesystem;
unsigned checks=0;
void check(bool ok,std::source_location where=std::source_location::current()){
    ++checks;
    if(!ok)throw std::runtime_error("Replay scene assertion at line "+std::to_string(where.line()));
}

struct Capture {
    NativeReplay run;
    ReplaySession session;
    explicit Capture(const NativeReplayConfig &c):session(c){run.config=c;run.name="TEST";run.played_at=1;}
    ReplayStep tick(EncounterScene &s,std::uint16_t input=0,bool group=false,bool fast=false){
        auto step=session.tick(s,normalize_replay_tick(input,group,fast));
        if(step.status==Status::Failed)throw std::runtime_error(step.error);
        if(step.consumed_tick)run.events.push_back(step.event);
        if(step.stage_advanced)run.events.push_back({Kind::NextStage});
        return step;
    }
    void cancel(EncounterScene &s){
        const auto step=session.cancel_continue(s);
        check(step.status==Status::Finished&&!step.consumed_tick&&step.event.kind==Kind::CancelContinue);
        run.events.push_back(step.event);
    }
    void finish(const EncounterScene &s){run.summary=session.stop(s);}
};

struct PlaybackResult {
    ReplayStep step;
    std::size_t cursor=0;
    std::uint32_t ticks=0;
    unsigned stage_advances=0,display_frames=0;
    std::optional<NativeReplaySummary> summary;
};
template<class Fixture> PlaybackResult play(EncounterScene &s,const NativeReplay &run,Fixture fixture,bool held_z=false){
    ReplaySession playback(run);
    PlaybackResult result;
    do{
        // A temporary release returns to one ordinary tick immediately, even
        // when a stream has zero-time commands bordering a Tick.
        const bool fast=held_z&&result.display_frames%20!=5;
        ++result.display_frames;
        for(unsigned i=0;i<replay_view_speed(fast)&&result.step.status==Status::Running;++i){
            fixture(s);
            result.step=playback.step_playback(s,run,result.cursor);
            result.stage_advances+=result.step.stage_advanced;
            check(result.step.status!=Status::Running||result.step.consumed_tick);
        }
    }while(result.step.status==Status::Running);
    result.ticks=static_cast<std::uint32_t>(playback.ticks());
    result.summary=playback.summary();
    return result;
}
const auto no_fixture=[](EncounterScene &){};
NativeReplay bytes_roundtrip(const NativeReplay &run,bool require_deflate=false){
    using namespace native_replay_detail;
    const auto bytes=encode(run);
    check(get(bytes,8,2)==(run.practice?(run.practice->start_rule==PracticeStartRule::Calibrated?5u:4u):run.summary?3u:1u));
    if(require_deflate)check(get(bytes,22,2)==1);
    const auto copied=decode(bytes);
    check(copied==run);
    return copied;
}
void finished(const PlaybackResult &result,const NativeReplay &run){
    if(result.step.status==Status::Failed)throw std::runtime_error(result.step.error);
    check(result.step.status==Status::Finished&&result.cursor==run.events.size());
    if(run.summary)check(result.ticks==run.summary->ticks&&result.summary==run.summary);
}
void check_fast_parity(EncounterScene &scene,const NativeReplay &run,const PlaybackResult &ordinary,std::uint64_t expected_digest,const auto &fixture){
    const auto accelerated=play(scene,run,fixture,true);
    finished(accelerated,run);
    check(accelerated.ticks==ordinary.ticks&&accelerated.cursor==ordinary.cursor&&
        accelerated.stage_advances==ordinary.stage_advances&&accelerated.summary==ordinary.summary);
    check(replay_state_digest(scene)==expected_digest);
    check(accelerated.display_frames<ordinary.display_frames);
}
void check_v2_parity(EncounterScene &scene,const NativeReplay &run,const PlaybackResult &v3,std::uint64_t v3_digest){
    using namespace native_replay_detail;
    const auto bytes=encode(run,v2_version);
    check(get(bytes,8,2)==2&&get(bytes,22,2)==0);
    const auto v2=decode(bytes);
    const auto result=play(scene,v2,no_fixture);
    finished(result,v2);
    // Summary equality covers the observed outcome, scores, and all split/terminal digests.
    check(v2==run&&result.summary==v3.summary&&result.stage_advances==v3.stage_advances&&result.ticks==v3.ticks);
    check(replay_state_digest(scene)==v3_digest&&v3_digest==run.summary->state_hash);
}

void replay_intro_speed(const fs::path &root){
    const auto scene=load_replay_scene(root,NativeReplayConfig{});
    const auto digest=replay_state_digest(scene);
    StageIntro ordinary,viewer;
    ordinary.arm(0,true);viewer.arm(0,true);
    viewer.advance(17,false,replay_view_speed(true));
    viewer.advance(34,true,replay_view_speed(true));
    check(viewer.elapsed==0); // No background catch-up.
    viewer.advance(51,true,replay_view_speed(true));
    viewer.advance(68,true,replay_view_speed(false));
    check(viewer.elapsed==5);
    unsigned frames=0;
    for(;viewer.active();++frames){
        const auto now=85+17*frames;
        viewer.advance(now,true,replay_view_speed(true));
        ordinary.advance(now,true);
    }
    check(frames==(StageIntro::total_frames-5+3)/4&&ordinary.active());
    check(viewer.elapsed==StageIntro::total_frames&&replay_state_digest(scene)==digest);
}

void host_timing_preserves_replay(const fs::path &root){
    const NativeReplayConfig config{};
    constexpr unsigned count=384;
    const auto record=[&](bool timed){
        auto scene=load_replay_scene(root,config);
        Capture capture(config);
        PresentationClock clock;
        RecordedHostTiming stats;
        unsigned consumed=0;
        std::uint64_t now=0;
        constexpr std::array intervals={1u,10u,23u,79u,180u,301u,17u};
        for(unsigned callback=0;consumed<count&&callback<2000;++callback){
            const bool enabled=!timed||callback%11!=5;
            now+=timed?intervals[callback%intervals.size()]:17u;
            HostTimingSample sample;
            const auto steps=clock.advance(now,enabled,scene.presentation_slowdown(),timed?&sample:nullptr);
            if(timed)stats.observe(enabled,sample);
            for(unsigned i=0;i<steps&&consumed<count;++i,++consumed){
                const std::uint16_t input=(consumed%3?Shot:0)|(consumed%64<32?Left:Right);
                const auto step=capture.tick(scene,input);
                check(step.status==Status::Running&&step.consumed_tick);
            }
        }
        check(consumed==count);
        check(!timed||(stats.total.lost_ms>0&&stats.total.lost_ms<stats.total.active_ms));
        capture.finish(scene);
        return capture.run;
    };
    const auto baseline=record(false),measured=record(true);
    check(baseline==measured&&native_replay_detail::encode(baseline)==native_replay_detail::encode(measured));
    auto scene=load_replay_scene(root,config);
    finished(play(scene,measured,no_fixture),measured);
    check(replay_state_digest(scene)==baseline.summary->state_hash);
}

void loss_and_continue(const fs::path &root){
    NativeReplayConfig c;c.lives=0;c.bombs=0;
    auto live=load_replay_scene(root,c);
    Capture capture(c);
    // No fixture: only real input, real enemies, and the natural death state.
    bool continued=false;
    for(unsigned i=0;i<20000&&live.continue_choice!=EncounterScene::ContinueChoice::Declined;++i){
        if(live.continue_choice==EncounterScene::ContinueChoice::Choose){
            if(!continued){
                const auto gameplay_ticks=live.ticks;
                check(capture.tick(live,Down).event.context==Context::Continue&&!live.continue_yes);
                capture.tick(live,Up);check(live.continue_yes);
                capture.tick(live,OK,true);continued=true;
                check(live.continues_used==1&&live.ticks==gameplay_ticks&&live.transition.phase==Phase::Entry);
            }else{
                const auto count=capture.session.ticks();
                capture.cancel(live);check(capture.session.ticks()==count);
                break;
            }
        }else capture.tick(live);
    }
    capture.finish(live);
    const auto run=bytes_roundtrip(capture.run,true);
    check(continued&&run.summary->outcome==NativeReplayOutcome::GameOver&&run.events.back().kind==Kind::CancelContinue);
    check(live.life.total_misses==2&&live.life.game_over&&live.runtime.spawns>0);
    auto replay=load_replay_scene(root,c);
    const auto result=play(replay,run,no_fixture);finished(result,run);
    const auto digest=replay_state_digest(replay);
    check(replay_state_digest(live)==digest);
    auto v2=load_replay_scene(root,c);
    check_v2_parity(v2,run,result,digest);
    auto accelerated=load_replay_scene(root,c);
    check_fast_parity(accelerated,run,result,digest,no_fixture);
    auto corrupt=run;
    auto i=std::find_if(corrupt.events.begin(),corrupt.events.end(),[](const auto &e){return e.context==Context::Continue;});
    check(i!=corrupt.events.end());i->context=Context::Gameplay;
    auto diverged=load_replay_scene(root,c);
    const auto bad=play(diverged,bytes_roundtrip(corrupt),no_fixture);
    check(bad.step.status==Status::Failed&&bad.step.error.find("context")!=std::string::npos);
    std::cout<<"input-only Game Over + Continue: samples="<<run.summary->ticks<<" score="<<run.summary->final_score<<'\n';
}

void rika_clear_fixture(EncounterScene &s){
    s.bind_rika_battle({0,0,0,0,0,0});s.rika.done=true;
    s.dialogue.phase=StageDialogue::Phase::Complete;s.dialogue.current=s.dialogue.pre_count;
    s.score_core.score=4400;s.score_core.delta=13;s.pickups.points_collected=7;
}
void stage_boundary(const fs::path &root){
    NativeReplayConfig c;
    auto live=load_replay_scene(root,c);rika_clear_fixture(live);
    Capture capture(c);
    check(!capture.tick(live).stage_advanced&&live.transition.phase==Phase::Postboss);
    bool advanced=false;
    for(unsigned i=0;i<800&&!advanced;++i){
        const auto context=replay_tick_context(live);
        const bool fast=context==Context::Dialogue;
        const bool group=!fast&&live.transition.gate.waiting_press;
        advanced=capture.tick(live,fast?Shot:0,group,fast).stage_advanced;
    }
    check(advanced&&live.stage==1&&live.transition.phase==Phase::Entry&&live.transition.frame==0);
    const auto split_score=live.score_core.score;
    for(unsigned i=0;i<StageTransition::fade_frames+14;++i)capture.tick(live,Shot|Right);
    capture.finish(live);
    const auto run=bytes_roundtrip(capture.run,true);
    check(run.summary->outcome==NativeReplayOutcome::MenuReturn&&run.summary->stage_scores[0]==split_score);
    check(run.summary->stage_checks[0]&&run.summary->stage_checks[0]->ticks<run.summary->ticks);
    auto replay=load_replay_scene(root,c,Language::English);rika_clear_fixture(replay);
    const auto result=play(replay,run,no_fixture);finished(result,run);
    const auto digest=replay_state_digest(replay);
    check(result.stage_advances==1&&replay.stage==1&&digest==replay_state_digest(live));
    auto v2=load_replay_scene(root,c,Language::English);rika_clear_fixture(v2);
    check_v2_parity(v2,run,result,digest);
    check(live.language==Language::Japanese&&replay.language==Language::English);
    if(has_locale_assets(root))check(live.dialogue.boxes.front().lines[0].text!=replay.dialogue.boxes.front().lines[0].text);
    auto bad_split=run;bad_split.summary->stage_checks[0]->state_hash^=1;
    auto bad=load_replay_scene(root,c);rika_clear_fixture(bad);
    auto error=play(bad,bytes_roundtrip(bad_split),no_fixture);
    check(error.step.status==Status::Failed&&error.step.error.find("stage split")!=std::string::npos);

    // Convert this same stream to v1 and put back the idle Ready samples the
    // old timer could record. The explicit marker alone commits the handoff.
    auto v1=run;v1.name.clear();v1.played_at=0;v1.summary.reset();
    for(auto &e:v1.events)e.context=Context::Legacy;
    const auto marker=std::find_if(v1.events.begin(),v1.events.end(),[](const auto &e){return e.kind==Kind::NextStage;});
    check(marker!=v1.events.end());v1.events.insert(marker,3,NativeReplayEvent{});
    auto legacy=load_replay_scene(root,c,Language::English);rika_clear_fixture(legacy);
    const auto old=bytes_roundtrip(v1);
    const auto legacy_result=play(legacy,old,no_fixture);
    finished(legacy_result,old);
    check(legacy_result.ticks==result.ticks+3&&legacy_result.stage_advances==1&&replay_state_digest(legacy)==replay_state_digest(live));
    {
        auto accelerated=std::make_unique<EncounterScene>(load_replay_scene(root,c,Language::English));
        rika_clear_fixture(*accelerated);
        check_fast_parity(*accelerated,old,legacy_result,digest,no_fixture);
        *accelerated=load_replay_scene(root,c,Language::English);rika_clear_fixture(*accelerated);
        check_fast_parity(*accelerated,native_replay_detail::decode(native_replay_detail::encode(run,2)),result,digest,no_fixture);
    }

    const auto seek_fixture=[&](const NativeReplay &recording,Language display=Language::Japanese){
        auto scene=load_replay_scene(root,c,simulation_language(display));rika_clear_fixture(scene);
        ReplaySession session(recording);std::size_t cursor=0;
        const auto boundary=story_replay_stage_start(recording,1);
        check((native_replay_detail::listing(native_replay_detail::encode(recording)).seek_stages&(1u<<1))!=0);
        while(scene.stage==0){
            const auto step=session.step_playback(scene,recording,cursor,1);
            check(step.status==Status::Running);
        }
        check(cursor==boundary&&recording.events[cursor].kind==Kind::Tick&&scene.transition.frame==0);
        const auto entry= replay_state_digest(scene);
        ReplayStep step;
        do{step=session.step_playback(scene,recording,cursor);}while(step.status==Status::Running);
        check(step.status==Status::Finished&&replay_state_digest(scene)==digest);
        return entry;
    };
    const auto base_entry=seek_fixture(run);
    check(seek_fixture(old)==base_entry);
    const auto v2_recording=native_replay_detail::decode(native_replay_detail::encode(run,2));
    check(seek_fixture(v2_recording)==base_entry);
    for(auto display:{Language::ChineseSimplified,Language::ChineseTraditional}){
        check(seek_fixture(old,display)==base_entry);
        check(seek_fixture(v2_recording,display)==base_entry);
        check(seek_fixture(run,display)==base_entry);
    }
    auto empty_stage=old;empty_stage.events.resize(story_replay_stage_start(old,1));
    check(native_replay_detail::listing(native_replay_detail::encode(empty_stage)).seek_stages==0);

    // A well-formed NextStage event still cannot force an early handoff.
    auto early=old;early.events={{},{Kind::NextStage},{}};
    auto invalid=load_replay_scene(root,c);
    const auto failed=play(invalid,bytes_roundtrip(early),no_fixture);
    check(failed.step.status==Status::Failed&&invalid.stage==0);
    std::cout<<"Stage 1 settlement + stage split fixture and v1 Ready batching: split_tick="
        <<run.summary->stage_checks[0]->ticks<<" score="<<split_score<<'\n';
}

void protected_clear(const fs::path &root,bool extra,bool new_taps=false){
    NativeReplayConfig c;c.reduced=true;c.shot=0;
    if(extra){c.stage=5;c.rank=4;c.bombs=1;c.extra=true;}
    else{c.stage=4;c.stage5_trial=true;}
    auto live=load_replay_scene(root,c);
    Capture capture(c);
    // Run the complete road, dialogue, boss damage and bonus systems. The
    // fixture protects the player on both sides; this is NOT input-only Clear.
    const auto protect=[](EncounterScene &s){s.life.invincibility=200;};
    bool bombed=false;
    MimaDialogueTap tap;unsigned tapped_box=0,taps=0;
    for(unsigned i=0;i<125000&&!capture.session.summary();++i){
        protect(live);
        const auto ctx=replay_tick_context(live);
        std::uint16_t input=0;bool group=false,fast=false;
        if(ctx==Context::Dialogue){
            if(new_taps&&live.dialogue.mima_pre_final_dialogue()){
                if(live.dialogue.phase==StageDialogue::Phase::Text&&tapped_box!=live.dialogue.current){
                    tapped_box=live.dialogue.current;++taps;
                    tap.event(&live.dialogue,true);tap.event(&live.dialogue,false);
                }
                const auto event=tap.poll(live.dialogue);input=event.accelerate?Shot:0;group=event.group;
            }else {input=Shot;group=fast=true;}
        }
        else if(ctx==Context::StageClear)group=live.extra_settlement.gate.waiting_press;
        else if(extra&&live.sigma_enabled){
            const auto dx=live.sigma.left[live.runtime.page^1]+60-(live.player.x+16);
            input=Shot|(dx>4?Right:dx<-4?Left:0)|(live.player.y>230?Up:0);
            if(!bombed&&live.sigma.phase==3){input|=Bomb;bombed=true;}
        }else if(!extra&&live.mima_enabled){
            const auto dx=live.mima.left+64-(live.player.x+8);
            input=Shot|(dx>4?Right:dx<-4?Left:0)|(live.player.y>256?Up:0);
        }
        capture.tick(live,input,group,fast);
    }
    capture.finish(live);
    const auto run=bytes_roundtrip(capture.run);
    check(run.summary->outcome==NativeReplayOutcome::Clear&&run.summary->final_stage==c.stage);
    check(live.runtime.scroll.done&&live.runtime.spawns>0&&live.bullet_core.spawned>0&&live.shots.fired>0);
    if(extra)check(live.extra_midboss.finished&&live.sigma.done&&bombed&&live.extra_settlement.phase==ExtraSettlement::Phase::Clear);
    else check(live.stage5_result->good_ending&&live.mima.form2&&(!new_taps||taps==4));
    auto replay=load_replay_scene(root,c);
    // Presentation state differs; replay must not depend on audio delivery.
    replay.sound.enabled=true;
    unsigned sounds=0;replay.sound_output=[&](unsigned){++sounds;};
    replay.set_hitbox_mode(HitboxMode::All);
    finished(play(replay,run,protect),run);
    check(sounds>0&&replay_state_digest(replay)==replay_state_digest(live));
    std::cout<<(extra?"Extra":"Stage 5")<<" protected Clear fixture: samples="<<run.summary->ticks
        <<" score="<<run.summary->final_score<<" hash="<<run.summary->state_hash<<'\n';
}

void input_only_stage5_clear(const fs::path &root){
    NativeReplayConfig c;c.stage=4;c.stage5_trial=true;c.reduced=true;c.shot=0;c.rank=0;c.lives=4;
    auto live=load_replay_scene(root,c);
    Capture capture(c);
    // Choices use only the same input bits available to a player; no scene
    // mutation or invulnerability injection on either side of this roundtrip.
    for(unsigned i=0;i<125000&&!capture.session.summary();++i){
        const auto ctx=replay_tick_context(live);
        if(ctx==Context::Continue){
            const bool accept=live.continue_choice==EncounterScene::ContinueChoice::Choose;
            capture.tick(live,accept?OK:0,accept);
        }else if(ctx==Context::Dialogue)capture.tick(live,Shot,true,true);
        else if(ctx==Context::StageClear)capture.tick(live);
        else{
            std::uint16_t input=Shot;
            if(live.mima_enabled){
                const auto dx=live.mima.left+64-(live.player.x+8);
                input|=(dx>4?Right:dx<-4?Left:0)|(live.player.y>248?Up:0);
            }
            capture.tick(live,input);
        }
    }
    capture.finish(live);
    const auto run=bytes_roundtrip(capture.run,true);
    std::cout<<"Stage 5 input-only run: samples="<<run.summary->ticks<<" score="<<run.summary->final_score
        <<" outcome="<<unsigned(run.summary->outcome)<<" continues="<<live.continues_used
        <<" misses="<<live.life.total_misses<<" battle="<<live.mima_enabled<<" phase="<<unsigned(live.mima.phase)
        <<" damage="<<live.mima.damage_taken<<'\n';
    check(run.summary->outcome==NativeReplayOutcome::Clear&&live.stage5_result);
    auto replay=load_replay_scene(root,c);
    const auto result=play(replay,run,no_fixture);finished(result,run);
    const auto digest=replay_state_digest(replay);
    check(digest==replay_state_digest(live));
    auto v2=load_replay_scene(root,c);
    check_v2_parity(v2,run,result,digest);
}

void extra_loss(const fs::path &root){
    NativeReplayConfig c;c.stage=5;c.rank=4;c.lives=0;c.bombs=0;c.extra=true;
    auto live=load_replay_scene(root,c);Capture capture(c);
    for(unsigned i=0;i<30000&&!capture.session.summary();++i){
        const bool confirm=live.extra_settlement.gate.waiting_press;
        capture.tick(live,0,confirm);
    }
    capture.finish(live);
    const auto run=bytes_roundtrip(capture.run);
    check(live.extra_settlement.phase==ExtraSettlement::Phase::Lost&&!live.continue_prompt()&&live.continues_used==0);
    check(run.summary->outcome==NativeReplayOutcome::GameOver);
    auto replay=load_replay_scene(root,c);
    finished(play(replay,run,no_fixture),run);
    check(replay_state_digest(replay)==replay_state_digest(live));
    std::cout<<"Extra input-only Game Over: samples="<<run.summary->ticks<<'\n';
}

void campaign_clear(const fs::path &root){
    NativeReplayConfig c;c.stage5_mainline=true;c.reduced=true;c.shot=0;c.rank=0;c.lives=4;
    auto live=load_replay_scene(root,c);
    Capture capture(c);
    const auto protect=[](EncounterScene &s){s.life.invincibility=200;};
    for(unsigned i=0;i<200000&&!capture.session.summary();++i){
        protect(live);
        const auto ctx=replay_tick_context(live);
        if(ctx==Context::Dialogue){capture.tick(live,Shot,true,true);continue;}
        if(ctx==Context::StageClear){capture.tick(live,0,live.transition.gate.waiting_press);continue;}
        int target=-1,y=260;
        const auto page=live.runtime.page^1;
        if(live.rika_enabled)target=live.rika.left[page]+24;
        if(live.meira_enabled)target=live.meira.left[page]+40;
        if(live.stones_enabled)for(const auto &stone:live.stones.stones)if(stone.flag==StonesCore::Flag::Active){target=stone.x+12;break;}
        if(live.marisa_enabled){target=live.marisa.left+40;for(const auto &orb:live.marisa.orbs)if(orb.active&&!orb.dying){target=orb.x+12;break;}}
        if(live.mima_enabled){target=live.mima.left+64;y=248;}
        const auto dx=target-(live.player.x+8);
        const std::uint16_t movement=target<0?0:(dx>4?Right:dx<-4?Left:0)|(live.player.y>y?Up:0);
        capture.tick(live,Shot|movement);
    }
    capture.finish(live);
    const auto run=bytes_roundtrip(capture.run);
    std::cout<<"Campaign protected fixture: samples="<<run.summary->ticks<<" end_stage="<<unsigned(run.summary->final_stage)+1
        <<" outcome="<<unsigned(run.summary->outcome)<<" continues="<<live.continues_used<<'\n';
    check(run.summary->outcome==NativeReplayOutcome::Clear&&run.summary->final_stage==4&&live.continues_used==0);
    auto playback=load_replay_scene(root,c,Language::English);
    playback.set_hitbox_mode(HitboxMode::Self);
    const auto result=play(playback,run,protect);finished(result,run);
    const auto digest=replay_state_digest(live);
    check(result.stage_advances==4&&replay_state_digest(playback)==digest);
    auto accelerated=std::make_unique<EncounterScene>(load_replay_scene(root,c,Language::English));
    check_fast_parity(*accelerated,run,result,digest,protect);
}

void practice_language_baseline(const fs::path &root){
    if(!has_locale_assets(root))return;
    PracticeSpec spec;spec.stage=0;spec.section=PracticeSection::StageStart;spec.dialogue=PracticeDialogue::Show;
    constexpr std::uint64_t japanese_start=10817141639303819786ull;
    constexpr std::uint64_t english_start=2621883412688071912ull;
    for(auto display:{Language::Japanese,Language::English,Language::ChineseSimplified,Language::ChineseTraditional}){
        const auto base=simulation_language(display);
        const auto expected=display==Language::English?english_start:japanese_start;
        check(base==(display==Language::English?Language::English:Language::Japanese));
        PracticeBuilder builder(root,spec,{},spec.seed,true,base);
        if(builder.status()!=PracticeBuilder::Status::Ready)throw std::runtime_error(builder.error());
        const auto scene=builder.release();
        // Pinned against the original staged JA/EN records, before the Chinese display path.
        check(scene->language==base&&practice_replay_start_digest(*scene)==expected);
        PracticeSession session(spec,{},spec.seed);
        NativeReplayRecorder recorder(practice_replay_config(spec,1,true,false),{spec,scene->language,expected});
        for(unsigned i=0;i<64;++i){
            const auto event=practice_replay_tick(*scene,i%2?Shot|Left:UpRight,false,false);
            session.tick(*scene,event.input,event.dialogue_group,event.dialogue_fast_forward);
            check(recorder.record(event));
        }
        check(recorder.finalize(practice_replay_summary(*scene,session)));
        const auto bytes=native_replay_detail::encode(recorder.replay());
        check(bytes.at(176)==(base==Language::English?1:0));
        const auto persisted=native_replay_detail::decode(bytes);
        // Viewing in either Chinese locale must not select the restart base for an existing v4 replay.
        auto restored=load_practice_replay_scene(root,persisted);
        check(restored->language==base&&practice_replay_start_digest(*restored)==expected);
        finished(play(*restored,persisted,no_fixture),persisted);
        check(replay_state_digest(*restored)==replay_state_digest(*scene));
    }
}

void legacy_preboss_recordings(const fs::path &root){
    // Captured by the unmodified v4 writer, 96 ticks Left/Right/idle. Each pair
    // is the fixed seed 1234, then the already-resolved random seed 0x9876abcd.
    // Store the common wire bytes just once; never ask the current builder or
    // encoder to manufacture the expected start, terminal, or compressed input.
    struct Golden {std::uint64_t bilingual,flat,terminal;std::uint32_t bilingual_sum,flat_sum;};
    constexpr Golden golden[]={
        {17615842858216582306ull,7358289729636263584ull,7523524315739046217ull,1130912848u,2651898575u},
        {2073006779836234408ull,6143169648813828762ull,3028480466568102635ull,1969385845u,749666681u},
        {12280041308776899916ull,2322559512610729467ull,5072593777974901238ull,3536892810u,1469887704u},
        {2662566348620029602ull,13981494996810944713ull,6156410362202883932ull,2336899039u,3034555551u},
        {11726885442263243415ull,7194706362028506741ull,11817365874800437350ull,1762599731u,1199495210u},
        {1862308931525994948ull,9702097437738951610ull,5376089784647361161ull,3947693845u,2978047509u},
        {643317538001383039ull,11567534119467010019ull,11889664202823362783ull,1409296829u,1232343403u},
        {16575141533861786646ull,3504287717388775674ull,7718273274128118578ull,3135162212u,3645737879u},
        {4284370184282923ull,15933016293233166311ull,2847480553097942210ull,3769886448u,107179759u},
        {5572444506037067476ull,3470784697305516356ull,8150865962088638366ull,105712096u,408416058u},
        {13542507658745409156ull,10075870952893256635ull,12140497722179891201ull,1644551615u,1384779940u},
        {11442848197011069943ull,10392951549740298896ull,12123781029588344656ull,2839509199u,10700110u},
    };
    const bool bilingual=has_locale_assets(root);
    for(int stage=0;stage<6;++stage)for(int random=0;random<2;++random){
        if(stage==5&&!fs::exists(root/"STAGE5.TXT"))continue;
        const auto &g=golden[stage*2+random];
        native_replay_detail::Bytes b(192,0);
        const auto le=[&](int at,std::uint64_t v,int n){for(int i=0;i<n;++i)b[at+i]=std::uint8_t(v>>(8*i));};
        std::copy_n("SOEWRPLY",8,b.begin());b[8]=4;b[10]=1;le(12,random?0x9876abcdu:1234u,4);
        b[16]=b[50]=std::uint8_t(stage);b[17]=1;b[18]=stage==5?4:1;b[19]=b[52]=2;b[20]=b[53]=3;
        b[21]=b[22]=b[32]=1;b[24]=b[60]=b[96+12*stage]=96;b[40]='V';b[41]='4';b[48]=2;b[49]=b[54]=1;
        b[51]=b[55]=std::uint8_t(1u<<stage);le(64,g.terminal,8);le(100+12*stage,g.terminal,8);
        b[168]=1;b[169]=std::array<std::uint8_t,6>{46,51,56,61,62,72}[stage];b[171]=6;b[172]=80;b[173]=3;b[174]=1;b[175]=random;
        le(184,bilingual?g.bilingual:g.flat,8);le(28,bilingual?g.bilingual_sum:g.flat_sum,4);
        b.insert(b.end(),{0x78,0xda,0x63,0x60,0x61,0x61,0x60,0xc0,0xc0,0x1c,0x58,0x30,0x03,0x99,0x78,0xd4,0x7c,0x7c,0x18,0x00,0xee,0x2f,0x02,0x71});
        const auto run=native_replay_detail::decode(b);
        const auto row=native_replay_detail::listing(b);
        check(row.practice==run.practice&&row.summary==run.summary&&row.seek_stages==0&&row.name=="V4");
        check(run.practice->spec.seed_mode==(random?PracticeSeedMode::Random:PracticeSeedMode::Fixed));
        auto restored=load_practice_replay_scene(root,run);
        const auto ordinary=play(*restored,run,no_fixture);
        finished(ordinary,run);
        auto accelerated=load_practice_replay_scene(root,run);
        check_fast_parity(*accelerated,run,ordinary,replay_state_digest(*restored),no_fixture);
    }
}

void practice_recordings(const fs::path &root){
    using Result=PracticeSession::Result;
    const auto capture=[&](PracticeSpec spec,std::uint32_t seed,unsigned limit,const auto &input){
        PracticeBuilder builder(root,spec,{},seed,true);
        if(builder.status()!=PracticeBuilder::Status::Ready)throw std::runtime_error(builder.error());
        auto scene=builder.release();
        PracticeSession session(spec,{},seed);
        spec.seed=seed;
        const auto config=practice_replay_config(spec,1,true,false);
        NativeReplayRecorder recorder(config,{spec,Language::Japanese,practice_replay_start_digest(*scene),practice_start_rule(spec)});
        scene->paused=true;session.tick(*scene,Shot,true);scene->paused=false;
        check(session.samples()==0&&!recorder.can_save());
        for(unsigned i=0;i<limit&&session.result()==Result::Running;++i){
            const auto event=input(*scene,i);
            session.tick(*scene,event.input,event.dialogue_group,event.dialogue_fast_forward);
            check(recorder.record(event));
        }
        if(session.simulation_error())throw std::runtime_error(session.error());
        scene->paused=true;
        check(recorder.finalize(practice_replay_summary(*scene,session)));
        const auto run=bytes_roundtrip(recorder.replay());
        check(run.summary->ticks==session.samples()&&run.summary->ticks==run.events.size());
        check(run.practice->spec.seed==seed&&run.practice->spec.seed_mode==spec.seed_mode);
        auto replay=load_practice_replay_scene(root,run);
        const auto ordinary=play(*replay,run,no_fixture);
        finished(ordinary,run);
        auto accelerated=load_practice_replay_scene(root,run);
        check_fast_parity(*accelerated,run,ordinary,replay_state_digest(*replay),no_fixture);
        return run;
    };
    const auto idle=[](const EncounterScene &s,unsigned){return practice_replay_tick(s,0,false,false);};
    PracticeSpec start;start.stage=0;start.section=PracticeSection::StageStart;start.lives=0;start.bombs=0;
    const auto fragment=capture(start,start.seed,9,idle);
    check(fragment.summary->outcome==NativeReplayOutcome::Aborted);
    const auto loss=capture(start,start.seed,20000,idle);
    check(loss.summary->outcome==NativeReplayOutcome::GameOver);
    PracticeSpec chapter;chapter.stage=1;chapter.section=PracticeSection::Chapter;chapter.chapter_id=50;
    chapter.seed_mode=PracticeSeedMode::Random;chapter.dialogue=PracticeDialogue::Show;
    const auto input=[](const EncounterScene &s,unsigned i){return practice_replay_tick(s,i%2?Shot|Left:UpLeft,i%3!=0,i%4!=0);};
    const auto first=capture(chapter,0x12345678u,100,input);
    const auto retry=capture(chapter,0x23456789u,100,input);
    check(first.practice->start_hash!=retry.practice->start_hash&&first.summary->state_hash!=retry.summary->state_hash);
    auto altered=first;altered.practice->start_hash^=1;
    bool rejected=false;try{load_practice_replay_scene(root,altered);}catch(const std::runtime_error&){rejected=true;}
    check(rejected);
    auto corrupted=first;corrupted.summary->outcome=NativeReplayOutcome::Clear;
    auto bad=load_practice_replay_scene(root,corrupted);
    check(play(*bad,corrupted,no_fixture).step.status==Status::Failed);
    PracticeSpec boss;boss.stage=2;boss.section=PracticeSection::Boss;boss.boss_phase=5;boss.shot=0;boss.rank=0;boss.lives=5;
    const auto fight=[](const EncounterScene &s,unsigned i){
        const auto &stone=s.stones.stones[4];const auto dx=stone.x+12-(s.player.x+8);
        return practice_replay_tick(s,Shot|(dx>4?Right:dx<-4?Left:0)|(s.player.y>210?Up:0)|(i<3?Bomb:0),false,false);
    };
    const auto direct=capture(boss,boss.seed,20000,fight);
    check(direct.summary->outcome==NativeReplayOutcome::Clear);
}

void protection_display_is_read_only(const fs::path &root){
    NativeReplayConfig c;
    auto plain=load_replay_scene(root,c),with_info=load_replay_scene(root,c);
    // Begin at the entry boundary so the next two real scene updates cross 17/18.
    for(auto *scene:{&plain,&with_info}){
        scene->midboss.trigger(116);
        scene->midboss.phase_frame=16;
    }
    Capture without_hud(c),with_hud(c);
    constexpr std::array languages={Language::Japanese,Language::English,Language::ChineseSimplified,Language::ChineseTraditional};
    unsigned locked_frames=0;
    for(unsigned i=0;i<64;++i){
        const std::uint16_t input=i%2?Shot:0;
        const auto a=without_hud.tick(plain,input),b=with_hud.tick(with_info,input);
        check(a.status==b.status&&a.consumed_tick==b.consumed_tick&&a.stage_advanced==b.stage_advanced&&a.event==b.event);
        const auto hud=encounter_info(with_info);
        if(hud.protection.kind==EncounterProtectionKind::TimedLock){
            ++locked_frames;
            for(auto language:languages){
                const auto lines=encounter_protection_lines(hud.protection,language);
                check(!lines[0].empty()&&!lines[1].empty());
            }
        }
        check(replay_state_digest(plain)==replay_state_digest(with_info));
    }
    check(locked_frames==1);
    without_hud.finish(plain);with_hud.finish(with_info);
    check(without_hud.run.events==with_hud.run.events&&without_hud.run.summary==with_hud.run.summary);
}

void background_smoothing_is_read_only(const fs::path &root){
    std::vector<std::uint32_t> sharp(640*400),smooth(sharp.size());
    std::vector<std::uint32_t> intermediate(sharp.size());
    for(int stage:{0,5}){
        auto scene=EncounterScene::load(root,stage,1,stage==5?4:1,2,3);
        if(stage==5)scene.enable_extra_road();
        scene.tick(0);
        const auto digest=replay_state_digest(scene);
        scene.render(sharp);scene.render(smooth,true,true);
        check(sharp!=smooth&&replay_state_digest(scene)==digest);
        scene.render(intermediate,true,true,scroll_subpixels/2);
        check(intermediate!=sharp&&intermediate!=smooth&&replay_state_digest(scene)==digest);
        const auto &player=*scene.bank.sprites.at(0);
        unsigned opaque=0;
        for(unsigned y=0;y<player.height;++y)for(unsigned x=0;x<player.width;++x){
            if(player.pixels[y*player.width+x]==player.transparent)continue;
            const auto index=(scene.player.y+y)*640+scene.player.x+x;
            check(sharp[index]==smooth[index]&&sharp[index]==intermediate[index]);++opaque;
        }
        check(opaque>0);
        for(int y=384;y<400;++y)for(int x=32;x<416;++x)check(sharp[y*640+x]==smooth[y*640+x]);
        for(unsigned i=1;i<scene.scroll.interval;++i)scene.tick(0);
        const auto settled_digest=replay_state_digest(scene);
        check(background_scroll_lag(scene.scroll,scroll_subpixels)==0);
        scene.render(sharp);scene.render(smooth,true,true);
        check(sharp!=smooth&&replay_state_digest(scene)==settled_digest);
    }
}

void animated_backgrounds(const fs::path &root){
    std::vector<std::uint32_t> sharp(640*400),smooth(sharp.size()),next(sharp.size());
    const auto at=[](int x,int y){return y*640+x;};
    const auto scene_for=[&](int stage,unsigned index){
        auto scene=EncounterScene::load(root,stage,0,1,2,3);
        scene.script.formations.clear();scene.script.declared_formations=0;
        for(auto &c:scene.background.palette)c={0,0,0};
        for(auto &t:scene.background.tiles)t.fill(std::uint8_t(index));
        return scene;
    };
    const auto check_both=[&](const EncounterScene &scene){
        const auto before=replay_state_digest(scene);
        scene.render(sharp);scene.render(smooth,true,true);
        check(replay_state_digest(scene)==before);
        check(std::equal(sharp.begin()+at(0,16),sharp.begin()+at(0,300),smooth.begin()+at(0,16)));
    };
    {
        auto s=scene_for(0,8);
        s.enable_midboss();s.runtime.scroll.progress=116;
        s.tick(0);check_both(s);check(sharp[at(96,200)]==0);
        s.tick(0);check_both(s);check(sharp[at(96,200)]==0xb09090);
        for(unsigned i=2;i<550;++i)s.tick(0);
        check_both(s);check(sharp[at(96,200)]==0xb0b0d0);
    }
    {
        auto s=scene_for(1,0);
        s.background.palette[3]={17,85,153};
        for(unsigned i=0;i<52;++i)s.tick(0);
        check_both(s);
        const auto last=sharp.begin()+at(0,300);
        check(std::find(sharp.begin()+at(0,16),last,0x115599)!=last);
        MotionFrame previous;s.capture_motion(previous);
        s.tick(0);s.render(next);check(next!=sharp);
        s.render(smooth,true,true,128,&previous);check(smooth!=next&&smooth!=sharp);
        s.paused=true;s.tick(0);s.render(sharp);check(sharp==next);
    }
    {
        auto s=scene_for(2,0);
        s.background.palette[7]={255,255,255};
        for(unsigned i=0;i<248;++i)s.tick(0);
        check_both(s);
        check(sharp[at(64,64)]==0x4c0000);
        const auto ring=enemy_vector(49,48);
        check(sharp[at(224+ring[0],200+ring[1])]==0xffffff);
        for(unsigned i=248;i<400;++i)s.tick(0);
        check_both(s);check(sharp[at(64,64)]==0x006400);
    }
    {
        auto s=scene_for(3,0);
        s.background.palette[8]={128,128,128};
        s.background.tiles[0].fill(8);
        // Use a dark map tile independently of the intensity-plane motif.
        s.background.sections.fill(1);
        s.render(sharp);s.render(smooth,true,true);
        check(sharp[at(176,160)]==0x808080&&smooth[at(176,160)]==0x808080);
        // Full palette, fade out, hold invisible, fade in, full palette.
        for(auto [position,value]:std::array<std::pair<unsigned,unsigned>,7>{{
            {160,0x808080},{204,0x404040},{216,0},{256,0},{352,0},{372,0x404040},{400,0x808080}
        }}){
            s.scroll.display_pixels=position;
            s.rendered_page=0;s.render(sharp);
            s.rendered_page=1;s.render(next);
            s.render(smooth,true,true);
            check(sharp==next&&sharp[at(176,160)]==value&&smooth[at(176,160)]==value);
        }
        s.reduced_effects=true;s.render(sharp);check(sharp[at(176,160)]==0);
    }
    {
        auto s=EncounterScene::load(root,3,0,1,2,3);
        // In STAGE3.MPN the black center already has the intensity bit.
        // The mask becomes visible later, when the lower palette bank scrolls in.
        for(auto [position,visible]:std::array<std::pair<unsigned,bool>,4>{{
            {160,false},{4700,true},{4858,false},{5072,true}
        }}){
            s.scroll.display_pixels=position;
            s.reduced_effects=false;s.render(sharp);
            s.reduced_effects=true;s.render(next);
            check((sharp!=next)==visible);
            if(visible){
                const auto orange=0xff9900u;
                check(std::ranges::count(sharp,orange)>std::ranges::count(next,orange));
            }
        }
    }
    {
        auto s=EncounterScene::load(root,4,0,1,2,3);
        s.script.formations.clear();s.script.declared_formations=0;
        const auto purple=0x660055u;
        const auto advance_to=[&](unsigned scroll){
            while(s.scroll.display_pixels<scroll&&!s.runtime.blocked&&!s.runtime.scroll.done)s.tick(0);
            check(s.scroll.display_pixels==scroll);
            s.render(sharp);s.render(smooth,true,true);
        };
        // The tapered road is now a moving ramp, not just a purple/black cut.
        advance_to(512);
        for(const auto *frame:{&sharp,&smooth})check((*frame)[at(224,32)]==purple&&(*frame)[at(224,320)]==purple);
        advance_to(880);
        for(const auto *frame:{&sharp,&smooth}){
            const auto a=(*frame)[at(224,180)],b=(*frame)[at(224,230)],c=(*frame)[at(224,280)];
            check(0<a&&a<b&&b<c&&c<purple);
            check((*frame)[at(224,32)]==0&&(*frame)[at(200,370)]==purple);
        }
        const auto carried=sharp[at(224,230)];
        advance_to(944);
        check(sharp[at(224,294)]==carried&&sharp[at(224,230)]<carried);
        advance_to(1000);
        for(const auto *frame:{&sharp,&smooth})check((*frame)[at(224,32)]==0&&0<(*frame)[at(224,320)]&&(*frame)[at(224,320)]<purple);
        advance_to(1200);
        for(const auto *frame:{&sharp,&smooth})check((*frame)[at(224,32)]==0&&(*frame)[at(224,320)]==0);
        check(!s.mima_enabled);
    }
}

void boss_backgrounds(const fs::path &root){
    std::vector<std::uint32_t> sharp(640*400),smooth(sharp.size());
    const auto at=[](int x,int y){return y*640+x;};
    for(int stage=0;stage<6;++stage){
        PracticeSpec spec;spec.stage=stage;spec.rank=stage==5?4:1;
        PracticeBuilder practice(root,spec);
        if(practice.status()!=PracticeBuilder::Status::Ready)throw std::runtime_error(practice.error());
        auto s=practice.release();
        for(auto &c:s->background.palette)c={};
        if(stage<3){
            for(auto &tile:s->background.tiles)tile.fill(stage==0?8:0);
            s->background.palette[stage==1?3:7]={255,0,255};
            const auto before=replay_state_digest(*s);
            s->render(sharp);s->render(smooth,true,true);
            check(replay_state_digest(*s)==before);
            if(stage==0)check(sharp[at(64,160)]!=0&&smooth[at(64,160)]!=0);
            else{
                const auto first=sharp.begin()+at(0,150),last=sharp.begin()+at(0,300);
                check(std::any_of(first,last,[](auto pixel){return (pixel&0xff00ff)==0xff00ff;}));
            }
        }else if(stage==3){
            for(unsigned c:{9u,10u,12u,13u})s->background.palette[c]={255,0,255};
            for(unsigned i=0;i<24;++i)s->tick(0);
            const auto before=replay_state_digest(*s);
            s->render(sharp);s->render(smooth,true,true);
            check(replay_state_digest(*s)==before);
            for(auto *image:{&sharp,&smooth}){
                const auto first=image->begin()+at(0,17),last=image->begin()+at(0,64);
                check(std::count(first,last,0xff00ffu)>=8);
            }
        }else if(stage==4){
            for(unsigned i=0;i<16;++i)s->tick(0);
            MotionFrame previous;s->capture_motion(previous);
            s->tick(0);s->mima_palette0={52,32,68};
            const auto before=replay_state_digest(*s);
            s->render(sharp);s->render(smooth,true,true,128,&previous);
            check(sharp[at(64,64)]==0x342044&&smooth[at(64,64)]==0x1e1026);
            check(replay_state_digest(*s)==before);
        }else{
            s->background.palette[4]={255,0,255};
            s->tick(0);
            check(s->sigma.decor_visible&&s->sigma.decor_advanced);
            s->render(sharp);
            for(const auto p:{at(216,200),at(232,200),at(224,192),at(224,208)})
                check(sharp[p]==0xff00ff);
            check(sharp[at(208,224)]!=0xff00ff);
            MotionFrame previous;s->capture_motion(previous);
            s->tick(0);
            const auto before=replay_state_digest(*s);
            check(!s->sigma.decor_advanced);
            s->render(sharp);s->render(smooth,true,true,128,&previous);
            check(sharp[at(232,200)]==0xff00ff&&smooth[at(230,200)]==0xff00ff);
            check(replay_state_digest(*s)==before);
        }
    }
}

void boss_entrances(const fs::path &root){
    using D=StageDialogue;
    std::vector<std::uint32_t> image(640*400),next(image.size());
    const auto at=[](int x,int y){return y*640+x;};
    for(const auto [stage,box,peak]:std::array<std::array<unsigned,3>,4>{{
        {1,2,16},{3,D::stage4_intro_end,161},{3,D::stage4_generic_end,5},{5,D::extra_intro_end,18}}}){
        PracticeSpec spec;spec.stage=int(stage);spec.rank=stage==5?4:1;spec.dialogue=PracticeDialogue::Show;
        PracticeBuilder practice(root,spec);
        if(practice.status()!=PracticeBuilder::Status::Ready)throw std::runtime_error(practice.error());
        auto s=practice.release();
        s->tick(0);
        s->dialogue.current=box;s->dialogue.phase=D::Phase::IntroAppearance;
        s->appearance_frame=peak-1;
        s->background.palette[0]={};s->dialogue_snapshot.assign(640*400,0);
        s->tick(0);
        const auto before=replay_state_digest(*s);
        s->render(image);
        check(image[at(48,16)]==(stage==3&&box==D::stage4_generic_end?0xc0c0c0:0xffffff));
        s->reduced_effects=true;s->render(next);
        check(next[at(48,16)]<image[at(48,16)]);
        check(replay_state_digest(*s)==before);
        if(stage==3&&box==D::stage4_intro_end){
            s->reduced_effects=false;s->appearance_frame=12;
            s->background.palette[10]={17,17,17};
            s->background.palette[13]={221,0,0};
            s->background.palette[15]={0,255,0};
            s->render(image);
            check(image[at(224,144)]==0x111111);
            check(std::find(image.begin(),image.begin()+at(0,320),0xdd0000u)!=image.begin()+at(0,320));
            check(std::find(image.begin(),image.begin()+at(0,320),0x00ff00u)!=image.begin()+at(0,320));
        }
    }
}

void paced_background_positions(){
    constexpr std::uint64_t begin=2'000'000;
    for(unsigned hz:{60u,120u,144u})for(int stage:{0,3,4,5}){
        DisplayCadence cadence;cadence.reset(begin,hz);
        PresentationClock logic;
        StageScroll scroll(stage);
        unsigned steps=0,previous=0;
        for(unsigned frame=0;frame<=hz*3;++frame){
            const auto due=cadence.deadline();
            check(!cadence.consume(due-1)&&cadence.consume(due));
            check(due==begin+(std::uint64_t(frame)*1'000'000+hz-1)/hz);
            const auto tick=logic.advance(due/1000,true);
            steps+=tick;
            for(unsigned i=0;i<tick;++i)scroll.finish_tick(90);
            const auto phase=logic.subframe(1,scroll_subpixels,unsigned(due%1000));
            const auto position=scroll.display_pixels*scroll_subpixels-background_scroll_lag(scroll,phase);
            const auto elapsed_us=due-begin;
            const auto moving_us=elapsed_us>17'000?elapsed_us-17'000:0;
            const auto expected=moving_us*scroll.speed*scroll_subpixels/(17'000*scroll.interval);
            check(position>=previous&&position>=expected-std::min<std::uint64_t>(2,expected)&&position<=expected+2);
            if(frame>3)check(position>previous);
            previous=position;
        }
        check(steps==3000/17);
        // Rendering a still scene does not change the captured subframe.
        const auto paused=logic.subframe(1,scroll_subpixels);
        const auto pause_ticks=logic.advance(begin/1000+3050,false);
        check(!pause_ticks&&display_subframe(paused,logic.subframe(1,scroll_subpixels),pause_ticks)==paused);
        const auto resumed=logic.advance(begin/1000+3100,true);
        check(!resumed&&display_subframe(paused,logic.subframe(1,scroll_subpixels),resumed)==paused);
        const auto next=logic.advance(begin/1000+3117,true);
        check(next==1&&display_subframe(paused,logic.subframe(1,scroll_subpixels),next)==0);
        check(cadence.consume(begin+5'000'000)&&cadence.deadline()>begin+5'000'000);
    }

    constexpr int bottom=388;
    std::vector<std::uint32_t> source(640*400);
    for(unsigned y=15;y<bottom;++y){
        source[y*640+32]=y%2?0xff8040:0;
        source[y*640+33]=y<200?0u:0xffffffu;
    }
    unsigned previous_edge=0;
    for(unsigned lag=0;lag<=2*scroll_subpixels;lag+=8){
        auto pixels=source;
        filter_stage_background(pixels,lag,bottom);
        // The nyquist stripes no longer fade in and out in subpixel motion.
        for(unsigned y=16;y<384;++y)check(pixels[y*640+32]==0x7f4020);
        for(unsigned y:{15u,384u,385u,386u,387u})check(pixels[y*640+32]==0);
        unsigned edge=0;
        for(unsigned y=195;y<205;++y)edge+=pixels[y*640+33]&255;
        check(!lag||edge>previous_edge);
        previous_edge=edge;
    }
}

void measured_frame_rate(){
    constexpr std::uint64_t begin=2'000'000;
    DisplayCadence cadence;cadence.reset(begin,60);
    FrameRateMeter full,half;
    for(unsigned frame=0;frame<=60;++frame){
        const auto now=cadence.deadline();
        check(cadence.consume(now));
        full.submitted(now);
        if(frame%2==0)half.submitted(now);
        if(frame<60)check(!full.tenths(now)&&!half.tenths(now));
    }
    check(full.tenths(begin+1'000'000)==600&&half.tenths(begin+1'000'000)==300);
    constexpr auto later=begin+2'000'001;
    check(!full.tenths(later)&&!half.tenths(later));
    full.submitted(later);
    check(!full.tenths(later));
    half.reset();
    check(!half.tenths(begin+1'000'000));
}

void original_gameplay_lock(){
    constexpr std::uint64_t start=1'000'000;
    DisplayCadence cadence;cadence.reset(start);
    FrameRateMeter meter;
    LockedGameClock full,half,third;
    unsigned full_ticks=0,half_ticks=0,third_ticks=0;
    for(unsigned frame=0;frame<3400;++frame){
        const auto due=cadence.deadline();
        check(due==start+(frame*9'328'000'000ull+526314)/526315);
        check(!cadence.consume(due-1)&&cadence.consume(due)&&!cadence.consume(due));
        full_ticks+=full.advance(1);half_ticks+=half.advance(2);third_ticks+=third.advance(3);
        check(full_ticks==frame+1&&half_ticks==(frame+1)/2&&third_ticks==(frame+1)/3);
        check(full.subframe(1,256)==256&&half.subframe(2,256)==(frame%2?0:128));
        meter.submitted(due);
        if(frame>=57)check(meter.tenths(due)==564);
    }
    // A 100 ms overrun grants exactly one frame; no backlog or short follow-up.
    const auto stall=cadence.deadline()+100'000;
    check(cadence.consume(stall)&&cadence.late_us==100'000&&full.advance(1)==1);
    check(!cadence.consume(stall)&&cadence.deadline()==stall+17'724);
    check(cadence.consume(stall+17'724)&&full.advance(1)==1);
}

void interpolated_gameplay(const fs::path &root){
    auto scene=EncounterScene::load(root,0,0,1,2,3);
    scene.effects_enabled=true;
    for(auto &c:scene.background.palette)c={0,0,0};
    scene.background.palette[15]={255,255,255};scene.background.palette[14]={255,255,255};
    for(auto &t:scene.background.tiles)t.fill(0);
    const StageSprites::Sprite pixel{1,1,0,{15}};
    scene.bank.sprites[0]=pixel;scene.bank.sprites[68]=pixel;
    scene.player.x=100;scene.player.y=200;
    auto &item=scene.items.slots[0];item.flag=1;item.left[0]=160;item.top[0]=200*16;item.age=1;
    scene.sparks.add(220,200,30,1,false,0,true);
    auto &spark=scene.sparks.slots[0];spark.age=1;
    scene.sparks.add(240,200,30,1,false,0);
    auto &bullet=scene.bullet_core.bullets[0];bullet.flag=1;bullet.position[0]={280*16,200*16};
    MotionFrame prev;scene.capture_motion(prev);
    scene.player.x+=5;item.top[0]+=16;++item.age;spark.position[0][0]+=16;++spark.age;bullet.position[0][1]+=16;++scene.ticks;
    std::vector<std::uint32_t> old(640*400),mid(old.size()),current(old.size()),sharp(old.size());
    const auto at=[](const auto &frame,int x,int y){return frame[y*640+x];};
    const auto digest=replay_state_digest(scene);
    scene.render(old,true,true,0,&prev);scene.render(mid,true,true,128,&prev);scene.render(current,true,true,256,&prev);
    scene.render(sharp,true,false,128,&prev);
    check(at(old,100,200)==0xffffff&&at(mid,102,200)==0x7f7f7f&&at(mid,103,200)==0x7f7f7f&&at(current,105,200)==0xffffff);
    check(at(old,160,200)==0xffffff&&at(mid,160,200)==0x7f7f7f&&at(mid,160,201)==0x7f7f7f&&at(current,160,201)==0xffffff);
    check(at(old,220,200)==0x7f7f7f&&at(mid,220,200)==0x3f3f3f&&at(mid,221,200)==0x3f3f3f);
    check(at(current,221,200)==0x7f7f7f&&at(sharp,221,200)==0x7f7f7f&&at(mid,240,200)==0xffffff);
    check(at(old,282,200)==0xffffff&&at(mid,282,200)==0x7f7f7f&&at(current,282,200)==0);
    check(current==sharp&&digest==replay_state_digest(scene));
    // Replacing an item in the same slot must not animate it from the old item.
    item.age=1;item.top[0]=240*16;
    scene.render(mid,true,true,0,&prev);
    check(at(mid,160,200)==0&&at(mid,160,240)==0xffffff);
    // A stage change rejects unrelated coordinates even if tick counts happen to match.
    prev.stage=4;scene.render(mid,true,true,0,&prev);
    check(at(mid,100,200)==0&&at(mid,105,200)==0xffffff);

    auto run=EncounterScene::load(root,0,1,1,2,3),reference=run;
    for(unsigned i=0;i<180;++i){
        run.capture_motion(prev);
        const auto input=std::uint16_t(i%60<30?Left:Right);
        run.tick(input);reference.tick(input);
        for(unsigned phase:{0u,61u,199u,256u})run.render(mid,true,true,phase,&prev);
        check(replay_state_digest(run)==replay_state_digest(reference));
    }
}

void sprite_edges(const fs::path &root){
    constexpr auto white=0xffffffu;
    const auto at=[](int x,int y){return y*640+x;};
    std::vector<std::uint32_t> pixels(640*400),sharp(pixels.size());
    MotionFrame previous;
    auto make_scene=[&]{
        auto s=EncounterScene::load(root,0,0,3,2,3);
        s.combat_enabled=true;
        s.runtime.dispatch.cursor=1;
        for(auto &c:s.background.palette)c={0,0,0};
        s.background.palette[15]={255,255,255};
        for(auto &tile:s.background.tiles)tile.fill(0);
        return s;
    };
    const auto cel=[](unsigned height){return StageSprites::Sprite{1,height,0,std::vector<std::uint8_t>(height,15)};};
    {
        auto s=make_scene();
        auto &type=s.script.enemies[0];type.raw[2]=9;type.raw[3]=0;type.raw[8]=128;type.raw[9]=0;
        type.raw[10]=type.raw[11]=type.raw[26]=type.raw[27]=0;
        s.bank.sprites[128]=cel(16);
        s.script.paths[type.path_slot].bytes={0,255};
        StageScript::Formation row;row.trigger=0;row.selectors.fill(0xff);row.selectors[12]=0;
        s.script.formations={row};s.script.declared_formations=1;
        s.runtime.dispatch.cursor=0;s.runtime.scroll.pending_progress=true;
        s.tick(0);s.capture_motion(previous);s.render(pixels);
        check(s.runtime.enemies.slots[0].raw[0xe]==1&&s.runtime.enemies.high_water==0);
        check(!previous.at(MotionTrack::Enemy,0).visible&&pixels[at(120,16)]==0);
        s.tick(0);s.render(pixels,true,true,128,&previous);
        check(s.runtime.enemies.high_water==1&&pixels[at(120,16)]==white);
    }
    {
        auto s=make_scene();
        s.bank.sprites[52]=cel(16);
        auto &shot=s.shots.slots[0];shot.flag=1;shot.position[1]={200*16,9*16};shot.vy=-2*16;
        s.tick(0);check(shot.flag==2&&PlayerShots::pixel(shot.position[0][1])==7);
        const auto hash=replay_state_digest(s);
        s.render(pixels);check(pixels[at(200,15)]==0&&pixels[at(200,22)]==white&&pixels[at(200,23)]==0);
        check(replay_state_digest(s)==hash);
        // The logical slot is free again while the old sprite finishes leaving.
        s.tick(Shot);check(shot.flag==1);s.render(pixels);
        check(pixels[at(200,20)]==white&&pixels[at(200,21)]==0);
        s.tick(0);s.tick(0);s.tick(0);s.tick(0);s.render(pixels);
        check(pixels[at(200,16)]==0);
    }
    {
        auto s=make_scene();
        auto &e=s.runtime.enemies.slots[0];e.raw[0xe]=1;e.word(4,120);e.word(6,9);
        auto &type=s.script.enemies[0];type.raw[8]=128;type.raw[9]=0;type.raw[10]=0;type.raw[11]=0;
        type.raw[26]=type.raw[27]=0;s.bank.sprites[128]=cel(16);
        s.script.paths[type.path_slot].bytes={1,255,4,0};
        s.script.paths[type.path_slot].bytes={0,255};
        s.tick(0);e.raw[0xf]=255;s.capture_motion(previous);
        e.word(8,0);e.word(0xa,0);s.script.paths[type.path_slot].bytes={1,255,4,0};
        s.tick(0);check(e.raw[0xf]==0);
        s.render(pixels,true,true,128,&previous);s.render(sharp);
        check(pixels[at(122,16)]==white&&pixels[at(124,16)]==0&&sharp[at(124,16)]==white);
        // A position assignment is not movement, even when less than 64 pixels away.
        s.capture_motion(previous);
        e.word(8,0);s.script.paths[type.path_slot].bytes={4,17,1,0,255};
        s.tick(0);s.render(pixels,true,true,128,&previous);
        check(pixels[at(136,16)]==white&&pixels[at(130,16)]==0);
        // An outward road enemy gets its last visible rows even after pooling ends.
        s.bank.sprites[128]=cel(36);
        e.raw[0x1e]=1;e.word(8,0);e.word(0xa,0);e.word(s.rendered_page*4+2,std::uint16_t(-15));
        s.script.paths[type.path_slot].bytes={1,255,0,255};
        s.capture_motion(previous);s.tick(0);check(e.raw[0xe]==2);
        s.render(pixels);check(pixels[at(136,19)]==white&&pixels[at(136,20)]==0);
        // A script ending at that same coordinate is a cut, not a boundary exit.
        auto cut=make_scene();auto &other=cut.runtime.enemies.slots[0];
        cut.script.enemies[0]=type;cut.bank.sprites[128]=cel(36);
        cut.script.paths[type.path_slot].bytes={0x10};
        other.raw[0xe]=1;other.word(4,136);other.word(6,std::uint16_t(-16));
        cut.tick(0);cut.render(pixels);check(other.raw[0xe]==2&&pixels[at(136,19)]==0);
    }
    {
        auto s=make_scene();
        auto &e=s.runtime.enemies.slots[0];e.raw[0xe]=1;e.word(4,120);e.word(6,16);
        auto &type=s.script.enemies[0];type.raw[8]=128;type.raw[9]=0;type.raw[10]=0;type.raw[11]=0;
        type.raw[14]=type.raw[15]=0;type.raw[22]=1;type.raw[23]=0;
        type.raw[26]=type.raw[27]=0;s.bank.sprites[128]=cel(16);s.bank.sprites[10]=cel(1);
        s.script.paths[type.path_slot].bytes={0,255};
        auto &shot=s.shots.slots[0];shot.flag=1;shot.position[1]={121*16,16*16};shot.vy=-12*16;
        s.tick(0);check(e.raw[0x10]==1&&!s.death_patterns[0]);
        s.render(pixels);check(pixels[at(120,17)]==0);
        s.tick(0);check(s.death_patterns[0]==10);
        s.render(pixels);check(pixels[at(120,16)]==white&&pixels[at(120,17)]==0);
    }
    {
        auto s=make_scene();
        s.bank.sprites[88]=cel(16);
        s.bank.sprites[0]=cel(1);
        auto &bullet=s.bullet_core.bullets[0];bullet.flag=1;bullet.kind=EnemyBulletRequest::Bullet16;
        bullet.pattern=88;bullet.position[1]={260*16,4*16};bullet.velocity={0,-4*16};
        auto &hit=s.bullet_core.bullets[1];hit.flag=1;hit.position[1]={220*16,351*16};
        s.rendered_page=1;s.capture_motion(previous);s.tick(0);
        check(bullet.flag==2&&hit.flag==2&&s.hit_checks==1);
        s.render(pixels,true,true,128,&previous);s.render(sharp);
        check(pixels[at(260,17)]==white&&pixels[at(260,18)]==0&&sharp[at(260,16)]==0);
        check(pixels[at(224,355)]==0&&sharp[at(224,355)]==0);
    }
    // A real Lunatic stage shot moves from y=18 to y=6. Cel 52 still has
    // opaque rows inside the viewport, although its gameplay slot is retired.
    auto run=EncounterScene::load(root,0,0,3,2,3);
    run.combat_enabled=true;run.enable_effects();
    for(unsigned tick=0;tick<83;++tick)run.tick(Shot|(tick>=50&&tick<53?Up:0));
    run.render(pixels);
    auto terrain=run;terrain.combat_enabled=false;terrain.render(sharp);
    const auto shot=std::ranges::find_if(run.shots.slots,[&](const auto &s){
        return s.flag==2&&PlayerShots::pixel(s.position[run.rendered_page][1])==6&&s.render_pattern==52;
    });
    check(shot!=run.shots.slots.end());
    const auto x=PlayerShots::pixel(shot->position[run.rendered_page][0]);
    const auto width=run.bank.sprites[52]->width;
    for(int y=16;y<=20;++y){
        const auto row=pixels.begin()+at(x,y),blank=sharp.begin()+at(x,y);
        check(!std::equal(row,row+width,blank));
    }
    // The same inputs hit a real Stage 1 enemy while it spans the top edge.
    for(unsigned tick=83;tick<964;++tick)run.tick(Shot);
    auto &enemies=run.runtime.enemies.slots;
    const auto hit=std::ranges::find_if(enemies,[&](const auto &e){
        return e.raw[0xe]==1&&e.raw[0x10]&&enemy_s16(e.word(run.rendered_page*4+2))==14&&
               !run.death_patterns[&e-enemies.data()];
    });
    check(hit!=enemies.end());
    const auto slot=unsigned(hit-enemies.begin());
    run.tick(Shot);check(run.death_patterns[slot].has_value());
}

void pause_and_menu_return(const fs::path &root){
    NativeReplayConfig c;
    auto scene=load_replay_scene(root,c);
    Capture record(c);
    scene.paused=true;
    check(!record.tick(scene,Shot,true).consumed_tick&&scene.ticks==0&&record.run.events.empty());
    scene.paused=false;
    check(record.tick(scene,Shot|UpLeft|Up|Down|0x0200).event.input==(Shot|UpLeft|Up|Down));
    const auto state=replay_state_digest(scene);
    scene.paused=true;record.finish(scene);
    check(state==record.run.summary->state_hash&&record.run.summary->outcome==NativeReplayOutcome::MenuReturn);
    auto replay=load_replay_scene(root,c);
    finished(play(replay,bytes_roundtrip(record.run),no_fixture),record.run);
    auto altered=record.run;
    altered.events[0].input=Right;
    auto mismatch=load_replay_scene(root,c);
    check(play(mismatch,bytes_roundtrip(altered),no_fixture).step.status==Status::Failed);
}

bool is_focus_red(std::uint32_t rgb){
    const int r=int((rgb>>16)&255),g=int((rgb>>8)&255),b=int(rgb&255);
    return r-g>128&&r-b>100;
}
void focus_marker_appearance(){
    constexpr int x=224,y=357;
    constexpr auto at=[](int px,int py){return (y+py)*640+x+px;};
    constexpr CollisionPoint player{208,336};
    std::vector<std::uint32_t> pixels(640*400);
    for(const auto bg:{0x20283cu,0xf8eacbu}){
        std::fill(pixels.begin(),pixels.end(),bg);
        draw_player_focus_point(pixels,player);
        check(pixels[at(0,0)]==0xffffff&&pixels[at(1,0)]==0xffd9dc&&pixels[at(2,0)]==0xff697d);
        check(pixels[at(3,0)]==0xb7153d&&pixels[at(4,0)]==0x281020&&pixels[at(4,4)]==bg);
        for(int dy=-4;dy<=4;++dy)for(int dx=-4;dx<=4;++dx)
            check(pixels[at(dx,dy)]==pixels[at(-dy,dx)]);
        for(int dy=-12;dy<=8;++dy)for(int dx=-11;dx<=10;++dx)
            if(dx<-4||dx>4||dy<-4||dy>4)check(pixels[at(dx,dy)]==bg);
        std::fill(pixels.begin(),pixels.end(),bg);
        draw_player_focus_point(pixels,player,-16);
        check(pixels[at(-16,0)]==0xffffff&&pixels[at(-21,-8)]==bg&&pixels[at(0,0)]==bg);
    }
}
void focus_mode(const fs::path &root){
    focus_marker_appearance();
    NativeReplayConfig config;config.shot=2;
    auto live=std::make_unique<EncounterScene>(load_replay_scene(root,config));
    ReplaySession session(config);
    NativeReplayRecorder recorder(config);
    const auto tick=[&](std::uint16_t input){
        const auto step=session.tick(*live,normalize_replay_tick(input));
        check(step.status==Status::Running&&step.event.input==input&&recorder.record(step.event));
    };
    std::vector<std::uint32_t> pixels(640*400),without_marker(pixels.size());
    const auto at=[](int x,int y){return y*640+x;};
    MotionFrame previous;
    live->capture_motion(previous);
    tick(Focus|Right);
    check(live->player.x==209&&live->player.focus_rx==1);
    const auto state=replay_state_digest(*live);
    live->render(pixels);
    check(pixels[at(225,357)]==0xffffff&&is_focus_red(pixels[at(223,357)]));
    live->render(pixels,true,true,0,&previous);
    check(pixels[at(225,357)]==0xffffff&&is_focus_red(pixels[at(223,357)]));
    check(replay_state_digest(*live)==state&&live->collision_snapshot()==nullptr);
    live->paused=true;live->render(pixels);
    live->player.focused=false;live->render(without_marker);
    check(pixels==without_marker);
    live->player.focused=true;live->paused=false;

    tick(Focus|Right);check(live->player.x==211);
    tick(Right);check(live->player.x==214&&!live->player.focused);
    live->render(pixels);
    live->paused=true;live->render(without_marker);live->paused=false;
    check(pixels==without_marker);
    tick(Focus|UpLeft|Shot);
    check(live->player.x==213&&live->player.y==335&&live->player.focus_rx==-1&&live->player.focus_ry==-1);
    check(recorder.finalize(session.stop(*live)));
    const auto run=bytes_roundtrip(recorder.replay());
    check(run.events==recorder.replay().events);
    auto playback=std::make_unique<EncounterScene>(load_replay_scene(root,config));
    finished(play(*playback,run,no_fixture),run);
    check(replay_state_digest(*playback)==replay_state_digest(*live)&&playback->player.focused);

    // Shift is a movement modifier, never a dialogue or Continue key.
    live->runtime.scroll.done=true;live->scroll.done=true;live->start_preboss();
    check(live->dialogue.active()&&!live->player.focused);
    *playback=*live;
    for(unsigned i=0;i<48;++i){live->tick(Focus);playback->tick(0);}
    check(replay_state_digest(*playback)==replay_state_digest(*live));
    live->life.game_over=true;live->continue_choice=EncounterScene::ContinueChoice::Release;
    live->tick(Focus);check(live->continue_choice==EncounterScene::ContinueChoice::Choose&&!live->player.focused);
}
void touch_replay(const fs::path &root){
    NativeReplayConfig config;config.shot=2;
    auto live=std::make_unique<EncounterScene>(load_replay_scene(root,config));
    ReplaySession session(config);
    NativeReplayRecorder recorder(config);
    const auto tick=[&](std::uint64_t motion){
        auto input=normalize_replay_tick(0);input.pointer=motion;
        const auto step=session.tick(*live,input);
        check(step.status==Status::Running&&step.event.pointer==motion&&recorder.record(step.event));
    };
    const int start=live->player.x;
    tick(pointer_active|pointer_begin|(48ull<<16));
    check(live->player.x==start+3);
    tick(pointer_active);check(live->player.x==start+6);
    tick(0);check(live->player.x==start+6);
    tick(pointer_active|pointer_begin|pointer_unlimited|(48ull<<16));
    check(live->player.x==start+54);
    const int y=live->player.y;
    tick(pointer_active|pointer_begin|pointer_radial|(320ull<<12)|(16224ull<<26));
    check(live->player.x==start+56&&live->player.y==y-1);
    tick(pointer_active|pointer_radial);
    check(live->player.x==start+59&&live->player.y==y-2);
    tick(0);
    check(live->player.x==start+59&&live->player.y==y-2);
    check(recorder.finalize(session.stop(*live)));
    const auto run=bytes_roundtrip(recorder.replay());
    check(run.events==recorder.replay().events);
    auto playback=std::make_unique<EncounterScene>(load_replay_scene(root,config));
    finished(play(*playback,run,no_fixture),run);
    check(replay_state_digest(*playback)==replay_state_digest(*live)&&playback->player.x==live->player.x);
}
} // namespace

int main(int argc,char **argv){
    try{
        if(argc<2||argc>3)throw std::runtime_error("Usage: soew-replay-session-tests <staged assets> [frozen stage 2 v4 replay | --background-smoothing | --focus-mode | --sprite-edges]");
        const fs::path root=argv[1];
        if(argc==3&&std::string_view(argv[2])=="--focus-mode"){
            focus_mode(root);
            touch_replay(root);
            std::cout<<"focus_and_touch_checks="<<checks<<" failures=0\n";
            return 0;
        }
        if(argc==3&&std::string_view(argv[2])=="--sprite-edges"){
            sprite_edges(root);
            std::cout<<"sprite_edge_checks="<<checks<<" failures=0\n";
            return 0;
        }
        if(argc==3&&std::string_view(argv[2])=="--background-smoothing"){
            background_smoothing_is_read_only(root);
            animated_backgrounds(root);
            boss_backgrounds(root);
            boss_entrances(root);
            paced_background_positions();
            measured_frame_rate();
            original_gameplay_lock();
            interpolated_gameplay(root);
            std::cout<<"background_smoothing_checks="<<checks<<" failures=0\n";
            return 0;
        }
        if(argc==3){
            std::ifstream file(argv[2],std::ios::binary);
            check(bool(file));
            const native_replay_detail::Bytes bytes{std::istreambuf_iterator<char>(file),{}};
            check(bytes.size()>=native_replay_detail::v4_header_size&&native_replay_detail::get(bytes,8,2)==4);
            const auto run=native_replay_detail::decode(bytes);
            check(run.practice&&run.summary&&run.config.stage==1&&run.practice->spec.chapter_id==50);
            check(run.events.size()==647&&run.practice->start_hash==4037019907930527065ull&&run.summary->state_hash==5108721023409443306ull);
            check(std::any_of(run.events.begin(),run.events.end(),[](const auto &e){return e.context==NativeReplayContext::Dialogue;}));
            auto scene=load_practice_replay_scene(root,run);
            finished(play(*scene,run,no_fixture),run);
            check(scene->meira_enabled);
            std::cout<<"Frozen v4 stage 2: 647 samples, dialogue and Meira handoff verified\n";
        }
        replay_intro_speed(root);
        host_timing_preserves_replay(root);
        practice_language_baseline(root);
        legacy_preboss_recordings(root);
        pause_and_menu_return(root);
        protection_display_is_read_only(root);
        practice_recordings(root);
        loss_and_continue(root);
        stage_boundary(root);
        input_only_stage5_clear(root);
        campaign_clear(root);
        protected_clear(root,false);
        protected_clear(root,false,true);
        if(fs::exists(root/"STAGE5.TXT")){protected_clear(root,true);extra_loss(root);}
        else std::cout<<"Extra fixture skipped: staged STAGE5.TXT not present\n";
        std::cout<<"replay_session_checks="<<checks<<" failures=0\n";
        return 0;
    }catch(const std::exception &e){std::cerr<<e.what()<<'\n';return 1;}
}
