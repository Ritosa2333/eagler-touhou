import test from 'node:test';import assert from 'node:assert/strict';
import {FrameClock,keyboardBits} from '../../sdl-runtime/frame-clock.mjs';
test('TH11 browser clock preserves 60 Hz simulation across refresh rates and stalls',()=>{
 for(const hz of [30,50,60,75,90,120,144,165,240]){
  const clock=new FrameClock();clock.reset(0);let count=0;
  for(let i=1;i<=hz*10;++i)count+=clock.advance(i*1000/hz);
  assert.equal(count,600,`${hz} Hz`);
 }
 const clock=new FrameClock();clock.reset(0);assert.equal(clock.advance(10000),4);assert.equal(clock.advance(10000),0);clock.reset(20000);assert.equal(clock.advance(20000),0);
});
test('TH11 keyboard separates pause from dialogue skip and supports both modifier keys',()=>{
 assert.equal(keyboardBits(new Set(['Escape'])),0);
 assert.equal(keyboardBits(new Set(['ControlLeft'])),0x200);assert.equal(keyboardBits(new Set(['ControlRight'])),0x200);
 assert.equal(keyboardBits(new Set(['ShiftRight','ArrowRight','KeyZ'])),0x89);
 assert.equal(keyboardBits(new Set(['KeyC'])),0x400);
 assert.equal(keyboardBits(new Set(['Enter','NumpadEnter'])),0x100);
});
