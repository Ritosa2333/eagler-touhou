#include "../portable/sdl/FrameCadence.hpp"
#include <eagler/netplay/SessionPacing.hpp>
#include <eagler/netplay/PendingFrameSchedule.hpp>
#include <eagler/netplay/InputGapPacing.hpp>
#include <eagler/netplay/NetplayCore.hpp>
#include "../th10_web/cpp/multiplayer/NetworkInputGapPacing.hpp"
#include "../th10_web/cpp/multiplayer/NetworkFrameSchedule.hpp"
#include "../th10_web/cpp/multiplayer/RecoveryBudget.hpp"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <limits>
namespace {
void near(double actual,double expected){
    assert(std::abs(actual-expected)<1.e-10);
}

void test_recovery_budget(){
    using Budget=th10::multiplayer::RecoveryBudget;
    constexpr auto cap=Netplay::FrameBudget::MaxCatchupTicks;
    constexpr auto ordinary=Netplay::FrameBudget::CatchupBudgetNs;
    constexpr auto tail=Budget::LastTickBudgetNs;
    constexpr auto overdue=std::numeric_limits<std::uint64_t>::max();
    static_assert(cap==6&&ordinary==8'000'000&&tail==12'000'000);
    static_assert(!Budget::CanStartTick(0,0,0));
    static_assert(Budget::CanStartTick(0,1,overdue));
    static_assert(!Budget::CanStartTick(cap,1,0));

    // Empty history never starts work. An overdue callback with pending
    // history can still complete one transaction, regardless of its depth.
    for(std::uint32_t completed:{0u,1u,cap-1,cap})
        for(std::uint64_t elapsed:{std::uint64_t{0},ordinary,tail,overdue})
            assert(!Budget::CanStartTick(completed,0,elapsed));
    for(std::uint32_t remaining:{1u,2u,cap,cap+1})
        for(std::uint64_t elapsed:{std::uint64_t{0},ordinary,tail,overdue})
            assert(Budget::CanStartTick(0,remaining,elapsed));

    // Only the last historical transaction gets the larger start window.
    // Exact 8/12 ms boundaries reject additional work; no rounding grace.
    for(std::uint32_t completed=1;completed<cap;++completed){
        assert(Budget::CanStartTick(completed,2,ordinary-1));
        assert(!Budget::CanStartTick(completed,2,ordinary));
        assert(!Budget::CanStartTick(completed,2,tail-1));
        assert(Budget::CanStartTick(completed,1,ordinary));
        assert(Budget::CanStartTick(completed,1,tail-1));
        assert(!Budget::CanStartTick(completed,1,tail));
        assert(!Budget::CanStartTick(completed,1,overdue));
    }
    for(std::uint32_t completed:{cap,cap+1,std::numeric_limits<std::uint32_t>::max()})
        for(std::uint32_t remaining:{1u,2u,cap+1})
            assert(!Budget::CanStartTick(completed,remaining,0));

    // With three transactions costing 4, 5 and 5 ms, the final one starts
    // at 9 ms and finishes at 14 ms. This is intentional admission behavior,
    // not a claim that total callback time is bounded by 12 ms. With four
    // pending transactions, the same first two costs must yield at 9 ms.
    constexpr std::uint64_t costs[]{4'000'000,5'000'000,5'000'000};
    for(std::uint32_t depth:{3u,4u}){
        std::uint32_t completed=0,remaining=depth;
        std::uint64_t elapsed=0;
        while(Budget::CanStartTick(completed,remaining,elapsed)){
            assert(completed<3);elapsed+=costs[completed];
            ++completed;--remaining;
        }
        if(depth==3){assert(completed==3&&remaining==0&&elapsed==14'000'000);}
        else{assert(completed==2&&remaining==2&&elapsed==9'000'000);}
    }
    // Very cheap history cannot bypass the count cap, even before 8 ms.
    std::uint32_t completed=0,remaining=cap+3;
    while(Budget::CanStartTick(completed,remaining,0)){++completed;--remaining;}
    assert(completed==cap&&remaining==3);
}

void test_input_gap_pacing(){
    // A steady latency baseline inside four missing frames earns no brake,
    // even if it lasts for seconds or the observer runs at high refresh.
    for(unsigned missing:{0u,1u,2u,3u,4u}){
        Netplay::InputGapPacing pacing;pacing.Reset(1000);
        for(std::uint64_t now:{1000u,1001u,1008u,1016u,1050u,1150u,5000u}){
            pacing.Update(now,missing,true);near(pacing.Scale(),1.);
        }
    }
    // Five/six/seven missing frames have distinct targets; larger gaps cap at
    // twice the nominal interval rather than increasing the brake forever.
    for(unsigned missing:{5u,6u,7u,12u}){
        Netplay::InputGapPacing pacing;pacing.Reset(1000);
        pacing.Update(1050,missing,true);
        const double expected=missing==5?4./3.:missing==6?5./3.:2.;
        near(pacing.Scale(),expected);
        pacing.Update(5000,missing,true);near(pacing.Scale(),expected);
    }
    Netplay::InputGapPacing pacing;pacing.Reset(1000);
    pacing.Update(1025,7,true);near(pacing.Scale(),1.5);
    pacing.Update(1050,7,true);near(pacing.Scale(),2.);
    // One scale unit ramps up over 50 ms and releases over 150 ms.
    pacing.Update(1075,4,true);near(pacing.Scale(),11./6.);
    pacing.Update(1125,4,true);near(pacing.Scale(),1.5);
    pacing.Update(1200,4,true);near(pacing.Scale(),1.);

    pacing.Reset(2000);pacing.Update(2025,7,true);near(pacing.Scale(),1.5);
    for(unsigned i=0;i<100;++i){
        pacing.Update(2025,i%2?0:12,true);
        pacing.Update(2005,12,true); // a reversed clock cannot move the anchor
        near(pacing.Scale(),1.5);
    }
    pacing.Update(2050,7,true);near(pacing.Scale(),2.);
    pacing.Update(2065,0,true);near(pacing.Scale(),1.9);
    for(unsigned i=0;i<100;++i)pacing.Update(2065,0,true);
    near(pacing.Scale(),1.9);
    // Leaving live rollback clears the brake immediately; re-entry starts
    // from this timestamp and cannot reuse the disabled interval as credit.
    pacing.Update(2065,12,false);near(pacing.Scale(),1.);
    pacing.Update(2090,7,true);near(pacing.Scale(),1.5);
}

void test_network_gap_tail_targets_and_release(){
    using Tail=th10::multiplayer::NetworkInputGapPacing;
    // The whole old region must match the real common implementation on
    // changing input and clock traces, including stale pumps and disable.
    for(unsigned hz:{90u,120u,144u,165u,240u}){
        Tail tail;Netplay::InputGapPacing legacy;tail.Reset(1000);legacy.Reset(1000);
        for(unsigned callback=1;callback<=hz*4;++callback){
            const auto now=1000+std::uint64_t(callback)*1000/hz;
            const auto missing=(callback/7)%8;
            const bool enabled=callback%53!=0;
            tail.Update(now,missing,enabled);legacy.Update(now,missing,enabled);
            assert(tail.Scale()==legacy.Scale());
            tail.Update(now,12,true);tail.Update(now-1,12,true);
            assert(tail.Scale()==legacy.Scale());
        }
    }
    for(unsigned missing:{0u,1u,2u,3u,4u}){
        Tail tail;tail.Reset(0);
        for(std::uint64_t now:{1u,16u,1000u,60000u}){
            tail.Update(now,missing,true);assert(tail.Scale()==1.);
        }
    }
    constexpr double expected[]{1.,1.,1.,1.,1.,4./3.,5./3.,2.,2.375,3.5,5.375,8.,8.};
    for(unsigned missing=0;missing<=12;++missing){
        Tail tail;tail.Reset(0);tail.Update(1000,missing,true);near(tail.Scale(),expected[missing]);
    }
    Tail tail;tail.Reset(1000);
    tail.Update(1025,12,true);near(tail.Scale(),1.5);
    tail.Update(1050,12,true);near(tail.Scale(),2.);
    tail.Update(1100,12,true);near(tail.Scale(),3.);
    tail.Update(1350,12,true);near(tail.Scale(),8.);
    tail.Update(2000,std::numeric_limits<std::uint32_t>::max(),true);near(tail.Scale(),8.);

    // Analytic endpoint checks: 8 -> 2 consumes 225 ms in speed, then the
    // old 2 -> 1 release consumes 150 ms. A single callback crossing 2 must
    // use its remainder, just as many short callbacks do (not another 150).
    for(unsigned elapsed:{1u,50u,100u,224u,225u,226u,300u,374u,375u,600u}){
        Tail single;single.Reset(1000);single.Update(1350,12,true);
        single.Update(1350+elapsed,4,true);
        for(unsigned part:{1u,7u,8u,11u,16u,33u,375u}){
            Tail split;split.Reset(1000);split.Update(1350,12,true);
            unsigned at=0;
            while(at<elapsed){at=std::min(elapsed,at+part);split.Update(1350+at,4,true);}
            near(split.Scale(),single.Scale());
        }
        if(elapsed==225)near(single.Scale(),2.);
        if(elapsed==300)near(single.Scale(),1.5);
        if(elapsed==374)near(single.Scale(),1.+1./150.);
        if(elapsed>=375)near(single.Scale(),1.);
    }
    // Receive part of the missing interval, but not all of it. Release must
    // stop at that remaining target instead of overshooting to free speed.
    for(unsigned missing=5;missing<=11;++missing){
        Tail partial;partial.Reset(0);partial.Update(350,12,true);
        partial.Update(2000,missing,true);near(partial.Scale(),expected[missing]);
    }
    tail.Reset(1000);tail.Update(1350,12,true);
    for(unsigned repeat=0;repeat<100;++repeat){
        tail.Update(1350,0,true);tail.Update(1300,0,true);near(tail.Scale(),8.);
    }
    tail.Update(1350,12,false);near(tail.Scale(),1.);
    tail.Update(1375,12,true);near(tail.Scale(),1.5);
    // Oscillation at both policy boundaries cannot ratchet past the larger
    // target or leave a permanent brake after the remote information returns.
    for(unsigned low:{4u,7u}){
        Tail oscillating;oscillating.Reset(0);
        for(unsigned sample=1;sample<=200;++sample){
            oscillating.Update(sample*10,low+(sample&1),true);
            assert(oscillating.Scale()>=1.&&oscillating.Scale()<=expected[low+1]);
        }
        oscillating.Update(2375,4,true);near(oscillating.Scale(),1.);
    }
}

void test_network_gap_tail_phase_and_debt(){
    using Tail=th10::multiplayer::NetworkInputGapPacing;
    using Clock=th10::multiplayer::NetworkFrameSchedule;
    constexpr double step=Clock::interval;
    for(unsigned hz:{90u,120u,144u,165u,240u})for(double phase:{0.,.5,.999}){
        Clock clock;Tail pacing;pacing.Reset(0);
        assert(clock.advance(phase*step)==0);
        double normalized=phase*step;unsigned completed=0;
        for(unsigned callback=1;callback<=hz*2;++callback){
            const auto now=std::uint64_t(callback)*1000/hz;
            const auto old_debt=clock.debt_seconds(),old_alpha=clock.presentation_alpha();
            const unsigned missing=callback<hz/3?4:callback<hz?11:4;
            pacing.Update(now,missing,true);
            near(clock.debt_seconds(),old_debt); // changing speed cannot rescale debt
            const auto elapsed=(1./hz)/pacing.Scale();normalized+=elapsed;
            const auto due=clock.advance(elapsed);assert(due<=1);
            if(due){clock.complete();++completed;}
            else{
                assert(clock.presentation_alpha()>=old_alpha);
                assert(clock.presentation_alpha()<1.);
            }
            near(completed*step+clock.debt_seconds(),normalized);
        }
        near(pacing.Scale(),1.);
    }
    Clock cpu;Tail pacing;pacing.Reset(0);pacing.Update(350,12,true);
    const auto offered=cpu.advance(3.75*step);assert(offered==3);cpu.complete();cpu.defer(2);
    near(cpu.debt_seconds(),2.75*step);
    pacing.Update(400,4,true);near(cpu.debt_seconds(),2.75*step);
    assert(cpu.advance(0)==2);cpu.complete();cpu.complete();near(cpu.debt_seconds(),.75*step);
    // CPU catch-up/reconciliation owes whole ticks even while the network
    // scale changes. No new wall time is necessary to service those ticks.
    assert(cpu.advance(2.*step)==2);cpu.recovering(2);
    pacing.Update(725,4,true);near(pacing.Scale(),1.);near(cpu.debt_seconds(),2.75*step);
    assert(cpu.advance(0)==2);cpu.complete();cpu.complete();near(cpu.debt_seconds(),.75*step);
    pacing.Update(60000,4,true);near(pacing.Scale(),1.);
    assert(cpu.advance(60./pacing.Scale())==Clock::max_ticks);cpu.recovering(Clock::max_ticks);
    assert(cpu.advance(0)==Clock::max_ticks);
    for(unsigned tick=0;tick<Clock::max_ticks;++tick)cpu.complete();
    near(cpu.debt_seconds(),0.);

    // A genuine H wait discards elapsed wall time, including a deep brake's
    // recovery interval. It keeps exactly one pending tick and the old phase.
    Clock pending;assert(pending.advance(1.75*step)==1);pending.blocked(true);
    pacing.Reset(0);pacing.Update(350,12,true);
    for(unsigned callback=1;callback<=16;++callback){
        pacing.Update(350+callback*50,callback<8?12:4,true);
        assert(pending.advance(.05/pacing.Scale())==1);pending.blocked(true);
        near(pending.debt_seconds(),.75*step);near(pending.presentation_alpha(),1.);
    }
    pending.complete();assert(pending.advance(0)==0);near(pending.presentation_alpha(),.75);
    assert(pending.advance(.25*step)==1);pending.complete();near(pending.debt_seconds(),0.);
    assert(pending.advance(1.5*step)==1);pending.blocked(true);
    assert(pending.advance(10./pacing.Scale())==1);pending.recovering();
    assert(!pending.retry_pending()&&pending.advance(0)==1);pending.complete();
    near(pending.presentation_alpha(),.5);
}

// A real core with fixed deterministic input, H=12 and TH10 absolute touch.
// This is an isolated receive/cadence fixture; it does not claim a rendering,
// two-way network, CPU-budget, or real-game performance measurement.
struct GapCoreFixture {
    Netplay::RollbackCore core;
    std::uint32_t next=0;
    static Netplay::FrameInput remote(){
        Netplay::FrameInput input{};input.analogMode=Netplay::AnalogMode::DirectTouch;
        input.x=8300;input.y=36500;input.unlimited=true;input.touchUsed=true;return input;
    }
    static Netplay::FrameInput local(std::uint32_t frame){
        auto input=remote();input.x=12300+float(frame)*17;input.y=30000+float(frame)*3;
        input.buttons=std::uint16_t(frame&3);return input;
    }
    GapCoreFixture(){
        Netplay::CoreConfig config{};config.sessionId=101;config.maxRollbackFrames=12;
        config.directTouchIsAbsolute=true;config.maxDirectTouchDeltaPredictionFrames=0;
        assert(core.Reset(config));
        assert(core.SubmitRemoteInput(1,0,remote())==Netplay::RemoteInputResult::Accepted);
        for(unsigned frame=0;frame<5;++frame)assert(advance());
        assert(missing()==4);
    }
    unsigned missing()const{
        const auto confirmed=core.ConfirmedThroughAllRemotes();
        assert(confirmed!=Netplay::INVALID_FRAME);
        return next>confirmed+1?next-confirmed-1:0;
    }
    void receiveThrough(std::uint32_t frame){
        auto confirmed=core.ConfirmedThroughAllRemotes();
        while(confirmed<frame){
            const auto result=core.SubmitRemoteInput(1,++confirmed,remote());
            assert(result==Netplay::RemoteInputResult::Accepted||result==Netplay::RemoteInputResult::PredictionCorrect);
        }
        assert(!core.HasRollbackRequest());
    }
    bool advance(){
        const auto input=local(next);
        if(!core.HasLocalCapture(next))assert(core.ScheduleLocalInput(next,input));
        assert(core.LocalInput(next)==input);
        const auto decision=core.PrepareFrame(next);
        if(!decision.canAdvance){
            assert(missing()==12);
            auto changed=input;changed.x+=1;
            assert(!core.ScheduleLocalInput(next,changed)&&core.LocalInput(next)==input);
            return false;
        }
        assert(decision.inputs[0]==input&&decision.inputs[1]==remote());
        assert(core.MarkSimulated(next,decision));++next;assert(missing()<=12);return true;
    }
};
template<class Pacing> void check_steady_gap_core(unsigned hz){
    GapCoreFixture fixture;th10::multiplayer::NetworkFrameSchedule clock;
    Pacing pacing;pacing.Reset(1000);
    for(unsigned callback=1;callback<=hz*3;++callback){
        // Independently paced 60 Hz source, four frames behind the initial
        // simulation frontier. Its delivery clock is not fed by local scale.
        fixture.receiveThrough(std::uint64_t(callback)*60/hz);
        assert(fixture.missing()<=4);
        pacing.Update(1000+std::uint64_t(callback)*1000/hz,fixture.missing(),true);
        assert(pacing.Scale()==1.);
        const auto due=clock.advance((1./hz)/pacing.Scale());assert(due<=1);
        if(due){assert(fixture.advance());clock.complete();}
        assert(fixture.missing()<=4&&!clock.retry_pending());
    }
    assert(fixture.next==185);near(clock.debt_seconds(),0.);
}
struct DroughtResult {
    double first_wait=-1,first_horizon=-1,first_resume=-1;
    unsigned blocked=0,fractional=0,final_frame=0;
};
template<class Pacing> DroughtResult run_gap_drought(unsigned hz,double phase,unsigned drought_ms){
    using Clock=th10::multiplayer::NetworkFrameSchedule;
    GapCoreFixture fixture;Clock clock;Pacing pacing;pacing.Reset(1000);
    assert(clock.advance(phase*Clock::interval)==0);
    DroughtResult result{};
    const auto callbacks=unsigned(std::ceil(double(drought_ms+800)*hz/1000.));
    for(unsigned callback=1;callback<=callbacks;++callback){
        const double elapsed=double(callback)*1000./hz;
        const bool received=elapsed+1.e-7>=drought_ms;
        // The finite drought ends with the missing contiguous input. Then
        // new actual samples are available before each next forward frame.
        if(received)fixture.receiveThrough(fixture.next);
        pacing.Update(1000+std::uint64_t(elapsed),fixture.missing(),true);
        const auto old_next=fixture.next;const auto old_alpha=clock.presentation_alpha();
        const auto due=clock.advance((1./hz)/pacing.Scale());assert(due<=1);
        if(due){
            if(fixture.advance()){
                clock.complete();
                if(received&&result.first_resume<0)result.first_resume=elapsed;
                if(fixture.missing()==12&&result.first_horizon<0)result.first_horizon=elapsed;
            }else{
                assert(!received);clock.blocked(true);++result.blocked;
                if(result.first_wait<0)result.first_wait=elapsed;
                near(clock.presentation_alpha(),1.);
            }
        }else{
            assert(!clock.retry_pending()&&fixture.next==old_next);
            assert(clock.presentation_alpha()>=old_alpha&&clock.presentation_alpha()<1.);
            ++result.fractional;
        }
        assert(fixture.missing()<=12&&clock.debt_seconds()<Clock::interval);
    }
    assert(result.first_wait>=0&&result.first_wait<drought_ms&&result.blocked>0);
    assert(result.first_resume>=drought_ms-1.e-7&&result.first_resume<drought_ms+1000./hz+1.e-7);
    assert(!clock.retry_pending()&&fixture.missing()==0);near(pacing.Scale(),1.);
    result.final_frame=fixture.next;return result;
}
void test_network_gap_tail_core_cadence(){
    using Tail=th10::multiplayer::NetworkInputGapPacing;
    for(unsigned hz:{90u,120u,144u,165u,240u}){
        check_steady_gap_core<Netplay::InputGapPacing>(hz);check_steady_gap_core<Tail>(hz);
        for(double phase:{0.,.5,.999})for(unsigned drought:{600u,2000u}){
            const auto off=run_gap_drought<Netplay::InputGapPacing>(hz,phase,drought);
            const auto on=run_gap_drought<Tail>(hz,phase,drought);
            // The experiment must buy prediction time in this cheap, known
            // input-drought fixture, without pretending to avoid H forever.
            assert(on.first_horizon>off.first_horizon&&on.first_wait>off.first_wait);
            assert(on.blocked<off.blocked&&on.fractional>off.fractional);
            assert(on.final_frame>13&&off.final_frame>13);
        }
    }
    // Counterexample to a blanket no-hold claim: already-owed CPU work is
    // not slowed retroactively. It can still consume the remaining H window
    // after the pacing target rises; the real core must reject the next tick.
    using Clock=th10::multiplayer::NetworkFrameSchedule;
    GapCoreFixture fixture;Clock clock;Tail pacing;pacing.Reset(1000);
    const auto offered=clock.advance(Clock::max_ticks*Clock::interval);
    assert(offered==6&&fixture.advance());clock.complete();clock.defer(offered-1);
    pacing.Update(1025,fixture.missing(),true);near(pacing.Scale(),4./3.);
    assert(clock.advance(0)==5);
    for(unsigned tick=0;tick<5;++tick){assert(fixture.advance());clock.complete();}
    assert(fixture.missing()==10);pacing.Update(1275,fixture.missing(),true);near(pacing.Scale(),5.375);
    assert(clock.advance(0)==0); // target changes invent no normalized time
    assert(clock.advance(3.*Clock::interval)==3);
    for(unsigned tick=0;tick<2;++tick){assert(fixture.advance());clock.complete();}
    assert(fixture.missing()==12&&!fixture.advance());clock.blocked(true);
    assert(clock.retry_pending());near(clock.presentation_alpha(),1.);
}

void test_network_frame_schedule(){
    using Clock=th10::multiplayer::NetworkFrameSchedule;
    constexpr double step=Clock::interval;
    Clock clock;
    // A 240 Hz display has three fractional views before the next 60 Hz
    // tick. A pace change scales only new elapsed time, never existing phase.
    for(unsigned i=1;i<4;++i){
        assert(clock.advance(step/4)==0);near(clock.presentation_alpha(),i/4.);
    }
    assert(clock.advance(step/4)==1);clock.complete();
    near(clock.presentation_alpha(),0.);
    assert(clock.advance(step*.75)==0);near(clock.presentation_alpha(),.75);
    assert(clock.advance((step*.25)/2.)==0);near(clock.presentation_alpha(),.875);
    assert(clock.advance((step*.25)/2.)==1);clock.complete();
    near(clock.presentation_alpha(),0.);
    for(unsigned hz:{90u,120u,144u,165u,240u}){
        Clock high;unsigned ticks=0;
        for(unsigned callback=0;callback<hz*2;++callback){
            const auto due=high.advance(1./hz);
            assert(due<=1);ticks+=due;if(due)high.complete();
            assert(high.presentation_alpha()>=0&&high.presentation_alpha()<=1);
        }
        assert(ticks==120);near(high.debt_seconds(),0.);
    }

    // If one due tick spends the 8 ms start budget, the two offered but
    // unexecuted ticks survive for the next callback without new wall time.
    clock.reset();const auto offered=clock.advance(step*3.5);
    assert(offered==3);near(clock.debt_seconds(),step*.5);
    assert(Netplay::FrameBudget::CanStartTick(0,Netplay::FrameBudget::CatchupBudgetNs));
    clock.complete();
    assert(!Netplay::FrameBudget::CanStartTick(1,Netplay::FrameBudget::CatchupBudgetNs));
    clock.defer(offered-1);
    near(clock.debt_seconds(),step*2.5);near(clock.presentation_alpha(),1.);
    const auto returned=clock.advance(0);
    assert(returned==2);
    for(unsigned i=0;i<returned;++i)clock.complete();
    assert(1+returned==offered);near(clock.presentation_alpha(),.5);
    assert(clock.advance(0)==0);
    assert(clock.advance(step*.5)==1);clock.complete();

    // A long CPU interruption is bounded by the declared catch-up cap.
    // Returning the unfinished portion must not create a second backlog.
    clock.reset();const auto capped=clock.advance(10.);
    assert(capped==Clock::max_ticks);clock.complete();clock.defer(capped-1);
    assert(clock.advance(0)==capped-1);clock.complete();
    assert(clock.advance(0)==0);near(clock.debt_seconds(),0.);

    {
        clock.reset();assert(clock.advance(step*2.75)==2);clock.complete();
        clock.blocked(true);
        const auto phase=clock.debt_seconds();near(phase,step*.75);
        assert(clock.retry_pending());near(clock.presentation_alpha(),1.);
        // The pending forward frame already owns the obligation. Even a
        // caller returning the other offered ticks cannot add it a second time.
        clock.defer(1);clock.defer(Clock::max_ticks);
        near(clock.debt_seconds(),phase);
        for(double wait:{0.,.001,step*.5,step*2,10.,100.}){
            assert(clock.advance(wait)==1);
            clock.blocked(true);
            clock.defer(Clock::max_ticks);
            assert(clock.retry_pending());near(clock.debt_seconds(),phase);
            near(clock.presentation_alpha(),1.);
        }
        clock.complete();assert(!clock.retry_pending());
        assert(clock.advance(0)==0);near(clock.presentation_alpha(),.75);
        assert(clock.advance(step*.25)==1);clock.complete();
        assert(clock.advance(0)==0);near(clock.debt_seconds(),0.);
    }
    // Runtime/generation reset must discard both retry and fractional phase.
    assert(clock.advance(step*1.5)==1);clock.recovering();clock.reset();
    assert(!clock.retry_pending());assert(clock.advance(0)==0);
    near(clock.presentation_alpha(),0.);
}

void test_cpu_recovery_schedule(){
    using Clock=th10::multiplayer::NetworkFrameSchedule;
    constexpr double step=Clock::interval;
    Clock clock;
    // A CPU slice executes no forward frame. Preserve every offered but
    // unfinished frame, including siblings of the frame being reconciled.
    const auto offered=clock.advance(step*3.75);
    assert(offered==3);clock.complete();
    clock.recovering(offered-1);
    assert(!clock.retry_pending());near(clock.debt_seconds(),step*2.75);
    near(clock.presentation_alpha(),1.);
    const auto resumed=clock.advance(step*.5);
    assert(resumed==3);
    for(unsigned i=0;i<resumed;++i)clock.complete();
    near(clock.presentation_alpha(),.25);
    assert(clock.advance(0)==0);

    // Information arrives during a genuine network wait and requests undo.
    // Keep the one already-due frame but do not resurrect any waiting time.
    clock.reset();assert(clock.advance(step*1.25)==1);clock.blocked(true);
    for(double waiting:{.5,10.,100.}){
        assert(clock.advance(waiting)==1);clock.blocked(true);
        near(clock.debt_seconds(),step*.25);
    }
    assert(clock.advance(100.)==1);clock.recovering();
    assert(!clock.retry_pending());near(clock.debt_seconds(),step*1.25);
    assert(clock.advance(step*.75)==2);clock.complete();clock.complete();
    near(clock.debt_seconds(),0.);assert(clock.advance(0)==0);

    // CPU recovery stays bounded even when the event loop was interrupted.
    clock.reset();assert(clock.advance(step)==1);clock.recovering();
    const auto capped=clock.advance(100.);
    assert(capped==Clock::max_ticks);clock.recovering(capped);
    near(clock.debt_seconds(),step*Clock::max_ticks);
    assert(clock.advance(0)==Clock::max_ticks);
    for(unsigned i=0;i<Clock::max_ticks;++i)clock.complete();
    near(clock.debt_seconds(),0.);

    // The nominal clock must conserve two seconds across many partial
    // corrections on actual high-refresh callback cadences. Retrying CPU
    // work cannot reduce 120 owed ticks; no input or logical tick is skipped.
    for(unsigned hz:{90u,120u,144u,165u,240u}){
        Clock live;unsigned completed=0,yields=0;bool resuming=false;
        for(unsigned callback=0;callback<hz*2;++callback){
            const auto due=live.advance(1./hz);
            if(!due)continue;
            if(!resuming&&callback%7==0){
                live.recovering(due);resuming=true;++yields;
            }else{
                for(unsigned i=0;i<due;++i){live.complete();++completed;}
                resuming=false;
            }
            assert(live.debt_seconds()<=Clock::max_ticks*step);
            assert(live.presentation_alpha()>=0&&live.presentation_alpha()<=1);
        }
        near(completed*step+live.debt_seconds(),2.);
        const auto final_due=live.advance(0);
        for(unsigned i=0;i<final_due;++i){live.complete();++completed;}
        assert(completed==120&&yields>0);near(live.debt_seconds(),0.);
    }
}
}

int main(){
    using Clock=Netplay::PendingFrameSchedule<touhou::sdl::FrameCadence>;
    constexpr auto step=touhou::sdl::FrameCadence::interval;
    // Healthy SP/live cadence remains the title's original cadence.
    for(bool live:{false,true})for(double hz:{30.,59.94,60.,90.,120.,144.,165.,240.}){
        Clock clock;touhou::sdl::FrameCadence original;
        for(unsigned i=0;i<unsigned(hz*10);++i){
            assert(clock.advance(1./hz,live)==original.advance(1./hz));
            clock.complete();
        }
    }
    Clock stalled;
    assert(stalled.advance(step*1.75,true)==1);stalled.blocked(true);
    const auto phase=stalled.debt;
    for(double wait:{0.,.001,step*.5,step*2,10.,100.}){
        assert(stalled.advance(wait,true)==1);
        stalled.blocked(true);
        assert(stalled.retry_pending()&&stalled.debt==phase);
    }
    // Recovery services that pending frame once; it cannot repay the stall.
    stalled.complete();assert(!stalled.retry_pending());
    assert(stalled.advance(0,true)==0);
    assert(stalled.advance(step*.3,true)==1);stalled.complete();
    assert(stalled.advance(0,true)==0);
    // CPU stalls also discard expired wall-time slots, preserving logical order.
    stalled.reset();assert(stalled.advance(1.,true)==1);stalled.complete();
    for(unsigned i=0;i<60;++i){assert(stalled.advance(step,true)==1);stalled.complete();}
    assert(stalled.advance(0,true)==0);
    stalled.blocked(true);stalled.reset();assert(stalled.advance(0,true)==0);
    assert(stalled.advance(step,true)==1);stalled.blocked(true);
    assert(stalled.advance(0,false)==0&&!stalled.retry_pending());
    for(unsigned hz:{60u,120u,144u,165u}){
        Clock clock[2];Netplay::SessionPacing pacing[2];
        unsigned frame[2]{108,100};
        for(unsigned callback=0;callback<hz*60;++callback){
            for(unsigned seat=0;seat<2;++seat){
                const auto peer=1-seat;Netplay::InputPacket packet{};
                packet.senderPlayer=peer;packet.senderFrame=frame[peer]-3;
                packet.frameAdvantage=short(int(frame[peer])-int(frame[seat])+3);
                pacing[seat].Observe(frame[seat],packet);
            }
            for(unsigned seat=0;seat<2;++seat){
                const auto ticks=clock[seat].advance((1./hz)/pacing[seat].IntervalScale(),true);
                assert(ticks<=1);frame[seat]+=ticks;
                clock[seat].complete();
                assert(clock[seat].interpolation_alpha()>=0&&clock[seat].interpolation_alpha()<=1);
            }
        }
        const auto difference=int(frame[0])-int(frame[1]);
        assert(difference>=-2&&difference<=2);
        assert(frame[0]>3600&&frame[1]>3600);
    }
    test_input_gap_pacing();
    test_network_gap_tail_targets_and_release();
    test_network_gap_tail_phase_and_debt();
    test_network_gap_tail_core_cadence();
    test_recovery_budget();
    test_network_frame_schedule();
    test_cpu_recovery_schedule();
    std::puts("TH10 netplay display cadence: PASS");
}
