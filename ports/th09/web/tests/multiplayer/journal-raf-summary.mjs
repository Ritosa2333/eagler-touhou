// Offline request/dispatch timing audit. Does not change the driver or its body window.
import {readFileSync,writeFileSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
const sha=b=>createHash('sha256').update(b).digest('hex');
const finite=n=>typeof n==='number'&&Number.isFinite(n);
const dist=values=>{
 const s=values.filter(finite).sort((a,b)=>a-b);
 return {count:s.length,unavailable:values.length-s.length,p50:s[Math.floor(s.length*.5)]??null,
  p95:s[Math.floor(s.length*.95)]??null,p99:s[Math.floor(s.length*.99)]??null,max:s.at(-1)??null,
  over50ms:s.filter(x=>x>50).length,over100ms:s.filter(x=>x>100).length,over250ms:s.filter(x=>x>250).length};
};
function summarizePeer(report,side){
 const result=report.results?.[side],measure=result?.measure,d=result?.rafDiagnostics,w=report.summary?.[side]?.window;
 const reasons=[],issue=(code,details={})=>reasons.push({code,...details});
 if(report.parameters?.rafDiagnostics!==true)issue('diagnostics-not-declared');
 if(!d?.valid||d?.overflow||d?.hookErrors?.length)issue('diagnostics-invalid');
 const requests=d?.requests??[],frames=d?.frames??[],samples=measure?.samples??[];
 const requestMap=new Map(requests.map(q=>[q.index,q])),framesByRaf=new Map();
 for(const f of frames){if(!framesByRaf.has(f.rafIndex))framesByRaf.set(f.rafIndex,[]);framesByRaf.get(f.rafIndex).push(f);}
 const start=w?.startSampleIndex,end=w?.endSampleIndexExclusive;
 if(!Number.isInteger(start)||!Number.isInteger(end)||start>=end||end>=samples.length)
  return {side,qualified:false,reasons:[...reasons,{code:'body-window-unavailable'}]};
 const bodyFrames=frames.filter(f=>f.sampleBefore>=start&&f.sampleBefore<end&&f.sampleAfter===f.sampleBefore+1);
 const countsBySample=new Map();
 for(const f of bodyFrames)countsBySample.set(f.sampleBefore,(countsBySample.get(f.sampleBefore)||0)+1);
 const badSamples=[];
 for(let i=start;i<end;++i)if(countsBySample.get(i)!==1)badSamples.push({index:i,frames:countsBySample.get(i)||0});
 if(badSamples.length)issue('body-frame-mapping', {badSamples});
 const firstFrame=bodyFrames.find(f=>f.sampleBefore===start),boundaryFrame=frames.find(f=>f.sampleBefore===end&&f.sampleAfter===end+1);
 const first=requestMap.get(firstFrame?.rafIndex),boundary=requestMap.get(boundaryFrame?.rafIndex);
 if(!finite(first?.enteredAt)||!finite(boundary?.enteredAt))
  return {side,qualified:false,reasons:[...reasons,{code:'body-dispatch-bounds-unavailable'}]};
 const entryBounds=[first.enteredAt,boundary.enteredAt],life=result?.lifecycle??[],controls=d.controls??[];
 const visibleThroughout=(from,to)=>{
  const before=life.filter(e=>e[0]<=from).at(-1);
  return before?.[2]==='visible'&&!life.some(e=>e[0]>from&&e[0]<=to&&(e[2]!=='visible'||['freeze','pagehide'].includes(e[1])));
 };
 function row(q){
  const parent=requestMap.get(q.parentIndex),fs=framesByRaf.get(q.index)||[],classification=[];
  const parentReady=finite(parent?.exitedAt)?Math.max(q.requestedAt,parent.exitedAt):null;
  const requestedWait=finite(q.enteredAt)&&finite(q.requestedAt)?q.enteredAt-q.requestedAt:null;
  const dispatchWait=finite(parentReady)&&finite(q.enteredAt)?q.enteredAt-parentReady:null;
  const callbackWork=finite(q.exitedAt)&&finite(q.enteredAt)?q.exitedAt-q.enteredAt:null;
  if(!parent||!finite(parent.exitedAt)||!(parent.requestedAt<=q.requestedAt&&parent.exitedAt<=q.enteredAt))
   classification.push('parent-not-fully-paired');
  if(!fs.length)classification.push('no-game-frame');
  if(fs.some(f=>f.activeBefore!==true||f.endedBefore||f.threw))classification.push('not-observed-active');
  if(q.pauseAtEntry!==0||q.pauseAtExit!==0||parent?.pauseAtExit!==0)classification.push('pause-state-not-explicitly-zero');
  if(q.frameHookAtEntry!==true||q.frameHookAtExit!==true)classification.push('frame-hook-not-current');
  if(q.cancelledAt!==null||q.requestThrew||q.callbackThrew)classification.push('request-error-or-cancel');
  if([requestedWait,dispatchWait,callbackWork].some(n=>!finite(n)||n<0))classification.push('invalid-timing');
  if(!finite(parentReady)||!visibleThroughout(parentReady,q.enteredAt))classification.push('visibility-not-continuously-observed-visible');
  const pauseControls=controls.filter(c=>finite(parentReady)&&c.enteredAt>=parentReady&&c.enteredAt<=q.enteredAt&&
   (c.name==='_th09_loop_stop'||(c.name==='_th09_loop_pause'&&c.argument!==0)));
  if(pauseControls.length)classification.push('pause-or-stop-during-wait');
  return {index:q.index,parentIndex:q.parentIndex,callbackId:q.callbackId,observedActive:classification.length===0,classification,
   requestedAt:q.requestedAt,enteredAt:q.enteredAt,exitedAt:q.exitedAt,parentExitedAt:parent?.exitedAt??null,
   registeredToEntryMs:requestedWait,parentReadyToEntryMs:dispatchWait,callbackWallMs:callbackWork,
   parentRemainderAfterRegistrationMs:finite(parentReady)?parentReady-q.requestedAt:null,
   pauseAtEntry:q.pauseAtEntry,pauseAtExit:q.pauseAtExit,frameCalls:q.frameCalls,
   frames:fs.map(f=>({index:f.index,sampleBefore:f.sampleBefore,sampleAfter:f.sampleAfter,activeBefore:f.activeBefore,
    endedBefore:f.endedBefore,cppWorkMs:f.cppWorkMs,ok:f.ok}))};
 }
 const bodyRequests=requests.filter(q=>finite(q.enteredAt)&&q.enteredAt>=entryBounds[0]&&q.enteredAt<entryBounds[1]&&d.callbacks?.[q.callbackId]?.game);
 const rows=bodyRequests.map(row),observed=rows.filter(r=>r.observedActive);
 if(!observed.length)issue('no-observed-active-game-raf');
 const bodyRafIds=new Set(bodyFrames.map(f=>f.rafIndex));
 const missingRaf=[...bodyRafIds].filter(id=>!bodyRequests.some(q=>q.index===id));
 if(missingRaf.length)issue('sampled-game-raf-unavailable',{missingRaf});
 const exclusions={};for(const r of rows)for(const code of r.classification)exclusions[code]=(exclusions[code]||0)+1;
 const timing=rs=>({registeredToEntryMs:dist(rs.map(r=>r.registeredToEntryMs)),
  parentReadyToEntryMs:dist(rs.map(r=>r.parentReadyToEntryMs)),callbackWallMs:dist(rs.map(r=>r.callbackWallMs))});
 const largest=(key)=>observed.slice().sort((a,b)=>b[key]-a[key]).slice(0,8);
 function gapDetail(g){
  if(!g)return null;
  const from=g.fromSample,to=g.toSample;
  return {...g,samples:samples.slice(from,to+1).map((sample,j)=>{
   const i=from+j,f=frames.find(f=>f.sampleBefore===i&&f.sampleAfter===i+1),q=requestMap.get(f?.rafIndex);
   return {index:i,pageAt:measure.started+sample[0],rafTimestamp:sample[13],nativeWorkMs:sample[2],next:sample[3],
    legacyStatsWatermark:sample[4],corrections:sample[5],resimulated:sample[6],inputCaptures:sample[7],
    presentations:sample[11],sessionPhase:sample[12],networkPacing:measure.networkPacing?.[i],
    worldSaveCount:measure.frameDetails?.[i]?.[1],request:q?row(q):null};
  })};
 }
 const ns=w.segments?.body?.newStates;
 return {side,qualified:reasons.length===0,reasons,bodySampleRange:[start,end],bodyRequestEntryRangeMs:entryBounds,
  bodySampleCount:end-start,bodyGameRafCount:rows.length,observedActiveGameRafCount:observed.length,
  requestInventory:{retained:requests.length,entered:requests.filter(q=>finite(q.enteredAt)).length,
   pendingAtSnapshot:d.pendingIndices,neverEntered:requests.filter(q=>!finite(q.enteredAt)).map(q=>({index:q.index,
    requestedAt:q.requestedAt,parentIndex:q.parentIndex,knownGame:!!d.callbacks?.[q.callbackId]?.game,
    registeredWithinBodyEntryRange:q.requestedAt>=entryBounds[0]&&q.requestedAt<entryBounds[1],cancelledAt:q.cancelledAt}))},
  excludedGameRafCount:rows.length-observed.length,exclusions,allBodyGameRafTiming:timing(rows),observedActiveTiming:timing(observed),
  lifecycle:life,controlsDuringBody:controls.filter(c=>c.enteredAt>=entryBounds[0]&&c.enteredAt<entryBounds[1]),
  largestActiveDispatchWaits:largest('parentReadyToEntryMs'),largestActiveCallbackWalls:largest('callbackWallMs'),
  maximumPageNewStateGap:gapDetail(ns?.maximumSampledAtGap),maximumRafNewStateGap:gapDetail(ns?.maximumRafGap),
  boundary:w.formalWithEndBoundary,crossing:w.crossingNewStateIntervals};
}
const [outputArg,...inputs]=process.argv.slice(2);
if(!outputArg||!inputs.length)throw Error('Usage: journal-raf-summary.mjs output.json report.json [...]');
const output=resolve(outputArg);
if(existsSync(output))throw Error('Refusing to overwrite output');
const ownPath=fileURLToPath(import.meta.url);
const runs=inputs.map(input=>{
 const path=resolve(input);
 if(path.toLowerCase()===output.toLowerCase())throw Error('Output equals input');
 const bytes=readFileSync(path),report=JSON.parse(bytes);
 return {path,rawSha256:sha(bytes),wasm:report.identity?.wasm,passed:report.passed,peers:[0,1].map(side=>summarizePeer(report,side))};
});
const result={schema:'th09/journal-raf-summary/2',createdAt:new Date().toISOString(),source:{path:ownPath,sha256:sha(readFileSync(ownPath))},
 method:{window:'Use frozen driver [start,end) sample range; select known-game RAF requests that entered from the first body sample RAF entry to the end-boundary sample RAF entry. Inventory never-entered requests separately. Unknown/non-game and outside-body requests remain in the raw report, outside this timing denominator.',
 legacyWatermark:'sample[4] is retained as legacyStatsWatermark, not interpreted as the continuous actual remote input frontier. Use networkPacing F/A/outstanding for that distinction.',
 registeredWait:'enteredAt - requestedAt; includes any remainder of the parent callback.',
 availableDispatchWait:'enteredAt - max(requestedAt,parent.exitedAt); a dispatch waiting interval, not proof that the browser or CPU was idle.',
 observedActive:'Associated game frame was active and not ended; hooks current; request completed without cancel/exception; explicit requested pause 0 at entry/exit and parent exit; visible lifecycle through wait; no pause/stop call during wait.',
 boundary:'First selected request may have been registered before the body. End-boundary request is outside the body. Driver end/crossing new-state gaps are retained separately.',
 causality:'CPU, other JavaScript, OS scheduling, browser refresh policy and remote input arrival need independent attribution. No scheduling-cause, physical display or general no-stall guarantee.',
 filtering:'All game requests and exclusion counts are retained. Active-only timing is a named subset, not a replacement for all-body evidence.',
 thresholds:'50/100/250ms are descriptive counts, not an automatic acceptance rule.'},
 runs};
writeFileSync(output,JSON.stringify(result,null,2)+'\n',{flag:'wx'});
console.log(JSON.stringify({output,runs:runs.map(r=>({path:r.path,wasm:r.wasm,peers:r.peers.map(p=>({side:p.side,qualified:p.qualified,reasons:p.reasons,
 counts:[p.bodyGameRafCount,p.observedActiveGameRafCount,p.excludedGameRafCount],exclusions:p.exclusions,timing:p.observedActiveTiming}))}))}));
