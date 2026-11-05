// Offline evidence only: never launches a browser, build, relay or timer.
import fs from 'node:fs';
import path from 'node:path';
import {createHash} from 'node:crypto';
import {fileURLToPath} from 'node:url';
import assert from 'node:assert/strict';
const [input,output]=process.argv.slice(2);
assert(input&&output,'Usage: node analyze-raf-timeout.mjs input-failure.json NEW-output.json');
assert(path.resolve(input)!==path.resolve(output),'Never overwrite the raw report');
const sha=b=>createHash('sha256').update(b).digest('hex');
const bytes=fs.readFileSync(input),raw=JSON.parse(bytes);
const own=fs.readFileSync(fileURLToPath(import.meta.url));
const finite=Number.isFinite;
const decode=(columns,row)=>Object.fromEntries(columns.map((key,index)=>[key,row[index]]));
function dist(values){
 const a=values.filter(finite).toSorted((a,b)=>a-b),pick=q=>a.length?a[Math.max(0,Math.ceil(q*a.length)-1)]:null;
 return {count:a.length,min:a[0]??null,p50:pick(.5),p95:pick(.95),p99:pick(.99),max:a.at(-1)??null};
}
function peerEvidence(peer,side){
 const m=peer.measure,d=peer.rafDiagnostics,a=peer.audioOutput;
 assert(d?.valid&&!d.overflow,'RAF diagnostic coverage must be intact');
 const requests=new Map(d.requests.map(r=>[r.index,r]));
 const sample=i=>i==null?null:{index:i,raw:m.samples[i],completedAt:m.started+m.samples[i][0]};
 const frame=f=>f?{...f,sample:sample(f.sampleAfter-1)}:null;
 const framesFor=r=>d.frames.filter(f=>f.rafIndex===r.index).map(frame);
 const decorate=r=>({...r,dispatchDelayMs:finite(r.enteredAt)?r.enteredAt-r.requestedAt:null,
   callbackMs:finite(r.exitedAt)&&finite(r.enteredAt)?r.exitedAt-r.enteredAt:null,frames:framesFor(r)});
 const pending=d.pendingIndices.map(index=>{
   const r=requests.get(index);assert(r&&!finite(r.enteredAt),'Pending request must not have entered');
   const parent=requests.get(r.parentIndex);
   return {request:decorate(r),parent:parent?decorate(parent):null,snapshotAgeMs:d.snapshotAt-r.requestedAt,
     controlsAfterRequest:d.controls.filter(c=>c.enteredAt>=r.requestedAt)};
 });
 const entered=d.requests.filter(r=>finite(r.enteredAt)),enteredGaps=[];
 for(let i=1;i<entered.length;i++){
   const before=entered[i-1],after=entered[i],ms=after.enteredAt-before.enteredAt;
   if(ms>250)enteredGaps.push({ms,before:decorate(before),after:decorate(after)});
 }
 const sampleGaps=[];
 for(let i=1;i<m.samples.length;i++){
   const ms=m.samples[i][0]-m.samples[i-1][0];if(ms>250)sampleGaps.push({ms,before:sample(i-1),after:sample(i)});
 }
 const newFrames=m.samples.map((s,index)=>({s,index})).filter((x,i,all)=>i===0||x.s[11]!==all[i-1].s[11]);
 const presentGaps=[];
 for(let i=1;i<newFrames.length;i++){
   const before=newFrames[i-1],after=newFrames[i],ms=after.s[13]-before.s[13];
   if(ms>250)presentGaps.push({ms,before:sample(before.index),after:sample(after.index)});
 }
 const audioGaps=a.blocks.slice(1).map((b,i)=>b[0]-a.blocks[i][0]);
 return {side,finalStats:peer.stats,started:m.started,ended:m.ended,finished:m.finished,limit:m.limit,
   sampleCount:m.samples.length,inputTape:m.inputTape,notices:m.notices,closed:m.closed,loads:m.loads,
   lifecycle:peer.lifecycle,raf:{version:d.version,valid:d.valid,counts:d.counts,snapshotAt:d.snapshotAt,
     overflow:d.overflow,hookErrors:d.hookErrors,rafHookCurrent:d.rafHookCurrent,cancelHookCurrent:d.cancelHookCurrent,
     frameHookCurrent:d.frameHookCurrent,callbacks:d.callbacks,controls:d.controls,pending,
     allEnteredGapsOver250Ms:enteredGaps,
     allDispatchDelaysOver250Ms:d.requests.filter(r=>finite(r.enteredAt)&&r.enteredAt-r.requestedAt>250).map(decorate),
     allCallbackDurationsOver250Ms:d.requests.filter(r=>finite(r.exitedAt)&&r.exitedAt-r.enteredAt>250).map(decorate),
     firstFrames:d.frames.slice(0,20).map(frame),lastFrames:d.frames.slice(-5).map(frame)},
   samples:{columns:'Original 14 fields unchanged; time of completion=measure.started+row[0]. row[13] is prior rAF timestamp, never actual wrapper entry.',
     cppWorkMs:dist(m.samples.map(s=>s[2])),allCompletionGapsOver250Ms:sampleGaps,
     allPresentTimestampGapsOver250Ms:presentGaps},
   audio:{...a,blockCount:a.blocks.length,relativeSpanMs:a.blocks.at(-1)?.[0]??0,gapMs:dist(audioGaps),
     timingLimit:'Block wall times are relative to first audio callback. No absolute page timeOrigin was retained; do not invent exact per-block alignment.'}};
}
function relayEvidence(t){
 assert(t?.valid,'Relay diagnostic must be valid');
 const packets=new Map(t.packets.map(row=>{const p=decode(t.columns.packet,row);return [p.id,p];}));
 const events=t.events.map(row=>decode(t.columns.event,row));
 const receives=events.filter(e=>t.columns.phases[e.phase]==='receive');
 const linked=e=>({event:e,packet:packets.get(e.packetId),
   forwards:events.filter(x=>x.receiveEventId===e.id).map(x=>({...x,phaseName:t.columns.phases[x.phase]}))});
 const senders=[0,1].map(sender=>{
   const es=receives.filter(e=>packets.get(e.packetId).sender===sender);
   const changes=[];let lastAck;
   for(const e of es){const p=packets.get(e.packetId);if(p.ackFrame!==lastAck){changes.push(linked(e));lastAck=p.ackFrame;}}
   return {sender,receiveCount:es.length,first:es[0]?linked(es[0]):null,last:es.length?linked(es.at(-1)):null,
     sequenceRange:[packets.get(es[0]?.packetId)?.sequence??null,packets.get(es.at(-1)?.packetId)?.sequence??null],
     everyAckFrameTransition:changes};
 });
 return {valid:t.valid,clock:t.clock,sourceIdentity:t.sourceIdentity,coverage:t.coverage,identity:t.identity,
   injection:t.injection,counts:t.counts,overflow:t.overflow,errors:t.errors,columns:t.columns,pending:t.pending,
   connectionCount:t.connections.length,senders,timing:t.timing,
   timingLimit:'Relay clock has epoch alignment, but this failure has no browser timeOrigin field. Do not align page and relay timestamps by assuming a common origin. Fresh sequences and ACK advancement are independent connection-liveness observations.'};
}
const result={schema:'th09/raf-timeout-evidence/1',input:{path:path.resolve(input),bytes:bytes.length,sha256:sha(bytes)},
 analyzer:{path:fileURLToPath(import.meta.url),sha256:sha(own)},
 parameterIdentity:raw.parameters,sourceIdentity:raw.identity,
 outcome:{passed:false,error:raw.error,errors:raw.errors},
 definitions:{thresholdMs:250,distribution:'Nearest-rank quantiles; no samples removed by host load or duration.',
   pending:'A request with null enteredAt at final snapshot is right-censored; snapshot age is a measured lower bound, not a completed callback duration.',
   longGapCoverage:'Every completed wrapper-entry gap, request-dispatch delay, wrapper duration, sample-completion gap and Present timestamp gap above 250ms is retained, plus all pending requests and parents.',
   scope:'This analysis uses this raw only. No network-pump/live-cost/browser-network trace from other runs is substituted.'},
 peers:raw.partial.map(peerEvidence),relay:relayEvidence(raw.relayTransport),hostLoad:raw.hostLoad,
 limitations:[
   'P1 short prior C++ and wrapper do not rule out asynchronous GPU/compositor/driver work after submission; no GPU trace was collected.',
   'RAF pending is directly observed. This report cannot identify why Chromium did not dispatch it.',
   'Audio callback continuation does not establish non-silent playback or continuous game advancement.',
   'This raw retains active wasm-opt CPU growth. Aggregate CPU is not single-core saturation/frequency evidence, and concurrent compilation is not proven to cause the RAF stall.',
   'ConfirmedTimeout is a resulting observed failure, not a completed performance score. Keep this failure in evidence.'
 ]};
fs.writeFileSync(output,JSON.stringify(result,null,2)+'\n',{flag:'wx'});
console.log(JSON.stringify({path:path.resolve(output),sha256:sha(fs.readFileSync(output)),bytes:fs.statSync(output).size,
 rawSha256:result.input.sha256,analyzerSha256:result.analyzer.sha256,
 peers:result.peers.map(p=>({side:p.side,samples:p.sampleCount,pending:p.raf.pending.map(x=>({id:x.request.index,ageMs:x.snapshotAgeMs})),
   audioBlocks:p.audio.blockCount,audioSpanMs:p.audio.relativeSpanMs})),hostValid:result.hostLoad.valid}));

