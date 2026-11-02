// Production WASM, real game frames, production relay protocol and live TURN.
// Only the initial DataChannel's outbound input is impaired by this fixture.
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
const controlFault=!startupOnly&&process.env.ROUTE_CONTROL_FAULT==='1';
assert.ok(['stable','bursts','outage'].includes(scenario));
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
try {
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
  const iceServers = await page.evaluate(async origin => new Promise((accept,reject) => {
    const url = new URL('/eagler-netplay/?diagnostic=1', origin); url.protocol = 'wss:';
    const ws = new WebSocket(url);
    const timer = setTimeout(() => {ws.close();reject(Error('TURN credential timeout'));},10000);
    ws.onmessage = event => {const m=JSON.parse(event.data);if(m.type==='diagnostic-ready'){
      clearTimeout(timer);ws.close();accept(m.iceServers);}};
    ws.onerror=()=>reject(Error('TURN diagnostic failed'));
  }), process.env.TURN_ORIGIN || 'https://touhou.vip');
  const turn = iceServers.find(s => (Array.isArray(s.urls)?s.urls:[s.urls]).some(u=>/^turns?:/.test(u)));
  assert.ok(turn?.credential);
  const port = await new Promise(accept => {const s=net.createServer();s.listen(0,'127.0.0.1',()=>{
    const port=s.address().port;s.close(()=>accept(port));});});
  const externalRelay=process.env.ROUTE_RELAY_URL;
  if(!externalRelay){
   relay = spawn(process.execPath,[resolve(repository,'services/relay/src/netplay-relay.mjs')],{
    windowsHide:true,stdio:['ignore','pipe','pipe'],env:{...process.env,
      EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:String(port),
      EAGLER_NETPLAY_STUN_URLS:'',EAGLER_NETPLAY_TURN_CONFIG_FILE:'',EAGLER_NETPLAY_TURN_SHARED_SECRET:'',
      EAGLER_NETPLAY_TURN_URLS:(Array.isArray(turn.urls)?turn.urls:[turn.urls]).join(','),
      EAGLER_NETPLAY_TURN_USERNAME:turn.username,EAGLER_NETPLAY_TURN_CREDENTIAL:turn.credential,
  }});
  await new Promise((accept,reject)=>{
    relay.stdout.on('data',b=>{relayLog+=b;if(relayLog.includes('netplay relay listening'))accept();});
    relay.stderr.on('data',b=>relayLog+=b);relay.on('error',reject);
    relay.on('exit',code=>reject(Error('Relay exited '+code)));
  });
  }
  const relayUrl=externalRelay||`ws://127.0.0.1:${port}/`;
  const result = await page.evaluate(async ({relayUrl,scenario,burstMs,durationMs,startupOnly,adaptiveDisabled,controlFault}) => {
    const frames=[], lobbies=[], events=[], closes=[], hashMaps=[new Map(),new Map()];
    const began=performance.now();
    const sleep=ms=>new Promise(r=>setTimeout(r,ms));
    const wait=async condition=>{const start=performance.now();while(!condition()){
      if(closes.length)throw Error(JSON.stringify(closes));
      if(performance.now()-start>20000)throw Error('Room/start timeout');await sleep(25);}};
    const stats=w=>{const at=w.core._th09_rollback_info()/4;return Array.from(w.core.HEAPU32.subarray(at,at+8));};
    globalThis.routeGameEvidence={events,closes};
    globalThis.routeGameSnapshot=()=>({events,closes,packets:frames.map(w=>w.routePackets),
      peers:frames.map(w=>w.__eaglerPeerTransport?[...w.__eaglerPeerTransport.peers.values()].map(p=>p.adaptive.snapshot()):[])});
    // Prepare both worlds before reserving a public room. A failed resource
    // load must not leave a started run waiting for clients that never join.
    for(let i=0;i<2;i++){
      const frame=document.createElement('iframe');frame.src='/';document.body.append(frame);
      await new Promise(r=>frame.onload=r);const w=frame.contentWindow;frames.push(w);
      await w.openProbe(0,1,2,3,true);
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
      ws.onmessage=e=>{const m=JSON.parse(e.data);if(m.type==='error')closes.push(['lobby',m.error]);ws.latest=m;};
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
    if(!adaptiveDisabled&&!startupOnly)await wait(()=>peers.every(p=>p.adaptive.snapshot().standbyReady));
    if(peers.some(p=>p.primary.path!=='direct'))throw Error('Initial path must be direct');
    const initialFrames=frames.map(stats);
    // Give both routes a mature healthy baseline before a sustained failure.
    if(!startupOnly)await sleep(6500);
    const faultAt=performance.now(), faultFrames=frames.map(stats);
    for(let i=0;i<2;i++){
      const w=frames[i],dc=peers[i].primary.inputDc;
      w.droppedRouteInputs=0;w.attemptedRouteInputs=0;
      const original=dc.send.bind(dc),started=w.performance.now();
      dc.send=data=>{
        ++w.attemptedRouteInputs;
        const age=w.performance.now()-started;
        const drop=scenario==='outage'||scenario==='bursts'&&age%20000<burstMs;
        if(drop){++w.droppedRouteInputs;return;}original(data);
      };
      w.heldRouteControls=0;
      if(controlFault){
        const control=peers[i].primary.controlDc,sendControl=control.send.bind(control),held=[];
        let flushTimer=null;
        const flush=()=>{flushTimer=null;while(held.length&&control.readyState==='open')sendControl(held.shift());};
        control.send=data=>{
          const age=w.performance.now()-started,drop=scenario==='outage'||scenario==='bursts'&&age%20000<burstMs;
          if(drop){held.push(typeof data==='string'?data:data.slice());++w.heldRouteControls;
            if(scenario==='bursts'&&flushTimer===null)flushTimer=w.setTimeout(flush,burstMs-age%20000+1);
            return;
          }
          flush();sendControl(data);
        };
      }
      w.core._th09_key(29,1); // The actual game keeps producing shoot input.
    }
    let switchedAt=null;
    const routeTimeline=[];
    while(performance.now()-faultAt<(startupOnly?20000:durationMs)){
      if(closes.length)throw Error(JSON.stringify(closes));
      const snapshots=peers.map(p=>p.adaptive.snapshot());
      routeTimeline.push({at:performance.now()-faultAt,frames:frames.map(stats),
        selected:snapshots.map(s=>s.selected)});
      if(!switchedAt&&snapshots.every(s=>s.selected==='turn'))switchedAt=performance.now();
      if(startupOnly&&[...hashMaps[0].keys()].filter(frame=>
        frame>Math.max(...faultFrames.map(s=>s[1]))&&hashMaps[1].has(frame)).length>100)break;
      if(scenario==='outage'&&switchedAt&&performance.now()-switchedAt>12000&&frames.every((w,i)=>stats(w)[1]>faultFrames[i][1]+300))break;
      await sleep(100);
    }
    const final=frames.map(stats),snapshots=peers.map(p=>p.adaptive.snapshot());
    const selectedPairs=[];
    for(const peer of peers){const reports=await peer.pc.getStats();
      let pair;for(const r of reports.values())if(r.type==='transport'&&r.selectedCandidatePairId)pair=reports.get(r.selectedCandidatePairId);
      if(!pair)for(const r of reports.values())if(r.type==='candidate-pair'&&r.state==='succeeded'&&r.nominated)pair=r;
      selectedPairs.push({local:reports.get(pair?.localCandidateId)?.candidateType,remote:reports.get(pair?.remoteCandidateId)?.candidateType});
    }
    const common=[...hashMaps[0]].filter(([frame])=>hashMaps[1].has(frame));
    const mismatches=common.filter(([frame,hash])=>hashMaps[1].get(frame)!==hash);
    const after=common.filter(([frame])=>frame>Math.max(...faultFrames.map(s=>s[1]))+(startupOnly?0:180));
    const result={initialFrames,faultFrames,final,snapshots,selectedPairs,
      switchedAfterMs:switchedAt===null?null:switchedAt-faultAt,
      comparedFrames:common.length,comparedAfterRecovery:after.length,mismatches:mismatches.slice(0,5),
      droppedInputs:frames.map(w=>w.droppedRouteInputs),attemptedInputs:frames.map(w=>w.attemptedRouteInputs),routeTimeline,events:[...events],closes:[...closes],
      heldControls:frames.map(w=>w.heldRouteControls),
      errors:frames.flatMap(w=>w.testErrors)};
    await globalThis.cleanupRouteGame();
    return result;
  },{relayUrl,scenario,burstMs,durationMs,startupOnly,adaptiveDisabled,controlFault});
  await delivery.assertComplete();
  const routePassed=scenario==='stable' ? result.snapshots.every(s=>s.selected==='primary'&&s.epoch===0) :
    scenario==='outage' ? result.snapshots.every(s=>s.selected==='turn')&&
      result.selectedPairs.every(p=>p.local==='relay'&&p.remote==='relay') :
      result.snapshots.every(s=>s.epoch<=1);
  const passed=routePassed&&result.comparedAfterRecovery>100&&result.mismatches.length===0&&
    (scenario==='stable'||result.droppedInputs.every(n=>n>10))&&
    !result.errors.length&&!result.closes.length&&!errors.length;
  writeFileSync(output,JSON.stringify({passed,at:new Date().toISOString(),wasm:actualHash,
    browser:browser.version(),scenario,burstMs,durationMs,startupOnly,adaptiveDisabled,controlFault,runtimeDelivery:delivery.receipts,relayUrl:externalRelay||'isolated local relay',
    scope:'Two production TH09 WASM games | production relay protocol | live TURN | controlled initial input impairment',
    result,errors},null,2)+'\n');
  assert.ok(passed,JSON.stringify({...result,routeTimeline:undefined,events:undefined}));
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
