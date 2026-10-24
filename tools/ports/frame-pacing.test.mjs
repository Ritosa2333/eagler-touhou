import test from 'node:test';
import assert from 'node:assert/strict';
import {summarizeFramePacing} from './frame-pacing.mjs';

const samples=(count=601)=>Array.from({length:count},(_,i)=>({time:i*1000/60,ticks:i,processingMs:8}));
test('60 Hz requires both presentation and authoritative logic to keep pace',()=>{
  const steady=samples();assert.equal(summarizeFramePacing(steady).meets60Hz,true);
  assert.equal(summarizeFramePacing(steady.map(s=>({...s,ticks:Math.floor(s.ticks/2)}))).meets60Hz,false);
  assert.equal(summarizeFramePacing(steady.map(s=>({...s,time:s.time*2}))).meets60Hz,false);
});
test('deadline misses remain visible even with fast work on the other frames',()=>{
  const overloaded=samples().map((s,i)=>({...s,time:s.time+Math.floor(i/10)*1000/60,processingMs:i%10===0?24:4}));
  const result=summarizeFramePacing(overloaded);
  assert.equal(result.missed60HzSlots,60);assert.equal(result.overBudgetFrames,60);
  assert.equal(result.meets60Hz,false);
});
test('invalid or incomplete presentation evidence is rejected',()=>{
  assert.throws(()=>summarizeFramePacing([]));
  assert.throws(()=>summarizeFramePacing([{time:0},{time:0}]));
  assert.throws(()=>summarizeFramePacing(samples().map(s=>({...s,processingMs:NaN}))));
  assert.throws(()=>summarizeFramePacing(samples().map(s=>({...s,ticks:600-s.ticks}))));
});
test('60 Hz output on a 165 Hz display uses refresh quantization without enlarging the CPU budget',()=>{
  const refreshIntervalMs=1000/165;
  const quantized=samples().map(s=>({...s,time:Math.ceil(s.time/refreshIntervalMs)*refreshIntervalMs}));
  const result=summarizeFramePacing(quantized,{refreshIntervalMs});
  assert.equal(result.meets60Hz,true);assert.ok(result.intervalBudgetMs<19);
  assert.equal(summarizeFramePacing(quantized.map(s=>({...s,processingMs:18})),{refreshIntervalMs}).meets60Hz,false);
});
