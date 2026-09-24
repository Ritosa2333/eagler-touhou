#pragma once
#ifndef TH_MULTIPLAYER_FIXTURES
#error Native correction diagnostics must not enter production
#endif
#include "../../th08_web/cpp/multiplayer/WorldJournal.hpp"
#include "../../th08_web/cpp/multiplayer/AudioEvents.hpp"
#include "../../th08_web/cpp/platform/BrowserRuntime.hpp"
#include <cstdio>

namespace th08::multiplayer::fixture {
// Admit a diagnostic session at an already loaded stage. Both branches execute
// BrowserRuntime -> NetplayRuntime -> GameplayScene. Startup/transport and
// confirmed device output are separate acceptance boundaries.
inline const u32* correction_probe(BrowserRuntime& runtime){
    static u32 result[64]{};std::fill(result,result+64,0);result[0]=1;
    auto& session=runtime.app.session;auto& net=session.netplay;
    if(net.Configured()||!runtime.app.in_game()||runtime.app.loading_game()){result[2]=1;return result;}
    u8 keyboard[256]{};std::memcpy(keyboard,runtime.keyboard_state(),256);
    struct Cleanup {
        BrowserRuntime& runtime;const u8* keyboard;
        ~Cleanup(){auto& s=runtime.app.session;s.netplay.Clear();s.network_frame={};
            s.network_frame_open=s.network_waiting=false;std::memcpy(runtime.keyboard_state(),keyboard,256);}
    } cleanup{runtime,keyboard};
    WorldJournal journal;
    const auto fail=[&](u32 step){result[2]=step;result[36]=runtime.status(4);result[37]=journal.OwnerFaults();
        std::printf("native correction failed step=%u native=%u journal=%u reason=%s\n",step,result[36],result[37],journal.Error());return result;};
    if(!journal.Bind(runtime))return fail(2);
    if(!journal.DiagnosticInputSampler())return fail(4);
    result[38]=1;
    AudioEvents audio;audio.Reset();
    if(!runtime.bind_audio_events(&audio))return fail(5);
    struct AudioCleanup {BrowserRuntime& runtime;~AudioCleanup(){runtime.bind_audio_events(nullptr);}} audio_cleanup{runtime};
    constexpr u32 frames=NetplayRuntime::MaxRollbackFrames;
    static_assert(frames==WorldJournal::History);
    auto setup=session.multiplayer_session;setup.started=false;setup.session_id=0x800020260924ull;
    const auto wire=[&](const auto& packet){std::vector<u8> data;
        if constexpr(std::is_same_v<std::decay_t<decltype(packet)>,Netplay::SessionPacket>){
            if(!Netplay::EncodeSessionPacket(packet,&data))return false;
        }else if(!Netplay::EncodeInputPacket(packet,&data))return false;
        return net.ApplyWire(data.data(),data.size())==NetplayRuntime::WireResult::Accepted;
    };
    const auto barrier=[&](){
        net.Clear();if(!net.Reset(setup))return false;
        std::array<std::unique_ptr<NetplayRuntime>,3> remote;
        std::array<NetplayRuntime*,3> peers{};peers[setup.local_player]=&net;
        for(u32 seat=0;seat<setup.player_count;++seat)if(seat!=setup.local_player){
            auto config=setup;config.local_player=seat;remote[seat]=std::make_unique<NetplayRuntime>();
            if(!remote[seat]->Reset(config))return false;peers[seat]=remote[seat].get();
        }
        for(u32 receiver=0;receiver<setup.player_count;++receiver)
            for(u32 sender=0;sender<setup.player_count;++sender)if(receiver!=sender){
                const auto hello=peers[sender]->SessionPacket(Netplay::SessionPhase::Hello);
                if(receiver==setup.local_player){if(!wire(hello))return false;}
                else if(peers[receiver]->ApplySession(hello)!=Netplay::SessionPacketResult::Accepted)return false;
            }
        for(u32 seat=0;seat<setup.player_count;++seat)if(!peers[seat]->MarkReady())return false;
        for(u32 seat=0;seat<setup.player_count;++seat)if(seat!=setup.local_player)
            if(!wire(peers[seat]->SessionPacket(Netplay::SessionPhase::Ready)))return false;
        return net.CanStart()&&net.SetWorldReady(true);
    };
    const auto input=[](u32 seat,u32 frame){return Netplay::FrameInput(u16(InputButton::Shoot|
        ((seat+frame)%2?InputButton::Left:InputButton::Right)|
        (frame<4?InputButton::Focus:0)|(frame==1?InputButton::Bomb:0)));};
    const auto deliver=[&](u32 seat,u32 frame){
        Netplay::InputPacket packet;packet.sessionId=setup.session_id;packet.senderPlayer=u8(seat);
        packet.playerCount=u8(setup.player_count);packet.sequence=1+frame;
        packet.firstInputFrame=packet.latestFrame=frame;packet.inputCount=1;packet.inputs[0]=input(seat,frame);
        return wire(packet);
    };
    const auto step=[&](u32 frame,bool capture){
        if(capture&&!net.CaptureLocal(frame,input(setup.local_player,frame)))return false;
        // Independent low-level oracle: production Step owns different
        // journals and must not bind them over this fixture's existing owner.
        const auto decision=net.Prepare(frame);
        if(!decision.canAdvance||!journal.BeginFrame(frame)||!audio.BeginFrame(frame))return false;
        session.network_frame=decision;session.network_frame_open=true;
        u16 buttons[3]{};for(u32 seat=0;seat<setup.player_count;++seat)buttons[seat]=decision.inputs[seat].buttons;
        return runtime.app.game.commit_inputs(buttons,setup.player_count)&&runtime.app.update()&&
            runtime.app.draw()&&runtime.finish_network_frame()&&!session.network_frame_open&&
            net.NextFrame()==frame+1&&journal.EndFrame()&&audio.EndFrame();
    };
    if(!barrier())return fail(3);
    const auto initial=journal.AuditHash();
    std::array<std::array<u32,WorldJournal::GroupCount>,frames> expected{};
    std::array<std::vector<u32>,frames> blocks;
    std::array<u32,frames> expected_audio{};
    for(u32 frame=0;frame<frames;++frame){
        for(u32 seat=0;seat<setup.player_count;++seat)if(seat!=setup.local_player&&!deliver(seat,frame))return fail(10+frame);
        if(!step(frame,true))return fail(20+frame);
        expected[frame]=journal.AuditHash();blocks[frame]=journal.BlockHashes();expected_audio[frame]=audio.FrameDigest(frame);
    }
    auto reference_audio=audio;
    struct ReferenceOutput:AudioEventOutput {bool apply_audio_event(const AudioEvent&)override{return true;}} reference;
    if(!reference_audio.CommitThrough(frames-1,frames-1,reference))return fail(31);
    result[41]=reference_audio.CommittedEvents();result[43]=reference_audio.CommittedDigest();
    if(!journal.UndoTo(0)||journal.AuditHash()!=initial||!barrier()||!audio.DiscardFrom(0))return fail(30);
    for(u32 frame=0;frame<frames;++frame){
        if(!net.CaptureLocal(frame,input(setup.local_player,frame)))return fail(40+frame);
        const auto decision=net.Prepare(frame);
        if(!decision.canAdvance||!decision.predictedMask)return fail(50+frame);
        ++result[4];if(!step(frame,false))return fail(60+frame);
        result[40]+=audio.FrameDigest(frame)!=expected_audio[frame];
    }
    // Low-level admission must reject the ninth frame. The production driver's
    // actual Update/Draw stall is tested independently in check-admission.py;
    // do not bind that driver over this fixture's already-open journal owner.
    if(!net.CaptureLocal(frames,input(setup.local_player,frames)))return fail(70);
    const auto stalled=journal.AuditHash();
    if(net.Prepare(frames).canAdvance||net.NextFrame()!=frames||journal.AuditHash()!=stalled)return fail(71);
    result[33]=1;
    // Reverse-order packets and duplicates travel through the production
    // decoder. The fixture never writes an InputLane or FrameDecision.
    for(u32 frame=frames;frame-->0;)for(u32 seat=0;seat<setup.player_count;++seat)if(seat!=setup.local_player)
        if(!deliver(seat,frame)||!deliver(seat,frame))return fail(72);
    std::array<Netplay::FrameInput,Netplay::MAX_PLAYERS> confirmed{};
    if(!result[40]||audio.CommittedEvents()||audio.NextCommit())return fail(75);
    if(runtime.commit_audio_events(audio,net.ConfirmedThrough(),net.LastFrame())||audio.CommittedEvents())return fail(76);
    result[39]=1;
    if(net.RollbackFrame()!=0||net.ConfirmedInputs(0,confirmed)||!net.BeginCorrection(0)||!journal.UndoTo(0)||!audio.DiscardFrom(0))return fail(73);
    std::memset(runtime.keyboard_state(),128,256);
    result[6]=InputController::keyboard(runtime.keyboard_state(),false);
    if(!(result[6]&InputButton::Menu))return fail(74);
    for(u32 frame=0;frame<frames;++frame){
        if(net.CaptureLocal(frame,Netplay::FrameInput(0))||!step(frame,false))return fail(80+frame);
        const auto actual=journal.AuditHash();
        if(actual!=expected[frame]){
            for(u32 group=0;group<WorldJournal::GroupCount;++group){result[8+group]=expected[frame][group];result[20+group]=actual[group];}
            const auto current=journal.BlockHashes();
            for(u32 index=0;index<current.size();++index)if(current[index]!=blocks[frame][index]){
                result[5]=index+1;std::printf("native correction frame=%u first block=%s\n",frame,journal.BlockName(index));break;}
            return fail(90+frame);
        }
        if(audio.FrameDigest(frame)!=expected_audio[frame])return fail(110+frame);
    }
    result[32]=result[35]=1;
    if(!net.EndCorrection()||!net.ConfirmedInputs(frames-1,confirmed)||!net.CanRetire())return fail(100);
    for(u32 seat=0;seat<setup.player_count;++seat)if(confirmed[seat]!=input(seat,frames-1))return fail(101);
    if(!runtime.commit_audio_events(audio,net.ConfirmedThrough(),net.LastFrame())||audio.NextCommit()!=frames||audio.PendingFrames())return fail(120);
    result[42]=audio.CommittedEvents();result[44]=audio.CommittedDigest();
    if(result[41]<=frames*2||result[41]!=result[42]||result[43]!=result[44])return fail(121);
    if(!runtime.commit_audio_events(audio,net.ConfirmedThrough(),net.LastFrame())||audio.CommittedEvents()!=result[42])return fail(122);
    result[45]=1;
    if(!net.SetWorldReady(false))return fail(102);
    result[34]=1;result[3]=frames;result[1]=1;return result;
}
}
