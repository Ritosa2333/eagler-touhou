#include "../th10_web/cpp/multiplayer/ReplayArchive.hpp"
#include <cassert>
#include <cstring>
#include <cstdio>

using namespace th10::multiplayer;
using namespace Netplay;

static void handshake(NetplayRuntime& a,NetplayRuntime& b){
    a.ApplySession(b.Hello());b.ApplySession(a.Hello());
    assert(a.CanSendReady()&&b.CanSendReady());a.MarkLocalReady();b.MarkLocalReady();
    a.ApplySession(b.Ready());b.ApplySession(a.Ready());assert(a.CanStart()&&b.CanStart());
}
static std::vector<std::uint8_t> rewrite_description(const std::vector<std::uint8_t>& source,
                                                      std::vector<std::uint8_t> description){
    InputReplayInfo info;assert(InputReplay::Inspect(source.data(),source.size(),&info));
    info.config.description=std::move(description);InputReplay rewritten;
    assert(rewritten.Begin(info.config));ReplayArchive frames;
    assert(frames.Load(source.data(),source.size()));th10::u32 chapter=0;
    for(th10::u32 frame=0;frame<info.frameCount;++frame){
        while(chapter+1<info.chapterCount&&info.chapters[chapter+1].firstFrame<=frame)++chapter;
        const auto label=info.chapters[chapter].label;
        const auto* row=frames.PlaybackFrame(frame,label&255u);assert(row);
        assert(rewritten.Append(frame,label,row->data(),info.config.playerCount));
        assert(frames.Played(frame));
    }
    std::vector<std::uint8_t> bytes;assert(rewritten.Encode(&bytes));return bytes;
}
static void check_terminal_stage(){
    SessionSetup setup;const std::uint32_t words[]{1,2,1,4,1234,0,0,1,1,0,0};
    assert(DecodeSessionSetup(setup,words,11));
    th10::ApplicationConfig options{};th10::u16 keys[9]{};options.initialize(keys);
    ReplayArchive tape;assert(tape.Begin(setup,options));
    NetplayRuntime runtime,peer;setup.sessionId=77;assert(runtime.Reset(setup,77));
    auto other=setup;other.localPlayer=0;assert(peer.Reset(other,77));handshake(runtime,peer);
    // Native Extra rank registration changes game.stage from 7 to 8 while
    // Results is still ticking. Preserve that authored marker in the tape;
    // it is not an eighth selectable gameplay stage.
    for(unsigned frame=0;frame<3;++frame){
        assert(runtime.CaptureLocal(frame,FrameInput(0)));
        assert(runtime.SubmitRemote(0,frame,FrameInput(0))==RemoteInputResult::Accepted);
        assert(tape.Stamp(frame,frame?8:7));
        assert(runtime.MarkSimulated(frame,runtime.Prepare(frame)));
        assert(tape.Commit(runtime));
    }
    auto description=tape.Description();description.lastStage=8;
    assert(tape.RequestSave("replay/th10_01.rpy","CLEAR",2,500,0,8));
    std::vector<std::uint8_t> bytes;assert(tape.Encode(bytes,&description));
    ReplayArchive restored;assert(restored.Load(bytes.data(),bytes.size()));
    assert(restored.Description().lastStage==8);
    assert(restored.SeekFrame(7)==0&&restored.SeekFrame(8)==INVALID_FRAME);
    assert(restored.PlaybackFrame(0,7)&&restored.Played(0));
    assert(restored.PlaybackFrame(1,8)&&restored.Played(1));
    assert(restored.PlaybackFrame(2,8)&&restored.Played(2)&&restored.Complete());
}
static void check_seek_after_generation(){
    SessionSetup setup;const std::uint32_t words[]{1,2,1,0,4321,0,0,1,1,0,0};
    assert(DecodeSessionSetup(setup,words,11));
    th10::ApplicationConfig options{};th10::u16 keys[9]{};options.initialize(keys);
    ReplayArchive tape;assert(tape.Begin(setup,options));
    NetplayRuntime runtime,peer;setup.sessionId=100;assert(runtime.Reset(setup,100));
    auto other=setup;other.localPlayer=0;assert(peer.Reset(other,100));handshake(runtime,peer);
    for(unsigned frame=0;frame<3;++frame){
        assert(runtime.CaptureLocal(frame,FrameInput(4)));
        assert(runtime.SubmitRemote(0,frame,FrameInput(1))==RemoteInputResult::Accepted);
        assert(tape.Stamp(frame,1));assert(runtime.MarkSimulated(frame,runtime.Prepare(frame)));assert(tape.Commit(runtime));
    }
    assert(runtime.RetireRun());assert(runtime.BeginNextRun(setup,101));assert(tape.NextGeneration(1));
    other=setup;other.localPlayer=0;assert(peer.Reset(other,setup.sessionId));handshake(runtime,peer);
    for(unsigned frame=0;frame<4;++frame){
        assert(runtime.CaptureLocal(frame,FrameInput(8)));
        assert(runtime.SubmitRemote(0,frame,FrameInput(2))==RemoteInputResult::Accepted);
        assert(tape.Stamp(frame,frame<2?1:2));assert(runtime.MarkSimulated(frame,runtime.Prepare(frame)));assert(tape.Commit(runtime));
    }
    std::vector<std::uint8_t> bytes;assert(tape.Encode(bytes));ReplayArchive restored;
    assert(restored.Load(bytes.data(),bytes.size()));
    // Native Replay UI has one entry per stage, not per generation. Repeated
    // Stage 1 therefore resolves to the first occurrence, while a later stage
    // that exists only after Retry resolves to that later generation boundary.
    assert(restored.SeekFrame(1)==0);assert(restored.SeekFrame(2)==5);
    assert(restored.Info().chapterCount==3);
    assert(restored.Info().chapters[0].label==1&&restored.Info().chapters[0].firstFrame==0);
    assert(restored.Info().chapters[1].label==257&&restored.Info().chapters[1].firstFrame==3);
    assert(restored.Info().chapters[2].label==258&&restored.Info().chapters[2].firstFrame==5);
}
static ReplayCheckpoint checkpoint(unsigned stage,unsigned frame,unsigned players,unsigned seed){
    ReplayCheckpoint cp{};cp.label=stage;cp.firstFrame=frame;
    cp.scriptRandom={static_cast<th10::u16>(seed),0,0};
    cp.visualRandom={static_cast<th10::u16>(seed+7),0,13};
    cp.activationScriptRandom={static_cast<th10::u16>(seed+14),0,17};
    cp.activationVisualRandom={static_cast<th10::u16>(seed+21),0,29};
    cp.activationRandomValid=true;
    cp.faithCursor=static_cast<th10::u32>(120+stage);cp.laserLastId=456+stage;
    cp.reservedStage=stage==7?7:1;
    cp.retainedStateValid=true;
    cp.cooperation.seatCount=static_cast<th10::u8>(players);
    for(unsigned seat=0;seat<players;++seat){
        cp.inputSeats[seat].raw=static_cast<th10::u16>(0x10u<<seat);
        auto& cooperation=cp.cooperation.seats[seat];
        cooperation.lives=static_cast<std::int16_t>(2+seat);
        cooperation.power=static_cast<std::int16_t>(stage==1?0:60+seat*10);
        cooperation.character=static_cast<th10::u8>(seat&1);
        cooperation.shot=static_cast<th10::u8>(seat);
        cooperation.rescueTarget=-1;
        auto& snap=cp.pilots[seat];snap.initialize();
        snap.stage=static_cast<std::int16_t>(stage);snap.seed=static_cast<th10::u16>(seed);
        snap.lives=cooperation.lives;snap.power=cooperation.power;
        snap.score=static_cast<th10::i32>(frame*100+seat);
        snap.item_value=5000+static_cast<th10::i32>(frame);
        snap.faith=600+static_cast<th10::i32>(frame);snap.rank=17;snap.score_units=3;snap.extend_index=1;
        snap.position={static_cast<th10::i32>(seat*100),38000};
        cp.reservedPower[seat]=static_cast<th10::u16>(seat);
    }
    return cp;
}
static void check_stage_checkpoint_codec(){
    SessionSetup setup;const std::uint32_t words[]{1,2,1,0,222,0,0,1,1,0,0};
    assert(DecodeSessionSetup(setup,words,11));
    th10::ApplicationConfig options{};th10::u16 keys[9]{};options.initialize(keys);
    ReplayArchive tape;assert(tape.Begin(setup,options));
    NetplayRuntime runtime,peer;setup.sessionId=501;assert(runtime.Reset(setup,501));
    auto other=setup;other.localPlayer=0;assert(peer.Reset(other,501));handshake(runtime,peer);
    const auto first=checkpoint(1,0,2,222);
    assert(runtime.CaptureLocal(0,Netplay::FrameInput(4)));
    assert(runtime.SubmitRemote(0,0,Netplay::FrameInput(1))==RemoteInputResult::Accepted);
    assert(tape.Stamp(0,1));assert(runtime.MarkSimulated(0,runtime.Prepare(0)));assert(tape.Commit(runtime));
    auto transient=first;
    transient.cooperation.seats[0].rescueTicks=24;
    transient.cooperation.seats[0].rescueTarget=-4;
    assert(!tape.CaptureCheckpoint(transient));
    assert(tape.CaptureCheckpoint(first));
    assert(runtime.CaptureLocal(1,Netplay::FrameInput(8)));
    assert(runtime.SubmitRemote(0,1,Netplay::FrameInput(2))==RemoteInputResult::Accepted);
    assert(tape.Stamp(1,2));assert(runtime.MarkSimulated(1,runtime.Prepare(1)));assert(tape.Commit(runtime));
    const auto second=checkpoint(2,2,2,333);
    assert(runtime.CaptureLocal(2,Netplay::FrameInput(16)));
    assert(runtime.SubmitRemote(0,2,Netplay::FrameInput(4))==RemoteInputResult::Accepted);
    assert(tape.Stamp(2,2));assert(runtime.MarkSimulated(2,runtime.Prepare(2)));assert(tape.Commit(runtime));
    assert(tape.CaptureCheckpoint(second));
    std::vector<std::uint8_t> bytes;assert(tape.Encode(bytes));
    ReplayArchive restored;assert(restored.Load(bytes.data(),bytes.size()));
    const auto* loaded=restored.Checkpoint(2);assert(loaded);
    assert(restored.SeekFrame(2)==2&&restored.Info().chapters[1].firstFrame==1);
    assert(loaded->firstFrame==2&&loaded->scriptRandom.seed==333&&loaded->visualRandom.calls==13);
    assert(loaded->activationScriptRandom.seed==347&&loaded->activationVisualRandom.calls==29);
    assert(loaded->inputSeats[1].raw==0x20&&loaded->pilots[1].position.x==100);
    assert(loaded->retainedStateValid&&loaded->faithCursor==122&&loaded->laserLastId==458);
    assert(loaded->reservedStage==1);
    assert(restored.SelectCheckpoint(2)&&restored.Base()==2&&restored.Cursor()==2);
    assert(restored.SelectedCheckpoint()==loaded);
    const auto* row=restored.PlaybackFrame(0,2);assert(row&&(*row)[0].buttons==4&&(*row)[1].buttons==16);
    std::vector<std::uint8_t> prefix;assert(tape.Encode(prefix,nullptr,1));
    ReplayArchive truncated;assert(truncated.Load(prefix.data(),prefix.size()));
    assert(truncated.Checkpoint(1)&&!truncated.Checkpoint(2)&&!truncated.SelectCheckpoint(2));

    // Malformed external stage snapshots must fail import before native restore
    // can index score sprites or extend thresholds with an invalid value.
    Netplay::InputReplayInfo inspected;ReplayDescription metadata;
    assert(ReplayArchive::Inspect(bytes.data(),bytes.size(),inspected,metadata));
    const auto read_word=[](const std::vector<std::uint8_t>& data,std::size_t at){
        return std::uint32_t(data[at])|(std::uint32_t(data[at+1])<<8)|
            (std::uint32_t(data[at+2])<<16)|(std::uint32_t(data[at+3])<<24);
    };
    const auto write_word=[](std::vector<std::uint8_t>& data,std::size_t at,std::uint32_t value){
        for(unsigned byte=0;byte<4;++byte)data[at+byte]=std::uint8_t(value>>(byte*8));
    };
    const auto write_half=[](std::vector<std::uint8_t>& data,std::size_t at,std::uint16_t value){
        data[at]=std::uint8_t(value);data[at+1]=std::uint8_t(value>>8);
    };
    const auto& v4Description=inspected.config.description;
    const auto checkpointCount=read_word(v4Description,120),v4RecordBytes=read_word(v4Description,124);
    assert(checkpointCount==2&&read_word(v4Description,0)==4&&v4RecordBytes>40);
    const std::size_t firstRecord=128;
    const auto cooperationStart=firstRecord+40+MAX_PLAYERS*sizeof(th10::GameInput);
    const auto pilotStart=cooperationStart+34+MAX_PLAYERS*2+4;
    const auto reject_description=[&](const auto& mutate){
        auto description=v4Description;mutate(description);
        const auto malformed=rewrite_description(bytes,std::move(description));
        ReplayArchive rejected;assert(!rejected.Load(malformed.data(),malformed.size()));
    };
    reject_description([&](auto& description){
        write_word(description,pilotStart+offsetof(th10::ReplayStage,score_units),10);
    });
    reject_description([&](auto& description){
        write_word(description,pilotStart+offsetof(th10::ReplayStage,extend_index),5);
    });
    reject_description([&](auto& description){
        write_word(description,pilotStart+offsetof(th10::ReplayStage,focused),2);
    });
    reject_description([&](auto& description){description[cooperationStart+8]=1;});
    reject_description([&](auto& description){write_half(description,cooperationStart+4,3);});
    reject_description([&](auto& description){
        write_word(description,firstRecord+v4RecordBytes-12,2048);
    });
    reject_description([&](auto& description){
        write_word(description,firstRecord+v4RecordBytes-4,0);
    });

    // Version 3 has activation RNG but no retained manager state. It remains
    // readable and re-encodes as v3, while Stage selection falls back to the
    // chapter start. Version 2 also keeps its original record layout.
    const auto v3RecordBytes=v4RecordBytes-12;
    std::vector<std::uint8_t> v3Description(v4Description.begin(),v4Description.begin()+128);
    write_word(v3Description,0,3);write_word(v3Description,124,v3RecordBytes);
    for(std::uint32_t i=0;i<checkpointCount;++i){
        const auto start=std::size_t(128)+std::size_t(i)*v4RecordBytes;
        v3Description.insert(v3Description.end(),v4Description.begin()+start,
                             v4Description.begin()+start+v3RecordBytes);
    }
    auto v3Bytes=rewrite_description(bytes,std::move(v3Description));
    ReplayArchive v3;assert(v3.Load(v3Bytes.data(),v3Bytes.size()));
    assert(v3.Checkpoint(2)&&v3.Checkpoint(2)->activationRandomValid&&!v3.Checkpoint(2)->retainedStateValid);
    assert(!v3.SelectCheckpoint(2)&&v3.SeekFrame(2)==v3.Info().chapters[1].firstFrame);
    std::vector<std::uint8_t> v3Roundtrip;assert(v3.Encode(v3Roundtrip));
    Netplay::InputReplayInfo v3RoundtripInfo;ReplayDescription v3Metadata;
    assert(ReplayArchive::Inspect(v3Roundtrip.data(),v3Roundtrip.size(),v3RoundtripInfo,v3Metadata));
    assert(read_word(v3RoundtripInfo.config.description,0)==3);

    std::vector<std::uint8_t> v2Description(v4Description.begin(),v4Description.begin()+128);
    write_word(v2Description,0,2);write_word(v2Description,124,v4RecordBytes-28);
    for(std::uint32_t i=0;i<checkpointCount;++i){
        const auto start=std::size_t(128)+std::size_t(i)*v4RecordBytes;
        v2Description.insert(v2Description.end(),v4Description.begin()+start,v4Description.begin()+start+24);
        v2Description.insert(v2Description.end(),v4Description.begin()+start+40,
                             v4Description.begin()+start+v4RecordBytes-12);
    }
    auto legacyBytes=rewrite_description(bytes,std::move(v2Description));
    ReplayArchive legacy;assert(legacy.Load(legacyBytes.data(),legacyBytes.size()));
    assert(legacy.Checkpoint(1)&&!legacy.Checkpoint(1)->activationRandomValid);
    assert(!legacy.SelectCheckpoint(1)&&legacy.Base()==0&&legacy.Cursor()==0);
    assert(legacy.SeekFrame(2)==legacy.Info().chapters[1].firstFrame);
    std::vector<std::uint8_t> legacyRoundtrip;assert(legacy.Encode(legacyRoundtrip));
    Netplay::InputReplayInfo legacyRoundtripInfo;ReplayDescription legacyMetadata;
    assert(ReplayArchive::Inspect(legacyRoundtrip.data(),legacyRoundtrip.size(),
                                  legacyRoundtripInfo,legacyMetadata));
    assert(read_word(legacyRoundtripInfo.config.description,0)==2);
    ReplayArchive legacyReloaded;assert(legacyReloaded.Load(legacyRoundtrip.data(),legacyRoundtrip.size()));
    assert(legacyReloaded.Checkpoint(1)&&!legacyReloaded.Checkpoint(1)->activationRandomValid);
}
static void check_extra_checkpoint_extend_limit(){
    SessionSetup setup;const std::uint32_t words[]{1,2,1,4,654,0,0,1,1,0,0};
    assert(DecodeSessionSetup(setup,words,11));
    th10::ApplicationConfig options{};th10::u16 keys[9]{};options.initialize(keys);
    ReplayArchive tape;assert(tape.Begin(setup,options));
    NetplayRuntime runtime,peer;setup.sessionId=654;assert(runtime.Reset(setup,654));
    auto other=setup;other.localPlayer=0;assert(peer.Reset(other,654));handshake(runtime,peer);
    assert(runtime.CaptureLocal(0,FrameInput(4)));
    assert(runtime.SubmitRemote(0,0,FrameInput(1))==RemoteInputResult::Accepted);
    assert(tape.Stamp(0,7));assert(runtime.MarkSimulated(0,runtime.Prepare(0)));assert(tape.Commit(runtime));
    auto valid=checkpoint(7,0,2,654);
    for(auto& pilot:valid.pilots)pilot.extend_index=2;
    assert(tape.CaptureCheckpoint(valid));
    auto invalid=valid;invalid.pilots[0].extend_index=3;
    assert(!tape.CaptureCheckpoint(invalid));
    invalid=valid;invalid.reservedStage=6;
    assert(!tape.CaptureCheckpoint(invalid));
    std::vector<std::uint8_t> bytes;assert(tape.Encode(bytes));
    Netplay::InputReplayInfo inspected;ReplayDescription metadata;
    assert(ReplayArchive::Inspect(bytes.data(),bytes.size(),inspected,metadata));
    auto description=inspected.config.description;
    const auto recordBytes=std::size_t(description[124])|(std::size_t(description[125])<<8)|
        (std::size_t(description[126])<<16)|(std::size_t(description[127])<<24);
    const auto coop=std::size_t(128)+40+MAX_PLAYERS*sizeof(th10::GameInput);
    const auto pilots=coop+34+MAX_PLAYERS*2+4;
    const auto at=pilots+offsetof(th10::ReplayStage,extend_index);
    description[at]=3;description[at+1]=description[at+2]=description[at+3]=0;
    assert(recordBytes>at-128);
    const auto malformed=rewrite_description(bytes,std::move(description));
    ReplayArchive rejected;assert(!rejected.Load(malformed.data(),malformed.size()));
}
static void check_measured_policy(){
    for(unsigned players:{2u,3u})for(unsigned mode:{1u,2u}){
        const std::uint32_t words[]{4,players,0,1,1234,77,0,0,0,0,1,1,
            0,players==3?2u:0u,mode,1,2,1,2,3,4};
        SessionSetup setup;assert(DecodeSessionSetup(setup,words,21));
        setup.input_delay=1;setup.measured_prediction=mode==2?2:0;
        th10::ApplicationConfig options{};th10::u16 keys[9]{};options.initialize(keys);
        ReplayArchive tape;assert(tape.Begin(setup,options));
        std::array<NetplayRuntime,3> peers;
        for(unsigned seat=0;seat<players;++seat){auto peer=setup;peer.localPlayer=seat;assert(peers[seat].Reset(peer,77));}
        for(unsigned seat=0;seat<players;++seat){
            for(unsigned other=0;other<players;++other){
                if(other!=seat)peers[seat].ApplySession(peers[other].Hello());
            }
            peers[seat].MarkLocalReady();
        }
        auto& runtime=peers[0];
        for(unsigned seat=1;seat<players;++seat)runtime.ApplySession(peers[seat].Ready());
        assert(runtime.CanStart());
        for(unsigned seat=1;seat<players;++seat)assert(runtime.SubmitRemote(seat,0,FrameInput{})==RemoteInputResult::Accepted);
        for(unsigned frame=0;frame<4;++frame){
            assert(runtime.CaptureLocal(frame,FrameInput(4)));
            for(unsigned seat=1;seat<players;++seat)assert(runtime.SubmitRemote(seat,frame+1,FrameInput(1u<<seat))==RemoteInputResult::Accepted);
            assert(tape.Stamp(frame,1));assert(runtime.MarkSimulated(frame,runtime.Prepare(frame)));assert(tape.Commit(runtime));
        }
        std::vector<std::uint8_t> bytes;assert(tape.Encode(bytes));ReplayArchive playback;
        assert(playback.Load(bytes.data(),bytes.size()));auto restored=playback.Description().setup;
        assert(restored.version==4&&restored.adonis_mode==mode&&restored.input_delay==1&&restored.input_delay_auto);
        assert(restored.measured_prediction==setup.measured_prediction&&!std::memcmp(restored.build,setup.build,sizeof(setup.build)));
        NetplayRuntime viewer;assert(viewer.BeginPlayback(restored)&&viewer.Setup().input_delay==0&&!viewer.AllowsRollback());
        for(unsigned frame=0;frame<4;++frame){const auto* row=playback.PlaybackFrame(frame,1);assert(row);
            assert(viewer.FeedPlayback(frame,row->data(),players));auto decision=viewer.Prepare(frame);
            assert(decision.canAdvance&&!decision.predictedMask&&viewer.MarkSimulated(frame,decision));assert(playback.Played(frame));}
        auto description=playback.Info().config.description;description[144]=3;
        auto malformed=rewrite_description(bytes,std::move(description));ReplayArchive rejected;
        assert(!rejected.Load(malformed.data(),malformed.size()));
    }
}
int main(){
    check_measured_policy();
    SessionSetup setup;const std::uint32_t words[]{1,2,1,4,1234,0,0,1,1,0,0};
    assert(DecodeSessionSetup(setup,words,11));
    th10::ApplicationConfig options{};th10::u16 keys[9]{};options.initialize(keys);
    ReplayArchive tape;assert(tape.Begin(setup,options));
    NetplayRuntime runtime,peer;setup.sessionId=42;assert(runtime.Reset(setup,42));
    auto other=setup;other.localPlayer=0;assert(peer.Reset(other,42));handshake(runtime,peer);
    for(unsigned i=0;i<5;++i){
        assert(runtime.CaptureLocal(i,FrameInput(4)));
        if(i<2)assert(runtime.SubmitRemote(0,i,FrameInput(1))==RemoteInputResult::Accepted);
        assert(tape.Stamp(i,7));assert(runtime.MarkSimulated(i,runtime.Prepare(i)));
        assert(tape.Commit(runtime));assert(tape.Frames()==(i<2?i+1:2));
    }
    assert(runtime.SubmitRemote(0,2,FrameInput(2))==RemoteInputResult::RollbackRequired);
    assert(runtime.RewindSimulationTo(2));
    for(unsigned i=2;i<5;++i){
        if(i>2)assert(runtime.SubmitRemote(0,i,FrameInput(2))==RemoteInputResult::Accepted);
        assert(tape.Stamp(i,7));assert(runtime.MarkSimulated(i,runtime.Prepare(i)));
    }
    assert(tape.Commit(runtime)&&tape.Frames()==5);
    assert(runtime.RetireRun());assert(runtime.BeginNextRun(setup,54321));
    assert(tape.NextGeneration(1));assert(!tape.NextGeneration(1));
    other=setup;other.localPlayer=0;assert(peer.Reset(other,setup.sessionId));handshake(runtime,peer);
    for(unsigned i=0;i<3;++i){
        assert(runtime.CaptureLocal(i,FrameInput(16)));
        assert(runtime.SubmitRemote(0,i,FrameInput(32))==RemoteInputResult::Accepted);
        assert(tape.Stamp(i,7));assert(runtime.MarkSimulated(i,runtime.Prepare(i)));assert(tape.Commit(runtime));
    }
    assert(tape.Frames()==8&&tape.Base()==5);
    std::vector<std::uint8_t> bytes;assert(tape.Encode(bytes));
    ReplayArchive playback;assert(playback.Load(bytes.data(),bytes.size()));
    assert(playback.Description().setup.seed==1234&&playback.SeekFrame(7)==0);
    assert(playback.Info().chapters[1].label==263&&playback.Info().chapters[1].firstFrame==5);
    auto restored=playback.Description().setup;NetplayRuntime player;
    assert(player.BeginPlayback(restored)&&player.Playback()&&player.CanStart());
    assert(!player.Connect("ws://127.0.0.1/"));
    assert(!player.CaptureLocal(0,FrameInput(999)));
    assert(player.SubmitRemote(0,0,FrameInput(999))==RemoteInputResult::InvalidPlayer);
    for(unsigned i=0;i<5;++i){
        const auto* row=playback.PlaybackFrame(i,7);assert(row);
        assert((*row)[0].buttons==(i<2?1:2)&&(*row)[1].buttons==4);
        assert(player.FeedPlayback(i,row->data(),2));assert(player.MarkSimulated(i,player.Prepare(i)));
        assert(playback.Played(i));
    }
    assert(player.RetireRun());assert(player.BeginNextRun(restored,54321));assert(player.CanStart());
    assert(playback.NextGeneration(1));
    for(unsigned i=0;i<3;++i){
        const auto* row=playback.PlaybackFrame(i,7);assert(row);
        assert((*row)[0].buttons==32&&(*row)[1].buttons==16);
        assert(player.FeedPlayback(i,row->data(),2));assert(player.MarkSimulated(i,player.Prepare(i)));
        assert(playback.Played(i));
    }
    assert(playback.Complete()&&!playback.PlaybackFrame(3,7));
    auto original=bytes;bytes[8]=7;assert(!playback.Load(bytes.data(),bytes.size()));
    assert(playback.Complete());assert(playback.Encode(bytes)&&bytes==original);
    for(const auto* path:{"../replay/th10_01.rpy","replay/../../a.rpy","replay/a/b.rpy","replay/a\\b.rpy","replay/a:r.rpy","replay/a.rpyx"})
        assert(!ReplayArchive::SafePath(path));
    assert(ReplayArchive::SafePath("replay/th10_01.rpy"));
    assert(tape.RequestSave("replay/th10_01.rpy","TEST",2,100,0,7));
    const auto request=tape.save;assert(!tape.RequestSave("../../bad.rpy","TEST",2,100,0,7));
    assert(std::memcmp(&request,&tape.save,sizeof(request))==0);
    check_terminal_stage();
    check_seek_after_generation();
    check_stage_checkpoint_codec();
    check_extra_checkpoint_extend_limit();
    std::puts("TH10 Replay archive, corrected input and generation continuity: PASS");
}
