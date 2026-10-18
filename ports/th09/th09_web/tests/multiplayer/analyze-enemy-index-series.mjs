// Offline B/E/E/B evidence. No browser, subprocess, timers, game calls, or input changes.
// Run only after the serialized measurements have finished and root authorizes analysis.
// Usage: node analyze-enemy-index-series.mjs [plan.json [B1.json E1.json E2.json B2.json]]
//        [--output new-evidence.json]
// Output is create-only. Missing, failed, duplicate, and incomplete runs are retained.
import fs from 'node:fs';
import path from 'node:path';
import {createHash} from 'node:crypto';
import {fileURLToPath} from 'node:url';
import {isDeepStrictEqual} from 'node:util';

const project=path.resolve(fileURLToPath(new URL('../../../',import.meta.url)));
const self=fileURLToPath(import.meta.url);
const schema='continuous-prediction-frontier-v1';
const maxOutputBytes=512*1024;
const sources=new Map();
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const jsonSha=value=>sha(JSON.stringify(value));
const finite=Number.isFinite;
const arr=value=>Array.isArray(value)?value:[];
const own=(value,key)=>value!==null&&typeof value==='object'&&Object.hasOwn(value,key);
const canonical=value=>{
 if(Array.isArray(value))return value.map(canonical);
 if(value&&typeof value==='object')return Object.fromEntries(Object.keys(value).sort().map(key=>[key,canonical(value[key])]));
 return value;
};
const stableSha=value=>jsonSha(canonical(value));
const equal=(a,b)=>isDeepStrictEqual(a,b);
const errorText=error=>String(error?.stack??error);
const digestedText=value=>{
 if(value===undefined||value===null)return null;
 const text=String(value);
 return {text:text.slice(0,4096),chars:text.length,omittedChars:Math.max(0,text.length-4096),sha256:sha(text)};
};
function bounded(value,limit=12){
 if(!Array.isArray(value))return {available:false,observedType:value===null?'null':typeof value};
 return {count:value.length,sha256:jsonSha(value),items:value.slice(0,limit),omitted:Math.max(0,value.length-limit)};
}
function numberDistribution(values){
 const invalid=values.filter(x=>!finite(x)).length;
 if(invalid)return {count:values.length,valid:false,invalidCount:invalid,sha256:jsonSha(values),
  total:null,p50:null,p95:null,p99:null,max:null};
 const sorted=values.slice().sort((a,b)=>a-b);
 return {count:sorted.length,valid:true,total:sorted.reduce((sum,x)=>sum+x,0),
  min:sorted[0]??null,p50:sorted[Math.floor(sorted.length*.5)]??null,
  p95:sorted[Math.floor(sorted.length*.95)]??null,p99:sorted[Math.floor(sorted.length*.99)]??null,max:sorted.at(-1)??null};
}
function check(checks,id,passed,detail={},required=true){
 checks.push({id,passed:passed===true,required,...(passed===true?{}:detail)});
 return passed===true;
}
function checked(checks){
 return {total:checks.length,passed:checks.filter(x=>x.passed).length,
  requiredPassed:checks.every(x=>!x.required||x.passed),
  passedIds:checks.filter(x=>x.passed).map(x=>x.id),failed:checks.filter(x=>!x.passed)};
}
function readBytes(file){
 const absolute=path.resolve(file);
 try{
  const bytes=fs.readFileSync(absolute);
  const metadata={path:absolute,bytes:bytes.length,sha256:sha(bytes),available:true};
  if(!sources.has(absolute))sources.set(absolute,metadata);
  return {metadata,bytes};
 }catch(error){
  const metadata={path:absolute,available:false,error:digestedText(errorText(error))};
  if(!sources.has(absolute))sources.set(absolute,metadata);
  return {metadata,bytes:null};
 }
}
function readJson(file){
 const result=readBytes(file);
 if(!result.bytes)return {source:result.metadata,raw:null};
 try{return {source:result.metadata,raw:JSON.parse(result.bytes.toString('utf8'))};}
 catch(error){return {source:{...result.metadata,parseError:digestedText(errorText(error))},raw:null};}
}
function observedSpan(samples,range){
 const [a,b]=range;
 return {firstSampleIndex:b>a?a:null,lastSampleIndex:b>a?b-1:null,
  firstSampledAtMs:b>a?samples[a]?.[0]??null:null,lastSampledAtMs:b>a?samples[b-1]?.[0]??null:null,
  firstToLastSampledAtMs:b>a&&finite(samples[a]?.[0])&&finite(samples[b-1]?.[0])?samples[b-1][0]-samples[a][0]:null,
  toNextBoundarySampleMs:b>a&&b<samples.length&&finite(samples[a]?.[0])&&finite(samples[b]?.[0])?samples[b][0]-samples[a][0]:null,
  nextBoundaryObserved:b<samples.length};
}
function windowEvidence(summary,peer){
 if(!summary?.window||summary.window.schema!==schema)return null;
 const w=summary.window,samples=arr(peer.measure?.samples);
 const segments=Object.fromEntries(['startup','body','tail','allObserved'].map(name=>{
  const s=w.segments?.[name];
  return [name,s?{...s,observedSpan:Array.isArray(s.range)?observedSpan(samples,s.range):null}:null];
 }));
 return {schema:w.schema,complete:w.complete===true,status:w.status,progressSource:w.progressSource,
  nativeFrontierComplete:w.nativeFrontierComplete,epochValid:w.epochValid,
  nativeFrontierMissingSampleIndices:bounded(w.nativeFrontierMissingSampleIndices),
  nativeFrontierErrors:bounded(w.nativeFrontierErrors),epochErrors:bounded(w.epochErrors),
  nativeMatchesObservedNextHighWater:w.nativeMatchesObservedNextHighWater,
  requestedFrameRange:w.requestedFrameRange,bodyRange:w.bodyRange,
  startSampleIndex:w.startSampleIndex,endSampleIndexExclusive:w.endSampleIndexExclusive,
  startMarker:w.startMarker,endMarker:w.endMarker,closedBoundaryAvailable:w.closedBoundaryAvailable,
  markerDeltas:w.markerDeltas,bodyFirstToLast:w.bodyFirstToLast,
  continuousHz:w.complete===true?summary.logicHz:null,
  observedBoundaryLogicHz:summary.logicHz,elapsedMs:summary.elapsedMs,
  callbacks:summary.callbacks,waitCallbacks:summary.waitCallbacks,sessionPhases:summary.sessionPhases,
  callbackWorkMs:summary.callbackWorkMs,corrections:summary.corrections,resimulated:summary.resimulated,captures:summary.captures,
  cppWorkPerFrontierMs:w.markerDeltas?.frontier>0?summary.callbackWorkMs.total/w.markerDeltas.frontier:null,
  terminalClosedNewStateGaps:{qualified:w.complete===true&&w.formalWithEndBoundary?.available===true,
   endpointState:w.formalWithEndBoundary?.endMarkerHasNewState===true?'new-state-at-terminal-marker':
    w.formalWithEndBoundary?.available===true?'no-new-state-at-marker-retained-lower-bound':'terminal-boundary-unavailable',
   ...w.formalWithEndBoundary},
  crossingNewStateIntervals:{start:bounded(w.crossingNewStateIntervals?.start),end:bounded(w.crossingNewStateIntervals?.end)},
  segments};
}
function pacingEvidence(peer,window){
 const rows=arr(peer.measure?.networkPacing),samples=arr(peer.measure?.samples);
 const invalid=rows.flatMap((row,i)=>!Array.isArray(row)||row.length!==7||!row.every(finite)||
  !Number.isInteger(row[0])||row[0]<0||row[0]>=samples.length||row[1]!==1?[i]:[]);
 if(invalid.length)return {available:true,valid:false,count:rows.length,invalidRowIndices:bounded(invalid)};
 if(!rows.length)return {available:false,count:0};
 const range=window?.bodyRange;
 const segment=(a,b)=>{
  const selected=rows.filter(row=>row[0]>=a&&row[0]<b);
  return {range:[a,b],count:selected.length,outstandingFrames:numberDistribution(selected.map(row=>row[2])),
   networkScaleMilli:numberDistribution(selected.map(row=>row[3])),
   effectiveScaleMilli:numberDistribution(selected.map(row=>row[4])),
   networkBrakedSamples:selected.filter(row=>row[3]>1000).length,
   first:selected[0]??null,last:selected.at(-1)??null};
 };
 return {available:true,valid:true,count:rows.length,
  columns:['sampleIndex','version','outstandingFrames','networkIntervalScaleMilli','effectiveIntervalScaleMilli','frontier','actualContiguousExclusive'],
  allObserved:segment(0,samples.length),startup:range?segment(0,range[0]):null,
  body:range?segment(...range):null,tail:range?segment(range[1],samples.length):null,
  scope:'Original contiguous sample-index segments. Scale is an observed pacing value, not an inferred duration or cause; no clock alignment to relay is fabricated.'};
}
function callbackCostPartition(peer,window){
 if(!window||!Array.isArray(window.bodyRange))return {available:false,reason:'continuous-window-unavailable'};
 const samples=arr(peer.measure?.samples),[begin,end]=window.bodyRange;
 const native=window.nativeFrontierComplete===true?
  new Map(arr(peer.measure?.networkPacing).map(row=>[row[0],row[5]])):new Map();
 const names=['unknown','noNewFrontierNoRecordedRecovery','oneFreshNoRecordedRecovery',
  'multipleFreshNoRecordedRecovery','correctionOrResimulationMixed'];
 const groups=Object.fromEntries(names.map(name=>[name,{costs:[],indices:[],deltaFrontier:[],deltaCorrections:[],deltaResimulated:[],maximumWork:null}]));
 const unknownReasons={};
 for(let index=begin;index<end;++index){
  const row=samples[index],previous=index>begin?samples[index-1]:null;
  let category='unknown',reason=null,deltaFrontier=null,deltaCorrections=null,deltaResimulated=null;
  if(previous===null)reason='first-body-callback-no-in-body-predecessor';
  else if(window.nativeFrontierComplete!==true||window.epochValid!==true)reason='native-frontier-or-progress-epoch-incomplete';
  else{
   const currentF=native.get(index),previousF=native.get(index-1);
   if(!Number.isInteger(currentF)||!Number.isInteger(previousF)||
    ![row[5],row[6],previous[5],previous[6]].every(value=>Number.isInteger(value)&&value>=0))reason='invalid-adjacent-counters';
   else{
    deltaFrontier=currentF-previousF;deltaCorrections=row[5]-previous[5];deltaResimulated=row[6]-previous[6];
    if(deltaFrontier<0||deltaCorrections<0||deltaResimulated<0)reason='adjacent-counter-regression';
    else if(deltaCorrections>0||deltaResimulated>0)category='correctionOrResimulationMixed';
    else if(deltaFrontier===0)category='noNewFrontierNoRecordedRecovery';
    else if(deltaFrontier===1)category='oneFreshNoRecordedRecovery';
    else category='multipleFreshNoRecordedRecovery';
   }
  }
  if(reason)unknownReasons[reason]=(unknownReasons[reason]??0)+1;
  const group=groups[category],cost=row[2];
  group.costs.push(cost);group.indices.push(index);
  if(category!=='unknown'){
   group.deltaFrontier.push(deltaFrontier);group.deltaCorrections.push(deltaCorrections);group.deltaResimulated.push(deltaResimulated);
  }
  if(group.maximumWork===null||cost>group.maximumWork.cppCallbackWorkMs)
   group.maximumWork={sampleIndex:index,cppCallbackWorkMs:cost,deltaFrontier,deltaCorrections,deltaResimulated,reason};
 }
 const categories=Object.fromEntries(names.map(name=>{
  const group=groups[name];
  return [name,{callbacks:group.costs.length,cppCallbackWorkMs:numberDistribution(group.costs),
   sampleIndices:bounded(group.indices,8),deltaFrontier:numberDistribution(group.deltaFrontier),
   deltaCorrections:numberDistribution(group.deltaCorrections),deltaResimulated:numberDistribution(group.deltaResimulated),
   maximumWork:group.maximumWork}];
 }));
 const classifiedCallbacks=names.reduce((sum,name)=>sum+categories[name].callbacks,0);
 const categoryCostTotalMs=names.reduce((sum,name)=>sum+categories[name].cppCallbackWorkMs.total,0);
 const bodyCostTotalMs=window.callbackWorkMs?.total??null;
 const costDifferenceMs=finite(bodyCostTotalMs)?categoryCostTotalMs-bodyCostTotalMs:null;
 const toleranceMs=64*Number.EPSILON*Math.max(1,Math.abs(bodyCostTotalMs??0))*Math.max(1,end-begin);
 return {available:true,bodyRange:[begin,end],nativeFrontierComplete:window.nativeFrontierComplete,
  epochValid:window.epochValid,categories,unknownReasons,
  accounting:{bodyCallbacks:window.callbacks,classifiedCallbacks,callbacksConserved:classifiedCallbacks===window.callbacks,
   bodyCostTotalMs,categoryCostTotalMs,costDifferenceMs,toleranceMs,
   costConserved:finite(costDifferenceMs)&&Math.abs(costDifferenceMs)<=toleranceMs},
  scope:'Mutually exclusive measured-body callback partition. The first body callback remains unknown; all later classifications use its immediately preceding original body sample. Any correction/resimulation delta takes precedence over fresh progress. Zero frontier and recovery deltas do not prove an idle callback. Elapsed costs include the entire native callback, never isolated Restore or Tick costs. Per-class delta totals omit the explicitly unknown first callback and are not the terminal e-minus-s counter deltas.'};
}
function anomalyEvidence(value,configuredEnabled){
 const configured=configuredEnabled===true?true:configuredEnabled===false?false:null;
 if(!value)return {available:false,enabled:configured,configuredEnabled:configured,
  status:configured===false?'disabled':configured===true?'enabled-evidence-missing':'not-recorded'};
 const pre=arr(value.history?.pre),post=arr(value.history?.post);
 const endpoint=row=>row?{poll:row.poll,clock:row.clock,lifecycle:row.lifecycle,game:row.game}:null;
 return {available:true,version:value.version,enabled:value.enabled,configuredEnabled:configured,side:value.side,stopped:value.stopped,
  status:value.enabled===false?'disabled':value.episode?'episode-recorded':'enabled-no-episode-recorded',
  valid:value.valid,coverageComplete:value.coverageComplete,limits:value.limits,counts:value.counts,
  errors:bounded(value.errors),clocks:value.clocks,episode:value.episode??null,
  finalObservation:value.finalObservation??null,
  history:{pre:{count:pre.length,sha256:jsonSha(pre),first:endpoint(pre[0]),last:endpoint(pre.at(-1))},
   post:{count:post.length,sha256:jsonSha(post),first:endpoint(post[0]),last:endpoint(post.at(-1))},
   retainedJsonBytes:value.history?.retainedJsonBytes??null},
  scope:'Existing nominal 100 ms observation polls, not exact internal-event coverage. All skip/truncation/eviction/omission counters are retained. coverageComplete does not promise a full pre/post history. An absent episode is not a smoothness pass.'};
}
function rafEvidence(value){
 if(!value)return {available:false};
 const requests=arr(value.requests),pending=arr(value.pendingIndices).map(index=>{
  const row=requests[index];
  return row?{index:row.index,parentIndex:row.parentIndex,callbackId:row.callbackId,nativeId:row.nativeId,
   requestedAt:row.requestedAt,enteredAt:row.enteredAt,exitedAt:row.exitedAt,cancelledAt:row.cancelledAt,
   snapshotAgeMs:finite(value.snapshotAt)&&finite(row.requestedAt)?value.snapshotAt-row.requestedAt:null}: {index,missing:true};
 });
 return {available:true,valid:value.valid,counts:value.counts,overflow:value.overflow,hookErrors:bounded(value.hookErrors),
  snapshotAt:value.snapshotAt,clock:value.clock??null,lastPauseRequested:value.lastPauseRequested??null,
  rafHookCurrent:value.rafHookCurrent??null,cancelHookCurrent:value.cancelHookCurrent??null,frameHookCurrent:value.frameHookCurrent??null,
  callbacks:bounded(value.callbacks,8),controls:bounded(value.controls,12),lastControl:arr(value.controls).at(-1)??null,
  pending:bounded(pending,8),scope:'Final pending requests include ordinary paused/stopped tails. Pending age alone is not an active-game stall classification.'};
}
function nodePollEvidence(value){
 if(!value)return {available:false};
 const rows=arr(value.rows);
 return {available:true,clock:value.clock,timeOrigin:value.timeOrigin,limit:value.limit,attempted:value.attempted,
  overflow:value.overflow,recordedRows:rows.length,rowsSha256:jsonSha(rows),first:rows[0]??null,lastStored:rows.at(-1)??null,
  last:value.last??null,inFlight:value.inFlight??null,
  roundTripMs:numberDistribution(rows.map(row=>finite(row.requestedAt)&&finite(row.returnedAt)?row.returnedAt-row.requestedAt:null)),
  scope:'Node performance.now request/return brackets. Kept separate from each Page finalObservation; a long or missing Node poll does not identify an rAF wait.'};
}
function hostEvidence(h){
 if(!h)return {available:false};
 const cpu=arr(h.cpuSamples),processSamples=arr(h.processSamples),known=arr(h.knownProcesses);
 const knownByPid=new Map(known.map(row=>[row.pid,row.role])),series=new Map();
 const cpuComparable=cpu.filter(row=>finite(row.busyFraction));
 const candidate=/^(?:cl|clang(?:\+\+|-cl|-[0-9]+)?|gcc|g\+\+|cc|cc1(?:plus)?|ld(?:\.lld)?|lld(?:-link)?|link|wasm-(?:ld|opt)|ninja|make|mingw32-make|cmake|MSBuild|rustc|cargo|ccache|sccache)$/i;
 const malformedProcessRows=[];
 for(const [sampleIndex,sample] of processSamples.entries())for(const row of arr(sample.rows)){
  if(!Array.isArray(row)||row.length!==3||!Number.isInteger(row[0])||typeof row[1]!=='string'){
   malformedProcessRows.push({sampleIndex,row});continue;
  }
  const [pid,name,cpuSeconds]=row,key=pid+':'+name;
  let s=series.get(key);
  if(!s){s={pid,name,role:knownByPid.get(pid)??null,compilerNamedCandidate:candidate.test(name),observations:0,
   firstSourceUtcMs:sample.sourceUtcMs,lastSourceUtcMs:sample.sourceUtcMs,firstCpuSeconds:cpuSeconds,lastCpuSeconds:cpuSeconds,
   observedPositiveDeltaSeconds:0,comparableConsecutiveDeltas:0,counterRegressions:0,missingCpuObservations:0,
   previous:null};series.set(key,s);}
  ++s.observations;s.lastSourceUtcMs=sample.sourceUtcMs;s.lastCpuSeconds=cpuSeconds;
  if(!finite(cpuSeconds))++s.missingCpuObservations;
  if(s.previous&&finite(s.previous.cpuSeconds)&&finite(cpuSeconds)){
   const delta=cpuSeconds-s.previous.cpuSeconds;
   if(delta<0)++s.counterRegressions;
   else if(finite(sample.sourceUtcMs)&&sample.sourceUtcMs>s.previous.at){
    ++s.comparableConsecutiveDeltas;s.observedPositiveDeltaSeconds+=delta;
   }
  }
  s.previous={at:sample.sourceUtcMs,cpuSeconds};
 }
 const processes=[...series.values()].map(({previous,...row})=>({...row,
  observedPositiveDeltaSeconds:row.comparableConsecutiveDeltas?row.observedPositiveDeltaSeconds:null}));
 const category=predicate=>{
  const rows=processes.filter(predicate),withDelta=rows.filter(row=>row.observedPositiveDeltaSeconds!==null);
  return {processes:rows.length,processesWithComparableDelta:withDelta.length,
   observedPositiveDeltaSeconds:withDelta.length?withDelta.reduce((sum,row)=>sum+row.observedPositiveDeltaSeconds,0):null,
   counterRegressions:rows.reduce((sum,row)=>sum+row.counterRegressions,0)};
 };
 return {available:true,version:h.version,valid:h.valid,complete:h.complete,stopped:h.stopped,
  startedAtUtcMs:h.startedAtUtcMs,endedAtUtcMs:h.endedAtUtcMs,snapshotAtMs:h.snapshotAtMs,
  cpuIntervalMs:h.cpuIntervalMs,processIntervalMs:h.processIntervalMs,processSampler:h.processSampler,
  counts:h.counts,overflow:h.overflow,errors:bounded(h.errors),marks:bounded(h.marks,32),knownProcesses:bounded(known,32),
  cpu:{count:cpu.length,rowsWithoutComparableBusyFraction:cpu.length-cpuComparable.length,
   busyPercent:numberDistribution(cpuComparable.map(row=>row.busyFraction*100)),
   sampleIntervalMs:numberDistribution(cpu.filter(row=>row.intervalMs!==null).map(row=>row.intervalMs)),
   counterDiscontinuities:cpu.filter(row=>row.counterDiscontinuity).length,
   first:cpu[0]??null,last:cpu.at(-1)??null},
  process:{sampleCount:processSamples.length,failedSamples:processSamples.filter(row=>row.failed).length,
   sourceDurationMs:numberDistribution(processSamples.map(row=>row.sourceDurationMs)),
   observedProcesses:bounded(processes,96),malformedRows:bounded(malformedProcessRows),
   known:category(row=>row.role!==null),compilerNamedCandidates:category(row=>row.role===null&&row.compilerNamedCandidate),
   otherCandidates:category(row=>row.role===null&&!row.compilerNamedCandidate)},
  scope:'Host observations never exclude or reweight game samples. A first process CPU value is lifetime CPU, not interval work. Positive deltas are observed, not proof of compilation or causation; regressions/PID reuse and missing short tasks remain limitations. Aggregate occupancy does not establish single-core, GPU, frequency, or thermal availability.'};
}
function relayEvidence(t){
 if(!t)return {available:false};
 const eventColumns=arr(t.columns?.event),packetColumns=arr(t.columns?.packet),timingColumns=arr(t.columns?.timing);
 const eventIndex=Object.fromEntries(eventColumns.map((name,index)=>[name,index]));
 const packetIndex=Object.fromEntries(packetColumns.map((name,index)=>[name,index]));
 const phases=t.columns?.phases,events=arr(t.events),packets=arr(t.packets);
 const decodeValid=['phase','packetId','enteredAt'].every(key=>own(eventIndex,key))&&
  ['id','sender'].every(key=>own(packetIndex,key))&&phases!==undefined;
 let directions=null;
 if(decodeValid){
  const byId=new Map(packets.map(row=>[row[packetIndex.id],row]));
  const phase=row=>phases[row[eventIndex.phase]];
  directions=[0,1].map(side=>{
   const group=kind=>events.filter(row=>phase(row)===kind&&byId.get(row[eventIndex.packetId])?.[packetIndex.sender]===side);
   const gaps=rows=>numberDistribution(rows.slice(1).map((row,i)=>row[eventIndex.enteredAt]-rows[i][eventIndex.enteredAt]));
   const receives=group('receive'),sends=group('send');
   return {side,receiveCount:receives.length,sendCount:sends.length,
    receiveGapMs:gaps(receives),sendGapMs:gaps(sends)};
  });
 }
 return {available:true,version:t.version,valid:t.valid,sourceIdentity:t.sourceIdentity,clock:t.clock,coverage:t.coverage,
  counts:t.counts,overflow:t.overflow,errors:bounded(t.errors),pending:bounded(t.pending,8),
  eventCount:events.length,packetCount:packets.length,decodedColumnsAvailable:decodeValid,directions,
  schedulerTiming:Object.fromEntries(['lateMs','gapMs'].map(name=>{
   const index=timingColumns.indexOf(name);
   return [name,index<0?null:numberDistribution(arr(t.timing).map(row=>row[index]))];
  })),
  scope:'Full relay-observer lifetime, not the peer formal window. Receive/send boundaries are relay process API observations, not end-to-end delivery or native acceptance. No timeline is aligned across clocks without recorded evidence.'};
}
function expectedKeys(frame,side){
 const cycle=Math.floor(frame/22)+side;
 return frame===0?0:(frame<132?256:0)|(frame%126<90?1:0)|(frame%180<45?4:0)|(cycle%4===0?64:0)|(cycle%4===2?128:0);
}
function peerEvidence(peer,side,rawSummary,context){
 const checks=[],{plan,module,identityMatches,sourceKind,successful}=context;
 if(!peer||typeof peer!=='object')return {side,available:false,checks:checked([{id:'peer-present',passed:false,required:true}])};
 const limit=plan.releaseSeries.frames,samples=arr(peer.measure?.samples),tape=arr(peer.measure?.inputTape);
 const prefix=tape.slice(0,limit),expected=Array.from({length:limit},(_,frame)=>expectedKeys(frame,side));
 const mismatches=prefix.flatMap((buttons,frame)=>buttons!==expected[frame]?[{frame,actual:buttons,expected:expected[frame]}]:[]);
 const hashes=arr(peer.confirmedHashes).slice(0,15);
 check(checks,'wire-tape-1800-frames',prefix.length===limit,{actual:prefix.length,expected:limit});
 check(checks,'wire-tape-equals-frozen-hosted-key-function',mismatches.length===0,{mismatches:bounded(mismatches)});
 const tapeSha=jsonSha(prefix);
 if(successful&&sourceKind==='results')check(checks,'recorded-wire-tape-digest',peer.inputTapeSha256===tapeSha,
  {actual:peer.inputTapeSha256??null,expected:tapeSha});
 else if(own(peer,'inputTapeSha256'))check(checks,'recorded-wire-tape-digest',peer.inputTapeSha256===tapeSha,
  {actual:peer.inputTapeSha256,expected:tapeSha});
 const hashesValid=hashes.length===15&&hashes.every((row,index)=>Array.isArray(row)&&row.length===2&&
  row[0]===(index+1)*120&&Number.isInteger(row[1])&&row[1]>=0&&row[1]<=0xffffffff);
 check(checks,'fifteen-confirmed-checkpoints',hashesValid,{observed:hashes,expectedFrames:Array.from({length:15},(_,i)=>(i+1)*120)});
 check(checks,'measure-limit',peer.measure?.limit===limit,{actual:peer.measure?.limit??null,expected:limit});
 let summary=null,summaryError=null,recomputed=false;
 if(module){
  try{summary=module.summarizeSmoothnessWindow(peer,{startFrame:plan.releaseSeries.formalFrameBoundaries[0],endFrame:limit});recomputed=true;}
  catch(error){summaryError=digestedText(errorText(error));}
 }else summaryError={text:'Frozen window helper unavailable or does not match plan; no substitute helper or legacy fallback used.'};
 check(checks,'window-recomputed-with-frozen-helper',recomputed,{error:summaryError});
 if(rawSummary!==undefined){
  check(checks,'stored-summary-window-schema',rawSummary?.window?.schema===schema,{actual:rawSummary?.window?.schema??null});
  if(recomputed)check(checks,'stored-summary-equals-exact-recomputation',equal(rawSummary,summary),
   {storedSha256:jsonSha(rawSummary),recomputedSha256:jsonSha(summary)});
 }else if(successful&&sourceKind==='results')check(checks,'stored-summary-present',false);
 const window=summary?windowEvidence(summary,peer):null;
 const partition=callbackCostPartition(peer,window);
 check(checks,'callback-cost-partition-conserved',partition.accounting?.callbacksConserved===true&&partition.accounting?.costConserved===true,
  {accounting:partition.accounting??null,available:partition.available});
 check(checks,'continuous-window-complete',window?.complete===true,{status:window?.status??null,error:summaryError});
 if(successful&&sourceKind==='results'){
  check(checks,'successful-measurement-ended',finite(peer.measure?.started)&&peer.measure.started>0&&
   finite(peer.measure?.ended)&&peer.measure.ended>peer.measure.started,
   {started:peer.measure?.started??null,ended:peer.measure?.ended??null});
  check(checks,'successful-native-endpoint-covered',Array.isArray(peer.stats)&&peer.stats.length===8&&
   peer.stats.every(value=>Number.isInteger(value)&&value>=0)&&peer.stats[0]>=limit&&peer.stats[1]>=limit,
   {stats:peer.stats??null,requestedEndFrame:limit});
  check(checks,'actual-native-gpu-renderer',typeof peer.renderer==='string'&&peer.renderer.length>0&&
   !/swiftshader|llvmpipe|microsoft basic render/i.test(peer.renderer),{renderer:peer.renderer??null});
  check(checks,'actual-relay-route',peer.route==='relay',{actual:peer.route??null});
  check(checks,'audio-output-exercised',arr(peer.audioOutput?.blocks).length>100&&peer.audioOutput?.nonzeroBlocks>100&&peer.audioOutput?.overflow===false,
   {blocks:arr(peer.audioOutput?.blocks).length,nonzeroBlocks:peer.audioOutput?.nonzeroBlocks??null,overflow:peer.audioOutput?.overflow??null});
 }
 check(checks,'raf-diagnostics-valid',peer.rafDiagnostics?.valid===true&&peer.rafDiagnostics?.counts?.gameFrames>100,
  {valid:peer.rafDiagnostics?.valid??null,gameFrames:peer.rafDiagnostics?.counts?.gameFrames??null});
 check(checks,'raf-anomaly-enabled-valid',peer.rafAnomaly?.enabled===true&&peer.rafAnomaly?.valid===true&&peer.rafAnomaly?.side===side,
  {enabled:peer.rafAnomaly?.enabled??null,valid:peer.rafAnomaly?.valid??null,side:peer.rafAnomaly?.side??null});
 const storedSummary=rawSummary?.window?.schema===schema?windowEvidence(rawSummary,peer):null;
 return {side,available:true,observationSource:sourceKind,renderer:peer.renderer??null,route:peer.route??null,
  finalStats:peer.stats??null,finalHash:peer.hash??null,measure:{started:peer.measure?.started??null,ended:peer.measure?.ended??null,
   finished:peer.measure?.finished??null,limit:peer.measure?.limit??null,sampleCount:samples.length,networkPacingCount:arr(peer.measure?.networkPacing).length,
   notices:bounded(peer.measure?.notices),closed:bounded(peer.measure?.closed),lifecycle:bounded(peer.lifecycle,12)},
  input:{tapeLength:tape.length,comparedFrameRange:[0,limit],prefixSha256:tapeSha,
   recordedSha256:peer.inputTapeSha256??null,expectedPrefixSha256:jsonSha(expected),mismatches:bounded(mismatches),
   source:'Actual wire-verified measure.inputTape. Tail length may differ; the complete 1800-frame prefix is compared per side. Input capture counter deltas remain in the window.'},
  confirmed:{expectedCount:15,observedTotalCount:arr(peer.confirmedHashes).length,checkpoints:hashes,sha256:jsonSha(hashes),valid:hashesValid},
  summaryIntegrity:{recomputed,identityMatches,storedPresent:rawSummary!==undefined,
   storedSha256:rawSummary===undefined?null:jsonSha(rawSummary),recomputedSha256:summary?jsonSha(summary):null,error:summaryError,
   storedUnverifiedWindow:!recomputed?storedSummary:null},
  window,callbackCostPartition:partition,pacing:pacingEvidence(peer,window),raf:rafEvidence(peer.rafDiagnostics),rafAnomaly:anomalyEvidence(peer.rafAnomaly,context.anomalyEnabled),
  audio:{advancedSeconds:peer.audio?.advancedSeconds??null,blocks:arr(peer.audioOutput?.blocks).length,
   nonzeroBlocks:peer.audioOutput?.nonzeroBlocks??null,overflow:peer.audioOutput?.overflow??null},
  checks:checked(checks)};
}
function metricRow(peer){
 const w=peer?.window,g=w?.terminalClosedNewStateGaps?.newStates?.rafTimestampMs,c=w?.callbackWorkMs;
 const costs=peer?.callbackCostPartition?.categories,one=costs?.oneFreshNoRecordedRecovery,recovery=costs?.correctionOrResimulationMixed;
 return {continuousHz:w?.continuousHz??null,newStateGapP50Ms:g?.p50??null,newStateGapP95Ms:g?.p95??null,
  newStateGapP99Ms:g?.p99??null,newStateGapMaxMs:g?.max??null,
  cppCallbackP50Ms:c?.p50??null,cppCallbackP95Ms:c?.p95??null,cppCallbackP99Ms:c?.p99??null,
  cppCallbackMaxMs:c?.max??null,cppCallbackTotalMs:c?.total??null,cppWorkPerFrontierMs:w?.cppWorkPerFrontierMs??null,
  corrections:w?.corrections??null,resimulated:w?.resimulated??null,captures:w?.captures??null,
  oneFreshNoRecordedRecoveryCallbacks:one?.callbacks??null,oneFreshNoRecordedRecoveryP50Ms:one?.cppCallbackWorkMs?.p50??null,
  oneFreshNoRecordedRecoveryP99Ms:one?.cppCallbackWorkMs?.p99??null,
  recoveryMixedCallbacks:recovery?.callbacks??null,recoveryMixedP50Ms:recovery?.cppCallbackWorkMs?.p50??null,
  recoveryMixedP99Ms:recovery?.cppCallbackWorkMs?.p99??null};
}
function delta(left,right){
 return Object.fromEntries(Object.keys(left).map(key=>{
  const a=left[key],b=right[key],valid=finite(a)&&finite(b);
  return [key,{difference:valid?a-b:null,percent:valid&&b!==0?100*(a-b)/b:null}];
 }));
}
function parseArgs(){
 const positional=[];let output=path.join(project,'docs/multiplayer/rollback-enemy-index-series-evidence.json');
 const args=process.argv.slice(2);
 for(let i=0;i<args.length;++i){
  if(args[i]==='--output'){if(!args[i+1])throw Error('--output requires a path');output=path.resolve(args[++i]);}
  else if(args[i].startsWith('--'))throw Error('Unknown option '+args[i]);
  else positional.push(path.resolve(args[i]));
 }
 if(![0,1,5].includes(positional.length))throw Error('Expected optional plan, then either zero or exactly four raw paths in B/E/E/B order.');
 return {planPath:positional[0]??path.join(project,'docs/multiplayer/rollback-enemy-index-validation-plan.json'),
  rawPaths:positional.slice(1),output};
}
function relatedInputs(label,explicit){
 const directory=explicit?path.dirname(explicit):path.join(project,'th09_web/artifacts/multiplayer-tests');
 let names=[];
 try{names=fs.readdirSync(directory).filter(name=>name.startsWith(label+'-')&&/-(report|failure)\.json$/.test(name));}catch{}
 const paths=names.sort().map(name=>path.join(directory,name));
 if(explicit&&!paths.includes(explicit))paths.unshift(explicit);
 if(!paths.length)paths.push(path.join(directory,label+'-report.json'));
 return paths;
}
function parameterExpectations(series){
 return {limit:series.frames,delay:series.delayPerDirectionMs,jitter:series.jitterPerDirectionMs,slow:series.cpuRatesBySide[1],
  dropEvery:series.dropEvery,release:true,profile:series.profileCpu,fallback:true,inputMode:series.inputMode,
  audioOutput:series.audioOutput,responsiveness:series.responsiveness,burstAt:0,burstFor:0,burstDelay:0,
  rafDiagnostics:series.rafDiagnostics,hostLoad:series.hostLoad,networkTrace:series.networkTrace,
  impairmentSite:'relay',relayDiagnostics:series.relayTrace,relayInputDiagnostics:series.relayTrace,
  expectedRtcPath:'any',liveCost:series.liveCost,clientSendMode:'synchronous',
  burstClock:'relay-first-nonempty-input-receive',startupTrace:series.startupTrace,
  headedControl:series.headedControl,chromeSchedulerTrace:series.chromeSchedulerTrace,rafAnomaly:series.rafAnomaly};
}
function analyzeRaw(file,slot,context){
 const input=readJson(file),checks=[],raw=input.raw;
 if(!raw)return {source:input.source,classification:'missing-or-unreadable',primarySnapshot:null,snapshots:{},
  checks:checked([{id:'raw-readable-json',passed:false,required:true}])};
 const {plan,module,builds}=context,series=plan.releaseSeries;
 const successful=raw.passed===true&&!raw.error;
 check(checks,'raw-run-passed',successful,{passed:raw.passed??null,error:digestedText(raw.error)});
 check(checks,'run-label-filename',path.basename(file)===slot.label+(successful?'-report.json':'-failure.json'),
  {filename:path.basename(file),expectedLabel:slot.label});
 check(checks,'browser-errors-empty',Array.isArray(raw.errors)&&raw.errors.length===0,{errors:bounded(raw.errors)});
 const expectedBinary=slot.build==='B'?plan.baseline.wasmSha256:plan.candidate.wasmSha256;
 check(checks,'wasm-is-declared-'+slot.build,raw.identity?.wasm===expectedBinary,{actual:raw.identity?.wasm??null,expected:expectedBinary});
 const frozenChecks=Object.entries(series.frozenIdentity).map(([key,expected])=>check(checks,'frozen-identity-'+key,
  raw.identity?.[key]===expected,{actual:raw.identity?.[key]??null,expected}));
 check(checks,'anomaly-helper-stable-during-run',raw.identity?.rafAnomalyProbeAfter===series.frozenIdentity.rafAnomalyProbe,
  {actual:raw.identity?.rafAnomalyProbeAfter??null,expected:series.frozenIdentity.rafAnomalyProbe});
 const supportKeys=['relay','shell','presentationHarness','audioProbe','hostLoadProbe','relayTransportProbe','relayTransportFixture','relayTransportController','relayStartupProbe'];
 check(checks,'supporting-source-identities-present',supportKeys.every(key=>/^[a-f0-9]{64}$/.test(raw.identity?.[key]??'')),
  {missingOrInvalid:supportKeys.filter(key=>!/^[a-f0-9]{64}$/.test(raw.identity?.[key]??''))});
 const artifact=builds[slot.build];
 check(checks,'loader-matches-declared-artifact',artifact.loader.available===true&&raw.identity?.loader===artifact.loader.sha256,
  {actual:raw.identity?.loader??null,artifact:artifact.loader});
 const expected=parameterExpectations(series);
 for(const [key,value] of Object.entries(expected))check(checks,'parameter-'+key,own(raw.parameters,key)&&equal(raw.parameters[key],value),
  {actual:raw.parameters?.[key]??null,expected:value});
 check(checks,'cpu-throttle-rates',equal(raw.parameters?.cpuThrottle?.ratesBySide,series.cpuRatesBySide),
  {actual:raw.parameters?.cpuThrottle?.ratesBySide??null,expected:series.cpuRatesBySide});
 check(checks,'cpu-throttle-after-open-probe',raw.parameters?.cpuThrottle?.configuredAfter==='openProbe',
  {actual:raw.parameters?.cpuThrottle?.configuredAfter??null});
 check(checks,'headed-explicit-native-gpu-arguments',['--enable-gpu','--use-gl=angle','--use-angle=d3d11']
  .every(flag=>arr(raw.parameters?.headedLaunch?.explicitArgs).includes(flag)),{args:raw.parameters?.headedLaunch?.explicitArgs??null});
 check(checks,'headed-mode-identity',raw.parameters?.headedLaunch?.headless===false&&raw.headedControl?.identityVerified===true,
  {headless:raw.parameters?.headedLaunch?.headless??null,identityVerified:raw.headedControl?.identityVerified??null});
 check(checks,'two-browser-versions',arr(raw.browsers).length===2&&raw.browsers.every(x=>typeof x==='string'),
  {browsers:raw.browsers??null});
 if(successful){
  check(checks,'summary-method-continuous-schema',raw.summaryMethod?.schema===schema,{actual:raw.summaryMethod?.schema??null});
  check(checks,'fifteen-confirmed-checks-declared',raw.confirmedChecks===15,{actual:raw.confirmedChecks??null});
 }
 const identityMatches=frozenChecks.every(Boolean);
 const snapshots={};
 for(const name of ['results','partial'])if(Array.isArray(raw[name])){
  snapshots[name]=[0,1].map(side=>peerEvidence(raw[name][side],side,name==='results'?raw.summary?.[side]:undefined,
   {plan,module,identityMatches,sourceKind:name,successful,anomalyEnabled:raw.parameters?.rafAnomaly}));
  check(checks,name+'-two-peers',raw[name].length===2,{actual:raw[name].length},name==='results'||!Array.isArray(raw.results));
  if(raw[name].length>2)snapshots[name+'ExtraPeerMetadata']=raw[name].slice(2).map((peer,index)=>({side:index+2,
   sampleCount:arr(peer?.measure?.samples).length,sha256:jsonSha(peer)}));
 }
 const primarySnapshot=Array.isArray(raw.results)?'results':Array.isArray(raw.partial)?'partial':null;
 const peers=primarySnapshot?snapshots[primarySnapshot]:[];
 check(checks,'primary-snapshot-present',primarySnapshot!==null);
 check(checks,'primary-peers-valid',peers.length===2&&peers.every(peer=>peer.checks.requiredPassed),
  {primarySnapshot,peerRequiredPassed:peers.map(peer=>peer.checks.requiredPassed)});
 check(checks,'within-run-confirmed-traces-identical',peers.length===2&&peers.every(peer=>peer.confirmed?.valid)&&
  equal(peers[0].confirmed.checkpoints,peers[1].confirmed.checkpoints),
  {digests:peers.map(peer=>peer.confirmed?.sha256??null)});
 if(raw.finalComparable===true){
  const values=arr(raw.results),eligible=values.length===2&&values.every(peer=>Array.isArray(peer.stats)&&peer.stats.length===8&&
   peer.stats[1]>=peer.stats[0]&&peer.stats[7]>=peer.stats[0])&&values[0].stats[0]===values[1].stats[0];
  check(checks,'declared-live-endpoint-is-fully-confirmed',eligible);
  check(checks,'fully-confirmed-live-endpoint-hashes-equal',eligible&&Number.isInteger(values[0].hash)&&values[0].hash===values[1].hash,
   {hashes:values.map(peer=>peer.hash??null)});
 }
 check(checks,'relay-observation-present-valid',raw.relayTransport?.valid===true,{valid:raw.relayTransport?.valid??null});
 for(const [field,identityKey] of [['relaySha256','relay'],['probeSha256','relayTransportProbe'],['fixtureSha256','relayTransportFixture'],['startupProbeSha256','relayStartupProbe']])
  check(checks,'relay-identity-'+field,typeof raw.identity?.[identityKey]==='string'&&raw.relayTransport?.sourceIdentity?.[field]===raw.identity[identityKey],
   {recorded:raw.relayTransport?.sourceIdentity?.[field]??null,identity:raw.identity?.[identityKey]??null});
 check(checks,'host-observation-present',raw.hostLoad!==undefined&&raw.hostLoad!==null,{},false);
 check(checks,'host-coverage-valid',raw.hostLoad?.valid===true,{valid:raw.hostLoad?.valid??null,complete:raw.hostLoad?.complete??null},false);
 const commonIdentity={...raw.identity};delete commonIdentity.wasm;delete commonIdentity.loader;
 return {source:input.source,classification:successful?'reported-success':'retained-failure',reportedPassed:raw.passed??null,
  error:digestedText(raw.error),errors:bounded(raw.errors),relayTraceError:digestedText(raw.relayTraceError),
  identity:raw.identity??null,parameters:raw.parameters??null,browsers:raw.browsers??null,
  sameFrame:raw.sameFrame??null,finalComparable:raw.finalComparable??null,confirmedChecks:raw.confirmedChecks??null,
  inputDelayEvidence:{declaredFrames:series.inputDelayFrames,rawRecordedFrames:raw.parameters?.inputDelayFrames??null,
   scope:'D=0 is declared by the plan and pinned binary identity; this harness does not separately export inputDelayFrames as a runtime parameter.'},
  summaryMethod:raw.summaryMethod??null,legacySummary:{present:own(raw,'legacySummary'),
   sha256:own(raw,'legacySummary')?jsonSha(raw.legacySummary):null,usedForAnyMetric:false},
  primarySnapshot,primarySnapshotRule:'Use results when present; otherwise partial. Later partial is retained separately, never spliced into or substituted for results.',
  snapshots,headedControl:raw.headedControl??null,host:hostEvidence(raw.hostLoad),relay:relayEvidence(raw.relayTransport),
  nodePolls:nodePollEvidence(raw.rafAnomalyNodePolls),checks:checked(checks),
  comparisonIdentity:{parametersSha256:stableSha(raw.parameters??null),commonIdentitySha256:stableSha(commonIdentity),
   browserVersionsSha256:stableSha(raw.browsers??null),renderersSha256:stableSha(peers.map(peer=>peer.renderer??null)),
   tapePrefixSha256:peers.map(peer=>peer.input?.prefixSha256??null),confirmedSha256:peers.map(peer=>peer.confirmed?.sha256??null)}};
}
async function main(){
 const args=parseArgs();
 if(fs.existsSync(args.output))throw Error('Output already exists; use a new --output path. Existing evidence is never overwritten.');
 const planInput=readJson(args.planPath),plan=planInput.raw,series=plan?.releaseSeries;
 if(plan?.schema!=='th09-enemy-index-validation-plan-v1'||!series)throw Error('Unsupported or unreadable validation plan.');
 if(series.windowSchema!==schema||series.frames!==1800||!equal(series.formalFrameBoundaries,[180,1800])||
  !equal(arr(series.sequence).map(row=>row.build),['B','E','E','B'])||series.sequence.some(row=>!/^[\w-]+$/.test(row.label))||
  new Set(series.sequence.map(row=>row.label)).size!==4)throw Error('Expected declared continuous 180..1800 B/E/E/B protocol; no implicit protocol conversion.');
 if(series.inputMode!=='tape'||series.inputDelayFrames!==0||series.impairmentSite!=='independent-relay'||series.burst!==false||
  !equal(series.cpuRatesBySide,[1,4])||series.nativeGpu!==true)throw Error('Unexpected release protocol. Review the analyzer mapping before changing the plan.');
 for(const key of ['harness','smoothnessWindow','rafAnomalyProbe'])if(!/^[a-f0-9]{64}$/.test(series.frozenIdentity?.[key]??''))
  throw Error('Missing frozen identity '+key);
 const checks=[],sourceFiles={harness:path.join(project,'th09_web/tests/multiplayer/smoothness-browser.mjs'),
  smoothnessWindow:path.join(project,'th09_web/tests/multiplayer/smoothness-window.mjs'),
  rafAnomalyProbe:path.join(project,'th09_web/tests/multiplayer/raf-anomaly-probe.mjs')};
 const analysisSources={analyzer:readBytes(self).metadata};
 let module=null;
 for(const [key,file] of Object.entries(sourceFiles)){
  const source=readBytes(file);analysisSources[key]=source.metadata;
  const matches=source.metadata.available===true&&source.metadata.sha256===series.frozenIdentity[key];
  check(checks,'analysis-source-matches-frozen-'+key,matches,{source:source.metadata,expected:series.frozenIdentity[key]});
  if(key==='smoothnessWindow'&&matches){
   // Execute the exact already-hashed pure helper bytes, not a later filesystem read.
   module=await import('data:text/javascript;base64,'+source.bytes.toString('base64'));
   check(checks,'helper-exported-schema',module.SMOOTHNESS_WINDOW_SCHEMA===schema);
  }
 }
 const builds={};
 for(const [name,declared] of [['B',plan.baseline],['E',plan.candidate]]){
  const directory=path.resolve(project,declared.artifact),wasm=readBytes(path.join(directory,'th09.wasm')).metadata;
  const loader=readBytes(path.join(directory,'th09.mjs')).metadata;
  builds[name]={declared,wasm,loader};
  check(checks,'artifact-'+name+'-wasm-sha',wasm.available===true&&wasm.sha256===declared.wasmSha256,
   {actual:wasm,expected:declared.wasmSha256});
 }
 const groups=series.sequence.map((slot,index)=>{
  const paths=relatedInputs(slot.label,args.rawPaths[index]);
  const reports=paths.map(file=>{
   try{return analyzeRaw(file,slot,{plan,module,builds});}
   catch(error){return {source:sources.get(path.resolve(file))??{path:path.resolve(file),available:false},
    classification:'analysis-failed',analysisError:digestedText(errorText(error)),primarySnapshot:null,snapshots:{},
    checks:checked([{id:'raw-analysis-completed',passed:false,required:true}])};}
  });
  check(checks,'slot-'+slot.label+'-exactly-one-artifact',reports.length===1,{paths:reports.map(row=>row.source.path)});
  return {index,label:slot.label,build:slot.build,explicitInput:args.rawPaths[index]??null,reports};
 });
 const singles=groups.map(group=>group.reports.length===1?group.reports[0]:null),reference=singles[0];
 for(const [index,run] of singles.entries()){
  check(checks,'slot-'+index+'-required-gates',run?.checks.requiredPassed===true,
   {classification:run?.classification??'ambiguous',source:run?.source.path??null});
  if(index&&reference&&run)for(const field of ['parametersSha256','commonIdentitySha256','browserVersionsSha256','renderersSha256','tapePrefixSha256','confirmedSha256'])
   check(checks,'slot-'+index+'-matches-B1-'+field,
    own(run.comparisonIdentity,field)&&own(reference.comparisonIdentity,field)&&
    equal(run.comparisonIdentity[field],reference.comparisonIdentity[field]),
    {expected:reference.comparisonIdentity?.[field]??null,actual:run.comparisonIdentity?.[field]??null});
 }
 const hostOrder=singles.map((run,index)=>({index,label:groups[index].label,
  startedAtUtcMs:run?.host?.startedAtUtcMs??null,endedAtUtcMs:run?.host?.endedAtUtcMs??null}));
 for(let index=1;index<hostOrder.length;++index){
  const previous=hostOrder[index-1],next=hostOrder[index];
  const available=finite(previous.endedAtUtcMs)&&finite(next.startedAtUtcMs);
  check(checks,'observed-host-interval-order-'+index,available&&previous.endedAtUtcMs<=next.startedAtUtcMs,
   {previous,next},available);
 }
 // Recheck every read input. A changed file invalidates comparison, but existing results stay in the output.
 for(const source of sources.values())if(source.available){
  const current=readBytes(source.path).metadata;
  check(checks,'source-unchanged-'+path.relative(project,source.path),current.available===true&&current.sha256===source.sha256,
   {before:source,after:current});
 }
 const contract=checked(checks),comparisonAvailable=contract.requiredPassed;
 const comparisons=[0,1].map(side=>{
  const ordered=singles.map((run,index)=>({label:groups[index].label,build:groups[index].build,
   available:!!run?.primarySnapshot,qualified:run?.checks.requiredPassed===true,
   metrics:metricRow(run?.primarySnapshot?run.snapshots[run.primarySnapshot]?.[side]:null)}));
  const [b1,e1,e2,b2]=ordered.map(row=>row.metrics);
  return {side,available:comparisonAvailable,ordered,
   descriptivePairs:comparisonAvailable?{
    E1_minus_B1:delta(e1,b1),E2_minus_B2:delta(e2,b2),B2_minus_B1_baselineDrift:delta(b2,b1),
    E2_minus_E1_repeatVariation:delta(e2,e1)}:null,
   scope:'Two observations per build in declared B/E/E/B order. No pooled percentiles, outlier deletion, statistical significance, or automatic performance acceptance. Host activity and actual correction work remain observed variables.'};
 });
 const result={schema:'th09-enemy-index-series-evidence-v1',analysisKind:'offline-frozen-raw-continuous-window-comparison',
  generatedAtUtc:new Date().toISOString(),plan:{source:planInput.source,declared:plan},analysisSources,builds,
  metrics:{windowSchema:schema,bounds:[180,1800],
   progress:'F = native PredictionFrontier. First crossings define one contiguous [s,e) body; closed Hz and counters use F[e]-F[s] and t[e]-t[s]. Captures never substitute for completed progress.',
   terminalGaps:'Primary new-state distributions are window.formalWithEndBoundary.newStates, including terminal closure. RAF and sampledAt clocks stay distinct. No physical scanout inference.',
   callbackWork:'C++ callback elapsed work for all original body callbacks, including the start callback. Endpoint callback cost is tail. It is not observer JavaScript duration or GPU completion.',
   callbackPartition:'Mutually exclusive body callback categories use adjacent native F/correction/resimulation deltas. First body callback is unknown; partition count/cost conservation is checked. All-body percentiles include no-frontier callbacks and are not game-tick costs.',
   phases:'SessionPhase values are retained without filtering; they are not internal MatchScene values.',
   quantiles:'Sorted values at floor(n*p), matching the frozen helper. Empty or unavailable metrics remain null, not zero.',
   failures:'All discovered report/failure artifacts for every declared label are retained. Multiple attempts block a clean four-run comparison. results and partial remain separate.',
   comparability:'Frozen identities, exact B/E Wasm, same helper/parameters/browser/renderer, deterministic per-side tape prefix, all 15 confirmed hashes and complete continuous windows are required. Host load never filters samples.'},
  contract,groups,comparisons,hostIntervalOrder:{rows:hostOrder,
   scope:'Host-probe intervals only. They do not by themselves establish full command serialization or absence of compilation between sparse observations.'},
  limitations:['This analyzer checks release-series evidence only; the controlled three-world deep8 gate and later wall-clock input-response followup are not re-run or promoted to rAF performance.',
   'Passing artifact and state checks is not smoothness acceptance. Long gaps, anomaly episodes, tail waits, failed collectors, and incomplete windows remain visible.',
   'No RAF episode in four finite runs does not resolve the frozen historical seconds-long wait. The probe records one eligible episode with bounded poll history.',
   'Exact Wasm identities bind the declared builds, not a fresh independent audit of compiler flags. InputDelayFrames is not a separately exported raw runtime parameter.',
   'Relayed local-host traffic does not cover public NAT/TURN, real remote devices, speaker output, or physical display scanout.'],
  preservation:{rawInputsNeverWritten:true,legacyMetricsUsed:false,maxOutputBytes,sourceFileCount:sources.size}};
 const outputBytes=JSON.stringify(result)+'\n';
 if(Buffer.byteLength(outputBytes)>maxOutputBytes)throw Error('Evidence exceeds '+maxOutputBytes+' bytes; refusing to omit runs or trim failures to fit.');
 if(sources.has(path.resolve(args.output)))throw Error('Output cannot replace an input.');
 fs.writeFileSync(args.output,outputBytes,{flag:'wx'});
 console.log(JSON.stringify({output:path.resolve(args.output),bytes:Buffer.byteLength(outputBytes),sha256:sha(outputBytes),
  comparisonAvailable,requiredFailures:contract.failed.filter(item=>item.required).map(item=>item.id),
  runs:groups.map(group=>({label:group.label,build:group.build,artifacts:group.reports.map(report=>({
   path:report.source.path,classification:report.classification,requiredPassed:report.checks.requiredPassed}))})),comparisons}));
 process.exitCode=comparisonAvailable?0:1;
}
await main();
