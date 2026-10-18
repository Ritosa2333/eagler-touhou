#include "input/TouchController.hpp"
#include "input/MotionTrack.hpp"
#include <cassert>
#include <cmath>
#include <cstdio>
using touhou::input::TouchController;
using touhou::input::TouchState;
using touhou::input::TouchSample;
using touhou::input::MotionTrack;

static TouchState gameplay(){
    TouchState s; s.context=1; s.instance=7; s.ready=true; s.x=100; s.y=200;
    s.fast=4; s.slow=2; s.min_x=-184; s.max_x=184; s.min_y=32; s.max_y=432; return s;
}
static void no_motion(const TouchSample& s){assert(!s.motion);assert(!s.keys[16]);assert(!s.keys[88]);assert(!s.keys[27]);}

int main(){
    const auto state=gameplay(); TouchController touch; unsigned checks=0;
    touch.begin_session(); touch.set_mode(1); touch.controls(true,true,0,0,0,0);
    touch.pointer(0,1,.5f,.5f,100,state,false);
    auto sample=touch.sample(state,101,false,false);
    assert(sample.keys[90]&&sample.keys[16]&&sample.motion==1); ++checks;
    assert(!touch.set_mode(1)&&touch.active());
    touch.pointer(1,1,.55f,.5f,102,state,false);
    sample=touch.sample(state,103,false,false); assert(sample.motion==1&&sample.x>100); ++checks;
    touch.cancel_transient(); sample=touch.sample(state,104,false,false);
    assert(sample.keys[90]); no_motion(sample); ++checks;
    touch.controls(false,false,4,9,32767,0); sample=touch.sample(state,105,false,false);
    assert(sample.keys[88]&&sample.keys[27]); ++checks;
    touch.begin_session(); touch.controls(false,false,0,0,0,0); sample=touch.sample(state,106,false,false);
    no_motion(sample); ++checks;
    touch.controls(true,true,5,6,NAN,INFINITY); assert(touch.stick_x==0&&touch.stick_y==0);
    touch.reset(); sample=touch.sample(state,107,false,false); assert(sample.keys[90]); no_motion(sample); ++checks;
    touch.begin_session(); touch.set_mode(1); touch.pointer(0,2,.5f,.5f,200,state,false);
    assert(!touch.set_mode(1)&&touch.active()); assert(touch.set_mode(2)&&!touch.active()); ++checks;
    MotionTrack v1;v1.begin(0,true,false,true);v1.record(0,true,1,2);const auto old=v1.trailer(10);assert(!old.empty()&&MotionTrack::word(old.data()+old.size()-12)==1);
    MotionTrack loaded_v1;assert(loaded_v1.load(old.data(),old.size(),10));loaded_v1.begin(0,false,true,false);float mx=0,my=0;MotionTrack::TouchPoint points[4]{};assert(loaded_v1.playback(0,mx,my)&&loaded_v1.replay_points(points,4)==0);++checks;
    MotionTrack v2;v2.begin(0,true,false,true);v2.touch_event(0,0,11,.25f,.5f);v2.record(0,true,1,2);v2.touch_event(0,1,11,.3f,.55f);v2.record(0,true,3,4);v2.touch_event(0,2,11,.35f,.6f);v2.record(0,true,5,6);v2.touch_event(0,1,99,.9f,.9f);v2.record(0,true,7,8);v2.touch_event(0,0,22,.7f,.2f);v2.record(0,true,9,10);v2.touch_cancel(0);v2.record(0,true,11,12);
    const auto extended=v2.trailer(10);assert(!extended.empty()&&MotionTrack::word(extended.data()+extended.size()-12)==2);MotionTrack playback;assert(playback.load(extended.data(),extended.size(),10));playback.begin(0,false,true,false);
    assert(playback.playback(0,mx,my)&&playback.replay_points(points,4)==1&&std::abs(points[0].x-.25f)<1e-6f);assert(playback.playback(0,mx,my)&&playback.replay_points(points,4)==1&&std::abs(points[0].x-.3f)<1e-6f);assert(playback.playback(0,mx,my)&&playback.replay_points(points,4)==1&&std::abs(points[0].x-.35f)<1e-6f);assert(playback.playback(0,mx,my)&&playback.replay_points(points,4)==0);assert(playback.playback(0,mx,my)&&playback.replay_points(points,4)==1&&points[0].id==22);
    playback.begin(1,false,true,false);assert(playback.replay_points(points,4)==1);playback.begin(0,false,true,false);assert(playback.replay_points(points,4)==0);playback.begin(0,false,true,false);for(int i=0;i<6;i++)playback.playback(0,mx,my);assert(playback.replay_points(points,4)==0);++checks;
    MotionTrack carry;carry.begin(0,true,false,true);carry.touch_event(0,0,31,.4f,.6f);carry.record(0,true,1,1);carry.begin(1,false,false,true);carry.record(1,true,2,2);const auto carried=carry.trailer(10);MotionTrack direct;assert(direct.load(carried.data(),carried.size(),10));direct.begin(1,false,true,false);assert(direct.playback(1,mx,my)&&direct.replay_points(points,4)==1&&points[0].id==31&&std::abs(points[0].x-.4f)<1e-6f);++checks;
    MotionTrack touch_only;touch_only.begin(0,true,false,true);touch_only.touch_event(0,0,41,.2f,.8f);touch_only.record(0,false,0,0);touch_only.touch_event(0,2,41,.2f,.8f);touch_only.record(0,false,0,0);const auto touch_only_bytes=touch_only.trailer(10);assert(touch_only.used()&&!touch_only_bytes.empty()&&MotionTrack::word(touch_only_bytes.data()+touch_only_bytes.size()-12)==2);++checks;
    std::printf("TH10 touch lifecycle: %u checks passed; mode idempotent, transient input cleared, fire toggle preserved\n",checks);
}
