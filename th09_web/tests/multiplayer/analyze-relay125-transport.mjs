import fs from 'node:fs';
import crypto from 'node:crypto';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
const [input,output]=process.argv.slice(2);
if(!input||!output)throw Error('Usage: node analyze-relay125-transport.mjs raw.json new-evidence.json');
const bytes=fs.readFileSync(input),raw=JSON.parse(bytes);
const sha=b=>crypto.createHash('sha256').update(b).digest('hex');
const assert=(v,msg)=>{if(!v)throw Error(msg);};
assert(sha(bytes)==='d03833c48742160afa7956a70b4aa82f135069005e3b4b38422a6b3c08bca770','Unexpected raw identity');
assert(raw.identity.wasm==='47367732b9bc4972d8adf2ffc49ace32cf9eca4a3868ff73c9c6494a0de4df97','Unexpected B scalar build');
assert(raw.results.length===2&&raw.parameters.limit===1800&&raw.parameters.networkTrace===false,'Unexpected report scope');
const dist=values=>{assert(values.every(Number.isFinite),'Non-finite distribution');const a=[...values].sort((x,y)=>x-y);return{count:a.length,total:a.reduce((s,n)=>s+n,0),min:a[0]??null,p50:a[Math.floor(a.length*.5)]??null,p95:a[Math.floor(a.length*.95)]??null,p99:a[Math.floor(a.length*.99)]??null,max:a.at(-1)??null,over25ms:a.filter(x=>x>25).length,over50ms:a.filter(x=>x>50).length,over100ms:a.filter(x=>x>100).length,over250ms:a.filter(x=>x>250).length};};
const numeric=(values,unit)=>{const result=dist(values);for(const k of Object.keys(result))if(k.startsWith('over'))delete result[k];return {...result,unit};};
const hist=a=>{const h={};for(const x of a)h[x]=(h[x]||0)+1;return h;};
const maxBy=(a,fn)=>a.length?a.reduce((best,x)=>fn(x)>fn(best)?x:best):null;
const relay=raw.relayTransport,rc=relay.columns;
const pci=Object.fromEntries(rc.packet.map((s,i)=>[s,i])),eci=Object.fromEntries(rc.event.map((s,i)=>[s,i]));
const ei=(e,k)=>e[eci[k]],pi=(p,k)=>p[pci[k]];
const packets=new Map(relay.packets.map(p=>[pi(p,'id'),p])),events=new Map(relay.events.map(e=>[ei(e,'id'),e]));
const phase=n=>relay.events.filter(e=>ei(e,'phase')===rc.phases.indexOf(n));
const recv=phase('receive'),scheduled=phase('scheduled'),sent=phase('send'),cancelled=phase('cancelled');
const schedules=new Map(scheduled.map(e=>[ei(e,'scheduledId'),e]));
const sends=new Map();for(const e of sent){const key=ei(e,'scheduledId');assert(!sends.has(key),'Duplicate terminal send for scheduled job');sends.set(key,e);}
const cancellations=new Map(cancelled.map(e=>[ei(e,'scheduledId'),e]));
const pending=new Map(relay.pending.map(p=>[p.scheduledId,p]));
const lcgSeeds=new Map();let lcgMismatches=0;
for(const e of scheduled){
 const packet=packets.get(ei(e,'packetId')),re=events.get(ei(e,'receiveEventId')),sender=pi(packet,'sender');
 assert(re&&ei(re,'phase')===rc.phases.indexOf('receive'),'Missing receive event');
 assert(ei(re,'packetId')===ei(e,'packetId'),'Scheduled packet changed');
 let seed=(Math.imul(lcgSeeds.get(sender)??(0x915ab+sender),1664525)+1013904223)>>>0;lcgSeeds.set(sender,seed);
 const expected=Math.max(0,125+(seed/0x100000000*2-1)*5);
 if(Math.abs(ei(e,'plannedDelayMs')-expected)>1e-8)++lcgMismatches;
 const id=ei(e,'scheduledId');assert(Number(sends.has(id))+Number(cancellations.has(id))+Number(pending.has(id))===1,'Missing or duplicate job outcome');
}
assert(lcgMismatches===0&&scheduled.length===recv.length,'Relay injection mismatch');
const relayOrigin=relay.clock.started.timeOrigin;
function relayRange(beginEpochMs,endEpochMs){
 const lo=beginEpochMs-relayOrigin,hi=endEpochMs-relayOrigin,inside=e=>ei(e,'enteredAt')>=lo&&ei(e,'enteredAt')<hi;
 return{relayAt:[lo,hi],counts:{receive:recv.filter(inside).length,scheduled:scheduled.filter(inside).length,sent:sent.filter(inside).length,cancelled:cancelled.filter(inside).length,timing:relay.timing.filter(x=>x[0]>=lo&&x[0]<hi).length},
  scheduledJobsBySender:[0,1].map(sender=>{const jobs=scheduled.filter(e=>inside(e)&&pi(packets.get(ei(e,'packetId')),'sender')===sender),done=jobs.filter(e=>sends.has(ei(e,'scheduledId')));
   return{sender,jobs:jobs.length,completedBySnapshot:done.length,pending:jobs.filter(e=>pending.has(ei(e,'scheduledId'))).length,plannedMs:dist(jobs.map(e=>ei(e,'plannedDelayMs'))),scheduleToSendMs:dist(done.map(e=>ei(sends.get(ei(e,'scheduledId')),'enteredAt')-ei(e,'enteredAt'))),dueToSendMs:dist(done.map(e=>ei(sends.get(ei(e,'scheduledId')),'enteredAt')-ei(e,'dueAt')))};})};
}
const relayDirections=[0,1].map(sender=>{
 const jobs=scheduled.filter(e=>pi(packets.get(ei(e,'packetId')),'sender')===sender),done=jobs.filter(e=>sends.has(ei(e,'scheduledId')));
 const details=e=>{const send=sends.get(ei(e,'scheduledId')),re=events.get(ei(e,'receiveEventId'));return{packet:packets.get(ei(e,'packetId')),receive:re,scheduled:e,send,scheduleToSendMs:ei(send,'enteredAt')-ei(e,'enteredAt'),dueToSendMs:ei(send,'enteredAt')-ei(e,'dueAt'),receiveEntryToSendEntryMs:ei(send,'enteredAt')-ei(re,'enteredAt')};};
 const longest=maxBy(done,e=>ei(sends.get(ei(e,'scheduledId')),'enteredAt')-ei(e,'enteredAt'));
 return{sender,scheduled:jobs.length,sent:done.length,plannedMs:dist(jobs.map(e=>ei(e,'plannedDelayMs'))),scheduleToSendMs:dist(done.map(e=>ei(sends.get(ei(e,'scheduledId')),'enteredAt')-ei(e,'enteredAt'))),dueToSendMs:dist(done.map(e=>ei(sends.get(ei(e,'scheduledId')),'enteredAt')-ei(e,'dueAt'))),longest:longest?details(longest):null};
});
const host=raw.hostLoad;
const compilerName=n=>/^(wasm-opt|wasm-ld|clang(\+\+)?|clang-cl|clang-[0-9]+|g\+\+|cc1plus|emcc|em\+\+|ninja|cmake|cl|link)$/i.test(n);
function hostRange(begin,end){
 const cpu=host.cpuSamples.filter(s=>Number.isFinite(s.busyFraction)&&s.utcMs>=begin&&s.utcMs<end);
 const processPairs=[];for(let i=1;i<host.processSamples.length;++i){const before=host.processSamples[i-1],after=host.processSamples[i];
  if(before.failed||after.failed||before.sourceUtcMs<begin||after.sourceUtcMs>=end)continue;
  const old=new Map(before.rows.map(r=>[r[0]+'|'+r[1],r]));for(const row of after.rows){const prior=old.get(row[0]+'|'+row[1]);if(!prior||!Number.isFinite(row[2])||!Number.isFinite(prior[2])||row[2]<prior[2])continue;
   processPairs.push({pid:row[0],name:row[1],fromUtcMs:before.sourceUtcMs,toUtcMs:after.sourceUtcMs,deltaCpuSeconds:row[2]-prior[2]});}}
 const byName={};for(const p of processPairs)byName[p.name]=(byName[p.name]||0)+p.deltaCpuSeconds;
 const observations=host.processSamples.filter(p=>p.sourceUtcMs>=begin&&p.sourceUtcMs<end),names=[...new Set(observations.flatMap(p=>p.rows.map(r=>r[1])))].sort();
 const total=cpu.reduce((s,x)=>s+x.deltaTotalMs,0),idle=cpu.reduce((s,x)=>s+x.deltaIdleMs,0);
 return{windowEpochMs:[begin,end],cpuSampleEndTimesSelected:true,busyPercent:numeric(cpu.map(x=>100*x.busyFraction),'percent'),weightedBusyPercent:total?100*(total-idle)/total:null,processSnapshotCount:observations.length,observedNames:names,observedCompilerNames:names.filter(compilerName),continuousSamePidNameCpuDeltaSecondsByName:byName,compilerCpuPairs:processPairs.filter(p=>compilerName(p.name))};
}
const peerOut=raw.results.map((r,side)=>{
 const s=r.measure.samples,p=r.measure.networkPacing;
 assert(p.length===s.length&&p.every((x,i)=>x[0]===i&&x[1]===1),'Missing pacing sample');
 assert(p.every((x,i)=>x[2]===Math.max(x[5]-x[6],0)&&x[5]>=s[i][3]),'Pacing field invariant');
 assert(p.every((x,i)=>!i||x[5]>=p[i-1][5]&&x[6]>=p[i-1][6]),'Frontier epoch/reversal');
 const originRows=raw.headedControl.windows.filter(w=>w.side===side).map(w=>w.page.timeOrigin);
 assert(originRows.length&&originRows.every(x=>x===originRows[0]),'Missing or inconsistent page clock');
 const origin=originRows[0],frames=new Map(),requests=new Map(r.rafDiagnostics.requests.map(q=>[q.index,q]));
 for(const f of r.rafDiagnostics.frames)if(f.sampleAfter===f.sampleBefore+1){assert(!frames.has(f.sampleBefore),'Duplicate frame for sample');frames.set(f.sampleBefore,f);}
 assert(s.every((_,i)=>frames.has(i)),'Unmatched sample/frame');
 const qAt=i=>requests.get(frames.get(i).rafIndex),epoch=i=>origin+r.measure.started+s[i][0];
 const start=p.findIndex(x=>x[5]>=180),end=p.findIndex(x=>x[5]>=1800);
 assert(start>=0&&end>start&&p[start][5]===180&&p[end][5]===1800,'Unexpected monotonic markers');
 const range=(a,b)=>Array.from({length:b-a},(_,n)=>a+n),formal=range(start,end),closure=range(start,end+1);
 const legacy=s.map((_,i)=>i).filter(i=>s[i][3]>=180&&s[i][3]<1800);
 const delta=(a,b)=>({frontier:p[b][5]-p[a][5],next:s[b][3]-s[a][3],corrections:s[b][5]-s[a][5],resimulated:s[b][6]-s[a][6],captures:s[b][7]-s[a][7],repairs:s[b][8]-s[a][8],present:s[b][11]-s[a][11]});
 const context=(a,b)=>({from:a,to:b,rows:range(a,b+1).map(i=>({sampleIndex:i,sample:s[i],pacing:p[i],frameHook:frames.get(i),request:qAt(i)}))});
 const gapPairs=indices=>indices.slice(1).map((b,n)=>({a:indices[n],b,rafTimestampMs:s[b][13]-s[indices[n]][13],sampleCompletionMs:s[b][0]-s[indices[n]][0],rafEntryMs:qAt(b).enteredAt-qAt(indices[n]).enteredAt}));
 const newPairs=indices=>{let prev=null;const out=[];for(const i of indices)if(prev==null||s[i][11]!==s[prev][11]&&s[i][3]>s[prev][3]){if(prev!=null)out.push({a:prev,b:i,rafTimestampMs:s[i][13]-s[prev][13],sampleCompletionMs:s[i][0]-s[prev][0],rafEntryMs:qAt(i).enteredAt-qAt(prev).enteredAt});prev=i;}return out;};
 const legacyGaps=gapPairs(legacy),legacyMax=maxBy(legacyGaps,x=>x.rafTimestampMs);
 const actualGaps=gapPairs(closure),newGaps=newPairs(closure),maxNew=maxBy(newGaps,x=>x.rafTimestampMs);
 const rq=indices=>indices.map(i=>({sampleIndex:i,request:qAt(i),waitMs:qAt(i).enteredAt-qAt(i).requestedAt}));
 const waits=rq(formal),maxWait=maxBy(waits,x=>x.waitMs);
 const classes={};const incrementClasses={};
 for(const i of formal){const R=p[i][5]-s[i][3],resim=i?s[i][6]-s[i-1][6]:0,corr=i?s[i][5]-s[i-1][5]:0;
  const key=R>0?'partialReplayAtEnd':s[i][1]===2&&resim>0?'caughtUpWithoutFresh':resim>0?'replayCompletedInCallback':corr>0?'correctionWithoutResimObserved':s[i][3]>(i?s[i-1][3]:-1)?'forwardNoReplay':'noNewCursor';
  (classes[key]??=[]).push(i);
  const incr=resim>0?'resimDeltaPositive':'resimDeltaZero';(incrementClasses[incr]??=[]).push(i);
 }
 const episodes=[];let open=null;
 for(const i of closure){const R=p[i][5]-s[i][3];if(R>0&&!open)open={start:i,maxR:R};if(open)open.maxR=Math.max(open.maxR,R);
  if(open&&(R===0||i===end)){episodes.push({...open,end:i,closed:R===0,durationMs:s[i][0]-s[open.start][0],fromPriorDelta:delta(Math.max(0,open.start-1),i)});open=null;}}
 const classStats=Object.fromEntries(Object.entries(classes).map(([key,indices])=>[key,{count:indices.length,callbackWorkMs:dist(indices.map(i=>s[i][2])),B:hist(indices.map(i=>p[i][2])),R:hist(indices.map(i=>p[i][5]-s[i][3])),sampleIndices:indices.filter(i=>key==='partialReplayAtEnd'||key==='caughtUpWithoutFresh')}]));
 const increasing=closure.slice(1).filter(i=>p[i][3]>p[i-1][3]),decreasing=closure.slice(1).filter(i=>p[i][3]<p[i-1][3]);
 const longerRequests=r.rafDiagnostics.requests.filter(q=>q.enteredAt!=null&&q.enteredAt-q.requestedAt>25);
 const allSampleWaits=rq(range(0,s.length)),allMaxNew=maxBy(newPairs(range(0,s.length)),x=>x.rafTimestampMs),allRawRafMax=maxBy(gapPairs(range(0,s.length)),x=>x.rafTimestampMs);
 const targetMaxMs=maxNew.rafTimestampMs,maxWindow=[origin+qAt(maxNew.a).enteredAt,origin+qAt(maxNew.b).enteredAt];
 const endRows=range(Math.max(0,end-1),s.length).map(i=>({sampleIndex:i,sample:s[i],pacing:p[i]}));
 return{side,clock:{pageTimeOrigin:origin,source:'headedControl.windows[].page.timeOrigin; setup and completed observations agree',measureStarted:r.measure.started,measureEnded:r.measure.ended},
  route:r.route,statsAtSnapshot:r.stats,predictedHashAtSnapshot:r.hash,confirmedHashes:r.confirmedHashes,rawSummary:raw.summary[side],
  invariantChecks:{pacingOnePerSample:true,BEqualsSaturatedFMinusA:true,FNotBelowNext:true,FMonotonic:true,AMonotonic:true,rafValid:r.rafDiagnostics.valid,rafOverflow:r.rafDiagnostics.overflow,hookErrors:r.rafDiagnostics.hookErrors},
  continuous:{definition:'Half-open callback cohort [first F>=180, first F>=1800); retain end marker to close elapsed, counter deltas and final new-state gap. F is PredictionFrontier, not rewound next.',startIndex:start,endIndexExclusive:end,callbackCount:formal.length,startMarker:{sample:s[start],pacing:p[start]},endMarker:{sample:s[end],pacing:p[end]},windowEpochMs:[epoch(start),epoch(end)],elapsedMs:s[end][0]-s[start][0],logicHz:1620*1000/(s[end][0]-s[start][0]),markerDeltas:delta(start,end),counterEndMarkerIncluded:true,
   callbackWorkMs:dist(formal.map(i=>s[i][2])),requestToEntryMs:dist(waits.map(x=>x.waitMs)),maxRequestToEntry:maxWait,newStateRafGapMs:dist(newGaps.map(x=>x.rafTimestampMs)),newStateGapEndMarkerIncluded:true,halfOpenBodyNewStateRafGapMs:dist(newPairs(formal).map(x=>x.rafTimestampMs)),terminalCrossingNewStateGap:newGaps.at(-1),newStateActualRafEntryGapMs:dist(newGaps.map(x=>x.rafEntryMs)),adjacentActualSamplesRafTimestampGapMs:dist(actualGaps.map(x=>x.rafTimestampMs)),maxNewState:{...maxNew,context:context(maxNew.a,maxNew.b),relayDuringActualRafEntryWindow:relayRange(...maxWindow),hostDuringActualRafEntryWindow:hostRange(...maxWindow)},
   pacing:{B:hist(formal.map(i=>p[i][2])),R:hist(formal.map(i=>p[i][5]-s[i][3])),networkMilli:numeric(formal.map(i=>p[i][3]),'milli-factor'),effectiveMilli:numeric(formal.map(i=>p[i][4]),'milli-factor'),networkAboveUnity:formal.filter(i=>p[i][3]>1000).length,actualAtOrAheadOfPrediction:formal.filter(i=>p[i][6]>=p[i][5]).length,risingObservedTransitions:increasing.length,fallingObservedTransitions:decreasing.length,samplingCaveat:'Callback-end sample is not entry scale; timer pumps and fresh progress can occur between pacing observations.'},
   classes:classStats,splitRecovery:{definition:'Episodes of R>0 callback-end observations; duration starts at the first observed partial endpoint, not the start of Restore/CPU work, and ends at the first observed R=0 or censored endpoint.',episodeCount:episodes.length,maxDurationMs:Math.max(0,...episodes.map(x=>x.durationMs)),maxRemainingFrames:Math.max(0,...episodes.map(x=>x.maxR)),episodes},host:hostRange(epoch(start),epoch(end)),relay:relayRange(epoch(start),epoch(end))},
  legacyCohortBoundary:{definition:'Raw harness filters every sample by rewound next in [180,1800), then differences consecutive retained rows.',indicesOnlyInLegacy:legacy.filter(i=>i<start||i>=end),indicesOnlyInContinuous:formal.filter(i=>!legacy.includes(i)),maxFilteredRafGap:{...legacyMax,actualRowsSkipped:legacyMax.b-legacyMax.a-1,context:context(legacyMax.a,legacyMax.b)},rawSummaryPreserved:true},
  startupAndTail:{costPartition:{definition:'Every sampled callback cost belongs exactly once to prefix [0,start), body [start,end), or tail [end,sampleCount); endpoint callback belongs to tail body cost while its counters close the boundary progress metric.',prefix:{range:[0,start],work:dist(s.slice(0,start).map(x=>x[2]))},body:{range:[start,end],work:dist(s.slice(start,end).map(x=>x[2]))},tail:{range:[end,s.length],work:dist(s.slice(end).map(x=>x[2]))},all:dist(s.map(x=>x[2]))},allActiveSampleMaxRafTimestampGap:{...allRawRafMax,context:context(allRawRafMax.a,allRawRafMax.b)},allActiveMaxNewStateGap:{...allMaxNew,context:context(allMaxNew.a,allMaxNew.b)},allSampleRequestToEntryMs:dist(allSampleWaits.map(x=>x.waitMs)),requestsOver25Ms:longerRequests,pendingRequests:r.rafDiagnostics.pendingIndices.map(id=>requests.get(id)),tailAfterFirst1800:endRows,scope:'Startup and tail retained; no excluded sample is relabelled a clean performance result.'},
  lifecycle:r.lifecycle,hostScopeCaveat:'Host valid means complete observations, not isolated CPU/GPU or no brief unobserved task.'
 };
});
assert(raw.results[0].confirmedHashes.length===15&&JSON.stringify(raw.results[0].confirmedHashes)===JSON.stringify(raw.results[1].confirmedHashes),'Confirmed hash mismatch');
const sourceContractPath='docs/multiplayer/rollback-chain-network-pacing-contract-evidence.json';
const sourceContractBytes=fs.readFileSync(sourceContractPath);
const outputObject={schemaVersion:1,kind:'th09-relay125-source-grounded-transport-analysis',source:{path:path.resolve(input),bytes:bytes.length,sha256:sha(bytes)},analyzer:{path:path.resolve(fileURLToPath(import.meta.url)),sha256:sha(fs.readFileSync(fileURLToPath(import.meta.url)))},sourceContract:{path:sourceContractPath,sha256:sha(sourceContractBytes)},identity:raw.identity,parameters:raw.parameters,passed:raw.passed,errors:raw.errors,rawSummary:raw.summary,confirmed:{checks:15,fullPairsEqual:true,pairs:raw.results[0].confirmedHashes,finalComparable:raw.finalComparable,finalPredictedHashes:raw.results.map(r=>r.hash),finalStats:raw.results.map(r=>r.stats),note:'Final speculative hashes are retained, not compared; same next/confirmed counters do not imply same speculative input history.'},peers:peerOut,
 relay:{clock:relay.clock,columns:relay.columns,injection:relay.injection,sourceIdentity:relay.sourceIdentity,valid:relay.valid,coverage:relay.coverage,counts:relay.counts,overflow:relay.overflow,errors:relay.errors,connections:relay.connections,linkChecks:{received:recv.length,scheduled:scheduled.length,sent:sent.length,cancelled:cancelled.length,pending:pending.size,allJobsHaveOneTerminalOrPending:true,lcgMismatches},directions:relayDirections,heartbeatGapMs:dist(relay.timing.map(x=>x[3])),heartbeatLateMs:dist(relay.timing.map(x=>x[2])),bufferedBeforeBytes:numeric(relay.events.map(e=>ei(e,'bufferedBefore')),'bytes'),bufferedAfterBytes:numeric(relay.events.map(e=>ei(e,'bufferedAfter')),'bytes'),pendingMessages:numeric(relay.events.map(e=>ei(e,'pendingMessages')),'messages'),pendingBytes:numeric(relay.events.map(e=>ei(e,'pendingBytes')),'bytes'),unresolvedTail:relay.pending.map(p=>({pending:p,scheduled:schedules.get(p.scheduledId),receive:events.get(ei(schedules.get(p.scheduledId),'receiveEventId')),packet:packets.get(ei(schedules.get(p.scheduledId),'packetId'))})),scope:'Same-host separate Node relay process; scheduled/send are process API boundaries. NETWORK_TRACE=0 means browser message, poll-copy and native acceptance times are unavailable. No inferred end-to-end/remote-WAN latency.'},
 host:{raw:host,wholeWindow:hostRange(host.startedAtUtcMs,host.endedAtUtcMs),limits:'CPU busy values are host aggregate; 5s candidate-process sampling misses short-lived work. Deltas require same PID/name in consecutive observations and exclude first-observed lifetime CPU. No performance samples removed, reweighted or baseline-divided.'},
 conclusions:[
 'The original P2 83.735ms filtered RAF gap joins samples 5728 and 5739 across ten excluded tail samples after first F=1800. It is not one request waiting 83.735ms. Preserve original summary and show continuous monotonic-frontier window separately.',
 'Continuous marker-to-marker logical throughput is about 40.61Hz on both peers. Actual-input-gap scale is active through nearly all that window; P2 also splits replay. Do not divide elapsed or infer exact pacing duty from callback-end scale.',
 'P2 partial replay and caught-up-without-fresh returns are explicit recovery work; the raw ok=2 callbacks examined have positive replay increments, not a demonstrated hard-window no-progress wait.',
 'Neither supplied 125ms nor median scale establishes actual rollback depth. The observed F/A deficit and remaining replay work are reported separately; correction counts include only each explicitly defined window.',
 'No claim of physical scanout, audible continuity, public network robustness, absence of host competition, or cause of prior seconds-long RAF stalls follows from this run. Input responsiveness/audio are analyzed separately.'
 ]};
fs.writeFileSync(output,JSON.stringify(outputObject,null,2)+'\n',{flag:'wx'});
const written=fs.readFileSync(output);console.log(JSON.stringify({path:path.resolve(output),bytes:written.length,sha256:sha(written),peers:peerOut.map(p=>({side:p.side,continuous:{start:p.continuous.startIndex,end:p.continuous.endIndexExclusive,logicHz:p.continuous.logicHz,deltas:p.continuous.markerDeltas,callbackMax:p.continuous.callbackWorkMs.max,requestMax:p.continuous.requestToEntryMs.max,newStateMax:p.continuous.newStateRafGapMs.max,pacing:p.continuous.pacing,classes:Object.fromEntries(Object.entries(p.continuous.classes).map(([k,v])=>[k,v.count]))},legacyOnly:p.legacyCohortBoundary.indicesOnlyInLegacy})),relay:outputObject.relay.linkChecks,host:outputObject.host.wholeWindow}));
