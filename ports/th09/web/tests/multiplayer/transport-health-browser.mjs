#!/usr/bin/env node
// No-WASM transport control. Run separately from live game/performance tests.
// The exact shared EM_JS browser transport bodies are extracted from an explicit
// EAGLER_COMMON_ROOT; the real shared relay is launched in an owned child process.
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import http from 'node:http';
import net from 'node:net';
import {execFileSync} from 'node:child_process';
import {createHash,randomBytes} from 'node:crypto';
import {fileURLToPath,pathToFileURL} from 'node:url';
import {setTimeout as delay} from 'node:timers/promises';
import {healthSchedule,healthPacket,parseHealthPacket,assertHealthPacket,createHealthConstructorTrace,healthPeerConnectionObserver} from './transport-health-fixture.mjs';

const here=path.dirname(fileURLToPath(import.meta.url));
const project=path.resolve(here,'../../..');
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const baselineCommon='43add4dc9085c65468f7ab15428c00206cbe3834';
const requiredBodies=['eagler_peer_connect','eagler_peer_close','eagler_peer_is_open',
  'eagler_peer_send','eagler_peer_send_to','eagler_peer_poll_copy'];
const packetNames=['id','sessionLo','sessionHi','sender','players','sequence','ackSequence',
  'latestFrame','ackFrame','firstInputFrame','senderFrame','frameAdvantage','inputCount','reserved',
  'payloadBytes','fnv1a32','destination'];

function extractBodies(source){
  const result={};
  for(const name of requiredBodies){
    const pattern=new RegExp('^EM_JS\\([^,]+,\\s*'+name+',\\s*\\(([^)]*)\\),\\s*\\{','gm');
    const match=pattern.exec(source);
    assert(match,'Missing exact shared EM_JS declaration: '+name);
    assert.equal(pattern.exec(source),null,'Duplicate shared EM_JS declaration: '+name);
    const begin=match.index+match[0].length,end=source.indexOf('\n});',begin);
    assert(end>=begin,'Missing top-level EM_JS end: '+name);
    const next=source.indexOf('\nEM_JS(',begin);
    assert(next<0||end<next,'EM_JS extraction crossed another function: '+name);
    const args=match[1].trim()?match[1].split(',').map(value=>{
      const argument=value.trim().match(/([A-Za-z_]\w*)$/);
      assert(argument,'Unsupported C ABI argument: '+value);return argument[1];
    }):[];
    const body=source.slice(begin,end);
    new Function(...args,body); // Parse only; never invokes a network API in Node.
    result[name]={args,body,sha256:sha(body)};
  }
  return result;
}
function distribution(values){
  const sorted=values.filter(Number.isFinite).toSorted((a,b)=>a-b);
  const quantile=q=>sorted.length?sorted[Math.min(sorted.length-1,Math.floor(q*(sorted.length-1)))]:null;
  return {count:sorted.length,total:sorted.reduce((sum,value)=>sum+value,0),min:sorted[0]??null,
    p50:quantile(.5),p95:quantile(.95),p99:quantile(.99),max:sorted.at(-1)??null,
    negative:sorted.filter(value=>value<0).length,over100:sorted.filter(value=>value>100).length,
    over250:sorted.filter(value=>value>250).length,over1000:sorted.filter(value=>value>1000).length};
}
// Kept separate from packet linking so self-tests exercise repeated occurrences.
function matchEvents(before,after){
  const queues=new Map(),matched=[],unmatchedAfter=[];
  for(const row of before){if(!queues.has(row.key))queues.set(row.key,[]);queues.get(row.key).push(row);}
  for(const row of after){
    const queue=queues.get(row.key);
    if(!queue?.length){unmatchedAfter.push(row);continue;}
    const previous=queue.shift();matched.push({before:previous,after:row,ms:row.at-previous.at});
  }
  return {matched,unmatchedBefore:[...queues.values()].flat(),unmatchedAfter};
}
function compactEvent(row,stopEpoch){
  return {id:row.id,packetId:row.packetId,key:row.key,sequence:row.sequence,lane:row.lane,
    side:row.side,at:row.at,ageAtStopMs:Number.isFinite(stopEpoch)?stopEpoch-row.at:null};
}
function stage(before,after,stopEpoch){
  const linked=matchEvents(before,after);
  return {completedMs:distribution(linked.matched.map(row=>row.ms)),
    longest:linked.matched.toSorted((a,b)=>b.ms-a.ms).slice(0,5).map(row=>({
      ms:row.ms,before:compactEvent(row.before,stopEpoch),after:compactEvent(row.after,stopEpoch)})),
    unmatchedBefore:linked.unmatchedBefore.map(row=>compactEvent(row,stopEpoch)),
    unmatchedAfter:linked.unmatchedAfter.map(row=>compactEvent(row,stopEpoch)),
    interpretation:'Completed links are conditional on observation; unmatched occurrences and their ages are retained. Cross-process clocks are not packet capture timestamps.'};
}
function normalizedEvents(raw,relay=false){
  const names=relay?raw.columns.packet:raw.columns.packets;
  assert.deepEqual(names,packetNames,'Unexpected packet trace schema');
  const eventNames=relay?raw.columns.event:raw.columns.events;
  const packetMap=new Map(raw.packets.map(row=>[row[0],Object.fromEntries(names.map((key,index)=>[key,row[index]]))]));
  assert.equal(packetMap.size,raw.packets.length,'Duplicate packet IDs');
  const origin=raw.clock.started.timeOrigin;
  assert(Number.isFinite(origin),'Missing monotonic epoch origin');
  return raw.events.map(row=>{
    const item=Object.fromEntries(eventNames.map((key,index)=>[key,row[index]]));
    const packet=packetMap.get(item.packetId);assert(packet,'Unresolved packet event ID');
    const key=names.slice(1).map(name=>packet[name]).join(':');
    return {...item,key,packet,side:relay?null:raw.side,
      sequence:packet.sequence,lane:(packet.sequence-1)%2,at:origin+item.enteredAt};
  });
}
function gapSummary(rows,at,kind){
  const entered=rows.filter(row=>Number.isFinite(at(row))),gaps=[];
  for(let i=1;i<entered.length;++i)gaps.push({ms:at(entered[i])-at(entered[i-1]),before:entered[i-1],after:entered[i]});
  return {kind,gapMs:distribution(gaps.map(row=>row.ms)),
    largest:gaps.toSorted((a,b)=>b.ms-a.ms).slice(0,5)};
}
function selectedPaths(raw){
  return raw.rtc.flatMap(sample=>sample.peers.filter(peer=>peer.pair).map(peer=>({
    at:sample.at,reason:sample.reason,route:sample.route,peerId:peer.peerId,
    pair:peer.pair,local:peer.local,remote:peer.remote,channels:peer.channels})));
}
function checkPeer(raw,parameters,errors){
  const add=(okay,message)=>{if(!okay)errors.push('P'+(raw.side+1)+': '+message);};
  const events=normalizedEvents(raw),expected=parameters.frames*2;
  const api=events.filter(row=>row.phase===0),sends=events.filter(row=>row.phase===1);
  const messages=events.filter(row=>row.phase===2),copies=events.filter(row=>row.phase===3);
  add(raw.done&&raw.nextPlan===expected,'deterministic schedule incomplete');
  add(raw.errors.length===0,'browser fixture errors: '+JSON.stringify(raw.errors));
  if(parameters.rtcConstructorTrace){
    const trace=raw.rtcConstructorTrace;
    add(trace?.valid===true,'RTC constructor trace invalid: '+JSON.stringify({counts:trace?.counts,overflow:trace?.overflow,identity:trace?.identity}));
    add(trace?.coverage?.exercised===true,'RTC constructor trace was not exercised');
  }
  add(Object.values(raw.overflow).every(value=>value===0),'trace overflow; incomplete observation');
  add(raw.counts.invalidPayloads===0,'received payload differs from complete deterministic bytes');
  add(raw.counts.unrecognizedBinary===0,'unrecognized binary transport data');
  add(api.length===expected&&sends.length===expected,'not every planned API call has exactly one actual send');
  add(messages.length===expected&&copies.length===expected,'not every remote packet reached message and poll-copy by drain cutoff');
  for(const [name,list,sender] of [['api',api,raw.side],['send',sends,raw.side],['message',messages,1-raw.side],['poll-copy',copies,1-raw.side]]){
    const sequences=list.map(row=>row.sequence).toSorted((a,b)=>a-b);
    add(sequences.length===expected&&sequences.every((sequence,index)=>sequence===index+1),name+' is not the complete exactly-once sequence set');
    add(list.every(row=>row.packet.sender===sender&&row.packet.destination===1-sender),name+' wrong sender or destination');
  }
  add(api.every(row=>row.result===1&&!row.threw),'shared API did not accept every planned packet');
  add(sends.every(row=>!row.threw),'actual send threw');
  add(raw.current.route===parameters.route,'final route differs from requested route');
  add(!raw.current.failed&&!raw.current.disconnected,'transport ended during health schedule');
  add(raw.states.filter(row=>row.kind==='transport'&&row.state.route).every(row=>row.state.route===parameters.route),'observed fallback or route switch');
  const channelMap=new Map(raw.channels.map(channel=>[channel.id,channel]));
  if(parameters.route==='rtc'){
    add(sends.every(row=>channelMap.get(row.channelId)?.kind==='rtc'),'direct assertion: E9NP left through non-RTC channel');
    add(messages.every(row=>channelMap.get(row.channelId)?.kind==='rtc'),'direct assertion: E9NP arrived through non-RTC channel');
    add(raw.current.peers.length===1,'direct assertion: expected exactly one RTC peer');
    for(const peer of raw.current.peers){
      add(peer.input?.readyState==='open'&&peer.control?.readyState==='open','both data channels must remain open');
      add(peer.input?.ordered===false&&peer.input?.maxRetransmits===0&&peer.input?.maxPacketLifeTime===null,'input channel must be unordered, maxRetransmits=0');
      add(peer.control?.ordered===true&&peer.control?.maxRetransmits===null&&peer.control?.maxPacketLifeTime===null,'control channel must be reliable and ordered');
    }
    const selected=selectedPaths(raw);
    add(selected.length>0,'direct assertion: no selected candidate-pair evidence');
    add(selected.every(row=>row.pair.state==='succeeded'&&row.local?.candidateType&&row.remote?.candidateType&&
      row.local.candidateType!=='relay'&&row.remote.candidateType!=='relay'),'direct assertion: unknown or relay selected candidate');
    const controlMessages=messages.filter(row=>row.lane===1);
    add(controlMessages.every((row,index)=>index===0||row.sequence>controlMessages[index-1].sequence),'reliable ordered lane delivered out of order');
  }else{
    add(sends.every(row=>channelMap.get(row.channelId)?.kind==='websocket'),'relay data did not use actual WebSocket');
    add(raw.current.peers.length===0,'forced relay unexpectedly retained RTC peers');
  }
  add(raw.current.queue===0,'application receive queue not drained');
  add(raw.blocks.length===(parameters.blockMs>0&&raw.side===parameters.blockSide?1:0),'requested short main-thread block coverage mismatch');
  const atStop=raw.clock.started.timeOrigin+raw.stopAt;
  const late=api.map(row=>row.at-(raw.startEpochMs+(row.sequence-1>>1)*1000/60));
  const laneSummary=[0,1].map(lane=>({lane:lane===0?'input':'control',
    api:api.filter(row=>row.lane===lane).length,actualSend:sends.filter(row=>row.lane===lane).length,
    message:messages.filter(row=>row.lane===lane).length,pollCopy:copies.filter(row=>row.lane===lane).length,
    receivedOutOfOrder:messages.filter(row=>row.lane===lane).reduce((result,row,index,all)=>result+Number(index>0&&row.sequence<all[index-1].sequence),0)}));
  const armedHeartbeat=raw.heartbeats.filter(row=>row.at>=raw.startAt);
  const armedRaf=raw.rafs.filter(row=>Number.isFinite(row.enteredAt)&&row.enteredAt>=raw.startAt);
  return {side:raw.side,lanes:laneSummary,plannedDeadlineToApiMs:distribution(late),
    apiToActualSend:stage(api,sends,atStop),messageToPollCopy:stage(messages,copies,atStop),
    actualSendDurationMs:distribution(sends.map(row=>row.exitedAt-row.enteredAt)),
    bufferedBefore:distribution(sends.map(row=>row.bufferedBefore)),bufferedAfter:distribution(sends.map(row=>row.bufferedAfter)),
    heartbeat:gapSummary(armedHeartbeat,row=>row.at,'actual heartbeat callback entry gaps after common start'),
    raf:gapSummary(armedRaf,row=>row.enteredAt,'actual requestAnimationFrame callback entry gaps after common start; no image is rendered'),
    allRecordedHeartbeat:gapSummary(raw.heartbeats,row=>row.at,'includes setup'),
    allRecordedRaf:gapSummary(raw.rafs,row=>row.enteredAt,'includes setup'),
    selectedPaths:selectedPaths(raw),blocks:raw.blocks,queueAtStop:raw.current.queue,
    unsentPlans:healthSchedule(parameters.frames).slice(raw.nextPlan),events};
}
function analyze(peers,relay,parameters,errors){
  const perPeer=peers.map(raw=>checkPeer(raw,parameters,errors));
  const stopEpoch=Math.max(...peers.map(raw=>raw.clock.started.timeOrigin+raw.stopAt));
  const relayEvents=normalizedEvents(relay,true);
  if(!relay.valid)errors.push('Relay diagnostic trace is not valid');
  if(parameters.route==='rtc'){
    if(relay.coverage.exercised||relayEvents.length)errors.push('Direct assertion: E9NP data traversed the relay or relay data trace was exercised');
  }else if(!relay.coverage.exercised)errors.push('Real shared relay data coverage is absent');
  if(parameters.impairmentSite==='relay'&&!relay.injection.anchor)errors.push('Independent relay impairment anchor was never exercised');
  const directions=[];
  for(let side=0;side<2;++side){
    const sender=perPeer[side].events,recipient=perPeer[1-side].events;
    const sent=sender.filter(row=>row.phase===1),received=recipient.filter(row=>row.phase===2);
    const item={sender:side,recipient:1-side,actualSendToPeerMessage:stage(sent,received,stopEpoch)};
    if(parameters.route==='relay'){
      const incoming=relayEvents.filter(row=>row.phase===0&&row.packet.sender===side);
      const outgoing=relayEvents.filter(row=>row.phase===2&&row.packet.sender===side);
      item.actualSendToRelayReceive=stage(sent,incoming,stopEpoch);
      item.relaySendToPeerMessage=stage(outgoing,received,stopEpoch);
      const byId=new Map(relayEvents.map(row=>[row.id,row]));
      const causal=[],unresolved=[];
      for(const out of outgoing){
        const before=byId.get(out.receiveEventId);
        if(before?.phase===0&&before.key===out.key)causal.push({ms:out.at-before.at,before,out});
        else unresolved.push(compactEvent(out,stopEpoch));
      }
      item.relayReceiveToActualSend={completedMs:distribution(causal.map(row=>row.ms)),unresolved,
        longest:causal.toSorted((a,b)=>b.ms-a.ms).slice(0,5).map(row=>({ms:row.ms,receiveEventId:row.before.id,
          sendEventId:row.out.id,scheduledId:row.out.scheduledId,plannedDelayMs:row.out.plannedDelayMs,
          dueAt:row.out.dueAt,receiveAt:row.before.at,sendAt:row.out.at,key:row.out.key})),
        link:'Explicit receiveEventId; not a guessed association by sequence.'};
      item.relayScheduledTimerLatenessMs=distribution(outgoing.filter(row=>row.scheduledId>=0&&Number.isFinite(row.dueAt)&&row.dueAt>=0)
        .map(row=>row.enteredAt-row.dueAt));
      if(unresolved.length)errors.push('Relay explicit receive-to-send causal IDs did not resolve');
      if(incoming.length!==parameters.frames*2||outgoing.length!==parameters.frames*2)errors.push('Relay direction '+side+' did not observe every send/receive');
    }
    directions.push(item);
  }
  return {peers:perPeer.map(({events,...value})=>value),directions,
    unmatchedInterpretation:'All unmatched packet occurrences and pending relay jobs are retained, including end-of-window tails. A completed-latency percentile is not an upper bound for them.',
    clockInterpretation:'One machine, independent process monotonic clocks mapped by timeOrigin. Browser message entry is after browser dispatch; poll-copy is JS transport dequeue, not C++ ApplyInputPacket or game confirmation.',
    acceptance:'PASS only means strict route, deterministic transport delivery/integrity and diagnostic coverage. There is no production FPS, public network, native repair or latency-performance acceptance gate.'};
}
function envNumber(name,fallback,min,max,integer=true){
  const value=process.env[name]===undefined?fallback:Number(process.env[name]);
  assert(Number.isFinite(value)&&value>=min&&value<=max&&(!integer||Number.isInteger(value)),'Invalid '+name);return value;
}
function configFromEnv(){
  const args=process.argv.slice(2),relay=args.includes('--relay'),rtc=args.includes('--rtc');
  assert(relay!==rtc&&args.every(arg=>arg==='--relay'||arg==='--rtc'),'Specify exactly one of --relay or --rtc');
  const route=rtc?'rtc':'relay',frames=envNumber('HEALTH_FRAMES',720,60,3600);
  const rtcConstructorTrace=process.env.HEALTH_RTC_CONSTRUCTOR_TRACE==='1';
  assert(!rtcConstructorTrace||rtc,'HEALTH_RTC_CONSTRUCTOR_TRACE requires --rtc');
  const impairmentSite=process.env.HEALTH_IMPAIRMENT_SITE??'none';
  assert(['none','relay'].includes(impairmentSite),'HEALTH_IMPAIRMENT_SITE must be none or relay');
  assert(!(route==='rtc'&&impairmentSite==='relay'),'Relay delay cannot be claimed as RTC input impairment');
  const blockMs=envNumber('HEALTH_BLOCK_MS',0,0,1000);
  const blockAtMs=envNumber('HEALTH_BLOCK_AT_MS',4000,0,60000);
  assert(!blockMs||blockAtMs+blockMs<frames*1000/60,'Block must finish inside the fixed schedule');
  return {route,frames,hz:60,packetsPerFrame:2,seed:envNumber('HEALTH_SEED',12345,0,0xffffffff),
    cpuRates:[1,envNumber('HEALTH_CPU_RATE',4,1,20,false)],pollMs:envNumber('HEALTH_POLL_MS',4,1,100),
    heartbeatMs:10,drainMs:envNumber('HEALTH_DRAIN_MS',2000,500,10000),blockMs,blockAtMs,
    blockSide:envNumber('HEALTH_BLOCK_SIDE',1,0,1),impairmentSite,hostLoad:process.env.HOST_LOAD==='1',
    browserOnlyProxyBypass:process.env.HEALTH_PROXY_BYPASS==='1',rtcConstructorTrace,
    gpuFlags:process.env.HEALTH_GPU_FLAGS==='1',
    injection:{inject:impairmentSite==='relay',
      delay:impairmentSite==='relay'?envNumber('HEALTH_DELAY_MS',39,0,2000):0,
      jitter:impairmentSite==='relay'?envNumber('HEALTH_JITTER_MS',5,0,1000):0,
      burstAt:impairmentSite==='relay'?envNumber('HEALTH_BURST_AT_MS',4000,0,60000):0,
      burstFor:impairmentSite==='relay'?envNumber('HEALTH_BURST_FOR_MS',500,0,5000):0,
      burstDelay:impairmentSite==='relay'?envNumber('HEALTH_BURST_DELAY_MS',120,0,3000):0}};
}
function readIdentity(file){const bytes=fs.readFileSync(file);return {path:path.resolve(file),bytes:bytes.length,sha256:sha(bytes)};}
function gitIdentity(directory){
  const run=(...args)=>execFileSync('git',['-C',directory,...args],{encoding:'utf8',windowsHide:true}).trim();
  return {root:run('rev-parse','--show-toplevel'),head:run('rev-parse','HEAD'),status:run('status','--porcelain','--untracked-files=no')};
}
async function freePort(){
  const server=net.createServer();await new Promise((resolve,reject)=>{server.once('error',reject);server.listen(0,'127.0.0.1',resolve);});
  const port=server.address().port;await new Promise(resolve=>server.close(resolve));return port;
}
async function waitFor(predicate,timeoutMs,message){
  const deadline=Date.now()+timeoutMs;
  while(Date.now()<deadline){if(await predicate())return;await delay(50);}
  throw Error(message);
}
function browserAssertions(page,origin,errors,observations){
  page.on('pageerror',error=>{if(errors.length<200)errors.push('Browser page error: '+error.message);});
  page.on('request',request=>{
    const url=request.url();if(observations.length<100)observations.push({url,type:request.resourceType()});
    if(![origin+'/',origin+'/health.mjs',origin+'/transport.json',origin+'/favicon.ico'].includes(url)&&errors.length<200)
      errors.push('Unexpected resource request in no-WASM fixture: '+url);
  });
}
async function run(){
  const parameters=configFromEnv();
  assert(process.env.EAGLER_COMMON_ROOT,'EAGLER_COMMON_ROOT must explicitly identify the frozen common worktree');
  assert(process.env.EAGLER_RELAY_SOURCE,'EAGLER_RELAY_SOURCE must explicitly identify the actual shared relay source');
  const commonRoot=path.resolve(process.env.EAGLER_COMMON_ROOT),relayPath=path.resolve(process.env.EAGLER_RELAY_SOURCE);
  const expectedHead=process.env.HEALTH_EXPECTED_COMMON_HEAD??baselineCommon;
  const label=process.env.RUN_LABEL;
  assert(label&&/^[a-zA-Z0-9_-]{1,120}$/.test(label),'RUN_LABEL must be explicit and filename-safe');
  const output=path.join(project,'web/artifacts/multiplayer-tests',label+'-report.json');
  assert(!fs.existsSync(output),'Raw report already exists; choose a new RUN_LABEL');
  const commonGit=gitIdentity(commonRoot);
  assert.equal(commonGit.head,expectedHead,'Common HEAD differs from explicit frozen identity');
  assert.equal(commonGit.status,'','Frozen common worktree has tracked changes');
  const commonPath=path.join(commonRoot,'src/netplay/BrowserPeerTransport.cpp');
  const sourceText=fs.readFileSync(commonPath,'utf8'),sources=extractBodies(sourceText);
  const files=[fileURLToPath(import.meta.url),path.join(here,'transport-health-fixture.mjs'),
    path.join(here,'relay-transport-controller.mjs'),path.join(here,'relay-transport-fixture.mjs'),
    path.join(here,'relay-transport-probe.mjs'),path.join(here,'host-load-probe.mjs'),
    path.resolve(here,'../../../../../tools/browser/launch.mjs'),commonPath,relayPath];
  const identities=files.map(readIdentity),schedule=healthSchedule(parameters.frames);
  const tapeSha256=[0,1].map(side=>sha(Buffer.concat(schedule.map(plan=>Buffer.from(healthPacket(plan,side,parameters.seed))))));
  const report={schema:'th09/transport-health/1',label,startedUtc:new Date().toISOString(),passed:false,parameters,
    fixture:{scheduleSha256:sha(JSON.stringify(schedule)),tapeSha256,plannedPacketsPerSide:schedule.length,
      bytesPerPacket:56,syntheticInputPayload:true,fixedLogicalSchedule:true,allDuePlansRetained:true,
      synchronousApplicationSend:true,clientImpairmentTimer:false,routeScope:'real shared relay/signaling; neutral transport-only room without product lobby admission',
      rtcScope:'two local browser processes on the same physical host; selected host/srflx pair is not public or remote acceptance',
      gpuScope:'Optional GPU/ANGLE launch arguments only; no WebGL context is created and no GPU renderer is measured or validated. Recorded browserArgs are arguments supplied to launchBrowser, excluding Playwright defaults.',
      abiScope:'shared EM_JS bodies with JavaScript Uint8Array memory, no WebAssembly, C++ SessionChannel/automatic repair policy, game, audio, drawing or assets'},
    identity:{commonGit,expectedCommonHead:expectedHead,relayGit:gitIdentity(path.dirname(relayPath)),files:identities,
      rtcConstructorTraceSourceSha256:parameters.rtcConstructorTrace?identities[1].sha256:null,
      functionBodies:Object.fromEntries(Object.entries(sources).map(([name,value])=>[name,{args:value.args,sha256:value.sha256}]))},
    browsers:[],peers:[],relayTrace:null,hostLoad:null,summary:null,errors:[],
    lifecycle:{events:[],relayStdout:'',relayStderr:'',relayOutputOverflowBytes:0,relayExit:null},
    performanceVerdict:'Not a game-performance acceptance run. No latency cutoff discards or reclassifies this sample.'};
  const {errors,lifecycle}=report,mark=name=>{lifecycle.events.push({name,epochMs:Date.now()});host?.mark(name);};
  let server=null,relay=null,host=null,relayClosed=false;const browsers=[],pages=[];
  const {launchBrowser}=await import('../../../../../tools/browser/launch.mjs');
  const {startRelayDiagnosticProcess}=await import('./relay-transport-controller.mjs');
  const {startHostLoadProbe}=await import('./host-load-probe.mjs');
  try{
    if(parameters.hostLoad)host=startHostLoadProbe();
    mark('health-start');
    const fixtureBytes=fs.readFileSync(path.join(here,'transport-health-fixture.mjs'));
    const sourceBytes=Buffer.from(JSON.stringify(sources));
    server=http.createServer((request,response)=>{
      response.setHeader('Cache-Control','no-store');response.setHeader('Cross-Origin-Opener-Policy','same-origin');
      response.setHeader('Cross-Origin-Embedder-Policy','require-corp');
      if(request.method!=='GET'&&request.method!=='HEAD'){response.writeHead(405);response.end();return;}
      const pathname=new URL(request.url,'http://127.0.0.1').pathname;
      const body=pathname==='/'?Buffer.from('<!doctype html><meta charset="utf-8"><title>TH09 transport health</title><body>Transport only</body>'):
        pathname==='/health.mjs'?fixtureBytes:pathname==='/transport.json'?sourceBytes:null;
      if(!body){response.writeHead(pathname==='/favicon.ico'?204:404);response.end();return;}
      response.setHeader('Content-Type',pathname==='/health.mjs'?'text/javascript':pathname==='/transport.json'?'application/json':'text/html');
      response.writeHead(200);response.end(request.method==='HEAD'?undefined:body);
    });
    await new Promise((resolve,reject)=>{server.once('error',reject);server.listen(0,'127.0.0.1',resolve);});
    const origin='http://127.0.0.1:'+server.address().port,port=await freePort();
    const childEnv={EAGLER_NETPLAY_RELAY_DELAY_MS:'0',EAGLER_NETPLAY_RELAY_JITTER_MS:'0',EAGLER_NETPLAY_RELAY_DROP_EVERY:'0',
      EAGLER_NETPLAY_STUN_URLS:'',EAGLER_NETPLAY_TURN_URLS:'',EAGLER_NETPLAY_TURN_SECRET:'',EAGLER_NETPLAY_TURN_USERNAME:'',EAGLER_NETPLAY_TURN_PASSWORD:'',
      TH07_RELAY_DELAY_MS:'0',TH07_RELAY_JITTER_MS:'0',TH07_RELAY_DROP_EVERY:'0',TH07_STUN_URLS:'',TH07_TURN_URLS:'',TH07_TURN_SECRET:''};
    relay=startRelayDiagnosticProcess({relayPath,port,config:parameters.injection,env:childEnv});
    host?.registerKnownProcess(relay.process.pid,'transport-health-relay');
    relay.process.once('exit',(code,signal)=>{relayClosed=true;lifecycle.relayExit={code,signal,epochMs:Date.now()};});
    relay.process.once('error',error=>errors.push('Owned relay child error: '+error.message));
    function outputChunk(field,chunk){const text=String(chunk),space=32768-lifecycle[field].length;
      lifecycle[field]+=text.slice(0,Math.max(0,space));lifecycle.relayOutputOverflowBytes+=Math.max(0,text.length-space);}
    relay.process.stdout.on('data',chunk=>outputChunk('relayStdout',chunk));
    relay.process.stderr.on('data',chunk=>outputChunk('relayStderr',chunk));
    await waitFor(()=>{if(relayClosed)throw Error('Owned relay exited before ready');return lifecycle.relayStdout.includes('netplay relay listening');},15000,'Owned shared relay did not become ready');
    mark('relay-ready');
    const room='health-'+randomBytes(8).toString('hex');
    const relayUrl='ws://127.0.0.1:'+port+'/?room='+room+'&run=1&players=2';
    report.fixture.origin=origin;report.fixture.relayUrl=relayUrl;
    const browserArgs=['--disable-background-timer-throttling','--disable-backgrounding-occluded-windows','--disable-renderer-backgrounding'];
    if(parameters.browserOnlyProxyBypass)browserArgs.push('--no-proxy-server');
    if(parameters.gpuFlags)browserArgs.push('--enable-gpu','--use-gl=angle','--use-angle=d3d11');
    report.fixture.browserArgs=browserArgs.slice();
    for(let side=0;side<2;++side){
      const browser=await launchBrowser({headless:true,args:browserArgs});browsers.push(browser);
      const page=await browser.newPage();pages.push(page);const requests=[];
      browserAssertions(page,origin,errors,requests);
      const cdp=await page.context().newCDPSession(page);
      await cdp.send('Emulation.setCPUThrottlingRate',{rate:parameters.cpuRates[side]});
      await page.goto(origin+'/',{waitUntil:'load',timeout:15000});
      const metadata=await page.evaluate(()=>({userAgent:navigator.userAgent,hardwareConcurrency:navigator.hardwareConcurrency,
        timeOrigin:performance.timeOrigin,visibility:document.visibilityState,hasFocus:document.hasFocus()}));
      report.browsers.push({side,version:browser.version(),requestedCpuRate:parameters.cpuRates[side],args:browserArgs,requests,...metadata});
    }
    // Connect only after both independent browser processes/pages are ready.
    await Promise.all(pages.map((page,side)=>page.evaluate(async options=>{
        const {installTransportHealthPage}=await import('/health.mjs');
        const response=await fetch('/transport.json');if(!response.ok)throw Error('Missing frozen transport bodies');
        return installTransportHealthPage({...options,sources:await response.json()});
      },{side,frames:parameters.frames,seed:parameters.seed,route:parameters.route,relayUrl,
        pollMs:parameters.pollMs,blockAtMs:parameters.blockAtMs,blockMs:parameters.blockMs,blockSide:parameters.blockSide,
        rtcConstructorTrace:parameters.rtcConstructorTrace})));
    await waitFor(async()=>{if(relayClosed)throw Error('Owned relay ended during route selection');
      const result=await Promise.all(pages.map(page=>page.evaluate(()=>({ready:globalThis.__transportHealth?.ready(),
        state:globalThis.__eaglerPeerTransport?.route,failed:globalThis.__eaglerPeerTransport?.failed,disconnected:globalThis.__eaglerPeerTransport?.disconnected}))));
      if(result.some(value=>value.failed||value.disconnected))throw Error('Shared browser transport failed before schedule');
      if(result.some(value=>value.state&&value.state!==parameters.route))throw Error('Requested '+parameters.route+' selected a different route');
      return result.every(value=>value.ready);
    },30000,'Requested shared transport route did not open');
    await Promise.all(pages.map(page=>page.evaluate(()=>globalThis.__transportHealth.observeRtc('ready'))));
    mark('routes-ready');
    const epochMs=Date.now()+1500;
    report.fixture.commonStartEpochMs=epochMs;
    await Promise.all(pages.map(page=>page.evaluate(epoch=>globalThis.__transportHealth.arm(epoch),epochMs)));
    mark('schedule-armed');
    await waitFor(async()=>{
      if(relayClosed)throw Error('Owned relay exited during schedule');
      return (await Promise.all(pages.map(page=>page.evaluate(()=>globalThis.__transportHealth.progress().done)))).every(Boolean);
    },parameters.frames*1000/60+15000,'Fixed packet schedule did not finish before bounded timeout');
    mark('schedule-complete');
    await delay(parameters.drainMs);
    report.peers=await Promise.all(pages.map(page=>page.evaluate(()=>globalThis.__transportHealth.stop())));
    mark('browser-drain-stopped');
    report.relayTrace=await relay.snapshot({stop:true});mark('relay-trace-stopped');
    report.summary=analyze(report.peers,report.relayTrace,parameters,errors);
    if(report.relayTrace.sourceIdentity?.relaySha256!==identities.at(-1).sha256)errors.push('Relay trace source identity differs from pre-run relay file');
  }catch(error){
    errors.push(error.stack||String(error));mark('run-failed');
  }finally{
    for(let side=0;side<pages.length;++side){
      if(!report.peers[side]){
        try{report.peers[side]=await pages[side].evaluate(()=>globalThis.__transportHealth?.stop()??null);}
        catch(error){errors.push('P'+(side+1)+' failure snapshot: '+error.message);}
      }
    }
    if(relay&&!report.relayTrace&&!relayClosed){
      try{report.relayTrace=await relay.snapshot({stop:true});}
      catch(error){errors.push('Relay failure snapshot: '+error.message);}
    }
    if(!report.summary&&report.peers.length===2&&report.peers.every(Boolean)&&report.relayTrace){
      try{report.summary=analyze(report.peers,report.relayTrace,parameters,errors);}
      catch(error){errors.push('Failure evidence analysis: '+error.message);}
    }
    for(const page of pages){try{await page.evaluate(()=>globalThis.__transportHealth?.close());}catch(error){errors.push('Owned page transport cleanup: '+error.message);}}
    for(const browser of browsers){try{await browser.close();}catch(error){errors.push('Owned browser cleanup: '+error.message);}}
    if(relay&&!relayClosed){
      relay.process.kill();
      try{await waitFor(()=>relayClosed,5000,'Owned relay did not confirm exit');}catch(error){errors.push(error.message);}
    }
    if(server){server.closeAllConnections();await new Promise(resolve=>server.close(resolve));}
    mark('owned-processes-stopped');
    if(host){try{report.hostLoad=await host.stop();}catch(error){errors.push('Host sampler stop: '+error.message);}}
    try{
      report.identity.after=files.map(readIdentity);
      for(let i=0;i<files.length;++i)if(report.identity.after[i].sha256!==identities[i].sha256)errors.push('Source changed during run: '+files[i]);
      report.identity.commonAfter=gitIdentity(commonRoot);
      if(report.identity.commonAfter.head!==commonGit.head||report.identity.commonAfter.status!==commonGit.status)errors.push('Frozen common worktree changed during run');
    }catch(error){errors.push('Final source identity: '+error.message);}
    report.finishedUtc=new Date().toISOString();report.passed=errors.length===0&&!!report.summary;
    fs.mkdirSync(path.dirname(output),{recursive:true});
    fs.writeFileSync(output,JSON.stringify(report,null,2)+'\n',{flag:'wx'});
    console.log(JSON.stringify({report:output,sha256:sha(fs.readFileSync(output)),passed:report.passed,
      parameters,packetCounts:report.peers.map(value=>value?.counts??null),
      errors,performanceVerdict:report.performanceVerdict}));
    if(!report.passed)process.exitCode=1;
  }
}
function constructorContractTest(){
  let time=0;const observed=[],nativeCalls=[],channel={marker:true};
  const trace=createHealthConstructorTrace(()=>time);
  class NativePC {
    constructor(...args){nativeCalls.push({args,target:new.target,receiver:this});time+=37;}
    addEventListener(name,callback){this.listener={name,callback,at:time};time+=3;}
    createDataChannel(...args){this.dataChannelArgs=args;return channel;}
  }
  const Observer=healthPeerConnectionObserver(NativePC,(...args)=>observed.push(args),trace);
  const configuration={},second={},first=new Observer(configuration,second);
  assert(first instanceof NativePC&&first instanceof Observer);
  assert.equal(nativeCalls[0].target,Observer);assert.equal(nativeCalls[0].receiver,first);
  assert.equal(nativeCalls[0].args[0],configuration);assert.equal(nativeCalls[0].args[1],second);
  assert.deepEqual(trace.snapshot(true).events.map(e=>[e.call,e.event,e.at]),[[0,'enter',0],[0,'returned',37]]);
  assert.equal(first.listener.name,'datachannel');assert.equal(first.listener.at,37);
  assert.equal(first.createDataChannel('input',configuration),channel);
  assert.deepEqual(first.dataChannelArgs,['input',configuration]);
  first.listener.callback({channel});
  assert.deepEqual(observed,[[channel,'rtc'],[channel,'rtc']]);
  class Derived extends Observer{}
  new Derived(configuration);
  assert.equal(nativeCalls[1].target,Derived);
  const failure=Object.assign(new Error('must not be retained'),{name:'OperationError'});
  class ThrowingPC {constructor(){time+=11;throw failure;}}
  const ThrowingObserver=healthPeerConnectionObserver(ThrowingPC,()=>{},trace);
  assert.throws(()=>new ThrowingObserver(),error=>error===failure);
  const complete=trace.snapshot(true);
  assert.deepEqual(complete.counts,{entered:3,returned:2,threw:1,observerErrors:0});
  assert.equal(complete.events.at(-1).event,'threw');assert.equal(complete.events.at(-1).errorName,'OperationError');
  assert.equal(complete.valid,true);assert.equal(trace.snapshot(false).valid,false);
  assert(!JSON.stringify(complete).includes('must not be retained'));
  const bounded=createHealthConstructorTrace(()=>time,2);
  const BoundedObserver=healthPeerConnectionObserver(NativePC,()=>{},bounded);
  new BoundedObserver();new BoundedObserver();
  assert.equal(bounded.snapshot(true).events.length,2);assert.equal(bounded.snapshot(true).overflow,2);
  assert.equal(bounded.snapshot(true).valid,false);
  const brokenClock=createHealthConstructorTrace(()=>{throw Error('observer clock');});
  assert(new (healthPeerConnectionObserver(NativePC,()=>{},brokenClock))() instanceof NativePC);
  assert.throws(()=>new (healthPeerConnectionObserver(ThrowingPC,()=>{},brokenClock))(),error=>error===failure);
  assert.equal(brokenClock.snapshot(true).counts.observerErrors,4);
  const OffObserver=healthPeerConnectionObserver(NativePC,()=>{});
  const off=new OffObserver(configuration);assert(off instanceof NativePC);
  assert.equal(nativeCalls.at(-1).target,OffObserver);
  return {passed:true,checks:['arguments','receiver','newTarget','subclass','entry-return-before-listener',
    'createDataChannel-return','original-exception','bounded-overflow','observer-error-isolation','default-off']};
}

function selfTest(){
  const constructorContract=constructorContractTest();
  const plans=healthSchedule(720);
  assert.equal(plans.length,1440);assert.equal(plans.at(-1).dueMs,719*1000/60);
  assert.deepEqual(healthSchedule(720),plans);
  for(const side of [0,1])for(const plan of plans){
    const bytes=healthPacket(plan,side,12345),parsed=assertHealthPacket(bytes,plans,side,12345);
    assert.equal(parsed.sequence,plan.sequence);assert.equal(parsed.lane,plan.lane);assert.equal(parsed.fields.at(-1),1-side);
    const envelope=new Uint8Array(58);envelope.set([0xe7,1-side]);envelope.set(bytes,2);
    assert.deepEqual(parseHealthPacket(envelope).fields,parsed.fields);
  }
  const altered=healthPacket(plans[50],1,12345);altered[48]^=1;
  assert.throws(()=>assertHealthPacket(altered,plans,1,12345));
  const linked=matchEvents([{id:0,key:'repeat',at:10},{id:1,key:'repeat',at:20},{id:2,key:'pending',at:30}],
    [{id:10,key:'repeat',at:11},{id:11,key:'repeat',at:18},{id:12,key:'unknown',at:40}]);
  assert.deepEqual(linked.matched.map(row=>[row.before.id,row.after.id,row.ms]),[[0,10,1],[1,11,-2]]);
  assert.deepEqual(linked.unmatchedBefore.map(row=>row.id),[2]);assert.deepEqual(linked.unmatchedAfter.map(row=>row.id),[12]);
  assert.equal(distribution(linked.matched.map(row=>row.ms)).negative,1);
  let bodies=null;
  if(process.env.EAGLER_COMMON_ROOT){
    const file=path.resolve(process.env.EAGLER_COMMON_ROOT,'src/netplay/BrowserPeerTransport.cpp');
    bodies=extractBodies(fs.readFileSync(file,'utf8'));
    assert.deepEqual(bodies.eagler_peer_connect.args,['relayUrlPtr','tagPtr','localPlayer','playerCount']);
    assert.deepEqual(bodies.eagler_peer_poll_copy.args,['out','capacity']);
  }
  console.log(JSON.stringify({passed:true,scope:'Node pure packet/schedule/FIFO/optional source parsing; no browser, relay process, network, WASM or benchmark',
    packetsChecked:plans.length*2,parsedFunctions:bodies?Object.keys(bodies):[],constructorContract}));
}
if(process.argv.includes('--self-test'))selfTest();
else run().catch(error=>{console.error(error.stack||String(error));process.exitCode=1;});

