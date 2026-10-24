// Use the production numeric backend for ReplayRecording::begin snapshots.
#define SOFTFLOAT_FAST_INT64
#define INLINE_LEVEL 5
extern "C" {
#include "../third_party/softfloat.c"
}
#include "ReplayRecording.hpp"
#include <cassert>
using namespace th08;
static GameplaySession session;
static EclGlobals globals;
static ReplayRecording fresh(){
    ReplayRecording recording;globals.game_flags=4;globals.shot=0;globals.difficulty=0;
    assert(recording.begin(0,session,globals,0));recording.input.flags=4;return recording;
}
static u16 word(const ReplayRecording& r,u32 offset){const auto& b=r.stage(0)->stream.input;return b[offset]|u16(b[offset+1])<<8;}
static u16 playback_pending(const ReplayRecording& r){
    const auto& s=r.stage(0)->stream;ReplayStream play;ReplayInputState input;input.flags=4;
    assert(play.begin_playback(s.input.data(),s.input_end,s.timing.data(),s.timing_end,false));
    assert(play.play(input)&&input.current==0);assert(play.play(input));return input.current;
}
int main(){
    // The failures are real pipeline states | clearing keys or live publication alone is insufficient.
    auto keys_only=fresh();keys_only.input.physical=128;keys_only.update();keys_only.input.physical=0;
    assert(keys_only.input.current==128&&playback_pending(keys_only)==128);
    auto live_only=fresh();live_only.input.physical=128;live_only.update();live_only.input.current=live_only.input.physical=0;
    assert(playback_pending(live_only)==128&&live_only.input.current==0);

    auto cancelled=fresh();cancelled.input.current=64;cancelled.input.physical=128;cancelled.update();
    auto& stream=cancelled.stage(0)->stream;const auto frame=stream.frame;const auto cursor=stream.input_cursor;
    const auto end=stream.input_end,timing_cursor=stream.timing_cursor,timing_end=stream.timing_end;
    const auto timing=stream.timing;const auto seed=cancelled.stage(0)->snapshot.seed;
    cancelled.cancel_pending_input();assert(cancelled.input.current==0&&cancelled.input.physical==0&&cancelled.input.previous==64);
    assert(playback_pending(cancelled)==0&&word(cancelled,cursor)==0);
    assert(stream.frame==frame&&stream.input_cursor==cursor&&stream.input_end==end&&stream.timing_cursor==timing_cursor&&stream.timing_end==timing_end&&stream.timing==timing&&cancelled.stage(0)->snapshot.seed==seed);
    const auto cleared=stream.input;cancelled.cancel_pending_input();assert(stream.input==cleared);
    cancelled.consume_pending_input();cancelled.input.physical=32;cancelled.update();assert(cancelled.input.current==32&&word(cancelled,stream.input_cursor)==32);
    ReplayStream resumed_play;ReplayInputState resumed_input;resumed_input.flags=4;
    assert(resumed_play.begin_playback(stream.input.data(),stream.input_end,stream.timing.data(),stream.timing_end,false));
    assert(resumed_play.play(resumed_input)&&resumed_input.current==0);
    assert(resumed_play.play(resumed_input)&&resumed_input.current==0);
    assert(resumed_play.play(resumed_input)&&resumed_input.current==32);

    auto consumed=fresh();consumed.input.physical=128;consumed.update();const auto history=consumed.stage(0)->stream.input;
    consumed.consume_pending_input();consumed.cancel_pending_input();assert(consumed.stage(0)->stream.input==history);

    // A paused/non-consuming tick does not retire the pending serialized sample.
    auto paused=fresh();paused.input.physical=128;paused.update();paused.input.flags=0;paused.update();
    paused.cancel_pending_input();assert(word(paused,2)==0&&paused.input.current==0);

    // No receipt may be fabricated by a non-serializing record callback.
    for(int mode=0;mode<5;++mode){
        auto r=fresh();r.input.physical=128;r.update();r.consume_pending_input();
        auto& s=r.stage(0)->stream;
        if(mode==0)r.input.flags=0;
        if(mode==1)r.input.slow_mode=true;
        if(mode==2)r.input.speedhack=true;
        if(mode==3){r.input.flags=4|512;s.transition_frames=3;}
        if(mode==4){r.input.flags=4|512;s.transition_frames=0;for(int i=0;i<3;++i){r.update();r.consume_pending_input();}}
        const auto bytes=s.input;const auto count=s.frame;const auto at=s.input_cursor;r.input.physical=32;r.update();
        assert(s.frame==count&&s.input_cursor==at);r.cancel_pending_input();assert(s.input==bytes);
        assert(r.input.current==0&&r.input.physical==0);
    }
    // Stage/reset/stop boundaries retire old ownership, never rewrite another stage or stopped history.
    auto stopped=fresh();stopped.input.physical=128;stopped.update();stopped.stop();const auto stopped_bytes=stopped.stage(0)->stream.input;
    stopped.cancel_pending_input();assert(stopped.stage(0)->stream.input==stopped_bytes);
    auto next=fresh();next.input.physical=128;next.update();const auto old=next.stage(0)->stream.input;
    assert(next.begin(1,session,globals,0));next.cancel_pending_input();assert(next.stage(0)->stream.input==old);
    next.reset();next.cancel_pending_input();assert(next.current()==-1);
}
