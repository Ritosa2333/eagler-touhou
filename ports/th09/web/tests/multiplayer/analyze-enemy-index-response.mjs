// Offline response evidence for the predeclared enemy-index followup.
// Never starts a browser, changes raw evidence, or treats missing responses as zero latency.
import fs from 'node:fs';
import path from 'node:path';
import crypto from 'node:crypto';
import {fileURLToPath} from 'node:url';
const self=fileURLToPath(import.meta.url),root=path.resolve(path.dirname(self),'../../..');
const [inputArg,outputArg]=process.argv.slice(2);
if(!inputArg||!outputArg)throw Error('usage: node analyze-enemy-index-response.mjs raw-report.json new-evidence.json');
const input=path.resolve(inputArg),output=path.resolve(outputArg);
if(fs.existsSync(output))throw Error('Output already exists; evidence is create-only.');
const sha=x=>crypto.createHash('sha256').update(x).digest('hex');
const sources=[];
function read(file){const bytes=fs.readFileSync(file);const source={path:path.resolve(file),bytes:bytes.length,sha256:sha(bytes)};sources.push(source);return {bytes,source};}
const rawInput=read(input),raw=JSON.parse(rawInput.bytes);
const planInput=read(path.join(root,'docs/multiplayer/rollback-enemy-index-validation-plan.json')),plan=JSON.parse(planInput.bytes);
const own=(x,k)=>x!=null&&Object.prototype.hasOwnProperty.call(x,k);
const finite=x=>typeof x==='number'&&Number.isFinite(x),arr=x=>Array.isArray(x)?x:[];
const same=(a,b)=>JSON.stringify(a)===JSON.stringify(b);
const checks=[];
function check(id,passed,details={}){checks.push({id,passed:passed===true,...details});return passed===true;}
function dist(values){const a=values.filter(finite).sort((a,b)=>a-b),n=a.length;return {count:n,total:a.reduce((s,x)=>s+x,0),
 p50:n?a[Math.floor(n*.5)]:null,p95:n?a[Math.floor(n*.95)]:null,p99:n?a[Math.floor(n*.99)]:null,max:n?a.at(-1):null,
 ...Object.fromEntries([25,50,100,250].map(t=>['over'+t+'ms',a.filter(x=>x>t).length]))};}
const helperInput=read(path.join(root,'web/tests/multiplayer/smoothness-window.mjs'));
if(helperInput.source.sha256!==plan.releaseSeries.frozenIdentity.smoothnessWindow)throw Error('Frozen window helper mismatch.');
const {summarizeSmoothnessWindow}=await import('data:text/javascript;base64,'+helperInput.bytes.toString('base64'));
const analyzerSource=read(self).source;
check('declared-followup-label',path.basename(input)===plan.inputResponseFollowup.label+'-report.json');
check('reported-passed',raw.passed===true);
check('no-browser-errors',Array.isArray(raw.errors)&&raw.errors.length===0);
check('candidate-wasm',raw.identity?.wasm===plan.candidate.wasmSha256,{actual:raw.identity?.wasm??null});
for(const [key,value] of Object.entries(plan.releaseSeries.frozenIdentity))check('frozen-'+key,raw.identity?.[key]===value,{actual:raw.identity?.[key]??null});
const expected={limit:1800,delay:39,jitter:5,dropEvery:0,release:true,profile:false,fallback:true,inputMode:'wall-clock',
 audioOutput:true,responsiveness:true,burstAt:20000,burstFor:500,burstDelay:120,rafDiagnostics:true,hostLoad:true,networkTrace:false,
 impairmentSite:'relay',relayDiagnostics:true,liveCost:false,rafAnomaly:true};
for(const [key,value] of Object.entries(expected))check('parameter-'+key,own(raw.parameters,key)&&same(raw.parameters[key],value),
 {actual:raw.parameters?.[key]??null,expected:value});
check('cpu-rates',same(raw.parameters?.cpuThrottle?.ratesBySide,[1,4]));
check('headed-control',raw.headedControl?.identityVerified===true&&raw.parameters?.headedLaunch?.headless===false);
const responseProbe=read(path.join(root,'web/tests/multiplayer/responsiveness-probe.mjs')).source;
check('response-probe-identity',raw.identity?.responsivenessProbe===responseProbe.sha256);
const primary=Array.isArray(raw.results)?'results':Array.isArray(raw.partial)?'partial':null,peers=primary?raw[primary]:[];
check('two-result-peers',primary==='results'&&peers.length===2);
const metricNames=['scheduleToSubmitMs','submitToFirstWireMs','scheduleToFirstWireMs','firstWireToPresentationObservedMs',
 'submitToPresentationObservedMs','scheduleToPresentationObservedMs'];
const phaseNames=['startup','body','tail','unavailable'];
function censoring(intent,intents){
 const laterPlanned=intents.some(x=>x.scheduledAtMs>intent.scheduledAtMs);
 if(intent.status==='not-submitted')return {classification:laterPlanned?'planned-state-expired-before-submission':'last-planned-state-not-submitted-at-end',
  rightCensored:!laterPlanned,laterPlanned,scope:laterPlanned?'A later planned mask exists; this synthetic intent was never submitted. Null supersededAtMs does not imply a continuously held input.':'No submission was observed before the probe ended.'};
 if(intent.status==='captured-not-presented')return {classification:'captured-response-not-observed-at-end',rightCensored:true,laterPlanned,
  scope:'The input was captured. Subsequent physical-mask replacement does not erase that capture; a later new-state response was not observed.'};
 const replaced=finite(intent.supersededAtMs);
 return {classification:replaced?'submitted-mask-replaced-without-capture':'submitted-mask-uncaptured-at-end',rightCensored:!replaced,laterPlanned,
  scope:replaced?'A historical full-mask intent was replaced before capture. This does not by itself mean every component key edge was missed.':'This submitted mask had no capture observation and no recorded replacement before the probe ended.'};
}
function cohort(intents,phaseAt){
 const changed=intents.filter(x=>x.changedSubmittedMask!==false);
 return {planned:intents.length,submitted:intents.filter(x=>finite(x.submittedAtMs)).length,
  captured:intents.filter(x=>Number.isInteger(x.captureFrame)).length,presented:intents.filter(x=>finite(x.presentationObservedAtMs)).length,
  statuses:Object.fromEntries(['not-submitted','submitted-not-captured','captured-not-presented','presented'].map(s=>[s,intents.filter(x=>x.status===s).length])),
  sameMaskResubmissions:intents.length-changed.length,conditionalDenominator:changed.length,
  observationPhases:Object.fromEntries(['submittedAtMs','firstWireAtMs','presentationObservedAtMs'].map(k=>[k,
   Object.fromEntries(phaseNames.map(p=>[p,intents.filter(x=>phaseAt(x[k])===p).length]))])),
  conditionalLatencyMs:Object.fromEntries(metricNames.map(k=>[k,{...dist(changed.map(x=>x[k])),
   missing:changed.filter(x=>!finite(x[k])).length,slowestIntentIds:changed.filter(x=>finite(x[k])).sort((a,b)=>b[k]-a[k]).slice(0,5).map(x=>x.id)}]))};
}
const sides=peers.map((peer,side)=>{
 if(!peer||typeof peer!=='object'){
  check('side-'+side+'-peer-available',false);
  return {side,available:false,rawPeer:peer??null,summary:null,summaryError:'Peer snapshot unavailable',all:null,cohorts:null,intents:null,
   unresolved:null,worstGaps:null,scope:'Missing peer evidence is unknown, not a zero-input cohort.'};
 }
 let summary=null,summaryError=null;
 try{summary=summarizeSmoothnessWindow(peer,{startFrame:180,endFrame:1800});}catch(error){summaryError=String(error);}
 check('side-'+side+'-complete-window',summary?.window.complete===true,{error:summaryError});
 check('side-'+side+'-recomputed-summary',same(summary,raw.summary?.[side]));
 const p=peer.responsiveness??{},m=peer.measure??{},rows=arr(m.samples),pacing=new Map(arr(m.networkPacing).map(x=>[x[0],x]));
 const responseAvailable=Array.isArray(p.intents)&&Array.isArray(p.captures);
 check('side-'+side+'-response-snapshot-available',responseAvailable);
 const s=summary?.window.startSampleIndex,e=summary?.window.endSampleIndexExclusive;
 const clocksValid=summary?.window.complete===true&&finite(m.started)&&finite(p.startedAt);
 const lo=clocksValid?m.started+rows[s][0]-p.startedAt:null,hi=clocksValid?m.started+rows[e][0]-p.startedAt:null;
 const phaseAt=t=>!clocksValid||!finite(t)?'unavailable':t<lo?'startup':t<hi?'body':'tail';
 check('side-'+side+'-response-valid',p.valid===true&&arr(p.errors).length===0);
 check('side-'+side+'-input-delay-zero',p.inputDelay===0);
 check('side-'+side+'-response-clock',clocksValid&&hi>lo);
 const intents=arr(p.intents).map(x=>({...x,
  scheduleToFirstWireMs:finite(x.firstWireAtMs)&&finite(x.scheduledAtMs)?x.firstWireAtMs-x.scheduledAtMs:null,
  submitToPresentationObservedMs:finite(x.submittedAtMs)&&finite(x.presentationObservedAtMs)?x.presentationObservedAtMs-x.submittedAtMs:null}));
 const captures=arr(p.captures),byFrame=new Map(captures.map(x=>[x.frame,x]));
 check('side-'+side+'-intent-id-order',intents.every((x,i)=>x.id===i));
 check('side-'+side+'-capture-sequence',captures.length>0&&captures.every((x,i)=>x.frame===p.firstFreshFrame+i&&x.contiguous===true&&x.analogNeutral===true)&&byFrame.size===captures.length);
 const observationFields=['captureGaps','inputMismatches','changedHistory','observationErrors'];
 check('side-'+side+'-input-observation-errors',observationFields.every(k=>p.observations?.[k]===0)&&p.observations?.overflow===false,
  {observations:p.observations??null});
 check('side-'+side+'-captured-intents-match-wire',intents.every(x=>x.captureFrame===null||
  (byFrame.get(x.captureFrame)?.intentId===x.id&&byFrame.get(x.captureFrame)?.buttons===x.mask&&byFrame.get(x.captureFrame)?.firstWireAtMs===x.firstWireAtMs)));
 const all=cohort(intents,phaseAt),cohorts=clocksValid?Object.fromEntries(phaseNames.map(name=>[name,cohort(intents.filter(x=>phaseAt(x.scheduledAtMs)===name),phaseAt)])):
  {startup:null,body:null,tail:null,unavailable:all};
 check('side-'+side+'-raw-counts',Object.entries({planned:all.planned,submitted:all.submitted,captured:all.captured,presented:all.presented,
  sameMaskResubmissions:all.sameMaskResubmissions}).every(([k,v])=>p.counts?.[k]===v),{recorded:p.counts??null});
 const context=gap=>{
  if(!gap)return null;const a=gap.fromSample,b=gap.toSample,interval=rows.slice(a+1,b+1);
  return {...gap,callbacks:interval.length,waitCallbacks:interval.filter(x=>x[1]===2).length,
   nativeElapsedMs:dist(interval.map(x=>x[2])),corrections:rows[b][5]-rows[a][5],resimulated:rows[b][6]-rows[a][6],captures:rows[b][7]-rows[a][7],
   boundary:rows[a],callbackRows:interval.map((row,i)=>({sampleIndex:a+i+1,sample:row,pacing:pacing.get(a+i+1)??null})),
   scope:'Callback costs are elapsed native measurements. Their sum is not subtracted from a RAF-timestamp interval to infer CPU idle or scheduler delay.'};
 };
 const anomaly=peer.rafAnomaly;
 return {side,available:true,responseAvailable,cpuRate:raw.parameters?.cpuThrottle?.ratesBySide?.[side],inputDelay:p.inputDelay??null,
  clock:{measureStartedAtMs:m.started??null,responseStartedAtMs:p.startedAt??null,formalResponseRangeMs:[lo,hi],
   rule:'One half-open [first native PredictionFrontier >=180, first >=1800) interval in the response page clock. Cohort membership follows scheduled intent time, never successful capture.'},
  summary,summaryError,all:responseAvailable?all:null,cohorts:responseAvailable?cohorts:null,
  intentIds:responseAvailable?Object.fromEntries(phaseNames.map(name=>[name,clocksValid||name==='unavailable'?intents.filter(x=>phaseAt(x.scheduledAtMs)===name).map(x=>x.id):null])):null,
  intents:responseAvailable?intents:null,unresolved:responseAvailable?intents.filter(x=>x.status!=='presented').map(intent=>({intent,phase:phaseAt(intent.scheduledAtMs),
   submittedHoldBeforeSupersededMs:finite(intent.supersededAtMs)&&finite(intent.submittedAtMs)?intent.supersededAtMs-intent.submittedAtMs:null,
   censoring:censoring(intent,intents),
   neighboringIntents:intents.filter(x=>Math.abs(x.id-intent.id)<=1),
   neighboringCaptures:captures.filter(x=>Math.abs(x.firstWireAtMs-(finite(intent.submittedAtMs)?intent.submittedAtMs:intent.scheduledAtMs))<50)})):null,
  captureEvidence:responseAvailable?{count:captures.length,first:captures[0]??null,last:captures.at(-1)??null,sha256:sha(JSON.stringify(captures))}:null,
  worstGaps:{bodyWithEnd:context(summary?.window.formalWithEndBoundary.newStates.maximumRafGap),
   startup:context(summary?.window.segments.startup.newStates.maximumRafGap),tail:context(summary?.window.segments.tail.newStates.maximumRafGap),
   allObserved:context(summary?.window.segments.allObserved.newStates.maximumRafGap)},
  terminalWaiting:summary?.window.formalWithEndBoundary.observedWithoutNewStateToEnd??null,
  anomaly:{enabled:raw.parameters?.rafAnomaly??null,valid:anomaly?.valid??null,coverageComplete:anomaly?.coverageComplete??null,
   counts:anomaly?.counts??null,episode:anomaly?.episode??null,finalObservation:anomaly?.finalObservation??null},
  audioOutput:peer.audioOutput??null,confirmedHashes:peer.confirmedHashes??null,
  confirmedTailCount:Array.isArray(peer.confirmedHashes)?Math.max(0,peer.confirmedHashes.length-15):null};
});
check('fifteen-confirmed-checks',raw.confirmedChecks===15);
const confirmedPrefixes=peers.map(peer=>arr(peer?.confirmedHashes).slice(0,15));
check('confirmed-prefixes-valid',confirmedPrefixes.length===2&&confirmedPrefixes.every(prefix=>prefix.length===15&&prefix.every((row,i)=>
 Array.isArray(row)&&row.length===2&&row[0]===(i+1)*120&&Number.isInteger(row[1])&&row[1]>=0&&row[1]<=0xffffffff)));
check('confirmed-prefixes-match',confirmedPrefixes.length===2&&same(confirmedPrefixes[0],confirmedPrefixes[1]));
if(raw.finalComparable===true){
 const eligible=peers.length===2&&peers.every(x=>Array.isArray(x?.stats)&&x.stats.length===8&&x.stats[1]>=x.stats[0]&&x.stats[7]>=x.stats[0])&&peers[0].stats[0]===peers[1].stats[0];
 check('declared-live-endpoint-confirmed',eligible);
 check('comparable-live-hashes-match',eligible&&Number.isInteger(peers[0].hash)&&peers[0].hash===peers[1].hash);
}
const relay=raw.relayTransport??{};
check('relay-valid',relay.valid===true);
function indexed(rows,names){return arr(rows).map(row=>Object.fromEntries(arr(names).map((key,i)=>[key,row[i]])));}
const packets=new Map(indexed(relay.packets,relay.columns?.packet).map(x=>[x.id,x]));
const events=indexed(relay.events,relay.columns?.event),phases=arr(relay.columns?.phases);
const sends=new Map(events.filter(x=>x.phase===phases.indexOf('send')).map(x=>[x.scheduledId,x]));
const scheduled=events.filter(x=>x.phase===phases.indexOf('scheduled')).map(x=>{
 const p=packets.get(x.packetId),send=sends.get(x.scheduledId);
 return {sender:p?.sender??null,destination:p?.destination??null,senderFrame:p?.senderFrame??null,burst:!!x.burst,
  plannedDelayMs:x.plannedDelayMs,sendApiDelayMs:send?send.enteredAt-x.enteredAt:null,
  sendApiTimerLatenessMs:send?send.enteredAt-x.dueAt:null};
});
const relayGroups=[0,1].map(sender=>({sender,...Object.fromEntries([['all',scheduled.filter(x=>x.sender===sender)],
 ['burst',scheduled.filter(x=>x.sender===sender&&x.burst)]].map(([name,rows])=>[name,{scheduled:rows.length,
 sent:rows.filter(x=>finite(x.sendApiDelayMs)).length,plannedDelayMs:dist(rows.map(x=>x.plannedDelayMs)),
 sendApiDelayMs:dist(rows.map(x=>x.sendApiDelayMs)),sendApiTimerLatenessMs:dist(rows.map(x=>x.sendApiTimerLatenessMs))}]))}));
check('burst-exercised-both-directions',relayGroups.every(x=>x.burst.scheduled>0));
const host=raw.hostLoad??{},cpu=arr(host.cpuSamples).filter(x=>finite(x.busyFraction));
for(const source of sources)check('unchanged-'+path.relative(root,source.path),sha(fs.readFileSync(source.path))===source.sha256);
const passed=checks.every(x=>x.passed);
const evidence={schema:'th09-enemy-index-response-evidence-v1',generatedAtUtc:new Date().toISOString(),source:rawInput.source,analyzer:analyzerSource,
 plan:planInput.source,helper:helperInput.source,responseProbe,reportedPassed:raw.passed??null,checksPassed:passed,checks,
 primarySnapshot:primary,otherSnapshot:primary==='results'&&raw.partial?{present:true,sha256:sha(JSON.stringify(raw.partial)),notSubstituted:true}:null,
 parameters:raw.parameters??null,identity:raw.identity??null,legacySummary:{present:own(raw,'legacySummary'),sha256:own(raw,'legacySummary')?sha(JSON.stringify(raw.legacySummary)):null,used:false},
 final:{sameFrame:raw.sameFrame??null,comparable:raw.finalComparable??null,hashes:peers.map(x=>x?.hash??null),confirmedChecks:raw.confirmedChecks??null},
 sides,relay:{valid:relay.valid??null,coverage:relay.coverage??null,clock:relay.clock??null,injection:relay.injection??null,
 counts:relay.counts??null,overflow:relay.overflow??null,groups:relayGroups,
 scope:'Whole relay observer interval; sendApi is the relay WebSocket API boundary, not peer receive or native input acceptance. Burst sender frames do not synchronize page and relay clocks.'},
 host:{valid:host.valid??null,complete:host.complete??null,sampleCount:cpu.length,busyFraction:dist(cpu.map(x=>x.busyFraction)),
 unweightedBusyMean:cpu.length?cpu.reduce((s,x)=>s+x.busyFraction,0)/cpu.length:null,
 scope:'Aggregate sampled host activity. No per-native-call causal attribution; first-seen process lifetime CPU is not summed.'},
 limitations:['Passing state and measurement checks is not game smoothness acceptance.',
 'D=0 is measured by the independent response probe; slowdown can still increase wall-clock response time.',
 'Conditional latency excludes missing observations and unchanged-mask submissions; all planned intents, statuses and missing denominators remain explicit.',
 'First wire follows native capture/serialization and precedes the transport send call. It is a capture-observation upper bound, not exact sampling or remote receipt.',
 'New-state response is a completed frontier and renderer submission observed after play.frame. It is not hardware key latency, pixel change or physical display scanout.',
 'A single candidate wall-clock burst run is not a matched input workload against the prior baseline burst, and cannot establish a speedup.',
 'No eligible anomaly episode in a finite run does not resolve prior seconds-long RAF scheduling waits.',
 'Raw reports and all their samples remain unmodified; startup, terminal boundary, tail, failed intents and conditional denominators remain visible.']};
const bytes=Buffer.from(JSON.stringify(evidence,null,2)+'\n');
if(bytes.length>1024*1024)throw Error('Response evidence exceeds 1 MiB; refusing silent truncation.');
fs.writeFileSync(output,bytes,{flag:'wx'});
console.log(JSON.stringify({output,bytes:bytes.length,sha256:sha(bytes),checksPassed:passed,
 failures:checks.filter(x=>!x.passed),sides:sides.map(x=>({side:x.side,hz:x.summary?.logicHz??null,
 newStates:x.summary?.window.formalWithEndBoundary.newStates.rafTimestampMs??null,
 observedNewStates:x.summary?.window.formalWithEndBoundary.newStates.sampledAtMs??null,
 native:x.summary?.callbackWorkMs??null,corrections:x.summary?.corrections??null,resimulated:x.summary?.resimulated??null,
 cohortCounts:x.cohorts?Object.fromEntries(Object.entries(x.cohorts).map(([k,c])=>[k,c?{planned:c.planned,captured:c.captured,presented:c.presented}:null])):null,
 response:x.cohorts?.body?.conditionalLatencyMs??null,unresolved:x.unresolved?.map(y=>({id:y.intent.id,phase:y.phase,status:y.intent.status,holdMs:y.submittedHoldBeforeSupersededMs}))??null,
 worstGap:{rafMs:x.worstGaps?.bodyWithEnd?.rafTimestampMs??null,waits:x.worstGaps?.bodyWithEnd?.waitCallbacks??null,
 nativeMs:x.worstGaps?.bodyWithEnd?.nativeElapsedMs.total??null,corrections:x.worstGaps?.bodyWithEnd?.corrections??null,resimulated:x.worstGaps?.bodyWithEnd?.resimulated??null}})),relay:relayGroups}));
process.exitCode=passed?0:1;

