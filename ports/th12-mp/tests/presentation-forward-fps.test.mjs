import test from 'node:test';
import assert from 'node:assert/strict';
import {createPresentationFrameRateMeter} from '../src/fps-presentation.mjs';
test('forward subframes count actual new visible time without counting stalled redraws',()=>{
 const meter=createPresentationFrameRateMeter();meter.reset(0);
 const endpoint=frame=>({stage:1,epoch:1,frame,nativeMotion:{}});
 for(let n=1;n<=120;n++){
  const frame=Math.floor(n/2),phase=n%2?.5:0;
  meter.frame(n*1000/120,endpoint(frame-1),endpoint(frame),1+phase,true);
 }
 assert.equal(meter.value,120);
 for(let n=1;n<=120;n++)meter.frame(1000+n*1000/120,endpoint(60),endpoint(60),1.5,true);
 assert.equal(meter.value,0,'redrawing an unchanged world must not fabricate FPS');
});
