// Read-only historical field-shape replay, never a fresh Chrome/CDP reproduction.
// The frozen raw identity is mandatory; optional positional paths relocate only
// that report and select the helper source under examination. No files are written.
// Saved from the already-executed inline replay; see the adjacent docs evidence.
import fs from 'node:fs';
import vm from 'node:vm';
import crypto from 'node:crypto';
import assert from 'node:assert/strict';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
const rawPath=resolve(process.argv[2]??fileURLToPath(new URL('../../artifacts/multiplayer-tests/rollback4x-chain-release-input-batch-headed-relay39-01-report.json',import.meta.url)));
const helperPath=resolve(process.argv[3]??fileURLToPath(new URL('./raf-anomaly-probe.mjs',import.meta.url)));
const sha=b=>crypto.createHash("sha256").update(b).digest("hex");
try{
 assert.ok(process.argv.length<=4,'Usage: node raf-anomaly-historical-replay.mjs [frozen-relay39-report.json] [raf-anomaly-probe.mjs]');
 const rawBytes=fs.readFileSync(rawPath),rawSha=sha(rawBytes),helperBytes=fs.readFileSync(helperPath),helperSha=sha(helperBytes);
 assert.equal(rawSha,"92a6711ebc768d6da8249100d85afabe3187a52bb327715af066f0e3f448fb20");
 const raw=JSON.parse(rawBytes);assert.equal(raw.passed,true);
 const helperSource=helperBytes.toString("utf8").replace("export function installRafAnomalyProbe","function installRafAnomalyProbe");
 assert.notEqual(helperSource,helperBytes.toString("utf8"),"Helper export anchor missing");
 const finite=Number.isFinite,all=[];
 function model(side,index){
  const r=raw.results[side],d=r.rafDiagnostics,target=d.requests[index],parent=d.requests[target.parentIndex];
  assert.equal(target.index,index);assert.equal(parent.index,target.parentIndex);
  assert.equal(d.valid,true);assert.equal(r.measure.finished,false);assert.equal(r.measure.closed.length,0);
  const origins=raw.headedControl.windows.filter(w=>w.side===side).map(w=>w.page.timeOrigin);
  assert.ok(origins.length&&origins.every(v=>v===origins[0]));
  const origin=origins[0];let now=0,state;
  const context=vm.createContext({performance:{now:()=>now,timeOrigin:origin},Date:{now:()=>Math.floor(origin+now)},
   document:{visibilityState:"visible"},core:{},play:{active:false,connected:false,finished:false,disconnected:false},
   measure:{samples:[],started:0,ended:0,finished:false,closed:[]},gameLifecycle:[],
   rafDiagnosticProbe:{peek:()=>state,snapshot(){throw Error("Full snapshot is forbidden in modeled observe");}},
   setTimeout(){throw Error("No timers in offline model");},setInterval(){throw Error("No timers in offline model");},
   requestAnimationFrame(){throw Error("No RAF scheduling in offline model");},cancelAnimationFrame(){throw Error("No RAF cancellation in offline model");}});
  vm.runInContext(helperSource+"\ninstallRafAnomalyProbe({enabled:true,side:"+side+"});",context,{timeout:1000});
  function clip(at){
   now=at;
   const frames=d.frames.filter(f=>f.enteredAt<=at).map(f=>{
    const row={...f};
    if(!finite(row.exitedAt)||row.exitedAt>at){row.exitedAt=null;row.sampleAfter=null;row.activeAfter=null;row.endedAfter=null;}
    for(const key of ["playEnteredAt","playExitedAt","sampleAt","afterSampleAt"])if(finite(row[key])&&row[key]>at)row[key]=null;
    return row;
   });
   const requests=d.requests.filter(q=>q.requestedAt<=at).map(q=>{
    const row={...q};
    if(!finite(row.enteredAt)||row.enteredAt>at){row.enteredAt=null;row.rafTimestamp=null;row.pauseAtEntry=null;row.frameHookAtEntry=null;}
    if(!finite(row.exitedAt)||row.exitedAt>at){row.exitedAt=null;row.pauseAtExit=null;row.frameHookAtExit=null;row.callbackThrew=false;}
    if(finite(row.cancelledAt)&&row.cancelledAt>at)row.cancelledAt=null;
    const observedFrames=frames.filter(f=>f.rafIndex===row.index);
    row.frameCalls=observedFrames.length;row.firstFrameIndex=observedFrames[0]?.index??null;row.lastFrameIndex=observedFrames.at(-1)?.index??null;
    return row;
   });
   requests.forEach((q,i)=>assert.equal(q.index,i,"Request array identity must stay contiguous"));
   frames.forEach((f,i)=>assert.equal(f.index,i,"Frame array identity must stay contiguous"));
   const controls=d.controls.filter(c=>c.enteredAt<=at).map(c=>({...c,exitedAt:finite(c.exitedAt)&&c.exitedAt<=at?c.exitedAt:null}));
   const callbacks=d.callbacks.map(c=>({...c,game:frames.some(f=>requests[f.rafIndex]?.callbackId===c.id)}));
   const pending=requests.filter(q=>q.enteredAt===null&&q.cancelledAt===null&&!q.requestThrew);
   state={requests,frames,controls,callbacks,valid:true,snapshotAt:at,lastPauseRequested:controls.filter(c=>c.name==="_th09_loop_pause").at(-1)?.argument??null,
    counts:{},pendingCount:pending.length,pendingIndices:pending.slice(0,4).map(q=>q.index),pendingTruncated:pending.length>4};
   const observedSamples=r.measure.samples.filter(s=>r.measure.started+s[0]<=at).map(s=>s.slice());
   Object.assign(context.measure,{samples:observedSamples,started:at>=r.measure.started?r.measure.started:0,
    ended:r.measure.ended&&r.measure.ended<=at?r.measure.ended:0,finished:false,closed:[]});
   const latest=frames.filter(f=>finite(f.exitedAt)&&f.exitedAt<=at).at(-1);
   context.play.active=latest?.activeAfter===true;context.play.connected=context.play.active;
   context.gameLifecycle=r.lifecycle.filter(row=>row[0]<=at).map(row=>row.slice());
   context.document.visibilityState=context.gameLifecycle.at(-1)?.[2]??"visible";
  }
  const poll=at=>{clip(at);return vm.runInContext("rafAnomalyProbe.observe()",context,{timeout:1000});};
  const snapshot=()=>JSON.parse(vm.runInContext("JSON.stringify(rafAnomalyProbe.snapshot())",context,{timeout:1000}));
  return {r,d,target,parent,origin,poll,snapshot,state:()=>state};
 }
 for(const [side,index] of [[0,704],[1,632]]){
  const f=model(side,index),q=f.target,preAt=Math.max(q.requestedAt+10,f.parent.exitedAt+1),duringAt=q.requestedAt+300,afterAt=q.exitedAt+1;
  assert.ok(preAt<duringAt&&duringAt<q.enteredAt);
  f.poll(preAt);assert.equal(f.snapshot().episode,null);
  const cut=f.state().requests[index];assert.equal(cut.enteredAt,null);assert.equal(cut.exitedAt,null);assert.equal(cut.frameCalls,0);
  const pending=f.poll(duringAt),pendingSnapshot=f.snapshot();
  assert.equal(pending.episode.requestIndex,index);assert.equal(pending.episode.outcome,"pending");
  assert.equal(pendingSnapshot.episode.trigger.eligible,true);assert.equal(pendingSnapshot.episode.trigger.enteredAt,null);
  assert.ok(pendingSnapshot.episode.trigger.eligibleWaitMs>=250);
  f.poll(afterAt);const settled=f.snapshot();
  assert.equal(settled.episode.outcome,"entered");assert.equal(settled.episode.censored,false);
  assert.equal(settled.episode.enteredAt,q.enteredAt);assert.equal(settled.episode.waitMs,q.enteredAt-q.requestedAt);
  const between=model(side,index);between.poll(preAt);between.poll(afterAt);const completed=between.snapshot();
  assert.equal(completed.episode.kind,"completed-before-poll");assert.equal(completed.episode.requestIndex,index);
  assert.equal(completed.episode.outcome,"entered");assert.equal(completed.episode.waitMs,q.enteredAt-q.requestedAt);
  const pf=f.d.frames[f.parent.lastFrameIndex];
  all.push({side,requestIndex:index,parentIndex:q.parentIndex,parentFrameIndex:f.parent.lastFrameIndex,parentActiveAfter:pf.activeAfter,parentEndedAfter:pf.endedAfter,parentSampleAfter:pf.sampleAfter,
   originalRequest:{requestedAt:q.requestedAt,enteredAt:q.enteredAt,exitedAt:q.exitedAt,parentExitedAt:f.parent.exitedAt,cancelledAt:q.cancelledAt},
   observations:{preAt,duringAt,afterAt},pendingAt300:{kind:pendingSnapshot.episode.kind,epoch:pendingSnapshot.episode.observedLoopEpoch,
    eligible:pendingSnapshot.episode.trigger.eligible,requestAgeMs:pendingSnapshot.episode.trigger.requestAgeMs,eligibleWaitMs:pendingSnapshot.episode.trigger.eligibleWaitMs},
   naturalSettlement:{outcome:settled.episode.outcome,waitMs:settled.episode.waitMs,eligibleWaitMs:settled.episode.eligibleWaitMs,censored:settled.episode.censored},
   completedBeforeNextPoll:{kind:completed.episode.kind,outcome:completed.episode.outcome,waitMs:completed.episode.waitMs},
   limitedReplayCoverage:{valid:settled.valid,coverageComplete:settled.coverageComplete,requestsSkipped:settled.counts.requestsSkipped,reason:"Replay starts shortly after the named request; earlier request history is not replayed as prior polls."}});
 }
 assert.equal(sha(fs.readFileSync(rawPath)),rawSha);assert.equal(sha(fs.readFileSync(helperPath)),helperSha);
 console.log(JSON.stringify({kind:"offline-historical-request-shape-replay",passed:true,raw:{path:rawPath,bytes:rawBytes.length,sha256:rawSha},helper:{path:helperPath,bytes:helperBytes.length,sha256:helperSha},
  scope:"Read-only VM replay of already-frozen relay39 request/frame/control/lifecycle fields. Historical request fields are clipped at each modeled observation time; future entries/exits/frameCalls are cleared. This is NOT a fresh browser/CDP sample or a reproduction in the current environment.",
  modelLimits:["Three chosen virtual times (or two for completed-before-poll), not observed historical Node status-poll times.","Current play active state is modeled from the most recent completed historical frame; no historical live play object snapshot was recorded.","Date.now is synthetic floor(timeOrigin+virtual performance.now), not a recorded historical wall clock.","Polled native stats and transport queues are omitted; no packet, heartbeat, host-load or CPU causal evidence is inferred.","Snapshot reconstruction work is a test fixture, not measurement of probe overhead or browser scheduling."],
  cases:all,sourceBytesUnchanged:true}));
}catch(error){console.log(JSON.stringify({passed:false,error:String(error.message)}));process.exitCode=1;}
