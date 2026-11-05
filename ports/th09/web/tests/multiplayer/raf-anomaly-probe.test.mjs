import test from 'node:test';
import assert from 'node:assert/strict';
import vm from 'node:vm';
import {readFileSync} from 'node:fs';
import {installRafAnomalyProbe} from './raf-anomaly-probe.mjs';

function fixture(){
  let time=120,peekCalls=0;
  const requests=[{index:0,nativeId:100,callbackId:0,parentIndex:null,requestedAt:100,enteredAt:100,exitedAt:110,
    cancelledAt:null,frameCalls:1,lastFrameIndex:0,pauseAtEntry:0,pauseAtExit:0},
    {index:1,nativeId:101,callbackId:0,parentIndex:0,requestedAt:110,enteredAt:null,exitedAt:null,
      cancelledAt:null,frameCalls:0,lastFrameIndex:null,pauseAtEntry:null,pauseAtExit:null}];
  const frames=[{activeAfter:true,endedAfter:false,sampleAfter:1}];
  const controls=[{name:'_th09_loop_start',enteredAt:0,argument:null},{name:'_th09_loop_pause',enteredAt:1,argument:0}];
  const d={requests,frames,controls,callbacks:[{id:0,game:true}],valid:true,lastPauseRequested:0,
    counts:{requested:2,entered:1,exited:1,cancelled:0,gameFrames:1}};
  const context=vm.createContext({Map,Date:{now:()=>1700000000000+time},performance:{now:()=>time,timeOrigin:1700000000000},
    document:{visibilityState:'visible'},core:{},play:{active:true,connected:true,finished:false,disconnected:false},
    measure:{samples:[[10,1,1,10,7,2,6,10,0,0,0,10,1,103]],started:100,ended:0,finished:false,closed:[]},
    gameLifecycle:[[0,'init','visible',true]],
    __eaglerPeerTransport:{route:'relay',received:[],receivedHead:0,relay:{bufferedAmount:0},pendingSignals:[],peers:new Map()},
    rafDiagnosticProbe:{peek(){++peekCalls;const pending=requests.filter(r=>r.enteredAt===null&&r.cancelledAt===null);
      return {...d,snapshotAt:time,pendingCount:pending.length,pendingIndices:pending.slice(0,4).map(r=>r.index),pendingTruncated:pending.length>4};},
      snapshot(){throw Error('Full snapshot must not be used');}},
    setTimeout(){throw Error('No new timer');},setInterval(){throw Error('No new timer');},
    requestAnimationFrame(){throw Error('No new frame');}});
  vm.runInContext('('+installRafAnomalyProbe.toString()+')({enabled:true,side:1})',context);
  const api=context.rafAnomalyProbe;
  return {context,d,requests,frames,controls,api,get peekCalls(){return peekCalls;},
    at(t){time=t;},poll(t,stats){time=t;return api.observe(stats?{stats}:{});},
    enter(index,enteredAt,exitedAt=enteredAt+2){Object.assign(requests[index],{enteredAt,exitedAt,frameCalls:1,pauseAtEntry:0,pauseAtExit:0});},
    pause(t,arg){controls.push({name:'_th09_loop_pause',enteredAt:t,argument:arg});d.lastPauseRequested=arg;}};
}
const plain=value=>JSON.parse(JSON.stringify(value));

test('actual harness peek inspects only four pending entries and leaves full snapshot behavior intact',()=>{
  const source=readFileSync(new URL('./smoothness-browser.mjs',import.meta.url),'utf8');
  const begin=source.indexOf('const installSmoothnessHooks=');
  const end=source.indexOf('const hookOptions=',begin);
  assert.ok(begin>=0&&end>begin);
  let nextCalls=0,nativeId=0;
  class CountedMap extends Map{
    values(){const it=super.values();return {[Symbol.iterator](){return this;},next(){++nextCalls;return it.next();}};}
  }
  const context=vm.createContext({Map:CountedMap,performance:{now:()=>100},
    document:{visibilityState:'visible',hasFocus:()=>true,addEventListener(){}},addEventListener(){},
    requestAnimationFrame(){return ++nativeId;},cancelAnimationFrame(){},WebSocket:class{send(){}},
    setTimeout(){throw Error('No timer required');}});
  vm.runInContext('globalThis.window=globalThis;'+source.slice(begin,end)+
    'installSmoothnessHooks({fallback:true,rafDiagnostics:true,side:0,networkTrace:false,impairmentSite:"none",headedControl:false});',context);
  for(let i=0;i<100;++i)vm.runInContext('requestAnimationFrame(()=>{})',context);
  nextCalls=0;const peek=context.rafDiagnosticProbe.peek();
  assert.equal(peek.pendingCount,100);assert.equal(peek.pendingIndices.length,4);
  assert.equal(peek.pendingTruncated,true);assert.equal(nextCalls,4);
  const full=context.rafDiagnosticProbe.snapshot();assert.equal(full.pendingIndices.length,100);
  assert.equal(full.requests,peek.requests);assert.equal(full.frames,peek.frames);
});

test('disabled is inert without a page, and installation is one owned global',()=>{
  assert.deepEqual(installRafAnomalyProbe(),{enabled:false,installed:false,reason:'disabled'});
  const f=fixture();assert.equal(f.peekCalls,0);
  const second=vm.runInContext('('+installRafAnomalyProbe.toString()+')({enabled:true})',f.context);
  assert.equal(second.reason,'install-conflict');assert.equal(f.peekCalls,0);
});

test('identical pending request plus independent queue and polled-stat progress is retained',()=>{
  const f=fixture();f.poll(120,[10,7,2,6,10,0,0,0]);
  Object.assign(f.context.__eaglerPeerTransport,{received:[new Uint8Array(40),new Uint8Array(40)],receivedHead:1});
  const status=f.poll(410,[10,8,2,6,10,0,0,0]);assert.equal(status.episode.requestIndex,1);
  assert.equal(status.episode.outcome,'pending');
  const s=plain(f.api.snapshot());assert.equal(s.episode.trigger.requestAgeMs,300);
  assert.equal(s.history.pre[0].game.polledStats[1],7);
  assert.equal(s.history.post[0].game.polledStats[1],8);
  assert.equal(s.history.post[0].game.sampleCount,1);
  assert.equal(s.history.post[0].transport.receivedHead,1);
  assert.equal(s.history.post[0].transport.queuedPackets,1);
  assert.equal(s.history.post[0].pending[0].index,s.history.pre[0].pending[0].index);
  assert.equal(s.clocks.page,'Page performance.now() with performance.timeOrigin');
});

test('completed wait between polls is found through the previous pending identity',()=>{
  const f=fixture();f.poll(120);f.enter(1,470);f.poll(500);
  const s=plain(f.api.snapshot());assert.equal(s.episode.kind,'completed-before-poll');
  assert.equal(s.episode.outcome,'entered');assert.equal(s.episode.waitMs,360);
  assert.equal(s.episode.enteredAt,470);assert.equal(s.episode.detectedAt,500);
});

test('native id reuse cannot turn cancellation of the latched request into recovery',()=>{
  const f=fixture();f.poll(400);f.requests[1].cancelledAt=420;
  f.requests.push({...f.requests[1],index:2,nativeId:101,requestedAt:430,enteredAt:460,exitedAt:462,cancelledAt:null});
  f.poll(500);const s=plain(f.api.snapshot());
  assert.equal(s.episode.requestIndex,1);assert.equal(s.episode.outcome,'cancelled');
  assert.equal(s.episode.censored,true);assert.equal(s.episode.enteredAt,null);
});

test('final pending survives pause and is censored instead of fabricated recovery',()=>{
  const f=fixture();f.poll(120);f.poll(400);f.pause(430,1);f.context.measure.ended=440;f.at(500);
  const s=plain(f.api.snapshot({final:true}));assert.equal(s.episode.outcome,'paused');
  assert.equal(s.episode.censored,true);assert.equal(s.episode.waitMs,null);
  assert.equal(s.finalObservation.pending[0].index,1);
  assert.equal(s.finalObservation.pending[0].enteredAt,null);
  assert.ok(s.finalObservation.pending[0].reasons.includes('harness-inactive'));
  const calls=f.peekCalls;f.api.observe();f.api.snapshot({final:true});assert.equal(f.peekCalls,calls);
});

test('pause after natural callback entry does not censor its completed wait',()=>{
  const f=fixture();f.poll(400);f.enter(1,480);f.pause(490,1);f.at(500);
  const s=plain(f.api.snapshot({final:true}));assert.equal(s.episode.outcome,'entered');
  assert.equal(s.episode.waitMs,370);assert.equal(s.episode.censored,false);
});

test('startup, paused chain and hidden interval do not trigger an active anomaly',()=>{
  for(const change of [f=>{f.frames[0].sampleAfter=0;},f=>f.pause(200,1),
    f=>{f.pause(200,1);f.pause(300,0);},
    f=>{f.context.gameLifecycle.push([200,'visibilitychange','hidden',false],[300,'visibilitychange','visible',true]);}]){
    const f=fixture();change(f);f.poll(500);const s=plain(f.api.snapshot());assert.equal(s.episode,null);
    assert.equal(s.finalObservation.pending[0].eligible,false);
  }
});

test('time inside an unfinished parent is separated from eligible dispatch wait',()=>{
  const f=fixture();f.requests[0].exitedAt=400;f.poll(600);
  assert.equal(f.api.snapshot().episode,null);
  f.poll(660);const s=plain(f.api.snapshot());
  assert.equal(s.episode.trigger.requestAgeMs,550);assert.equal(s.episode.trigger.eligibleWaitMs,260);
  assert.equal(s.episode.trigger.parentExitedAt,400);
});

test('loop restart and owner replacement preserve the censored first episode',()=>{
  const f=fixture();f.poll(400);f.controls.push({name:'_th09_loop_start',enteredAt:420});f.poll(450);
  assert.equal(f.api.snapshot().episode.outcome,'restarted');
  const g=fixture();g.poll(400);g.context.measure={...g.context.measure};g.poll(500);
  const s=plain(g.api.snapshot({final:true}));assert.equal(s.valid,false);
  assert.equal(s.episode.outcome,'owner-changed');assert.equal(s.episode.censored,true);
});

test('one-shot history is bounded by actual polls and final state survives post-history cap',()=>{
  const f=fixture();
  for(let i=0;i<45;++i)f.poll(120+i);
  f.poll(400);f.enter(1,420);
  for(let i=0;i<60;++i)f.poll(430+i*100);
  const s=plain(f.api.snapshot({final:true}));
  assert.equal(s.history.pre.length,32);assert.equal(s.history.post.length,32);
  assert.ok(s.counts.preEvicted>0);assert.ok(s.counts.postOmitted>0);
  assert.equal(s.episode.requestIndex,1);assert.equal(s.episode.outcome,'entered');
  assert.ok(s.finalObservation.clock.observedAt>s.history.post.at(-1).clock.observedAt);
  assert.ok(Buffer.byteLength(JSON.stringify(s))<=s.limits.retainedBytes);
});

test('request scan and pending limits report incomplete coverage without using poll gaps as evidence',()=>{
  const f=fixture();f.enter(1,125);f.poll(130);
  for(let i=2;i<100;++i)f.requests.push({...f.requests[1],index:i,nativeId:i+100,requestedAt:200+i,enteredAt:201+i,exitedAt:202+i});
  f.poll(10000);let s=plain(f.api.snapshot());assert.equal(s.episode,null);
  assert.ok(s.counts.maximumPollGapMs>9000);assert.ok(s.counts.requestsSkipped>0);assert.equal(s.coverageComplete,false);
  const g=fixture();for(let i=2;i<10;++i)g.requests.push({...g.requests[1],index:i,nativeId:i+100});
  g.poll(150);s=plain(g.api.snapshot());assert.equal(s.finalObservation.pending.length,4);
  assert.equal(s.finalObservation.raf.pendingCount,9);assert.equal(s.coverageComplete,false);
});

test('only compact allowlisted metadata leaves the page; full arrays and payload are not serialized',()=>{
  const f=fixture();
  for(const array of [f.requests,f.frames,f.controls,f.d.callbacks])Object.defineProperty(array,'toJSON',{value(){throw Error('raw array serialized');}});
  const token='secret.invalid/?token=private';
  f.context.__eaglerPeerTransport.relay.url=token;
  f.context.__eaglerPeerTransport.received=[new TextEncoder().encode(token)];
  f.context.__eaglerPeerTransport.pendingSignals=[token];
  f.context.__eaglerPeerTransport.peers.set(1,{pc:{connectionState:'connected',iceConnectionState:'connected',localDescription:{sdp:token}},
    inputDc:{bufferedAmount:68},controlDc:{bufferedAmount:0},lastReceivedAt:1700000000020});
  f.poll(400);const s=plain(f.api.snapshot({final:true}));
  assert.equal(JSON.stringify(s).includes(token),false);assert.equal(s.finalObservation.transport.peers[0].inputBufferedBytes,68);
  assert.equal(s.finalObservation.transport.queuedPackets,1);assert.equal(s.finalObservation.transport.pendingSignalCount,1);
});
