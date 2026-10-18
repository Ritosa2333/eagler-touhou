#include "../th10_web/cpp/multiplayer/TouchInputFrontier.hpp"
#include "../portable/input/TouchController.hpp"

#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace {
using Frontier=th10::multiplayer::TouchInputFrontier;
using Words=Frontier::Words;
using Identity=Frontier::Identity;
using Progress=Frontier::Progress;
using touhou::input::TouchController;
using touhou::input::TouchState;
using touhou::input::TouchSample;
static_assert(sizeof(Words)==32);

[[noreturn]] void fail(const char* expression,int line){
    std::fprintf(stderr,"touch input frontier: line %d: %s\n",line,expression);
    std::abort();
}
#define CHECK(expression) do { if(!(expression))fail(#expression,__LINE__); } while(false)

bool near(float left,float right){return std::abs(left-right)<0.0002f;}
Words state_words(float x=100,float y=200,bool ready=true,int context=1,
                  std::uint32_t instance=7,float fast=4,float slow=2,float rate=1){
    Words result{static_cast<std::uint32_t>(context),instance,ready?1u:0u};
    const float values[]{x,y,fast,slow,rate};
    std::memcpy(result.data()+3,values,sizeof(values));
    return result;
}
TouchState decode(const Words& words){
    TouchState state;
    state.context=static_cast<int>(words[0]);state.instance=static_cast<int>(words[1]);
    state.ready=words[2]!=0;
    float values[5]{};std::memcpy(values,words.data()+3,sizeof(values));
    state.x=values[0];state.y=values[1];state.fast=values[2];state.slow=values[3];
    state.min_x=-184;state.max_x=184;state.min_y=32;state.max_y=432;
    return state;
}

struct Host {
    int world_token=1,session_token=2;
    Frontier frontier;
    Identity identity;
    Progress progress;
    Words current=state_words();
    std::uint32_t next_frame=100;
    std::uint64_t gesture_epoch=0;
    unsigned source_reads=0;
    TouchController gestures;
    Host(){
        identity={&world_token,&session_token,0x71a4c2093b8d55e1ull,4,1,2,7};
        gestures.begin_session();
        // Recovery starts in an already sampled gameplay context. Establish
        // it before action serials, just as the preceding ordinary ticks did.
        gestures.sample(decode(current),0,false,false);
        CHECK(publish());
    }
    bool publish(){
        return frontier.Publish(identity,progress,next_frame,[&](Words& words){
            ++source_reads;words=current;
        });
    }
    Words read_words(){
        Words output{};
        frontier.Read(identity,progress,next_frame,[&](Words& words){
            ++source_reads;words=current;
        },output);
        // The production GameHost does this after application_touch_state.
        if(gesture_epoch!=frontier.Epoch()){
            gestures.cancel_transient();gesture_epoch=frontier.Epoch();
        }
        return output;
    }
    TouchState read(){return decode(read_words());}
    void pointer(int type,int id,float x,float y,std::uint64_t time){
        gestures.pointer(type,id,x,y,time,read(),false);
    }
    TouchSample capture(std::uint64_t time){
        // Keep physical pointer intake separate from the existing CanCapture
        // gate: historical recovery never consumes action/sample ticks.
        CHECK(progress.complete());
        return gestures.sample(read(),time,false,false);
    }
};

void new_press_uses_complete_frontier(){
    Host host;host.read();
    const auto complete=host.current;
    const auto reads=host.source_reads;
    const auto epoch=host.frontier.Epoch();
    host.progress.correcting=true;host.next_frame=40;
    host.current=state_words(60);
    CHECK(host.read_words()==complete);
    host.pointer(0,1,.5f,.5f,10);
    host.pointer(1,1,.525f,.5f,20); // +16 physical pixels
    CHECK(host.source_reads==reads&&host.frontier.Epoch()==epoch);
    CHECK(host.frontier.Frontier()==100);

    // Recovery can finish at the same exclusive frontier without a new tick.
    host.progress.correcting=false;host.next_frame=100;host.current=complete;
    const auto sample=host.capture(30);
    CHECK(sample.motion==1&&near(sample.x,116)&&near(sample.y,200));

    // Negative control: the old direct getter anchors the same gesture at 60.
    TouchController historical;
    historical.pointer(0,1,.5f,.5f,10,decode(state_words(60)),false);
    historical.pointer(1,1,.525f,.5f,20,decode(state_words(60)),false);
    const auto wrong=historical.sample(decode(complete),30,false,false);
    CHECK(wrong.motion==1&&near(wrong.x,76)&&near(sample.x-wrong.x,40));
}

void historical_ready_context_and_speed(){
    Host host;host.read();
    host.gestures.controls(true,false,4,9,0,0);
    host.pointer(0,1,.5f,.5f,0);
    TouchController reference;
    reference.sample(decode(host.current),0,false,false);
    reference.controls(true,false,4,9,0,0);
    reference.pointer(0,1,.5f,.5f,0,decode(host.current),false);
    const auto complete=host.current;
    const auto reads=host.source_reads;
    host.progress.correcting=true;
    const std::array<Words,5> history{
        state_words(60,30,false,1,7,9,1,.5f),
        state_words(20,400,true,0,7,1,1,2),
        state_words(-60,300,false,3,7,3,2,.5f),
        state_words(0,40,true,2,99,30,1,4),
        state_words(30,50,true,1,99,2,1,.25f)
    };
    for(std::size_t index=0;index<history.size();++index){
        host.current=history[index];host.next_frame=40+static_cast<std::uint32_t>(index);
        CHECK(host.read_words()==complete); // Freeze all eight words.
        const float x=.5f+float(index+1)*.025f;
        host.pointer(1,1,x,.5f,index+1);
        reference.pointer(1,1,x,.5f,index+1,decode(complete),false);
    }
    CHECK(host.source_reads==reads&&host.gestures.active());
    host.progress.correcting=false;host.current=state_words(110);host.next_frame=100;
    const auto actual=host.capture(10);
    const auto expected=reference.sample(decode(host.current),10,false,false);
    CHECK(actual.motion==1&&near(actual.x,180)&&near(actual.x,expected.x));
    CHECK(actual.keys[90]&&actual.keys[88]&&actual.keys[27]);

    TouchController wrong_context;
    wrong_context.sample(decode(complete),0,false,false);
    wrong_context.controls(true,false,4,9,0,0);
    wrong_context.pointer(0,1,.5f,.5f,0,decode(complete),false);
    wrong_context.pointer(1,1,.525f,.5f,1,decode(history[1]),false);
    const auto canceled=wrong_context.sample(decode(complete),10,false,false);
    CHECK(!canceled.motion&&!canceled.keys[88]&&!canceled.keys[27]);

    TouchController wrong_ready;
    wrong_ready.pointer(0,1,.5f,.5f,0,decode(complete),false);
    wrong_ready.pointer(1,1,.525f,.5f,1,decode(history[0]),false);
    const auto swallowed=wrong_ready.sample(decode(complete),10,false,false);
    CHECK(swallowed.motion==1&&near(swallowed.x,100)); // The move was swallowed.

    // A genuinely completed pause is published and still cancels the drag.
    host.current=state_words(110,200,false,3);
    host.pointer(1,1,.65f,.5f,20);
    CHECK(!host.gestures.active()&&!host.capture(21).motion);
    // Completed immobility keeps the title's deathbomb action policy.
    host.current=state_words(110,200,false,1);
    host.capture(22);host.gestures.controls(true,false,5,9,0,0);
    const auto deathbomb=host.capture(23);
    CHECK(!deathbomb.motion&&deathbomb.keys[88]);
}

void incomplete_transactions_and_no_forward_finish(){
    Host host;
    const auto complete=host.current;
    const auto epoch=host.frontier.Epoch();
    const auto reads=host.source_reads;
    const std::array<Progress,4> incomplete{
        Progress{true,false,false,true},Progress{false,true,false,true},
        Progress{false,false,true,true},Progress{false,false,false,false}
    };
    for(const auto& progress:incomplete){
        host.progress=progress;host.current=state_words(60,40,false,3,99);
        host.next_frame=17;
        CHECK(!host.publish());
        Words output{};const bool available=host.frontier.Copy(host.identity,progress,output);
        CHECK(available==progress.usable);
        CHECK(output==(progress.usable?complete:Words{}));
        CHECK(host.frontier.Frontier()==100&&host.frontier.Epoch()==epoch);
    }
    CHECK(host.source_reads==reads);

    // Nested rollback can rewind to zero/INVALID while the old frontier stays
    // 100. Neither a later chunk nor a fresh rollback may read current.
    host.progress={true,false,false,true};
    for(const auto frame:std::array<std::uint32_t,4>{60,30,0,Frontier::NoFrame}){
        host.next_frame=frame;host.current=state_words(float(frame&255u));
        CHECK(!host.publish()&&host.read_words()==complete);
    }
    CHECK(host.source_reads==reads&&host.frontier.Frontier()==100);

    // No "frame increased" condition: correction changed the state at the
    // same frontier and can finish before network permits another forward.
    host.progress={};host.next_frame=100;host.current=state_words(108);
    CHECK(host.publish()&&host.source_reads==reads+1);
    CHECK(host.frontier.Frontier()==100&&host.read_words()==host.current);
    host.pointer(0,3,.5f,.5f,30);host.pointer(1,3,.525f,.5f,31);
    CHECK(near(host.capture(32).x,124));

    // A reconciled lifecycle boundary can retire the old predicted future.
    host.next_frame=80;host.current=state_words(90);
    CHECK(host.publish()&&host.frontier.Frontier()==80);
}

void same_address_restart_and_serials(){
    Host host;host.read();
    host.gestures.controls(true,false,17,23,0,0);
    host.pointer(0,1,.5f,.5f,0);host.pointer(1,1,.525f,.5f,1);
    CHECK(host.gestures.active());
    const auto epoch=host.frontier.Epoch();
    const auto old_world=host.identity.world,old_session=host.identity.session;
    const auto old_instance=host.current[1];
    ++host.identity.generation;
    host.progress.correcting=true;host.current=state_words(20);
    CHECK(host.current[1]==old_instance&&host.identity.world==old_world&&host.identity.session==old_session);
    CHECK(host.read_words()==Words{}&&!host.frontier.Valid());
    CHECK(host.frontier.Epoch()!=epoch&&!host.gestures.active());
    host.progress={};host.next_frame=1;
    CHECK(host.publish());
    // Repeating a launcher snapshot must not look like new action serials.
    host.gestures.controls(true,false,17,23,0,0);
    auto sample=host.capture(10);
    CHECK(sample.keys[90]&&!sample.keys[88]&&!sample.keys[27]&&!sample.motion);
    host.pointer(0,1,.5f,.5f,11);host.pointer(1,1,.525f,.5f,12);
    CHECK(near(host.capture(13).x,36));

    // Negative control: changing cached coordinates alone cannot retire an
    // active gesture when the allocator reused the Player instance address.
    TouchController stale;
    stale.pointer(0,1,.5f,.5f,0,decode(state_words()),false);
    stale.pointer(1,1,.525f,.5f,1,decode(state_words()),false);
    const auto stale_sample=stale.sample(decode(state_words(20)),10,false,false);
    CHECK(stale_sample.motion==1&&near(stale_sample.x,116));

    // Explicit lifecycle invalidation also works when every identity value,
    // including generation, happens to be reused by a new owner.
    const auto repeated_epoch=host.frontier.Epoch();
    host.frontier.Invalidate();host.progress.correcting=true;
    CHECK(host.read_words()==Words{}&&host.frontier.Epoch()!=repeated_epoch);
    CHECK(!host.gestures.active());
    host.progress={};CHECK(host.publish());

    // The forbidden begin_session replacement would reset serial baselines.
    TouchController wrong_serials;
    wrong_serials.sample(decode(state_words()),0,false,false);
    wrong_serials.controls(true,false,17,23,0,0);
    wrong_serials.cancel_transient();wrong_serials.begin_session();
    wrong_serials.controls(true,false,17,23,0,0);
    const auto replayed=wrong_serials.sample(decode(state_words()),20,false,false);
    CHECK(replayed.keys[88]&&replayed.keys[27]);
}

void every_identity_owner_invalidates(){
    Host host;
    int another_world=3,another_session=4;
    const auto original=host.identity;
    std::array<Identity,13> replacements{};
    replacements.fill(original);
    replacements[0].world=&another_world;
    replacements[1].session=&another_session;
    ++replacements[2].session_id;
    ++replacements[3].generation;
    ++replacements[4].seat;
    ++replacements[5].players;
    for(std::size_t bit=0;bit<7;++bit)replacements[6+bit].role^=1u<<bit;
    for(const auto& identity:replacements){
        Frontier frontier;
        CHECK(frontier.Publish(original,{},100,[](Words& words){words=state_words();}));
        const auto epoch=frontier.Epoch();
        unsigned reads=0;
        CHECK(!frontier.Publish(identity,{true,false,false,true},40,[&](Words& words){
            ++reads;words=state_words(60);
        }));
        Words output{};CHECK(!frontier.Copy(identity,{true,false,false,true},output));
        CHECK(output==Words{}&&reads==0&&!frontier.Valid()&&frontier.Epoch()!=epoch);
    }
}

void cancel_repeated_recovery_and_actions(){
    Host host;host.read();
    host.gestures.controls(true,true,17,23,0,0);
    host.pointer(0,1,.5f,.5f,0);host.pointer(1,1,.525f,.5f,1);
    const auto epoch=host.frontier.Epoch();
    const auto frame=host.frontier.Frontier();
    const auto complete=host.current;
    host.progress.correcting=true;host.current=state_words(60,20,false,3);
    host.gestures.cancel_transient(); // pointercancel / blur does not clear world view
    CHECK(!host.gestures.active()&&host.frontier.Valid());
    CHECK(host.read_words()==complete&&host.frontier.Epoch()==epoch&&host.frontier.Frontier()==frame);
    host.gestures.controls(true,false,17,23,0,0);
    host.pointer(0,2,.5f,.5f,10);host.pointer(1,2,.525f,.5f,11);
    host.progress={};host.next_frame=100;host.current=complete;
    auto sample=host.capture(12);
    CHECK(sample.motion==1&&near(sample.x,116)&&sample.keys[90]);
    CHECK(!sample.keys[16]&&!sample.keys[88]&&!sample.keys[27]);

    host.gestures.controls(true,false,18,24,0,0);
    const auto reads=host.source_reads;
    host.progress.correcting=true;
    for(unsigned chunk=0;chunk<32;++chunk){
        host.current=state_words(float(chunk),30,false,chunk%2?0:3,99);
        host.next_frame=chunk;
        CHECK(host.read_words()==complete);
    }
    CHECK(host.source_reads==reads&&host.frontier.Epoch()==epoch);
    host.progress={};host.next_frame=100;host.current=complete;
    for(unsigned tick=0;tick<3;++tick){
        sample=host.capture(20+tick);
        CHECK(sample.keys[88]&&sample.keys[27]);
    }
    sample=host.capture(23);CHECK(!sample.keys[88]&&!sample.keys[27]);
    host.gestures.controls(true,false,18,24,0,0);
    sample=host.capture(24);CHECK(!sample.keys[88]&&!sample.keys[27]);
}

void empty_and_unusable_never_read_history(){
    Frontier frontier;
    const Identity identity{};
    unsigned reads=0;
    const auto source=[&](Words& words){++reads;words=state_words();};
    Words words{};
    CHECK(!frontier.Publish(identity,{true,false,false,true},0,source));
    CHECK(!frontier.Copy(identity,{true,false,false,true},words)&&words==Words{}&&reads==0);
    CHECK(frontier.Publish(identity,{},1,source)&&reads==1);
    CHECK(!frontier.Copy(identity,{false,false,false,false},words)&&words==Words{});
    CHECK(frontier.Valid()); // suspension alone does not retire this world's snapshot
    CHECK(frontier.Copy(identity,{true,false,false,true},words)&&words==state_words());
}

void cold_initial_controls_and_rewind_to_zero(){
    Frontier frontier;const Identity identity{};
    const Progress cold{false,false,false,true,false};
    unsigned reads=0;Words current=state_words(),output{};
    const auto source=[&](Words& words){++reads;words=current;};
    CHECK(!frontier.Publish(identity,cold,0,source)&&reads==0&&!frontier.Valid());
    CHECK(frontier.Read(identity,cold,0,source,output)&&output==current&&reads==1);
    CHECK(!frontier.Valid()&&frontier.Frontier()==Frontier::NoFrame);
    // Initial pointer input still uses the original native starting state.
    TouchController gestures;
    gestures.pointer(0,1,.5f,.5f,0,decode(output),false);
    gestures.pointer(1,1,.525f,.5f,1,decode(output),false);
    CHECK(near(gestures.sample(decode(output),2,false,false).x,116));

    current=state_words(108);
    CHECK(frontier.Publish(identity,{},1,source)&&reads==2&&frontier.Valid());
    const auto complete=current;current=state_words(60,30,false,3,99);
    const Progress rewound_to_zero{true,false,false,true,false};
    CHECK(frontier.Read(identity,rewound_to_zero,0,source,output)&&output==complete);
    CHECK(reads==2&&frontier.Frontier()==1);
}
} // namespace

int main(){
    new_press_uses_complete_frontier();
    historical_ready_context_and_speed();
    incomplete_transactions_and_no_forward_finish();
    same_address_restart_and_serials();
    every_identity_owner_invalidates();
    cancel_repeated_recovery_and_actions();
    empty_and_unusable_never_read_history();
    cold_initial_controls_and_rewind_to_zero();
    std::puts("touch input frontier: PASS real TouchController; 8-word freeze, cold initial controls, x60 vs x100 +16px negative, historical ready/context/instance/speed, nested/open-frame recovery, no-forward finish, same-address restart, all identity fields, cancel and action serials");
}
