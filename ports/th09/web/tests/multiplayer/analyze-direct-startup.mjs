import fs from 'node:fs';
import path from 'node:path';
import crypto from 'node:crypto';
import {fileURLToPath} from 'node:url';

// Offline reads only; no browser, socket, build or performance experiment.
const [input,output]=process.argv.slice(2);
if(!input||!output)throw Error('Usage: node analyze-direct-startup.mjs REPORT OUTPUT');
const sha=b=>crypto.createHash('sha256').update(b).digest('hex');
const identity=p=>{const b=fs.readFileSync(p);return {path:path.resolve(p),bytes:b.length,sha256:sha(b)};};
const raw=fs.readFileSync(input),r=JSON.parse(raw);
const check=(v,m)=>{if(!v)throw Error(m);};
check(r.passed&&r.errors.length===0&&r.results.length===2,'Report incomplete');
check(r.parameters.release&&!r.parameters.fallback&&r.parameters.expectedRtcPath==='direct'&&r.parameters.limit===900,'Wrong route/frame condition');
check(r.parameters.startupTrace&&r.parameters.rafDiagnostics&&r.parameters.hostLoad,'Missing observations');
check(r.parameters.clientSendMode==='synchronous'&&r.parameters.impairmentSite==='none','Wrong send condition');
const relay=r.relayTransport,rs=relay.startup,ro=rs.clock.started.timeOrigin;
check(rs.valid&&rs.coverage.exercised&&!rs.overflow&&!rs.observerErrors,'Relay startup incomplete');
check(rs.configuration.rtcTimeoutMs===4500,'Timeout changed');
const expected=(f,s)=>f===0?0:(f<132?256:0)|(f%126<90?1:0)|(f%180<45?4:0)|((Math.floor(f/22)+s)%4===0?64:0)|((Math.floor(f/22)+s)%4===2?128:0);
const clock=(s,t)=>r.results[s].startupTransport.clock.started.timeOrigin-ro+t;
const key=(side,kind,e)=>JSON.stringify([side,kind,e.type,e.descriptionType??null,!!e.candidatePresent,e.from??null,e.to??null,e.player??null,e.peers??null,e.mode??null]);
const sends=[],messages=[];
const peers=r.results.map((p,side)=>{
  const st=p.startupTransport,ev=st.events,sockets=new Map(st.sockets.map(s=>[s.id,s]));
  check(st.valid&&Object.values(st.overflow).every(v=>!v)&&!st.errors.length&&Object.values(st.coverage).every(Boolean),'Browser startup incomplete');
  const re=p.routeEvidence;check(p.route==='rtc'&&re.selectedPath==='direct'&&re.peers.length===1,'Final route mismatch');
  const peer=re.peers[0],sel=peer.selected;
  check(!peer.error&&sel.state==='succeeded'&&sel.localType&&sel.remoteType&&sel.localType!=='relay'&&sel.remoteType!=='relay','Direct candidate gate');
  check(peer.input.state==='open'&&!peer.input.ordered&&peer.input.maxRetransmits===0&&peer.control.state==='open'&&peer.control.ordered&&peer.control.maxRetransmits===null,'Channel gate');
  const tape=p.measure.inputTape,pre=tape.slice(0,r.parameters.limit);
  check(pre.length===r.parameters.limit&&pre.every((v,i)=>v===expected(i,side))&&sha(JSON.stringify(pre))===p.inputTapeSha256,'Input tape gate');
  const constructors=ev.filter(e=>e.event==='construct-enter').map(e=>{
    const end=ev.filter(x=>x.target==='pc'&&x.call===e.call&&['construct-returned','construct-threw'].includes(x.event));
    check(end.length===1,'Constructor terminal missing');
    return {entry:e,terminal:end[0],durationMs:end[0].at-e.at,relayClockEntry:clock(side,e.at),relayClockTerminal:clock(side,end[0].at)};
  });
  check(constructors.length===1&&constructors[0].terminal.event==='construct-returned','Constructor coverage');
  const methods=ev.filter(e=>e.target==='pc'&&e.event==='call').map(e=>{
    const terminals=ev.filter(x=>x.target==='pc'&&x.call===e.call&&['returned','threw','resolved','rejected'].includes(x.event));
    return {entry:e,terminals,durationMs:terminals.length===1?terminals[0].at-e.at:null};
  });
  for(const e of ev.filter(e=>e.target==='ws'&&['send-enter','message'].includes(e.event))){
    const s=sockets.get(e.entity);check(s,'Unknown socket');
    const item={side,event:e,socket:s,key:key(side,s.kind,e),relayClockAt:clock(side,e.at)};
    if(e.event==='send-enter'){item.terminals=ev.filter(x=>x.target==='ws'&&x.event==='send-exit'&&x.call===e.call);sends.push(item);}else messages.push(item);
  }
  const rd=p.rafDiagnostics,s=p.measure.samples,fm=new Map(),qm=new Map(rd.requests.map(q=>[q.index,q]));
  check(rd.valid&&Object.values(rd.overflow).every(v=>!v),'RAF trace incomplete');
  for(const f of rd.frames)if(f.sampleAfter===f.sampleBefore+1)fm.set(f.sampleBefore,f);
  let prior=null;const gaps=[];
  for(let i=0;i<s.length;i++){
    if(prior!==null&&(s[i][11]===s[prior][11]||s[i][3]<=s[prior][3]))continue;
    if(prior!==null){const a=s[prior],b=s[i],fa=fm.get(prior),fb=fm.get(i),qa=fa&&qm.get(fa.rafIndex),qb=fb&&qm.get(fb.rafIndex);
      gaps.push({indices:[prior,i],frames:[a[3],b[3]],rafTimestampMs:b[13]-a[13],postPlaySampleMs:b[0]-a[0],onGameFrameHookEntryMs:fa&&fb?fb.enteredAt-fa.enteredAt:null,actualRafEntryMs:qa&&qb?qb.enteredAt-qa.enteredAt:null,samples:[a,b],requests:[qa?.index??null,qb?.index??null]});}
    prior=i;
  }
  const maxGap=k=>gaps.reduce((a,b)=>!a||b[k]>a[k]?b:a,null);
  return {side,player:'P'+(side+1),origin:st.clock.started.timeOrigin,originOffsetFromRelayMs:st.clock.started.timeOrigin-ro,
    finalRoute:re,constructor:constructors[0],methods,unsettledMethods:methods.filter(m=>m.terminals.length!==1),
    marks:ev.filter(e=>e.target==='harness').map(e=>({event:e,relayClockAt:clock(side,e.at)})),
    startup:st,input:{exactExpected:true,firstFrames:pre.length,sha256:p.inputTapeSha256,totalCaptured:tape.length,tailBeyondLimit:tape.slice(r.parameters.limit)},
    confirmedHashes:p.confirmedHashes,originalFormalSummary:r.summary[side],
    adverse:{largestCallback:s.map((v,i)=>({index:i,sample:v})).reduce((a,b)=>!a||b.sample[2]>a.sample[2]?b:a,null),
      callbacksOver50ms:s.map((v,i)=>({index:i,sample:v,frame:fm.get(i)??null})).filter(x=>x.sample[2]>50),
      allPictureGapsOver50msByAnyClock:gaps.filter(g=>[g.rafTimestampMs,g.postPlaySampleMs,g.onGameFrameHookEntryMs,g.actualRafEntryMs].some(x=>x>50)),
      largestRawRafPictureGap:maxGap('rafTimestampMs'),largestRawPostPlayPictureGap:maxGap('postPlaySampleMs'),
      allRafRequestWaitsOver250ms:rd.requests.filter(q=>q.enteredAt!==null&&q.enteredAt-q.requestedAt>250).map(q=>({request:q,parent:qm.get(q.parentIndex)??null,pendingMs:q.enteredAt-q.requestedAt,frame:q.firstFrameIndex===null?null:rd.frames[q.firstFrameIndex]}))},
    ending:{stats:p.stats,hash:p.hash,measureStarted:p.measure.started,measureEnded:p.measure.ended,lastSamples:s.slice(-2),
      openRafRequests:rd.requests.filter(q=>q.enteredAt===null).map(q=>({request:q,pendingToSnapshotMs:rd.snapshotAt-q.requestedAt})),
      postMeasurementHookCount:rd.frames.filter(f=>f.endedBefore===true).length},
    lifecycle:p.lifecycle,audioOutput:p.audioOutput};
});
check(r.confirmedChecks===Math.floor((r.parameters.limit-1)/120),'Confirmation count');
check(JSON.stringify(peers[0].confirmedHashes)===JSON.stringify(peers[1].confirmedHashes),'Confirmed hash mismatch');
for(let i=0;i<r.confirmedChecks;i++)check(peers[0].confirmedHashes[i][0]===(i+1)*120,'Confirmation interval');
// Preserve FIFO occurrences; these safe metadata keys are not payload fingerprints.
function match(left,right){
  const queues=new Map();right.forEach((x,i)=>{if(!queues.has(x.key))queues.set(x.key,[]);queues.get(x.key).push(i);});
  const matched=[],unmatchedLeft=[];
  for(const a of left){const q=queues.get(a.key);if(!q?.length){unmatchedLeft.push(a);continue;}const b=right[q.shift()];matched.push({left:a,right:b,observedDeltaMs:b.relayClockAt-a.relayClockAt});}
  const count=a=>{const m=new Map();for(const x of a)m.set(x.key,(m.get(x.key)||0)+1);return [...m].filter(([,n])=>n>1);};
  return {matched,unmatchedLeft,unmatchedRight:[...queues.values()].flat().sort((a,b)=>a-b).map(i=>right[i]),duplicateMetadataKeys:{left:count(left),right:count(right)}};
}
const receives=rs.events.filter(e=>e.phase==='receive').map(e=>({event:e,key:key(e.socketPlayer,e.kind,e),relayClockAt:e.enteredAt}));
const relaysends=rs.events.filter(e=>e.phase==='send').map(e=>({event:e,key:key(e.socketPlayer,e.kind,e),relayClockAt:e.enteredAt}));
const clientToRelay=match(sends,receives),relayToClient=match(relaysends,messages);
const players=rs.events.filter(e=>e.phase==='connection'&&e.kind==='player'),signals=rs.events.filter(e=>e.phase==='connection'&&e.kind==='signal'),routes=rs.events.filter(e=>e.phase==='send'&&e.type==='route');
check(players.length===2&&signals.length===2&&routes.length===4&&routes.every(e=>e.mode==='rtc'),'Unexpected route trace');
const firstRoute=Math.min(...routes.map(e=>e.enteredAt)),lastPlayer=Math.max(...players.map(e=>e.enteredAt)),lastSignal=Math.max(...signals.map(e=>e.enteredAt));
const host=r.hostLoad,busy=host.cpuSamples.filter(s=>s.busyFraction!==null).map(s=>s.busyFraction*100).sort((a,b)=>a-b);
const processNames=[...new Set(host.processSamples.flatMap(s=>s.rows.map(row=>row[1])))].sort();
const sources=[
  ['web/tests/multiplayer/smoothness-browser.mjs','harness',['28,43-55: RTC/relay condition gates','335-345: startup observer before connect, CPU throttle after openProbe','448-507: final RTC/candidate/channel, tape and confirmation gates','523-530: original formal summary clock definitions']],
  ['web/tests/multiplayer/startup-transport-probe.mjs','startupTransportProbe',['126-129: same-call constructor entry and return/throw']],
  ['web/tests/multiplayer/relay-startup-probe.mjs','relayStartupProbe',['Connection observation precedes canonical admission; exact timeout firing unobserved']],
  ['web/artifacts/rollback4x-responsive-relay-fixture/server/netplay-relay.mjs','relay',['63: default 4500 ms','305,328-347: route latch, timeout and all-peer readiness']]
].map(([name,k,anchors])=>{const id=identity(name);check(id.sha256===r.identity[k],'Frozen source changed: '+name);return {...id,anchors};});
const result={version:1,complete:true,scope:'Offline reconstruction of one actual headed release RTC run. No new browser, network, build or timing experiment.',
  raw:{path:path.resolve(input),bytes:raw.length,sha256:sha(raw)},analyzer:identity(fileURLToPath(import.meta.url)),sources,
  originalPassed:r.passed,originalErrors:r.errors,identity:r.identity,parameters:r.parameters,originalScope:r.scope,originalPresentationMeasurement:r.presentationMeasurement,
  verdict:{directFinalRoute:true,wireTapeAndConfirmedHashes:true,
    formalObservation:{logicHz:r.summary.map(s=>s.logicHz),newStateGapMaxMs:r.summary.map(s=>s.effectivePresentationGapMs.max)},
    startupStillHasLongWaits:peers.some(p=>p.constructor.durationMs>250),
    statements:['双方最终候选均为 succeeded 的 host-to-host UDP；输入通道保留 unordered/maxRetransmits=0，控制通道保留 ordered/可靠配置。',
      '正式 frame 180..899 接近 60 Hz，不能覆盖启动段的连接构造等待与首帧重活。',
      '这是本机两浏览器进程的真实 RTC 运行，不能外推公网 NAT、TURN 或深度回滚；结束时的 ICE 快照也不是全程候选跟踪。',
      '本轮不能单独归因输入合并的 CPU 收益，也不抹去此前失败的 headless/direct 或有停顿的 relay 运行。']},
  peers,
  signaling:{matchingScope:'FIFO per side/socket-kind/type/description-type/candidate-present/from/to/player/peers/mode; no SDP or candidate payload fingerprint. Every unmatched observation is retained.',
    clientToRelay,relayToClient,relaySnapshot:rs,
    unmatchedInterpretation:relayToClient.unmatchedLeft.length===2&&relayToClient.unmatchedLeft.every(x=>x.event.kind==='player'&&x.event.type==='route'&&x.event.mode==='rtc')?
      'Both unmatched relay sends are route=rtc on player sockets. Signaling-socket route observations and player closes are retained separately; the missing messages remain unmatched, not relabeled as received.':
      'All unmatched observations remain unresolved; no absence is relabeled as delivery.',
    timeout:{configuredMs:rs.configuration.rtcTimeoutMs,sourceConfiguration:relay.sourceIdentity.rtcTimeout,lastObservedPlayerConnectionAt:lastPlayer,lastObservedSignalConnectionAt:lastSignal,firstRouteSendAt:firstRoute,
      observedPlayerConnectionToRouteMs:firstRoute-lastPlayer,observedSignalConnectionToRouteMs:firstRoute-lastSignal,marginToConfiguredTimeoutFromPlayerObservationMs:4500-(firstRoute-lastPlayer),
      limitation:'Connection hook runs before canonical admission. These are observed intervals, not an instrumented timer start/fire. Relay selected RTC after both ready receipts; no changed timeout or forced fallback.'},
    inputRelayTrace:{valid:relay.valid,coverage:relay.coverage,counts:relay.counts,overflow:relay.overflow,errors:relay.errors,sourceIdentity:relay.sourceIdentity,
      scope:'Relay startup/signaling observed. Zero E9NP relay input events are expected here. NETWORK_TRACE=0: browser RTC input receive/poll timings are unavailable.'}},
  ending:{sameFrame:r.sameFrame,finalComparable:r.finalComparable,confirmedChecks:r.confirmedChecks,
    limitation:'Final peer frames differ. Compare seven matching confirmed checkpoints and exact first 900 local input values. Retain extra captures, pending RAFs, and nonzero channel bufferedAmount; do not compare final hashes or claim all data drained.'},
  host:{snapshot:host,summary:{sampleCount:busy.length,quantile:'sorted[floor(n*p)]',busyPercent:{min:busy[0],p50:busy[Math.floor(busy.length*.5)],p95:busy[Math.floor(busy.length*.95)],max:busy.at(-1)},candidateProcessNames:processNames},
    interpretation:'No samples filtered or adjusted. valid/complete means observation completeness, not a quiet host. First-seen process CPU is lifetime; 5 s snapshots miss short compilers. Aggregate occupancy does not establish single-core/GPU/frequency/thermal stability.'},
  headed:{identityVerified:r.headedControl.identityVerified,windows:r.headedControl.windows.map(w=>({side:w.side,stage:w.stage,page:w.page})),scope:r.headedControl.scope},
  clockDefinitions:['Within-process durations subtract monotonic timestamps. Cross-process events use recorded performance.timeOrigin offsets; original clock observations are retained.',
    'RAF timestamp picture gaps preserve the original harness definition and are not physical scanout. postPlaySampleMs is not a complete callback-exit gap.',
    'onGameFrameHookEntryMs measures the JS observation hook after C++ main-loop work. actualRafEntryMs uses the RAF wrapper entry. These different distributions are not additive.',
    'All sample picture gaps over 50 ms by any recorded clock, native callbacks over 50 ms, and all RAF request waits over 250 ms are retained, including startup and stopped-window tails.'],
  limitations:['Only P2 uses CPU4 in this local live run. Sampled SDL audio output is retained; browser mute remains on, and this is not acoustic acceptance. First audio-block relative time has no independent performance origin.',
    '900 deterministic input frames and seven confirmed hashes provide limited live correction coverage. This is not the separate depth-8, long-session, results or round-reset gate.',
    'No response-intent probe or RTC packet-level receive/dequeue trace is enabled. Capture-to-display latency and per-packet RTC timing cannot be inferred.',
    'Headed windows and setup/CIM observations change environmental conditions. Visible/focus samples do not prove full-session physical window coverage or scanout.']};
fs.writeFileSync(output,JSON.stringify(result,null,2)+'\n');
console.log(JSON.stringify({output:identity(output),constructorMs:peers.map(p=>p.constructor.durationMs),playerConnectionToRouteMs:firstRoute-lastPlayer,
  signalingMatches:[clientToRelay.matched.length,relayToClient.matched.length],unmatched:[clientToRelay.unmatchedLeft.length,clientToRelay.unmatchedRight.length,relayToClient.unmatchedLeft.length,relayToClient.unmatchedRight.length],hostProcessNames:processNames}));
