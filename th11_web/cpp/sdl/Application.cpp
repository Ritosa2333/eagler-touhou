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
#endif
#include "../game/FrameStatistics.hpp"
#include "../game/AnmRenderer.hpp"
#include <SDL3/SDL.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include "../../../portable/sdl/FrameCadence.hpp"
#include "../../../portable/input/TouchController.hpp"
#include <algorithm>
EM_JS(int, th11_browser_keyboard, (), { return typeof Module["resetBrowserKeyboard"] === "function"; });
EM_JS(void, th11_reset_browser_keyboard, (), { Module["resetBrowserKeyboard"]?.(); });
EM_JS(void, th11_browser_frame, (int ok,double milliseconds,unsigned ticks), { Module["onGameFrame"]?.(ok,milliseconds,ticks); });
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
    AsciiText frame_text{session.resources.core.ascii};
    AnmRenderer renderer;
    std::vector<u8> archive;
    std::vector<u8> saved_score_state,saved_config_state;
    bool initialized=false,platform_prepared=false;
    AnmResource loading_signature;
    AnmManager loading_animations;
    AnmVm* loading_credit=nullptr;
    std::string error;

#ifdef TH11_MULTIPLAYER
    multiplayer::NetplayRuntime netplay;
    multiplayer::ReplayArchive multiplayer_replay;
    std::vector<u8> multiplayer_export;
    bool multiplayer_world=false,multiplayer_advanced=false,multiplayer_finish_pending=false;
    bool multiplayer_restart_pending=false,multiplayer_saved=false,multiplayer_pause_requested=false;
    u32 multiplayer_seek_target=0,multiplayer_last_stage=0;
    std::string multiplayer_auto_path;
    std::array<u32,4> multiplayer_build{};
    bool multiplayer_viewer=false;
    u32 multiplayer_replay_index=0,multiplayer_replay_stage=1;
    GameInput multiplayer_viewer_input;
    bool multiplayer_build_matches(const multiplayer::ReplayArchive& replay)const {
        return std::any_of(multiplayer_build.begin(),multiplayer_build.end(),[](u32 word){return word!=0;})&&replay.Setup().build==multiplayer_build;
    }
    bool multiplayer_completed()const {
        return (session.title&&session.title->multiplayer_result)||
            (session.pause_menu&&session.pause_menu->multiplayer_result&&session.pause_menu->completed);
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
        if(!initialize())return false;
        netplay.Clear();multiplayer_viewer=true;multiplayer_world=false;multiplayer_advanced=false;
        multiplayer_finish_pending=multiplayer_restart_pending=multiplayer_pause_requested=false;
        multiplayer_seek_target=0;multiplayer_viewer_input={};
        renderer.invalidate();renderer.multiplayer_animations=nullptr;renderer.multiplayer_hud_background=nullptr;renderer.multiplayer_opacity.fill(255);
        session.multiplayer_active=false;session.animations.rate=1;
        if(!session.open_title(resources,false,TitleScreen::Replays)){error=session.error;return false;}
        session.title->multiplayer_replay_menu=true;
        session.title->replay_files.resize(std::max<size_t>(100,(multiplayer_replay_index/25+1)*25));
        session.title->last_replay=int(multiplayer_replay_index);
        clear_inputs();audio.effects.reset();return tick(0);
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
        multiplayer_seek_target=target;return true;
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
        // These menus replay the recorded P1 controls on every consumer. Only
        // a live player's isolated storage may service their file writes.
        if(session.title&&session.title->replay_save_requested){auto& menu=*session.title;
            if(!netplay.ReadOnly()&&!multiplayer_save(menu.replay_file+1,menu.entered_name.data()))return false;
            menu.replay_save_requested=false;menu.pending_replay.reset();scan_replays();
        }
        if(session.pause_menu&&session.pause_menu->save_requested){auto& menu=*session.pause_menu;
            if(!netplay.ReadOnly()&&!multiplayer_save(menu.cursor.selected+1,menu.entered_name.data()))return false;
            menu.save_requested=false;scan_replays();
        }
        return true;
    }
    bool multiplayer_begin(){
        if(!netplay.Configured()||!initialize())return false;
        const auto& setup=netplay.Setup();MultiplayerOptions opts;
        opts.seat_count=setup.player_count;opts.local_seat=setup.local_player;
        opts.stage=setup.initial_stage();opts.difficulty=int(setup.difficulty);opts.seed=setup.seed;
        for(unsigned s=0;s<3;++s)opts.selections[s]=int(setup.selections[s]);
        renderer.invalidate();
        auto& core=session.resources.core;
        for(AnmResource* file:{&core.text,&core.ascii,&core.bullet,&core.enemy,&core.front,&core.title,&core.title_variant,&core.players[0],&core.players[1]})graphics.unload(*file);
        if(session.resources.stage)for(AnmResource* file:{&session.resources.stage->background,&session.resources.stage->logo,&session.resources.stage->enemies})graphics.unload(*file);
        if(!session.begin_multiplayer(resources,opts)){error=session.error;return false;}
        for(AnmResource* file:{&core.text,&core.ascii,&core.bullet,&core.enemy,&core.front,&core.title,&core.title_variant,&core.players[0],&core.players[1],&session.resources.stage->background,&session.resources.stage->logo,&session.resources.stage->enemies})
            if(!graphics.preload(*file)){error=graphics.error;return false;}
        multiplayer_world=true;multiplayer_advanced=false;multiplayer_finish_pending=multiplayer_restart_pending=multiplayer_saved=multiplayer_pause_requested=false;clear_inputs();
        multiplayer_last_stage=opts.stage;multiplayer_seek_target=0;multiplayer_auto_path.clear();frame_statistics.reset_run();
        if(netplay.Playback())session.recording_timestamp=multiplayer_replay.Timestamp();
        if(!play_stage_music(session.state.stage))return false;return audio_events()&&text_events();
    }
    bool multiplayer_pump(){
        if(!netplay.Configured())return true;
        if(multiplayer_finish_pending&&netplay.ReadOnly()&&!netplay.Finish()){error="Invalid TH11 read-only terminal frame";return false;}
        if(!netplay.Pump(multiplayer_world)){error=netplay.Error();return false;}
        // Terminal frames keep both input directions alive until every peer
        // acknowledged the fence. Retirement never depends on another tick.
        if(multiplayer_finish_pending){
            if(!netplay.Finish()){if(*netplay.Error()){error=netplay.Error();return false;}return true;}
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
        if(netplay.ReadOnly()||!multiplayer_replay.FrameCount())return true;
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
        if(!multiplayer_replay.Encode(bytes,name,u32(std::max(0,session.economy.score_units)),multiplayer_completed()&&!multiplayer_restart_pending)){error="Unable to encode TH11 multiplayer Replay";return false;}
        if(!write_file(path,bytes))return false;
        if(automatic_slot)multiplayer_auto_path=path;
        else if(path==multiplayer_auto_path){
            // A named native save now owns that slot. Later periodic/exit
            // backups must choose another empty path, never rename it PLAYER.
            multiplayer_auto_path.clear();multiplayer_saved=false;
        }
        return true;
    }
    bool multiplayer_tick(){
        multiplayer_advanced=false;
        if(!multiplayer_pump()||!multiplayer_world)return error.empty();
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
        const bool restart=before==GameSessionPhase::paused&&(input[0].held&0x200000u)!=0;
        if(!session.update_multiplayer(input)){error=session.error;return false;}
        const auto after=session.state.phase;
        if(before!=GameSessionPhase::paused&&after==GameSessionPhase::paused&&!stage_music_held())pause_music(true);
        if(before==GameSessionPhase::paused&&after==GameSessionPhase::stage&&!stage_music_held())pause_music(false);
        if(before!=GameSessionPhase::game_over&&after==GameSessionPhase::game_over){
            audio.pause_music(false);if(!request_music(17)){error=audio.error;return false;}
        }
        if(!multiplayer_menu_metadata())return false;
        if(!capture_pause_frame())return false;
        if(!audio_events()||!text_events())return false;
        audio.update();audio.pump();
        frame_statistics.sample(double(SDL_GetTicks())*.001,after==GameSessionPhase::stage&&session.battle&&session.battle->stage_active&&!netplay.ReadOnly());
        frame_text.clear();const auto label=frame_statistics.label();frame_text.add(label.text.c_str(),label.position,label.style);
        if(!session.draw(renderer,&frame_text)){error=session.error;return false;}graphics.present();
        if(!netplay.MarkSimulated(frame,decision)){error=netplay.Error();return false;}
        if(!netplay.ReadOnly()&&!multiplayer_replay.Append(frame,stage,decision.inputs.data(),netplay.Setup().player_count)){error="TH11 confirmed Replay append failed";return false;}
        if(!multiplayer_menu_save())return false;
        multiplayer_advanced=true;multiplayer_last_stage=stage;
        // All consumers retire after the last representable confirmed frame.
        // Never simulate/publish an additional frame which cannot be archived.
        if(restart||after==GameSessionPhase::finished||frame+1>=multiplayer::ReplayArchive::MaxFrames){multiplayer_finish_pending=true;multiplayer_restart_pending=restart&&!netplay.ReadOnly();return multiplayer_pump();}
        return true;
    }
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
        audio.update();audio.pump();
        frame_statistics.sample(double(SDL_GetTicks())*.001,after==GameSessionPhase::stage&&session.battle&&session.battle->stage_active&&!session.state.replay);
        frame_text.clear();const auto label=frame_statistics.label();frame_text.add(label.text.c_str(),label.position,label.style);
        if(!session.draw(renderer,&frame_text)){error=session.error;return false;}
#ifdef TH_ENABLE_THPRAC
        renderer.flush();browser::ThpracUi::render(session,graphics.backend);
#endif
        graphics.present();
        return true;
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
touhou::sdl::FrameCadence cadence;
bool running=false,suspended=false;u32 loop_epoch=0;double previous_frame=-1;
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
    if(app.netplay.ReadOnly()){s.context=3;return s;}
    if(session.state.phase==GameSessionPhase::ending){s.context=app.netplay.Setup().local_player==0?2:3;return s;}
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
    if(session.battle->dialogue&&session.battle->dialogue->active){s.context=2;return s;}
    const auto& player=*session.battle->player;const auto& p=player.motion.state;
    s.context=1;s.instance=int(session.state.stage);s.ready=player.state.life_state==1;
    s.x=p.position.x;s.y=p.position.y;s.fast=float(p.normal_speed)/128;s.slow=float(p.focus_speed)/128;
    s.min_x=-184;s.max_x=184;s.min_y=32;s.max_y=432;return s;
}
void clear_inputs(){
#ifdef TH_ENABLE_THPRAC
    browser::ThpracUi::cancel_pointer();
#endif
    th11_reset_browser_keyboard();SDL_ResetKeyboard();for(auto& k:keyboard_map)k.hosted=false;previous_scans.fill(0);gestures.reset();if(app.session.battle)app.session.battle->player_input.movement.touch_mode=0;
}
void touch_event(unsigned type,int id,float x,float y){
    if(type>2||!std::isfinite(x)||!std::isfinite(y))return;
#ifdef TH_ENABLE_THPRAC
    // Same normalized direct-touch -> native ImGui bridge as TH08/TH10.
    // Both the launcher carrier and canvas-origin SDL fingers enter here.
    if(browser::ThpracUi::captures_game_input()){
        const bool consumed=browser::ThpracUi::captures_pointer(x*640.f,y*480.f);
        browser::ThpracUi::mouse(type==0?1:type==1?0:2,x*640.f,y*480.f);
        if(consumed){gestures.cancel_transient();return;}
    }
#endif
    gestures.pointer(type,id,x,y,SDL_GetTicks(),touch_state(),false);
}
void cancel_touch(){
#ifdef TH_ENABLE_THPRAC
    browser::ThpracUi::cancel_pointer();
#endif
    gestures.cancel_transient();if(app.session.battle)app.session.battle->player_input.movement.touch_mode=0;
}
bool sample_and_tick(){
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
    if(!app.multiplayer_viewer&&!app.netplay.NeedsCapture())return app.multiplayer_tick();
#endif
    bool keys[256]{};std::array<u8,256> scans{};const auto* physical=th11_browser_keyboard()?nullptr:SDL_GetKeyboardState(nullptr);
    for(const auto& k:keyboard_map)if(k.hosted||(physical&&k.native!=SDL_SCANCODE_UNKNOWN&&physical[k.native])){if(k.scan<256)scans[k.scan]=128;if(k.vk<256)keys[k.vk]=true;if(k.vk>=160&&k.vk<=165)keys[16+(k.vk-160)/2]=true;if(k.scan==28||k.scan==156)keys[13]=true;}
    if(app.session.title){auto& title=*app.session.title;for(u32 i=0;i<256;++i)title.key_edges[i]=scans[i]&~previous_scans[i];title.number_keys=0;for(u32 i=0;i<9;++i)if(keys[49+i])title.number_keys|=1u<<i;}
    previous_scans=scans;
    const auto sample=gestures.sample(touch_state(),SDL_GetTicks(),keys[16],keys[37]||keys[38]||keys[39]||keys[40]);
    for(u32 n=0;n<256;++n)keys[n]=keys[n]||sample.keys[n];
#ifdef TH_ENABLE_THPRAC
    browser::ThpracUi::update_input(app.session,keys);
    const bool captured=browser::ThpracUi::captures_game_input();
#else
    const bool captured=false;
#endif
#ifndef TH11_MULTIPLAYER
    if(auto* b=app.session.battle.get()){b->player_input.movement.touch_mode=sample.motion;b->player_input.movement.touch_x=sample.x;b->player_input.movement.touch_y=sample.y;}
#endif
    const u32 raw=sample_controller()|keyboard_keys(keys);
    const u32 held=(raw&~0x80100u)|((raw&0x80000)?256:0);
#ifdef TH11_MULTIPLAYER
    if(app.multiplayer_viewer){
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
       !app.netplay.Capture(app.netplay.NextFrame(),input)){app.error="Unable to capture TH11 multiplayer input";return false;}
    app.multiplayer_pause_requested=false;
    return app.multiplayer_tick();
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
    if(!th11::multiplayer::DecodeSessionSetup(setup,words,count)){app.error="TH11 multiplayer supports measured pure delay only";return 0;}
    app.error.clear();app.multiplayer_replay.Clear();app.multiplayer_world=false;app.multiplayer_viewer=false;
    app.multiplayer_build=setup.build;return app.netplay.Reset(setup)?1:0;
}
EMSCRIPTEN_KEEPALIVE int th11_mp_connect(const char* url){return th11::sdl::app.netplay.Connect(url);}
EMSCRIPTEN_KEEPALIVE int th11_mp_spectator_connect(const char* url,const char* id){return th11::sdl::app.netplay.ConnectSpectator(url,id);}
EMSCRIPTEN_KEEPALIVE int th11_mp_start(){using namespace th11::sdl;clear_inputs();return app.multiplayer_begin();}
EMSCRIPTEN_KEEPALIVE int th11_mp_pump(){return th11::sdl::app.multiplayer_pump();}
EMSCRIPTEN_KEEPALIVE const unsigned* th11_mp_status(){return th11::sdl::app.netplay.Status();}
EMSCRIPTEN_KEEPALIVE const unsigned* th11_mp_calibration_status(){return th11::sdl::app.netplay.CalibrationStatus();}
EMSCRIPTEN_KEEPALIVE const char* th11_mp_error(){auto& app=th11::sdl::app;return app.error.empty()?app.netplay.Error():app.error.c_str();}
EMSCRIPTEN_KEEPALIVE void th11_mp_stop(){using namespace th11::sdl;app.netplay.Clear();app.multiplayer_world=false;app.multiplayer_viewer=false;app.multiplayer_finish_pending=false;clear_inputs();}
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
EMSCRIPTEN_KEEPALIVE int th11_mp_replay_load(const unsigned char* bytes,unsigned size){
    using namespace th11::sdl;if(running)return 0;return app.multiplayer_load_replay(bytes,size);
}
EMSCRIPTEN_KEEPALIVE unsigned th11_mp_replay_size(){
    auto& a=th11::sdl::app;
    return a.multiplayer_replay.Encode(a.multiplayer_export,"PLAYER",unsigned(std::max(0,a.session.economy.score_units)),a.multiplayer_completed()&&!a.multiplayer_restart_pending)?unsigned(a.multiplayer_export.size()):0;
}
EMSCRIPTEN_KEEPALIVE const unsigned char* th11_mp_replay_data(){return th11::sdl::app.multiplayer_export.data();}
EMSCRIPTEN_KEEPALIVE int th11_mp_replay_save(unsigned slot,const char* name){return th11::sdl::app.multiplayer_save(slot,name);}
EMSCRIPTEN_KEEPALIVE int th11_mp_replay_seek(unsigned frame){
    using namespace th11::sdl;if(!app.netplay.Playback()||frame>=app.multiplayer_replay.FrameCount())return 0;
    if(frame<app.netplay.NextFrame()){
        if(!app.netplay.BeginPlayback(app.multiplayer_replay.Setup())||!app.multiplayer_begin())return 0;clear_inputs();
    }
    app.multiplayer_seek_target=frame;return 1;
}
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
EMSCRIPTEN_KEEPALIVE int th11_draw_loading(unsigned frames){return th11::sdl::app.draw_loading(frames);}
EMSCRIPTEN_KEEPALIVE int th11_prepare_loading(){return th11::sdl::app.prepare_loading();}
#if TH11_DEVELOPMENT_HARNESS
#ifdef TH_ENABLE_THPRAC
EMSCRIPTEN_KEEPALIVE int th11_probe_practice_menu(int character,int subtype,int difficulty){auto& a=th11::sdl::app;auto& s=a.session;if(!s.open_title(a.resources,false,th11::TitleScreen::Practice))return 0;s.title->selection.character=character;s.title->selection.partner=subtype;s.title->selection.difficulty=difficulty;return a.tick(0);}
EMSCRIPTEN_KEEPALIVE const double* th11_probe_practice_state(){static double words[20];const auto& s=th11::sdl::app.session;s.practice.run.encode(words);words[14]=s.practice.menu;words[15]=s.practice.active;words[16]=s.economy.power;words[17]=s.practice.cheats;words[18]=s.practice.tracker_bombs;words[19]=s.practice.tracker_misses;return words;}
EMSCRIPTEN_KEEPALIVE const int* th11_probe_title_state(){static int words[4];const auto* t=th11::sdl::app.session.title.get();words[0]=t?int(t->screen):-1;words[1]=t?t->substate:-1;words[2]=t?t->cursor.selected:-1;words[3]=t?t->timer.current:-1;return words;}
EMSCRIPTEN_KEEPALIVE int th11_probe_paused(){return th11::sdl::app.session.state.phase==th11::GameSessionPhase::paused;}
EMSCRIPTEN_KEEPALIVE int th11_probe_function_pose(int side){auto* b=th11::sdl::app.session.battle.get();if(!b||!b->player)return 0;auto& p=b->player->motion.state;p.x=side<0?-0x5c00:side>0?0x5c00:0;p.position.x=float(p.x)/128;p.warp=p.warp_timer=0;return 1;}
EMSCRIPTEN_KEEPALIVE const int* th11_probe_function_state(){static int out[5];const auto* b=th11::sdl::app.session.battle.get();if(!b||!b->player)return out;const auto& p=b->player->motion.state;out[0]=p.x;out[1]=p.warp;out[2]=p.weapon_mode;out[3]=b->player_input.movement.enemies;out[4]=b->player_input.movement.bomb;return out;}
EMSCRIPTEN_KEEPALIVE unsigned th11_probe_gameplay_held(){const auto* b=th11::sdl::app.session.battle.get();return b?b->player_input.movement.held:0;}
EMSCRIPTEN_KEEPALIVE const char* th11_probe_spell_name(unsigned id,unsigned rank){return th11::Localization::SpellName(id,"original",rank);}
EMSCRIPTEN_KEEPALIVE const double* th11_probe_practice_pending(){static double words[th11::PracticeConfig::word_count];th11::sdl::app.session.practice.configured.encode(words);return words;}
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
EMSCRIPTEN_KEEPALIVE int th11_practice_configure(const double* words,unsigned count){th11::PracticeConfig p;if(!p.decode(words,count))return 0;auto& s=th11::sdl::app.session;s.practice.configured=p;s.practice.warp=p.section>=10000?1:p.section?(th11::practice_sections[p.section].spell?5:4):0;if(p.section>0&&p.section<10000&&th11::practice_sections[p.section].appearance>7)s.practice.spell_category=th11::practice_sections[p.section].appearance-7;return 1;}
#endif
EMSCRIPTEN_KEEPALIVE void th11_music_enabled(unsigned on){using namespace th11::sdl;app.audio.music_enabled=on!=0;app.audio.refresh_volume();}
EMSCRIPTEN_KEEPALIVE const unsigned* th11_audio_statistics(){return th11::sdl::app.audio.statistics();}
EMSCRIPTEN_KEEPALIVE void th11_loop_pause(unsigned on){
    using namespace th11::sdl;
#ifdef TH11_MULTIPLAYER
    if(app.netplay.Configured()&&!app.netplay.ReadOnly()){
        if(on&&app.session.state.phase==th11::GameSessionPhase::stage)app.multiplayer_pause_requested=true;
        suspended=false;app.audio.suspend(on!=0);previous_frame=-1;cadence.reset();clear_inputs();app.reset_frame_window();return;
    }
#endif
    suspended=on!=0;app.audio.suspend(suspended);previous_frame=-1;cadence.reset();clear_inputs();app.reset_frame_window();
}
EMSCRIPTEN_KEEPALIVE void th11_loop_stop(){using namespace th11::sdl;running=false;++loop_epoch;previous_frame=-1;cadence.reset();clear_inputs();app.audio.suspend(true);app.reset_frame_window();}
EMSCRIPTEN_KEEPALIVE void th11_audio_close(){th11::sdl::app.audio.close();}
EMSCRIPTEN_KEEPALIVE void th11_loop_start(){
    using namespace th11::sdl;if(running||!app.initialized)return;app.release_loading();
    running=true;suspended=false;previous_frame=-1;cadence.reset();clear_inputs();app.audio.suspend(false);app.reset_frame_window();
    emscripten_request_animation_frame_loop([](double time,void* epoch)->EM_BOOL{
        if(!running||uintptr_t(epoch)!=loop_epoch)return EM_FALSE;
        const double begin=emscripten_get_now(),delta=previous_frame<0?0:(time-previous_frame)/1000.;previous_frame=time;
#ifdef TH11_MULTIPLAYER
        bool ok=app.multiplayer_pump();unsigned advanced=0;
        if(!suspended&&ok){
            const auto ticks=cadence.advance(app.netplay.PacedElapsed(delta));app.graphics.backend.defer=true;
            const bool seeking=app.netplay.Playback()&&app.netplay.NextFrame()<app.multiplayer_seek_target;
            const unsigned budget=seeking?120:ticks;
            for(unsigned n=0;n<budget&&ok;++n){
                if(n&&emscripten_get_now()-begin>=8)break;
                ok=sample_and_tick();advanced+=unsigned(app.multiplayer_advanced);
                if(!app.multiplayer_advanced)break;
                if(seeking&&app.netplay.NextFrame()>=app.multiplayer_seek_target)break;
            }
            app.graphics.backend.commit();app.graphics.backend.defer=false;
        }else cadence.reset();
        // A missing input never blocks WebRTC callbacks, confirmation repair
        // or audio pumping, and never calls a simulation-bearing Draw twice.
        app.audio.pump();th11_browser_frame(ok?1:0,emscripten_get_now()-begin,advanced);
        if(!ok)running=false;return running?EM_TRUE:EM_FALSE;
#else
        if(suspended){cadence.reset();return EM_TRUE;}
        const auto ticks=cadence.advance(delta);bool ok=true;app.graphics.backend.defer=true;
        for(unsigned n=0;n<ticks&&ok;++n)ok=sample_and_tick();
        app.graphics.backend.commit();app.graphics.backend.defer=false;
        app.audio.pump();
        if(ticks)th11_browser_frame(ok?1:0,emscripten_get_now()-begin,ticks);
        if(!ok)running=false;return running?EM_TRUE:EM_FALSE;
#endif
    },reinterpret_cast<void*>(uintptr_t(++loop_epoch)));
}
EMSCRIPTEN_KEEPALIVE void th11_touch(unsigned type,int id,float x,float y){th11::sdl::touch_event(type,id,x,y);}
EMSCRIPTEN_KEEPALIVE void th11_touch_cancel(){th11::sdl::cancel_touch();}
EMSCRIPTEN_KEEPALIVE void th11_touch_options(unsigned enabled,unsigned mode,float sensitivity,unsigned two_finger,unsigned double_tap){using namespace th11::sdl;gestures.enabled=enabled!=0;gestures.unlimited=mode==1;gestures.sensitivity=std::isfinite(sensitivity)?std::clamp(sensitivity,1.f,3.f):1;gestures.two_finger=two_finger!=0;gestures.double_tap=double_tap!=0;if(gestures.set_mode(mode<=3?int(mode):0))if(app.session.battle)app.session.battle->player_input.movement.touch_mode=0;if(!enabled)gestures.cancel();}
EMSCRIPTEN_KEEPALIVE void th11_touch_controls(unsigned enabled,unsigned fire,unsigned focus,unsigned bomb,unsigned escape){using namespace th11::sdl;gestures.enabled=enabled!=0;gestures.controls(fire!=0,focus!=0,bomb,escape,0,0);}
EMSCRIPTEN_KEEPALIVE void th11_touch_stick(float x,float y){using namespace th11::sdl;gestures.stick_x=std::isfinite(x)?std::clamp(x/32767.f,-1.f,1.f):0;gestures.stick_y=std::isfinite(y)?std::clamp(y/32767.f,-1.f,1.f):0;}
// Shared-host ABI (mirrors th10/th20 cpp/sdl/GameHost.cpp). The portable
// eagler-host.mjs drives touch through these names; game entry points above
// stay TH11-specific for the development harness.
__attribute__((export_name("sdl_touch"))) void sdl_touch(unsigned type,int id,float x,float y){th11_touch(type,id,x,y);}
__attribute__((export_name("sdl_touch_cancel"))) void sdl_touch_cancel(){th11_touch_cancel();}
__attribute__((export_name("sdl_touch_options"))) void sdl_touch_options(unsigned on,unsigned free,float speed){using namespace th11::sdl;gestures.enabled=on!=0;gestures.unlimited=free!=0;gestures.sensitivity=std::isfinite(speed)?std::clamp(speed,1.f,3.f):1;if(!on)gestures.cancel();}
__attribute__((export_name("sdl_touch_gestures"))) void sdl_touch_gestures(unsigned two,unsigned taps){using namespace th11::sdl;gestures.two_finger=two!=0;gestures.double_tap=taps!=0;}
__attribute__((export_name("sdl_touch_mode"))) void sdl_touch_mode(unsigned mode){using namespace th11::sdl;if(gestures.set_mode(mode<=3?int(mode):0))if(app.session.battle)app.session.battle->player_input.movement.touch_mode=0;}
__attribute__((export_name("sdl_touch_controls"))) void sdl_touch_controls(unsigned shoot,unsigned slow,unsigned bomb,unsigned escape,float x,float y){using namespace th11::sdl;gestures.controls(shoot!=0,slow!=0,bomb,escape,x,y);}
__attribute__((export_name("sdl_loop_pause"))) void sdl_loop_pause(unsigned on){th11_loop_pause(on);}
__attribute__((export_name("sdl_music_resource_changed"))) void sdl_music_resource_changed(){}
}
