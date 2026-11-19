// Production WASM, real game frames, production relay protocol and live TURN.
// Impairment is scoped to the initial DataChannels of the isolated match.
import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {createHash} from 'node:crypto';
import {mkdirSync, readFileSync, writeFileSync} from 'node:fs';
import net from 'node:net';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {launchBrowser} from '../../../../../tools/browser/launch.mjs';
import {verifyRuntimeDelivery} from '../../../../../tools/browser/verified-runtime.mjs';

const repository = fileURLToPath(new URL('../../../../../', import.meta.url));
const output = resolve(process.env.ROUTE_REPORT || 'artifacts/adaptive-route-game.json');
const artifactDirectory = resolve(process.env.PC_BUILD);
const startupOnly=process.env.ROUTE_STARTUP_ONLY==='1';
const scenario = startupOnly ? 'stable' : process.env.ROUTE_SCENARIO || 'outage';
const burstMs = Number(process.env.ROUTE_BURST_MS || 2500);
const durationMs = Number(process.env.ROUTE_DURATION_MS || (scenario === 'outage' ? 35000 : 60000));
const adaptiveDisabled=process.env.ROUTE_ADAPTIVE_DISABLED==='1';
const onDemand=process.env.ROUTE_ON_DEMAND==='1';
const controlFault=!startupOnly&&process.env.ROUTE_CONTROL_FAULT==='1';
const localTurnUrl = process.env.ROUTE_LOCAL_TURN_URL;
const localTurnSecret = process.env.ROUTE_LOCAL_TURN_SECRET;
const localTurnUsername = process.env.ROUTE_LOCAL_TURN_USERNAME;
const localTurnCredential = process.env.ROUTE_LOCAL_TURN_CREDENTIAL;
const turnProxyTarget = process.env.ROUTE_TURN_PROXY_TARGET;
if (localTurnUrl) {
  assert.match(localTurnUrl, /^turn:127\.0\.0\.1:\d+(?:\?transport=udp)?$/);
  assert.ok(localTurnSecret || localTurnUsername && localTurnCredential);
}
assert.ok(['stable','bursts','degraded','outage'].includes(scenario));
assert.ok(Number.isFinite(burstMs) && burstMs > 0 && burstMs <= 10000);
assert.ok(Number.isFinite(durationMs) && durationMs >= 15000 && durationMs <= 180000);
const build = JSON.parse(readFileSync(resolve(artifactDirectory, 'build.json')));
const actualHash = createHash('sha256').update(readFileSync(resolve(artifactDirectory, 'th09.wasm'))).digest('hex');
assert.equal(build.sha256, actualHash);
assert.equal(build.kind, 'th09-native-web-release-candidate');
mkdirSync(resolve(output, '..'), {recursive:true});
const host = await presentationServer(0, {release:true, artifactDirectory});
let browser, relay, page, delivery;
const errors = [];
let relayLog = '';
function loadIceServers(origin) {
  const url = new URL('/eagler-netplay/?diagnostic=1', origin);
  url.protocol = 'wss:';
  return new Promise((accept,reject) => {
    const ws = new WebSocket(url);
    const finish = (error, servers) => {
      clearTimeout(timer);
      ws.close();
      if(error)reject(error);
      else accept(servers);
    };
    const timer = setTimeout(() => finish(Error('TURN credential timeout')), 10000);
    ws.onmessage = event => {
      const message = JSON.parse(event.data);
      if(message.type==='diagnostic-ready')finish(null,message.iceServers);
    };
    ws.onerror = () => finish(Error('TURN diagnostic failed'));
  });
}
try {
  const iceServers = localTurnUrl ? [{urls:[localTurnUrl],username:localTurnUsername,credential:localTurnCredential}] :
    await loadIceServers(process.env.TURN_ORIGIN || 'https://touhou.vip');
  browser = await launchBrowser({args:['--enable-gpu','--use-gl=angle','--use-angle=d3d11',
    '--autoplay-policy=no-user-gesture-required','--no-proxy-server']});
  page = await browser.newPage();
  page.on('pageerror', error => errors.push(String(error)));
  const runtimeUrl=process.env.ROUTE_RUNTIME_URL;
  delivery=await verifyRuntimeDelivery(page,{
    scriptUrl:runtimeUrl||host.url+'/probe.mjs',
    scriptBytes:readFileSync(resolve(artifactDirectory,'th09.mjs')),
    wasmName:'th09.wasm',wasmBytes:readFileSync(resolve(artifactDirectory,'th09.wasm')),
  });
  if(runtimeUrl){
    await page.route(host.url+'/probe.mjs',route=>route.fulfill({status:302,headers:{location:runtimeUrl}}));
  }
  await page.goto(host.url);
  const turnHost = process.env.ROUTE_TURN_HOST;
  const turn = iceServers.map(s => ({...s, urls:[s.urls].flat().filter(url =>
    /^turns?:/.test(url) && (!turnHost || new URL(url.replace(/^turns?:/, 'http://')).hostname === turnHost))}))
    .find(s => s.urls.length);
  assert.ok(turn && (localTurnUrl || turn.credential));
  const port = await new Promise(accept => {const s=net.createServer();s.listen(0,'127.0.0.1',()=>{
    const port=s.address().port;s.close(()=>accept(port));});});
  const externalRelay=process.env.ROUTE_RELAY_URL;
  if(!externalRelay){
   relay = spawn(process.execPath,[resolve(repository,'services/relay/src/netplay-relay.mjs')],{
    windowsHide:true,stdio:['ignore','pipe','pipe'],env:{...process.env,
      EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:String(port),
      EAGLER_NETPLAY_STUN_URLS:'',EAGLER_NETPLAY_TURN_CONFIG_FILE:'',EAGLER_NETPLAY_TURN_SHARED_SECRET:localTurnSecret || '',
      EAGLER_NETPLAY_TURN_URLS:(Array.isArray(turn.urls)?turn.urls:[turn.urls]).join(','),
      EAGLER_NETPLAY_TURN_USERNAME:turn?.username || '',EAGLER_NETPLAY_TURN_CREDENTIAL:turn?.credential || '',
  }});
  await new Promise((accept,reject)=>{
    relay.stdout.on('data',b=>{relayLog+=b;if(relayLog.includes('netplay relay listening'))accept();});
    relay.stderr.on('data',b=>relayLog+=b);relay.on('error',reject);
    relay.on('exit',code=>reject(Error('Relay exited '+code)));
  });
  }
  const relayUrl=externalRelay||`ws://127.0.0.1:${port}/`;
  const result = await page.evaluate(async ({relayUrl,scenario,burstMs,durationMs,startupOnly,adaptiveDisabled,controlFault,onDemand}) => {
    const frames=[], lobbies=[], events=[], closes=[], hashMaps=[new Map(),new Map()];
    const began=performance.now();
    const sleep=ms=>new Promise(r=>setTimeout(r,ms));
    const wait=async condition=>{const start=performance.now();while(!condition()){
      if(closes.length)throw Error(JSON.stringify(closes));
      if(performance.now()-start>20000)throw Error('Room/start timeout');await sleep(25);}};
    const stats=w=>{const at=w.core._th09_rollback_info()/4;return Array.from(w.core.HEAPU32.subarray(at,at+8));};
    const selectedPair=async peer=>{
      const reports=await peer.pc.getStats();
      let pair;
      for(const row of reports.values())if(row.type==='transport'&&row.selectedCandidatePairId)pair=reports.get(row.selectedCandidatePairId);
      if(!pair)for(const row of reports.values())if(row.type==='candidate-pair'&&row.state==='succeeded'&&row.nominated)pair=row;
      const local=reports.get(pair?.localCandidateId),remote=reports.get(pair?.remoteCandidateId);
      return {local:local?.candidateType,remote:remote?.candidateType,turn:local?.url,protocol:local?.relayProtocol,
        iceRttMs:pair?.currentRoundTripTime==null?null:pair.currentRoundTripTime*1000};
    };
    globalThis.routeGameEvidence={events,closes};
    globalThis.routeGameSnapshot=()=>({events,closes,packets:frames.map(w=>w.routePackets),
      routeTrace:frames.map(w=>w.routeTrace),
      peers:frames.map(w=>w.__eaglerPeerTransport?[...w.__eaglerPeerTransport.peers.values()].map(p=>p.adaptive.snapshot()):[])});
    // Prepare both worlds before reserving a public room. A failed resource
    // load must not leave a started run waiting for clients that never join.
    for(let i=0;i<2;i++){
      const frame=document.createElement('iframe');frame.src='/';document.body.append(frame);
      await new Promise(r=>frame.onload=r);const w=frame.contentWindow;frames.push(w);
      await w.openProbe(0,1,2,3,true);
      w.turnConnections=0;
      const trace=w.routeTrace=[];
      const record=(type,info)=>trace.push({at:Math.round(performance.now()-began),type,...info});
      const Socket=w.WebSocket;
      const watchedMessages=new Set(['peers','rtc-startup','rtc-startup-quality','rtc-startup-select',
        'rtc-standby','rtc-route-slow','rtc-route-unavailable','rtc-route-report','route']);
      const recordSignal=(direction,data)=>{
        if(typeof data!=='string')return;
        try {
          const {type,serial,phase,route,quality,direct}=JSON.parse(data);
          if(watchedMessages.has(type))record(direction,{message:type,serial,phase,route,quality,direct});
        }catch{}
      };
      w.WebSocket=class extends Socket {
        constructor(...args){
          super(...args);
          this.addEventListener('message',event=>recordSignal('received',event.data));
        }
        send(data){recordSignal('sent',data);return super.send(data);}
      };
      const PeerConnection=w.RTCPeerConnection;
      let connections=0;
      w.RTCPeerConnection=class extends PeerConnection {
        constructor(config){
          super(config);
          const id=++connections, relay=config?.iceServers?.some(server=>[server.urls].flat().some(url=>/^turns?:/.test(url)))===true;
          if(relay)++w.turnConnections;
          record('connection',{id,relay});
          this.addEventListener('connectionstatechange',()=>record('connection-state',{id,state:this.connectionState}));
          this.addEventListener('icecandidate',({candidate})=>{
            if(candidate?.type==='relay')record('relay-candidate',{id});
          });
        }
        close(){
          record('connection-closed',{relay:this.getConfiguration().iceTransportPolicy==='relay'});
          return super.close();
        }
      };
    }
    const room='th09mp-'+(10000000+crypto.getRandomValues(new Uint32Array(1))[0]%90000000);
    const members=[crypto.randomUUID(),crypto.randomUUID()],clientIds=[crypto.randomUUID(),crypto.randomUUID()];
    const address=params=>{const url=new URL(relayUrl);for(const [key,value] of Object.entries(params))url.searchParams.set(key,String(value));return url.href;};
    let cleaned=false;
    globalThis.cleanupRouteGame=async()=>{
      if(cleaned)return;cleaned=true;
      for(const w of frames){w.core?._th09_loop_pause(1);w.play?.close();}
      for(const member of members){
        const ws=new WebSocket(address({directory:1,member}));
        await new Promise(resolve=>{
          const timer=setTimeout(()=>{ws.close();resolve();},2500);
          ws.onmessage=event=>{const m=JSON.parse(event.data);
            if(m.mine?.recoveryToken)ws.send(JSON.stringify({type:'release-membership',recoveryToken:m.mine.recoveryToken}));
            else{clearTimeout(timer);ws.close();resolve();}};
          ws.onerror=()=>{clearTimeout(timer);ws.close();resolve();};
        });
      }
      for(const ws of lobbies)ws.close();
    };
    for(let i=0;i<2;i++){
      const ws=new WebSocket(address({room,lobby:clientIds[i],member:members[i],intent:i?'join':'create',visibility:'private'}));
      ws.onmessage=e=>{
        const m=JSON.parse(e.data);
        if(m.type==='error')closes.push(['lobby',m.error]);
        if(m.type==='start'||m.type==='state'&&m.room)ws.latest=m;
      };
      ws.onclose=e=>closes.push(['lobby-close',e.code,e.reason]);
      lobbies.push(ws);await wait(()=>ws.readyState===1&&ws.latest);
    }
    const send=(i,m)=>lobbies[i].send(JSON.stringify(m));
    send(0,{type:'take-seat',seat:0,loadout:0});await wait(()=>lobbies[0].latest.room?.seats[0]);
    send(1,{type:'take-seat',seat:1,loadout:1});await wait(()=>lobbies[0].latest.room?.seats[1]);
    send(0,{type:'set-ready',ready:true});send(1,{type:'set-ready',ready:true});
    await wait(()=>lobbies[0].latest.room?.seats.slice(0,2).every(s=>s?.ready));
    send(0,{type:'start',adonisMode:1,inputDelay:4,inputDelayAuto:false,predictionReserve:2});
    await wait(()=>lobbies[0].latest.type==='start');
    for(let i=0;i<2;i++){
      const w=frames[i];
      const {SharedNetplay}=await w.eval("import('/app/shared-netplay.mjs')");
      const options={netplayUrl:address({room,run:1,players:2,player:i,member:members[i]}),
        netplayPlayer:i,netplayPlayerCount:2,netplaySeed:12345,netplayDifficulty:3,
        netplayLoadouts:[{character:0},{character:1}],netplaySpectatorCount:0,
        netplayAdaptiveTurn:!adaptiveDisabled,
        netplayAdonisMode:1,netplayInputDelay:4,netplayInputDelayAuto:false,netplayPredictionReserve:2};
      w.core.eaglerOptions=options;
      w.play=new SharedNetplay(w.core,{onStatus:s=>events.push({peer:i,at:performance.now()-began,status:s}),
        onClose:s=>closes.push([i,s]),onResult:()=>{}});
      w.core.onGameFrame=()=>{w.play.frame();const s=stats(w);
        if(s[0]&&s[0]===s[1])hashMaps[i].set(s[0],w.core._th09_network_hash()>>>0);};
      w.core._th09_loop_start();w.core._th09_loop_pause(1);await w.play.connect(options);
      const transport=w.__eaglerPeerTransport,receive=transport.receiveBinary.bind(transport);
      w.routePackets=[];
      transport.receiveBinary=data=>{
        if(!w.play.active&&w.routePackets.length<256){const bytes=new w.Uint8Array(data);
          w.routePackets.push({at:performance.now()-began,length:bytes.length,prefix:Array.from(bytes.slice(0,16))});}
        return receive(data);
      };
    }
    await wait(()=>frames.every(w=>w.play.active&&stats(w)[1]>30));
    const peers=frames.map(w=>[...w.__eaglerPeerTransport.peers.values()][0]);
    if(onDemand)assertOnDemand();
    else if(!adaptiveDisabled&&!startupOnly)await wait(()=>peers.every(p=>p.adaptive.snapshot().standbyReady));
    if(peers.some(p=>p.primary.path!=='direct'))throw Error('Initial path must be direct');
    const initialPairs=await Promise.all(peers.map(selectedPair));
    const initialFrames=frames.map(stats);
    // Give both routes a mature healthy baseline before a sustained failure.
    if(!startupOnly)await sleep(6500);
    const faultAt=performance.now(), faultFrames=frames.map(stats),initialTurnConnections=frames.map(w=>w.turnConnections);
    function assertOnDemand(){
      if(frames.some(w=>w.__eaglerPeerTransport.onDemandTurn!==true))
        throw Error('Both runtimes must negotiate on-demand TURN');
    }
    for(let i=0;i<2;i++){
      const w=frames[i],dc=peers[i].primary.inputDc;
      w.droppedRouteInputs=0;w.attemptedRouteInputs=0;
      const original=dc.send.bind(dc),started=w.performance.now();
      dc.send=data=>{
        ++w.attemptedRouteInputs;
        const age=w.performance.now()-started;
        const drop=scenario==='outage'||scenario==='bursts'&&age%20000<burstMs||
          scenario==='degraded'&&w.attemptedRouteInputs%7===0;
        if(drop){++w.droppedRouteInputs;return;}
        if(scenario==='degraded'){
          const copy=typeof data==='string'?data:data.slice();
          w.setTimeout(()=>{if(dc.readyState==='open')original(copy);},110);
        }else original(data);
      };
      w.heldRouteControls=0;
      if(controlFault){
        const control=peers[i].primary.controlDc,sendControl=control.send.bind(control),held=[];
        let flushTimer=null;
        const flush=()=>{flushTimer=null;while(held.length&&control.readyState==='open')sendControl(held.shift());};
        control.send=data=>{
          const age=w.performance.now()-started,drop=scenario==='outage'||scenario==='bursts'&&age%20000<burstMs;
          if(drop){++w.heldRouteControls;
            if(scenario==='outage')return;
            held.push(typeof data==='string'?data:data.slice());
            if(scenario==='bursts'&&flushTimer===null)flushTimer=w.setTimeout(flush,burstMs-age%20000+1);
            return;
          }
          flush();sendControl(data);
        };
      }
      w.core._th09_key(29,1); // The actual game keeps producing shoot input.
    }
    let switchedAt=null,offerAt=null,routeOffer=null,confirmedSwitch=false;
    const routeTimeline=[];
    while(performance.now()-faultAt<(startupOnly?20000:durationMs)){
      if(closes.length)throw Error(JSON.stringify(closes));
      const snapshots=peers.map(p=>p.adaptive.snapshot());
      if(scenario==='degraded'&&!confirmedSwitch){
        routeOffer=peers[0].adaptive.routeSwitchStatus();
        if(routeOffer?.to==='TURN'){
          offerAt=performance.now();
          confirmedSwitch=peers[0].adaptive.confirmOffer();
        }
      }
      routeTimeline.push({at:performance.now()-faultAt,frames:frames.map(stats),
        selected:snapshots.map(s=>s.selected)});
      if(!switchedAt&&snapshots.every(s=>s.selected==='turn'))switchedAt=performance.now();
      if(startupOnly&&[...hashMaps[0].keys()].filter(frame=>
        frame>Math.max(...faultFrames.map(s=>s[1]))&&hashMaps[1].has(frame)).length>100)break;
      if(['degraded','outage'].includes(scenario)&&switchedAt&&performance.now()-switchedAt>12000&&
        frames.every((w,i)=>stats(w)[1]>faultFrames[i][1]+300))break;
      await sleep(100);
    }
    const final=frames.map(stats),snapshots=peers.map(p=>p.adaptive.snapshot());
    const selectedPairs=await Promise.all(peers.map(selectedPair));
    const common=[...hashMaps[0]].filter(([frame])=>hashMaps[1].has(frame));
    const mismatches=common.filter(([frame,hash])=>hashMaps[1].get(frame)!==hash);
    const after=common.filter(([frame])=>frame>Math.max(...faultFrames.map(s=>s[1]))+(startupOnly?0:180));
    const result={initialFrames,faultFrames,final,snapshots,selectedPairs,initialPairs,initialTurnConnections,faultAtMs:Math.round(faultAt-began),
      routeTrace:frames.map(w=>w.routeTrace),
      totalTurnConnections:frames.map(w=>w.turnConnections),routeOffer,offerAfterMs:offerAt===null?null:offerAt-faultAt,
      switchedAfterMs:switchedAt===null?null:switchedAt-faultAt,
      comparedFrames:common.length,comparedAfterRecovery:after.length,mismatches:mismatches.slice(0,5),
      droppedInputs:frames.map(w=>w.droppedRouteInputs),attemptedInputs:frames.map(w=>w.attemptedRouteInputs),routeTimeline,events:[...events],closes:[...closes],
      heldControls:frames.map(w=>w.heldRouteControls),
      errors:frames.flatMap(w=>w.testErrors)};
    await globalThis.cleanupRouteGame();
    return result;
  },{relayUrl,scenario,burstMs,durationMs,startupOnly,adaptiveDisabled,controlFault,onDemand});
  await delivery.assertComplete();
  const routePassed=scenario==='stable' ? result.snapshots.every(s=>s.selected==='primary'&&s.epoch===0) :
    ['degraded','outage'].includes(scenario) ? result.snapshots.every(s=>s.selected==='turn')&&
      result.selectedPairs.every(p=>p.local==='relay'&&p.remote==='relay') :
      result.snapshots.every(s=>s.epoch<=1);
  const remainedDirect=scenario!=='bursts'||!onDemand||result.routeTimeline.every(row=>row.selected.every(path=>path==='primary'));
  const passed=routePassed&&remainedDirect&&result.comparedAfterRecovery>100&&result.mismatches.length===0&&
    (scenario==='stable'||result.droppedInputs.every(n=>n>10))&&
    !result.errors.length&&!result.closes.length&&!errors.length;
  writeFileSync(output,JSON.stringify({passed,at:new Date().toISOString(),wasm:actualHash,
    browser:browser.version(),scenario,burstMs,durationMs,startupOnly,adaptiveDisabled,controlFault,onDemand,runtimeDelivery:delivery.receipts,relayUrl:externalRelay||'isolated local relay',
    scope:'Two production th09 WASM games | production relay protocol | real TURN | controlled initial DataChannel impairment',
    turnScope:turnProxyTarget?'public TURN via Wi-Fi-bound UDP forwarder':localTurnUrl?'isolated local TURN':'public TURN',
    turnProxyTarget,
    result,errors},null,2)+'\n');
  assert.ok(passed,JSON.stringify({...result,routeTrace:undefined,routeTimeline:undefined,events:undefined}));
  console.log(JSON.stringify({passed,report:output,wasm:actualHash,switchedAfterMs:result.switchedAfterMs,
    comparedFrames:result.comparedFrames,comparedAfterRecovery:result.comparedAfterRecovery,final:result.final}));
} catch(error) {
  let evidence;try{evidence=await page?.evaluate(()=>globalThis.routeGameSnapshot?.()||globalThis.routeGameEvidence);}catch{}
  writeFileSync(output+'.failure.json',JSON.stringify({error:String(error.stack),errors,evidence,relayLog,runtimeDelivery:delivery?.receipts,deliveryErrors:delivery?.failures},null,2));
  throw error;
} finally {
  try{await page?.evaluate(()=>globalThis.cleanupRouteGame?.());}catch{}
  await browser?.close();relay?.kill();host.netplay.close();await new Promise(r=>host.server.close(r));
}
