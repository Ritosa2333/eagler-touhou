// Offline startup causality evidence. Reads existing JSON only; never runs a browser.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';

const [input,output]=process.argv.slice(2);
assert.ok(input&&output,'Usage: node analyze-startup-transport.mjs input.json new-output.json');
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const raw=readFileSync(input),r=JSON.parse(raw),xs=r.results||r.partial;
assert.equal(r.parameters.startupTrace,true);
assert.equal(xs.length,2);
const relay=r.relayTransport,rt=relay.startup,origin=rt.clock.started.timeOrigin;
assert.equal(relay.valid,true);assert.equal(rt.valid,true);
const route=rt.events.find(e=>e.phase==='send'&&e.type==='route');
const relayAt=route?.enteredAt??Infinity;
const conns=relay.connections.filter(c=>['player','signal'].includes(c.kind));
function completeAt(kind){const cs=[0,1].map(side=>conns.find(c=>c.kind===kind&&c.player===side));return cs.every(Boolean)?Math.max(...cs.map(c=>c.openedAt)):null;}
const playersAt=completeAt('player'),signalsAt=completeAt('signal');
const beforeRoute=rt.events.filter(e=>e.enteredAt<relayAt);
const rawSignalKey=e=>JSON.stringify([e.type,e.descriptionType,e.candidatePresent,e.from,e.to,e.player,e.peers,e.mode,e.errorName]);
const peerEvidence=xs.map((x,side)=>{
 const t=x.startupTransport,a=x.rafDiagnostics;
 assert.equal(t.valid,true);assert.equal(a.valid,true);
 const offset=t.clock.started.timeOrigin-origin;
 const invite=t.events.find(e=>e.event==='message'&&(e.type==='peer-join'||e.type==='peers'&&e.peers?.length));
 const created=t.events.find(e=>e.target==='pc'&&e.event==='created');
 const lo=invite?.at??null,hi=created?.at??null,window=lo!=null&&hi!=null?{start:lo,end:hi,durationMs:hi-lo,relayStart:lo+offset,relayEnd:hi+offset}:null;
 const startupRequests=a.requests.filter(q=>q.enteredAt!=null&&q.enteredAt<x.measure.started);
 const longRequests=startupRequests.filter(q=>q.enteredAt-q.requestedAt>250||q.exitedAt-q.enteredAt>250)
  .map(q=>({request:q,previous:q.parentIndex==null?null:a.requests.find(p=>p.index===q.parentIndex)??null}));
 const constructorCalls=t.events.filter(e=>e.event==='construct-enter').map(e=>{
  const end=t.events.find(v=>v.call===e.call&&['construct-returned','construct-threw'].includes(v.event));
  return {entry:e,end:end??null,durationMs:end?end.at-e.at:null};
 });
 const methods=t.events.filter(e=>e.target==='pc'&&e.event==='call').map(e=>{
  const end=t.events.find(v=>v.call===e.call&&v.method===e.method&&['resolved','rejected','returned','throw'].includes(v.event));
  return {entry:e,end:end??null,observedDurationMs:end?end.at-e.at:null};
 });
 return {side,stats:x.stats,route:x.route,routeEvidence:x.routeEvidence,inputTapeLength:x.measure?.inputTape?.length??null,
  observerIdentity:r.identity.startupTransportProbe,trace:t,offsetToRelayMs:offset,firstActiveAt:x.measure.started,
  inviteToCreated:window,constructorCalls,methods,
  withinWindow:window?{rafEntries:a.requests.filter(q=>q.enteredAt>=lo&&q.enteredAt<hi).length,
   gameFrameEntries:a.frames.filter(q=>q.enteredAt>=lo&&q.enteredAt<hi).length,
   startupEvents:t.events.filter(e=>e.at>lo&&e.at<hi)}:null,
  lifecycle:x.lifecycle,controls:a.controls,allStartupRafIntervalsOver250Ms:longRequests,
  liveMarks:x.liveCallbackCosts?.marks.map(m=>({label:m.label,observedAt:m.observedAt})),
  firstLiveFrame:x.liveCallbackCosts?.frames?.[0]??null,
  interpretation:{allJsStoppedEstablished:false,constructorDurationAvailable:constructorCalls.length>0,
   beforeActiveAudioCoverage:false,perPumpTimestampCoverage:r.parameters.networkTrace,
   nativeGameplayBeginsAfterRoute:'SharedNetplay.pump starts native begin only after state===1 and !routeReady; a paused startup RAF is not a native gameplay callback.'}};
});
const incoming=[];
for(const p of peerEvidence){
 const sockets=new Map(p.trace.sockets.map(s=>[s.id,s]));
 for(const e of p.trace.events.filter(e=>e.event==='message'&&e.target==='ws')){
  const s=sockets.get(e.entity);incoming.push({side:p.side,kind:s?.kind??null,event:e,relayAt:e.at+p.offsetToRelayMs,key:rawSignalKey(e),matched:false});
 }
}
const links=rt.events.filter(e=>e.phase==='send').map(e=>{
 const m=incoming.find(m=>!m.matched&&m.side===e.socketPlayer&&m.kind===e.kind&&m.key===rawSignalKey(e));
 if(m)m.matched=true;
 return {relay:e,browser:m?{side:m.side,event:m.event,relayAt:m.relayAt}:null,
  relaySendToBrowserMessageMs:m?m.relayAt-e.enteredAt:null,
  unmatchedReason:m?null:'No browser message observation for this socket and redacted metadata occurrence. This is not proof of packet loss.'};
});
const windows=peerEvidence.map(p=>p.inviteToCreated).filter(Boolean);
const begin=Math.min(...windows.map(w=>w.relayStart)),end=Math.max(...windows.map(w=>w.relayEnd));
const utcBegin=origin+begin,utcEnd=origin+end,h=r.hostLoad;
const cpu=h.cpuSamples.filter(s=>s.utcMs>=utcBegin-1100&&s.utcMs<=utcEnd+1100);
const ps=h.processSamples.filter(s=>s.sourceUtcMs>=utcBegin-6000&&s.sourceUtcMs<=utcEnd+6000);
const first=ps.filter(s=>s.sourceUtcMs<=utcBegin).at(-1),last=ps.find(s=>s.sourceUtcMs>=utcEnd),known=new Map(h.knownProcesses.map(p=>[p.pid,p.role]));
const deltas=[];
if(first&&last){const before=new Map(first.rows.map(row=>[row[0],row]));for(const row of last.rows){const old=before.get(row[0]);if(old&&row[2]>old[2])deltas.push({pid:row[0],name:row[1],role:known.get(row[0])??'other-process-candidate',cpuSeconds:row[2]-old[2]});}}
const independentTicks=relay.timing.filter(row=>row[0]>=begin&&row[0]<=end);
const out={version:1,input:{path:resolve(input),bytes:raw.length,sha256:sha(raw)},analyzer:{path:resolve(fileURLToPath(import.meta.url)),sha256:sha(readFileSync(fileURLToPath(import.meta.url)))},
 identity:r.identity,parameters:r.parameters,originalError:r.error,originalPageErrors:r.errors,
 clock:{reference:'Relay performance.now in milliseconds',timeOrigin:origin,formula:'browser local performance.now + browser timeOrigin - relay timeOrigin; host UTC compared with relay timeOrigin + local time'},
 decision:{firstRoute:route??null,rtcTimeoutMs:rt.configuration.rtcTimeoutMs,lastObservedPlayerConnectionAt:playersAt,lastObservedSignalConnectionAt:signalsAt,
  routeAfterObservedAllPlayersMs:playersAt==null?null:relayAt-playersAt,routeAfterObservedAllSignalsMs:signalsAt==null?null:relayAt-signalsAt,
  explicitRtcFailedBeforeRoute:beforeRoute.filter(e=>e.phase==='receive'&&e.type==='rtc-failed'),
  rtcReadyBeforeRoute:beforeRoute.filter(e=>e.phase==='receive'&&e.type==='rtc-ready'),
  signalCloseBeforeRoute:beforeRoute.filter(e=>e.phase==='close'&&e.kind==='signal'),
  exactTimerFireObserved:rt.coverage.exactTimerFire,classification:route?.mode==='relay'&&beforeRoute.every(e=>e.type!=='rtc-failed'&&!(e.phase==='close'&&e.kind==='signal'))?'deadline-consistent inference; no explicit failure/close observed':'inspect retained explicit events'},
 relay:{sourceIdentity:relay.sourceIdentity,connections:relay.connections,startup:rt,clockTicksDuringPeerInitialization:independentTicks,
  maximumClockGapDuringWindowMs:Math.max(...independentTicks.map(row=>row[3]))},peers:peerEvidence,
 textDelivery:{matching:'FIFO occurrences by destination player, socket kind and complete retained redacted metadata; no payload fingerprint for private signaling.',links,unmatchedBrowserMessages:incoming.filter(m=>!m.matched)},
 hostWindow:{valid:h.valid,utcBounds:[utcBegin,utcEnd],cpuSamples:cpu,processSamples:ps,knownProcesses:h.knownProcesses,limits:h.limits,errors:h.errors,
  processDeltaBounds:first&&last?[first.sourceUtcMs,last.sourceUtcMs]:null,processCpuDeltas:deltas,limitations:h.limitations},
 limitations:[
  'Constructor entry was absent from the original v1 observer. Peer-message to PC-created includes original event handling and native/proxied constructor; it cannot alone isolate constructor CPU time.',
  'No RAF/game-frame/startup event inside this window is observed. There is no independent per-pump or enabled audio trace before active, so cessation of all JavaScript is not established.',
  'Relay connection observation precedes canonical admission and no route timer callback was wrapped. Deadline attribution remains an inference from complete signaling history, configuration and source.',
  'Closed signaling sockets can leave relay route sends without browser message observations; retained unmatched entries are not automatically drops.',
  'Both route and tape gates remain authoritative. A fallback run with an empty requested RTC input tape is not a direct-performance result.',
  'Host aggregate CPU and periodic candidate process samples neither locate all contention nor identify injected browser code.'
 ]};
writeFileSync(output,JSON.stringify(out,null,2)+'\n',{flag:'wx'});
const bytes=readFileSync(output);console.log(JSON.stringify({output:resolve(output),bytes:bytes.length,sha256:sha(bytes),rawSha256:sha(raw),decision:out.decision,peerWindows:peerEvidence.map(p=>({side:p.side,window:p.inviteToCreated,within:p.withinWindow,constructors:p.constructorCalls})),hostProcessDeltas:deltas}));
