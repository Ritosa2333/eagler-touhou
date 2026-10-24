import test from 'node:test';
import assert from 'node:assert/strict';
import {assessPerformance,diagnosticModes,progressEvents,windowHolds,timingTails} from './rollback-browser-metrics.mjs';

const metrics=(native=12,present=25,frontier=30)=>({nativeReportedWall:{max:native},presentWindowHolds:{max:present},frontierWindowHolds:{max:frontier}});
const peers=()=>[0,1].map(seat=>({seat,metrics:metrics()}));

test('each diagnostic mode rejects performance admission while retaining passing threshold checks',()=>{
  assert.equal(assessPerformance(peers(),{maxGapMs:100}).performancePassed,true);
  for(const mode of ['nativeProfile','audioProfile','cpuSamples','stationary']){
    const settings={maxGapMs:100,[mode]:true},result=assessPerformance(peers(),settings);
    assert.deepEqual(diagnosticModes(settings),[mode]);
    assert.equal(result.diagnosticOnly,true);assert.equal(result.thresholdsMet,true);
    assert.equal(result.performancePassed,false);assert.equal(result.failures.length,1);
    assert.match(result.failures[0].kind,/^diagnostic-.*-not-(?:movement-)?acceptance$/);
    assert(result.peers.every(peer=>peer.checks.every(check=>check.passed)));
  }
  const combined=assessPerformance(peers(),{maxGapMs:100,nativeProfile:true,audioProfile:true,cpuSamples:true});
  assert.equal(combined.performancePassed,false);assert.equal(combined.failures.length,3);
});

test('a diagnostic run retains every 100 ms violation instead of explaining it away',()=>{
  // Historical rounded extrema are synthetic regression inputs, not a new run
  // or a reconstruction of the lost jr10 raw receipt.
  const result=assessPerformance([{seat:0,metrics:metrics(57.3,291.7,291.7)},
    {seat:1,metrics:metrics(10.9,156,156)}],{maxGapMs:100,nativeProfile:true});
  assert.equal(result.thresholdsMet,false);assert.equal(result.performancePassed,false);
  assert.deepEqual(result.peers.map(peer=>peer.checks.map(check=>check.passed)),[[true,false,false],[true,false,false]]);
  assert(result.peers.every(peer=>peer.checks.every(check=>check.maximumMs===100)));
  assert.equal(result.failures.filter(failure=>failure.kind==='performance-threshold').length,2);
  assert.deepEqual(result.failures.filter(failure=>failure.kind==='performance-threshold').map(failure=>failure.presentMax),[291.7,156]);
});

test('finite values exactly at the unchanged threshold pass, missing or invalid timings do not',()=>{
  const atLimit=[0,1].map(seat=>({seat,metrics:metrics(100,100,100)}));
  assert.equal(assessPerformance(atLimit,{maxGapMs:100}).performancePassed,true);
  for(const value of [NaN,Infinity,undefined,-1]){
    const input=peers();input[1].metrics.presentWindowHolds.max=value;
    const result=assessPerformance(input,{maxGapMs:100});
    assert.equal(result.performancePassed,false);
    assert.deepEqual(result.failures[0].invalidMetrics,['presentWindowHolds.max']);
  }
  assert.equal(assessPerformance(peers().slice(0,1),{maxGapMs:100}).performancePassed,false);
});

function callback(id,entry,exit,nativeMs,presents,frame,extra={}){
  const rollback=Array(24).fill(0),net=Array(24).fill(0);rollback[21]=presents;net[3]=frame;
  return {id,entry,exit,requested:entry-5,stamp:entry-2,nativeMs,nativeTicks:1,
    shellHookMs:2,observerMs:1,rollback,net,...extra};
}
function timeline(){return [
  callback(1,2,10,5,7,20),
  callback(2,18,30,8,8,21),
  callback(3,35,40,2,8,19,{shellHookMs:1}),
  callback(4,70,80,6,9,21,{requested:40,stamp:48}),
  callback(5,92,110,10,10,22),
];}

test('presentation/frontier gaps keep the original counter, rewind and window-edge semantics',()=>{
  const rows=timeline(),events=progressEvents(rows);
  assert.deepEqual(events.presents.map(event=>event.at),[30,80,110]);
  assert.deepEqual(events.frontiers.map(event=>event.at),[30,110]);
  assert.deepEqual(windowHolds(events.presents,0,100).map(span=>span.durationMs),[30,50,20]);
  assert.deepEqual(windowHolds(events.frontiers,0,100).map(span=>span.durationMs),[30,70]);
  const tail=timingTails(rows,{start:0,end:100,thresholdMs:45,limit:1});
  const gap=tail.presentation.worst[0];
  assert.equal(tail.presentation.overThreshold,1);assert.equal(gap.begin,30);assert.equal(gap.end,80);
  assert.equal(gap.fromEvent.id,2);assert.equal(gap.toEvent.id,4);
  assert.deepEqual(gap.callbacks.map(row=>row.id),[3,4]);
  assert.equal(gap.insideObservedCallbacksMs,15);assert.equal(gap.outsideObservedCallbacksMs,35);
  assert.deepEqual(gap.completeCallbacks,{count:2,wallMs:15,nativeReportedMs:8,shellHookMs:3,observerMs:2,otherCallbackMs:2});
  const last=gap.callbacks[1];assert.equal(last.previousCallback.id,3);
  assert.equal(last.previousExitToEntryMs,30);assert.equal(last.requestToEntryMs,30);assert.equal(last.rafTimestampToEntryMs,22);
  assert.equal(tail.observationalOnly,true);assert.match(tail.meaning.presentation,/not GPU completion/);
});

test('a callback crossing the end contributes only its measured overlap to a gap partition',()=>{
  const result=timingTails(timeline(),{start:0,end:100,thresholdMs:100});
  const edge=result.presentation.worst.find(span=>span.endBoundary);
  assert.equal(edge.durationMs,20);assert.equal(edge.insideObservedCallbacksMs,8);assert.equal(edge.outsideObservedCallbacksMs,12);
  assert.equal(edge.completeCallbacks.nativeReportedMs,0);assert.equal(edge.clippedCallbackOverlapMs,8);
  assert.equal(edge.callbacks[0].nativeReportedMs,10);assert.equal(edge.callbacks[0].clipped,true);
  assert.equal(result.nativeCallbacks.worst[0].nativeReportedMs,10,'the entry-selected callback metric remains unchanged');
  assert.match(result.meaning.edges,/unrecognized first presentation/);
});

test('transport timer evidence is joined to the same local gap, including delivery after its end',()=>{
  const transport=[
    {id:1,kind:'ws',phase:'measure',scheduled:35,due:45,actual:75,lateness:30,requestedDelay:10},
    {id:2,kind:'ws',phase:'measure',scheduled:85,due:95,actual:105,lateness:10,requestedDelay:10},
    {id:3,kind:'ws',phase:'idle',scheduled:35,due:45,actual:75,lateness:999},
    {kind:'receive',phase:'measure',at:72},{kind:'receive',phase:'measure',at:10},
  ];
  const result=timingTails(timeline(),{start:0,end:100,transport,thresholdMs:45});
  const middle=result.presentation.worst.find(span=>span.begin===30),edge=result.presentation.worst.find(span=>span.endBoundary);
  assert.equal(middle.transport.maxObservedLatenessMs,30);assert.equal(middle.transport.receives.count,1);
  assert.deepEqual(middle.transport.worstObservedTimers.map(timer=>timer.id),[1]);
  assert.equal(edge.transport.pendingAtGapEnd,1);assert.equal(edge.transport.dueWithinGap,1);
  assert.equal(edge.transport.worstObservedTimers[0].actual,105);assert.equal(edge.transport.worstObservedTimers[0].deliveredWithinGap,false);
  assert.match(result.meaning.timers,/does not establish causality/);
});

test('counter jumps count one observed submission and never synthesize invisible GPU events',()=>{
  const rows=[callback(1,1,10,4,1,10),callback(2,15,30,8,4,13)];
  const events=progressEvents(rows);assert.equal(events.presents.length,1);assert.equal(events.frontiers.length,1);
  assert.deepEqual(events.presents[0],{at:30,index:1,id:2,before:1,after:4});
  assert.deepEqual(windowHolds(events.presents,0,40).map(span=>span.durationMs),[30,10]);
});
