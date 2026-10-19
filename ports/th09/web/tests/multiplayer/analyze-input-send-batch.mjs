// Offline comparison only: no browser, native pump, compilation or traffic.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';

const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const finite=x=>typeof x==='number'&&Number.isFinite(x);
const dist=values=>{
 const a=[...values].sort((x,y)=>x-y),at=q=>a[Math.min(a.length-1,Math.floor(a.length*q))]??null;
 return {count:a.length,min:a[0]??null,p50:at(.5),p95:at(.95),p99:at(.99),max:a.at(-1)??null,
  negative:a.filter(x=>x<0).length,over50ms:a.filter(x=>x>50).length,over100ms:a.filter(x=>x>100).length,over250ms:a.filter(x=>x>250).length};
};
const hashIdentity=identity=>Object.fromEntries(Object.entries(identity??{}).filter(([,v])=>typeof v==='string'&&/^[a-f0-9]{64}$/i.test(v)));
const rowObject=(columns,row)=>{
 assert.equal(row.length,columns.length,'Unexpected trace row width');
 return Object.fromEntries(columns.map((name,index)=>[name,row[index]]));
};
function decodeTrace(trace,owner){
 assert.ok(trace?.columns?.packet&&trace.columns.event&&trace.columns.phases,'Required packet trace missing');
 assert.ok(finite(trace.clock?.started?.timeOrigin),'Trace clock origin missing');
 const packets=new Map();
 for(const row of trace.packets){
  const packet=rowObject(trace.columns.packet,row);assert.ok(!packets.has(packet.id),'Duplicate packet id');
  assert.ok(Number.isInteger(packet.inputCount)&&packet.inputCount>=0&&packet.inputCount<=32);
  if(packet.inputCount)assert.equal(packet.latestFrame,packet.firstInputFrame+packet.inputCount-1);
  packets.set(packet.id,packet);
 }
 const ids=new Set(),events=trace.events.map(row=>{
  const raw=rowObject(trace.columns.event,row),packet=packets.get(raw.packetId);
  assert.ok(packet&&finite(raw.enteredAt)&&finite(raw.exitedAt),'Invalid event reference or clock');
  assert.ok(!ids.has(raw.id),'Duplicate event id');ids.add(raw.id);
  const phase=trace.columns.phases[raw.phase];assert.ok(typeof phase==='string');
  return {ref:owner+':'+raw.id,owner,raw,packet,phase,localAt:raw.enteredAt,
   at:trace.clock.started.timeOrigin+raw.enteredAt,end:trace.clock.started.timeOrigin+raw.exitedAt,
   key:trace.columns.packet.filter(name=>name!=='id').map(name=>packet[name]).join(':')};
 });
 events.sort((a,b)=>a.at-b.at||a.raw.id-b.raw.id);
 return {trace,packets,events};
}
function pairOccurrences(left,right){
 const buckets=new Map(),offsets=new Map(),matched=[],used=new Set();
 for(const event of right){if(!buckets.has(event.key))buckets.set(event.key,[]);buckets.get(event.key).push(event);}
 for(const event of left){
  const index=offsets.get(event.key)??0,target=buckets.get(event.key)?.[index];
  if(target){offsets.set(event.key,index+1);used.add(target.ref);matched.push([event,target]);}
 }
 const paired=new Set(matched.map(([event])=>event.ref));
 return {matched,unmatchedLeft:left.filter(e=>!paired.has(e.ref)),unmatchedRight:right.filter(e=>!used.has(e.ref))};
}
function stage(left,right){
 const result=pairOccurrences(left,right),rows=result.matched.map(([a,b])=>[a.ref,b.ref,b.at-a.at]);
 const duplicates=events=>{
  const counts=new Map();for(const e of events)counts.set(e.key,(counts.get(e.key)??0)+1);
  return [...counts.values()].reduce((sum,n)=>sum+Math.max(0,n-1),0);
 };
 return {leftCount:left.length,rightCount:right.length,matched:rows.length,
  leftDuplicateOccurrences:duplicates(left),rightDuplicateOccurrences:duplicates(right),
  latencyMs:dist(rows.map(row=>row[2])),columns:['fromEvent','toEvent','elapsedMs'],
  allNegativeOrOver100ms:rows.filter(row=>row[2]<0||row[2]>100),
  unmatchedLeft:result.unmatchedLeft.map(e=>e.ref),unmatchedRight:result.unmatchedRight.map(e=>e.ref)};
}
const sameEpoch=(a,b)=>a.sessionLo===b.sessionLo&&a.sessionHi===b.sessionHi&&a.sender===b.sender&&a.destination===b.destination;
function oldThenFresh(a,b){
 return sameEpoch(a,b)&&a.latestFrame!==0xffffffff&&a.senderFrame===b.senderFrame&&
  a.latestFrame+1===b.latestFrame&&a.ackFrame===b.ackFrame;
}
function coveredOldSend(a,b){
 return oldThenFresh(a,b)&&a.ackSequence===b.ackSequence&&a.frameAdvantage===b.frameAdvantage&&
  ((b.sequence-a.sequence)>>>0)===1&&(!a.inputCount||(b.inputCount>0&&
   b.firstInputFrame<=a.firstInputFrame&&b.latestFrame>=a.latestFrame));
}
function lowerBound(events,at,inclusive){
 let lo=0,hi=events.length;
 while(lo<hi){const mid=(lo+hi)>>>1;if(events[mid].localAt<at||(inclusive&&events[mid].localAt===at))lo=mid+1;else hi=mid;}
 return lo;
}
function peerCallbacks(peer,decoded,side,limit){
 const raf=peer.rafDiagnostics,samples=peer.measure?.samples;
 assert.ok(raf?.requests&&raf.frames&&Array.isArray(samples),'RAF diagnostics and samples are required');
 const frames=new Map(raf.frames.map(frame=>[frame.index,frame]));
 const intents=decoded.events.filter(e=>e.phase==='intent'&&e.packet.sender===side);
 const seen=new Set(),fresh=new Map(),sequenceEvents=[],sequences=new Map();
 for(const event of intents){
  const p=event.packet,epoch=[p.sessionLo,p.sessionHi,p.sender,p.destination].join(':');
  const previous=sequences.get(epoch);
  if(previous){const d=(p.sequence-previous.packet.sequence)>>>0;
   if(d!==1)sequenceEvents.push([previous.ref,event.ref,d,d===0?'reused-sequence':d<0x80000000?'unobserved-sequence':'older-sequence']);}
  sequences.set(epoch,event);
  const added=[];
  for(let i=0;i<p.inputCount;++i){const frame=p.firstInputFrame+i,key=epoch+':'+frame;if(!seen.has(key)){seen.add(key);added.push(frame);}}
  if(added.length)fresh.set(event.ref,added);
 }
 const callbacks=[],ownerByEvent=new Map(),pairs=[],freshRows=[];
 for(const q of raf.requests){
  if(q.frameCalls!==1||!finite(q.enteredAt)||!finite(q.exitedAt))continue;
  const f=frames.get(q.firstFrameIndex);assert.ok(f,'Missing onGameFrame record');
  if(f.sampleAfter!==f.sampleBefore+1)continue;
  const sample=samples[f.sampleBefore];assert.ok(Array.isArray(sample)&&sample.length===14,'Unsupported sample format');
  assert.ok(finite(f.enteredAt),'Sampled JS frame entry is missing');
  const events=intents.slice(lowerBound(intents,q.enteredAt,false),lowerBound(intents,q.exitedAt,true));
  const scope=sample[3]<180?'startup':sample[3]<limit?'formal':'tail';
  const previous=samples[f.sampleBefore-1],captureDelta=previous?sample[7]-previous[7]:null;
  const callback={rafIndex:q.index,sampleIndex:f.sampleBefore,frame:sample[3],scope,
   captureDelta,repairDelta:previous?sample[8]-previous[8]:null,intents:events.length,pairs:0,strictPairs:0,strictBeforeJs:0};
  for(const event of events){
   if(!ownerByEvent.has(event.ref))ownerByEvent.set(event.ref,[]);
   ownerByEvent.get(event.ref).push(q.index);
   if(fresh.has(event.ref))freshRows.push([q.index,f.sampleBefore,sample[3],event.raw.id,event.packet.id,
    fresh.get(event.ref).length,event.localAt-q.enteredAt,f.enteredAt-event.localAt,captureDelta,scope]);
  }
  for(let i=1;i<events.length;++i){
   const a=events[i-1],b=events[i];
   if(!oldThenFresh(a.packet,b.packet))continue;
   const strict=coveredOldSend(a.packet,b.packet),beforeJs=b.localAt<=f.enteredAt;
   ++callback.pairs;if(strict)++callback.strictPairs;if(strict&&beforeJs)++callback.strictBeforeJs;
   pairs.push([q.index,f.sampleBefore,sample[3],a.raw.id,b.raw.id,a.packet.id,b.packet.id,
    b.localAt-a.localAt,+strict,+beforeJs,scope]);
  }
  callbacks.push(callback);
 }
 const scopes={};
 for(const name of ['all','startup','formal','tail']){
  const c=callbacks.filter(row=>name==='all'||row.scope===name),p=pairs.filter(row=>name==='all'||row[10]===name),f=freshRows.filter(row=>name==='all'||row[9]===name);
  scopes[name]={singleFrameCallbacks:c.length,inputIntents:c.reduce((s,r)=>s+r.intents,0),
   callbacksWithMultipleInputIntents:c.filter(r=>r.intents>=2).length,oldThenFreshPairs:p.length,
   oldCoverageAndAckEquivalentPairs:p.filter(r=>r[8]).length,oldCoverageEquivalentBeforeJsFrame:p.filter(r=>r[8]&&r[9]).length,
   oldToFreshIntentMs:dist(p.map(r=>r[7])),firstAdvertisedPackets:f.length,newlyAdvertisedFrames:f.reduce((s,r)=>s+r[5],0),
   rafEntryToFirstAdvertisementMs:dist(f.map(r=>r[6])),firstAdvertisementToJsFrameEntryMs:dist(f.map(r=>r[7])),
   sampledCaptureDeltaSum:c.reduce((s,r)=>s+(r.captureDelta??0),0),sampledRepairDeltaSum:c.reduce((s,r)=>s+(r.repairDelta??0),0)};
 }
 return {rafValid:raf.valid,rafOverflow:raf.overflow,networkValid:decoded.trace.valid,networkCoverage:decoded.trace.coverage,
  totalIntentEvents:intents.length,scopes,pairColumns:['rafIndex','sampleIndex','frame','oldEventId','newEventId','oldPacketId','newPacketId','intentGapMs','oldCoverageAndAckEquivalent','beforeJsFrameEntry','scope'],pairs,
  freshColumns:['rafIndex','sampleIndex','frame','eventId','packetId','newlyAdvertisedFrames','rafEntryToIntentMs','intentToJsFrameEntryMs','sampledCaptureDelta','scope'],
  firstAdvertisementAllOver50ms:freshRows.filter(row=>row[6]>50||row[7]>50||row[6]<0||row[7]<0),
  captureToIntent:{available:false,reason:'No native sample completion/Capture-entry timestamp. firstWireAt is inside the send observer; subtracting it from intent measures observer work, not native capture latency.'},
  intentOutsideSingleFrameCallbacks:intents.filter(e=>!ownerByEvent.has(e.ref)).map(e=>e.ref),
  ambiguousCallbackMembership:[...ownerByEvent].filter(([,owners])=>owners.length>1),
  observedSequenceColumns:['previousEvent','currentEvent','unsignedDelta','classification'],observedSequenceExceptions:sequenceEvents,
  repairs:{firstSample:samples[0]?.[8]??null,lastSample:samples.at(-1)?.[8]??null,
   lastExported:peer.stats?.[5]??null,route:peer.route??null,
   scope:'Sample counter deltas include independent pumps between samples. Relay SendRepairTo is unsupported; zero repairs on relay does not exercise reliable RTC repair.'}};
}

function linkedRelayStages(decoded){
 const receives=new Map(decoded.events.filter(e=>e.phase==='receive').map(e=>[e.raw.id,e]));
 const schedules=new Map(),receiveSchedule=[],scheduleSend=[],scheduleCancel=[],receiveSend=[];
 for(const e of decoded.events.filter(e=>e.phase==='scheduled')){
  const receive=receives.get(e.raw.receiveEventId);
  assert.ok(receive,'Relay schedule has no receive event');assert.equal(receive.key,e.key,'Relay packet changed at schedule');
  assert.ok(!schedules.has(e.raw.scheduledId),'Duplicate relay schedule id');schedules.set(e.raw.scheduledId,e);
  receiveSchedule.push([receive,e]);
 }
 const terminal=new Set();
 for(const e of decoded.events.filter(e=>e.phase==='send'||e.phase==='cancelled')){
  const schedule=schedules.get(e.raw.scheduledId);
  assert.ok(schedule,'Relay terminal event has no schedule');assert.equal(schedule.key,e.key,'Relay packet changed at send/cancel');
  assert.equal(schedule.raw.receiveEventId,e.raw.receiveEventId);
  terminal.add(schedule.raw.scheduledId);
  if(e.phase==='send'){
   scheduleSend.push([schedule,e]);receiveSend.push([receives.get(e.raw.receiveEventId),e]);
  }else scheduleCancel.push([schedule,e]);
 }
 const summarize=rows=>{
  const values=rows.map(([a,b])=>[a.ref,b.ref,b.at-a.at]);
  return {matched:values.length,latencyMs:dist(values.map(r=>r[2])),columns:['fromEvent','toEvent','elapsedMs'],
   allNegativeOrOver100ms:values.filter(r=>r[2]<0||r[2]>100)};
 };
 const scheduledReceives=new Set(receiveSchedule.map(([a])=>a.raw.id));
 return {receiveToSchedule:summarize(receiveSchedule),scheduleToSend:summarize(scheduleSend),
  receiveToSend:summarize(receiveSend),scheduleToCancel:summarize(scheduleCancel),
  receivesWithoutSchedule:[...receives.values()].filter(e=>!scheduledReceives.has(e.raw.id)).map(e=>e.ref),
  schedulesWithoutTerminal:[...schedules.values()].filter(e=>!terminal.has(e.raw.scheduledId)).map(e=>e.ref)};
}

function networkStages(browsers,relay){
 const events=browsers.flatMap(t=>t.events).sort((a,b)=>a.at-b.at||a.ref.localeCompare(b.ref));
 const byPhase=(items,phase)=>items.filter(e=>e.phase===phase);
 const intents=byPhase(events,'intent'),sends=byPhase(events,'ws-send'),messages=byPhase(events,'message');
 const copies=byPhase(events,'poll-copy').filter(e=>e.raw.resultBytes>0&&!e.raw.threw);
 const receives=byPhase(relay.events,'receive'),relaySends=byPhase(relay.events,'send');
 const sentWithoutThrow=sends.filter(e=>!e.raw.threw),relaySentWithoutThrow=relaySends.filter(e=>!e.raw.threw);
 const bySender={};
 for(const sender of [0,1]){
  const select=list=>list.filter(e=>e.packet.sender===sender);
  bySender['P'+(sender+1)]={intentToWsSend:stage(select(intents),select(sends)),
   wsSendToRelayReceive:stage(select(sentWithoutThrow),select(receives)),
   relaySendToPeerMessage:stage(select(relaySentWithoutThrow),select(messages)),
   peerMessageToPositivePollCopy:stage(select(messages),select(copies))};
 }
 return {browserValid:browsers.every(t=>t.trace.valid===true),relayValid:relay.trace.valid,
  relayCoverage:relay.trace.coverage,relayOverflow:relay.trace.overflow,relayErrorCount:relay.trace.errors?.length??0,
  relayPendingCount:Array.isArray(relay.trace.pending)?relay.trace.pending.length:null,
  bySender,relayLinked:linkedRelayStages(relay),
  thrownBrowserEvents:events.filter(e=>e.raw.threw).map(e=>e.ref),
  thrownRelayEvents:relay.events.filter(e=>e.raw.threw).map(e=>e.ref),
  scope:'Complete header, FNV32 and destination keys; equal keys are matched as FIFO occurrences, never deduplicated. intentToWsSend includes all API attempts; downstream matching excludes synchronous thrown sends and preserves their references separately. WS send is API entry; relay receive is its JS callback entry; poll-copy is a positive native buffer copy, not SessionChannel acceptance. Cross-process clock error is not calibrated; negative deltas remain visible. All unmatched events, including stop-window tails, remain in this output.'};
}

function expectedTapeKeys(frame,side){
 if(frame===0)return 0;
 const cycle=Math.floor(frame/22)+side;
 return (frame<132?256:0)|(frame%126<90?1:0)|(frame%180<45?4:0)|
  (cycle%4===0?64:0)|(cycle%4===2?128:0);
}

function inputEvidence(report){
 const limit=report.parameters.limit,mode=report.parameters.inputMode;
 const expectedHashFrames=Array.from({length:Math.floor(limit/120)},(_,i)=>(i+1)*120);
 const peers=report.results.map((peer,side)=>{
  assert.ok(Array.isArray(peer.confirmedHashes),'Confirmed hash trace required');
  const hashes=new Map(),hashConflicts=[];
  for(const row of peer.confirmedHashes){
   assert.ok(Array.isArray(row)&&row.length===2&&row.every(Number.isInteger),'Malformed confirmed hash row');
   if(hashes.has(row[0])&&hashes.get(row[0])!==row[1])hashConflicts.push(row[0]);
   hashes.set(row[0],row[1]);
  }
  const tape=peer.measure?.inputTape;assert.ok(Array.isArray(tape),'Wire tape observation required');
  const prefix=tape.slice(0,limit),digest=sha(JSON.stringify(prefix)),mismatches=[];
  if(mode==='tape')for(let frame=0;frame<prefix.length;++frame)if(prefix[frame]!==expectedTapeKeys(frame,side))mismatches.push(frame);
  return {side:side+1,wireTape:{mode,observedFrames:tape.length,checkedPrefixFrames:prefix.length,
    sha256:digest,declaredSha256:peer.inputTapeSha256??null,declaredDigestMatches:peer.inputTapeSha256===digest,
    expectedScheduleAvailable:mode==='tape',expectedScheduleMismatchCount:mode==='tape'?mismatches.length:null,
    firstMismatchFrames:mismatches.slice(0,20),mismatchExampleLimit:20,completePrefix:tape.length>=limit},
   confirmedHashes:peer.confirmedHashes,conflictingDuplicateFrames:hashConflicts,
   missingExpectedHashFrames:expectedHashFrames.filter(frame=>!hashes.has(frame)),
   final:{frame:peer.stats?.[0]??null,confirmed:peer.stats?.[1]??null,hash:peer.hash??null,
    captures:peer.stats?.[4]??null,repairs:peer.stats?.[5]??null},
   _hashes:hashes,_tape:prefix};
 });
 const first=peers[0]._hashes,second=peers[1]._hashes;
 const commonFrames=[...first.keys()].filter(frame=>second.has(frame)).sort((a,b)=>a-b);
 const mismatches=commonFrames.filter(frame=>first.get(frame)!==second.get(frame));
 const expectedScheduleChecks=mode==='tape'&&peers.every(p=>p.wireTape.completePrefix&&p.wireTape.declaredDigestMatches&&p.wireTape.expectedScheduleMismatchCount===0);
 return {mode,expectedScheduleChecks,withinRunConfirmed:{sharedFrames:commonFrames,mismatchFrames:mismatches,
   expectedFrames:expectedHashFrames,completeAndEqual:peers.every(p=>!p.missingExpectedHashFrames.length&&!p.conflictingDuplicateFrames.length)&&!mismatches.length},
  peers,
  scope:'Offline tape checks cover observed local wire button sequences. The frozen harness also checks each observed analog input for neutrality, but this JSON has no full input payload to independently repeat that check. Matching FNV/header packets is not bitwise payload proof. Canonical hashes are compared only at identical confirmed frames; captures/corrections/model time are not browser CPU performance.'};
}

function publicInput(input){
 return {...input,peers:input.peers.map(({_hashes,_tape,...peer})=>peer)};
}

function analyzeReport(path){
 const absolute=resolve(path),bytes=readFileSync(absolute),raw=JSON.parse(bytes);
 assert.ok(Array.isArray(raw.results)&&raw.results.length===2,'Exactly two player results required');
 assert.ok(Number.isInteger(raw.parameters?.limit)&&raw.parameters.limit>180,'Frame limit required');
 assert.ok(Array.isArray(raw.errors),'Original report errors array required');
 const browsers=raw.results.map((peer,side)=>decodeTrace(peer.networkTransport,'P'+(side+1)));
 const relay=decodeTrace(raw.relayTransport,'relay');
 for(const browser of browsers)assert.deepEqual(browser.trace.columns.packet,relay.trace.columns.packet,'Packet column identities differ');
 const input=inputEvidence(raw);
 const value={source:{path:absolute,sha256:sha(bytes),bytes:bytes.length},rawPassed:raw.passed,rawErrorCount:raw.errors.length,
  sourceIdentity:hashIdentity(raw.identity),parameters:raw.parameters,
  route:raw.results.map(peer=>({route:peer.route,declaredPath:peer.routeEvidence?.declaredPath??null,
   selectedPath:peer.routeEvidence?.selectedPath??null})),
  sourceTraceIdentity:{relay:hashIdentity(raw.relayTransport.sourceIdentity)},
  clocks:{browserOrigins:browsers.map(t=>t.trace.clock.started.timeOrigin),relayOrigin:relay.trace.clock.started.timeOrigin},
  hostObservation:{present:!!raw.hostLoad,valid:raw.hostLoad?.valid??null,complete:raw.hostLoad?.complete??null,
   scope:'valid/complete describe sampling, not an idle or controlled host. No game sample is excluded according to host load.'},
  callbacks:raw.results.map((peer,side)=>peerCallbacks(peer,browsers[side],side,raw.parameters.limit)),
  transport:networkStages(browsers,relay),input:publicInput(input)};
 return {raw,input,value};
}

function compareReports(reference,candidate){
 const a=reference.value,b=candidate.value;
 const parameterKeys=[...new Set([...Object.keys(a.parameters),...Object.keys(b.parameters)])].sort();
 const parameterDifferences=parameterKeys.filter(key=>JSON.stringify(a.parameters[key])!==JSON.stringify(b.parameters[key]))
  .map(key=>({parameter:key,reference:a.parameters[key]??null,candidate:b.parameters[key]??null}));
 const identityKeys=[...new Set([...Object.keys(a.sourceIdentity),...Object.keys(b.sourceIdentity)])].sort();
 const identityDifferences=identityKeys.filter(key=>a.sourceIdentity[key]!==b.sourceIdentity[key])
  .map(key=>({field:key,reference:a.sourceIdentity[key]??null,candidate:b.sourceIdentity[key]??null}));
 const peerChecks=reference.input.peers.map((ap,side)=>{
  const bp=candidate.input.peers[side],frames=[...ap._hashes.keys()].filter(frame=>bp._hashes.has(frame)).sort((a,b)=>a-b);
  const hashMismatches=frames.filter(frame=>ap._hashes.get(frame)!==bp._hashes.get(frame));
  const available=reference.input.mode==='tape'&&candidate.input.mode==='tape',length=Math.min(ap._tape.length,bp._tape.length),tapeMismatches=[];
  if(available)for(let i=0;i<length;++i)if(ap._tape[i]!==bp._tape[i])tapeMismatches.push(i);
  return {side:side+1,tape:{available,commonPrefixFrames:available?length:null,
    mismatchCount:available?tapeMismatches.length:null,firstMismatchFrames:tapeMismatches.slice(0,20),
    completeComparableLength:available&&ap.wireTape.completePrefix&&bp.wireTape.completePrefix&&
     ap._tape.length===bp._tape.length&&a.parameters.limit===b.parameters.limit},
   confirmed:{commonFrames:frames,mismatchFrames:hashMismatches,
    onlyReferenceFrames:[...ap._hashes.keys()].filter(frame=>!bp._hashes.has(frame)),
    onlyCandidateFrames:[...bp._hashes.keys()].filter(frame=>!ap._hashes.has(frame))},
   formal:{reference:a.callbacks[side].scopes.formal,candidate:b.callbacks[side].scopes.formal}};
 });
 return {reference:a.source.sha256,candidate:b.source.sha256,parameterDifferences,identityDifferences,peers:peerChecks,
  captureToIntent:{available:false},causalCpuOrLatencyComparison:false,
  scope:'The same input tape and confirmed hashes establish workload/state correspondence where covered. Callback counts and rollback depth may still differ. Header-pair reductions are observed send-path behavior, not a CPU speedup or direct proof of lower capture latency. Independent pumps and reliable repair remain separate obligations. Wall-clock input runs are never declared the same tape.'};
}

function selfTest(){
 const event=(ref,key,at)=>({ref,key,at});
 const a=[event('a0','repeat',1),event('a1','repeat',2),event('a2','tail',3)];
 const b=[event('b0','repeat',4),event('b1','repeat',5),event('b2','orphan',6)];
 const matched=pairOccurrences(a,b);
 assert.deepEqual(matched.matched.map(([x,y])=>[x.ref,y.ref]),[['a0','b0'],['a1','b1']]);
 assert.deepEqual(matched.unmatchedLeft.map(e=>e.ref),['a2']);assert.deepEqual(matched.unmatchedRight.map(e=>e.ref),['b2']);
 const packet={sessionLo:1,sessionHi:2,sender:0,destination:1,sequence:40,ackSequence:25,frameAdvantage:0,
  latestFrame:31,firstInputFrame:20,inputCount:12,senderFrame:32,ackFrame:29};
 const next={...packet,sequence:41,latestFrame:32,inputCount:13};
 assert.ok(coveredOldSend(packet,next));
 assert.ok(oldThenFresh(packet,{...next,ackSequence:26}));assert.ok(!coveredOldSend(packet,{...next,ackSequence:26}));
 assert.ok(!coveredOldSend({...packet,firstInputFrame:0,inputCount:32},{...next,firstInputFrame:1,inputCount:32}));
 assert.ok(coveredOldSend({...packet,sequence:0xffffffff},{...next,sequence:0}));
 assert.ok(!coveredOldSend(packet,{...next,sequence:42}));assert.ok(!oldThenFresh(packet,{...next,sessionHi:3}));
 const fake={columns:{packet:['id','inputCount','latestFrame','firstInputFrame'],event:['id','phase','packetId','enteredAt','exitedAt'],phases:['intent']},
  clock:{started:{timeOrigin:100}},packets:[[9,1,7,7]],events:[[17,0,9,4,5]]};
 const decoded=decodeTrace(fake,'test');assert.equal(decoded.events[0].packet.id,9);assert.equal(decoded.events[0].at,104);
 assert.throws(()=>decodeTrace({...fake,events:[[17,0,8,4,5]]},'test'));
 assert.deepEqual(stage([event('negative-a','k',2)],[event('negative-b','k',1)]).latencyMs.negative,1);
 console.log(JSON.stringify({passed:true,scope:'Pure offline association and coverage contracts; no browser, traffic or native code.'}));
}

const args=process.argv.slice(2);
if(args.length===1&&args[0]==='--self-test')selfTest();
else{
 assert.ok(args.length>=2,'Usage: node analyze-input-send-batch.mjs OUTPUT.json BASELINE-report.json [CANDIDATE-report.json ...]');
 const [output,...paths]=args,reports=paths.map(analyzeReport),analyzerPath=fileURLToPath(import.meta.url);
 const result={version:1,scope:'Offline input-send batching evidence. Original raw reports and their failure flags remain authoritative.',
  analyzer:{path:analyzerPath,sha256:sha(readFileSync(analyzerPath))},
  definitions:{formal:'180 <= sampled logical frame < report.parameters.limit; startup and tail are retained separately.',
   callback:'A recorded actual rAF entry/exit contains exactly one sampled onGameFrame call. Browser rAF timestamp is not used as actual entry.',
   oldThenFresh:'Consecutive local input intents inside that callback: same session/sender/destination, senderFrame and ackFrame; latestFrame increments by one.',
   strictSubset:'Also sequence increments by one modulo uint32, ackSequence and frameAdvantage equal, and the second input range covers all of the first.',
   firstAdvertisement:'First observed intent containing each local input frame in its session; it is not native sampling time.',
   sequenceException:'An observation boundary, duplicate, transport rejection or other send can explain a sequence discontinuity. No discontinuity is relabeled as packet loss.',
   repair:'Relay counts cannot prove reliable RTC repair. Focused channel/session/Adonis gates and a separately covered RTC path are needed for that contract.'},
  reports:reports.map(r=>r.value),comparisons:reports.slice(1).map(r=>compareReports(reports[0],r)),
  limitations:['No native capture completion timestamp exists in these source reports.',
   'The sender observer validates the wire tape and sets the next synthetic keys before intent; its work is inside measured send paths.',
   'Hash/packet summaries do not replace complete game correctness, response coverage, audio or 4x effective-new-picture acceptance.',
   'Report errors are counted without copying external resource URLs or queries. No original failed run or exceptional timing is discarded.']};
 writeFileSync(resolve(output),JSON.stringify(result,null,2)+'\n',{flag:'wx'});
 console.log(JSON.stringify({output:resolve(output),sha256:sha(readFileSync(resolve(output))),sources:reports.map(r=>r.value.source),
  callbacks:reports.map(r=>({source:r.value.source.sha256,peers:r.value.callbacks.map(p=>p.scopes.formal)}))}));
}
