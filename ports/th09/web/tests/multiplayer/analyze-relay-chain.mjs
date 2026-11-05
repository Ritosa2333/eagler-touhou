// Read-only offline analysis of the recorded relay/browser packet chain.
// Keeps unmatched occurrences and all >250 ms display/RAF gaps. No browser is launched.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
const [input,output]=process.argv.slice(2);assert.ok(input&&output,'Pass input report and new output JSON paths');
const raw=readFileSync(input),report=JSON.parse(raw),peers=report.results||report.partial,relay=report.relayTransport;
assert.ok(peers?.length===2&&relay,'Expected two recorded peers and a relay trace');
const sha=b=>createHash('sha256').update(b).digest('hex'),base=peers[0].networkTransport.clock.started.timeOrigin;
const dist=a=>{a=a.slice().sort((a,b)=>a-b);return {count:a.length,p50:a[Math.floor(a.length*.5)]??null,p95:a[Math.floor(a.length*.95)]??null,p99:a[Math.floor(a.length*.99)]??null,max:a.at(-1)??null,min:a[0]??null,over250ms:a.filter(x=>x>250).length,negative:a.filter(x=>x<0).length};};
const clock=t=>Object.fromEntries(Object.entries(t.clock).filter(([,v])=>v&&typeof v==='object'&&'timeOrigin'in v).map(([k,v])=>[k,{...v,dateMinusPerformanceMs:v.epochMs-v.timeOrigin-(v.before+v.after)/2,pairWidthMs:v.after-v.before}]));
function decode(t,side,isRelay=false){
 const pc=t.columns.packet,ec=t.columns.event,ps=t.columns.phases;
 const map=new Map(t.packets.map(row=>{assert.equal(row.length,pc.length);return [row[0],Object.fromEntries(pc.map((n,i)=>[n,row[i]]))];}));
 let ordinal=0;
 const events=t.events.map(row=>{const e=Object.fromEntries(ec.map((n,i)=>[n,row[i]])),p=map.get(e.packetId);assert.ok(p);
  const key=pc.slice(1).map(n=>p[n]).join(':');
  const entry={ref:(isRelay?'relay':'p'+side)+':'+e.id,side,phase:ps[e.phase],header:p,key,
   at:t.clock.started.timeOrigin-base+e.enteredAt,end:t.clock.started.timeOrigin-base+e.exitedAt,raw:e};
  if(!isRelay&&entry.phase==='poll-copy'&&e.resultBytes>0)entry.dequeueOrdinal=++ordinal;return entry;
 });
 return {trace:t,events};
}
const browser=peers.map((p,i)=>decode(p.networkTransport,i)),rt=decode(relay,-1,true),allBrowser=browser.flatMap(p=>p.events);
const phase=(list,name)=>list.filter(e=>e.phase===name),pairings={};
function pair(name,left,right){
 const buckets=new Map(),used=new Set(),map=new Map();
 for(const e of [...right].sort((a,b)=>a.at-b.at||a.raw.id-b.raw.id)){if(!buckets.has(e.key))buckets.set(e.key,[]);buckets.get(e.key).push(e);}
 const positions=new Map();
 for(const e of [...left].sort((a,b)=>a.at-b.at||a.raw.id-b.raw.id)){
  const pos=positions.get(e.key)||0,target=buckets.get(e.key)?.[pos];
  if(target){positions.set(e.key,pos+1);map.set(e.ref,target);used.add(target.ref);}
 }
 const duplicates=values=>{const m=new Map();for(const e of values)m.set(e.key,(m.get(e.key)||0)+1);return [...m.values()].reduce((s,n)=>s+Math.max(0,n-1),0);};
 pairings[name]={leftCount:left.length,rightCount:right.length,matched:map.size,leftDuplicateOccurrences:duplicates(left),rightDuplicateOccurrences:duplicates(right),
  unmatchedLeft:left.filter(e=>!map.has(e.ref)).map(e=>e.ref),unmatchedRight:right.filter(e=>!used.has(e.ref)).map(e=>e.ref)};
 return map;
}
const intentToSend=pair('intent_to_ws',phase(allBrowser,'intent'),phase(allBrowser,'ws-send'));
const wsToReceive=pair('ws_to_relay_receive',phase(allBrowser,'ws-send'),phase(rt.events,'receive'));
const sendToMessage=pair('relay_send_to_message',phase(rt.events,'send'),phase(allBrowser,'message'));
const messageToCopy=pair('message_to_positive_copy',phase(allBrowser,'message'),phase(allBrowser,'poll-copy').filter(e=>e.raw.resultBytes>0&&!e.raw.threw));
const reverse=map=>new Map([...map].map(([k,v])=>[v.ref,k]));
const byRef=new Map([...allBrowser,...rt.events].map(e=>[e.ref,e]));
const intentBySend=reverse(intentToSend),wsByReceive=reverse(wsToReceive);
const schedules=new Map(),sends=new Map(),cancels=new Map();
for(const e of rt.events){
 if(e.phase==='scheduled'){const id=e.raw.receiveEventId;if(!schedules.has(id))schedules.set(id,[]);schedules.get(id).push(e);}
 if(e.phase==='send')sends.set(e.raw.scheduledId,e);
 if(e.phase==='cancelled')cancels.set(e.raw.scheduledId,e);
}
function safeAfter(copy){
 if(!copy)return null;const p=peers[copy.side].networkTransport,atOffset=p.clock.started.timeOrigin-base;
 const row=p.pacing.find(row=>row[3]>=copy.dequeueOrdinal&&atOffset+row[0]>=copy.end);
 return row?{at:atOffset+row[0],raw:row,columns:p.columns.pacing}:null;
}
const compact=e=>e?{ref:e.ref,side:e.side,at:e.at,end:e.end,raw:e.raw}:null;
const chains=phase(rt.events,'receive').map(receive=>{
 const ws=byRef.get(wsByReceive.get(receive.ref)),intent=ws&&byRef.get(intentBySend.get(ws.ref));
 const forwards=(schedules.get(receive.raw.id)||[]).map(schedule=>{
  assert.equal(schedule.key,receive.key,'Relay link changed packet identity');
  const send=sends.get(schedule.raw.scheduledId),cancel=cancels.get(schedule.raw.scheduledId),message=send&&sendToMessage.get(send.ref),copy=message&&messageToCopy.get(message.ref);
  return {schedule:compact(schedule),send:compact(send),cancel:compact(cancel),message:compact(message),copy:compact(copy),safeFrontier:safeAfter(copy)};
 });
 return {header:receive.header,key:receive.key,intent:compact(intent),ws:compact(ws),receive:compact(receive),forwards,
  outsideSenderObservation:receive.at<(peers[receive.header.sender].networkTransport.clock.started.timeOrigin-base+peers[receive.header.sender].networkTransport.clock.started.before)||
   receive.at>(peers[receive.header.sender].networkTransport.clock.stopped.timeOrigin-base+peers[receive.header.sender].networkTransport.clock.stopped.after)};
});
const stages={intentToWs:[],wsToRelayReceive:[],relayReceiveToSchedule:[],relayScheduleToSend:[],relaySendToMessage:[],messageToCopy:[],copyToSafeFrontier:[]};
const segment=(name,a,b,ref)=>{if(a&&b)stages[name].push({ms:b.at-a.at,from:a.ref??null,to:b.ref??null,chain:ref});};
for(const c of chains){segment('intentToWs',c.intent,c.ws,c.receive.ref);segment('wsToRelayReceive',c.ws,c.receive,c.receive.ref);
 for(const f of c.forwards){segment('relayReceiveToSchedule',c.receive,f.schedule,c.receive.ref);segment('relayScheduleToSend',f.schedule,f.send,c.receive.ref);
  segment('relaySendToMessage',f.send,f.message,c.receive.ref);segment('messageToCopy',f.message,f.copy,c.receive.ref);segment('copyToSafeFrontier',f.copy,f.safeFrontier,c.receive.ref);}
}
const displayGaps=peers.map((p,side)=>{let previous;const gaps=[];
 for(const [index,s]of p.measure.samples.entries())if(!previous||(s[11]!==previous.s[11]&&s[3]>previous.s[3])){
  if(previous)gaps.push({side,indexBefore:previous.index,indexAfter:index,frameBefore:previous.s[3],frameAfter:s[3],
   before:previous.s,after:s,rafGapMs:s[13]-previous.s[13],completionGapMs:s[0]-previous.s[0],
   formal:previous.s[3]>=180&&s[3]<report.parameters.limit});
  previous={index,s};
 }return {side,count:gaps.length,allOver250ms:gaps.filter(g=>g.rafGapMs>250||g.completionGapMs>250),maximum:gaps.reduce((a,b)=>!a||b.rafGapMs>a.rafGapMs?b:a,null)};
});
const delayed=[];
for(let side=0;side<2;++side){const d=peers[side].rafDiagnostics;
 for(const q of d.requests)if(q.enteredAt!==null&&q.enteredAt-q.requestedAt>250){const parent=d.requests.find(p=>p.index===q.parentIndex);
  delayed.push({side,game:!!d.callbacks[q.callbackId]?.game,pendingMs:q.enteredAt-q.requestedAt,request:q,parent,
   parentFrames:d.frames.filter(f=>f.rafIndex===parent?.index),frames:d.frames.filter(f=>f.rafIndex===q.index)});
 }}
const longest=delayed.filter(x=>x.game&&x.parentFrames.some(f=>f.sampleAfter>f.sampleBefore)).sort((a,b)=>b.pendingMs-a.pendingMs)[0]||null;
function valueDelta(p,before,after){
 if(!before||!after)return null;
 return {beforeSample:before.sampleIndex,afterSample:after.sampleIndex,elapsedMs:after.observedAt-before.observedAt,
  fields:Object.fromEntries(p.liveCallbackCosts.fieldNames.map((name,i)=>[name,after.values[i]-before.values[i]]).filter(([name])=>!['version','enabled','epoch','phaseCount','counterCount','gaugeCount'].includes(name)))};
}
let episode=null;
if(longest){
 const owner=peers[longest.side],offset=owner.networkTransport.clock.started.timeOrigin-base;
 const start=offset+longest.parent.exitedAt,end=offset+longest.request.enteredAt;
 episode={selection:'Largest pending game RAF after an active sample; all startup and other delayed RAF records remain in allRafPendingOver250ms.',side:longest.side,start,end,durationMs:end-start,request:longest,peers:[]};
 for(let side=0;side<2;++side){
  const p=peers[side],t=p.networkTransport,off=t.clock.started.timeOrigin-base,lo=start-off,hi=end-off;
  const pacings=t.pacing.filter(a=>a[0]>=lo&&a[0]<hi),frames=p.rafDiagnostics.frames.filter(f=>f.enteredAt>=lo&&f.enteredAt<hi);
  const live=p.liveCallbackCosts.frames,before=live.filter(f=>f.observedAt<=lo).at(-1),after=live.find(f=>f.observedAt>=hi);
  const prior=before&&live.find(f=>f.sampleIndex===before.sampleIndex-1);
  const eventRows=browser[side].events.filter(e=>e.at>=start&&e.at<end);
  const blocks=p.audioOutput.blocks,originEstimates=blocks.map(b=>p.measure.started+1000*(b[2]-p.measure.audioStart)-b[0]).sort((a,b)=>a-b);
  const origin=originEstimates[Math.floor(originEstimates.length/2)];
  episode.peers.push({side,localWindow:[lo,hi],lifecycle:p.lifecycle,rafCalls:p.rafDiagnostics.requests.filter(q=>q.enteredAt>=lo&&q.enteredAt<hi).length,
   gameCallbacks:frames.length,cppWorkTotalMs:frames.reduce((sum,f)=>sum+f.cppWorkMs,0),
   controls:p.rafDiagnostics.controls.filter(q=>q.enteredAt>=lo&&q.enteredAt<hi),pacing:pacings,
   networkByPhase:Object.fromEntries(['intent','ws-send','message','poll-copy'].map(name=>[name,{all:eventRows.filter(e=>e.phase===name).length,
    nonemptyInput:eventRows.filter(e=>e.phase===name&&e.header.inputCount>0).length,refs:eventRows.filter(e=>e.phase===name).map(e=>e.ref)}])),
   beforeLive:before?{...before,values:undefined}:null,afterLive:after?{...after,values:undefined}:null,
   precedingSampleCosts:valueDelta(p,prior,before),acrossGapCosts:valueDelta(p,before,after),
   audio:{blocks:blocks.length,relativeCallbackGapMs:dist(blocks.slice(1).map((b,i)=>b[0]-blocks[i][0])),
    inferredOrigin:{min:originEstimates[0],median:origin,max:originEstimates.at(-1)},
    approximateBlocksInWindow:blocks.map((b,index)=>({index,row:b,inferredAt:origin+b[0]})).filter(b=>b.inferredAt>=lo&&b.inferredAt<hi),
    limitation:'The audio probe omits its wall-clock origin. Window alignment is inferred from measure.started/audioStart and AudioContext time; callback-relative gaps are directly observed.'}});
 }
 const previousFrame=longest.parentFrames.at(-1)?.sampleBefore,missingFrame=owner.measure.samples[previousFrame]?.[3];
 const covering=chains.filter(c=>c.header.sender===longest.side&&c.header.inputCount>0&&c.header.firstInputFrame<=missingFrame&&missingFrame<c.header.firstInputFrame+c.header.inputCount);
 episode.missingSourceFrame=missingFrame;episode.coveringChains=covering.sort((a,b)=>(a.ws?.at??Infinity)-(b.ws?.at??Infinity)).map(c=>c.receive.ref);
 const timing=relay.timing.filter(a=>relay.clock.started.timeOrigin-base+a[0]>=start&&relay.clock.started.timeOrigin-base+a[0]<end);
 episode.relayTiming={rows:timing,gapMs:dist(timing.map(a=>a[3])),lateMs:dist(timing.map(a=>a[2]))};
 const host=report.hostLoad;
 if(host){const lo=base+start,hi=base+end,proc=host.processSamples.filter(p=>p.sourceUtcMs>=lo-5000&&p.sourceUtcMs<=hi+5000);
  const first=proc[0],last=proc.at(-1),prior=new Map(first?.rows.map(row=>[row[0],row])||[]);
  episode.host={valid:host.valid,cpu:host.cpuSamples.filter(p=>p.utcMs>=lo-1000&&p.utcMs<=hi+1000),processSamples:proc,knownProcesses:host.knownProcesses,
   processDeltas:!first||first===last?[]:last.rows.filter(row=>prior.has(row[0])).map(row=>({pid:row[0],name:row[1],cpuSeconds:row[2]-prior.get(row[0])[2],intervalMs:last.sourceUtcMs-first.sourceUtcMs})).sort((a,b)=>b.cpuSeconds-a.cpuSeconds)};
 }
}
const analysis={version:1,source:{path:resolve(input),sha256:sha(raw),identity:report.identity,passed:report.passed,parameters:report.parameters},
 analyzer:{path:fileURLToPath(import.meta.url),sha256:sha(readFileSync(fileURLToPath(import.meta.url)))},commonTimeOrigin:base,
 scope:'Recorded API/handler/copy boundaries. FIFO occurrence matching uses all header fields plus length, FNV32 and destination. The hash is not collision-proof; duplicate occurrences remain explicit.',
 limitations:['No OS receive or actual NIC transmit timestamps. WebSocket send entry/return bracket the API call, not physical send.',
  'poll-copy means bytes copied into WASM, not per-packet protocol acceptance. Safe frontier can reflect several dequeues.',
  'An already requested RAF entering late does not identify the unobserved browser/compositor/task source. No antivirus causal claim follows from this record.',
  'Host aggregate CPU does not measure individual-core scheduling/frequency. Process snapshots miss short exited tasks.',
  'Relay trace stops after browser result capture; unmatched tail events outside browser observation are retained.'],
 valid:{browser:peers.map(p=>p.networkTransport.valid),relay:relay.valid},
 clocks:{browser:peers.map(p=>clock(p.networkTransport)),relay:clock(relay)},pairings,
 unmatchedBrowserEvents:allBrowser.filter(e=>Object.values(pairings).some(p=>p.unmatchedLeft.includes(e.ref)||p.unmatchedRight.includes(e.ref))).map(compact),
 failedOrPeekCopies:phase(allBrowser,'poll-copy').filter(e=>e.raw.resultBytes<=0||e.raw.threw).map(compact),
 relayPending:relay.pending,relayCoverage:relay.coverage,relayCounts:relay.counts,displayGaps,allRafPendingOver250ms:delayed,
 allRafCallsOver250ms:peers.map((p,side)=>({side,requests:p.rafDiagnostics.requests.filter(q=>q.enteredAt!==null&&q.exitedAt!==null&&q.exitedAt-q.enteredAt>250)})),
 stageSummary:Object.fromEntries(Object.entries(stages).map(([name,rows])=>[name,{...dist(rows.map(r=>r.ms)),allOver250ms:rows.filter(r=>r.ms>250),allNegative:rows.filter(r=>r.ms<0)}])),
 episode,chains};
writeFileSync(output,JSON.stringify(analysis,null,2),{flag:'wx'});
process.stdout.write(JSON.stringify({output:resolve(output),sha256:sha(readFileSync(output)),bytes:readFileSync(output).length,chains:chains.length,pairings:Object.fromEntries(Object.entries(pairings).map(([k,v])=>[k,{...v,unmatchedLeft:v.unmatchedLeft.length,unmatchedRight:v.unmatchedRight.length}])),
 stages:Object.fromEntries(Object.entries(analysis.stageSummary).map(([k,v])=>[k,{...v,allOver250ms:v.allOver250ms.length,allNegative:v.allNegative.length}])),episode:episode&&{side:episode.side,durationMs:episode.durationMs,missingSourceFrame:episode.missingSourceFrame,coveringChains:episode.coveringChains,peers:episode.peers.map(p=>({side:p.side,rafCalls:p.rafCalls,gameCallbacks:p.gameCallbacks,cppWorkTotalMs:p.cppWorkTotalMs,pumpObservations:p.pacing.filter(a=>a[1]===0).length,network:Object.fromEntries(Object.entries(p.networkByPhase).map(([k,v])=>[k,{all:v.all,nonemptyInput:v.nonemptyInput}])),audioBlocks:p.audio.approximateBlocksInWindow.length}))}},null,2));

