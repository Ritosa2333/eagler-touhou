#include <eagler/netplay/AdonisTiming.hpp>
#include <eagler/netplay/NetplayCore.hpp>
#include <cassert>
#include <cstdio>
#include <limits>

using namespace Netplay;
static void lockstep(unsigned delay) {
    CoreConfig c; c.sessionId=901;c.inputDelay=delay;c.allowPrediction=false;
    RollbackCore core;assert(core.Reset(c));
    for(unsigned frame=0;frame<600;++frame) {
        assert(core.ScheduleLocalInput(frame,FrameInput(1u+(frame%8))));
        assert(!core.PrepareFrame(frame).canAdvance);
        // An exact future slot may not punch through a missing prefix.
        assert(core.SubmitRemoteInput(1,frame+1,FrameInput((frame+1)%16))!=RemoteInputResult::RollbackRequired);
        assert(!core.PrepareFrame(frame+1).canAdvance);
        auto result=core.SubmitRemoteInput(1,frame,FrameInput(frame%16));
        assert(result==RemoteInputResult::Accepted || result==RemoteInputResult::Duplicate);
        auto d=core.PrepareFrame(frame);assert(d.canAdvance && d.predictedMask==0);
        assert(d.inputs[0]==(frame<delay?FrameInput{}:FrameInput(1u+((frame-delay)%8))));
        auto forged=d;forged.predictedMask=2;assert(!core.MarkSimulated(frame,forged));
        forged=d;forged.inputs[1].buttons^=1;assert(!core.MarkSimulated(frame,forged));
        assert(core.MarkSimulated(frame,d));assert(!core.HasRollbackRequest());
        assert(!core.RewindSimulationTo(frame));
        std::array<FrameInput,MAX_PLAYERS> actual;
        assert(core.ConfirmedInputs(frame,&actual) && actual==d.inputs);
        // Remove the future insertion expectation on the next iteration: its
        // already present actual input is allowed, but never its successor.
        if(frame+1<600) {
            ++frame;
            assert(core.ScheduleLocalInput(frame,FrameInput(1u+(frame%8))));
            d=core.PrepareFrame(frame);assert(d.canAdvance && !d.predictedMask);
            assert(core.MarkSimulated(frame,d));
        }
    }
}
static void phase() {
    AdonisPhase p;assert(p.Reset(9,0,2));
    unsigned windows=0;
    for(unsigned frame=0;frame<192;++frame) {
        const std::uint64_t due=100000+frame*16667ull;
        p.ObserveArrival(1,frame,due-6000);
        p.ObserveArrival(1,frame,due+99000); // retransmit cannot replace arrival
        p.ObserveDue(frame,due);p.ObserveDue(frame,due+99000); // retry/resim ignored
        AdonisPhaseSample s;
        if(p.PollLocalSample(&s)) {
            ++windows;assert(s.waitCount==0 && s.leadCount==16 && s.leadUs==96000);
            std::vector<std::uint8_t> bytes;assert(EncodeAdonisPhaseSample(s,&bytes));
            AdonisPhaseSample decoded;assert(DecodeAdonisPhaseSample(bytes.data(),bytes.size(),&decoded));
            assert(decoded.frame==s.frame && decoded.leadUs==s.leadUs);
            assert(!DecodeAdonisPhaseSample(bytes.data(),bytes.size()-1,&decoded));
            auto remote=s;remote.senderPlayer=1;remote.targetPlayer=0;remote.leadUs=16*15000;
            assert(p.ReceiveSample(remote));assert(p.ReceiveSample(remote));
            const auto delay=p.TakeDelayMs();
            assert(delay==0 || delay==4.5);assert(p.TakeDelayMs()==0);
        }
    }
    assert(windows==12 && p.Adjustments()==3 && p.TotalDelayUs()==13500);
    AdonisPhaseSample wrong;wrong.sessionId=90;wrong.frame=192;
    wrong.senderPlayer=1;wrong.targetPlayer=0;wrong.leadCount=16;
    assert(!p.ReceiveSample(wrong));

    // Four ms is a strict deadband. Waiting uses first arrival minus due,
    // not the callback's later completion time (nor repeated attempts).
    for(unsigned kind=0;kind<3;++kind) {
        assert(p.Reset(10,0,2));
        for(unsigned frame=0;frame<80;++frame) {
            const auto due=100000+frame*16667ull;
            p.ObserveDue(frame,due);
            p.ObserveArrival(1,frame,kind?due+5000:due-6000);
            AdonisPhaseSample s;
            if(p.PollLocalSample(&s)) {
                auto remote=s;remote.senderPlayer=1;remote.targetPlayer=0;
                remote.waitCount=0;remote.waitUs=0;remote.leadCount=16;remote.leadUs=160000;
                if(kind!=2)assert(p.ReceiveSample(remote)); // positive wait needs no remote report
            }
        }
        assert(p.TakeDelayMs()==(kind?5:0));
        assert(p.Adjustments()==(kind?1u:0u));
    }
    assert(!p.Reset(0,0,2));assert(!p.Reset(1,2,2));
    assert(p.Reset(11,0,3));
    for(unsigned f=0;f<16;++f){p.ObserveDue(f,100000+f*16667);p.ObserveArrival(1,f,99000+f*16667);p.ObserveArrival(2,f,98000+f*16667);}
    AdonisPhaseSample a,b;assert(p.PollLocalSample(&a)&&p.PollLocalSample(&b));
    assert(a.targetPlayer!=b.targetPlayer && !p.PollLocalSample(&a));
}
int main() {
    for(auto delay:{0u,1u,3u,9u})lockstep(delay);
    phase();
    assert(RecommendAdonisDelay({})==1);
    assert(RecommendAdonisDelay({0,-1,std::numeric_limits<double>::infinity()})==1);
    assert(RecommendAdonisDelay({77,77,77,77})==3);
    assert(RecommendAdonisDelay({1000})==9);
    assert(AdonisGameplayAbi(123,AdonisMode::Rollback,0)==123);
    assert(AdonisGameplayAbi(123,AdonisMode::Delay,3)!=AdonisGameplayAbi(123,AdonisMode::Hybrid,3));
    // Existing title ABIs already salt D into the low bits. Do not cancel it
    // by XOR-ing that same low-bit representation a second time.
    for(unsigned a=0;a<10;++a)for(unsigned b=a+1;b<10;++b)
        assert(AdonisGameplayAbi(123^(a?(0x49444c00u^a):0),AdonisMode::Delay,a)!=
               AdonisGameplayAbi(123^(b?(0x49444c00u^b):0),AdonisMode::Delay,b));
    std::puts("Adonis exact-input gate, delay mapping, phase/deadband/cooldown, codec, 3P and RTT recommendation PASS");
}
