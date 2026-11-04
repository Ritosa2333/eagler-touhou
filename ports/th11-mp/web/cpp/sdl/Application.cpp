#include <eagler/input/BrowserGamepad.hpp>
#include "GraphicsDevice.hpp"
#include "AudioDevice.hpp"
#include "FontDevice.hpp"
#ifdef TH_ENABLE_THPRAC
#include "ThpracUi.hpp"
#include "../game/PracticeSections.hpp"
#if TH11_DEVELOPMENT_HARNESS
#include "../game/Localization.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#endif
#endif
#include "../game/PracticeBgm.hpp"
#include "../game/MusicCatalog.hpp"
#include "../game/GameSession.hpp"
#ifdef TH11_MULTIPLAYER
#include "../multiplayer/NetplayRuntime.hpp"
#include "../multiplayer/InputLanes.hpp"
#include "../multiplayer/ReplayArchive.hpp"
#include "../multiplayer/ReplayStorage.hpp"
#include "../multiplayer/SpectatorStageState.hpp"
#include "../multiplayer/ReplayStageSeek.hpp"
#include "../game/WorldRollback.hpp"
#include "../game/RollbackCapture.hpp"
#include "../multiplayer/FrameSchedule.hpp"
#include "../multiplayer/PresentationSchedule.hpp"
#endif
#include "../game/FrameStatistics.hpp"
#include "../game/AnmRenderer.hpp"
#include "../game/RuntimeCost.hpp"
#include <SDL3/SDL.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include "../../../portable/sdl/FrameCadence.hpp"
#include "../../../portable/sdl/PresentationCadence.hpp"
#include "../game/HighRefresh.hpp"
#include "../game/PresentationAudit.hpp"
#include "../../../portable/input/TouchController.hpp"
#include <eagler/thprac/PracticePointer.hpp>
#include <algorithm>
EM_JS(int, th11_browser_keyboard, (), { return typeof Module["resetBrowserKeyboard"] === "function"; });
EM_JS(void, th11_reset_browser_keyboard, (), { Module["resetBrowserKeyboard"]?.(); });
EM_JS(void, th11_browser_frame, (int ok,double milliseconds,unsigned ticks), { Module["onGameFrame"]?.(ok,milliseconds,ticks); });
#ifdef TH11_MULTIPLAYER
EM_JS(void, th11_request_recovery_task, (unsigned epoch), {
 if(typeof MessageChannel!=='function'||document.hidden)return;
 if(!Module.th11RecoveryChannel){
  const channel=Module.th11RecoveryChannel=new MessageChannel();
  channel.port1.onmessage=event=>{
   Module.th11RecoveryQueued=false;if(document.hidden)return;
   Module.onRecoveryTaskStart?.(event.data.requested);
   try{Module._th11_mp_continue_recovery(event.data.epoch);}
   finally{Module.onRecoveryTaskEnd?.();}
  };
 }
 if(Module.th11RecoveryQueued)return;
 Module.th11RecoveryQueued=true;
 Module.th11RecoveryChannel.port2.postMessage({epoch,requested:performance.now()});
});
#endif
EM_JS(int, th11_limit_presentation_to_60, (), { return Module["eaglerOptions"]?.limitPresentationTo60 ? 1 : 0; });
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <sys/stat.h>
#include <dirent.h>

namespace th11::sdl {
namespace {
void clear_inputs();
void reset_cadences();
struct Application:StageResourceEffects {
    GraphicsDevice graphics;
    AudioDevice audio;
#ifdef TH_ENABLE_THPRAC
    PracticeBgm practice_bgm;
#endif
    FontDevice fonts{graphics};
    GameResources resources;
    GameSession session;
    FrameStatistics frame_statistics;
    // Display-rate readout. Counted once per visible frame (the presentation
    // pass when high refresh is active), never per simulation tick.
    double presentation_origin=0;u32 presentation_frames=0;float presentation_fps=0;
    AsciiText frame_text{session.resources.core.ascii};
    AnmRenderer renderer;
    std::vector<u8> archive;
    std::vector<u8> saved_score_state,saved_config_state;
    bool initialized=false,platform_prepared=false,room_preview=false;
    AnmResource loading_signature;
    AnmManager loading_animations;
    AnmVm* loading_credit=nullptr;
    std::string error;

#ifdef TH11_MULTIPLAYER
    multiplayer::NetplayRuntime netplay;
    multiplayer::ReplayArchive multiplayer_replay;
    std::vector<u8> multiplayer_export;
    bool multiplayer_world=false,multiplayer_advanced=false,multiplayer_finish_pending=false;
    bool multiplayer_restart_pending=false,multiplayer_saved=false,multiplayer_pause_requested=false,multiplayer_local_results=false;
    u32 multiplayer_seek_target=0,multiplayer_seek_stage=0,multiplayer_last_stage=0;
    std::string multiplayer_auto_path;
    std::array<u32,4> multiplayer_build{};
    bool multiplayer_viewer=false;
    u32 multiplayer_spectator_stage=Netplay::INVALID_FRAME;
    u32 multiplayer_spectator_checkpoint_serial=0;
    u32 multiplayer_spectator_capture_attempts=0,multiplayer_spectator_capture_failures=0;
    u32 multiplayer_spectator_capture_frame=Netplay::INVALID_FRAME,multiplayer_spectator_capture_stage=0;
    u32 multiplayer_spectator_capture_reason=0,multiplayer_spectator_capture_bytes=0;
    std::vector<u8> multiplayer_spectator_pending_initializer;
    u32 multiplayer_spectator_pending_stage=0;
    u32 multiplayer_replay_index=0,multiplayer_replay_stage=1;
    GameInput multiplayer_viewer_input;
    bool multiplayer_build_matches(const multiplayer::ReplayArchive& replay)const {
        return std::any_of(multiplayer_build.begin(),multiplayer_build.end(),[](u32 word){return word!=0;})&&replay.Setup().build==multiplayer_build;
    }
    bool multiplayer_completed()const {
        return session.state.phase==GameSessionPhase::ending||(session.title&&session.title->multiplayer_result)||
            (session.pause_menu&&session.pause_menu->multiplayer_result&&session.pause_menu->completed);
    }
    u32 multiplayer_recorded_score()const {
        return netplay.AllowsRollback()?rollback_committed_score:u32(std::max(0,session.economy.score_units));
    }
    bool multiplayer_recorded_completed()const {
        return (netplay.AllowsRollback()?rollback_committed_completed:multiplayer_completed())&&!multiplayer_restart_pending;
    }
    static void multiplayer_preview(ReplayMenuEntry& entry,const multiplayer::ReplayArchive& replay){
        entry.is_multiplayer=true;auto& out=entry.multiplayer;const auto& setup=replay.Setup();const auto& info=replay.Info();
        out.name=replay.Name();out.timestamp=replay.Timestamp();out.score=replay.Score();out.completed=replay.Completed();
        out.selection=setup.selections[0];out.difficulty=setup.difficulty;
        out.player_count=setup.player_count;out.recorded_player=setup.local_player;
        for(u32 i=0;i<info.chapterCount;++i){const auto& chapter=info.chapters[i];
            out.stages[chapter.label]=true;out.stage_frames[chapter.label]=chapter.firstFrame;out.last_stage=chapter.label;
        }
    }
    bool multiplayer_open_replay_menu(){
        rollback_release();
        if(!initialize())return false;
        netplay.Clear();multiplayer_viewer=true;multiplayer_world=false;multiplayer_advanced=false;
        multiplayer_finish_pending=multiplayer_restart_pending=multiplayer_pause_requested=multiplayer_local_results=false;
        multiplayer_seek_target=multiplayer_seek_stage=0;multiplayer_viewer_input={};
        renderer.invalidate();renderer.multiplayer_animations=nullptr;renderer.multiplayer_hud_background=nullptr;renderer.multiplayer_opacity.fill(255);
        session.multiplayer_active=false;session.animations.rate=1;
        if(!session.open_title(resources,false,TitleScreen::Replays)){error=session.error;return false;}
        session.title->multiplayer_replay_menu=true;
        session.title->replay_files.resize(std::max<size_t>(100,(multiplayer_replay_index/25+1)*25));
        session.title->last_replay=int(multiplayer_replay_index);
        clear_inputs();audio.effects.reset();return tick(0);
    }
    bool multiplayer_replay_seeking()const{
        return netplay.Playback()&&multiplayer::ReplayStageSeeking(netplay.NextFrame(),
            multiplayer_seek_target,multiplayer_replay.FrameCount(),multiplayer_seek_stage,
            session.state.stage,session.battle&&session.battle->stage_active);
    }
    bool multiplayer_load_replay(const u8* bytes,u32 size,u32 stage=0){
        multiplayer::ReplayArchive replay;
        if(!replay.Load(bytes,size)){error="Invalid TH11 multiplayer Replay";return false;}
        if(!multiplayer_build_matches(replay)){error="TH11 multiplayer Replay requires the same Runtime build";return false;}
        if(!stage)stage=replay.Setup().initial_stage();
        u32 target=0;bool found=false;const auto& info=replay.Info();
        for(u32 i=0;i<info.chapterCount;++i)if(info.chapters[i].label==stage){target=info.chapters[i].firstFrame;found=true;break;}
        if(!found){error="Stage absent from TH11 multiplayer Replay";return false;}
        error.clear();multiplayer_replay=std::move(replay);
        if(!netplay.BeginPlayback(multiplayer_replay.Setup())){error=netplay.Error();return false;}
        multiplayer_viewer=true;multiplayer_replay_stage=stage;multiplayer_viewer_input={};
        if(!multiplayer_begin())return false;
        // Chapters are input positions, not world snapshots. Restore frame zero
        // and replay every prior confirmed input through the native owner.
        multiplayer_seek_target=target;multiplayer_seek_stage=stage;return true;
    }
    bool multiplayer_menu_metadata(){
        if(session.title&&session.title->replay_scan_requested)scan_replays();
        if(session.title&&session.title->screen==TitleScreen::ReplaySave){auto& menu=*session.title;
            if(menu.substate==3&&!menu.pending_replay){
                auto entry=std::make_shared<ReplayMenuEntry>();multiplayer_preview(*entry,multiplayer_replay);
                entry->multiplayer.name="        ";entry->multiplayer.score=u32(session.economy.score_units);
                if(netplay.Spectator()){
                    // Spectator history does not carry the recording's wall
                    // clock. Keep that field unknown, with the real room facts.
                    auto& preview=entry->multiplayer;const auto& setup=netplay.Setup();
                    preview.selection=setup.selections[0];preview.difficulty=setup.difficulty;
                    preview.player_count=setup.player_count;preview.recorded_player=0;preview.last_stage=session.state.stage;
                }
                entry->multiplayer.completed=multiplayer_completed();menu.pending_replay=std::move(entry);
            }
        }
        if(session.pause_menu){auto& menu=*session.pause_menu;
            if(menu.scan_requested)scan_replays();
            if(menu.recording_metadata_requested)menu.timestamp=multiplayer_replay.Timestamp();
        }
        return true;
    }
    bool multiplayer_menu_save(){
        // Postgame menus use each browser's own input and save directory.
        // Replay consumers never write a player's recording.
        if(session.title&&session.title->replay_save_requested){auto& menu=*session.title;
            if((!netplay.ReadOnly()||multiplayer_local_results)&&!multiplayer_save(menu.replay_file+1,menu.entered_name.data()))return false;
            menu.replay_save_requested=false;menu.pending_replay.reset();scan_replays();
        }
        if(session.pause_menu&&session.pause_menu->save_requested){auto& menu=*session.pause_menu;
            if((!netplay.ReadOnly()||multiplayer_local_results)&&!multiplayer_save(menu.cursor.selected+1,menu.entered_name.data()))return false;
            menu.save_requested=false;scan_replays();
        }
        return true;
    }
    bool multiplayer_begin(u32 stage_override=0,bool reset_clock=true,const std::array<ReplayStageState,3>* stream=nullptr){
        rollback_release();
        if(!netplay.Spectator())audio.spectator_silent(false);
        if(reset_clock)reset_cadences();
        multiplayer_spectator_stage=Netplay::INVALID_FRAME;
        multiplayer_spectator_checkpoint_serial=0;
        multiplayer_spectator_capture_attempts=multiplayer_spectator_capture_failures=0;
        multiplayer_spectator_capture_frame=Netplay::INVALID_FRAME;multiplayer_spectator_capture_stage=0;
        multiplayer_spectator_capture_reason=multiplayer_spectator_capture_bytes=0;
        multiplayer_spectator_pending_initializer.clear();multiplayer_spectator_pending_stage=0;
        if(!netplay.Configured()||!initialize())return false;
        const auto& setup=netplay.Setup();MultiplayerOptions opts;
        opts.seat_count=setup.player_count;opts.local_seat=setup.local_player;opts.spectator=netplay.Spectator();
        opts.stage=stage_override?stage_override:setup.initial_stage();opts.difficulty=int(setup.difficulty);opts.seed=setup.seed;opts.challenge=setup.challenge;
        for(unsigned s=0;s<3;++s)opts.selections[s]=int(setup.selections[s]);
        renderer.invalidate();
        auto& core=session.resources.core;
        for(AnmResource* file:{&core.text,&core.ascii,&core.bullet,&core.enemy,&core.front,&core.title,&core.title_variant,&core.players[0],&core.players[1]})graphics.unload(*file);
        if(session.resources.stage)for(AnmResource* file:{&session.resources.stage->background,&session.resources.stage->logo,&session.resources.stage->enemies})graphics.unload(*file);
        if(!session.begin_multiplayer(resources,opts,stream)){error=session.error;return false;}
        for(AnmResource* file:{&core.text,&core.ascii,&core.bullet,&core.enemy,&core.front,&core.title,&core.title_variant,&core.players[0],&core.players[1],&session.resources.stage->background,&session.resources.stage->logo,&session.resources.stage->enemies})
            if(!graphics.preload(*file)){error=graphics.error;return false;}
        multiplayer_world=true;multiplayer_advanced=false;multiplayer_finish_pending=multiplayer_restart_pending=multiplayer_saved=multiplayer_pause_requested=multiplayer_local_results=false;clear_inputs();
        multiplayer_last_stage=opts.stage;multiplayer_spectator_stage=opts.stage;
        multiplayer_seek_target=multiplayer_seek_stage=0;multiplayer_auto_path.clear();frame_statistics.reset_run();
        if(netplay.Playback())session.recording_timestamp=multiplayer_replay.Timestamp();
        if(!play_stage_music(session.state.stage)||!audio_events()||!text_events()||!rollback_initialize())return false;
        refresh_multiplayer_touch_frontier();return true;
    }
    bool multiplayer_capture_spectator_checkpoint(u32 frame,u32& stage,std::vector<u8>& initializer){
        const bool stable=session.state.phase==GameSessionPhase::stage&&session.battle&&session.battle->stage_active;
        u32 previous=0;
        if(stable||!frame||!netplay.SpectatorRecordedStage(frame-1,previous))stage=session.state.stage;
        else stage=previous;
        initializer.clear();if(!netplay.SpectatorCheckpointRequired(frame,stage))return true;
        ++multiplayer_spectator_capture_attempts;multiplayer_spectator_capture_frame=frame;
        multiplayer_spectator_capture_stage=stage;multiplayer_spectator_capture_reason=0;multiplayer_spectator_capture_bytes=0;
        std::vector<u8> candidate;
        if(!multiplayer::CaptureSpectatorStageState(session,frame,candidate)){
            ++multiplayer_spectator_capture_failures;
            if(!session.multiplayer_active)multiplayer_spectator_capture_reason=1;
            else if(session.state.phase!=GameSessionPhase::stage)multiplayer_spectator_capture_reason=2;
            else if(!session.battle)multiplayer_spectator_capture_reason=3;
            else if(!session.battle->stage_active)multiplayer_spectator_capture_reason=4;
            else if(session.battle->transitioning||session.battle->outgoing_stage)multiplayer_spectator_capture_reason=5;
            else if(!session.battle->stage)multiplayer_spectator_capture_reason=6;
            else for(u32 i=0;i<session.multiplayer_options.seat_count;++i)
                if(!session.battle->pilots[i]||!session.battle->pilots[i]->player){multiplayer_spectator_capture_reason=7+i;break;}
            if(!multiplayer_spectator_capture_reason)multiplayer_spectator_capture_reason=10;
            return false;
        }
        multiplayer::SpectatorStageState decoded;
        if(!multiplayer::DecodeSpectatorStageState(candidate,decoded)||decoded.stage!=stage){
            ++multiplayer_spectator_capture_failures;multiplayer_spectator_capture_reason=11;return false;
        }
        multiplayer_spectator_capture_bytes=u32(candidate.size());
        initializer=std::move(candidate);multiplayer_spectator_stage=stage;return true;
    }
    bool multiplayer_spectator_playback_tick(bool forward_due){
        if(!multiplayer_pump()||!forward_due||!netplay.StagePlaybackReady())return error.empty();
        auto restore=[&](const Netplay::SpectatorStageEvent& event){
            audio.spectator_silent(true);
            multiplayer::SpectatorStageState state;
            if(event.kind!=Netplay::SpectatorEventKind::Checkpoint||event.schema!=multiplayer::SpectatorStageSchema||
               !multiplayer::DecodeSpectatorStageState(event.state,state)||state.frame!=event.frame||state.stage!=event.stage)return false;
            multiplayer_world=false;multiplayer_viewer=true;
            // The outer callback already owns its due forward tick. Native
            // construction must not erase that offer before it is completed.
            if(!multiplayer_begin(state.stage,false,&state.players))return false;
            if(!multiplayer::ApplySpectatorStageState(session,state)){
                error="TH11 streaming Replay stage initialization failed";return false;
            }
            multiplayer_spectator_stage=state.stage;multiplayer_world=true;multiplayer_viewer=true;
            return true;
        };
        auto& playback=netplay.StagePlayback();
        const bool fast_forwarding=playback.FastForwarding();
        audio.spectator_silent(playback.Paused()||fast_forwarding);
        auto tick=[&](const Netplay::SpectatorFramePacket& packet,bool historical){
            audio.spectator_silent(historical||fast_forwarding);
            if(!multiplayer_world||packet.frame!=session.multiplayer_frame)return false;
            std::array<MultiplayerInput,3> inputs{};
            if(!multiplayer::InputLanes::Decode(packet.inputs.data(),netplay.Setup().player_count,inputs)||
               !session.update_multiplayer(inputs)||!multiplayer_menu_metadata()||!capture_pause_frame()||
               !audio_events()||!text_events()||!audio.update())return false;
            frame_text.clear();const auto label=frame_statistics.label();frame_text.add(label.text.c_str(),label.position,label.style);
            if(!session.draw(renderer,&frame_text))return false;graphics.present();multiplayer_advanced=true;
            return true;
        };
        const auto begin=SDL_GetTicksNS();
        const bool ok=playback.Step(restore,tick,[&]{return SDL_GetTicksNS()-begin>=4000000;},fast_forwarding?32:1);
        audio.spectator_silent(playback.Paused()||playback.Seeking());
        return ok;
    }
    bool multiplayer_pump(bool capture_follows=false){
        if(!netplay.Configured())return true;
        if(multiplayer_finish_pending&&netplay.ReadOnly()&&!netplay.Finish()){error="Invalid TH11 read-only terminal frame";return false;}
        if(!netplay.Pump(multiplayer_world,capture_follows)){error=netplay.Error();return false;}
        // Restart waits for the shared fence. Postgame keeps its final input
        // delivery alive while each browser advances its local menus.
        if(multiplayer_finish_pending){
            if(!netplay.Finish(multiplayer_restart_pending,multiplayer_local_results)){if(*netplay.Error()){error=netplay.Error();return false;}return true;}
            if(!netplay.ReadOnly()&&!multiplayer_saved){
                if(!multiplayer_save(0,"PLAYER"))return false;multiplayer_saved=true;
            }
            if(multiplayer_restart_pending){
                const auto seed=(netplay.Setup().seed+1u)&65535u;
                if(!netplay.BeginNextRun(seed)){error=netplay.Error();return false;}
                multiplayer_replay.Clear();
                if(!multiplayer_begin())return false;
                th11_reset_browser_keyboard();
            }
        }
        return true;
    }
    bool multiplayer_save(u32 slot,const char* name){
        if((netplay.ReadOnly()&&!multiplayer_local_results)||!multiplayer_replay.FrameCount())return true;
        if(!name||std::strlen(name)>8||slot>99)return false;
        if(!slot&&multiplayer_saved)return true;
        const bool automatic_slot=!slot;
        std::string path=automatic_slot?multiplayer_auto_path:std::string{};
        if(automatic_slot&&path.empty()){
            path=multiplayer::AutomaticReplayPath([](const char* file){SDL_PathInfo info{};return SDL_GetPathInfo(file,&info);});
            if(path.empty()){error="TH11 multiplayer Replay filenames are exhausted";return false;}
        }else if(!automatic_slot){
            char file[80];std::snprintf(file,sizeof(file),"/save/replay/th11_%.2u.rpy",slot);path=file;
        }
        std::vector<u8> bytes;
        // Native score storage is in ten-point units and fits a u32 even when
        // its displayed ten-billion-point score needs an i64.
        if(!multiplayer_replay.Encode(bytes,name,multiplayer_recorded_score(),multiplayer_recorded_completed())){error="Unable to encode TH11 multiplayer Replay";return false;}
        if(!write_file(path,bytes))return false;
        if(multiplayer_finish_pending&&!multiplayer_restart_pending)Netplay::BrowserPeerTransport::ShareSpectatorReplay(bytes.data(),bytes.size());
        if(automatic_slot)multiplayer_auto_path=path;
        else if(path==multiplayer_auto_path){
            // A named native save now owns that slot. Later periodic/exit
            // backups must choose another empty path, never rename it PLAYER.
            multiplayer_auto_path.clear();multiplayer_saved=false;
        }
        return true;
    }
    bool multiplayer_exact_tick(bool historical=false){
        auto measured=rollback_timer(10);
        multiplayer_advanced=false;
        if(historical){
            if(!netplay.CanReplayConfirmed()||!rollback_frames.empty()||rollback_journal.FrameCount()){
                error="TH11 confirmed history still owns speculative checkpoints";return false;
            }
        }else if((rollback_callback_ms<0&&!multiplayer_pump())||!multiplayer_world)return error.empty();
        if(netplay.Playback()&&multiplayer_finish_pending)return multiplayer_open_replay_menu();
        if(multiplayer_finish_pending||!netplay.CanStart())return true;
        if(!netplay.ReadOnly()&&!multiplayer_replay.Recording()){
            if(!multiplayer_replay.Begin(netplay.Setup(),u64(std::time(nullptr)))){error="Unable to reserve TH11 multiplayer Replay";return false;}
            session.recording_timestamp=multiplayer_replay.Timestamp();
        }
        const auto frame=netplay.NextFrame();
        if(netplay.Playback()){
            if(frame>=multiplayer_replay.FrameCount())return multiplayer_open_replay_menu();
            const auto* input=multiplayer_replay.FrameAt(frame);
            if(!input||!netplay.FeedPlayback(frame,input->data(),netplay.Setup().player_count)){error="Invalid TH11 multiplayer Replay frame";return false;}
        }else if(netplay.Spectator()&&!netplay.FeedSpectator(frame))return true;
        const auto decision=netplay.Prepare(frame);if(!decision.canAdvance)return true;
        std::array<MultiplayerInput,3> input;
        if(decision.predictedMask||!multiplayer::InputLanes::Decode(decision.inputs.data(),netplay.Setup().player_count,input)){error="TH11 multiplayer requires exact inputs";return false;}
        const auto before=session.state.phase;const auto stage=session.state.stage;
        std::vector<u8> spectator_initializer;u32 spectator_stage=stage;

        {auto timed=rollback_timer(5);if(!session.update_multiplayer(input,[&]{if(netplay.SpectatorPublisherOwner())multiplayer_capture_spectator_checkpoint(frame,spectator_stage,spectator_initializer);})){error=session.error;return false;}}
        const bool restart=before==GameSessionPhase::paused&&
            ((input[0].held&0x200000u)!=0||(session.pause_menu&&session.pause_menu->action==PauseAction::Restart));
        const auto after=session.state.phase;
        if(before!=GameSessionPhase::paused&&after==GameSessionPhase::paused&&!stage_music_held())pause_music(true);
        if(before==GameSessionPhase::paused&&after==GameSessionPhase::stage&&!stage_music_held())pause_music(false);
        if(before!=GameSessionPhase::game_over&&after==GameSessionPhase::game_over){
            audio.pause_music(false);if(!request_music(17)){error=audio.error;return false;}
        }
        if(!multiplayer_menu_metadata())return false;
        if(!capture_pause_frame())return false;
        {auto timed=rollback_timer(9);if(!audio_events())return false;}
        {auto timed=rollback_timer(6);if(!text_events())return false;}
        {auto timed=rollback_timer(9);if(!audio.update()){error=audio.error;return false;}}
        if(!historical){auto timed=rollback_timer(12);audio.pump();}
        if(!historical)frame_statistics.sample(double(SDL_GetTicks())*.001,after==GameSessionPhase::stage&&session.battle&&session.battle->stage_active&&!netplay.ReadOnly());
        frame_text.clear();const auto label=frame_statistics.label();frame_text.add(label.text.c_str(),label.position,label.style);
        {auto timed=rollback_timer(7);if(!session.draw(renderer,&frame_text)){error=session.error;return false;}}
        if(!historical)graphics.present();
        if(!netplay.MarkSimulated(frame,decision)){error=netplay.Error();return false;}
        if(netplay.AllowsRollback()&&!(historical?netplay.CommitReconciledPrefix(frame):netplay.SetCommitLimit(frame))){error="TH11 exact boundary commit failed";return false;}
        if(netplay.SpectatorPublisherOwner()&&!netplay.RecordSpectatorFrame(frame,spectator_stage,spectator_initializer,decision.inputs)){
            // Spectator publication is optional and cannot invalidate a player frame.
        }
        if(!netplay.ReadOnly()&&!multiplayer_replay.Append(frame,stage,decision.inputs.data(),netplay.Setup().player_count)){error="TH11 confirmed Replay append failed";return false;}
        rollback_committed_score=u32(std::max(0,session.economy.score_units));rollback_committed_completed=multiplayer_completed();
        if(!multiplayer_menu_save())return false;
        netplay.PublishConfirmedSpectatorHistoryAfterReconcile();
        multiplayer_advanced=true;multiplayer_last_stage=stage;
        // All consumers retire after the last representable confirmed frame.
        // Never simulate/publish an additional frame which cannot be archived.
        if(!netplay.ReadOnly()&&!restart&&(after==GameSessionPhase::ending||after==GameSessionPhase::game_over||after==GameSessionPhase::finished))multiplayer_local_results=true;
        if(restart||multiplayer_local_results||after==GameSessionPhase::finished||frame+1>=multiplayer::ReplayArchive::MaxFrames){multiplayer_finish_pending=true;multiplayer_restart_pending=restart&&!netplay.ReadOnly();return historical||multiplayer_pump();}
        return true;
    }
    bool multiplayer_local_tick(u32 held,bool pause){
        if(!multiplayer_pump())return false;
        if(!netplay.Retired())return true;
        std::array<MultiplayerInput,3> input{};input[0].held=held;input[0].pause=pause;
        if(!session.update_multiplayer(input)){error=session.error;return false;}
        if(!multiplayer_menu_metadata()||!audio_events()||!text_events()||!multiplayer_menu_save())return false;
        if(!audio.update()){error=audio.error;return false;}audio.pump();
        if(!session.draw(renderer,&frame_text)){error=session.error;return false;}
        graphics.present();multiplayer_advanced=true;return true;
    }
#include "MultiplayerRollback.inc"
#endif

    Application():renderer(graphics) {session.resources.effects=this;}
    void reset_frame_window(){frame_statistics.window_start=double(SDL_GetTicks())*.001;frame_statistics.frames=0;}
    bool read_file(const char* path,std::vector<u8>& data,u32 limit){
        SDL_IOStream* io=SDL_IOFromFile(path,"rb");if(!io)return false;const auto size=SDL_GetIOSize(io);
        if(size<0||u64(size)>limit){SDL_CloseIO(io);return false;}data.resize(size_t(size));
        const bool ok=SDL_ReadIO(io,data.data(),data.size())==data.size();SDL_CloseIO(io);return ok;
    }
    bool write_file(const std::string& path,const std::vector<u8>& data){
        const auto temporary=path+".tmp";SDL_IOStream* io=SDL_IOFromFile(temporary.c_str(),"wb");
        if(!io){error="Unable to create save file";return false;}
        const bool written=SDL_WriteIO(io,data.data(),data.size())==data.size();const bool closed=SDL_CloseIO(io);
        if(!written||!closed||std::rename(temporary.c_str(),path.c_str())!=0){error="Unable to finish save file";return false;}return true;
    }
    bool save_scores(){
#ifdef TH11_MULTIPLAYER
        // Multiplayer records have their own all-seat Replay. Ordinary score
        // and unlock files are neither imported nor overwritten by this build.
        return true;
#else
        if(!initialized)return true;
        if(!session.title)session.scores.write_records(session.spell_records,session.clear_records);
        std::vector<u8> state;state.reserve(7*0x68d4+0x448);
        for(const auto& character:session.scores.characters)state.insert(state.end(),character.begin(),character.end());
        state.insert(state.end(),session.scores.settings.begin(),session.scores.settings.end());
        if(state!=saved_score_state){std::vector<u8> data;if(!session.scores.save(data)){error="Unable to encode scores";return false;}
            if(!write_file("/save/scoreth11.dat",data))return false;saved_score_state=std::move(state);}
        const auto& cfg=session.config.bytes;std::vector<u8> config(cfg.begin(),cfg.end());
        if(config!=saved_config_state){if(!write_file("/save/th11.cfg",config))return false;saved_config_state=std::move(config);}
        return true;
#endif
    }
    bool save_replay(u32 slot,const char* name){
#ifdef TH11_MULTIPLAYER
        return multiplayer_save(slot,name);
#else
        if(slot>99||!name||std::strlen(name)>8)return false;std::vector<u8> data;
        if(!session.save_replay(name,data,true,frame_statistics.slowdown())){error="No active recording to save";return false;}
        const bool touch=session.recording.uses_touch();char path[80];std::snprintf(path,sizeof(path),"/save/replay/th11_%.2u.%s",slot,touch?"rpyx":"rpy");
        if(!write_file(path,data))return false;
        std::snprintf(path,sizeof(path),"/save/replay/th11_%.2u.%s",slot,touch?"rpy":"rpyx");std::remove(path);return true;
#endif
    }
    bool filter_practice_music(PracticeBgmEvent event,int id=0){
#ifdef TH_ENABLE_THPRAC
        const auto& p=session.practice;
        return p.enabled&&practice_bgm.filter(event,id,p.everlasting_bgm&&!session.state.replay&&p.active&&p.run.section,session.state.practice);
#else
        return false;
#endif
    }
    bool practice_music_skip_intro()const{
#ifdef TH_ENABLE_THPRAC
        return session.practice.enabled&&session.practice.active&&session.practice.run.section;
#else
        return false;
#endif
    }
    bool stage_music_held()const{return session.state.stage==6&&session.battle&&session.battle->frame<300&&!session.state.demo&&!practice_music_skip_intro();}
    bool request_music(i32 cue){
#ifdef TH11_MULTIPLAYER
        // Launcher music:none deliberately supplies no OGG resources. Keep
        // sound effects active without opening an absent background track.
        if(!audio.music_enabled)return true;
#endif
        return audio.music(cue);
    }
    bool play_stage_music(u32 stage){
        if(session.state.demo)return true;
        bool boss=false;
#ifdef TH_ENABLE_THPRAC
        const auto& p=session.practice.run;
        boss=session.practice.enabled&&session.practice.active&&p.section>0&&p.section<10000&&!p.dlg&&practice_sections[p.section].bgm;
#endif
        if(!filter_practice_music(PracticeBgmEvent::Other)&&!request_music(stage_music(stage,boss))){error=audio.error;return false;}
        audio.pause_music(stage==6&&!practice_music_skip_intro());return true;
    }
    void pause_music(bool paused){if(!filter_practice_music(paused?PracticeBgmEvent::Pause:PracticeBgmEvent::Resume))audio.pause_music(paused);}
    bool pause(){if(!session.pause())return false;if(!stage_music_held())pause_music(true);return true;}
    bool resume(){if(!session.resume())return false;if(!stage_music_held())pause_music(false);return true;}
    bool capture_pause_frame(){
        if(!session.pause_menu||!session.pause_menu->capture_requested)return true;
        auto& menu=*session.pause_menu;
        auto* vm=menu.background_vm();if(!vm||!vm->sprite||!vm->resource){error="Pause screenshot sprite missing";return false;}
        const auto& sprite=*vm->sprite;const u32 target=graphics.texture(*vm->resource,sprite.texture);renderer.flush();
        const i32 from[]={32,16,416,464},to[]={i32(sprite.x),i32(sprite.y),i32(sprite.x+sprite.width),i32(sprite.y+sprite.height)};
        if(!graphics.backend.resample(GraphicsDevice::screen,from,target,to,nullptr,0,0)){error="Pause GPU capture failed";return false;}
        graphics.changed(target);renderer.invalidate();menu.capture_requested=false;return true;
    }
    bool audio_events(){
        if(session.ending){auto& ending=*session.ending;for(const auto sound:ending.sounds)audio.effects.enqueue(sound);
            if(ending.music_request>=0){audio.pause_music(false);if(!request_music(ending.music_request)){error=audio.error;return false;}}
            if(ending.music_fade>=0)audio.fade_music(ending.music_fade);
        }
        if(session.pause_menu){for(const auto sound:session.pause_menu->sounds)audio.effects.enqueue(sound);session.pause_menu->sounds.clear();}
        if(session.title){
            for(const auto sound:session.title->sounds)audio.effects.enqueue(sound);
            if(session.title->music_pause)audio.pause_music(true);
            if(session.title->music_request>=0){audio.pause_music(false);if(!request_music(session.title->music_request)){error=audio.error;return false;}}
            if(session.title->volume_changed){audio.music_volume=session.config.music_volume();audio.effects.master_volume=session.config.sound_volume();audio.refresh_volume();}
            session.title->sounds.clear();
        }
        if(!session.battle)return true;
        for(const auto& event:session.battle->events){
            switch(event.kind){
            case BattleEventKind::Sound:
                if(event.positional)audio.effects.positioned(event.value,event.position.x);else audio.effects.enqueue(event.value);
                break;
            case BattleEventKind::StopSound:audio.effects.stop(event.value);break;
            case BattleEventKind::StageMusic:if(!play_stage_music(event.value))return false;break;
            case BattleEventKind::BossMusic:
                if(!filter_practice_music(PracticeBgmEvent::Play,stage_music(event.value,true))&&!request_music(stage_music(event.value,true))){error=audio.error;return false;}break;
            case BattleEventKind::MusicResume:pause_music(false);break;
            case BattleEventKind::MusicFade:audio.fade_music(i32(event.position.x*60.f));break;
            default:break;
            }
        }
        session.battle->events.clear();return true;
    }
    bool text_events(){
        if(session.ending){renderer.flush();for(const auto& request:session.ending->text_requests){auto* vm=session.animations.find(request.animation);if(vm&&!fonts.text(*vm,request)){error=fonts.error;return false;}}session.ending->text_requests.clear();}
        if(session.title){renderer.flush();for(const auto& request:session.title->text_requests){auto* vm=session.animations.find(request.animation);if(vm&&!fonts.text(*vm,request)){error=fonts.error;return false;}}session.title->text_requests.clear();}
        if(!session.battle)return true;
        renderer.flush();
        for(const auto& request:session.battle->dialogue_text_requests){auto* vm=session.animations.find(request.animation);if(vm&&!fonts.text(*vm,request)){error=fonts.error;return false;}}
        session.battle->dialogue_text_requests.clear();return true;
    }
    bool prepare(StageResources& stage)override{
        renderer.flush();
        for(auto* file:{&stage.background,&stage.logo,&stage.enemies})if(!graphics.preload(*file)){error=graphics.error;return false;}
        return true;
    }
    void release(StageResources& stage)override{
        renderer.flush();renderer.invalidate();
        for(auto* file:{&stage.background,&stage.logo,&stage.enemies})graphics.unload(*file);
    }
    bool prepare_animation(AnmResource& file)override{renderer.flush();return graphics.preload(file);}
    void release_animation(AnmResource& file)override{renderer.flush();renderer.invalidate();graphics.unload(file);}
    bool read_replay(const std::string& path,std::vector<u8>& bytes)override{return read_file(path.c_str(),bytes,64*1024*1024);}

    bool read_archive() {
        SDL_IOStream* io=SDL_IOFromFile("/th11.dat","rb");
        if(!io){error="Unable to open /th11.dat";return false;}
        const Sint64 size=SDL_GetIOSize(io);
        if(size<=0||size>256*1024*1024){SDL_CloseIO(io);error="Invalid th11.dat size";return false;}
        archive.resize(size_t(size));
        const size_t got=SDL_ReadIO(io,archive.data(),archive.size());
        SDL_CloseIO(io);
        if(got!=archive.size()){error="Unable to read /th11.dat";return false;}
        return resources.open_archive(archive.data(),u32(archive.size()));
    }

    bool prepare_platform(){
        if(platform_prepared)return true;
        if(!graphics.initialize()){error=graphics.error;return false;}
        if(!read_archive()){if(error.empty())error=resources.error();return false;}
        platform_prepared=true;return true;
    }
    void release_loading(){renderer.invalidate();loading_animations.clear();loading_credit=nullptr;graphics.unload(loading_signature);graphics.release_startup_branding();}
    bool prepare_loading(){
        release_loading();
        if(!prepare_platform()||!resources.open_anm("sig.anm",loading_signature)||!graphics.preload(loading_signature)){
            if(error.empty())error=graphics.error.empty()?resources.error():graphics.error;return false;
        }
        loading_credit=loading_animations.create(loading_signature,0,1,0);
        if(!loading_credit){error="Startup signature animation failed";return false;}
        return draw_loading(1);
    }
    bool draw_loading(u32 frames){
        if(!loading_credit)return false;
        for(u32 i=0;i<std::min(frames,120u);++i)if(!loading_animations.update(false)||!loading_animations.update(true)){error="Startup signature update failed";return false;}
        session.compositor.reset_cameras(false);renderer.viewport={0,0,640,480,0,1};
        renderer.invalidate();if(!renderer.select_target(nullptr,0)||!renderer.clear_target(0xff000000))return false;
        renderer.set_camera(session.compositor.full_camera,true);
        for(u32 layer=0;layer<31;++layer){
            if(renderer.draw_layer(loading_animations.layer_first(layer))==-2){error="Startup signature draw failed";return false;}
        }
        // This isolated manager contains only sig.anm and its background children.
        // Finish those children before drawing the credit, including the build
        // timestamp outside the central signature panel. Keep the same fade tint.
        renderer.flush();graphics.draw_startup_branding(loading_credit->color);
        renderer.flush();graphics.present();return true;
    }
    bool initialize() {
        if(initialized){if(!audio.initialize(resources)){error=audio.error;return false;}return true;}
        if(!prepare_platform())return false;
        if(!audio.initialize(resources)){error=audio.error;return false;}
        if(!fonts.initialize()){error=fonts.error;return false;}
#ifdef TH_ENABLE_THPRAC
        session.practice.enabled=EM_ASM_INT({return Module.eaglerOptions?.thpracEnabled?1:0;})!=0;
        if(!browser::ThpracUi::initialize()){error="Unable to initialize TH11 thprac font/UI";return false;}
#endif
        mkdir("/save",0777);mkdir("/save/replay",0777);
#ifndef TH11_MULTIPLAYER
        if(SDL_GetPathInfo("/save/scoreth11.dat",nullptr)){std::vector<u8> saved;if(!read_file("/save/scoreth11.dat",saved,4*1024*1024)||!session.load_scores(saved.data(),u32(saved.size()))){error="Invalid scoreth11.dat: "+session.scores.error;return false;}}
        if(SDL_GetPathInfo("/save/th11.cfg",nullptr)){std::vector<u8> saved;if(!read_file("/save/th11.cfg",saved,60)||!session.config.open(saved.data(),u32(saved.size()))){error="Invalid th11.cfg";return false;}saved_config_state=saved;}
#endif
        audio.music_volume=session.config.music_volume();audio.effects.master_volume=session.config.sound_volume();audio.refresh_volume();
        session.recording_timestamp=u64(std::time(nullptr));
        if(!session.open_title(resources,true)){error=session.error;return false;}
        auto& core=session.resources.core;
        for(AnmResource* file:{&core.text,&core.ascii,&core.bullet,&core.enemy,&core.front,&core.title,&core.title_variant,&core.players[0],&core.players[1]})
            if(!graphics.preload(*file)){error=graphics.error;return false;}
        renderer.viewport={0,0,640,480,0,1};
        // GameBattle's STD owner creates backdrop VMs and publishes its camera.
        frame_statistics.window_start=double(SDL_GetTicks())*.001;
        initialized=true;return audio_events()&&text_events();
    }

    bool restart(u32 stage=1,i32 character=0,i32 subtype=0,i32 difficulty=1) {
        if(!initialize())return false;
        if(!save_scores())return false;session.recording_timestamp=u64(std::time(nullptr));
        renderer.invalidate();
        auto& core=session.resources.core;
        for(AnmResource* file:{&core.text,&core.ascii,&core.bullet,&core.enemy,&core.front,&core.title,&core.title_variant,&core.players[0],&core.players[1]})graphics.unload(*file);
        if(session.resources.stage)for(AnmResource* file:{&session.resources.stage->background,&session.resources.stage->logo,&session.resources.stage->enemies})graphics.unload(*file);
        if(!session.begin(resources,stage,character,subtype,difficulty,false)){error=session.error;return false;}
        frame_statistics.reset_run();
        for(AnmResource* file:{&core.text,&core.ascii,&core.bullet,&core.enemy,&core.front,&core.title,&core.title_variant,&core.players[0],&core.players[1],&session.resources.stage->background,&session.resources.stage->logo,&session.resources.stage->enemies})
            if(!graphics.preload(*file)){error=graphics.error;return false;}
        if(!play_stage_music(session.state.stage))return false;
        return audio_events()&&text_events();
    }

    bool tick(u32 held,bool pause_key=false) {
        if(!initialize())return false;
        const auto before=session.state.phase;
        const GameSessionInput input{held,0,0,pause_key,bool(held&0x100),bool(held&0x2),frame_statistics.fps};
        if(!session.update(input)){error=session.error;return false;}
#ifdef TH11_MULTIPLAYER
        if(multiplayer_viewer&&session.title&&session.title->replay_start_requested){
            const auto entry=session.title->replay_files[session.title->replay_file];
            multiplayer_replay_index=u32(session.title->replay_file);const u32 stage=u32(session.title->replay_stage+1);
            std::vector<u8> bytes;
            if(!entry||!entry->is_multiplayer||!read_file(entry->path.c_str(),bytes,16*1024*1024)){error="Selected TH11 multiplayer Replay could not be loaded";return false;}
            return multiplayer_load_replay(bytes.data(),u32(bytes.size()),stage);
        }
#endif
        presentation_audit::simulation_tick();
        presentation_audit::sample_state(session);
        const auto after=session.state.phase;
        if(before!=GameSessionPhase::title&&after==GameSessionPhase::title)filter_practice_music(PracticeBgmEvent::Stop);
        if(before!=GameSessionPhase::stage&&after==GameSessionPhase::stage&&session.state.frame==0)frame_statistics.reset_run();
        if(session.pause_menu)session.pause_menu->slowdown=frame_statistics.slowdown();
        if(session.title&&session.title->screen==TitleScreen::Results)session.title->result_slowdown=frame_statistics.slowdown();
        if(before!=GameSessionPhase::stage&&after==GameSessionPhase::stage){
            if(before==GameSessionPhase::paused&&session.state.frame!=0){if(!stage_music_held())pause_music(false);}
            else{audio.effects.reset();if(!play_stage_music(session.state.stage))return false;}
        }
        if(before!=GameSessionPhase::paused&&after==GameSessionPhase::paused&&!stage_music_held())pause_music(true);
        if(before!=GameSessionPhase::game_over&&after==GameSessionPhase::game_over){session.pause_menu->timestamp=u64(std::time(nullptr));audio.pause_music(false);if(!audio.music(17)){error=audio.error;return false;}}
        if(before==GameSessionPhase::ending&&session.title)session.title->result_timestamp=u64(std::time(nullptr));
        if(session.title&&session.title->replay_scan_requested)scan_replays();
        if(session.title&&session.title->screen==TitleScreen::ReplaySave){auto& menu=*session.title;
            if(menu.substate==3&&!menu.pending_replay){const u64 stamp=u64(std::time(nullptr));std::memcpy(session.recording.header.data()+12,&stamp,8);auto entry=std::make_shared<ReplayMenuEntry>();
                if(!session.save_replay("        ",entry->file,true,frame_statistics.slowdown())||!entry->replay.open(entry->file.data(),u32(entry->file.size()))){error="Completed replay metadata unavailable";return false;}menu.pending_replay=std::move(entry);
            }
            if(menu.replay_save_requested){if(!save_replay(menu.replay_file+1,menu.entered_name.data()))return false;menu.replay_save_requested=false;menu.pending_replay.reset();scan_replays();if(!save_scores())return false;}
        }
        if(session.pause_menu){auto& menu=*session.pause_menu;
            if(!capture_pause_frame())return false;
            if(menu.scan_requested)scan_replays();
            if(menu.recording_metadata_requested){menu.timestamp=u64(std::time(nullptr));std::memcpy(session.recording.header.data()+12,&menu.timestamp,8);}
            if(menu.save_requested){if(!save_replay(menu.cursor.selected+1,menu.entered_name.data()))return false;menu.save_requested=false;scan_replays();if(!save_scores())return false;}
        }
        if(session.battle&&after==GameSessionPhase::stage&&!session.battle->spell_timing.update(session.battle->spell_flags,session.battle->spells.frame_count,double(SDL_GetTicks())*.001,session.state.replay)){error="Invalid spell timing state";return false;}
        if(!audio_events()||!text_events())return false;
        if(!audio.update()){error=audio.error;return false;}audio.pump();
        frame_statistics.sample(double(SDL_GetTicks())*.001,after==GameSessionPhase::stage&&session.battle&&session.battle->stage_active&&!session.state.replay);
        return draw_session();
    }
    void presentation_frame(double seconds){
        if(!presentation_origin)presentation_origin=seconds;
        ++presentation_frames;
        const double elapsed=seconds-presentation_origin;
        if(elapsed<.5)return;
        presentation_fps=float(double(presentation_frames)/elapsed);
        presentation_origin=seconds;presentation_frames=0;
    }
    void reset_presentation_frames(){presentation_origin=0;presentation_frames=0;presentation_fps=0;}
    bool draw_session(){
        if(presentation_audit::enabled()){
            if(high_refresh::render_only)presentation_audit::begin_sample(high_refresh::alpha);
            else presentation_audit::begin_reference();
        }
        // TH10 behavior: the on-screen counter follows the presented frames, so
        // a 120/144 Hz display reads 120/144 while logic stays at 60.
        const float shown=high_refresh::render_only&&presentation_fps>0?presentation_fps:frame_statistics.fps;
        frame_text.clear();const auto label=frame_statistics.label(shown);frame_text.add(label.text.c_str(),label.position,label.style);
        if(high_refresh::render_only)presentation_frame(double(SDL_GetTicks())*.001);
        if(!session.draw(renderer,&frame_text)){error=session.error;presentation_audit::end_frame();return false;}
#ifdef TH_ENABLE_THPRAC
        renderer.flush();browser::ThpracUi::render(session,graphics.backend);
#endif
        presentation_audit::end_frame();
        graphics.present();
        return true;
    }
    // Display-only Draw transaction. Simulation, input, RNG, audio and
    // Replay/recording cursors are untouched; the only state written here is
    // the temporary presentation window owned by GameSession/Stage.
    bool presentation_draw(float alpha,bool interpolate,bool world_interpolate){
        high_refresh::begin(alpha,interpolate,true,world_interpolate);
        const bool ok=draw_session();
        high_refresh::end();
        return ok;
    }
    bool return_to_title(){
#ifdef TH11_MULTIPLAYER
        if(multiplayer_viewer)return multiplayer_open_replay_menu();
        if(multiplayer_world){error="A live multiplayer run cannot return through the single-player title path";return false;}
#endif
        if(!save_scores())return false;
        if(!session.return_to_title()){error=session.error;return false;}
        filter_practice_music(PracticeBgmEvent::Stop);
        audio.effects.reset();return true;
    }
    void scan_replays(){
        if(!session.title&&!session.pause_menu)return;
#ifdef TH11_MULTIPLAYER
        if(session.title)session.title->clear_replays();else session.pause_menu->replay_files.fill(nullptr);
        auto load=[&](u32 slot,const std::string& path){
            std::vector<u8> bytes;if(!read_file(path.c_str(),bytes,16*1024*1024))return;
            multiplayer::ReplayArchive replay;if(!replay.Load(bytes.data(),bytes.size())||!multiplayer_build_matches(replay))return;
            auto entry=std::make_shared<ReplayMenuEntry>();entry->path=path;multiplayer_preview(*entry,replay);
            if(session.title)session.title->replay_files[slot]=std::move(entry);else if(slot<25)session.pause_menu->replay_files[slot]=std::move(entry);
        };
        const bool catalog=session.title&&session.title->screen==TitleScreen::Replays;
        if(catalog){
            std::vector<std::string> names;auto* dir=opendir("/save/replay");
            if(dir){while(auto* entry=readdir(dir)){
                const std::string name=entry->d_name;
                if(name.size()!=15||name.compare(0,7,"th11_ud")||name.compare(11,4,".rpy"))continue;
                if(std::all_of(name.begin()+7,name.begin()+11,[](char c){return (c>='0'&&c<='9')||(c>='a'&&c<='z');}))names.push_back(name);
            }closedir(dir);}
            std::sort(names.begin(),names.end());
            session.title->replay_files.resize(100+((names.size()+24)/25)*25);
            for(u32 i=0;i<names.size();++i)load(100+i,"/save/replay/"+names[i]);
        }
        for(u32 slot=0;slot<(catalog?99u:25u);++slot){char path[80];std::snprintf(path,sizeof(path),"/save/replay/th11_%.2u.rpy",slot+1);load(slot,path);}
        if(catalog)session.title->replay_scan_complete();
#else
        auto load=[&](u32 slot,const std::string& path){std::vector<u8> bytes;if(!read_file(path.c_str(),bytes,64*1024*1024))return;
            auto entry=std::make_shared<TitleMenu::ReplayEntry>();if(!entry->replay.open(bytes.data(),u32(bytes.size())))return;
            entry->path=path;entry->replay.retain_metadata();if(session.title)session.title->replay_files[slot]=std::move(entry);else if(slot<25)session.pause_menu->replay_files[slot]=std::move(entry);};
        for(u32 slot=0;slot<25;++slot){char path[80];std::snprintf(path,sizeof(path),"/save/replay/th11_%.2u.rpy",slot+1);load(slot,path);load(slot,std::string(path)+"x");}
        if(session.title&&session.title->screen==TitleScreen::Replays){std::vector<std::string> names;auto* dir=opendir("/save/replay");if(dir){while(auto* e=readdir(dir)){const std::string n=e->d_name;if(n.size()>=15&&n.rfind("th11_ud",0)==0&&(n.substr(11)==".rpy"||n.substr(11)==".rpyx"))names.push_back(n);}closedir(dir);}
            std::sort(names.begin(),names.end());for(u32 i=0;i<names.size()&&i<50;++i)load(25+i,"/save/replay/"+names[i]);}
        if(session.title&&session.title->screen==TitleScreen::Replays)session.title->replay_scan_complete();
#endif
    }
};
Application app;
struct Key {const char* code;const char* sdl;u32 scan,vk;bool hosted=false;SDL_Scancode native=SDL_SCANCODE_UNKNOWN;};
#include "../../../portable/input/KeyboardMap.inc"
touhou::input::TouchController gestures;
eagler::thprac::PracticePointer practice_pointers;
touhou::sdl::FrameCadence cadence;
#ifdef TH_ENABLE_THPRAC
th11::PracticeCadence practice_cadence;
#endif
#ifdef TH11_MULTIPLAYER
multiplayer::FrameSchedule rollback_cadence,exact_cadence;
multiplayer::PresentationSchedule multiplayer_presentation;
unsigned recovery_tasks_remaining=0;
constexpr bool recovery_tasks_enabled=false; // r37/r38 did not improve cadence
#endif
touhou::sdl::PresentationCadence presentation;
touhou::sdl::PresentationGate presentation_gate;
unsigned presentation_draws=0;
bool running=false,suspended=false;u32 loop_epoch=0;double previous_frame=-1;
void reset_cadences(){
    cadence.reset();
#ifdef TH11_MULTIPLAYER
    rollback_cadence.reset();exact_cadence.reset();
    multiplayer_presentation.reset();
#endif
}
std::array<u8,256> previous_scans{};
SDL_Joystick* controller=nullptr;
void add_controller(SDL_JoystickID id){if(!controller)controller=SDL_OpenJoystick(id);}
void initialize_controller(){
    if(controller){SDL_CloseJoystick(controller);controller=nullptr;}
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);int count=0;auto* ids=SDL_GetJoysticks(&count);
    for(int i=0;i<count;++i)add_controller(ids[i]);SDL_free(ids);
}
u32 sample_controller(){
    if(app.session.title)app.session.title->controller_buttons=0;
    if(!controller)return 0;std::array<u8,128> buttons{};
    for(int i=0;i<std::min(128,SDL_GetNumJoystickButtons(controller));++i)if(SDL_GetJoystickButton(controller,i)){
        buttons[i]=128;if(i<32&&app.session.title)app.session.title->controller_buttons|=1u<<i;
    }
    const auto axis=[&](int index){const int value=index<SDL_GetNumJoystickAxes(controller)?SDL_GetJoystickAxis(controller,index):0;return i32(std::floor((value<0?value/32768.:value/32767.)*1000+.5));};
    i32 x=axis(0),y=axis(1);const auto hat=SDL_GetNumJoystickHats(controller)?SDL_GetJoystickHat(controller,0):0;
    if(hat&SDL_HAT_LEFT)x=-1000;if(hat&SDL_HAT_RIGHT)x=1000;if(hat&SDL_HAT_UP)y=-1000;if(hat&SDL_HAT_DOWN)y=1000;
    return controller_keys(0,buttons.data(),u32(buttons.size()),x,y,app.session.config.bytes.data());
}
touhou::input::TouchState touch_state(){
    touhou::input::TouchState s;auto& session=app.session;
#ifdef TH11_MULTIPLAYER
    if(app.multiplayer_world&&app.multiplayer_touch_valid)return app.multiplayer_touch_frontier;
    if(app.netplay.ReadOnly()){s.context=3;return s;}
    if(session.state.phase==GameSessionPhase::ending){s.context=2;return s;}
    if(session.state.phase==GameSessionPhase::game_over){s.context=app.netplay.Setup().local_player==0?0:3;return s;}
    if(session.state.phase==GameSessionPhase::stage&&app.multiplayer_world){
        if(session.battle&&session.battle->dialogue&&session.battle->dialogue->active){s.context=2;return s;}
        const auto v=session.multiplayer_seat(app.netplay.Setup().local_player);
        s.context=1;s.instance=int(session.state.stage);s.ready=v.active&&!v.ghost&&v.life_state==1;
        s.x=v.x;s.y=v.y;s.fast=v.fast_speed;s.slow=v.slow_speed;
        s.min_x=-184;s.max_x=184;s.min_y=32;s.max_y=432;return s;
    }
#endif
    if(session.state.phase==GameSessionPhase::ending){s.context=2;return s;}
    if(session.state.phase!=GameSessionPhase::stage||!session.battle||!session.battle->player)return s;
    if(session.state.replay){s.context=3;return s;}
    const auto& player=*session.battle->player;const auto& p=player.motion.state;
    s.context=session.battle->dialogue&&session.battle->dialogue->active?2:1;s.instance=int(session.state.stage);s.ready=player.state.life_state==1;
    s.x=p.position.x;s.y=p.position.y;s.fast=float(p.normal_speed)/128;s.slow=float(p.focus_speed)/128;
    s.min_x=-184;s.max_x=184;s.min_y=32;s.max_y=432;return s;
}
// Browser/device callbacks never edit a retained multiplayer world. A cancel
// belongs to the next once-only input capture; historical ticks use their
// recorded InputLanes even when recovery yields to browser events.
void neutralize_live_touch(){
#ifndef TH11_MULTIPLAYER
    if(app.session.battle)app.session.battle->player_input.movement.touch_mode=0;
#endif
}
void clear_inputs(){
    practice_pointers.reset();
#ifdef TH_ENABLE_THPRAC
    browser::ThpracUi::cancel_pointer();
    app.session.practice.input.reset();
#endif
    th11_reset_browser_keyboard();SDL_ResetKeyboard();for(auto& k:keyboard_map)k.hosted=false;previous_scans.fill(0);gestures.reset();neutralize_live_touch();
}
void touch_event(unsigned type,int id,float x,float y){
    if(type>2||!std::isfinite(x)||!std::isfinite(y))return;
#ifdef TH_ENABLE_THPRAC
    // Same normalized direct-touch -> native ImGui bridge as TH08/TH10.
    // Both the launcher carrier and canvas-origin SDL fingers enter here.
    if(type==0&&practice_pointers.route(1,id,false))browser::ThpracUi::cancel_pointer();
    using Route=eagler::thprac::PracticePointer::TouchRoute;
    const auto decision=practice_pointers.route_touch(type,id,type==0&&browser::ThpracUi::captures_pointer(x*640.f,y*480.f),type==0&&browser::ThpracUi::passive_pointer(x*640.f,y*480.f),x*640.f,y*480.f);
    if(decision.route==Route::Pending)return;
    if(decision.route==Route::Tap){browser::ThpracUi::mouse(1,decision.x,decision.y);browser::ThpracUi::mouse(2,x*640.f,y*480.f);return;}
    if(decision.route==Route::Ui){browser::ThpracUi::mouse(type==0?1:type==1?0:2,x*640.f,y*480.f);return;}
    if(decision.route==Route::BeginGame)gestures.pointer(0,id,decision.x/640.f,decision.y/480.f,SDL_GetTicks(),touch_state(),false);
#endif
    gestures.pointer(type,id,x,y,SDL_GetTicks(),touch_state(),false);
}
void cancel_touch(){
    practice_pointers.reset();
#ifdef TH_ENABLE_THPRAC
    browser::ThpracUi::cancel_pointer();
#endif
    gestures.cancel_transient();neutralize_live_touch();
}
bool sample_and_tick(bool advance=true){
    SDL_Event event;while(SDL_PollEvent(&event)){
#ifdef TH_ENABLE_THPRAC
        browser::ThpracUi::process_event(event);
#endif
        // Canvas gestures belong to SDL; the host bridge handles only gestures
        // originating outside it. Match TH08/TH10's native finger-event path.
        if(event.type==SDL_EVENT_FINGER_CANCELED){cancel_touch();continue;}
        if(event.type==SDL_EVENT_FINGER_DOWN||event.type==SDL_EVENT_FINGER_MOTION||event.type==SDL_EVENT_FINGER_UP)
            touch_event(event.type==SDL_EVENT_FINGER_DOWN?0:event.type==SDL_EVENT_FINGER_MOTION?1:2,
                        int(event.tfinger.fingerID),event.tfinger.x,event.tfinger.y);
        if(event.type==SDL_EVENT_WINDOW_FOCUS_LOST)clear_inputs();
        else if(event.type==SDL_EVENT_JOYSTICK_ADDED)add_controller(event.jdevice.which);
        else if(event.type==SDL_EVENT_JOYSTICK_REMOVED&&controller&&SDL_GetJoystickID(controller)==event.jdevice.which){SDL_CloseJoystick(controller);controller=nullptr;int count=0;auto* ids=SDL_GetJoysticks(&count);for(int i=0;i<count;++i)add_controller(ids[i]);SDL_free(ids);}
    }
#ifdef TH11_MULTIPLAYER
    // SDL events keep pumping during a remote wait. Gesture pulses belong to
    // the next unscheduled frame and are never consumed by a retry.
    if(app.room_preview){
        app.session.suppress_title_demo();
        if(app.session.title)app.session.title->room_preview=true;
    }
    const auto phase=app.session.state.phase;
    app.netplay.SetCapturePhase(app.multiplayer_world&&!app.multiplayer_finish_pending&&
        (!app.netplay.AllowsRollback()||phase==GameSessionPhase::paused||
         phase==GameSessionPhase::ending||phase==GameSessionPhase::game_over),SDL_GetTicksNS()/1000);
    if(!app.room_preview&&!app.multiplayer_viewer&&!app.multiplayer_local_results&&(app.multiplayer_finish_pending||!app.netplay.NeedsCapture()||app.netplay.CaptureFrame()>=multiplayer::ReplayArchive::MaxFrames))return advance?app.multiplayer_tick():true;
#endif
    bool keys[256]{};std::array<u8,256> scans{};const auto* physical=th11_browser_keyboard()?nullptr:SDL_GetKeyboardState(nullptr);
    for(const auto& k:keyboard_map)if(k.hosted||(physical&&k.native!=SDL_SCANCODE_UNKNOWN&&physical[k.native])){if(k.scan<256)scans[k.scan]=128;if(k.vk<256)keys[k.vk]=true;if(k.vk>=160&&k.vk<=165)keys[16+(k.vk-160)/2]=true;if(k.scan==28||k.scan==156)keys[13]=true;}
    const int mappedPad=eagler::input::BrowserGamepadState();eagler::input::ApplyGamepadVirtualKeys(keys,mappedPad);eagler::input::ApplyGamepadScanKeys(scans,mappedPad);
    // Live/recorded shared UI consumes the committed seat words below. A
    // future physical capture must not overwrite its current title key edges.
    bool local_title_input=true;
#ifdef TH11_MULTIPLAYER
    local_title_input=app.room_preview||app.multiplayer_local_results||(app.multiplayer_viewer&&!app.multiplayer_world);
#endif
    if(app.session.title&&local_title_input){auto& title=*app.session.title;for(u32 i=0;i<256;++i)title.key_edges[i]=scans[i]&~previous_scans[i];title.number_keys=0;for(u32 i=0;i<9;++i)if(keys[49+i])title.number_keys|=1u<<i;}
    previous_scans=scans;
    const auto sample=gestures.sample(touch_state(),SDL_GetTicks(),keys[16],keys[37]||keys[38]||keys[39]||keys[40]);
    for(u32 n=0;n<256;++n)keys[n]=keys[n]||sample.keys[n];
#ifdef TH11_MULTIPLAYER
    // Story taps keep their native confirmation path. Retain the action
    // buttons too, so touch players can give resources while dialogue is open.
    if(app.multiplayer_world&&app.session.battle&&app.session.battle->dialogue&&app.session.battle->dialogue->active&&gestures.enabled){keys[90]=keys[90]||gestures.fire;keys[16]=keys[16]||gestures.focus;}
#endif
#ifdef TH_ENABLE_THPRAC
    browser::ThpracUi::update_input(app.session,keys);
    const bool captured=browser::ThpracUi::captures_game_input();
    browser::ThpracUi::apply_input(app.session,keys);
#else
    const bool captured=false;
#endif
#ifndef TH11_MULTIPLAYER
    if(auto* b=app.session.battle.get()){b->player_input.movement.touch_mode=sample.motion;b->player_input.movement.touch_x=sample.x;b->player_input.movement.touch_y=sample.y;}
#endif
    const u32 raw=(mappedPad<0?sample_controller():0)|keyboard_keys(keys);
    const u32 held=(raw&~0x80100u)|((raw&0x80000)?256:0);
#ifdef TH11_MULTIPLAYER
    if(app.room_preview){app.multiplayer_advanced=advance;return !advance||app.tick(captured?0:held,!captured&&(raw&256)!=0);}
    if(app.multiplayer_local_results)return !advance||app.multiplayer_local_tick(captured?0:held,!captured&&(raw&256)!=0);
    if(app.multiplayer_viewer){
        if(app.netplay.Spectator())return app.multiplayer_tick();
        if(!app.multiplayer_world){app.multiplayer_advanced=true;return app.tick(captured?0:held,!captured&&(raw&256)!=0);}
        // TH08MP/TH10MP give physical Escape a local viewer exit. Every other
        // gameplay/pause/result action comes only from the recorded seats.
        app.multiplayer_viewer_input.update(captured?0:raw);
        if(app.multiplayer_viewer_input.pressed&256u)return app.multiplayer_open_replay_menu();
        return app.multiplayer_tick();
    }
    int motion=sample.motion;float x=sample.x,y=sample.y;
    // The ordinary free stick derives a target from the current player
    // position. A delayed input carries the stable raw vector instead.
    if(gestures.mode==3&&touch_state().context==1&&!(keys[37]||keys[38]||keys[39]||keys[40])){
        motion=(gestures.stick_x||gestures.stick_y)?3:0;x=gestures.stick_x;y=gestures.stick_y;
    }
    Netplay::FrameInput input;
    if(!th11::multiplayer::InputLanes::Capture(captured?0:held,app.multiplayer_pause_requested||(!captured&&(raw&256)!=0),captured?0:motion,x,y,input)||
       !app.netplay.Capture(app.netplay.CaptureFrame(),input)){app.error="Unable to capture TH11 multiplayer input";return false;}
    app.multiplayer_pause_requested=false;
    return advance?app.multiplayer_tick():true;
#else
    return app.tick(captured?0:held,!captured&&(raw&256)!=0);
#endif
}
}
}

extern "C" {
#ifdef TH11_MULTIPLAYER
EMSCRIPTEN_KEEPALIVE int th11_mp_configure(const unsigned* words,unsigned count){
    using namespace th11::sdl;if(running)return 0;th11::multiplayer::SessionSetup setup;
    if(!th11::multiplayer::DecodeSessionSetup(setup,words,count)){app.error="Invalid TH11 multiplayer session contract";return 0;}
    app.rollback_release();rollback_cadence.reset();
    app.room_preview=false;
    app.multiplayer_local_results=false;
    app.error.clear();app.multiplayer_replay.Clear();app.multiplayer_world=false;app.multiplayer_viewer=false;
    app.multiplayer_build=setup.build;return app.netplay.Reset(setup)?1:0;
}
EMSCRIPTEN_KEEPALIVE int th11_mp_connect(const char* url){return th11::sdl::app.netplay.Connect(url);}
EMSCRIPTEN_KEEPALIVE int th11_mp_spectator_connect(const char* url,const char* id){
    auto& app=th11::sdl::app;if(!app.netplay.ConnectSpectator(url,id))return 0;
    app.multiplayer_viewer=true;app.multiplayer_world=false;app.multiplayer_finish_pending=false;app.error.clear();return 1;
}
EMSCRIPTEN_KEEPALIVE const char* th11_mp_spectator_state(){return th11::sdl::app.netplay.SpectatorPlaybackState();}
EMSCRIPTEN_KEEPALIVE const unsigned* th11_mp_spectator_diagnostics(){
    auto& a=th11::sdl::app;static std::array<unsigned,34> out{};out.fill(0);
    const auto native=a.netplay.SpectatorDiagnostics();std::copy(native.begin(),native.end(),out.begin());
    out[21]=a.multiplayer_spectator_capture_attempts;out[22]=a.multiplayer_spectator_capture_failures;
    out[23]=a.multiplayer_spectator_capture_frame;out[24]=a.multiplayer_spectator_capture_stage;
    out[25]=a.multiplayer_spectator_capture_reason;out[26]=a.multiplayer_spectator_capture_bytes;
    out[27]=a.session.multiplayer_frame;out[28]=a.session.state.stage;out[29]=unsigned(a.session.state.phase);
    if(a.session.battle){out[30]=unsigned(a.session.battle->stage_active);out[31]=unsigned(a.session.battle->transitioning);
        out[32]=unsigned(a.session.battle->outgoing_stage!=nullptr);out[33]=unsigned(a.session.battle->stage!=nullptr);}
    return out.data();
}
EMSCRIPTEN_KEEPALIVE int th11_mp_spectator_control(const char* run,const char* action,unsigned frame){
    return th11::sdl::app.netplay.SpectatorPlaybackControl(run,action,frame)?1:0;
}
EMSCRIPTEN_KEEPALIVE int th11_mp_start(){using namespace th11::sdl;clear_inputs();return app.multiplayer_begin();}
EMSCRIPTEN_KEEPALIVE int th11_mp_pump(){return th11::sdl::app.multiplayer_pump();}
EMSCRIPTEN_KEEPALIVE const unsigned* th11_mp_status(){return th11::sdl::app.netplay.Status();}
EMSCRIPTEN_KEEPALIVE const double* th11_mp_pacing_statistics(){return th11::sdl::app.netplay.PacingStatistics();}
EMSCRIPTEN_KEEPALIVE const double* th11_mp_runtime_cost(int enabled){if(enabled>=0)th11::runtime_cost::enabled=enabled!=0;return th11::runtime_cost::bins.data();}
EMSCRIPTEN_KEEPALIVE const unsigned* th11_mp_rollback_status(){
    auto& a=th11::sdl::app;auto& out=a.rollback_status;out.fill(0);
    out[0]=1;out[1]=a.netplay.AllowsRollback();out[2]=a.rollback_world&&a.rollback_world->CanCapture();
    out[3]=a.netplay.Correcting();out[4]=a.netplay.RollbackFrame();out[5]=a.netplay.PredictionFrontier();
    out[6]=a.netplay.CaptureFrame();out[7]=a.netplay.CommittedThrough();out[8]=a.rollback_frames.size();out[9]=a.rollback_journal.FrameCount();
    out[10]=a.rollback_restores;out[11]=a.rollback_history_ticks;out[12]=a.rollback_peak_depth;
    out[13]=a.rollback_peak_bytes;out[14]=a.rollback_peak_blocks;out[15]=a.rollback_network_wait;out[16]=a.rollback_cpu_wait;
    out[17]=a.rollback_barrier;out[18]=a.multiplayer_touch_valid;out[19]=a.multiplayer_touch_frame;
    out[20]=a.rollback_revisions;out[21]=a.graphics.backend.stats.presentations;out[22]=a.rollback_published;out[23]=!a.error.empty()||*a.netplay.Error();return out.data();
}
EMSCRIPTEN_KEEPALIVE const unsigned* th11_mp_calibration_status(){return th11::sdl::app.netplay.CalibrationStatus();}
EMSCRIPTEN_KEEPALIVE const unsigned* th11_mp_recovery_statistics(){
    auto& a=th11::sdl::app;static std::array<unsigned,8> out{};
    out={1,unsigned(out.size()),a.rollback_exact_history_ticks,a.rollback_history_ticks,
        unsigned(a.rollback_frames.size()),unsigned(a.rollback_journal.FrameCount()),
        a.netplay.CommittedThrough(),unsigned(a.netplay.Correcting())};return out.data();
}
// Read-only complete-frontier observation; zero seat fields while invalid are
// unavailable evidence, not evidence that historical Bombs have ended.
EMSCRIPTEN_KEEPALIVE const unsigned* th11_mp_bomb_status(){
    auto& a=th11::sdl::app;static std::array<unsigned,24> out{};out.fill(0);
    out[0]=1;out[1]=unsigned(out.size());out[3]=a.netplay.Generation();
    out[4]=a.session.multiplayer_frame;out[5]=a.netplay.NextFrame();out[6]=a.netplay.PredictionFrontier();
    out[7]=a.netplay.CommittedThrough();out[8]=unsigned(a.session.state.phase);out[9]=a.netplay.Setup().player_count;
    out[10]=a.netplay.Correcting();out[11]=a.netplay.RollbackFrame();
    const auto* battle=a.session.battle.get();
    out[2]=a.netplay.Configured()&&a.multiplayer_world&&battle&&battle->mp_enabled&&
        (a.session.state.phase==th11::GameSessionPhase::stage||a.session.state.phase==th11::GameSessionPhase::paused)&&
        !out[10]&&out[11]==Netplay::INVALID_FRAME&&out[4]==out[5]&&out[5]==out[6]&&
        a.error.empty()&&!*a.netplay.Error();
    if(!out[2])return out.data();
    for(unsigned seat=0;seat<a.netplay.Setup().player_count&&seat<3;++seat){
        const auto* pilot=battle->pilots[seat].get();const auto* bomb=pilot?pilot->bomb:nullptr;
        if(!bomb)continue;auto* words=out.data()+12+seat*4;words[0]=1;words[1]=unsigned(bomb->state.active);
        words[2]=unsigned(bomb->state.elapsed.current);words[3]=unsigned(bomb->last_error);
    }return out.data();
}
EMSCRIPTEN_KEEPALIVE const double* th11_mp_rollback_profile(int enabled){
    auto& a=th11::sdl::app;if(enabled>=0)a.rollback_profile_enabled=enabled!=0;
    a.rollback_profile[0]=1;a.rollback_profile[1]=a.rollback_profile_enabled;return a.rollback_profile.data();
}
EMSCRIPTEN_KEEPALIVE const char* th11_mp_error(){auto& app=th11::sdl::app;return app.error.empty()?app.netplay.Error():app.error.c_str();}
EMSCRIPTEN_KEEPALIVE void th11_mp_stop(){using namespace th11::sdl;app.rollback_release();rollback_cadence.reset();app.netplay.Clear();app.audio.spectator_silent(false);app.session.multiplayer_active=false;app.multiplayer_world=false;app.multiplayer_viewer=false;app.multiplayer_local_results=false;app.multiplayer_finish_pending=false;app.multiplayer_restart_pending=false;app.multiplayer_spectator_stage=Netplay::INVALID_FRAME;app.multiplayer_spectator_checkpoint_serial=0;app.multiplayer_spectator_pending_initializer.clear();app.multiplayer_spectator_pending_stage=0;clear_inputs();}
EMSCRIPTEN_KEEPALIVE int th11_mp_replay_menu(const unsigned* build,unsigned count){
    using namespace th11::sdl;if(!build||count!=4||std::none_of(build,build+count,[](unsigned word){return word!=0;}))return 0;
    std::copy_n(build,4,app.multiplayer_build.begin());app.error.clear();return app.multiplayer_open_replay_menu();
}
EMSCRIPTEN_KEEPALIVE const unsigned* th11_mp_replay_ui_status(){
    using namespace th11::sdl;static std::array<unsigned,12> out{};out.fill(0);out[0]=1;
    if(app.multiplayer_viewer)out[1]=app.multiplayer_world?2:app.session.title&&app.session.title->screen==th11::TitleScreen::Exit?4:1;
    out[2]=app.netplay.NextFrame();out[3]=app.multiplayer_replay.FrameCount();out[4]=app.multiplayer_seek_target;
    const auto* menu=app.session.title.get();out[5]=menu?unsigned(menu->screen):0xffffffffu;
    if(menu){out[6]=unsigned(menu->substate);out[7]=unsigned(menu->cursor.selected);out[8]=unsigned(menu->page.selected);}
    out[9]=app.multiplayer_replay_index;out[10]=app.multiplayer_replay_stage;return out.data();
}
EMSCRIPTEN_KEEPALIVE int th11_mp_replay_validate(const unsigned char* bytes,unsigned size){
    th11::multiplayer::ReplayArchive replay;if(!replay.Load(bytes,size))return 0;
    auto& app=th11::sdl::app;
    return std::none_of(app.multiplayer_build.begin(),app.multiplayer_build.end(),[](unsigned word){return word!=0;})||app.multiplayer_build_matches(replay);
}
EMSCRIPTEN_KEEPALIVE int th11_mp_spectator_result(const unsigned char* bytes,unsigned size){
    using namespace th11::sdl;th11::multiplayer::ReplayArchive candidate;
    if(!app.netplay.Spectator()||app.multiplayer_local_results||!candidate.Load(bytes,size)||!app.multiplayer_build_matches(candidate)||!app.netplay.EnterSpectatorResults())return 0;
    const bool completed=candidate.Completed();app.multiplayer_replay=std::move(candidate);
    app.multiplayer_local_results=true;app.multiplayer_finish_pending=app.multiplayer_restart_pending=false;
    app.session.multiplayer_options.spectator=false;clear_inputs();
    return app.session.multiplayer_open_result(completed,0)?1:0;
}
EMSCRIPTEN_KEEPALIVE int th11_mp_replay_load(const unsigned char* bytes,unsigned size){
    using namespace th11::sdl;if(running)return 0;return app.multiplayer_load_replay(bytes,size);
}
EMSCRIPTEN_KEEPALIVE unsigned th11_mp_replay_size(){
    auto& a=th11::sdl::app;
    return a.multiplayer_replay.Encode(a.multiplayer_export,"PLAYER",a.multiplayer_recorded_score(),a.multiplayer_recorded_completed())?unsigned(a.multiplayer_export.size()):0;
}
EMSCRIPTEN_KEEPALIVE const unsigned char* th11_mp_replay_data(){return th11::sdl::app.multiplayer_export.data();}
EMSCRIPTEN_KEEPALIVE int th11_mp_replay_save(unsigned slot,const char* name){return th11::sdl::app.multiplayer_save(slot,name);}
EMSCRIPTEN_KEEPALIVE int th11_mp_replay_seek(unsigned frame){
    using namespace th11::sdl;if(!app.netplay.Playback()||frame>=app.multiplayer_replay.FrameCount())return 0;
    if(frame<app.netplay.NextFrame()){
        if(!app.netplay.BeginPlayback(app.multiplayer_replay.Setup())||!app.multiplayer_begin())return 0;clear_inputs();
    }
    app.multiplayer_seek_stage=0;app.multiplayer_seek_target=frame;return 1;
}
EMSCRIPTEN_KEEPALIVE void th11_mp_local_visibility(unsigned on){th11::sdl::app.session.multiplayer_local_visibility=on!=0;}
EMSCRIPTEN_KEEPALIVE void th11_mp_always_hitbox(unsigned on){th11::sdl::app.session.multiplayer_always_hitbox=on!=0;}
EMSCRIPTEN_KEEPALIVE const unsigned* th11_mp_game_status(){
    auto& app=th11::sdl::app;static std::array<unsigned,56> out{};out.fill(0);
    out[0]=1;out[1]=app.session.multiplayer_hash();out[2]=app.session.state.stage;out[3]=unsigned(app.session.state.phase);
    out[4]=app.session.multiplayer_frame;out[5]=app.multiplayer_replay.FrameCount();out[6]=app.netplay.NextFrame();
    out[7]=app.multiplayer_finish_pending;
    for(unsigned s=0;s<app.netplay.Setup().player_count&&s<3;++s){
        const auto v=app.session.multiplayer_seat(s);auto* p=out.data()+8+s*16;
        p[0]=v.active;p[1]=v.ghost;p[2]=v.life_state;p[3]=unsigned(v.lives);p[4]=unsigned(v.power);
        p[5]=unsigned(v.max_power);p[6]=unsigned(v.power_step);p[7]=unsigned(v.graze);p[8]=unsigned(v.communication);
        std::memcpy(p+9,&v.x,4);std::memcpy(p+10,&v.y,4);p[11]=unsigned(v.selection);p[12]=v.power_taps;
        p[13]=unsigned(v.life_fragments);p[14]=unsigned(v.score/10);
    }return out.data();
}
// Read-only scene evidence for full-run verification. This is deliberately
// separate from the world hash and never changes interpreter/menu state.
EMSCRIPTEN_KEEPALIVE const unsigned* th11_mp_scene_status(){
    const auto& s=th11::sdl::app.session;static std::array<unsigned,12> out{};out.fill(0);
    out[0]=1;out[1]=unsigned(s.state.phase);out[2]=s.state.stage;out[3]=s.multiplayer_frame;
    const auto* ending=s.ending.get();out[4]=ending!=nullptr;
    if(ending){
        out[5]=ending->active;out[6]=ending->flags;
        const auto& name=ending->message;
        out[7]=name.empty()?0:name=="staff.msg"?2:
            name.size()==7&&name[0]=='e'&&name[1]>='0'&&name[1]<='9'&&name[2]>='0'&&name[2]<='9'&&name.compare(3,4,".msg")==0?1:3;
        out[8]=ending->frames;out[9]=unsigned(ending->time.current);
    }
    out[10]=s.pause_menu?unsigned(s.pause_menu->state):0xffffffffu;
    out[11]=s.pause_menu?unsigned(s.pause_menu->completed):0;return out.data();
}
#endif
EMSCRIPTEN_KEEPALIVE int th11_validate_file(unsigned kind,const unsigned char* data,unsigned size){
#ifdef TH11_MULTIPLAYER
    return kind==1&&th11_mp_replay_validate(data,size);
#else
    if(kind==0){th11::ScoreFile scores;return scores.open(data,size);}
    if(kind==1){th11::Replay replay;return replay.open(data,size);}
    if(kind==2){th11::GameConfig config;return config.open(data,size);}
    return 0;
#endif
}
EMSCRIPTEN_KEEPALIVE int th11_save_scores(){return th11::sdl::app.save_scores();}
EMSCRIPTEN_KEEPALIVE int th11_save_replay(unsigned slot,const char* name){return th11::sdl::app.save_replay(slot,name);}
EMSCRIPTEN_KEEPALIVE int th11_initialize(){if(!th11::sdl::app.initialize())return 0;for(auto& k:th11::sdl::keyboard_map)k.native=SDL_GetScancodeFromName(k.sdl);th11::sdl::initialize_controller();return 1;}
EMSCRIPTEN_KEEPALIVE int th11_room_preview(){
#ifdef TH11_MULTIPLAYER
    using namespace th11::sdl;
    if(!app.initialized||app.netplay.Configured()||app.multiplayer_world)return 0;
    app.multiplayer_local_results=false;
    clear_inputs();app.session.suppress_title_demo();app.room_preview=true;return 1;
#else
    return 0;
#endif
}
EMSCRIPTEN_KEEPALIVE int th11_draw_loading(unsigned frames){return th11::sdl::app.draw_loading(frames);}
EMSCRIPTEN_KEEPALIVE int th11_prepare_loading(){return th11::sdl::app.prepare_loading();}
#if TH11_DEVELOPMENT_HARNESS
#ifdef TH_ENABLE_THPRAC
EMSCRIPTEN_KEEPALIVE int th11_probe_practice_menu(int character,int subtype,int difficulty){auto& a=th11::sdl::app;auto& s=a.session;if(!s.open_title(a.resources,false,th11::TitleScreen::Practice))return 0;s.title->selection.character=character;s.title->selection.partner=subtype;s.title->selection.difficulty=difficulty;return a.tick(0);}
EMSCRIPTEN_KEEPALIVE const double* th11_probe_practice_state(){static double words[20];const auto& s=th11::sdl::app.session;s.practice.run.encode(words);words[14]=s.practice.menu;words[15]=s.practice.active;words[16]=s.economy.power;words[17]=s.practice.cheats;words[18]=s.practice.tracker_bombs;words[19]=s.practice.tracker_misses;return words;}
EMSCRIPTEN_KEEPALIVE const int* th11_probe_title_state(){static int words[4];const auto* t=th11::sdl::app.session.title.get();words[0]=t?int(t->screen):-1;words[1]=t?t->substate:-1;words[2]=t?t->cursor.selected:-1;words[3]=t?t->timer.current:-1;return words;}
EMSCRIPTEN_KEEPALIVE int th11_probe_paused(){return th11::sdl::app.session.state.phase==th11::GameSessionPhase::paused;}
EMSCRIPTEN_KEEPALIVE int th11_probe_tracker_death(){auto* b=th11::sdl::app.session.battle.get();if(!b||!b->player)return -1;b->events.clear();if(!b->player->die())return -1;int deaths=0;for(const auto& e:b->events)if(e.kind==th11::BattleEventKind::Death)++deaths;return deaths;}
EMSCRIPTEN_KEEPALIVE int th11_probe_function_pose(int side){auto* b=th11::sdl::app.session.battle.get();if(!b||!b->player)return 0;auto& p=b->player->motion.state;p.x=side<0?-0x5c00:side>0?0x5c00:0;p.position.x=float(p.x)/128;p.warp=p.warp_timer=0;return 1;}
EMSCRIPTEN_KEEPALIVE const int* th11_probe_function_state(){static int out[5];const auto* b=th11::sdl::app.session.battle.get();if(!b||!b->player)return out;const auto& p=b->player->motion.state;out[0]=p.x;out[1]=p.warp;out[2]=p.weapon_mode;out[3]=b->player_input.movement.enemies;out[4]=b->player_input.movement.bomb;return out;}
EMSCRIPTEN_KEEPALIVE unsigned th11_probe_gameplay_held(){const auto* b=th11::sdl::app.session.battle.get();return b?b->player_input.movement.held:0;}
EMSCRIPTEN_KEEPALIVE const char* th11_probe_spell_name(unsigned id,unsigned rank){return th11::Localization::SpellName(id,"original",rank);}
EMSCRIPTEN_KEEPALIVE const double* th11_probe_practice_pending(){static double words[th11::PracticeConfig::word_count];th11::sdl::app.session.practice.configured.encode(words);return words;}
EMSCRIPTEN_KEEPALIVE const int* th11_probe_practice_tools(){static int words[12];const auto& s=th11::sdl::app.session.practice;words[0]=s.input.disable_xkey;words[1]=s.input.disable_shiftkey;words[2]=s.input.disable_zkey;words[3]=s.input.force_shiftkey;words[4]=s.input.enable_fast_retry;words[5]=s.show_keyboard_monitor;words[6]=s.map_inf_life_to_no_continue;words[7]=s.force_boss_move_down;words[8]=s.disable_master_display;words[9]=s.lock_marisa_b;words[10]=s.show_lock_timer;words[11]=s.all_clear_bonus;return words;}
EMSCRIPTEN_KEEPALIVE int th11_probe_practice_popup(){return th11::browser::ThpracUi::captures_pointer(-100,-100);}
EMSCRIPTEN_KEEPALIVE int th11_probe_records(){
 auto& a=th11::sdl::app;auto& s=a.session;if(!s.open_title(a.resources,false,th11::TitleScreen::Records))return 0;
 for(int id:{162,164,171}){auto* aggregate=s.scores.characters[6].data()+0x664+id*0x90;auto* own=s.scores.characters[0].data()+0x664+id*0x90;
  std::snprintf(reinterpret_cast<char*>(aggregate),64,"Original spell %d",id+1);
  const int seen=id+1,captured=id-160;std::memcpy(aggregate+0x84,&seen,4);std::memcpy(own+0x80,&captured,4);std::memcpy(own+0x84,&seen,4);
 }return a.tick(0);
}
EMSCRIPTEN_KEEPALIVE unsigned th11_probe_practice_windows(){unsigned mask=0;const char* names[]{"Mod Menu###th11-thprac-overlay","Tracker###th11-thprac-tracker","Advanced Options###th11-thprac-advanced"};for(unsigned i=0;i<3;++i){auto* w=ImGui::FindWindowByName(names[i]);if(w&&w->Active&&!w->Hidden)mask|=1u<<i;}return mask;}
static std::vector<th11::u8> th11_probe_replay;
EMSCRIPTEN_KEEPALIVE int th11_probe_practice_save(){return th11::sdl::app.session.save_replay("THPRAC",th11_probe_replay,true);}
EMSCRIPTEN_KEEPALIVE unsigned th11_probe_practice_replay_size(){return unsigned(th11_probe_replay.size());}
EMSCRIPTEN_KEEPALIVE const th11::u8* th11_probe_practice_replay_data(){return th11_probe_replay.data();}
EMSCRIPTEN_KEEPALIVE int th11_probe_practice_play(){auto& a=th11::sdl::app;return a.session.begin_replay(a.resources,th11_probe_replay.data(),unsigned(th11_probe_replay.size()))&&a.tick(0);}
#endif
EMSCRIPTEN_KEEPALIVE int th11_tick(unsigned held){return !th11::sdl::running&&th11::sdl::app.tick(held)?1:0;}
EMSCRIPTEN_KEEPALIVE int th11_probe_platform_tick(){return !th11::sdl::running&&th11::sdl::sample_and_tick()?1:0;}
EMSCRIPTEN_KEEPALIVE float th11_probe_player_x(){return th11::sdl::touch_state().x;}
EMSCRIPTEN_KEEPALIVE int th11_probe_touch_motion(){const auto* b=th11::sdl::app.session.battle.get();return b?b->player_input.movement.touch_mode:0;}
EMSCRIPTEN_KEEPALIVE int th11_probe_stage(unsigned stage,int character,int subtype,int difficulty){return th11::sdl::app.restart(stage,character,subtype,difficulty);}
EMSCRIPTEN_KEEPALIVE int th11_probe_finish(){auto& s=th11::sdl::app.session;if(!s.battle)return 0;s.economy.score_units=1234567;s.battle->completion.state.exit=th11::StageExit::Ending;return 1;}
EMSCRIPTEN_KEEPALIVE int th11_probe_complete(){auto& s=th11::sdl::app.session;return s.battle&&s.battle->dialogue_stage_complete();}
EMSCRIPTEN_KEEPALIVE int th11_probe_dialogue(int id){auto& app=th11::sdl::app;return app.session.battle&&app.session.battle->start_dialogue(id)&&app.text_events();}
EMSCRIPTEN_KEEPALIVE unsigned th11_text_writes(){return th11::sdl::app.fonts.writes;}
EMSCRIPTEN_KEEPALIVE int th11_audio_probe(unsigned command,int value,float x){
    auto& a=th11::sdl::app.audio;
    if(command==0)return a.music(value);
    if(command==1){a.effects.positioned(value,x);a.update();return 1;}
    if(command==2){for(unsigned n=1;n<=56;++n)a.sound_stop(n);a.effects.reset();return a.music(-1);}
    if(command==3){a.fade_music(value);return 1;}
    if(command==4){a.update();return 1;}
    return 0;
}
EMSCRIPTEN_KEEPALIVE const float* th11_audio_samples(){static float pcm[2048];return th11::sdl::app.audio.mix(pcm,1024)?pcm:nullptr;}
#endif
EMSCRIPTEN_KEEPALIVE const char* th11_error(){return th11::sdl::app.error.c_str();}
EMSCRIPTEN_KEEPALIVE unsigned th11_frame(){return th11::sdl::app.session.state.frame;}
EMSCRIPTEN_KEEPALIVE unsigned th11_phase(){return unsigned(th11::sdl::app.session.state.phase);}
EMSCRIPTEN_KEEPALIVE int th11_return_title(){return th11::sdl::app.return_to_title()?1:0;}
#ifdef TH11_MULTIPLAYER
EMSCRIPTEN_KEEPALIVE int th11_restart(){return 0;}
EMSCRIPTEN_KEEPALIVE int th11_pause(){auto& a=th11::sdl::app;if(a.netplay.ReadOnly()||!a.multiplayer_world)return 0;a.multiplayer_pause_requested=a.session.state.phase==th11::GameSessionPhase::stage;return 1;}
EMSCRIPTEN_KEEPALIVE int th11_resume(){auto& a=th11::sdl::app;if(a.netplay.ReadOnly()||!a.multiplayer_world)return 0;a.multiplayer_pause_requested=a.session.state.phase==th11::GameSessionPhase::paused;return 1;}
#else
EMSCRIPTEN_KEEPALIVE int th11_restart(){return th11::sdl::app.restart()?1:0;}
EMSCRIPTEN_KEEPALIVE int th11_pause(){return th11::sdl::app.pause()?1:0;}
EMSCRIPTEN_KEEPALIVE int th11_resume(){return th11::sdl::app.resume()?1:0;}
#endif
EMSCRIPTEN_KEEPALIVE void th11_key(unsigned scan,unsigned down){for(auto& k:th11::sdl::keyboard_map)if(k.scan==scan)k.hosted=down!=0;}
EMSCRIPTEN_KEEPALIVE void th11_keys_clear(){th11::sdl::clear_inputs();}
#ifdef TH_ENABLE_THPRAC
__attribute__((export_name("sdl_thprac_mouse"))) void sdl_thprac_mouse(int type,float x,float y){th11::browser::ThpracUi::mouse(type,x,y);}
EMSCRIPTEN_KEEPALIVE int th11_practice_configure(const double* words,unsigned count){th11::PracticeConfig p;if(!p.decode(words,count))return 0;auto& s=th11::sdl::app.session;s.practice.configured=p;s.practice.warp=p.section>=10000?1:p.section?(th11::practice_sections[p.section].spell?5:4):0;if(p.section>0&&p.section<10000)s.practice.spell_category=th11::practice_sections[p.section].appearance>7?th11::practice_sections[p.section].appearance-7:0;return 1;}
#endif
EMSCRIPTEN_KEEPALIVE void th11_music_enabled(unsigned on){using namespace th11::sdl;app.audio.music_enabled=on!=0;app.audio.refresh_volume();}
EMSCRIPTEN_KEEPALIVE const unsigned* th11_audio_statistics(){return th11::sdl::app.audio.statistics();}
bool interpolation_ready(){
    auto& application=th11::sdl::app;
    if(!application.initialized||!application.error.empty())return false;
    const auto& session=application.session;
    if(session.state.phase!=th11::GameSessionPhase::stage)return false;
    if(!session.battle||!session.battle->stage_active||session.battle->transitioning)return false;
    if(session.battle->dialogue&&session.battle->dialogue->active)return false;
    return true;
}
#ifdef TH11_PRESENTATION_DIAGNOSTICS
// Diagnostic surface for high-refresh verification. Read-only; never affects
// simulation. Excluded from production packages by build.mjs.
EMSCRIPTEN_KEEPALIVE const unsigned* th11_presentation_state(){
    static unsigned state[10];
    auto& s=th11::sdl::app.session;
    state[0]=th11::sdl::presentation.high_refresh?1u:0u;
#ifdef TH11_MULTIPLAYER
    state[1]=unsigned(th11::sdl::multiplayer_presentation.alpha()*1000.);
#else
    state[1]=unsigned(th11::sdl::cadence.interpolation_alpha()*1000.);
#endif
    state[2]=th11::sdl::presentation_draws;
    state[3]=interpolation_ready()?1u:0u;
    state[4]=unsigned(s.state.phase);
    state[5]=s.battle?1u:0u;
    state[6]=s.battle&&s.battle->stage_active?1u:0u;
    state[7]=(s.battle&&s.battle->transitioning?1u:0u)|((s.battle&&s.battle->dialogue&&s.battle->dialogue->active)?2u:0u);
    state[8]=unsigned(th11::sdl::app.presentation_fps*100.f);
    state[9]=th11::sdl::app.session.state.frame;
    return state;
}
#endif
#ifdef TH_PRESENTATION_AUDIT
EMSCRIPTEN_KEEPALIVE void th11_loop_start();
// A launch may finish after the lab has already frozen the session; block the
// shell's loop start until resume() so freeze is not silently undone.
bool presentation_lab_frozen=false;
// Isolated diagnostic driver. Freeze stops the display callback without an
// in-game pause; tick and draw are separate transactions.
EMSCRIPTEN_KEEPALIVE int presentation_lab_freeze(){
    using namespace th11::sdl;
    presentation_lab_frozen=true;
    if(!running)return 0;
    running=false;++loop_epoch;previous_frame=-1;cadence.reset();presentation.reset();presentation_gate.reset();
    app.audio.suspend(true);clear_inputs();app.graphics.backend.defer=false;
    return 0;
}
EMSCRIPTEN_KEEPALIVE int presentation_lab_resume(){
    using namespace th11::sdl;
    presentation_lab_frozen=false;
    if(running)return 0;
    th11_loop_start();
    return running?0:1;
}
EMSCRIPTEN_KEEPALIVE int presentation_lab_tick(){
    using namespace th11::sdl;
    if(running)return -1;
    // The lab studies the high-refresh presentation, so authoritative
    // references use the same continuous-submission rule as live 120 Hz.
    th11::high_refresh::smooth_submission=true;
    return sample_and_tick()?0:1;
}
EMSCRIPTEN_KEEPALIVE int presentation_lab_draw(float alpha,int world){
    using namespace th11::sdl;
    if(running)return -1;
    ++presentation_draws;
    return app.presentation_draw(alpha,true,world!=0)?0:1;
}
// Opens a real stage through the production restart path so a diagnostic
// session has gameplay owners to observe. Diagnostic builds only.
EMSCRIPTEN_KEEPALIVE int presentation_lab_start_stage(unsigned stage,int character,int subtype,int difficulty){
    using namespace th11::sdl;
    if(running)return -1;
    return app.restart(stage,character,subtype,difficulty)?0:1;
}
// Recompute the authoritative digests for every read so a draw-only pass is
// compared against the current state, not a cached post-update snapshot.
extern "C" __attribute__((export_name("audit_state"))) const th11::u32* audit_state_live(){
    th11::presentation_audit::sample_state(th11::sdl::app.session);
    return th11::presentation_audit::state();
}
EMSCRIPTEN_KEEPALIVE void presentation_lab_fault(unsigned on){th11::presentation_audit::fault(on!=0);}
EMSCRIPTEN_KEEPALIVE int presentation_lab_gate(){using namespace th11::sdl;return presentation_gate.primed?1:0;}
EMSCRIPTEN_KEEPALIVE int presentation_lab_world_frozen(){return interpolation_ready()?0:1;}
// Start an archive demo (0..3) or an explicit replay payload without the title
// menu, so a campaign can cover real stage/boss motion.
// Read-only replay metadata: [count, character, subtype, difficulty,
// stage1..7 numbers, stage1..7 frames].
EMSCRIPTEN_KEEPALIVE const unsigned* presentation_lab_replay_info(const unsigned char* data,unsigned size){
    static unsigned info[20]{};
    th11::Replay replay;
    if(!data||!replay.open(data,size)){info[0]=0;return info;}
    info[0]=replay.stage_count();info[1]=replay.character();info[2]=replay.subtype();info[3]=replay.difficulty();
    for(th11::u32 i=0;i<8;++i){const auto* stage=replay.stage(i);info[4+i]=stage?stage->number:0u;info[12+i]=stage?stage->frames:0u;}
    return info;
}
EMSCRIPTEN_KEEPALIVE int presentation_lab_demo(unsigned index){
    using namespace th11::sdl;
    if(running)return -1;
    if(!app.session.load_demo(index))return 1;
    return app.tick(0)?0:2;
}
EMSCRIPTEN_KEEPALIVE int presentation_lab_replay(const unsigned char* data,unsigned size,unsigned stage){
    using namespace th11::sdl;
    if(running||!data||size<16)return -1;
    if(!app.session.begin_replay(app.resources,data,size,stage,false))return 1;
    return app.tick(0)?0:2;
}
#endif
EMSCRIPTEN_KEEPALIVE const unsigned* th11_sfx_statistics(){return th11::sdl::app.audio.sound_statistics();}
EMSCRIPTEN_KEEPALIVE const double* th11_audio_pump_profile(int enabled){return th11::sdl::app.audio.pump_profile(enabled);}
EMSCRIPTEN_KEEPALIVE void th11_loop_pause(unsigned on){
    using namespace th11::sdl;
#ifdef TH11_MULTIPLAYER
    if(app.netplay.Configured()&&!app.netplay.ReadOnly()){
        if(on&&app.session.state.phase==th11::GameSessionPhase::stage)app.multiplayer_pause_requested=true;
        suspended=false;app.audio.suspend(on!=0);previous_frame=-1;reset_cadences();clear_inputs();app.reset_frame_window();return;
    }
#endif
    suspended=on!=0;app.audio.suspend(suspended);previous_frame=-1;reset_cadences();clear_inputs();app.reset_frame_window();
}
EMSCRIPTEN_KEEPALIVE void th11_loop_stop(){using namespace th11::sdl;running=false;++loop_epoch;previous_frame=-1;reset_cadences();clear_inputs();app.audio.suspend(true);app.reset_frame_window();}
EMSCRIPTEN_KEEPALIVE void th11_audio_close(){th11::sdl::app.audio.close();}
#ifdef TH11_MULTIPLAYER
EMSCRIPTEN_KEEPALIVE void th11_mp_continue_recovery(unsigned epoch){
    using namespace th11::sdl;
    if(!recovery_tasks_enabled||epoch!=loop_epoch||!running||suspended||!recovery_tasks_remaining||
       !app.netplay.AllowsRollback()||app.netplay.ReadOnly())return;
    const bool correcting=app.netplay.Correcting()||app.netplay.RollbackFrame()!=Netplay::INVALID_FRAME;
    if(!correcting&&(rollback_cadence.retry_pending()||rollback_cadence.phase()<th11::multiplayer::FrameSchedule::interval))return;
    --recovery_tasks_remaining;
    const double begin=emscripten_get_now();
    app.audio.begin_pump_profile_frame();
    if(app.rollback_profile_enabled){app.rollback_profile.fill(0);app.rollback_profile[0]=app.rollback_profile[1]=1;}
    app.rollback_callback_ms=begin;app.graphics.backend.defer=true;
    // Only rAF adds elapsed wall-time. A continuation may spend CPU credit
    // already returned by that rAF, sampling each NEW due input exactly once.
    // Historical transactions still neither sample nor complete forward debt.
    bool ok=app.multiplayer_pump();unsigned advanced=0;
    const auto generation=app.netplay.Generation(),history=app.rollback_history_ticks;
    const auto ticks=rollback_cadence.advance(0);
    if(ok&&!ticks&&correcting){ok=sample_and_tick(false);if(ok)ok=app.multiplayer_tick(false);}
    for(unsigned n=0;n<ticks&&ok;++n){
        const auto actual=app.netplay.Confirmed();
        const bool exact_pair=history==app.rollback_history_ticks&&app.rollback_frames.empty()&&actual!=Netplay::INVALID_FRAME&&actual>=app.netplay.NextFrame();
        if(!th11::multiplayer::FrameSchedule::can_start(advanced,th11::u64(std::max(0.,emscripten_get_now()-begin)*1000000.),exact_pair))break;
        ok=sample_and_tick();
        if(app.netplay.Generation()!=generation){rollback_cadence.reset();break;}
        if(app.multiplayer_advanced){++advanced;if(!rollback_cadence.complete()){app.error="TH11 continued forward clock consumed twice";ok=false;}}
        else{if(app.rollback_cpu_wait)rollback_cadence.cpu_yield();else rollback_cadence.network_wait();break;}
    }
    rollback_cadence.cpu_yield();
    if(!app.netplay.FlushDeferredInputs()){if(app.error.empty())app.error=app.netplay.Error();ok=false;}
    if(!ok||app.netplay.Correcting())app.graphics.backend.cancel_present();
    {auto timed=app.rollback_timer(11);app.graphics.backend.commit();}
    app.graphics.backend.defer=false;app.rollback_callback_ms=-1;
    {auto timed=app.rollback_timer(12);app.audio.pump();}
    if(app.rollback_profile_enabled){app.rollback_profile[2]=emscripten_get_now()-begin;app.rollback_profile[15]=advanced;}
    th11_browser_frame(ok?1:0,emscripten_get_now()-begin,advanced);
    if(!ok)running=false;
    const bool pending=(app.rollback_cpu_wait&&app.netplay.Correcting())||(!rollback_cadence.retry_pending()&&rollback_cadence.phase()>=th11::multiplayer::FrameSchedule::interval);
    if(running&&recovery_tasks_remaining&&pending)th11_request_recovery_task(epoch);
}
#endif
EMSCRIPTEN_KEEPALIVE void th11_loop_start(){
    using namespace th11::sdl;
#ifdef TH_PRESENTATION_AUDIT
    if(presentation_lab_frozen)return;
#endif
    presentation.reset();presentation_gate.reset();app.reset_presentation_frames();if(running||!app.initialized)return;app.release_loading();
    running=true;suspended=false;previous_frame=-1;reset_cadences();clear_inputs();app.audio.suspend(false);app.reset_frame_window();
    emscripten_request_animation_frame_loop([](double time,void* epoch)->EM_BOOL{
        if(!running||uintptr_t(epoch)!=loop_epoch)return EM_FALSE;
        const double begin=emscripten_get_now(),delta=previous_frame<0?0:(time-previous_frame)/1000.;previous_frame=time;
        // Reset before any exact/rollback tick: every internal pump contributes.
        app.audio.begin_pump_profile_frame();
#ifdef TH11_MULTIPLAYER
        if(app.room_preview){
            if(suspended){cadence.reset();presentation.reset();presentation_gate.reset();return EM_TRUE;}
            const auto ticks=cadence.advance(delta);bool ok=true;app.graphics.backend.defer=true;
            for(unsigned n=0;n<ticks&&ok;++n)ok=sample_and_tick();
            app.graphics.backend.commit();app.graphics.backend.defer=false;app.audio.pump();
            if(ticks)th11_browser_frame(ok?1:0,emscripten_get_now()-begin,ticks);
            if(!ok)running=false;return running?EM_TRUE:EM_FALSE;
        }
        recovery_tasks_remaining=2; // hard cap between real rAF callbacks
        const bool display_ready=!suspended&&app.multiplayer_world&&!app.multiplayer_finish_pending&&
            interpolation_ready()&&!th11_limit_presentation_to_60();
        if(display_ready)presentation.advance(delta);
        else{presentation.reset();app.reset_presentation_frames();}
        th11::high_refresh::smooth_submission=display_ready&&presentation.high_refresh;
        th11::runtime_cost::begin();
        if(app.rollback_profile_enabled){app.rollback_profile.fill(0);app.rollback_profile[0]=1;app.rollback_profile[1]=1;}
        const bool capture_follows=!suspended&&app.netplay.AllowsRollback()&&
            (rollback_cadence.retry_pending()||rollback_cadence.phase()+std::max(0.,delta)>=th11::multiplayer::FrameSchedule::interval);
        bool ok;{auto measured=app.rollback_timer(13);ok=app.multiplayer_pump(capture_follows);}unsigned advanced=0;
        app.rollback_callback_ms=begin;
        if(!suspended&&ok){
            app.graphics.backend.defer=true;
            if(app.netplay.AllowsRollback()&&!app.netplay.ReadOnly()){
                const auto generation=app.netplay.Generation();
                const auto history_at_entry=app.rollback_history_ticks;
                const auto ticks=rollback_cadence.advance(app.netplay.PacedElapsed(delta));
                if(!ticks&&(app.netplay.Correcting()||app.netplay.RollbackFrame()!=Netplay::INVALID_FRAME)){
                    // Freeze and send the old complete frontier before the
                    // first Restore, even on a high-refresh history-only turn.
                    ok=sample_and_tick(false);if(ok)ok=app.multiplayer_tick(false);
                }
                for(unsigned n=0;n<ticks&&ok;++n){
                    // Pair only fully confirmed forward transactions. Building
                    // more predicted history to amortize Draw instead raises
                    // correction depth and stalled-presentation tails.
                    const auto actual=app.netplay.Confirmed();
                    const bool exact_pair=history_at_entry==app.rollback_history_ticks&&app.rollback_frames.empty()&&
                        actual!=Netplay::INVALID_FRAME&&actual>=app.netplay.NextFrame();
                    if(!th11::multiplayer::FrameSchedule::can_start(advanced,th11::u64(std::max(0.,emscripten_get_now()-begin)*1000000.),exact_pair))break;
                    ok=sample_and_tick();
                    if(app.netplay.Generation()!=generation){rollback_cadence.reset();break;}
                    if(app.multiplayer_advanced){++advanced;if(!rollback_cadence.complete()){app.error="TH11 forward clock consumed twice";ok=false;}}
                    else{if(app.rollback_cpu_wait)rollback_cadence.cpu_yield();else rollback_cadence.network_wait();break;}
                }
                rollback_cadence.cpu_yield(); // exactly once; no-op after a settled network wait
            }else{
                // Offering work is not completing it. The old FrameCadence
                // subtracted every due tick before the CPU gate could stop
                // the loop, silently losing two/three-tick catch-up offers.
                const auto ticks=exact_cadence.advance(app.netplay.PacedElapsed(delta));
                const bool stage_seeking=app.netplay.StagePlaybackReady()&&app.netplay.StagePlayback().FastForwarding();
                const bool seeking=stage_seeking||app.multiplayer_replay_seeking();
                const auto seek_target=stage_seeking?app.netplay.StagePlayback().Target():app.multiplayer_seek_target;
                const unsigned budget=seeking?120:ticks;
                for(unsigned n=0;n<budget&&ok;++n){
                    if(n&&emscripten_get_now()-begin>=8)break;
                    const auto beforeFrame=app.netplay.NextFrame(),beforeGeneration=app.netplay.Generation();
                    ok=sample_and_tick();advanced+=unsigned(app.multiplayer_advanced);
                    if(app.netplay.Generation()!=beforeGeneration||app.netplay.NextFrame()<beforeFrame||!app.multiplayer_world){exact_cadence.reset();break;}
                    if(!app.multiplayer_advanced){if(!seeking)exact_cadence.network_wait();break;}
                    if(!seeking&&ok&&!exact_cadence.complete()){app.error="TH11 exact frame clock consumed twice";ok=false;break;}
                    if(seeking&&(stage_seeking?app.netplay.NextFrame()>=seek_target:!app.multiplayer_replay_seeking()))break;
                }
                if(seeking)exact_cadence.reset();else exact_cadence.cpu_yield();
            }
            if(!app.netplay.FlushDeferredInputs()){if(app.error.empty())app.error=app.netplay.Error();ok=false;}
            const bool seeking=app.netplay.StagePlaybackReady()&&
                (app.netplay.StagePlayback().Paused()||app.netplay.StagePlayback().Seeking()||app.netplay.StagePlayback().FastForwarding());
            const bool display_eligible=ok&&presentation.high_refresh&&interpolation_ready()&&
                app.multiplayer_world&&!app.multiplayer_finish_pending&&!app.netplay.Correcting()&&
                app.netplay.RollbackFrame()==Netplay::INVALID_FRAME&&!seeking&&
                !app.multiplayer_replay_seeking()&&
                !th11_limit_presentation_to_60();
            const auto phase=app.netplay.AllowsRollback()&&!app.netplay.ReadOnly()?rollback_cadence.phase():exact_cadence.phase();
            const bool draw=multiplayer_presentation.observe(delta,display_eligible,app.netplay.Generation(),
                app.rollback_revisions,app.session.state.stage,app.netplay.NextFrame(),phase);
            if(draw){++presentation_draws;ok=app.presentation_draw(multiplayer_presentation.alpha(),true,true);}
            if(!ok||app.netplay.Correcting())app.graphics.backend.cancel_present();
            {auto measured=app.rollback_timer(11);app.graphics.backend.commit();}app.graphics.backend.defer=false;
        }else{cadence.reset();rollback_cadence.reset();exact_cadence.reset();}
        if(!app.netplay.FlushDeferredInputs()){if(app.error.empty())app.error=app.netplay.Error();ok=false;}
        app.rollback_callback_ms=-1;
        // A missing input never blocks WebRTC callbacks, confirmation repair
        // or audio pumping, and never calls a simulation-bearing Draw twice.
        {auto measured=app.rollback_timer(12);app.audio.pump();}
        if(app.rollback_profile_enabled){app.rollback_profile[2]=emscripten_get_now()-begin;app.rollback_profile[15]=advanced;}
        th11_browser_frame(ok?1:0,emscripten_get_now()-begin,advanced);
        const bool pending=(app.rollback_cpu_wait&&app.netplay.Correcting())||(!rollback_cadence.retry_pending()&&rollback_cadence.phase()>=th11::multiplayer::FrameSchedule::interval);
        if(recovery_tasks_enabled&&ok&&!suspended&&app.netplay.AllowsRollback()&&!app.netplay.ReadOnly()&&pending)th11_request_recovery_task(loop_epoch);
        if(!ok)running=false;return running?EM_TRUE:EM_FALSE;
#else
        if(suspended){cadence.reset();
#ifdef TH_ENABLE_THPRAC
            practice_cadence.reset();
#endif
            presentation.reset();presentation_gate.reset();return EM_TRUE;}
        bool custom_speed=false;
#ifdef TH_ENABLE_THPRAC
        const auto period=th11::browser::ThpracUi::simulation_interval(app.session);
        custom_speed=std::abs(period-1./60.)>1e-12;
        if(period!=practice_cadence.period){practice_cadence.period=period;practice_cadence.reset();cadence.reset();presentation.reset();presentation_gate.reset();}
#endif
        // Display cadence is independent of the fixed 60 Hz simulation. The
        // kill switch restores retail one-tick-per-picture presentation.
        const bool limit60=th11_limit_presentation_to_60()!=0;
        const bool ready=interpolation_ready()&&!limit60&&!custom_speed;
        if(ready)presentation.advance(delta);else{app.reset_presentation_frames();presentation.reset();}
#ifdef TH_ENABLE_THPRAC
        const auto ticks=custom_speed?practice_cadence.advance(delta):cadence.advance(delta);
#else
        const auto ticks=cadence.advance(delta);
#endif
        bool ok=true;app.graphics.backend.defer=true;
        const bool high=presentation.high_refresh&&ready;
        th11::high_refresh::smooth_submission=high;
        const bool interpolate=presentation_gate.advance(high,ticks!=0);
        for(unsigned n=0;n<ticks&&ok;++n)ok=sample_and_tick();
        // Extra draw-only frames never consume input, RNG, audio or Replay.
        if(ok&&high){++presentation_draws;ok=app.presentation_draw(cadence.interpolation_alpha(),interpolate,true);}
        app.graphics.backend.commit();app.graphics.backend.defer=false;
        app.audio.pump();
        if(ticks)th11_browser_frame(ok?1:0,emscripten_get_now()-begin,ticks);
        if(!ok)running=false;return running?EM_TRUE:EM_FALSE;
#endif
    },reinterpret_cast<void*>(uintptr_t(++loop_epoch)));
}
EMSCRIPTEN_KEEPALIVE void th11_touch(unsigned type,int id,float x,float y){th11::sdl::touch_event(type,id,x,y);}
EMSCRIPTEN_KEEPALIVE void th11_touch_cancel(){th11::sdl::cancel_touch();}
EMSCRIPTEN_KEEPALIVE void th11_always_hitbox(unsigned on){th11::sdl::app.session.always_hitbox=on!=0;}
EMSCRIPTEN_KEEPALIVE void th11_touch_options(unsigned enabled,unsigned mode,float sensitivity,unsigned two_finger,unsigned double_tap){using namespace th11::sdl;gestures.enabled=enabled!=0;gestures.unlimited=mode==1;gestures.sensitivity=std::isfinite(sensitivity)?std::clamp(sensitivity,1.f,3.f):1;gestures.two_finger=two_finger!=0;gestures.double_tap=double_tap!=0;if(gestures.set_mode(mode<=3?int(mode):0))neutralize_live_touch();if(!enabled)gestures.cancel();}
EMSCRIPTEN_KEEPALIVE void th11_touch_controls(unsigned enabled,unsigned fire,unsigned focus,unsigned bomb,unsigned escape){using namespace th11::sdl;gestures.enabled=enabled!=0;gestures.controls(fire!=0,focus!=0,bomb,escape,0,0);}
EMSCRIPTEN_KEEPALIVE void th11_touch_stick(float x,float y){using namespace th11::sdl;gestures.stick_x=std::isfinite(x)?std::clamp(x/32767.f,-1.f,1.f):0;gestures.stick_y=std::isfinite(y)?std::clamp(y/32767.f,-1.f,1.f):0;}
// Shared-host ABI (mirrors th10/th20 cpp/sdl/GameHost.cpp). The portable
// eagler-host.mjs drives touch through these names; game entry points above
// stay TH11-specific for the development harness.
__attribute__((export_name("sdl_touch"))) void sdl_touch(unsigned type,int id,float x,float y){th11_touch(type,id,x,y);}
__attribute__((export_name("sdl_touch_cancel"))) void sdl_touch_cancel(){th11_touch_cancel();}
__attribute__((export_name("sdl_touch_options"))) void sdl_touch_options(unsigned on,unsigned free,float speed){using namespace th11::sdl;gestures.enabled=on!=0;gestures.unlimited=free!=0;gestures.sensitivity=std::isfinite(speed)?std::clamp(speed,1.f,3.f):1;if(!on)gestures.cancel();}
__attribute__((export_name("sdl_touch_gestures"))) void sdl_touch_gestures(unsigned two,unsigned taps){using namespace th11::sdl;gestures.two_finger=two!=0;gestures.double_tap=taps!=0;}
__attribute__((export_name("sdl_touch_mode"))) void sdl_touch_mode(unsigned mode){using namespace th11::sdl;if(gestures.set_mode(mode<=3?int(mode):0))neutralize_live_touch();}
__attribute__((export_name("sdl_touch_controls"))) void sdl_touch_controls(unsigned shoot,unsigned slow,unsigned bomb,unsigned escape,float x,float y){using namespace th11::sdl;gestures.controls(shoot!=0,slow!=0,bomb,escape,x,y);}
__attribute__((export_name("sdl_loop_pause"))) void sdl_loop_pause(unsigned on){th11_loop_pause(on);}
__attribute__((export_name("sdl_music_resource_changed"))) void sdl_music_resource_changed(){th11::sdl::app.audio.resource_changed();}
}

#include "LocalReplay.inc"
