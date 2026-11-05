import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readFileSync} from 'node:fs';
import test from 'node:test';
import vm from 'node:vm';
import {SMOOTHNESS_WINDOW_SCHEMA,summarizeSmoothnessWindow} from './smoothness-window.mjs';

const fixture=rows=>({measure:{samples:rows.map((r,i)=>[
 r.time??i*10,r.ok??1,r.work??1,r.next,0,r.corrections??0,r.resimulated??0,r.captures??r.frontier,0,0,0,r.present??r.frontier,r.phase??1,1000+(r.time??i*10)
]),networkPacing:rows.map((r,i)=>[i,1,0,1000,1000,r.frontier,r.frontier])},impairment:{planned:[39,40],delivered:[41,42]}});
const run=(result,startFrame=2,endFrame=5)=>summarizeSmoothnessWindow(result,{startFrame,endFrame});
const row=(next,extra={})=>({next,frontier:next,...extra});
const approx=(actual,expected)=>assert.ok(Math.abs(actual-expected)<1e-7,`${actual} != ${expected}`);

test('tail rollback cannot re-enter the continuous body or manufacture a callback gap',()=>{
 const r=fixture([row(0),row(1),row(2),row(3),row(4),row(5),row(6),row(4,{frontier:6,present:6}),row(6)]);
 const before=JSON.stringify(r),s=run(r);
 assert.equal(s.window.schema,SMOOTHNESS_WINDOW_SCHEMA);assert.equal(s.window.complete,true);
 assert.deepEqual(s.window.bodyRange,[2,5]);assert.equal(s.callbacks,3);
 assert.equal(s.rafGapMs.max,10);assert.equal(s.effectivePresentationGapMs.max,10);
 assert.equal(s.window.formalWithEndBoundary.newStates.rafTimestampMs.count,3);
 assert.equal(s.window.formalWithEndBoundary.newStates.rafTimestampMs.max,10);
 assert.equal(s.window.formalWithEndBoundary.endMarkerHasNewState,true);
 assert.equal(s.firstFrame,2);assert.equal(s.lastFrame,5);assert.equal(s.elapsedMs,30);assert.equal(s.logicHz,100);
 assert.deepEqual(s.window.segments.tail.range,[5,9]);assert.equal(s.window.segments.allObserved.callbacks,9);
 const legacy=r.measure.samples.filter(x=>x[3]>=2&&x[3]<5);
 assert.equal(legacy.at(-1)[13]-legacy.at(-2)[13],30,'The old splice is present in this regression fixture');
 assert.equal(JSON.stringify(r),before,'All samples and trace rows remain byte-equivalent');
});

test('rollback below the lower frame bound and non-Match phases retain their real work and long gap',()=>{
 const r=fixture([row(0),row(1),row(2),row(3),row(1,{frontier:3,present:3,phase:3,work:70,ok:2}),
  row(2,{frontier:3,present:3,phase:0,work:20,ok:2}),row(3,{frontier:3,present:3}),row(4,{time:150}),row(5,{time:160})]);
 const s=run(r);
 assert.deepEqual(s.window.bodyRange,[2,8]);assert.equal(s.callbacks,6);assert.equal(s.waitCallbacks,2);
 assert.deepEqual(s.sessionPhases,[1,3,0]);assert.deepEqual(s.scenes,s.sessionPhases);
 assert.equal(s.callbackWorkMs.max,70);assert.equal(s.effectivePresentationGapMs.max,120);
 assert.equal(s.effectivePresentationGapMs.over100ms,1);
 assert.equal(s.window.segments.allObserved.callbackWorkMs.total,
  s.window.segments.startup.callbackWorkMs.total+s.window.segments.body.callbackWorkMs.total+s.window.segments.tail.callbackWorkMs.total);
});

test('capture progress never selects a boundary and native frontier may exceed the current replay cursor',()=>{
 const r=fixture([row(0,{captures:1}),row(1,{captures:2}),row(1,{frontier:2,captures:3,present:1}),
  row(2,{captures:3}),row(3,{captures:4}),row(4,{captures:5}),row(5,{captures:6})]);
 const s=run(r);
 assert.equal(s.window.startSampleIndex,2);assert.equal(s.window.startMarker.sample[3],1);
 assert.equal(s.window.startMarker.predictionFrontier,2);assert.equal(s.window.startMarker.sample[7],3);
 assert.equal(s.window.endSampleIndexExclusive,6);assert.equal(s.window.nativeMatchesObservedNextHighWater,false);
 assert.equal(s.captures,3);assert.equal(s.logicHz,75);
 assert.equal(s.window.progressSource,'native-networkPacing-PredictionFrontier');
});

test('endpoint counters close progress while endpoint work remains in the tail',()=>{
 const r=fixture([row(0),row(1),row(2,{work:7,corrections:3,resimulated:4,captures:3}),
  row(3,{corrections:4,resimulated:8,captures:4}),row(4,{corrections:4,resimulated:8,captures:5}),
  row(5,{work:999,corrections:8,resimulated:20,captures:6}),row(6,{corrections:8,resimulated:20,captures:7})]);
 const s=run(r);
 assert.equal(s.corrections,5);assert.equal(s.resimulated,16);assert.equal(s.captures,3);
 assert.equal(s.callbackWorkMs.total,9);assert.equal(s.callbackWorkMs.max,7);
 assert.equal(s.window.segments.tail.callbackWorkMs.total,1000);assert.equal(s.window.segments.allObserved.callbackWorkMs.max,999);
 assert.equal(s.window.bodyFirstToLast.deltas.corrections,1);
 assert.equal(s.window.crossingNewStateIntervals.end[0].toSample,5);
});

test('a missing terminal crossing is explicitly right-censored with no closed throughput or counter delta',()=>{
 const s=run(fixture([row(0),row(1),row(2),row(3),row(2,{frontier:3,present:3,work:80})]));
 assert.equal(s.window.complete,false);assert.ok(s.window.status.includes('end-not-observed'));
 assert.equal(s.logicHz,null);assert.equal(s.elapsedMs,null);assert.equal(s.corrections,null);assert.equal(s.lastFrame,null);
 assert.deepEqual(s.window.bodyRange,[2,5]);assert.equal(s.callbackWorkMs.max,80);
 assert.deepEqual(s.window.segments.tail.range,[5,5]);assert.equal(s.window.formalWithEndBoundary.available,false);
});

test('missing native F uses only labelled observed cursor high-water and never claims complete frontier evidence',()=>{
 const r=fixture([row(0,{captures:8}),row(1,{captures:9}),row(2,{captures:10}),row(3,{captures:11}),row(4,{captures:12}),row(5,{captures:13}),row(4,{frontier:5,present:5,captures:13})]);
 r.measure.networkPacing=[];
 const s=run(r);
 assert.equal(s.window.progressSource,'observed-next-high-water');assert.equal(s.window.nativeFrontierComplete,false);
 assert.equal(s.window.complete,false);assert.ok(s.window.status.includes('native-frontier-incomplete'));
 assert.equal(s.window.closedBoundaryAvailable,true);assert.deepEqual(s.window.bodyRange,[2,5]);assert.equal(s.logicHz,100);
 assert.equal(s.window.nativeFrontierMissingSampleIndices.length,7);
 r.measure.networkPacing=[[2,1,0,1000,1000,2,2]];
 const partial=run(r);assert.equal(partial.window.nativeFrontierComplete,false);assert.deepEqual(partial.window.bodyRange,[2,5]);
});

test('a counter reset or native frontier regression is retained as invalid epoch evidence',()=>{
 const r=fixture([row(0),row(1),row(2),row(3),row(2),row(4),row(5)]);
 const s=run(r);
 assert.equal(s.window.complete,false);assert.equal(s.window.epochValid,false);assert.equal(s.logicHz,null);
 assert.ok(s.window.epochErrors.some(e=>e.kind==='frontier-regressed'));
 assert.ok(s.window.epochErrors.some(e=>e.kind==='renderer-submissions-regressed'));
 assert.equal(s.window.segments.allObserved.callbacks,7);
});

test('no new renderer submission at the endpoint leaves observed new-state silence visible',()=>{
 const r=fixture([row(0),row(1),row(2),row(3,{present:2,time:30}),row(4,{present:2,time:130}),row(5,{present:2,time:230})]);
 const s=run(r);
 assert.equal(s.window.formalWithEndBoundary.endMarkerHasNewState,false);
 assert.equal(s.window.formalWithEndBoundary.newStates.rafTimestampMs.count,0);
 assert.equal(s.window.formalWithEndBoundary.newStates.rafTimestampMs.max,null);
 assert.equal(s.window.formalWithEndBoundary.observedWithoutNewStateToEnd.rafTimestampMs,210);
 assert.equal(s.rafGapMs.max,100);
});

test('unreached start, empty samples and malformed input never fabricate a green formal window',()=>{
 for(const r of [fixture([]),fixture([row(0),row(1)])]){
  const s=run(r);assert.equal(s.window.complete,false);assert.ok(s.window.status.includes('start-not-observed'));
  assert.equal(s.logicHz,null);assert.equal(s.callbacks,0);assert.equal(s.window.segments.startup.callbacks,r.measure.samples.length);
 }
 const malformed=fixture([row(0)]);malformed.measure.samples[0][2]=NaN;
 assert.throws(()=>run(malformed),/Non-finite/);
 assert.throws(()=>run(fixture([]),5,5),/Invalid formal/);
});

const rawPath=process.env.TH09_SMOOTHNESS_WINDOW_RAW;
test('frozen relay125 regression: corrected window agrees with independent evidence and legacy summary stays exact',{skip:!rawPath},t=>{
 const bytes=readFileSync(rawPath),sha=createHash('sha256').update(bytes).digest('hex');
 assert.equal(sha,'d03833c48742160afa7956a70b4aa82f135069005e3b4b38422a6b3c08bca770');
 const raw=JSON.parse(bytes),source=readFileSync(new URL('./smoothness-browser.mjs',import.meta.url),'utf8');
 const legacyStart=source.indexOf(' const legacySummary=results.map(r=>{'),legacyEnd=source.indexOf('\n const summary=results.map(r=>summarizeSmoothnessWindow',legacyStart);
 const distStart=source.indexOf('const distribution=values=>'),distEnd=source.indexOf('\n',distStart);
 assert.ok(legacyStart>=0&&legacyEnd>legacyStart&&distStart>=0&&distEnd>distStart);
 const legacy=JSON.parse(vm.runInNewContext(source.slice(distStart,distEnd)+'\n'+source.slice(legacyStart,legacyEnd)+'\nJSON.stringify(legacySummary);',
  {results:structuredClone(raw.results),limit:1800},{timeout:5000}));
 assert.deepEqual(legacy,raw.summary,'The exact old summary remains available without mutating frozen raw bytes');
 const expected=[{start:573,end:5745,hz:40.608460121,yieldMax:25.795,newMax:41.135,workMax:4.175,corr:292,resim:1764},
  {start:561,end:5729,hz:40.615851640,yieldMax:25.105,newMax:44.695,workMax:14.190,corr:299,resim:1740}];
 for(let side=0;side<2;++side){
  const r=raw.results[side],before=JSON.stringify(r),s=run(r,180,1800),e=expected[side];
  assert.equal(s.window.complete,true);assert.equal(s.window.nativeMatchesObservedNextHighWater,true);
  assert.deepEqual(s.window.bodyRange,[e.start,e.end]);assert.equal(s.callbacks,e.end-e.start);
  approx(s.logicHz,e.hz);approx(s.rafGapMs.max,e.yieldMax);approx(s.callbackWorkMs.max,e.workMax);
  approx(s.window.formalWithEndBoundary.newStates.rafTimestampMs.max,e.newMax);
  assert.equal(s.corrections,e.corr);assert.equal(s.resimulated,e.resim);assert.equal(s.captures,1620);
  assert.equal(s.window.formalWithEndBoundary.newStates.rafTimestampMs.count,1620);
  assert.equal(s.window.formalWithEndBoundary.endMarkerHasNewState,true);
  assert.equal(s.window.segments.tail.callbacks,20);assert.equal(JSON.stringify(r),before);
  t.diagnostic(JSON.stringify({side,rawSha256:sha,start:e.start,end:e.end,logicHz:s.logicHz,bodyRafGapMax:s.rafGapMs.max,
   newStateGapMax:s.window.formalWithEndBoundary.newStates.rafTimestampMs.max,legacyRafGapMax:legacy[side].rafGapMs.max,
   callbackMax:s.callbackWorkMs.max,corrections:s.corrections,resimulated:s.resimulated,tailCallbacks:s.window.segments.tail.callbacks}));
 }
 approx(legacy[1].rafGapMs.max,83.735);approx(legacy[1].presentationGapMs.max,99.180);
 assert.equal(createHash('sha256').update(readFileSync(rawPath)).digest('hex'),sha);
});
