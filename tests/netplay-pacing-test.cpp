#include "../portable/sdl/FrameCadence.hpp"
#include <eagler/netplay/SessionPacing.hpp>
#include <eagler/netplay/PendingFrameSchedule.hpp>
#include <eagler/netplay/InputGapPacing.hpp>
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
    test_recovery_budget();
    test_network_frame_schedule();
    test_cpu_recovery_schedule();
    std::puts("TH10 netplay display cadence: PASS");
}
