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
