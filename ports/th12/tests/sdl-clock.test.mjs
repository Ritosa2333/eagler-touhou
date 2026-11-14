import test from 'node:test';
import assert from 'node:assert/strict';
import {resolve} from 'node:path';
import {runNativeCpp} from '../../../launcher/tests/support/native-cpp.mjs';

test('SDL clock keeps 60 logical ticks under 144 Hz presentation and rebases fullscreen',async()=>{
  const result=await runNativeCpp(`
    #include "FrameClock.hpp"
    #include <cassert>
    #include <cstdio>
    using th12host::FrameClock;
    double credited=0;
    double pace(double elapsed){credited+=elapsed;return elapsed*.98;}
    int main(){
      FrameClock c;unsigned ticks=0;c.advance(0);
      for(unsigned i=1;i<=144;++i)ticks+=c.advance(i/144.);
      assert(ticks==60);
      c.reset();assert(c.advance(10)==0);assert(c.advance(10+1./60)==1);
      c.reset();c.advance(0);assert(c.advance(2)==4);
      c.reset();credited=0;c.advance(0);ticks=0;
      for(unsigned i=1;i<=600;++i)ticks+=c.advance(i/60.,1./60.,pace);
      assert(ticks==588);assert(credited>9.999&&credited<10.001);
      c.reset();assert(c.present(0,true));assert(!c.present(1./144,true));
      assert(c.present(2./144,false));
      c.reset();c.advance(0);c.present(0,true);ticks=0;unsigned presented=0;
      for(unsigned i=1;i<=144;++i){
        ticks+=c.advance(i/144.,1./120.);
        presented+=c.present(i/144.,true);
      }
      assert(ticks==120&&presented==60); // accelerated logic obeys the display limit
      for(unsigned hz:{60u,90u,120u,144u}){
        c.reset();assert(c.advance(0)==0);assert(!c.present_after_tick(0,0));
        unsigned fresh=0;
        for(unsigned i=1;i<=hz;++i){
          const double now=i/double(hz);
          fresh+=c.present_after_tick(now,c.advance(now));
        }
        assert(fresh==60);
        assert(!c.present(1.005,true)); // returning to a menu respects the last draw
        assert(c.present(1.+1./60,true));
      }
      c.reset();c.advance(0);assert(c.present(1./120,true));
      assert(c.present_after_tick(1./60,c.advance(1./60))); // old deadline would skip this tick
      assert(!c.present_after_tick(1./40,c.advance(1./40)));
      assert(c.present_after_tick(.1,c.advance(.1))); // a catch-up batch only draws once
      c.reset();assert(c.advance(0)==0);assert(c.present(0,true));
      for(unsigned i=1;i<=180;++i){
        const double deadline=i*FrameClock::interval;
        const double early=deadline-.002,callback=deadline+(i%2?-.0008:.0003);
        assert(c.advance(early)==0);assert(!c.present(early,true));
        assert(c.advance(callback)==1);assert(c.present(callback,true));
      }
      th12host::PresentationCounter pictures;pictures.frame(0,1,0,true);
      for(unsigned i=1;i<=120;++i)pictures.frame(i*1000./120,1,i/2.,true);
      assert(pictures.value()>119&&pictures.value()<121);
      for(unsigned i=121;i<=240;++i)pictures.frame(i*1000./120,1,60,true);
      assert(pictures.value()<1); // stalled world's redraws are not pictures
      for(unsigned i=241;i<=360;++i)pictures.frame(i*1000./120,1,55,true);
      assert(pictures.value()==0); // a rollback cannot inflate FPS
      std::puts("SDL clock: logical rate, viewport reset, bounded debt and actual-elapsed pacing passed");
    }
  `,{includes:[resolve(import.meta.dirname,'../portable/sdl')]});
  assert.match(result,/passed/);
});
