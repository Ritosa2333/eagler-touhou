#include "RollbackDriver.hpp"
#include "../platform/BrowserRuntime.hpp"
#include "../platform/PlatformDevices.hpp"
#include <algorithm>

namespace th08::multiplayer {
RollbackDriver::RollbackDriver(BrowserRuntime& r):runtime(r),network(r.app.session.netplay){}
const char* RollbackDriver::Error()const{return failed?error:network.Error();}
bool RollbackDriver::Connect(const char* relay){return !failed&&network.Connect(relay);}
bool RollbackDriver::Pump(){
    const auto& a=runtime.app;
    return !failed&&(network.Pump(a.session.multiplayer_session.started&&!a.loading_game())||Fail("network channel failed"));
}
bool RollbackDriver::Reconcile(){
    if(failed||open||!Pump())return false;
    if(!initialized||!runtime.app.session.netplay.CanStart())return true;
    return Correct()&&Commit();
}
bool RollbackDriver::PresentCorrection(){
    if(!corrected_present_pending)return true;
    corrected_present_pending=false;
    return graphics_device().present(runtime.back)||Fail("corrected presentation failed");
}
bool RollbackDriver::Stable()const{
    const auto& a=runtime.app;
    return a.in_game()&&!a.loading_game()&&!a.invalid()&&a.supervisor.state.active==i32(th08::Scene::Game)&&
        a.supervisor.state.target==i32(th08::Scene::Game)&&a.game.menus.context.supervisor_state==i32(th08::Scene::Game)&&
        !a.supervisor.state.close_requested&&!(a.game.globals.game_flags&8);
}
bool RollbackDriver::apply_file_event(const char* path,const u8* data,u32 size){
    return runtime.resources_.put(path,data,size)&&file_device().save(path,data,size);
}
bool RollbackDriver::Commit(){
    auto& net=runtime.app.session.netplay;
    if(net.Correcting()||net.RollbackFrame()!=Netplay::INVALID_FRAME)return true;
    const auto through=net.ConfirmedThrough(),last=net.LastFrame();
    if(!runtime.commit_audio_events(audio,through,last)||!files.CommitThrough(through,last,*this))return Fail("confirmed external output failed");
    if(through!=Netplay::INVALID_FRAME&&last!=Netplay::INVALID_FRAME&&bound){
        const auto next=std::min(through,last)+1;world.DiscardBefore(next);textures.DiscardBefore(next);
    }
    return true;
}
bool RollbackDriver::Admit(){
    auto& a=runtime.app;auto& net=a.session.netplay;
    if(bound&&!world.CanAdvance()){
        // Do not execute the next scene's resource destructors until the frame
        // that selected that scene is reconciled and confirmed.
        if(!net.CanRetire()){a.session.network_waiting=true;return true;}
        if(!Commit())return false;
        world.Clear();textures.Clear();bound=false;
        if(!net.SetWorldReady(false))return Fail("world retirement failed");
    }
    const auto target=th08::Scene(a.supervisor.state.target);
    if(!generation_transition_pending&&a.supervisor.state.active==i32(th08::Scene::Game)&&
       (target==th08::Scene::Restart||target==th08::Scene::SpellRestart)){
        if(!network.CanRetire()){a.session.network_waiting=true;return true;}
        if(!Commit()||!network.Retire())return Fail("generation retirement failed");
        SessionSetup next;
        if(!net.BeginNextRun(next,a.session.random.seed)||!network.BeginGeneration())return Fail("generation bootstrap failed");
        next.started=true;a.session.multiplayer_session=next;
        a.session.random={u16(next.seed),u16(next.seed),0};
        audio.Reset();files.Reset();generation=net.Generation();generation_transition_pending=true;
        a.session.network_waiting=true;return true;
    }
    if(!bound&&Stable()){
        if(!world.Bind(runtime)||!textures.Bind(runtime)||!net.SetWorldReady(true))return Fail("production owner bootstrap failed");
        bound=true;
    }
    return true;
}
bool RollbackDriver::RunFrame(bool render){
    auto& a=runtime.app;auto& session=a.session;auto& net=session.netplay;
    const auto frame=net.NextFrame();const auto decision=net.Prepare(frame);
    if(!decision.canAdvance){session.network_waiting=true;return true;}
    if(bound&&(!world.BeginFrame(frame)||!textures.BeginFrame(frame)))return Fail("begin world frame failed");
    if(!audio.BeginFrame(frame)||!files.BeginFrame(frame))return Fail("begin output frame failed");
    open=true;session.network_frame=decision;session.network_frame_open=true;
    if(a.in_game()){
        u16 buttons[3]{};for(u32 seat=0;seat<session.player_count;++seat)buttons[seat]=decision.inputs[seat].buttons;
        if(!a.game.commit_inputs(buttons,session.player_count))return Fail("committed input handoff failed");
    }
    if(decision.predictedMask)++predicted;
    if(!a.update()||runtime.capture_failed)return Fail("native update failed");
    if(generation_transition_pending&&a.supervisor.state.target==i32(th08::Scene::Game))generation_transition_pending=false;
    return !render||(a.draw()&&FinishFrame());
}
bool RollbackDriver::FinishFrame(){
    if(failed||!open)return false;
    auto& session=runtime.app.session;auto& net=session.netplay;const auto frame=net.NextFrame();
    if((bound&&(!world.EndFrame()||!textures.EndFrame()))||!audio.EndFrame()||!files.EndFrame())return Fail("end frame ownership failed");
    if(!net.MarkSimulated(frame,session.network_frame))return Fail("simulated input mismatch");
    if(bound)max_bytes=std::max(max_bytes,u32(world.BytesForFrame(frame)+textures.BytesForFrame(frame)));
    open=false;session.network_frame_open=false;++runtime.multiplayer_logic_frame;
    if(!correcting)corrected_present_pending=false;
    return correcting||Commit();
}
bool RollbackDriver::Correct(){
    auto& net=runtime.app.session.netplay;const auto first=net.RollbackFrame();
    if(first==Netplay::INVALID_FRAME)return true;
    const auto end=net.NextFrame();
    if(!bound||!net.BeginCorrection(first)||!world.UndoTo(first)||!textures.UndoTo(first)||
       !audio.DiscardFrom(first)||!files.DiscardFrom(first))return Fail("rollback restore failed");
    runtime.correction_present_suppressed=true;correcting=true;++corrections;
    while(net.NextFrame()<end&&world.CanAdvance()){
        const auto before=net.NextFrame();
        if(!RunFrame(true)||net.NextFrame()==before){correcting=false;runtime.correction_present_suppressed=false;return Fail("resimulation stalled");}
        ++resimulated;
    }
    const bool boundary=net.NextFrame()!=end;
    if(!net.EndCorrection(boundary)){correcting=false;runtime.correction_present_suppressed=false;return Fail("correction frontier failed");}
    correcting=false;runtime.correction_present_suppressed=false;
    corrected_present_pending=true;
    return Commit();
}
bool RollbackDriver::Step(bool render){
    auto& session=runtime.app.session;auto& net=session.netplay;
    if(failed||open||session.network_frame_open||!runtime.prepared||runtime.capture_failed)return false;
    session.network_waiting=false;
    if(!Pump())return false;
    if(!net.CanStart()){session.network_waiting=true;return true;}
    if(!initialized){audio.Reset(net.NextFrame());files.Reset(net.NextFrame());
        if(!runtime.bind_audio_events(&audio))return Fail("audio sink binding failed");
        initialized=true;generation=net.Generation();}
    if(!Correct()||!Commit()||!Admit())return false;
    if(session.network_waiting)return PresentCorrection();
    const auto frame=net.NextFrame();
    if(!net.HasLocal(frame)){
        const auto touch=u16(file_device().supplemental_input());
        const auto physical=runtime.input.controller(InputController::keyboard(runtime.keys,false)|touch,runtime.pad,session.display_config);
        if(!net.CaptureLocal(frame,Netplay::FrameInput(physical)))return Fail("local capture failed");
    }
    if(!network.Captured(frame))return Fail("captured input send failed");
    const bool success=RunFrame(render);
    return success&&(!session.network_waiting||PresentCorrection());
}
void RollbackDriver::Shutdown(){
    if(!initialized&&!bound&&!network.Enabled())return;
    runtime.correction_present_suppressed=false;
    // Shutdown may arrive while the displayed state is still predicted. Never
    // serialize it as a completed personal record.
    runtime.discard_network_shutdown_writes=true;
    world.Clear();textures.Clear();bound=false;
    runtime.bind_audio_events(nullptr);audio.Reset();files.Reset();network.Close();
    runtime.app.session.network_frame_open=false;open=initialized=false;
}
}
