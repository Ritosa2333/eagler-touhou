// Offline comparison only. No browser, build, relay, subprocess, or timers.
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {createHash} from 'node:crypto';
import {fileURLToPath} from 'node:url';
const [headlessPath,preparationPath,outputPath,...headedPaths]=process.argv.slice(2);
assert(headedPaths.length&&headlessPath&&preparationPath&&outputPath,'Usage: node analyze-headed-control.mjs headless-failure headed-preparation-failure NEW-output headed-report...');
const hash=value=>createHash('sha256').update(value).digest('hex');
const read=file=>{const bytes=fs.readFileSync(file);return {input:{path:path.resolve(file),bytes:bytes.length,sha256:hash(bytes)},raw:JSON.parse(bytes)};};
const inputs=[headlessPath,preparationPath,...headedPaths].map(read);
assert(inputs.every(x=>x.input.path!==path.resolve(outputPath)),'Never overwrite a raw input');
const [headless,preparation,...headedInputs]=inputs;
assert(headedInputs.every(run=>run.raw.passed&&run.raw.results.length===2),'Expected completed headed measurements');
assert(headless.raw.error&&headless.raw.partial.length===2,'Expected retained headless failure');
assert(preparation.raw.error&&preparation.raw.partial.length===0,'Expected preparation-only failure');
const finite=Number.isFinite;
function dist(values){
 assert(values.every(finite),'Nonfinite metric is not silently dropped');const sorted=values.toSorted((a,b)=>a-b);
 return {count:sorted.length,total:sorted.reduce((s,x)=>s+x,0),min:sorted[0]??null,p50:sorted[Math.floor(sorted.length*.5)]??null,p95:sorted[Math.floor(sorted.length*.95)]??null,p99:sorted[Math.floor(sorted.length*.99)]??null,max:sorted.at(-1)??null};
}
function hostEvidence(h){
 assert(h?.valid&&!Object.values(h.overflow).some(Boolean),'Host observation must retain full coverage');
 const compiler=/^(?:clang(?:\+\+)?|cl|gcc|g\+\+|cc1(?:plus)?|wasm-opt|wasm-ld|lld(?:-link)?|link|ninja|cmake|make|msbuild|emcc|em\+\+|tsc)$/i;
 const series=new Map(),compilerSamples=[];
 for(const [sampleIndex,sample] of h.processSamples.entries()){
  const rows=sample.rows.filter(row=>compiler.test(row[1]));
  compilerSamples.push({sampleIndex,sourceUtcMs:sample.sourceUtcMs,receivedAtMs:sample.receivedAtMs,rows});
  for(const [pid,name,cpuSeconds] of rows){const key=pid+':'+name;let s=series.get(key);if(!s){s={pid,name,observations:[]};series.set(key,s);}s.observations.push({sampleIndex,sourceUtcMs:sample.sourceUtcMs,cpuSeconds});}
 }
 const compilerProcesses=[...series.values()].map(s=>({...s,firstCpuSeconds:s.observations[0].cpuSeconds,lastCpuSeconds:s.observations.at(-1).cpuSeconds,observedIncreaseSeconds:s.observations.length>1?s.observations.at(-1).cpuSeconds-s.observations[0].cpuSeconds:null}));
 const cpu=h.cpuSamples.filter(row=>finite(row.busyFraction)).map(row=>row.busyFraction*100);
 return {raw:h,summary:{cpuBusyPercent:dist(cpu),cpuSamplesWithoutInterval:h.cpuSamples.length-cpu.length,compilerSamples,compilerProcesses},limitations:['Every original host row is retained; no gameplay result is filtered using host load.','A single compiler CPU observation is lifetime cumulative CPU, not measured CPU growth during this interval. Other node/python processes remain candidates, not classified compilations.','Aggregate logical-CPU utilization cannot establish single-core contention, clock stability, or GPU occupancy; 5s process samples miss short exited tasks.']};
}
function relayEvidence(t){
 assert(t?.valid,'Relay trace incomplete');
 const decode=(columns,row)=>Object.fromEntries(columns.map((key,i)=>[key,row[i]]));
 const packets=new Map(t.packets.map(row=>{const p=decode(t.columns.packet,row);return [p.id,p];}));
 const events=t.events.map(row=>decode(t.columns.event,row)),phase=e=>t.columns.phases[e.phase];
 const receives=events.filter(e=>phase(e)==='receive'),sends=events.filter(e=>phase(e)==='send'),forwardByReceive=new Map();
 for(const e of sends){if(!forwardByReceive.has(e.receiveEventId))forwardByReceive.set(e.receiveEventId,[]);forwardByReceive.get(e.receiveEventId).push(e);}
 const describe=e=>({event:e,packet:packets.get(e.packetId)}),receiveIds=new Set(receives.map(e=>e.id));
 const receiveToSend=[];for(const e of receives)for(const send of forwardByReceive.get(e.id)||[])receiveToSend.push({ms:send.enteredAt-e.enteredAt,receive:e,send});
 const range=values=>values.length?[Math.min(...values),Math.max(...values)]:null;
 const senders=[0,1].map(sender=>{
  const rs=receives.filter(e=>packets.get(e.packetId).sender===sender).toSorted((a,b)=>a.enteredAt-b.enteredAt);
  const ss=sends.filter(e=>packets.get(e.packetId).sender===sender).toSorted((a,b)=>a.enteredAt-b.enteredAt),bins=new Map();
  for(const [kind,rows] of [['receive',rs],['send',ss]])for(const e of rows){const second=Math.floor(e.enteredAt/1000);if(!bins.has(second))bins.set(second,{at:second*1000,receive:[],send:[]});bins.get(second)[kind].push(e);}
  const gaps=rows=>rows.slice(1).map((e,i)=>({ms:e.enteredAt-rows[i].enteredAt,before:rows[i],after:e}));
  const rg=gaps(rs),sg=gaps(ss);
  return {sender,receiveCount:rs.length,sendCount:ss.length,firstReceive:rs[0]?describe(rs[0]):null,lastReceive:rs.length?describe(rs.at(-1)):null,receiveGapMs:dist(rg.map(x=>x.ms)),sendGapMs:dist(sg.map(x=>x.ms)),
   allReceiveGapsOver100Ms:rg.filter(x=>x.ms>100).map(x=>({ms:x.ms,before:describe(x.before),after:describe(x.after)})),allSendGapsOver100Ms:sg.filter(x=>x.ms>100).map(x=>({ms:x.ms,before:describe(x.before),after:describe(x.after)})),
   windows:[...bins.values()].toSorted((a,b)=>a.at-b.at).map(bin=>{const ps=bin.receive.map(e=>packets.get(e.packetId));return {at:bin.at,epochMs:t.clock.started.timeOrigin+bin.at,receiveCount:bin.receive.length,sendCount:bin.send.length,sequenceRange:range(ps.map(p=>p.sequence)),latestFrameRange:range(ps.map(p=>p.latestFrame)),ackFrameRange:range(ps.map(p=>p.ackFrame).filter(f=>f!==4294967295)),invalidAckCount:ps.filter(p=>p.ackFrame===4294967295).length};})};
 });
 const timing=t.timing.map(row=>decode(t.columns.timing,row));
 return {sourceIdentity:t.sourceIdentity,clock:t.clock,valid:t.valid,coverage:t.coverage,counts:t.counts,overflow:t.overflow,errors:t.errors,pending:t.pending,senders,
  linkage:{receiveCount:receives.length,sendCount:sends.length,matchedSendCount:receiveToSend.length,allReceivesWithoutObservedSend:receives.filter(e=>!forwardByReceive.has(e.id)).map(describe),allSendsWithoutObservedReceive:sends.filter(e=>!receiveIds.has(e.receiveEventId)).map(describe)},
  receiveToSendMs:dist(receiveToSend.map(x=>x.ms)),allReceiveToSendOver100Ms:receiveToSend.filter(x=>x.ms>100).map(x=>({ms:x.ms,receive:describe(x.receive),send:describe(x.send)})),
  timing:{count:timing.length,lateMs:dist(timing.map(x=>x.lateMs)),gapMs:dist(timing.map(x=>x.gapMs)),allLateOrGapOver100Ms:timing.filter(x=>x.lateMs>100||x.gapMs>100)},
  scope:'Full-run relay receive/send counts and 1-second windows, plus every >100ms gap and unmatched record. Browser intent/actual-send/message/native-consumption packet trace was disabled in these release runs; relay timing cannot supply those missing segments. A relay send entry is a WS API call, not wire delivery.'};
}
function pacingEvidence(peer){
 const m=peer.measure,rows=m.networkPacing||[];
 if(!rows.length)return {available:false,reason:'No pacing export observations retained in this raw'};
 assert(rows.every(row=>row.length===7&&row.every(finite)&&row[1]===1&&row[0]>=0&&row[0]<m.samples.length),'Pacing schema changed');
 const at=row=>m.started+m.samples[row[0]][0],describe=row=>({row,sampleAt:at(row),sample:m.samples[row[0]]});
 const episodes=[];let current=null;
 for(const row of rows){
  if(row[3]>1000){if(!current){current={first:describe(row),last:describe(row),samples:0,peakOutstanding:0,peakNetworkScaleMilli:0,peakEffectiveScaleMilli:0};episodes.push(current);}++current.samples;current.last=describe(row);current.peakOutstanding=Math.max(current.peakOutstanding,row[2]);current.peakNetworkScaleMilli=Math.max(current.peakNetworkScaleMilli,row[3]);current.peakEffectiveScaleMilli=Math.max(current.peakEffectiveScaleMilli,row[4]);}
  else if(current){current.firstObservedUnbraked=describe(row);current=null;}
 }
 for(const episode of episodes){episode.observedSampleSpanMs=episode.last.sampleAt-episode.first.sampleAt;episode.rightCensored=!episode.firstObservedUnbraked;}
 return {available:true,columns:['sampleIndex','version','outstandingFrames','networkIntervalScaleMilli','effectiveIntervalScaleMilli','frontier','actualContiguousExclusive'],count:rows.length,first:describe(rows[0]),last:describe(rows.at(-1)),
  outstandingFrames:dist(rows.map(row=>row[2])),networkScaleMilli:dist(rows.map(row=>row[3])),effectiveScaleMilli:dist(rows.map(row=>row[4])),networkBrakedSamples:rows.filter(row=>row[3]>1000).length,
  allNetworkBrakingEpisodes:episodes,largestOutstandingSamples:rows.toSorted((a,b)=>b[2]-a[2]||a[0]-b[0]).slice(0,10).map(describe),
  scope:'Every retained pacing row contributes to distributions. All contiguous observed networkScale>1000 episodes retained; other largest-outstanding rows are selected explicitly. Sample spans bound observation times, not exact actuation duration. Effective scale may reflect phase pacing below 1000 when network scale is 1000.'};
}
function peerEvidence(peer,side){
 const m=peer.measure,d=peer.rafDiagnostics,a=peer.audioOutput;
 assert(d?.valid&&!d.overflow&&d.hookErrors.length===0,'RAF observation incomplete');
 assert(m.samples.every(row=>row.length===14&&row.every(finite)),'Original sample shape changed');
 const requests=new Map(d.requests.map(row=>[row.index,row])),framesByRequest=new Map(),frameBySample=new Map();
 for(const f of d.frames){
  if(!framesByRequest.has(f.rafIndex))framesByRequest.set(f.rafIndex,[]);framesByRequest.get(f.rafIndex).push(f);
  for(let i=f.sampleBefore;i<f.sampleAfter;++i){assert(!frameBySample.has(i));frameBySample.set(i,f);}
 }
 assert.equal(frameBySample.size,m.samples.length,'Every retained sample must have its frame hook');
 const activeEntry=requests.get(frameBySample.get(0)?.rafIndex)?.enteredAt??m.started;
 const formalIndex=m.samples.findIndex(s=>s[3]>=180),formalEntry=formalIndex<0?null:requests.get(frameBySample.get(formalIndex).rafIndex).enteredAt;
 const phase=at=>at<activeEntry?'pre-active':m.ended&&at>=m.ended?'after-measurement':formalEntry===null||at<formalEntry?'startup-active':'formal-active';
 const samplePhase=s=>s[3]<180?'startup-active':s[3]<m.limit?'formal-active':'limit-tail';
 const sample=index=>{const s=m.samples[index],f=frameBySample.get(index);return {index,raw:s,sampleAt:m.started+s[0],frame:f};};
 const request=r=>r?{...r,phase:phase(r.enteredAt??r.requestedAt),dispatchMs:finite(r.enteredAt)?r.enteredAt-r.requestedAt:null,wrapperMs:finite(r.enteredAt)&&finite(r.exitedAt)?r.exitedAt-r.enteredAt:null,frames:(framesByRequest.get(r.index)||[]).map(f=>({...f,sampleIndices:Array.from({length:f.sampleAfter-f.sampleBefore},(_,i)=>f.sampleBefore+i)}))}:null;
 const entered=d.requests.filter(r=>finite(r.enteredAt)).toSorted((a,b)=>a.enteredAt-b.enteredAt||a.index-b.index);
 const entryGaps=entered.slice(1).map((r,i)=>({ms:r.enteredAt-entered[i].enteredAt,before:entered[i],after:r}));
 const dispatch=entered.map(r=>({ms:r.enteredAt-r.requestedAt,request:r}));
 const wrappers=entered.filter(r=>finite(r.exitedAt)).map(r=>({ms:r.exitedAt-r.enteredAt,request:r}));
 const byPhase=(rows,timeOf)=>Object.fromEntries(['pre-active','startup-active','formal-active','after-measurement'].map(p=>[p,dist(rows.filter(row=>phase(timeOf(row))===p).map(row=>row.ms))]));
 const sampleGaps=m.samples.slice(1).map((s,i)=>({ms:s[0]-m.samples[i][0],before:i,after:i+1}));
 const submissions=[],effective=[];let priorPresent=null,priorEffective=null;
 for(let i=0;i<m.samples.length;++i){const s=m.samples[i];
  if(priorPresent===null||s[11]!==m.samples[priorPresent][11]){if(priorPresent!==null)submissions.push({ms:s[13]-m.samples[priorPresent][13],sampleTimeGapMs:s[0]-m.samples[priorPresent][0],before:priorPresent,after:i});priorPresent=i;}
  if(priorEffective===null||(s[11]!==m.samples[priorEffective][11]&&s[3]>m.samples[priorEffective][3])){if(priorEffective!==null)effective.push({ms:s[13]-m.samples[priorEffective][13],sampleTimeGapMs:s[0]-m.samples[priorEffective][0],before:priorEffective,after:i});priorEffective=i;}
 }
 const samplesMetric=rows=>Object.fromEntries(['startup-active','formal-active','limit-tail'].map(p=>[p,dist(rows.filter(row=>samplePhase(m.samples[row.after])===p).map(row=>row.ms))]));
 const decorateGap=row=>({...row,before:sample(row.before),after:sample(row.after)});
 const ordinary=[];const excluded={firstWithoutPrior:1,notExactlyOneForward:0,correctionOrReplay:0,notExactlyOneCapture:0,notExactlyOneSubmission:0,waitResult:0};
 for(let i=1;i<m.samples.length;++i){const s=m.samples[i],p=m.samples[i-1];
  if(s[3]-p[3]!==1){++excluded.notExactlyOneForward;continue;}
  if(s[5]!==p[5]||s[6]!==p[6]){++excluded.correctionOrReplay;continue;}
  if(s[7]-p[7]!==1){++excluded.notExactlyOneCapture;continue;}
  if(s[11]-p[11]!==1){++excluded.notExactlyOneSubmission;continue;}
  if(s[1]===2){++excluded.waitResult;continue;}ordinary.push(i);
 }
 assert.equal(ordinary.length+Object.values(excluded).reduce((s,n)=>s+n,0),m.samples.length);
 const oneForwardNoReplay=m.samples.flatMap((s,i)=>i>0&&s[3]-m.samples[i-1][3]===1&&s[5]===m.samples[i-1][5]&&s[6]===m.samples[i-1][6]?[i]:[]);
 const cppGroup=indices=>({indices,cppWorkMs:dist(indices.map(i=>m.samples[i][2])),wrapperMs:dist(indices.map(i=>{const r=requests.get(frameBySample.get(i).rafIndex);return r.exitedAt-r.enteredAt;})),slowest:indices.toSorted((a,b)=>m.samples[b][2]-m.samples[a][2]).slice(0,5).map(sample)});
 assert(a&&!a.overflow,'Audio probe incomplete');const zeroIndices=[];
 for(const [i,block] of a.blocks.entries()){assert(block.length===5&&block.every(finite));if(block[4]<=1e-6)zeroIndices.push(i);}
 assert.equal(a.blocks.length-zeroIndices.length,a.nonzeroBlocks,'Independently counted nonzero blocks');
 const audioGaps=a.blocks.slice(1).map((b,i)=>({ms:b[0]-a.blocks[i][0],beforeIndex:i,afterIndex:i+1,before:a.blocks[i],after:b}));
 const pending=d.pendingIndices.map(index=>{const r=requests.get(index);assert(r&&!finite(r.enteredAt));return {request:request(r),parent:request(requests.get(r.parentIndex)),snapshotAgeMs:d.snapshotAt-r.requestedAt,snapshotPhase:phase(d.snapshotAt),controlsAfterRequest:d.controls.filter(c=>c.enteredAt>=r.requestedAt)};});
 return {side,finalStats:peer.stats,renderer:peer.renderer??null,started:m.started,activeRafEntry:activeEntry,formalRafEntry:formalEntry,ended:m.ended,limit:m.limit,finished:m.finished,
  sampleCount:m.samples.length,firstSamples:m.samples.slice(0,10).map((_,i)=>sample(i)),lastSamples:m.samples.slice(-3).map((_,i)=>sample(m.samples.length-Math.min(3,m.samples.length)+i)),notices:m.notices,closed:m.closed,lifecycle:peer.lifecycle,
  raf:{valid:d.valid,counts:d.counts,snapshotAt:d.snapshotAt,overflow:d.overflow,hookErrors:d.hookErrors,callbacks:d.callbacks,controls:d.controls,pending,
   wrapperEntryGapMs:byPhase(entryGaps,row=>row.after.enteredAt),requestDispatchMs:byPhase(dispatch,row=>row.request.enteredAt),wrapperDurationMs:byPhase(wrappers,row=>row.request.enteredAt),
   allEntryGapsOver100Ms:entryGaps.filter(row=>row.ms>100).map(row=>({ms:row.ms,before:request(row.before),after:request(row.after)})),
   allDispatchOver100Ms:dispatch.filter(row=>row.ms>100).map(row=>({ms:row.ms,request:request(row.request)})),
   allWrapperOver100Ms:wrappers.filter(row=>row.ms>100).map(row=>({ms:row.ms,request:request(row.request)}))},
  samples:{allCppWorkMs:dist(m.samples.map(s=>s[2])),cppWorkByPhase:Object.fromEntries(['startup-active','formal-active','limit-tail'].map(p=>[p,dist(m.samples.filter(s=>samplePhase(s)===p).map(s=>s[2]))])),
   sampleTimestampGapMs:samplesMetric(sampleGaps),submissionRafTimestampGapMs:samplesMetric(submissions),effectiveRafTimestampGapMs:samplesMetric(effective),
   allCppWorkOver100Ms:m.samples.flatMap((s,i)=>s[2]>100?[sample(i)]:[]),allSampleGapsOver100Ms:sampleGaps.filter(row=>row.ms>100).map(decorateGap),
   allSubmissionGapsOver100Ms:submissions.filter(row=>row.ms>100||row.sampleTimeGapMs>100).map(decorateGap),allEffectiveGapsOver100Ms:effective.filter(row=>row.ms>100||row.sampleTimeGapMs>100).map(decorateGap),
   oneForwardNoReplay:Object.fromEntries(['startup-active','formal-active','limit-tail'].map(p=>[p,cppGroup(oneForwardNoReplay.filter(i=>samplePhase(m.samples[i])===p))])),
   strictOneForwardOneCaptureOneSubmission:{excludedPartition:excluded,groups:Object.fromEntries(['startup-active','formal-active','limit-tail'].map(p=>[p,cppGroup(ordinary.filter(i=>samplePhase(m.samples[i])===p))]))}},
  pacing:pacingEvidence(peer),impairment:peer.impairment?{matched:peer.impairment.matched,sent:peer.impairment.sent,dropped:peer.impairment.dropped,pending:peer.impairment.pending,peakPending:peer.impairment.peakPending,lanes:peer.impairment.lanes,syncCalls:peer.impairment.syncCalls,plannedMs:dist(peer.impairment.planned),deliveredMs:dist(peer.impairment.delivered)}:null,
  audio:{...a,blockCount:a.blocks.length,zeroIndices,gapMs:dist(audioGaps.map(row=>row.ms)),allGapsOver100Ms:audioGaps.filter(row=>row.ms>100),relativeSpanMs:a.blocks.at(-1)?.[0]??0,
   scope:'Peak <=1e-6 means zero among sparse channel-0 samples. It is not proof of all-channel silence or speaker output. Audio first-block performance.now origin was not retained, even when a page timeOrigin is available; do not invent exact RAF alignment.'}};
}
function invocationEvidence(run){
 const file=run.input.path.replace(/-(?:report|failure)\.json$/,'-invocation.json');
 if(file===run.input.path||!fs.existsSync(file))return {available:false};
 const evidence=read(file),r=evidence.raw;
 return {available:true,input:evidence.input,startedAt:r.startedAt,endedAt:r.endedAt,elapsedMs:r.elapsedMs,status:r.status,signal:r.signal,runLabel:r.runLabel,scriptSha256:r.scriptSha256,scriptAfterSha256:r.scriptAfterSha256,
  scope:'Outer command duration includes setup, polling, report capture and teardown. It is not game measurement duration; no raw command line, environment, or process log is copied.'};
}
function playwrightObservation(){
 const file=process.env.PLAYWRIGHT_CORE_BUNDLE;
 if(!file)return {available:false,scope:'No current dependency source observation supplied; historical launch conditions are not inferred.'};
 const bytes=fs.readFileSync(file),lines=bytes.toString('utf8').split(/\r?\n/),selected=[34856,34857,34878,37633,37634,37637,37638].map(line=>({line,text:lines[line-1]}));
 assert(selected.some(x=>x.text.includes('Emulation.setFocusEmulationEnabled')),'Recheck current focus-emulation source');
 assert(selected.filter(x=>x.text.includes('--disable-')).length===3,'Recheck current background defaults');
 return {available:true,input:{path:path.resolve(file),bytes:bytes.length,sha256:hash(bytes)},selected,
  scope:'Read-only observation of the current Playwright dependency, not a browser trace or retrospective file snapshot. Under the current noDefaults=false main-frame flow, document.hasFocus may be emulated. The recorded true values cannot prove foreground, non-overlap, or physical visibility; these defaults alone do not identify the rAF cause.'};
}
function runEvidence(run,peers){
 const r=run.raw;
 return {input:run.input,sourceIdentity:r.identity,parameters:r.parameters,outcome:{passed:r.passed===true,error:r.error?.split('\n')[0]??null,errors:r.errors},rawSummary:r.summary??null,
  finalComparable:r.finalComparable??null,confirmedChecks:r.confirmedChecks??null,headedControl:r.headedControl??null,
  correctness:peers.map((p,side)=>({side,inputTapeSha256:p.inputTapeSha256??null,capturedTapeLength:p.measure.inputTape.length,confirmedHashes:p.confirmedHashes,finalStats:p.stats})),
  invocation:invocationEvidence(run),peers:peers.map(peerEvidence),relay:relayEvidence(r.relayTransport),host:hostEvidence(r.hostLoad)};
}
const result={schema:'th09/headed-control-comparison/1',analyzer:{path:fileURLToPath(import.meta.url),sha256:hash(fs.readFileSync(fileURLToPath(import.meta.url)))},
 definitions:{sampleColumns:['relativeSampleTime','ok','cppWorkMs','frontier','confirmedExclusive','corrections','resimulated','captures','repairsSent','ready','publishedExclusive','PresentCount','sessionPhase','rafTimestamp'],
  quantiles:'floor(n*q), matching the original harness; all selected samples contribute. No trimming, winsorization, host-load filtering, or failure deletion.',
  timing:'measure.started+sample[0] is the sample timestamp after play.frame and before subsequent observer work, not a C++ or rAF exit. Actual rAF wrapper and JS onGameFrame entries/exits are retained separately. Original rawSummary names/definitions are preserved verbatim.',
  phases:'pre-active ends at the first wrapper with a recorded active sample; startup-active ends at the first wrapper reaching frame 180. Formal-active runs until measure.ended; limit-tail samples are separately marked. Every >100ms completed entry, dispatch, wrapper, sample and submission gap is retained, including startup. All final pending requests are retained with right-censored snapshot ages.',
  ordinary:'oneForwardNoReplay requires adjacent sample frontier +1 and unchanged correction/resim counters; it never includes no-forward callbacks. The strict subset additionally requires capture +1, Present +1, and non-wait return. Both preserve every selected raw index; this is a counter-based classification, not native phase instrumentation.',
  rawAvailability:'Original report SHA and exact paths retain the full event arrays. Compact output retains full host rows and audio blocks, all threshold breaches and unmatched records, all pacing network-brake episodes, and explicit selected maxima.'},
 environmentSourceObservation:playwrightObservation(),
 headlessFailure:runEvidence(headless,headless.raw.partial),
 preparationFailure:{input:preparation.input,sourceIdentity:preparation.raw.identity,parameters:preparation.raw.parameters,error:preparation.raw.error.split('\n')[0],partialCount:preparation.raw.partial.length,headedControl:preparation.raw.headedControl??null,scope:'Preparation failed before any page/game sample; preserve as failure, not a performance score.'},
 headedRuns:headedInputs.map(run=>runEvidence(run,run.raw.results)),
 limitations:[
  'The headless failure and all completed headed conditions remain present. A successful correctness gate or absent >250ms gap in one condition is not proof that production stalls were fixed.',
  'Headed mode also adds one post-launch/pre-page CIM identity observation per browser. Its measured duration can change process warmup; this is not a pure single-factor headless causal comparison.',
  'Old headless raw lacks actual executable/argv identity. Current headed identity verifies only its own two browsers, not the historical binary selection.',
  'Same window bounds and visible/focus samples do not establish non-overlap, actual compositor presentation, or physical scanout. Playwright focus emulation can make hasFocus true independently of OS foreground. No window was moved or focused by the harness.',
  'Native release code and selected input tape identities distinguish A/B/C; unequal rollback workload, phase/network pacing and host/compiler conditions prevent treating raw elapsed differences as isolated implementation effects.',
  'Short C++ and wrapper durations do not exclude asynchronous GPU/compositor/driver work. These reports contain no scheduler/GPU trace and cannot identify the cause of withheld rAF dispatch.'
 ]};
result.comparison=result.headedRuns.map(run=>({label:path.basename(run.input.path),sameWasmAsHeadless:run.sourceIdentity.wasm===result.headlessFailure.sourceIdentity.wasm,identityVerified:run.headedControl?.identityVerified===true,
 changedSourceIdentityKeys:[...new Set([...Object.keys(run.sourceIdentity),...Object.keys(result.headlessFailure.sourceIdentity)])].filter(k=>JSON.stringify(run.sourceIdentity[k])!==JSON.stringify(result.headlessFailure.sourceIdentity[k])),
 peers:run.peers.map(p=>({side:p.side,ordinaryFormal:p.samples.oneForwardNoReplay['formal-active'].cppWorkMs,strictOrdinaryFormal:p.samples.strictOneForwardOneCaptureOneSubmission.groups['formal-active'].cppWorkMs,formalWrapperEntryGap:p.raf.wrapperEntryGapMs['formal-active'],pending:p.raf.pending.map(x=>({id:x.request.index,ageMs:x.snapshotAgeMs,phase:x.snapshotPhase})),audioBlocks:p.audio.blockCount,zeroAudioBlocks:p.audio.zeroIndices.length,pacing:p.pacing.available?{maxOutstanding:p.pacing.outstandingFrames.max,networkBrakedSamples:p.pacing.networkBrakedSamples,networkScaleMilli:p.pacing.networkScaleMilli,effectiveScaleMilli:p.pacing.effectiveScaleMilli}:null}))}));
function compactProjection(full,fullIdentity){
 const point=s=>({index:s.index,raw:s.raw,sampleAt:s.sampleAt,frameIndex:s.frame?.index,rafIndex:s.frame?.rafIndex});
 const group=g=>({count:g.indices.length,indicesSha256:hash(JSON.stringify(g.indices)),cppWorkMs:g.cppWorkMs,wrapperMs:g.wrapperMs,slowest:g.slowest.map(point)});
 const pacePoint=p=>p?{row:p.row,sampleAt:p.sampleAt}:null;
 const compactPeer=p=>{
  const audio={...p.audio};delete audio.blocks;
  audio.zeroRows=p.audio.zeroIndices.map(index=>({index,row:p.audio.blocks[index]}));
  audio.firstRow=p.audio.blocks[0]??null;audio.lastRow=p.audio.blocks.at(-1)??null;
  audio.allBlocksSha256=hash(JSON.stringify(p.audio.blocks));
  const samples={...p.samples,oneForwardNoReplay:Object.fromEntries(Object.entries(p.samples.oneForwardNoReplay).map(([k,v])=>[k,group(v)])),strictOneForwardOneCaptureOneSubmission:{...p.samples.strictOneForwardOneCaptureOneSubmission,groups:Object.fromEntries(Object.entries(p.samples.strictOneForwardOneCaptureOneSubmission.groups).map(([k,v])=>[k,group(v)]))}};
  const pacing=p.pacing.available?{...p.pacing,first:pacePoint(p.pacing.first),last:pacePoint(p.pacing.last),largestOutstandingSamples:p.pacing.largestOutstandingSamples.map(pacePoint),allNetworkBrakingEpisodes:p.pacing.allNetworkBrakingEpisodes.map(e=>({...e,first:pacePoint(e.first),last:pacePoint(e.last),firstObservedUnbraked:pacePoint(e.firstObservedUnbraked)}))}:p.pacing;
  return {...p,firstSamples:p.firstSamples.map(point),lastSamples:p.lastSamples.map(point),samples,pacing,audio};
 };
 const compactRun=r=>({...r,peers:r.peers.map(compactPeer)});
 return {...full,schema:'th09/headed-control-comparison/compact-1',fullEvidence:fullIdentity,
  compactScope:'All original host rows, all >100ms RAF/sample/submission/audio/relay breaches, every zero-audio row, every pacing-brake episode and unmatched relay record remain. Repeated sample-hook objects, ordinary-group index arrays and nonzero audio blocks are retained in the linked full evidence and SHA-addressed raw; no statistical sample or failure was removed.',
  headlessFailure:compactRun(full.headlessFailure),headedRuns:full.headedRuns.map(compactRun)};
}
for(const input of inputs)assert.equal(hash(fs.readFileSync(input.input.path)),input.input.sha256,'Raw changed during offline analysis');
fs.writeFileSync(outputPath,JSON.stringify(result)+'\n',{flag:'wx'});
const outputIdentity={path:path.resolve(outputPath),bytes:fs.statSync(outputPath).size,sha256:hash(fs.readFileSync(outputPath))};
let compactIdentity=null;
if(process.env.HEADED_COMPACT_OUTPUT){
 const target=path.resolve(process.env.HEADED_COMPACT_OUTPUT);
 assert(target!==outputIdentity.path&&inputs.every(x=>x.input.path!==target),'Compact output must be a new derived file');
 fs.writeFileSync(target,JSON.stringify(compactProjection(result,outputIdentity))+'\n',{flag:'wx'});
 compactIdentity={path:target,bytes:fs.statSync(target).size,sha256:hash(fs.readFileSync(target))};
}
console.log(JSON.stringify({output:outputIdentity,compact:compactIdentity,analyzerSha256:result.analyzer.sha256,comparison:result.comparison}));
