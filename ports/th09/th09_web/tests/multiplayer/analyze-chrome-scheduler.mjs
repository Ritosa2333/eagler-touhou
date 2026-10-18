// Offline only: audit sanitized scheduler traces before making cross-clock claims.
// Usage: node analyze-chrome-scheduler.mjs REPORT P1_TRACE P2_TRACE NEW_OUTPUT
import assert from 'node:assert/strict';
import { readFileSync, writeFileSync } from 'node:fs';
import { resolve } from 'node:path';
import { createHash } from 'node:crypto';
import { fileURLToPath } from 'node:url';
const sha256=x=>createHash('sha256').update(x).digest('hex');
const finite=x=>typeof x==='number'&&Number.isFinite(x);
const identities=[];
function read(path){const bytes=readFileSync(path),identity={path:resolve(path).replaceAll('\\','/'),bytes:bytes.length,sha256:sha256(bytes)};
  identities.push(identity);return {value:JSON.parse(bytes),identity};}
function dist(values){const a=values.filter(finite).sort((x,y)=>x-y),q=p=>a.length?a[Math.floor((a.length-1)*p)]:null;
  return {n:a.length,min:q(0),p50:q(.5),p95:q(.95),p99:q(.99),max:q(1),over100:a.filter(x=>x>100).length,over250:a.filter(x=>x>250).length};}
function by(items,key){const result=new Map();for(const row of items){const k=key(row);if(!result.has(k))result.set(k,[]);result.get(k).push(row);}return result;}
function range(rows){let first=null,last=null,end=null,previous=-Infinity,arrivalRegressions=0;
  for(const x of rows){if(!first||x.e.ts<first.e.ts)first=x;if(!last||x.e.ts>last.e.ts)last=x;
    if(!end||x.e.ts+(x.e.dur??0)>end.e.ts+(end.e.dur??0))end=x;
    if(x.e.ts<previous)++arrivalRegressions;previous=x.e.ts;}
  return {count:rows.length,minStartUs:first?.e.ts??null,maxStartUs:last?.e.ts??null,
    maxEndUs:end?end.e.ts+(end.e.dur??0):null,spanMs:end?(end.e.ts+(end.e.dur??0)-first.e.ts)/1000:null,
    minimumIndex:first?.index??null,maximumStartIndex:last?.index??null,maximumEndIndex:end?.index??null,
    lastInArrivalIndex:rows.at(-1)?.index??null,lastInArrivalTs:rows.at(-1)?.e.ts??null,arrivalRegressions};}
function clockAudit(t){const allowance=t.clock.quantizationAllowanceMs;
  const matches=t.marks.map(m=>{const candidates=t.events.filter(e=>e.name===m.name&&e.cat.split(',').includes('blink.user_timing'));
    const unique=[...new Map(candidates.map(e=>[[e.pid,e.tid,e.ts].join(':'),e])).values()],e=unique.length===1?unique[0]:null;
    return {...m,candidateCount:candidates.length,uniqueCandidateCount:unique.length,trace:e?{pid:e.pid,tid:e.tid,ts:e.ts}:null,
      offsetMs:e?[e.ts/1000-m.after-allowance,e.ts/1000-m.before+allowance]:null};});
  const paired=matches.filter(m=>m.trace),origins=new Set(paired.map(m=>m.timeOrigin)),threads=new Set(paired.map(m=>m.trace.pid+':'+m.trace.tid));
  const low=paired.length?Math.max(...paired.map(m=>m.offsetMs[0])):null,high=paired.length?Math.min(...paired.map(m=>m.offsetMs[1])):null;
  const span=paired.length>1?Math.max(...paired.map(m=>m.before))-Math.min(...paired.map(m=>m.after)):0;
  const mapped=paired.length>=2&&paired.length===matches.length&&origins.size===1&&threads.size===1&&low<=high&&span>1;
  return {matches,matchedMarks:paired.length,sameThread:threads.size===1,samePageTimeOrigin:origins.size===1,
    offsetIntersectionMs:paired.length?[low,high]:null,distinctMarkSpanMs:span,clockMapped:mapped,
    agreesWithRawMapped:mapped===t.clock.clockMapped,
    scope:'The helper requires every recorded page marker to match, at least two separated markers, one page origin and one renderer thread. A missing stop marker is not repaired by extrapolation.'};}
function traceAudit(t,identity,meta,game){assert.equal(t.events.length,t.counts.retainedEvents);assert.equal(identity.sha256,meta.artifact.sha256);
  assert.equal(identity.bytes,meta.artifact.bytes);assert.equal(t.counts.rawEvents-t.counts.retainedEvents,t.counts.droppedEvents);
  const timed=t.events.map((e,index)=>({e,index})).filter(x=>x.e.ph!=='M'&&x.e.ts>0),ext=range(timed),clock=clockAudit(t);
  assert.ok(clock.agreesWithRawMapped);
  assert.deepEqual(clock.offsetIntersectionMs,t.clock.offsetIntersectionMs,'Marker offset bracket differs');
  const categories=t.config.traceConfig.includedCategories.map(cat=>{const rows=timed.filter(x=>x.e.cat.split(',').includes(cat));
    assert.equal(rows.length,t.coverage.categories[cat]);return {cat,...range(rows),unknownNameCount:rows.filter(x=>x.e.nameHash).length};});
  const threadMetadata=new Map(t.events.filter(e=>e.ph==='M'&&e.name==='thread_name').map(e=>[e.pid+':'+e.tid,e.args]));
  const processMetadata=new Map(t.events.filter(e=>e.ph==='M'&&e.name==='process_name').map(e=>[e.pid,e.args]));
  const processes=[...by(timed,x=>x.e.pid)].map(([pid,rows])=>({pid,label:processMetadata.get(pid)??null,...range(rows)}));
  const threads=[...by(timed,x=>x.e.pid+':'+x.e.tid)].map(([key,rows])=>({pid:rows[0].e.pid,tid:rows[0].e.tid,
    label:threadMetadata.get(key)??null,...range(rows)}));
  const unknown=t.events.filter(e=>e.nameHash).length;
  assert.ok(unknown<=t.counts.unknownEventNames,'Retained unknown events exceed the parsed-event counter');
  const marksByStage=new Map(t.marks.map(m=>[m.stage,m])),armed=clock.matches.find(m=>m.stage==='armed'),stop=marksByStage.get('capture-stop');
  let extentRelativeToMarkers=null;
  if(armed?.trace&&ext.maxEndUs!==null){const offset=clock.offsetIntersectionMs,latestPageBracket=[ext.maxEndUs/1000-offset[1],ext.maxEndUs/1000-offset[0]];
    extentRelativeToMarkers={scope:'Extent-only arithmetic using available marker brackets; NOT a validated active-window mapping or per-frame attribution.',
      calibrationValid:clock.clockMapped,retainedMaxEndAfterArmedTraceMs:(ext.maxEndUs-armed.trace.ts)/1000,
      requestedCaptureStopAfterArmedPageMs:stop?[stop.before-armed.after,stop.after-armed.before]:null,
      latestRetainedEndPageBracketMs:latestPageBracket,
      captureStopMinusLatestRetainedEndMs:stop?[stop.before-latestPageBracket[1],stop.after-latestPageBracket[0]]:null};}
  const peak=t.bufferUsage.length?Math.max(...t.bufferUsage.map(x=>x.percentFull??x.value??0)):null;
  const peakRows=t.bufferUsage.filter(x=>(x.percentFull??x.value??0)===peak),firstPeak=peakRows[0],lastPeak=peakRows.at(-1);
  const capture=t.capture,completionError=t.errors.find(e=>e.code==='tracing-complete-timeout');
  const hasOverflow=Object.entries(t.overflow).some(([k,v])=>!k.startsWith('first')&&v>0);
  const expectedCollectionValid=finite(capture.nodeStartedAt)&&capture.tracingCompleteReceived&&capture.dataLossOccurred===false&&
    !capture.chromeBufferFull&&!hasOverflow&&t.counts.errors===0&&t.counts.malformedEvents===0&&t.sourceIdentity.stable&&t.status==='stopped';
  assert.equal(expectedCollectionValid,t.collectionValid,'Collection validity disagrees with retained contract fields');
  const stages=['armed','first-active','capture-stop'],ordered=stages.every((stage,i)=>{
    const m=t.marks.find(x=>x.stage===stage),prev=i?t.marks.find(x=>x.stage===stages[i-1]):null;
    return !!m&&(i===0||!!prev&&m.before>=prev.after);});
  const allThree=stages.every(stage=>clock.matches.some(m=>m.stage===stage&&m.uniqueCandidateCount===1));
  const expectedActiveCoverage=expectedCollectionValid&&clock.clockMapped&&ordered&&allThree&&!capture.deadlineFired;
  assert.equal(expectedActiveCoverage,t.coverage.activeWindowCovered,'Active coverage differs from complete/marker/deadline contract');
  const sourceMatchesReport=t.sourceIdentity.before.harness.sha256===game.identity.harness&&t.sourceIdentity.after.harness.sha256===game.identity.harness&&
    t.sourceIdentity.before.helper.sha256===game.identity.chromeSchedulerTrace&&t.sourceIdentity.after.helper.sha256===game.identity.chromeSchedulerTrace;
  const attributionAllowed=t.collectionValid===true&&clock.clockMapped&&t.coverage.activeWindowCovered===true;
  return {identity,collectionValid:t.collectionValid,availability:t.availability,status:t.status,sourceIdentity:t.sourceIdentity,sourceMatchesReport,
    counts:t.counts,overflow:t.overflow,errors:t.errors,limits:t.limits,config:t.config,capture,
    independentCollectionValidity:expectedCollectionValid,independentActiveCoverage:expectedActiveCoverage,
    droppedEventAccounting:{dropped:t.counts.droppedEvents,malformed:t.counts.malformedEvents,
      localRetentionOverflow:hasOverflow,
      policyFilteredCountWhenUnambiguous:!hasOverflow?t.counts.droppedEvents-t.counts.malformedEvents:null,
      scope:'When malformed and overflow are zero, the collector source has only the out-of-scope category/metadata rejection path left. This filtering is different from local-capacity loss.'},
    captureDurationsMs:{startToStopRequest:capture.nodeStopRequestedAt-capture.nodeStartedAt,
      startToEndRequest:capture.nodeEndRequestedAt-capture.nodeStartedAt,
      endRequestToCompletionError:completionError?completionError.at-capture.nodeEndRequestedAt:null,
      endRequestToComplete:finite(capture.nodeCompletedAt)?capture.nodeCompletedAt-capture.nodeEndRequestedAt:null,
      stopRequestToStopped:capture.nodeStoppedAt-capture.nodeStopRequestedAt},
    localBudget:{reservedMetadataBytes:65536,chargedDataBytes:t.counts.retainedDataJsonBytes,
      chargedDataAndReserveBytes:t.counts.retainedDataJsonBytes+65536,
      fractionOfLimit:(t.counts.retainedDataJsonBytes+65536)/t.limits.sanitizedBytes,
      scope:'The local budget counts only sanitized data already delivered. No local overflow cannot prove the native buffer was not full or that undelivered events would fit.'},
    buffer:{samples:t.bufferUsage,peakPercentFull:peak,firstPeakAt:firstPeak?.at??null,lastPeakAt:lastPeak?.at??null,
      peakRows:peakRows.length,peakPlateauSpanMs:firstPeak?lastPeak.at-firstPeak.at:null,
      startToFirstPeakMs:firstPeak?firstPeak.at-capture.nodeStartedAt:null,
      rawChromeBufferFull:capture.chromeBufferFull,allReportedEventCountsZero:t.bufferUsage.every(x=>x.eventCount===0),
      limitation:'The original full flag only trips at >=1. A persistent 0.996245 plateau does not establish remaining usable capacity; reported eventCount zero does not mean no events.'},
    clock:{raw:t.clock,recomputed:clock},rawCoverage:{...t.coverage,threads:undefined,unknownNames:undefined},
    observedData:{nonMetadataPositiveTime:ext,metadataCount:t.events.filter(e=>e.ph==='M').length,processes,threads,categories,
      unknownNames:t.coverage.unknownNames,unknownEventCount:unknown,unknownEventFraction:unknown/t.events.length,
      parsedUnknownEventCount:t.counts.unknownEventNames,parsedButUnretainedUnknownEvents:t.counts.unknownEventNames-unknown,
      unknownCounterScope:'The raw unknown-name counter is incremented before retention; the first byte-budget rejection may increment it without retaining the event.',
      longDurationEventsOver100Ms:timed.filter(x=>x.e.dur>100000),extentRelativeToMarkers},
    attribution:{allowed:attributionAllowed,reason:attributionAllowed?'complete-calibrated-window':'incomplete-or-unmapped-trace',
      mappedActiveLongWindows:[],scope:'No missing event is interpreted as idle; unknown event-name hashes are not renamed or reverse guessed.'}};
}

function derivedFormal(samples,limit){
  const rows=samples.filter(s=>s[3]>=180&&s[3]<limit),first=rows[0],last=rows.at(-1);
  if(!first)return {available:false};
  const newState=[],raf=[];let prior;
  for(let i=1;i<rows.length;i++)if(rows[i][13]!==rows[i-1][13])raf.push(rows[i][13]-rows[i-1][13]);
  for(const s of rows)if((!prior||s[11]!==prior[11])&&(!prior||s[3]>prior[3])){
    if(prior)newState.push(s[13]-prior[13]);prior=s;
  }
  return {available:true,scope:'Independently derived even for failed harness runs; this does not change a raw failure into a pass.',
    firstFrame:first[3],lastFrame:last[3],callbacks:rows.length,elapsedMs:last[0]-first[0],
    logicHz:(last[3]-first[3])*1000/(last[0]-first[0]),corrections:last[5]-first[5],resimulated:last[6]-first[6],
    reportedNativeWorkMs:dist(rows.map(s=>s[2])),suppliedRafGapMs:dist(raf),newStateSuppliedRafGapMs:dist(newState)};
}
function gameAudit(p,summary,limit){
  const m=p.measure,d=p.rafDiagnostics,samples=m.samples,requests=d.requests,frames=d.frames;
  assert.ok(samples.every(s=>Array.isArray(s)&&s.length===14),'Unexpected sample schema');
  const qm=new Map(requests.map(q=>[q.index,q])),fm=new Map();
  for(const f of frames)for(let i=f.sampleBefore;i<f.sampleAfter;i++){assert.ok(!fm.has(i),'Duplicate frame sample mapping');fm.set(i,f);}
  const qFor=i=>qm.get(fm.get(i)?.rafIndex),formalIndex=samples.findIndex(s=>s[3]>=180),tailIndex=samples.findIndex(s=>s[3]>=limit);
  const active=qFor(0)?.enteredAt??m.started,formal=formalIndex<0?Infinity:qFor(formalIndex)?.enteredAt??m.started+samples[formalIndex][0];
  const tail=tailIndex<0?m.ended:qFor(tailIndex)?.enteredAt??m.started+samples[tailIndex][0];
  const phase=t=>t<active?'pre-active':t<formal?'active-startup':t<tail?'formal':t<=m.ended?'confirmation-tail':'after-measure';
  const sample=i=>({index:i,row:samples[i],sampleAt:m.started+samples[i][0],frame:fm.get(i)??null,request:qFor(i)??null});
  const context=q=>({request:q,parent:qm.get(q.parentIndex)??null,frame:frames[q.firstFrameIndex]??null,
    firstSample:q.firstFrameIndex!==null&&frames[q.firstFrameIndex]?.sampleAfter>frames[q.firstFrameIndex]?.sampleBefore?sample(frames[q.firstFrameIndex].sampleBefore):null});
  const pack=rows=>({distribution:dist(rows.map(x=>x.valueMs)),
    byPhase:Object.fromEntries([...by(rows,x=>x.phase)].map(([k,a])=>[k,dist(a.map(x=>x.valueMs))])),
    maximum:rows.reduce((a,b)=>!a||b.valueMs>a.valueMs?b:a,null),allOver100Ms:rows.filter(x=>x.valueMs>100)});
  const entered=requests.filter(q=>finite(q.enteredAt)).sort((a,b)=>a.enteredAt-b.enteredAt||a.index-b.index);
  const requestRows=entered.map(q=>({phase:phase(q.enteredAt),valueMs:q.enteredAt-q.requestedAt,index:q.index,
    requestedAt:q.requestedAt,enteredAt:q.enteredAt,context:context(q)}));
  const wrapperRows=entered.filter(q=>finite(q.exitedAt)).map(q=>({phase:phase(q.enteredAt),valueMs:q.exitedAt-q.enteredAt,index:q.index,context:context(q)}));
  const entryRows=entered.slice(1).map((q,i)=>({phase:phase(q.enteredAt),valueMs:q.enteredAt-entered[i].enteredAt,
    fromPhase:phase(entered[i].enteredAt),previous:context(entered[i]),next:context(q)}));
  const sampleRows=samples.slice(1).map((s,j)=>({phase:phase(qFor(j+1)?.enteredAt??m.started+s[0]),
    valueMs:s[0]-samples[j][0],previous:sample(j),next:sample(j+1)}));
  const nativeRows=samples.map((s,i)=>({phase:phase(qFor(i)?.enteredAt??m.started+s[0]),valueMs:s[2],sample:sample(i)}));
  const observerRows=frames.filter(f=>finite(f.exitedAt)).map(f=>({phase:phase(f.enteredAt),valueMs:f.exitedAt-f.enteredAt,index:f.index,frame:f}));
  const fresh=[];let lastFrontier=samples[0]?.[3]??-1,lastPresent=samples[0]?.[11]??-1;
  for(let i=1;i<samples.length;i++){const s=samples[i];if(s[11]!==lastPresent&&s[3]>lastFrontier){fresh.push(sample(i));lastFrontier=s[3];}lastPresent=s[11];}
  const freshRows=fresh.slice(1).map((s,i)=>({phase:phase(s.request?.enteredAt??s.sampleAt),
    valueMs:(s.request?.enteredAt??s.sampleAt)-(fresh[i].request?.enteredAt??fresh[i].sampleAt),
    suppliedRafTimestampGapMs:s.row[13]-fresh[i].row[13],sampleAtGapMs:s.sampleAt-fresh[i].sampleAt,previous:fresh[i],next:s}));
  const pending=requests.filter(q=>q.enteredAt===null&&q.cancelledAt===null&&!q.requestThrew).map(q=>({
    phase:phase(q.requestedAt),observedLowerBoundMs:d.snapshotAt-q.requestedAt,rightCensoredAt:d.snapshotAt,context:context(q)}));
  const b=p.audioOutput?.blocks??[],zeroBlocks=b.flatMap((row,index)=>row[4]<=1e-6?[{index,row}]:[]);
  return {stats:p.stats,route:p.route,renderer:p.renderer,rawFormalSummary:summary??null,
    independentlyDerivedFormalSummary:derivedFormal(samples,limit),
    measurement:{samples:samples.length,frameDetails:m.frameDetails.length,started:m.started,ended:m.ended,limit,
      boundaries:{activeAt:active,formalAt:finite(formal)?formal:null,confirmationTailAt:tail,formalIndex,tailIndex},
      first:sample(0),last:sample(samples.length-1)},
    raf:{valid:d.valid,overflow:d.overflow,counts:d.counts,snapshotAt:d.snapshotAt,controls:d.controls,callbacks:d.callbacks,
      errors:d.hookErrors,pending,requestedToActualEntry:pack(requestRows),actualEntryGaps:pack(entryRows),actualWrapperWork:pack(wrapperRows),
      actualOnGameFrameObserverWork:pack(observerRows),sampleObservationGaps:pack(sampleRows),reportedNativeWork:pack(nativeRows),
      freshStateActualEntryGaps:pack(freshRows),firstFreshState:fresh[0]??null,
      scope:'Actual entry/exit boundaries differ from supplied rAF timestamp and reported C++ work. Raw formal summary is unchanged. All >100 ms rows and right-censored pending requests are retained.'},
    audio:{outputProbeAvailable:!!p.audioOutput,runtimeClock:p.audio??null,
      blocks:p.audioOutput?b.length:null,nonzeroBlocks:p.audioOutput?.nonzeroBlocks??null,overflow:p.audioOutput?.overflow??null,zeroBlocks,
      relativeBlockGapMs:dist(b.slice(1).map((row,i)=>row[0]-b[i][0])),
      scope:'Audio rows lack an absolute first-block performance origin. Relative gaps and zero blocks cannot be assigned to an exact rAF instant.'},
    lifecycle:p.lifecycle};
}

function hostAudit(h,sides){
  if(!h)return {available:false};
  const intervals=sides.map(s=>{
    const origin=s.trace.clock.recomputed.matches[0]?.timeOrigin;
    return finite(origin)?[origin+s.game.measurement.boundaries.activeAt,origin+s.game.measurement.ended]:null;
  }).filter(Boolean);
  const active=intervals.length?[Math.min(...intervals.map(x=>x[0])),Math.max(...intervals.map(x=>x[1]))]:null;
  const cpu=h.cpuSamples??[],cpuOverlapping=[];
  for(let i=1;i<cpu.length;i++)if(active&&cpu[i-1].utcMs<active[1]&&cpu[i].utcMs>active[0])
    cpuOverlapping.push({index:i,intervalUtcMs:[cpu[i-1].utcMs,cpu[i].utcMs],busyFraction:cpu[i].busyFraction});
  const compilerNames=new Set(['clang','clang++','clang-cl','gcc','g++','cc1','cc1plus','ld','ld.lld','lld-link','link','wasm-ld','wasm-opt','ninja','cmake','emcc','em++']);
  const observed=new Map();
  for(const p of h.processSamples??[])for(const row of p.rows??[])if(compilerNames.has(row[1])){
    const key=row[0]+':'+row[1];if(!observed.has(key))observed.set(key,[]);
    observed.get(key).push({sourceUtcMs:p.sourceUtcMs,pid:row[0],name:row[1],cpuSeconds:row[2]});
  }
  const compilers=[...observed.values()].map(rows=>{
    rows.sort((a,b)=>a.sourceUtcMs-b.sourceUtcMs);const brackets=[];
    for(let i=1;i<rows.length;i++)if(active&&rows[i-1].sourceUtcMs<active[1]&&rows[i].sourceUtcMs>active[0])
      brackets.push({from:rows[i-1],to:rows[i],cpuDeltaSeconds:rows[i].cpuSeconds-rows[i-1].cpuSeconds});
    return {pid:rows[0].pid,name:rows[0].name,observations:rows.length,first:rows[0],last:rows.at(-1),
      sampledFirstToLastCpuDeltaSeconds:rows.at(-1).cpuSeconds-rows[0].cpuSeconds,bracketsOverlappingActive:brackets};
  });
  return {available:true,valid:h.valid,complete:h.complete,counts:h.counts,overflow:h.overflow,
    hostWindowUtcMs:[h.startedAtUtcMs,h.endedAtUtcMs],peerActiveWindowsUtcMs:intervals,combinedActiveWindowUtcMs:active,
    cpuSampleGapMs:dist(cpu.slice(1).map((row,i)=>row.utcMs-cpu[i].utcMs)),cpuIntervalsOverlappingActive:cpuOverlapping,
    observedBusyFractionsForOverlappingIntervals:Object.fromEntries(Object.entries(dist(cpuOverlapping.map(x=>x.busyFraction))).filter(([k])=>!k.startsWith('over'))),
    compilerNamedProcesses:compilers,
    scope:'All raw host samples remain in hostLoad. These interval brackets may extend beyond game time; their CPU deltas are not attributed wholly to gameplay. Aggregate CPU is not a core/frequency guarantee; sampled PID/name is not full process-lifetime identity. Other Node processes are not labeled compilers.'};
}

function analyzeRun(paths){
  const raw=read(paths[0]),traces=[read(paths[1]),read(paths[2])],r=raw.value;
  const results=r.retainedResults??r.results,resultSource=r.retainedResults?'retainedResults':'results';
  assert.equal(results?.length,2,'Two retained gameplay results are required; later partial snapshots are not a substitute');
  assert.equal(r.chromeSchedulerTrace.length,2);
  const sides=results.map((p,i)=>({side:'P'+(i+1),game:gameAudit(p,r.summary?.[i],r.parameters.limit),
    trace:traceAudit(traces[i].value,traces[i].identity,r.chromeSchedulerTrace[i],r)}));
  for(let i=0;i<sides.length;i++){
    const side=sides[i],t=traces[i].value,clock=side.trace.clock.recomputed,offset=clock.offsetIntersectionMs;
    const activeLong=side.game.raf.requestedToActualEntry.allOver100Ms.filter(w=>['active-startup','formal','confirmation-tail'].includes(w.phase));
    side.trace.attribution.observedActiveLongDispatchCount=activeLong.length;
    const bounds=side.game.measurement.boundaries,end=side.game.measurement.ended;
    const phase=at=>at<bounds.activeAt?'pre-active':bounds.formalAt!==null&&at<bounds.formalAt?'active-startup':
      at<bounds.confirmationTailAt?'formal':at<=end?'confirmation-tail':'after-measure';
    side.trace.retainedPositiveObservations={clockMapped:clock.clockMapped,collectionComplete:side.trace.collectionValid,
      scope:'Only retained long X events are mapped. A valid marker calibration permits these positive observations despite local loss; absence, complete coverage, task-body identity and causal repair are not inferred.',
      events:clock.clockMapped?side.trace.observedData.longDurationEventsOver100Ms.filter(x=>x.e.ph==='X').map(x=>{
        const start=[x.e.ts/1000-offset[1],x.e.ts/1000-offset[0]],finish=[(x.e.ts+x.e.dur)/1000-offset[1],(x.e.ts+x.e.dur)/1000-offset[0]];
        return {index:x.index,event:x.e,pageStartBracketMs:start,pageEndBracketMs:finish,
          phaseAtStart:phase((start[0]+start[1])/2),phaseAtEnd:phase((finish[0]+finish[1])/2),
          sameAsMarkedRendererMain:x.e.pid===t.clock.renderer?.pid&&x.e.tid===t.clock.renderer?.tid,
          overlappingLongRequests:side.game.raf.requestedToActualEntry.allOver100Ms.filter(w=>start[0]<=w.enteredAt&&finish[1]>=w.requestedAt).map(w=>({
            requestIndex:w.index,phase:w.phase,requestWindowMs:[w.requestedAt,w.enteredAt],
            fullyInsideRequestWait:start[0]>=w.requestedAt&&finish[1]<=w.enteredAt,
            eventEndToActualEntryMs:[w.enteredAt-finish[1],w.enteredAt-finish[0]]}))};
      }):[]};
    if(side.trace.attribution.allowed){
      side.trace.attribution.mappedActiveLongWindows=activeLong.map(w=>{
        const lo=(w.requestedAt+offset[0])*1000,hi=(w.enteredAt+offset[1])*1000;
        const selected=t.events.map((e,index)=>({e,index})).filter(x=>x.e.ph!=='M'&&x.e.ts<=hi&&x.e.ts+(x.e.dur??0)>=lo);
        return {requestIndex:w.index,pageWindowMs:[w.requestedAt,w.enteredAt],traceEnvelopeUs:[lo,hi],events:selected,
          limitation:'Interval overlap is observation, not causation; events can nest, cross threads, and carry unknown names.'};
      });
    }
  }
  const failureText=typeof r.error==='string'?r.error:'',failed=!!failureText||r.passed===false;
  const expectedChecks=Math.floor(r.parameters.limit/120),confirmed=results.map(p=>(p.confirmedHashes??[]).slice(0,expectedChecks));
  const verifiedRows=confirmed.every(a=>a.length===expectedChecks&&a.every((row,i)=>row[0]===(i+1)*120));
  const sameConfirmed=JSON.stringify(confirmed[0])===JSON.stringify(confirmed[1]);
  return {inputs:[raw.identity,...traces.map(x=>x.identity)],gamePassed:r.passed===true,
    rawPassedField:Object.hasOwn(r,'passed')?r.passed:null,harnessOutcome:failed?'failed':r.passed===true?'passed':'unavailable',
    retainedResultSource:resultSource,
    failure:{present:failed,errorSha256:failureText?sha256(failureText):null,
      knownAssertion:failureText.includes('Actual audio output not exercised')?'Actual audio output not exercised':null,
      originalFailurePreserved:true,scope:'The known assertion is allowlisted; other raw error text remains in the SHA-identified source, not in a new unsanitized stack.'},
    rawGameErrorCount:(r.errors??[]).length,rawGameErrorsSha256:sha256(JSON.stringify(r.errors??[])),
    traceAcquisitionPassed:sides.every(s=>s.trace.collectionValid&&s.trace.clock.recomputed.clockMapped&&s.trace.rawCoverage.activeWindowCovered),
    identity:r.identity,parameters:r.parameters,sameFrame:r.sameFrame??null,finalComparable:r.finalComparable??null,confirmedChecks:r.confirmedChecks??null,
    independentChecks:{scope:'Offline observations on retained results, not reclassification of harness exit status or proof that later harness gates ran.',
      confirmed:{expectedChecks,rows:confirmed,orderedCoverage:verifiedRows,equalBetweenPeers:sameConfirmed},
      audio:results.map(p=>({enabled:r.parameters.audioOutput,outputProbeAvailable:!!p.audioOutput,
        blocks:p.audioOutput?.blocks?.length??null,nonzeroBlocks:p.audioOutput?.nonzeroBlocks??null,overflow:p.audioOutput?.overflow??null,
        originalCountGateSatisfied:r.parameters.audioOutput?((p.audioOutput?.blocks?.length??0)>100&&(p.audioOutput?.nonzeroBlocks??0)>100):null})),
      frameTape:results.map(p=>({recorded:p.measure.inputTape?.length??0,required:r.parameters.limit,
        sufficient:(p.measure.inputTape?.length??0)>=r.parameters.limit,
        prefixSha256:sha256(JSON.stringify((p.measure.inputTape??[]).slice(0,r.parameters.limit)))}))},
    laterPartialSnapshots:(r.partial??[]).map(p=>({samples:p.measure?.samples?.length??null,stats:p.stats,
      rafSnapshotAt:p.rafDiagnostics?.snapshotAt??null,scope:'Taken on the failure cleanup path; not used as the original measurement endpoint.'})),
    sides,hostLoad:r.hostLoad,hostObservations:hostAudit(r.hostLoad,sides),windowObservation:r.headedControl};
}
const args=process.argv.slice(2),replaceDerived=args[0]==='--replace-derived';if(replaceDerived)args.shift();
assert.ok(args.length>=4&&(args.length-1)%3===0,'[--replace-derived] REPORT P1_TRACE P2_TRACE [REPORT P1_TRACE P2_TRACE ...] NEW_OUTPUT');
const outputPath=args.at(-1),triples=[];for(let i=0;i<args.length-1;i+=3)triples.push(args.slice(i,i+3));
assert.ok(triples.flat().every(p=>resolve(p)!==resolve(outputPath)),'Output aliases raw input');
const runs=triples.map(analyzeRun),analyzerBytes=readFileSync(fileURLToPath(import.meta.url));
const out={kind:'th09-scheduler-analysis',version:1,
  scope:'Offline, source-identified audit of game reports and already sanitized Chrome traces. No browser or performance test is run.',
  definitions:{quantiles:'sorted[floor((n-1)*p)] in derived diagnostics; original report summaries are copied verbatim',
    longThresholdMs:100,severeThresholdMs:250,
    preActive:'Before the first request associated with sample zero',
    activeStartup:'First active request through the request before first frontier >=180',
    formal:'First frontier >=180 through the request before first frontier >= configured limit',
    confirmationTail:'First frontier >=limit until measurement ended',
    freshState:'Present count changed and logic frontier advanced; actual wrapper-entry interval is an additional observation, not physical scanout'},
  analyzer:{path:resolve(fileURLToPath(import.meta.url)).replaceAll('\\','/'),bytes:analyzerBytes.length,sha256:sha256(analyzerBytes)},
  runs,limitations:[
    'Passing gameplay correctness and failed trace collection remain separate outcomes.',
    'A run without a long active rAF gap does not establish repair of an intermittent stall.',
    'The old 5 s completion timeout can leave partial delivered data; absence of tracingComplete leaves data loss unknown.',
    'A near-full native-buffer plateau is not negated by the old >=1 full flag or by zero local overflow.',
    'Available marker arithmetic only describes observed extent when strict mapping is false; no per-frame causal mapping is produced in that case.',
    'Category presence is not complete temporal coverage. Unknown-name hashes retain timing but do not identify an operation.',
    'These traces lack per-batch arrival clocks; the drain bottleneck cannot be separated into Chrome, transport, Node scheduling, or sanitizer cost.',
    'Headed window geometry and Playwright-emulated focus do not prove an unobscured physical foreground window.',
    'Host aggregate CPU is not single-core utilization or frequency stability; process samples can miss short-lived processes.',
    'Activity from a different run is not imported as evidence for any window in this run.'
  ]};
for(const id of identities)assert.equal(sha256(readFileSync(id.path)),id.sha256,'Input changed during analysis');
if(replaceDerived){
  const previousBytes=readFileSync(outputPath),previous=JSON.parse(previousBytes);
  assert.equal(previous.kind,out.kind,'Only this analyzer derived output may be replaced');
  const retained=new Set(out.runs.map(r=>r.inputs[0].sha256));
  assert.ok(previous.runs.every(r=>retained.has(r.inputs[0].sha256)),'Replacing derived evidence must retain every prior raw run');
  out.previousDerived={bytes:previousBytes.length,sha256:sha256(previousBytes)};
}
const bytes=Buffer.from(JSON.stringify(out,null,2)+'\n');writeFileSync(outputPath,bytes,{flag:replaceDerived?'w':'wx'});
console.log(JSON.stringify({output:resolve(outputPath).replaceAll('\\','/'),bytes:bytes.length,sha256:sha256(bytes),analyzer:out.analyzer,
  runs:runs.map(r=>({raw:r.inputs[0],gamePassed:r.gamePassed,traceAcquisitionPassed:r.traceAcquisitionPassed,
    sides:r.sides.map(s=>({side:s.side,traceValid:s.trace.collectionValid,clockMapped:s.trace.clock.recomputed.clockMapped,
      retainedEvents:s.trace.counts.retainedEvents,dataSpanMs:s.trace.observedData.nonMetadataPositiveTime.spanMs,
      bufferPeak:s.trace.buffer.peakPercentFull,activeLongDispatch:s.trace.attribution.observedActiveLongDispatchCount}))}))}));



