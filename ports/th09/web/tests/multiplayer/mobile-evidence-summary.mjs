// Offline only. Usage: node mobile-evidence-summary.mjs OUTPUT.json REPORT.json [REPORT.json ...]
// Retains every supplied run. No browser, game tick, transport, or timer is created.
import {readFileSync,writeFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {fileURLToPath} from 'node:url';
const arr=Array.isArray, finite=Number.isFinite, integer=Number.isInteger, js=JSON.stringify;
const sha=v=>createHash('sha256').update(v).digest('hex'), equal=(a,b)=>js(a)===js(b);
const sum=a=>a.reduce((n,v)=>n+v,0), uint=v=>integer(v)&&v>=0&&v<=0xffffffff;
const dist=a=>{const v=a.slice().sort((x,y)=>x-y);return {count:v.length,p50:v[Math.floor(v.length*.5)]??null,p95:v[Math.floor(v.length*.95)]??null,p99:v[Math.floor(v.length*.99)]??null,max:v.at(-1)??null,total:sum(v)};};
const delta=(a,b)=>({numericVectorChanges:Number(a.x!==b.x||a.y!==b.y),bitVectorChanges:Number(a.xBits!==b.xBits||a.yBits!==b.yBits),modeChanges:Number(a.analogMode!==b.analogMode),flagsChanges:Number(a.flags!==b.flags),buttonsChanges:Number(a.buttons!==b.buttons)});
const zero=()=>({numericVectorChanges:0,bitVectorChanges:0,modeChanges:0,flagsChanges:0,buttonsChanges:0});
const add=(a,b)=>{for(const k of Object.keys(b))a[k]+=b[k];};
const wire=r=>r?{frame:r.frame,buttons:r.buttons,analogMode:r.analogMode,flags:r.flags,x:r.x,y:r.y,xBits:r.xBits,yBits:r.yBits,rawHex:r.rawHex}:null;
function hostSummary(h){
 if(!h)return {available:false};
 const cs=arr(h.cpuSamples)?h.cpuSamples:[], ps=arr(h.processSamples)?h.processSamples:[], good=cs.filter(x=>!x.counterDiscontinuity&&finite(x.busyFraction)&&x.busyFraction>=0&&x.busyFraction<=1&&finite(x.intervalMs)&&x.intervalMs>0&&finite(x.deltaTotalMs)&&x.deltaTotalMs>0&&finite(x.deltaIdleMs)&&x.deltaIdleMs>=0&&x.deltaIdleMs<=x.deltaTotalMs);
 const processes=new Map(), snapshots=ps.map(({rows,...meta},index)=>({index,...meta,retainedRows:arr(rows)?rows.length:null}));let prev=new Map();
 for(let i=0;i<ps.length;i++){
  const s=ps[i], now=new Map();
  for(const row of arr(s.rows)?s.rows:[]){
   const [pid,name,cpu]=row,key=js([pid,name]);now.set(key,cpu);
   let v=processes.get(key);if(!v){v={pid,name,observations:0,first:null,last:null,positiveDeltaCpuSeconds:0,pairedSourceIntervalMs:0,nondecreasingPairs:0,positivePairs:0,resetPairs:0,unknownPairs:0,boundaries:[]};processes.set(key,v);}
   const observation={sample:i,sourceUtcMs:s.sourceUtcMs,receivedAtMs:s.receivedAtMs,cpuSeconds:cpu};v.first??=observation;v.last=observation;v.observations++;
   if(!prev.has(key))v.boundaries.push({kind:i?'first-observed-after-absence':'left-censored-first-snapshot',fromSample:i?i-1:null,toSample:i,fromSourceUtcMs:ps[i-1]?.sourceUtcMs??null,toSourceUtcMs:s.sourceUtcMs});
   else {const dt=s.sourceUtcMs-ps[i-1].sourceUtcMs,dc=cpu-prev.get(key);
    if(finite(cpu)&&finite(prev.get(key))&&finite(dt)&&dt>0&&!s.failed&&!ps[i-1].failed){if(dc>=0){v.positiveDeltaCpuSeconds+=dc;v.pairedSourceIntervalMs+=dt;v.nondecreasingPairs++;v.positivePairs+=Number(dc>0);}else v.resetPairs++;}else v.unknownPairs++;}
  }
  for(const key of prev.keys())if(!now.has(key))processes.get(key).boundaries.push({kind:'not-observed-next-snapshot',fromSample:i-1,toSample:i,fromSourceUtcMs:ps[i-1].sourceUtcMs,toSourceUtcMs:s.sourceUtcMs});
  prev=now;
 }
 for(const key of prev.keys())processes.get(key).boundaries.push({kind:'right-censored-last-snapshot',fromSample:ps.length-1,toSample:null});
 const total=sum(good.map(x=>x.deltaTotalMs));
 return {available:true,valid:h.valid??null,complete:h.complete??null,startedAtUtcMs:h.startedAtUtcMs,endedAtUtcMs:h.endedAtUtcMs,snapshotAtMs:h.snapshotAtMs,knownProcesses:h.knownProcesses,marks:h.marks,processSampler:h.processSampler,counts:h.counts,overflow:h.overflow,errors:h.errors,
  cpu:{samples:cs.length,validDeltaIntervals:good.length,excludedFromCpuDeltaOnly:cs.length-good.length,first:cs[0]??null,last:cs.at(-1)??null,logicalCpus:[...new Set(cs.map(x=>x.logicalCpus))],counterDiscontinuities:cs.filter(x=>x.counterDiscontinuity).length,observedIntervalMs:sum(good.map(x=>x.intervalMs)),weightedBusyFraction:total?sum(good.map(x=>x.deltaTotalMs-x.deltaIdleMs))/total:null,maxBusyFraction:good.length?Math.max(...good.map(x=>x.busyFraction)):null},
  processSnapshots:snapshots,processes:[...processes.values()],limitations:h.limitations,method:'CPU occupancy uses legal counter-delta weights; no game sample is removed. Process CPU is lifetime seconds: only same pid/name in adjacent nonfailed snapshots contributes nonnegative deltas and paired source-clock intervals. Absence is not proven exit; truncation/failed snapshots and possible PID reuse remain visible. No wall-minus-CPU, compilation attribution, or per-call contention claim.'};
}
function captureContext(events,rows,delay){
 if(!arr(events)||!events.every((e,i)=>e&&integer(e.captureSlot)&&typeof e.kind==='string'&&(!i||e.captureSlot>=events[i-1].captureSlot))||!integer(delay))return null;
 const map=new Map();let ei=0,phase=null,native=null,controls=null;
 for(const r of rows){const slot=r.frame-delay;
  while(ei<events.length&&events[ei].captureSlot<=slot){const e=events[ei++];if(['pointer','stick','cancel','marker'].includes(e.kind))phase=e;if(['pointer','stick','cancel'].includes(e.kind))native=e;if(e.kind==='controls')controls=e;}
  map.set(r.frame,{captureSlot:slot,phase:slot<0?'input-delay-padding':phase?.label??'pre-gesture',epoch:phase?.epoch??null,sourceEvent:phase,latestNativeMovement:native,latestControls:controls});
 }return map;
}
function phaseActivity(rows,contexts){
 if(!contexts)return {available:false,reason:'No supported submitted capture-slot event trace'};
 const groups=new Map(),segments=[];let segment=null;
 rows.forEach((r,i)=>{const c=contexts.get(r.frame),prev=rows[i-1],pc=prev&&contexts.get(prev.frame),same=!!prev&&c.phase===pc.phase&&c.epoch===pc.epoch;
  let g=groups.get(c.phase);if(!g){g={phase:c.phase,frames:0,analogActiveFrames:0,...zero(),withinSegment:zero()};groups.set(c.phase,g);}g.frames++;g.analogActiveFrames+=Number(r.analogMode!==0);
  if(prev){const d=delta(r,prev);add(g,d);if(same)add(g.withinSegment,d);}
  if(!same){segment={phase:c.phase,epoch:c.epoch,firstFrame:r.frame,lastFrame:r.frame,frames:0,firstContext:c,withinSegment:zero()};segments.push(segment);}segment.lastFrame=r.frame;segment.frames++;if(same)add(segment.withinSegment,delta(r,prev));
 });
 const byFrame=new Map(rows.map(r=>[r.frame,r]));
 for(const s of segments)if(s.phase==='hold-stick-vector')s.boundaryWire={before:wire(byFrame.get(s.firstFrame-1)),first:wire(byFrame.get(s.firstFrame)),last:wire(byFrame.get(s.lastFrame)),after:wire(byFrame.get(s.lastFrame+1)),afterContext:contexts.get(s.lastFrame+1)??null};
 return {available:true,groups:[...groups.values()],segments,method:'Submitted phase mapped through actual wire frame minus input delay. Overall changes include phase entry; withinSegment excludes entry and requires same epoch. Numeric comparisons use exact JS !==, so +0/-0 are equal; bit changes remain separate. Control/game-state/float causality is not inferred.'};
}
function extract(path){
 let bytes,raw;try{bytes=readFileSync(path);raw=JSON.parse(bytes);}catch(error){return {path,available:false,rawSha256:bytes?sha(bytes):null,error:String(error)};}
 const r=raw.result??{},p=raw.parameters??{},limit=p.limit,checks={};const check=(name,ok)=>{checks[name]=ok===true;};
 const base={path,rawSha256:sha(bytes),rawBytes:bytes.length,available:true,declaredPassed:raw.passed===true,error:raw.error??null,errors:raw.errors??null,partialCapture:raw.partialCapture??null,cleanupErrors:raw.cleanupErrors??null,scope:raw.scope,parameters:p,identity:raw.identity,identityAfter:raw.identityAfter,browserVersion:raw.browserVersion,renderer:r.renderer,recordedScheduleSha256:raw.scheduleSha256??null};
 let host;try{host=hostSummary(raw.hostLoad);}catch(error){host={available:!!raw.hostLoad,extractionError:String(error)};}
 try{
  check('declaredComplete',raw.passed===true);check('harnessErrorsEmpty',arr(raw.errors)&&raw.errors.length===0);check('sourceStable',!!raw.identity&&!!raw.identityAfter&&equal(raw.identity,raw.identityAfter));check('limit',integer(limit)&&limit>0);
  const tapes=arr(r.localTape)?r.localTape:[null,null],inputs=arr(r.tape)?r.tape:[],contexts=[0,1].map(s=>arr(tapes[s])?captureContext(r.events?.[s],tapes[s],p.delay):null);check('twoLocalTapes',arr(r.localTape)&&r.localTape.length===2);if(['direct','joystick'].includes(p.touchProfile))check('captureSourceContexts',contexts.every(Boolean));
  check('confirmedTapeComplete',arr(r.tape)&&inputs.length===limit&&inputs.every((v,i)=>arr(v)&&v.length===9&&v[0]===i));check('endpointsComplete',arr(r.summary)&&r.summary.length===2&&r.summary.every(v=>arr(v)&&v.length===8&&v.every(x=>integer(x)&&x>=0)&&v[0]===limit&&v[1]===limit&&v[4]===limit));
  const tapeSha256=arr(r.tape)?sha(js(r.tape)):null;check('confirmedTapeSha256',!!tapeSha256&&tapeSha256===raw.tapeSha256);
  const local=tapes.map((rows,side)=>{
   if(!arr(rows)){check('localTape'+side,false);return {available:false};}
   const valid=rows.every((v,i)=>{if(!v||v.frame!==i||!/^[0-9a-f]{24}$/i.test(v.rawHex??''))return false;const b=Buffer.from(v.rawHex,'hex');return v.buttons===b.readUInt16LE(0)&&v.analogMode===b[2]&&v.flags===b[3]&&(v.word0===undefined||v.word0===b.readUInt32LE(0))&&v.xBits===b.readUInt32LE(4)&&v.yBits===b.readUInt32LE(8)&&finite(v.x)&&finite(v.y)&&v.x===b.readFloatLE(4)&&v.y===b.readFloatLE(8);});
   const prefixSha256=sha(js(rows.slice(0,limit).map(v=>[v.frame,v.rawHex])));check('localTape'+side,valid&&rows.length>=limit);check('localTapeSha256'+side,prefixSha256===raw.localTapeSha256?.[side]);
   const tupleMatches=valid&&inputs.length===limit&&inputs.every(v=>{const t=rows[v[0]],at=3+side*3;return !!t&&t.buttons===v[1+side]&&v[at]===(t.analogMode===0?0:t.analogMode===1?1:t.flags&1?3:2)&&t.x===v[at+1]&&t.y===v[at+2];});check('spectatorInputFields'+side,tupleMatches);
   return {available:true,rows:rows.length,frameRange:rows.length?[rows[0].frame,rows.at(-1).frame+1]:null,full12BFieldsValid:valid,prefixFrames:Math.min(rows.length,limit),prefixSha256,recordedPrefixSha256:raw.localTapeSha256?.[side]??null,allRowsSha256:sha(js(rows.map(v=>[v.frame,v.rawHex]))),spectatorExposedFieldsMatch:tupleMatches};
  });
  const required=integer(limit)?Math.floor(limit/120):0,traceOK=v=>arr(v)&&v.length>=required&&v.slice(0,required).every((x,i)=>arr(x)&&x.length===2&&x[0]===(i+1)*120&&uint(x[1]));
  check('confirmedHashPrefixes',required>0&&traceOK(r.confirmedHashes?.[0])&&traceOK(r.confirmedHashes?.[1])&&traceOK(r.referenceHashes)&&r.referenceHashes.length===required&&equal(r.confirmedHashes[0].slice(0,required),r.confirmedHashes[1].slice(0,required))&&equal(r.confirmedHashes[0].slice(0,required),r.referenceHashes));
  check('finalWorldHashes',arr(r.hashes)&&r.hashes.length===3&&r.hashes.every(uint)&&new Set(r.hashes).size===1);check('worldAudits',arr(r.audits)&&r.audits.length===3&&r.audits.every(v=>arr(v)&&v.length===48&&v.every(integer))&&r.audits.every(v=>equal(v,r.audits[0])));
  const replayValid=arr(r.replay)&&r.replay.length>0&&r.replay.every(x=>integer(x)&&x>=0&&x<=255),replaySha256=replayValid?sha(Uint8Array.from(r.replay)):null;check('retainedReplaySha256',replayValid&&replaySha256===raw.replaySha256&&r.replay.length===raw.replayBytes);
  check('framebufferReportedEqual',r.framebuffersEqual===true);
  const peers=[0,1].map(side=>{
   const rows=r.samples?.[side],tape=tapes[side],stats=r.summary?.[side],positions=r.positions?.[side],bad=arr(rows)?rows.flatMap((v,i)=>!arr(v)||v.length!==7||!v.every(finite)||![0,1,2,3,5,6].every(j=>integer(v[j]))||v.slice(0,5).some(x=>x<0)||v[2]>1||v[6]<0||v[6]>8?[i]:[]):null;
   check('samples'+side,arr(rows)&&rows.length>0&&bad.length===0);if(!arr(rows)||!rows.length||bad.length)return {side,available:false,stats,invalidSampleCount:bad?.length??null,invalidSamples:bad?.slice(0,8).map(i=>({index:i,sample:rows[i]}))??null};
   const costs={},visits=new Map(),starts=new Map(),unknown=[],returns={},depths=[];let resim=0,corr=0;
   const bucket=(map,key)=>{if(!map.has(key))map.set(key,{phase:key,frames:new Set(),calls:new Set(),visits:0,costs:[]});return map.get(key);};
   const ctx=(who,frame)=>contexts[who]?.get(frame)??{phase:'unavailable',frame};const contextAt=frame=>({frame,local:ctx(side,frame),opponent:ctx(1-side,frame)});
   rows.forEach((v,i)=>{
    const [before,after,c,rs,ms,code,depth]=v,advance=after-before,name=c||rs?'recovery-mixed':advance===0?'no-forward-no-recovery':advance===1?'one-fresh':advance>1?'multi-fresh':'unexplained-negative';
    (costs[name]??=[]).push(ms);returns[code]=(returns[code]??0)+1;resim+=rs;corr+=c;if(c)depths.push(depth);
    if(c||rs){const end=after-Number(code===1),start=end-rs,expected=before-(c?depth:0);
     if(start<0||end<start||start!==expected){unknown.push({index:i,sample:v,reason:'recovery-frame reconstruction mismatch'});return;}
     for(let frame=start;frame<end;frame++){const b=bucket(visits,ctx(1-side,frame).phase);b.frames.add(frame);b.calls.add(i);b.visits++;}
     if(c){const b=bucket(starts,ctx(1-side,start).phase);b.frames.add(start);b.calls.add(i);b.costs.push(ms);}
    }
   });
   const all=dist(rows.map(v=>v[4])),part=Object.fromEntries(Object.entries(costs).map(([k,v])=>[k,dist(v)])),partTotal=sum(Object.values(part).map(v=>v.total));
   check('costConservation'+side,sum(Object.values(part).map(v=>v.count))===all.count&&Math.abs(partTotal-all.total)<=Math.max(1e-7,Math.abs(all.total)*1e-12));check('counterSums'+side,!!stats&&corr===stats[2]&&resim===stats[3]);check('sampleContinuity'+side,rows.every((v,i)=>!i||v[0]===rows[i-1][1]));check('return2Count'+side,returns[2]===r.waits?.[side]||(returns[2]===undefined&&r.waits?.[side]===0));check('recoveryFormulaConsistency'+side,unknown.length===0);check('recordedAllCallDistribution'+side,Object.keys(all).every(k=>all[k]===r.measured?.[side]?.allCalls?.[k]));check('resimVisitConservation'+side,sum([...visits.values()].map(b=>b.visits))+sum(unknown.map(v=>v.sample[3]))===resim);check('restoreStartConservation'+side,sum([...starts.values()].map(b=>b.calls.size))+sum(unknown.map(v=>v.sample[2]))===corr);
   const top=rows.map((sample,index)=>({sample,index})).sort((a,b)=>b.sample[4]-a.sample[4]||a.index-b.index).slice(0,3).map(v=>({...v,atBeforeNext:contextAt(v.sample[0]),atRestoreStart:v.sample[2]?contextAt(v.sample[0]-v.sample[6]):null,atResimulationStart:v.sample[3]?contextAt(v.sample[1]-Number(v.sample[5]===1)-v.sample[3]):null,atExitLastSimulatedPrefixFrame:v.sample[1]>0?contextAt(v.sample[1]-1):null}));
   const movement=arr(tape)?{wireFrames:tape.length,analogActiveFrames:tape.filter(v=>v.analogMode!==0).length,allTransitions:zero(),sameActiveModeTransitions:zero()}:null;
   if(movement)tape.forEach((v,i)=>{if(i){const d=delta(v,tape[i-1]);add(movement.allTransitions,d);if(v.analogMode!==0&&v.analogMode===tape[i-1].analogMode)add(movement.sameActiveModeTransitions,d);}});
   return {side,available:true,stats,corrections:corr,resimulatedFrames:resim,captures:stats?.[4]??null,channelRepairsSent:stats?.[5]??null,returnCodes:returns,returnCode2WaitOrRecovery:returns[2]??0,recordedReturnCode2:r.waits?.[side]??null,allCallElapsedMs:all,costPartition:part,
    reconstructedCursorRestoreDistance:dist(depths),afterNextBelowBeforeCalls:rows.filter(v=>v[1]<v[0]).length,resimulatedDeltaPerCall:dist(rows.map(v=>v[3])),topCalls:top,recordedMeasured:r.measured?.[side]??null,recordedActivity:r.activity?.[side]??null,
    movement:{...movement,newFrontierObservations:arr(positions)?positions.length:null,positionChangedObservations:arr(positions)?positions.filter((v,i)=>i>0&&(v[1]!==positions[i-1][1]||v[2]!==positions[i-1][2])).length:null},capturePhases:arr(tape)?phaseActivity(tape,contexts[side]):null,
    opponentResimulationPhases:[...visits.values()].map(b=>({phase:b.phase,resimulationFrameVisits:b.visits,uniqueFrames:b.frames.size,distinctCalls:b.calls.size})),
    opponentRestoreStartPhases:[...starts.values()].map(b=>({phase:b.phase,correctionCalls:b.calls.size,uniqueStartFrames:b.frames.size,wholeAssociatedCallElapsedMs:dist(b.costs)})),unreconstructedCalls:unknown,unavailableOpponentFrameVisits:visits.get('unavailable')?.visits??0};
  });
  return {...base,checks,verificationPassed:Object.values(checks).every(Boolean),oracle:{summary:r.summary,hashes:r.hashes,audits:r.audits,confirmedHashes:r.confirmedHashes,referenceHashes:r.referenceHashes,requiredConfirmedCheckpoints:required,confirmedHashTailCounts:[0,1].map(s=>arr(r.confirmedHashes?.[s])?Math.max(0,r.confirmedHashes[s].length-required):null),recordedChecks:r.checks,framebuffersEqualReported:r.framebuffersEqual??null,replay:{bytes:r.replay?.length??null,sha256:replaySha256,recordedSha256:raw.replaySha256??null},tape:{rows:inputs.length,sha256:tapeSha256,recordedSha256:raw.tapeSha256??null,local},wireVerification:r.wireVerification??null,
   evidenceBoundary:'Equality of the retained final hashes/audits and confirmed prefixes is rechecked; tape and retained Replay SHA values are recomputed from saved bytes. Framebuffer bytes and the other two Replay copies are absent, so their three-world equality remains a harness-reported assertion. Spectator tuples expose buttons/motion/x/y, not all wire flags.'},
   peers,iterations:r.iterations,deliveryFrontier:r.deliveryFrontier,observationClock:r.observationClock,host};
 }catch(error){return {...base,checks,verificationPassed:false,extractionError:String(error),rawResultKeys:Object.keys(r),host};}
}
const [output,...paths]=process.argv.slice(2);
if(!output||!paths.length)throw Error('Usage: node mobile-evidence-summary.mjs OUTPUT.json REPORT.json [REPORT.json ...]');
const runs=paths.map(extract),keys=['mode','delay','limit','hardware','sourceHz','deliveryClock','fixedDelay','cpuRate','hostLoad','maximumIterations'];
const comparison=values=>({values,comparable:values.every(v=>v!==null),equal:values.every(v=>v!==null)?values.every(v=>equal(v,values[0])):null});
const shared=Object.fromEntries(keys.map(k=>[k,comparison(runs.map(r=>r.parameters?.[k]??null))]));
const sourceKeys=['harness','touchProbe','fixture','hostLoadProbe'],sharedSources=Object.fromEntries(sourceKeys.map(k=>[k,comparison(runs.map(r=>r.identity?.[k]??null))]));
const groups=[...new Set(runs.map(r=>r.parameters?.touchProfile??'unknown'))].map(profile=>{const indices=runs.flatMap((r,i)=>(r.parameters?.touchProfile??'unknown')===profile?[i]:[]),group=indices.map(i=>runs[i]),values=group.map(r=>({scheduleSha256:r.recordedScheduleSha256,confirmedTapeSha256:r.oracle?.tape.sha256??null,localTapeSha256:r.oracle?.tape.local.map(t=>t.prefixSha256??null)??null,replaySha256:r.oracle?.replay.sha256??null}));return {profile,runIndices:indices,values,comparisonAvailable:values.length>1,identical:values.length>1?values.every(v=>[v.scheduleSha256,v.confirmedTapeSha256,v.replaySha256,...(v.localTapeSha256??[])].every(x=>typeof x==='string'&&/^[0-9a-f]{64}$/.test(x))&&v.localTapeSha256?.length===2)&&values.every(v=>equal(v,values[0])):null};});
const evidence={schema:'th09-mobile-three-world-summary/v2',analyzer:{path:fileURLToPath(import.meta.url),sha256:sha(readFileSync(fileURLToPath(import.meta.url)))},inputs:paths,runs,comparison:{sharedParameters:shared,sharedObservationSources:sharedSources,withinProfileInputEquality:groups},
 methods:{allCalls:'Every sampled game_tick(1) call in each rollback world is retained, including startup, waits and recovery; elapsed includes synchronous JS transport/tape hooks. Third-world oracle tick costs are not sampled here. No rewind-cursor filtering and no FPS computed from three-world setup/oracle wall time.',
 recovery:'For recovery rows: end = afterNext - (returnCode===1 ? 1 : 0); start = end - resimulatedDelta. Frame visits [start,end) join opponent 1-side actual captured wire to submitted capture-slot phase. Repeated visits count work; they do not prove which input caused correction. Whole call cost is only in exclusive restore-start buckets, never copied into every visited phase.',
 reconstruction:'Source-derived single game_tick formula, not an independently observed restore point. For correction rows, the harness stored depth from the same formula, so the reverse check is algebraically self-consistent. Cursor restore distance is beforeNext minus restoreStart, not necessarily predictionFrontier minus restoreStart during partial recovery. Continued-recovery rows and bounds remain checked.',
 counters:'stats[5] is SessionChannel RepairsSent: only SendRepairTo returning true increments it. Browser RTC success means send API accepted, not receipt or repair completion; browser relay returns false. This manual harness supplies a simulated transport. Return code 2 also covers not-ready, Prepare blockage, resource exit barriers and partial recovery, not solely missing network information.',
 retainedLabels:'recordedActivity.partialRecoveryCalls counts afterNext below beforeNext, not every unfinished recovery call. recordedMeasured and recordedActivity are preserved harness metadata. one-fresh still includes synchronous bridges/draw/audio; an exit prefix anchor may not have been simulated in that call. full12BFieldsValid checks wire/decoded-field consistency, not an independent complete ValidInput implementation.',
 context:'Top calls contain the original seven fields and both sides at logical frame anchors. These are input-frame context, not proof a fresh capture occurred in that call or wall-clock gesture/latency evidence. Source control context and numeric/bit changes are retained without assigning cause.',
 scope:'Deterministic native ABI, synthetic frame-delayed transport, three worlds in the same CPU-throttled renderer. Not real relay/direct networking, real phone, hosted touch bridge, input-to-visible latency or gameplay Hz. Unknown/failed runs remain; cross-run tape inequality is reported, not discarded.'}};
writeFileSync(output,js(evidence,null,2)+'\n',{flag:'wx'});
console.log(js({output,analyzer:evidence.analyzer,runs:runs.map(r=>({path:r.path,rawSha256:r.rawSha256,declaredPassed:r.declaredPassed,verificationPassed:r.verificationPassed,failedChecks:Object.entries(r.checks??{}).filter(([,v])=>!v).map(([k])=>k),error:r.error??r.extractionError??null,peers:r.peers?.map(p=>({side:p.side,corrections:p.corrections,resim:p.resimulatedFrames,channelRepairsSent:p.channelRepairsSent,return2:p.returnCode2WaitOrRecovery,allCallElapsedMs:p.allCallElapsedMs}))})),comparison:evidence.comparison}));
if(runs.some(r=>!r.verificationPassed))process.exitCode=1;

