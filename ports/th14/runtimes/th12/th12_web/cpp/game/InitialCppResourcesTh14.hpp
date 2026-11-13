#pragma once
#include "GameSession.hpp"
#include "ShotFiles.hpp"
#include "FrameStatistics.hpp"
#include "Th14MusicQueue.hpp"
#include "MusicCatalog.hpp"
#include "StageCppWorkerTh14.hpp"
#include "BootFileMailboxTh14.hpp"
#include "StageCopyTailTh14.hpp"
namespace th11 {
// Owns CPU decode scratch and the initial Player/Scene preparation writer.
// Main alone uploads textures and interprets resource templates, then ACKs.
// Native's remaining constructor/callback tail is not certified by this job.
class InitialCppResourcesTh14 {
    GameSession& session;GameResources reader;StageResources& stage;FrameStatistics& statistics;Th14MusicQueue& music;
    BootFileMailboxTh14 mailbox;StageCppWorkerTh14 worker;
    std::atomic<int> stopping{0};bool started=false;std::string error;
    bool failed(const char* message){if(error.empty())error=message;return false;}
public:
    InitialCppResourcesTh14(GameSession& value,const GameResources& source,StageResources& output,FrameStatistics& fps,Th14MusicQueue& requests):
        session(value),reader(source),stage(output),statistics(fps),music(requests),mailbox(value.stage_load_host_flags){}
    ~InitialCppResourcesTh14(){cancel_and_join();}
    bool begin(){
        if(started||!session.initial_battle_private()||session.resources.stage.get()!=&stage||
           session.state.stage!=stage.stage_number||session.battle)return false;
        started=true;
        return worker.start([this](){
            StageCppWorkerScopeTh14 scope(&session);
            AnmExecutionContextTh14 execution(static_cast<AnmEnvironment*>(&session.animations));
            SessionFailureScopeTh14 errors(&session,error);
            if(stopping.load(std::memory_order_acquire))return false;
            if(!session.compositor.stage_start())return failed("initial compositor stage reset");
            session.animations.rate=1;
#if defined(TH14_CPP_STAGE_DECODE)
            if(auto* control=session.animations.th14_control.get())control->flags&=~0x4000u;
#endif
            const auto character=u32(session.state.character),subtype=u32(session.state.subtype);
            if(!session.construct_initial_player())return false;
            if(!session.resources.load_selected_player_from(reader,character,subtype))return failed(session.resources.error.c_str());
            if(!mailbox.publish_file(session.resources.core.players[character],9))return failed("initial player file wait cancelled");
            if(stopping.load(std::memory_order_acquire))return false;
            // Retail44c4c6..44c51a: SHT follows the completed file9 wait.
            // A new initial owner reads its selected file; later-stage owners
            // retain their existing selected SHT storage.
            const u32 shot_slot=shot_file_slot(character,subtype);
            if(!reader.open_sht(shot_file_name(shot_slot),session.resources.core.shots[shot_slot]))
                return failed(reader.error().c_str());
            if(!session.prepare_initial_player())return false;
            if(!session.prepare_initial_scene(false))return false;
            const auto complete=[this](AnmResource& resource,u16 file){return mailbox.publish_file(resource,file);};
            if(!reader.load_stage_scene_files(stage.stage_number,stage,complete))return failed(reader.error().c_str());
            if(!session.prepare_initial_scene(true))return false;
            const auto choice=u32(session.state.character*2+session.state.subtype);
            if(!reader.load_stage_hud_files(stage.stage_number,stage,choice,complete))return failed(reader.error().c_str());
            session.battle->hud.prepare_stage(&session.battle->countdown_seconds);
            session.battle->initialize_initial_pools();
            // Retail435f22 creates the persistent Pause owner after the empty
            // bullet/item/laser managers, before435f66 initializes ECL.
            if(!session.prepare_initial_pause())return false;
            if(!reader.load_stage_timeline_files(stage.stage_number,stage,complete))return failed(reader.error().c_str());
            if(stopping.load(std::memory_order_acquire))return false;
            if(!session.battle->prepare_initial_tail_owners(character,subtype))return failed(session.battle->error.c_str());
            // 435fcc: demo omits initial audio. Spell practice omits only
            // dummy, retaining both preload requests. Each enqueue has its
            // own audio critical section, allowing Main to consume between.
            const char* names[2]={nullptr,nullptr};
            for(u32 slot=0;slot<2;++slot){
                const auto cue=stage_music(i32(stage.stage_number),slot!=0);
                for(const auto& track:music_tracks)if(track.cue==cue){names[slot]=track.file;break;}
                if(!names[slot])return failed("Initial music cue unavailable");
            }
            const u32 mode_flags=session.state.demo?0x40u:session.state.spell_practice_id>=0?0x20u:session.state.practice?0x10u:0u;
            music.enqueue_initial(mode_flags,(session.persistent_config().bytes[0x38]&0x10)!=0,names[0],names[1]);
            // 43600b/436010 precede the actual436015 Control clock reset.
            // The world remains private; Main observes these ordinary Timer
            // fields only after the worker's Control-enable release.
            statistics.reset_run();
            session.battle->th14_control_clock.set(0,&session.animations.rate);
            // 43601a checks the queue word itself, not a generic busy flag
            // or the Main consumer's return value. Main keeps pumping while
            // the controller remains private/loading; no SDL runs here.
            while(music.head_command()&&!stopping.load(std::memory_order_acquire)){
#if defined(__wasm__)
                __builtin_wasm_memory_atomic_wait32(reinterpret_cast<int*>(&stopping),0,16000000);
#else
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
#endif
            }
            if(stopping.load(std::memory_order_acquire))return false;
            // 436044->42de20 precedes Supervisor family cleanup and globals.
            // This private owner may preserve a nonzero section-frame word
            // when the selected section is already zero; do not clear both.
            session.battle->enemy_commands.th14_set_stage_section(0);
            // 436060 precedes the busy/finished stores and four interrupts.
            // Supervisor family cleanup and remaining globals are separate
            // unqualified work; this accepts only this scalar mask operation.
#if defined(TH14_CPP_STAGE_DECODE)
            if(auto* control=session.animations.th14_control.get())control->flags&=~4u;
#endif
            session.battle->enemy_commands.th14_complete_loading_flags();
#if defined(TH14_CPP_STAGE_DECODE)
            worker.publish_loading_completion();
            session.animations.th14_effect_stop=0; // Native43607b.
#endif
            // 436099/4360a4/4360af/4360ba defer label2 on all four copies.
            // Do not call update here. Native Control callbacks are enabled
            // only afterward; Main compositor updates remain held while
            // this private initial owner is loading. Remaining global and
            // callback publication/cleanup is still unqualified.
#if defined(TH14_CPP_STAGE_DECODE)
            th14_defer_stage_copy_tail(session.compositor.copies);
            if(auto* control=session.animations.th14_control.get())control->callbacks_enabled=1;
#else
            for(auto& copy:session.compositor.copies)copy.interrupt(2);
#endif
            return !stopping.load(std::memory_order_acquire);
        },0,[this](){
            // The shared435ab0 entry waits on captures. The first/new path
            // takes 435ba4's nonzero branch and skips the next-stage60ms wait.
            if(session.animations.first_deferred_capture_file_index()<0)return ReplayRngRestoreResultTh14::Ready;
            return (__atomic_load_n(&session.stage_load_host_flags,__ATOMIC_ACQUIRE)&0x180u)?
                ReplayRngRestoreResultTh14::Failed:ReplayRngRestoreResultTh14::Waiting;
        }
#if defined(TH14_CPP_STAGE_DECODE)
        ,[this](){if(auto* control=session.animations.th14_control.get())control->flags|=4u;}
#endif
);
    }
    bool pending(AnmResource*& resource,u16& file)const noexcept{return mailbox.pending_file(resource,file);}
    bool complete(AnmResource& resource,u16 file)noexcept{return mailbox.complete_file(resource,file);}
    ReplayRngRestoreResultTh14 poll()const noexcept{return worker.poll();}
    bool has_started()const noexcept{return started;}
#if defined(TH14_CPP_STAGE_DECODE)
    bool loading_callback_allows_close()const noexcept{return started&&worker.loading_callback_allows_close();}
    bool control_ready()const noexcept{
        const auto* control=session.animations.th14_control.get();
        return started&&control&&u32(control->callbacks_enabled)!=0;
    }
#if defined(TH14_WORKER_RETURN_OBSERVER)
    bool observes_worker(const StageCppWorkerTh14* value)const noexcept{return value==&worker;}
#endif
#endif
    void join()noexcept{worker.join();}
    const std::string& error_after_join()const noexcept{return error;}
    void cancel_and_join()noexcept{stopping.store(1,std::memory_order_release);mailbox.cancel();worker.cancel_and_join();}
};
}
