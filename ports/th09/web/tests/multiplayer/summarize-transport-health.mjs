#!/usr/bin/env node
// Pure analysis of explicitly named, already completed transport-health reports.
// Never starts browsers, relay children, builds, or performance runs.
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {createHash} from 'node:crypto';
import {fileURLToPath} from 'node:url';
import {healthSchedule,healthPacket,parseHealthPacket} from './transport-health-fixture.mjs';

const here=path.dirname(fileURLToPath(import.meta.url)),root=path.resolve(here,'../../..');
const artifacts=path.join(root,'web/artifacts/multiplayer-tests');
const sha=value=>createHash('sha256').update(value).digest('hex');
const knownSha={
  'rollback-transport-health-relay-none-01':'3dbe5667f3d420b948c5c2fef8ce2be6bc32cf3f55143f8a03e51401e94aecd9',
  'rollback-transport-health-rtc-none-01':'ebc8d613e682d8ba5f6ce1ab219a60b61a7ec94b1e07c4e3af563f853926cf3a',
  'rollback-transport-health-relay-block-01':'e54509e7d57d9f3482141134240c943cc46a679102f52609aed64446c797bf2c',
  'rollback-transport-health-rtc-block-01':'26535b389eaabbe4bcbd3a03ff344df8a8e8efe84c40cbecda3d10b8bccea90f',
  'rollback-transport-health-relay-burst-01':'b2fe3d7840b7c8629bd1fdcaf64054adf57b6f661a4eeed06afdc471e3b3e375'
};
const args=process.argv.slice(2),flags=new Set(args);
assert(args.length===flags.size&&args.every(arg=>['--include-blocks','--include-relay-burst'].includes(arg)),'Unknown or duplicate analysis option');
const names=['rollback-transport-health-relay-none-01','rollback-transport-health-rtc-none-01'];
if(flags.has('--include-blocks'))names.push('rollback-transport-health-relay-block-01','rollback-transport-health-rtc-block-01');
if(flags.has('--include-relay-burst'))names.push('rollback-transport-health-relay-burst-01');
const packetColumns=['id','sessionLo','sessionHi','sender','players','sequence','ackSequence','latestFrame',
  'ackFrame','firstInputFrame','senderFrame','frameAdvantage','inputCount','reserved','payloadBytes','fnv1a32','destination'];
function dist(values){
  const a=values.filter(Number.isFinite).toSorted((a,b)=>a-b),q=f=>a.length?a[Math.floor(f*(a.length-1))]:null;
  return {count:a.length,total:a.reduce((sum,x)=>sum+x,0),min:a[0]??null,p50:q(.5),p95:q(.95),p99:q(.99),max:a.at(-1)??null,
    negative:a.filter(x=>x<0).length,over100:a.filter(x=>x>100).length,over250:a.filter(x=>x>250).length,over1000:a.filter(x=>x>1000).length};
}
function normalize(raw,isRelay=false){
  const packetNames=isRelay?raw.columns.packet:raw.columns.packets;
  const eventNames=isRelay?raw.columns.event:raw.columns.events;
  assert.deepEqual(packetNames,packetColumns);
  const packets=new Map(raw.packets.map(row=>[row[0],Object.fromEntries(packetNames.map((name,i)=>[name,row[i]]))]));
  assert.equal(packets.size,raw.packets.length,'Packet IDs must be unique');
  const origin=raw.clock.started.timeOrigin;assert(Number.isFinite(origin));
  const events=raw.events.map(row=>{
    const event=Object.fromEntries(eventNames.map((name,i)=>[name,row[i]])),packet=packets.get(event.packetId);
    assert(packet,'Event references unknown packet');
    return {...event,packet,key:packetNames.slice(1).map(name=>packet[name]).join(':'),
      sequence:packet.sequence,lane:(packet.sequence-1)%2,at:origin+event.enteredAt};
  });
  assert.equal(new Set(events.map(row=>row.id)).size,events.length,'Event IDs must be unique');
  return events;
}
function link(before,after){
  const waiting=new Map(),pairs=[],unmatchedAfter=[];
  for(const row of before){if(!waiting.has(row.key))waiting.set(row.key,[]);waiting.get(row.key).push(row);}
  for(const row of after){const queue=waiting.get(row.key);if(!queue?.length){unmatchedAfter.push(row);continue;}
    const previous=queue.shift();pairs.push({before:previous,after:row,ms:row.at-previous.at});}
  return {pairs,unmatchedBefore:[...waiting.values()].flat(),unmatchedAfter};
}
function eventRef(row,stopEpoch){
  return {id:row.id,packetId:row.packetId,sequence:row.sequence,lane:row.lane,at:row.at,
    ageAtStopMs:Number.isFinite(stopEpoch)?stopEpoch-row.at:null};
}
function stage(before,after,stopEpoch){
  const linked=link(before,after);
  return {completedMs:dist(linked.pairs.map(row=>row.ms)),
    longest:linked.pairs.toSorted((a,b)=>b.ms-a.ms).slice(0,5).map(row=>({
      ms:row.ms,before:eventRef(row.before,stopEpoch),after:eventRef(row.after,stopEpoch)})),
    unmatchedBefore:linked.unmatchedBefore.map(row=>eventRef(row,stopEpoch)),
    unmatchedAfter:linked.unmatchedAfter.map(row=>eventRef(row,stopEpoch))};
}
function gaps(rows){
  const samples=rows.filter(row=>Number.isFinite(row.at)),values=[];
  for(let i=1;i<samples.length;i++)values.push({ms:samples[i].at-samples[i-1].at,before:samples[i-1],after:samples[i]});
  return {gapMs:dist(values.map(row=>row.ms)),largest:values.toSorted((a,b)=>b.ms-a.ms).slice(0,5)};
}
function hostWindow(host,start,end,label){
  if(!host)return null;
  const cpu=host.cpuSamples.filter(row=>Number.isFinite(row.busyFraction)&&row.utcMs-row.intervalMs>=start&&row.utcMs<=end);
  const processes=host.processSamples.filter(row=>row.sourceUtcMs>=start&&row.sourceUtcMs+row.sourceDurationMs<=end);
  let idle=0,total=0;for(const row of cpu){idle+=row.deltaIdleMs;total+=row.deltaTotalMs;}
  const found=new Map();
  for(let i=0;i<processes.length;i++){
    const previous=i?new Map(processes[i-1].rows.map(row=>[row[0]+':'+row[1],row])):new Map();
    for(const row of processes[i].rows){
      if(!/^(wasm-opt|wasm-ld|ninja|clang|clang\+\+|g\+\+|cc1plus|emcc|em\+\+|cmake)$/i.test(row[1]))continue;
      const key=row[0]+':'+row[1];if(!found.has(key))found.set(key,{pid:row[0],name:row[1],samples:0,
        intervals:0,deltaCpuSeconds:0,firstCpuSeconds:row[2],lastCpuSeconds:row[2],observedAtUtcMs:[]});
      const value=found.get(key);value.samples++;value.lastCpuSeconds=row[2];value.observedAtUtcMs.push(processes[i].sourceUtcMs);
      const prior=previous.get(key);
      if(prior&&Number.isFinite(row[2])&&Number.isFinite(prior[2])&&row[2]>=prior[2]){
        value.intervals++;value.deltaCpuSeconds+=row[2]-prior[2];
      }
    }
  }
  return {label,startEpochMs:start,endEpochMs:end,durationMs:end-start,
    fullyContainedCpuIntervals:cpu.length,busyPercent:dist(cpu.map(row=>row.busyFraction*100)),
    weightedBusyPercent:total?(1-idle/total)*100:null,completeProcessSamples:processes.length,
    compilerCandidates:[...found.values()],
    method:'Only fully contained CPU intervals and source-time process snapshots. CPU deltas require the same PID/name in consecutive retained snapshots; first sightings are not charged to this run.'};
}
function safeGlobalErrors(errors){
  return errors.map((message,index)=>({index,sha256:sha(message),
    category:message.startsWith('Unexpected resource request in no-WASM fixture:')?'fixture-resource':'other',
    observedFamily:message.startsWith('Unexpected resource request in no-WASM fixture:')&&/kaspersky/i.test(message)?'Kaspersky':null}));
}
function readRun(name){
  const file=path.join(artifacts,name+'-report.json');
  assert(fs.existsSync(file),'Required explicit report missing: '+file);
  const bytes=fs.readFileSync(file),raw=JSON.parse(bytes),digest=sha(bytes);
  if(knownSha[name])assert.equal(digest,knownSha[name],'Historical raw report changed: '+name);
  assert.equal(raw.schema,'th09/transport-health/1');assert.equal(raw.peers.length,2);
  const plan=healthSchedule(raw.parameters.frames),expected=plan.length;
  const metadata=raw.peers.map((peer,side)=>normalize(peer));
  const relay=normalize(raw.relayTrace,true),stopEpoch=Math.max(...raw.peers.map(peer=>peer.clock.started.timeOrigin+peer.stopAt));
  const checks=[],check=(condition,detail,kind='contract')=>checks.push({passed:!!condition,detail,kind});
  const errorClasses=safeGlobalErrors(raw.errors);
  check(errorClasses.every(error=>error.category==='fixture-resource'),'No original non-isolation errors');
  check(raw.identity.commonGit.head==='43add4dc9085c65468f7ab15428c00206cbe3834','Explicit frozen common43');
  check(raw.identity.files.every((before,i)=>before.sha256===raw.identity.after[i]?.sha256),'Sources unchanged during run');
  check(raw.fixture.scheduleSha256===sha(JSON.stringify(plan)),'Recomputed deterministic schedule SHA');
  const tapes=[0,1].map(side=>sha(Buffer.concat(plan.map(item=>Buffer.from(healthPacket(item,side,raw.parameters.seed))))));
  check(JSON.stringify(tapes)===JSON.stringify(raw.fixture.tapeSha256),'Recomputed deterministic packet tape SHAs');
  const peerSummary=raw.peers.map((peer,side)=>{
    check(peer.side===side,'Peer ordering matches side '+side);
    check(peer.done&&peer.nextPlan===expected,'P'+(side+1)+' completed all planned sends');
    check(peer.errors.length===0&&Object.values(peer.overflow).every(value=>value===0),'P'+(side+1)+' no fixture errors/overflow');
    check(peer.counts.invalidPayloads===0&&peer.counts.unrecognizedBinary===0,'P'+(side+1)+' exact byte-validation counters clean','delivery');
    check(peer.current.route===raw.parameters.route&&!peer.current.failed&&!peer.current.disconnected,'P'+(side+1)+' final transport route/live state','path');
    check(peer.states.filter(item=>item.kind==='transport'&&item.state.route).every(item=>item.state.route===raw.parameters.route),'P'+(side+1)+' no observed fallback','path');
    const events=metadata[side],phase=n=>events.filter(row=>row.phase===n),api=phase(0),sent=phase(1),messages=phase(2),copies=phase(3);
    for(const [phaseName,list,sender] of [['api',api,side],['send',sent,side],['message',messages,1-side],['poll-copy',copies,1-side]]){
      const sequences=list.map(row=>row.sequence).toSorted((a,b)=>a-b);
      check(sequences.length===expected&&sequences.every((sequence,i)=>sequence===i+1),'P'+(side+1)+' '+phaseName+' exactly-once full sequence set','delivery');
      check(list.every(row=>{
        const item=plan[row.sequence-1];if(!item)return false;
        const fields=parseHealthPacket(healthPacket(item,sender,raw.parameters.seed)).fields;
        return packetColumns.slice(1).every((name,i)=>row.packet[name]===fields[i]);
      }),'P'+(side+1)+' '+phaseName+' recomputed header/fingerprint/destination','delivery');
    }
    check(api.every(row=>row.result===1&&!row.threw)&&sent.every(row=>!row.threw),'P'+(side+1)+' API/send success','delivery');
    check(peer.current.queue===0,'P'+(side+1)+' queue drained at cutoff','delivery');
    const channels=new Map(peer.channels.map(channel=>[channel.id,channel]));
    const paths=peer.rtc.flatMap(sample=>sample.peers.filter(value=>value.pair).map(value=>({
      at:sample.at,relativeToScheduleMs:sample.at-peer.startAt,reason:sample.reason,route:sample.route,state:value.pair.state,
      nominated:value.pair.nominated??null,pairId:value.pair.id,
      localType:value.local?.candidateType??null,remoteType:value.remote?.candidateType??null,
      localProtocol:value.local?.protocol??null,remoteProtocol:value.remote?.protocol??null})));
    if(raw.parameters.route==='rtc'){
      check([...sent,...messages].every(row=>channels.get(row.channelId)?.kind==='rtc'),'P'+(side+1)+' all E9NP uses RTC data channels','path');
      check(paths.length>0&&paths.every(value=>value.route==='rtc'&&value.state==='succeeded'&&value.localType&&value.remoteType&&value.localType!=='relay'&&value.remoteType!=='relay'),'P'+(side+1)+' selected non-relay candidate evidence','path');
      check(peer.current.peers.length===1&&peer.current.peers.every(value=>
        value.input?.readyState==='open'&&value.input.ordered===false&&value.input.maxRetransmits===0&&value.input.maxPacketLifeTime===null&&
        value.control?.readyState==='open'&&value.control.ordered===true&&value.control.maxRetransmits===null&&value.control.maxPacketLifeTime===null),
      'P'+(side+1)+' original fast/reliable channel configuration','path');
    }else check([...sent,...messages].every(row=>channels.get(row.channelId)?.kind==='websocket'),'P'+(side+1)+' all E9NP uses relay WebSocket','path');
    // Original uses one parenthesized absolute deadline; reproduce its rounding.
    const storedLate=dist(api.map(row=>row.at-(peer.startEpochMs+Math.floor((row.sequence-1)/2)*1000/60)));
    assert.deepEqual(storedLate,raw.summary.peers[side].plannedDeadlineToApiMs,'Stored deadline distribution must reproduce');
    const apiToSend=stage(api,sent,stopEpoch),messageToCopy=stage(messages,copies,stopEpoch);
    assert.deepEqual(apiToSend.completedMs,raw.summary.peers[side].apiToActualSend.completedMs);
    assert.deepEqual(messageToCopy.completedMs,raw.summary.peers[side].messageToPollCopy.completedMs);
    const sendDuration=dist(sent.map(row=>row.exitedAt-row.enteredAt));
    assert.deepEqual(sendDuration,raw.summary.peers[side].actualSendDurationMs);
    const heartbeat=gaps(peer.heartbeats.filter(row=>row.at>=peer.startAt).map((row,i)=>({id:i,at:row.at,queue:row.queue,nextPlan:row.nextPlan})));
    const raf=gaps(peer.rafs.filter(row=>Number.isFinite(row.enteredAt)&&row.enteredAt>=peer.startAt).map(row=>({
      id:row.id,at:row.enteredAt,exitedAt:row.exitedAt,rafTimestamp:row.rafTimestamp})));
    assert.deepEqual(heartbeat.gapMs,raw.summary.peers[side].heartbeat.gapMs);
    assert.deepEqual(raf.gapMs,raw.summary.peers[side].raf.gapMs);
    const lanes=[0,1].map(lane=>{
      const a=api.filter(row=>row.lane===lane),s=sent.filter(row=>row.lane===lane),m=messages.filter(row=>row.lane===lane),c=copies.filter(row=>row.lane===lane);
      const controlOrdered=m.every((row,i)=>i===0||row.sequence>m[i-1].sequence);
      if(lane===1&&raw.parameters.route==='rtc')check(controlOrdered,'P'+(side+1)+' reliable control lane preserves order','delivery');
      return {lane:lane===0?'input':'control',api:a.length,send:s.length,message:m.length,pollCopy:c.length,
        receivedOutOfOrder:m.filter((row,i)=>i>0&&row.sequence<m[i-1].sequence).length,
        actualSendGaps:gaps(s.map(row=>({id:row.id,sequence:row.sequence,at:row.at}))),
        messageEntryGaps:gaps(m.map(row=>({id:row.id,sequence:row.sequence,at:row.at})))};
    });
    return {side,counts:peer.counts,lanes,deadlineToApi:storedLate,apiToActualSend:apiToSend,messageToPollCopy:messageToCopy,actualSendDurationMs:sendDuration,
      bufferedBefore:dist(sent.map(row=>row.bufferedBefore)),bufferedAfter:dist(sent.map(row=>row.bufferedAfter)),heartbeat,raf,
      queue:{maximumObserved:Math.max(...events.map(row=>Math.max(row.queueBefore,row.queueAfter)),...peer.heartbeats.map(row=>row.queue)),atStop:peer.current.queue},
      paths,channels:peer.current.peers.map(value=>({peerId:value.peerId,input:{label:value.input?.label,ordered:value.input?.ordered,
        maxRetransmits:value.input?.maxRetransmits,maxPacketLifeTime:value.input?.maxPacketLifeTime,readyState:value.input?.readyState},
        control:{label:value.control?.label,ordered:value.control?.ordered,maxRetransmits:value.control?.maxRetransmits,
          maxPacketLifeTime:value.control?.maxPacketLifeTime,readyState:value.control?.readyState}})),
      blocks:peer.blocks.map(block=>({...block,actualDurationMs:block.exitedAt-block.enteredAt,
        startLatenessMs:block.enteredAt-block.plannedAt,enteredEpochMs:peer.clock.started.timeOrigin+block.enteredAt,
        exitedEpochMs:peer.clock.started.timeOrigin+block.exitedAt})),clock:peer.clock,startEpochMs:peer.startEpochMs,stopEpochMs:peer.clock.started.timeOrigin+peer.stopAt,
      clockPrecisionNote:'Date.now is integer milliseconds; cross-process mapped differences are retained without zero-clamping.'};
  });
  check(raw.relayTrace.valid,'Relay observer valid, including no-input direct case');
  check(raw.parameters.route==='rtc'?!raw.relayTrace.coverage.exercised&&relay.length===0:raw.relayTrace.coverage.exercised,'Relay data coverage agrees with actual route','path');
  const directions=[0,1].map(sender=>{
    const sent=metadata[sender].filter(row=>row.phase===1),message=metadata[1-sender].filter(row=>row.phase===2);
    const combined=stage(sent,message,stopEpoch);
    assert.deepEqual(combined.completedMs,raw.summary.directions[sender].actualSendToPeerMessage.completedMs);
    check(combined.unmatchedBefore.length===0&&combined.unmatchedAfter.length===0,'Direction '+sender+' all sender/recipient occurrences linked','delivery');
    const out={sender,recipient:1-sender,actualSendToMessage:combined,
      byLane:[0,1].map(lane=>({lane:lane===0?'input':'control',...stage(sent.filter(row=>row.lane===lane),message.filter(row=>row.lane===lane),stopEpoch)}))};
    if(raw.parameters.route==='relay'){
      const received=relay.filter(row=>row.phase===0&&row.packet.sender===sender),forwarded=relay.filter(row=>row.phase===2&&row.packet.sender===sender);
      out.actualSendToRelayReceive=stage(sent,received,stopEpoch);out.relaySendToMessage=stage(forwarded,message,stopEpoch);
      assert.deepEqual(out.actualSendToRelayReceive.completedMs,raw.summary.directions[sender].actualSendToRelayReceive.completedMs);
      assert.deepEqual(out.relaySendToMessage.completedMs,raw.summary.directions[sender].relaySendToPeerMessage.completedMs);
      const byId=new Map(relay.map(row=>[row.id,row])),causal=[];
      for(const row of forwarded){const before=byId.get(row.receiveEventId);
        check(before?.phase===0&&before.key===row.key,'Relay explicit causal link for send '+row.id);
        if(before?.phase===0&&before.key===row.key)causal.push({ms:row.at-before.at,sequence:row.sequence,lane:row.lane,
          receiveEventId:before.id,sendEventId:row.id,scheduledId:row.scheduledId,receiveAt:before.at,sendAt:row.at,
          plannedDelayMs:row.plannedDelayMs,dueAt:row.dueAt,burst:!!row.burst,timerLatenessMs:row.enteredAt-row.dueAt});}
      out.relayReceiveToSend={completedMs:dist(causal.map(row=>row.ms)),longest:causal.toSorted((a,b)=>b.ms-a.ms).slice(0,5),
        byInjectionBucket:[false,true].map(burst=>{const cohort=causal.filter(row=>row.burst===burst);return {burst,
          count:cohort.length,plannedDelayMs:dist(cohort.map(row=>row.plannedDelayMs)),
          actualReceiveToSendMs:dist(cohort.map(row=>row.ms)),timerLatenessMs:dist(cohort.map(row=>row.timerLatenessMs))};})};
      assert.deepEqual(out.relayReceiveToSend.completedMs,raw.summary.directions[sender].relayReceiveToActualSend.completedMs);
      check(received.length===expected&&forwarded.length===expected,'Relay complete direction '+sender,'delivery');
    }
    return out;
  });
  const blockCausal=[];
  for(const peer of peerSummary)for(const block of peer.blocks){
    const begin=block.enteredEpochMs,end=block.exitedEpochMs,other=1-peer.side;
    const sent=metadata[other].filter(row=>row.phase===1&&row.at>=begin&&row.at<end);
    const messages=new Map(metadata[peer.side].filter(row=>row.phase===2).map(row=>[row.key,row]));
    const copies=new Map(metadata[peer.side].filter(row=>row.phase===3).map(row=>[row.key,row]));
    const received=new Map(relay.filter(row=>row.phase===0).map(row=>[row.key,row]));
    const forwarded=new Map(relay.filter(row=>row.phase===2).map(row=>[row.key,row]));
    const packets=sent.map(row=>({sequence:row.sequence,lane:row.lane,senderEventId:row.id,
      actualSendAt:row.at,relayReceiveAt:received.get(row.key)?.at??null,relaySendAt:forwarded.get(row.key)?.at??null,
      messageAt:messages.get(row.key)?.at??null,pollCopyAt:copies.get(row.key)?.at??null}));
    blockCausal.push({blockedPeer:peer.side,sendingPeer:other,block,
      senderCallsInsideBlock:sent.length,
      receiverMessagesInsideBlock:metadata[peer.side].filter(row=>row.phase===2&&row.at>=begin&&row.at<end).length,
      relayReceivedInsideBlock:packets.filter(row=>row.relayReceiveAt!==null&&row.relayReceiveAt>=begin&&row.relayReceiveAt<end).length,
      relaySentInsideBlock:packets.filter(row=>row.relaySendAt!==null&&row.relaySendAt>=begin&&row.relaySendAt<end).length,
      allSubsetMessagesAfterBlock:packets.every(row=>row.messageAt!==null&&row.messageAt>=end),packets,
      scope:'Subset whose peer send entry is inside the measured deliberate busy-loop. Other packets and all outliers remain in complete distributions/raw reports. RTC has no OS-arrival timestamp.'});
  }
  const host=raw.hostLoad;
  const environment=host?{valid:host.valid,complete:host.complete,counts:host.counts,logicalCpus:host.cpuSamples[0]?.logicalCpus,
    full:hostWindow(host,host.startedAtUtcMs,host.endedAtUtcMs,'Full host capture, including setup/report transfer/cleanup'),
    active:hostWindow(host,raw.fixture.commonStartEpochMs,stopEpoch,'Common planned start through latest actual browser stop'),
    note:'valid means sampling completeness, not a quiet or stable machine. Aggregate load does not prove per-core/frequency/thermal stability; 5-second process sampling can miss short tasks. No sample is filtered by host results.'}:null;
  const transportIntegrity=checks.every(item=>item.passed);
  const isolated=errorClasses.every(error=>error.category!=='fixture-resource');
  return {name,raw:{path:file,sha256:digest,bytes:bytes.length,originalPassed:raw.passed,globalErrors:errorClasses},
    verdict:{transportIntegrity,dataDeliveryIntegrity:checks.filter(item=>item.kind==='delivery').every(item=>item.passed),
      strictPathEvidence:checks.filter(item=>item.kind==='path').every(item=>item.passed),fixtureIsolation:isolated,productionAcceptance:false,performanceCausalComparison:false,
      interpretation:isolated?'Fixture resource boundary retained; other host conditions still apply.':'External injected resources were observed. Original overall failure is preserved; transport delivery and isolation are reported separately.'},
    integrityChecks:{count:checks.length,failed:checks.filter(item=>!item.passed)},
    parameters:raw.parameters,identity:{common:raw.identity.commonGit,expectedCommonHead:raw.identity.expectedCommonHead,
      files:raw.identity.files,functionBodies:raw.identity.functionBodies,relaySource:raw.relayTrace.sourceIdentity},
    fixture:{scheduleSha256:raw.fixture.scheduleSha256,tapeSha256:raw.fixture.tapeSha256,plannedPacketsPerSide:expected,
      synchronousApplicationSend:raw.fixture.synchronousApplicationSend,clientImpairmentTimer:raw.fixture.clientImpairmentTimer},
    browsers:raw.browsers.map(item=>({side:item.side,version:item.version,requestedCpuRate:item.requestedCpuRate,
      hardwareConcurrency:item.hardwareConcurrency,initialVisibility:item.visibility,initialHasFocus:item.hasFocus})),
    peers:peerSummary,directions,blockCausal,relay:{valid:raw.relayTrace.valid,coverage:raw.relayTrace.coverage,counts:raw.relayTrace.counts,
      overflow:raw.relayTrace.overflow,pending:raw.relayTrace.pending,injection:raw.relayTrace.injection,clock:raw.relayTrace.clock},
    host:environment,lifecycle:raw.lifecycle.events,
    limits:['No WASM/game/audio/Draw code is loaded by the fixture; externally injected page scripts prevent a claim of an isolated blank page.',
      'control uses eagler_peer_send(control=1), not eagler_peer_send_repair_to. Its 32 KiB size/backpressure gates and C++ automatic repair policy are untested.',
      'The poll-copy body is the exact common43 JS transport dequeue, not native ApplyInputPacket or game confirmation.',
      'RTC candidate evidence is local host-to-host UDP on one machine; it is not public, remote, mobile or TURN acceptance.',
      'WS/DC send entry is an observed API boundary, not OS wire departure; message entry is after browser dispatch.',
      'RTC includes connection maintenance and periodic getStats observation absent from an empty relay peer list. Different route runs are not a causal cost experiment.',
      'Independent relay injection schedules application messages in the relay process; its timers can change forwarding order. This is not packet-level TCP delay/loss emulation.',
      'rAF here has no drawing. Its callback gaps are scheduling observations, not effective game presentation FPS.']};
}
const runs=names.map(readRun),baseline=runs[0];
const commonParameters=value=>{const {route,blockMs,impairmentSite,injection,...same}=value;return same;};
for(const run of runs){
  assert.deepEqual(run.fixture.scheduleSha256,baseline.fixture.scheduleSha256);
  assert.deepEqual(run.fixture.tapeSha256,baseline.fixture.tapeSha256);
  assert.deepEqual(commonParameters(run.parameters),commonParameters(baseline.parameters));
  assert.deepEqual(run.identity.functionBodies,baseline.identity.functionBodies);
  assert.deepEqual(run.identity.files.map(row=>[row.path,row.sha256]),baseline.identity.files.map(row=>[row.path,row.sha256]));
  assert.deepEqual(run.browsers.map(row=>[row.version,row.requestedCpuRate]),baseline.browsers.map(row=>[row.version,row.requestedCpuRate]));
}
const evidence={schema:'th09/transport-health-evidence/1',generator:{path:fileURLToPath(import.meta.url),
  sha256:sha(fs.readFileSync(fileURLToPath(import.meta.url)))},explicitSources:names,
  analysisValidation:{allRequiredSourcesPresent:true,knownOriginalReportHashesVerified:true,storedDistributionsIndependentlyRecomputed:true,
    sameDeterministicScheduleAndPacketTapes:true,sameFrozenTransportAndHarnessSources:true,noOutlierFiltering:true},
  interpretation:{currentTransportIntegrity:runs.every(run=>run.verdict.transportIntegrity),
    allDataDeliveryIntegrity:runs.every(run=>run.verdict.dataDeliveryIntegrity),
    allStrictPathEvidence:runs.every(run=>run.verdict.strictPathEvidence),allFixtureIsolation:runs.every(run=>run.verdict.fixtureIsolation),
    originalFailuresPreserved:true,gamePerformanceAcceptance:false,
    routeComparison:'Separate sequential observations with different process/browser dispatch conditions, external page injection and host activity. Do not infer causal speedup or regression from route totals.',
    pendingExplicitGroups:[...(!flags.has('--include-blocks')?['relay-block-01 and rtc-block-01']:[]),
      ...(!flags.has('--include-relay-burst')?['relay-burst-01']:[])]},
  runs};
const output=path.join(artifacts,'transport-health-chain-evidence.json');
const encoded=JSON.stringify(evidence,null,2)+'\n';
assert(!/(?:https?|wss?):\/\//i.test(encoded),'Sanitized evidence must not contain resource URLs or query strings');
fs.writeFileSync(output,encoded);
console.log(JSON.stringify({output,bytes:Buffer.byteLength(encoded),sha256:sha(encoded),sources:names,
  gates:runs.map(run=>({name:run.name,...run.verdict,originalPassed:run.raw.originalPassed,integrityFailures:run.integrityChecks.failed.length}))}));

