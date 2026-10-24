import test from 'node:test';
import assert from 'node:assert/strict';
import {distribution,holds,summarize,replayInputs} from './performance-browser-metrics.mjs';

const snapshot=(frame=10)=>({rollback:[1,0,frame,frame,frame-1,0,0,0,0,0,0,0,0,100,36864,200000,393216,0,0,0],
  net:[0,0,0,frame,frame-1,0,0,2],wasmBytes:512*1024*1024,nativePhase:1});
const row=(frame,entry,exit,extra={})=>({...snapshot(frame),native:true,entry,exit,observerMs:.2,...extra});
function fixture(rows=[]){return {start:0,end:1000,startSnapshot:snapshot(),rows,presentations:[],nativeTokens:[1],presentationsOutsideNativeCallback:0};}

test('unnotified correction callbacks remain in full callback wall distribution',()=>{
  const summary=summarize(fixture([row(11,20,30,{nativeMs:9,shellMs:.5}),row(8,40,240),row(12,300,310,{nativeMs:9,shellMs:.5})]));
  assert.equal(summary.callbackWall.max,200);assert.equal(summary.nativeReportedWall.max,9);
  assert.equal(summary.nativeNotificationCoverage.withoutNotification,1);
});
test('rewound simulation frames do not count as new progress',()=>{
  const summary=summarize(fixture([row(12,20,30),row(8,40,50),row(10,60,70),row(13,80,90)]));
  assert.equal(summary.forwardFrontierFrames,3);assert.equal(summary.logicHz,3);
  assert.equal(summary.longestFrontierHolds[0].begin,90);
});
test('correction already in progress at window start retains the warmup high frontier',()=>{
  const data=fixture([row(10,20,30),row(13,50,60)]);data.startSnapshot=snapshot(8);data.startSnapshot.highFrame=12;
  assert.equal(summarize(data).forwardFrontierFrames,1);
});
test('fixed end retains whole crossing callback timing without counting late progress',()=>{
  const data=fixture([row(12,20,30),row(30,990,1200)]);data.presentations=[25,1010];
  const summary=summarize(data);assert.equal(summary.completeNativeCallbacks,1);assert.equal(summary.crossingEndCallbacks,1);
  assert.equal(summary.presentWindowHolds.max,975);assert.equal(summary.presentationSubmissions,1);assert.equal(summary.logicHz,2);
  assert.equal(summary.callbackWall.max,210);assert.equal(summary.crossingCallbackWall.max,210);
});
test('one empty progress window remains a single full-duration hold',()=>{
  assert.deepEqual(holds([],100,1100),[{begin:100,end:1100,ms:1000,startBoundary:true,endBoundary:true}]);
  assert.equal(distribution([100,100.1]).over100,1);
});
test('confirmed Replay input accounting respects the measured frame range',()=>{
  const bytes=new Uint8Array(40+8+4*2*12),view=new DataView(bytes.buffer);bytes.set(new TextEncoder().encode('EAGLRPY1'));
  view.setUint32(8,1,true);bytes[20]=2;view.setUint32(28,1,true);view.setUint32(32,4,true);
  for(let frame=0;frame<4;++frame)for(let seat=0;seat<2;++seat)view.setUint16(48+(frame*2+seat)*12,1|(frame%2?64:128),true);
  const result=replayInputs(bytes,1,3);assert.equal(result.seats[0].frames,2);
  assert.equal(result.seats[1].fire,2);assert.equal(result.seats[0].left,1);assert.equal(result.seats[0].right,1);
  assert.throws(()=>replayInputs(bytes.subarray(0,bytes.length-1)),/bounds/);
});
