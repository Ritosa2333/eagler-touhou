#include "../th08_web/cpp/multiplayer/ReplayArchive.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
using namespace th08::multiplayer;
using Netplay::FrameInput;
static ReplayDescription description(unsigned count,unsigned local){
    ReplayDescription d;const std::uint32_t words[]{2,count,local,1,1234,71,0,0,0,1,0,count==3?2u:0u,0};
    assert(decode_session_setup(d.setup,words,13));std::memcpy(d.name,"Test",4);return d;
}
static void barrier(NetplayRuntime* peers,unsigned count){
    for(unsigned i=0;i<count;++i)for(unsigned j=0;j<count;++j)if(i!=j)
        assert(peers[i].ApplySession(peers[j].SessionPacket(Netplay::SessionPhase::Hello))==Netplay::SessionPacketResult::Accepted);
    for(unsigned i=0;i<count;++i)assert(peers[i].MarkReady());
    for(unsigned i=0;i<count;++i)for(unsigned j=0;j<count;++j)if(i!=j)
        assert(peers[i].ApplySession(peers[j].SessionPacket(Netplay::SessionPhase::Ready))==Netplay::SessionPacketResult::Accepted);
}
static std::uint32_t checksum(const std::vector<std::uint8_t>& bytes){
    std::uint32_t h=2166136261u;for(std::size_t i=0;i<bytes.size();++i)if(i<20||i>=24){h^=bytes[i];h*=16777619u;}return h;
}
static void repair(std::vector<std::uint8_t>& bytes){const auto h=checksum(bytes);for(unsigned i=0;i<4;++i)bytes[20+i]=std::uint8_t(h>>(8*i));}
static bool save(void* count,const char*,const std::uint8_t* data,std::uint32_t size){
    ReplayArchive copy;assert(copy.Load(data,size));++*static_cast<unsigned*>(count);return true;
}
static void roundtrip(unsigned count,unsigned local){
    auto d=description(count,local);ReplayArchive archive;const std::vector<std::uint8_t> boot{1,2,3};
    assert(archive.Begin(d,boot));NetplayRuntime peers[3];
    for(unsigned i=0;i<count;++i)assert(peers[i].Reset(description(count,i).setup));barrier(peers,count);
    auto& net=peers[local];unsigned saves=0;
    for(unsigned frame=0;frame<6;++frame){
        assert(archive.BeginFrame(frame));
        assert(net.CaptureLocal(frame,FrameInput(1)));
        for(unsigned i=0;i<count;++i)if(i!=local)assert(net.SubmitRemote(i,frame,FrameInput(4))==Netplay::RemoteInputResult::Accepted);
        assert(net.MarkSimulated(frame,net.Prepare(frame)));
        if(frame==2)assert(archive.RequestSave(frame,1,"Unit","09/25"));
        assert(archive.Stamp(frame,frame<3?0:1,frame*100));assert(archive.Commit(net,save,&saves));
    }
    assert(saves==1&&archive.Cursor()==6&&archive.StageFrame(1)==3);
    std::vector<std::uint8_t> bytes;assert(archive.Encode(bytes));ReplayArchive loaded;
    assert(loaded.Load(bytes.data(),bytes.size()));assert(loaded.BootScore()==boot);
    assert(loaded.Description().setup.local_player==local&&loaded.StageFrame(1)==3);
    NetplayRuntime player;assert(player.Reset(loaded.Description().setup)&&player.BeginPlayback());
    for(unsigned f=0;f<6;++f){const auto* inputs=loaded.PlaybackFrame(f);assert(inputs);
        assert(player.FeedPlayback(f,*inputs)&&!player.Prepare(f).predictedMask);
        assert(!player.CaptureLocal(f,FrameInput(0))&&!player.MarkReady());
        assert(player.MarkSimulated(f,player.Prepare(f))&&loaded.AdvancePlayback(f,f<3?0:1));
    }
    assert(loaded.Complete());
    assert(player.CanRetire()&&player.Retire());SessionSetup next;
    assert(player.BeginNextRun(next,200)&&player.Playback()&&player.CanStart());
    auto junk=bytes;junk.back()^=1;assert(!loaded.Load(junk.data(),junk.size())&&loaded.Complete());
    for(unsigned offset:{8u,12u,16u}){junk=bytes;std::memset(junk.data()+offset,255,4);repair(junk);assert(!loaded.Load(junk.data(),junk.size()));}
    junk=bytes;junk[24+offsetof(th08::GameConfiguration,lives)]=255;repair(junk);assert(!loaded.Load(junk.data(),junk.size()));
    for(std::size_t size:{std::size_t(0),std::size_t(23),bytes.size()-1})assert(!loaded.Load(bytes.data(),size));
}
static void corrected_frames_replace_speculative_save_requests(){
    auto d=description(2,0);ReplayArchive archive;assert(archive.Begin(d,{}));
    NetplayRuntime peers[2];for(unsigned seat=0;seat<2;++seat)assert(peers[seat].Reset(description(2,seat).setup));
    barrier(peers,2);auto& net=peers[0];assert(net.SetWorldReady(true));unsigned saves=0;
    for(unsigned f=0;f<3;++f){
        assert(net.CaptureLocal(f,FrameInput(1)));
        if(f==0)assert(net.SubmitRemote(1,f,FrameInput(0))==Netplay::RemoteInputResult::Accepted);
        assert(archive.BeginFrame(f));
        if(f==1)assert(archive.RequestSave(f,1,"Phantom","09/25"));
        const auto decision=net.Prepare(f);assert(decision.canAdvance);
        assert(net.MarkSimulated(f,decision)&&archive.Stamp(f,0,10*f));
        assert(archive.Commit(net,save,&saves));
    }
    assert(archive.Cursor()==1&&saves==0);
    assert(net.SubmitRemote(1,1,FrameInput(64))==Netplay::RemoteInputResult::RollbackRequired);
    assert(net.SubmitRemote(1,2,FrameInput(128))==Netplay::RemoteInputResult::RollbackRequired);
    assert(archive.Commit(net,save,&saves)&&archive.Cursor()==1);
    assert(net.BeginCorrection(1));
    for(unsigned f=1;f<3;++f){
        assert(archive.BeginFrame(f));
        if(f==2)assert(archive.RequestSave(f,2,"Correct","09/25"));
        assert(net.MarkSimulated(f,net.Prepare(f))&&archive.Stamp(f,0,100*f));
    }
    assert(net.EndCorrection()&&archive.Commit(net,save,&saves));
    assert(archive.Cursor()==3&&saves==1&&archive.Description().score==200);
    std::vector<std::uint8_t> bytes;assert(archive.Encode(bytes));ReplayArchive played;
    assert(played.Load(bytes.data(),bytes.size()));
    assert(played.AdvancePlayback(0,0));assert(played.PlaybackFrame(1)->at(1).buttons==64);
    assert(played.AdvancePlayback(1,0));assert(played.PlaybackFrame(2)->at(1).buttons==128);
}
int main(){for(unsigned count:{2u,3u})for(unsigned local=0;local<count;++local)roundtrip(count,local);
 corrected_frames_replace_speculative_save_requests();
 std::puts("TH08 confirmed all-seat Replay, boot files, readonly playback, bounds and atomic rejection: PASS");}
