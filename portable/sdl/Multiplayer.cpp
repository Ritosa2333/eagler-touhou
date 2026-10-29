// Multiplayer application scenes. Transport admission, rollback and confirmed
// replay recording remain owned by the existing C++ Session.
#include "Game.hpp"
#include "BinaryReader.hpp"
#ifdef TH12_MULTIPLAYER
#include "game/GameState.hpp"
#include "game/Multiplayer.hpp"
#include "game/StageFlow.hpp"
#include "multiplayer/Session.hpp"
#include "wasm/Api.hpp"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
namespace th12host {
bool Game::multiplayer_stage(int stage,int character,int shot,int difficulty,int story_character,int story_shot){
    if(!multiplayer_ready()||stage<1||stage>7||character<0||character>2||shot<0||shot>1||difficulty<0||difficulty>4)return false;
    mp_story_character=story_character;mp_story_shot=story_shot;
    if(!session){Session n;n.run={difficulty==4?ModeExtra:ModeStory,difficulty,character,shot,stage,{}};n.stage=stage;session=std::move(n);}
    auto& s=*session;s.stage=stage;s.run.stage=stage;s.hud_age=0;s.previous=0;s.cursor=0;
    s.high_score=std::max(s.high_score,double(profile.rankings(character,shot,difficulty)[0].score()));
    menus->run=s.run;menus->enter(MenuScene::Playing);menus->drain();
    phase=Phase::Loading;held_frame=mp_playing;show_indicator();audio.stop_effects();audio.pause_music(true);mp_audio_muted=true;
    if(prefetch&&prefetch->next.stage==stage&&prefetch->done&&prefetch->next.error.empty()){
        StageFiles next=std::move(prefetch->next);prefetch.reset();install_stage(next);
    }else{prefetch.reset();queue_stage(stage);}
    // Keep completion asynchronous even when the next stage was prefetched.
    jobs.push_back([]{return true;});
    after_jobs=[this,stage]{
        char context[32];std::snprintf(context,sizeof context,"st%02d_0%d%c.msg",stage,mp_story_character,mp_story_shot?'b':'a');translation_context(context);
        if(!load_core_stage(stage))return;
        scene->reset_presentation();captured=false;held_frame=false;phase=Phase::Menu;retire_indicator();clear_keys();
        play_music(stage==1?"th12_00":stage==2?"th12_04":stage==3?"th12_07":stage==4?"th12_09":stage==5?"th12_13":stage==6?"th12_16":"th12_18");
        audio.pause_music(true);
        session->music_serial=th12_music_serial();
    };
    return true;
}
void Game::multiplayer_start(bool playback,int local_seat,bool visible){
    mp_playing=true;mp_result=false;mp_replay=playback;mp_local=local_seat;mp_visible=visible;
    scene->multiplayer_view(local_seat,visible);
    mp_restores=th12::multiplayer::session.restores();ending.reset();pending_result.reset();
    menus->enter(MenuScene::Playing);menus->drain();captured=false;
}
void Game::multiplayer_finish(bool save_only){
    if(!mp_playing||mp_result)return;
    if(th12::multiplayer::session.network.Spectator())mp_replay=false;
    mp_result=true;audio.stop_effects();captured=false;
    const auto status=th12::fullGameFlow.status;
    if(status==th12::FullGameStatus::Ending&&!mp_replay){begin_ending();return;}
    complete_run(status==th12::FullGameStatus::Result||status==th12::FullGameStatus::Ending,save_only);
}
void Game::multiplayer_room(){
    if(!menus)return;
    jobs.clear();after_jobs=nullptr;phase=Phase::Menu;
    pending_replay.clear();seek_target=-1;external_paused=false;external_rate=1;rate_debt=0;
    mp_playing=mp_result=mp_replay=mp_audio_muted=false;return_title();menus->return_title();menus->drain();
    play_music("th12_01");
#ifdef __EMSCRIPTEN__
    EM_ASM({Module.th12MultiplayerRoom?.();});
#endif
}
void Game::multiplayer_frame(const PadSample& pad,u32 ticks,double now){
    using namespace th12::multiplayer;
    const u32 ui=(map_buttons(pad)|pad.keyboard|pad.pressed);
    for(u32 n=0;n<ticks&&mp_playing&&!mp_result&&phase==Phase::Menu;++n){
        const auto before_frame=th12::frame;const auto before_stage=th12::stageNumber;
        const auto before_restores=th12::multiplayer::session.restores();
        int playback_audio=0;
#ifdef __EMSCRIPTEN__
        // Browser boundary handles replay controls and transport diagnostics;
        // its step calls the existing C++ Session (including corrections).
        playback_audio=EM_ASM_INT({return Module.th12MultiplayerTick?.($0,$1,$2,$3,$4)??2;},ui,motion_mode,motion_x,motion_y,now);
#else
        if(!th12::multiplayer::session.step((ui&0x2ff)|((ui&InEnter)?0x1000:0)|((ui&InRetry)?0x2000:0),ui&InPause,motion_mode,motion_x/(motion_mode==3?32767.f:128.f),motion_y/(motion_mode==3?32767.f:128.f),now)){error=th12::multiplayer::session.error();return;}
        for(const auto& event:th12::multiplayer::session.audio())audio.effect(event.id,event.pan,event.stop);
        th12::multiplayer::session.clearAudio();
#endif
        if(!mp_playing||mp_result||phase!=Phase::Menu)break;
        if(session){session->stage=th12::stageNumber;session->hud_age=int(th12::frame);session->cursor=th12::multiplayer::session.network.NextFrame();}
        if(th12::multiplayer::session.restores()!=mp_restores){mp_restores=th12::multiplayer::session.restores();scene->reset_motion();}
        const auto serial=th12_music_serial();if(session&&serial!=session->music_serial){session->music_serial=serial;audio.play_music(th12_music_track());}
        const auto& network=th12::multiplayer::session.network;
        if(network.Spectator()){
            const auto& playback=network.StagePlayback();
            if(!playback.Ready()||playback.Seeking())playback_audio=2;
            else if(playback.Paused())playback_audio=1;
        }else if(!network.CanStart()||network.NextFrame()==0)playback_audio=2;
        const bool mute_audio=paused||playback_audio!=0;
        if(playback_audio==2)audio.stop_effects();
        else if(mute_audio&&!mp_audio_muted)audio.pause_effects();
        audio.pause_music(mute_audio);
        if(!mute_audio&&mp_audio_muted)audio.resume_effects();
        mp_audio_muted=mute_audio;
        const bool was_paused=menus->state.scene==MenuScene::Pause||menus->state.scene==MenuScene::PauseConfirm;
        if(paused&&!network.Spectator()){
            const auto kind=menuConfirm?MenuScene::PauseConfirm:MenuScene::Pause;
            auto& st=menus->state;if(st.scene!=kind)menus->enter(kind,int(menuSelection),[](MenuState& s){s.ready_at=0;s.phase=MenuState::Phase::Active;});
            if(st.selection!=int(menuSelection)){MenuEvent e;e.type=MenuEvent::Selection;e.previous=st.selection;e.selection=int(menuSelection);st.events.push_back(e);}
            ++st.age;st.selection=int(menuSelection);st.pause_selection=int(menuAction);st.pause_action=menuAction==0?"resume":menuAction==1?"title":menuAction==2?"save-exit":"retry";
        }else if(was_paused){menus->enter(MenuScene::Playing);}
        if(title)title->tick(menus->state,*menus);
        menus->drain();tick_loading();
        // Waiting input and paused menu ticks do not change the world. A
        // correction can finish at the same frame, so its restore serial also
        // invalidates the cached scene (including interpolation history).
        if(th12::frame!=before_frame||th12::stageNumber!=before_stage||
           th12::multiplayer::session.restores()!=before_restores)captured=false;
    }
    // Extra presentation callbacks reuse the captured world. Scene::render
    // updates the live FPS text independently, including a stalled world's 0.

}
}
#endif
