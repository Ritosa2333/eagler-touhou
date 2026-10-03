import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import net from 'node:net';
import {readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const root=fileURLToPath(new URL('../../',import.meta.url));
const fallback=process.argv.includes('--relay');
const release=process.argv.includes('--release');
const hardware=process.env.NATIVE_GPU==='1';
const adonisMode=Number(process.env.ADONIS_MODE||0),inputDelay=Number(process.env.INPUT_DELAY_FRAMES||0);
const automatic=process.env.INPUT_DELAY_AUTO==='1',predictionReserve=Number(process.env.PREDICTION_RESERVE||2);
const startupImpairment=process.env.STARTUP_IMPAIRMENT!=='0';
const spectatorFault=process.env.SPECTATOR_FAULT||'';
assert.ok(['','backpressure','disconnect'].includes(spectatorFault));
assert.ok(!(fallback&&spectatorFault==='disconnect'),'relay is required for gameplay in fallback mode');
assert.ok(!automatic||adonisMode>0);assert.ok([1,2].includes(predictionReserve));
assert.ok(Number.isInteger(adonisMode)&&adonisMode>=0&&adonisMode<=2);
assert.ok(Number.isInteger(inputDelay)&&inputDelay>=0&&inputDelay<=9);
const fullSpriteGeometry=process.env.FULL_SPRITE_GEOMETRY==='1';
if(release&&fullSpriteGeometry)throw Error('Draw comparison control requires diagnostic WASM');
const artifactDirectory=process.env.PC_BUILD?resolve(process.env.PC_BUILD):undefined;
const label=process.env.RUN_LABEL||`transport-${fallback?'relay':'rtc'}${release?'-release':''}`;
assert.match(label,/^[\w-]+$/);
const build=JSON.parse(readFileSync(artifactDirectory?resolve(artifactDirectory,'build.json'):resolve(root,release?'artifacts/sdl-release/build.json':'artifacts/sdl3/build.json')));
const port=await new Promise(r=>{const s=net.createServer();s.listen(0,'127.0.0.1',()=>{const p=s.address().port;s.close(()=>r(p));});});
const relayPath=process.env.EAGLER_RELAY_SOURCE||'D:/workspace/eagler/eagler-touhou/server/netplay-relay.mjs';
const relayTiming=process.env.TH09_RELAY_TIMING==='1';
const relayArgs=relayTiming?['--import',new URL('./relay-timing-observer.mjs',import.meta.url).href,relayPath]:[relayPath];
const relay=spawn(process.execPath,relayArgs,{windowsHide:true,env:{...process.env,TH09_OBSERVED_RELAY:relayPath,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:String(port),EAGLER_NETPLAY_STUN_URLS:''},stdio:['ignore','pipe','pipe']});
let relayLog='';relay.stderr.on('data',b=>relayLog+=b);
const relaySamples=()=>relayLog.split('\n').slice(0,-1).filter(line=>line.startsWith('TH09_RELAY_TIMING ')).map(line=>JSON.parse(line.slice(18)));
await new Promise((accept,reject)=>{relay.stdout.on('data',b=>{relayLog+=b;if(relayLog.includes('netplay relay listening'))accept();});relay.on('error',reject);relay.on('exit',code=>reject(Error('relay exited '+code+' '+relayLog)));});
const {server,netplay,url}=await presentationServer(0,{release,artifactDirectory});
const browser=await launchBrowser({args:[...(hardware?['--enable-gpu','--use-gl=angle','--use-angle=d3d11']:['--enable-unsafe-swiftshader']),'--autoplay-policy=no-user-gesture-required']});
const page=await browser.newPage(),errors=[];page.on('pageerror',e=>errors.push(e.stack));
const resourceErrors=[];
page.on('requestfailed',r=>{if(resourceErrors.length<64)resourceErrors.push({url:r.url(),failure:r.failure()});});
page.on('response',r=>{if(r.status()>=400&&resourceErrors.length<64)resourceErrors.push({url:r.url(),status:r.status()});});
if(release)assert.equal(build.exports.some(e=>e.name.startsWith('th09_probe_')),false);

try{
 await page.goto(url);
 const result=await page.evaluate(async ({port,fallback,release,fullSpriteGeometry,adonisMode,inputDelay,automatic,predictionReserve,startupImpairment,spectatorFault})=>{
  const frames=[],notices=[],closed=[],lobbies=[],timeline=[],lanes=[],phaseEvents=[];
  const diagnosticStarted=performance.now();
  let lastDiagnostic=diagnosticStarted;
  const readStats=w=>{const at=w.core._th09_rollback_info()/4;return Array.from(w.core.HEAPU32.subarray(at,at+8));};
  const diagnostics=()=>({
   scope:'Bounded test-only wall-clock and transport counters; not CPU/GPU or input-to-photon timing',
   wallStartedMs:performance.timeOrigin+diagnosticStarted,
   timeline:timeline.slice(),
   phaseEvents:phaseEvents.slice(),
   lanes:lanes.map(({channel,...entry})=>({...entry,readyState:channel.readyState,bufferedAmount:channel.bufferedAmount})),
  });
  // Keep evidence available even when the in-page correctness gate throws.
  globalThis.__th09TransportDiagnostics=diagnostics;
  const sampleDiagnostic=()=>{
   const now=performance.now();if(now-lastDiagnostic<100)return;
   const gapMs=now-lastDiagnostic;lastDiagnostic=now;
   if(timeline.length===600)timeline.shift();
   timeline.push({atMs:now-diagnosticStarted,pollGapMs:gapMs,
    peers:frames.map((w,i)=>({
     frame:i===2?w.core._th09_spectator_frame():readStats(w)[0],
     confirmed:i===2?w.core._th09_spectator_frame():readStats(w)[1],
     sinceCallbackMs:w.measure?.callbackAt?now-w.measure.callbackAt:null,
     route:w.__eaglerNetplayTransport,
     spectatorSent:w.play?.spectatorSentFrame??0,
     spectatorQueued:w.play?w.play.spectatorPending.length-w.play.spectatorHead:0,
     spectatorStopped:!!w.play?.spectatorOutputStopped,
     spectatorReceived:w.play?.spectatorFrame??0,
     transportQueued:w.__eaglerPeerTransport? w.__eaglerPeerTransport.received.length-w.__eaglerPeerTransport.receivedHead:0,
     relayBuffered:w.__eaglerPeerTransport?.relay?.bufferedAmount??null,
    })),
    lanes:lanes.map(({channel,peer,lane,sent,received,pending,lastSendAt,lastReceiveAt,maxTimerLatenessMs})=>({
     peer,lane,sent,received,pending,maxTimerLatenessMs,
     sendAgeMs:lastSendAt===null?null:now-lastSendAt,
     receiveAgeMs:lastReceiveAt===null?null:now-lastReceiveAt,
     bufferedAmount:channel.bufferedAmount,readyState:channel.readyState,
    })),
   });
  };
  const wait=async condition=>{const began=performance.now();while(!condition()){if(performance.now()-began>10000)throw Error('lobby timeout');await new Promise(r=>setTimeout(r,10));}};
  for(const id of ['rollbackleft','rollbackright','rollbacktest']){const ws=new WebSocket(`ws://127.0.0.1:${port}/?room=th09mp-0999&lobby=${id}`);ws.onmessage=e=>{const m=JSON.parse(e.data);if(m.type==='error')throw Error(m.error);ws.latest=m;};lobbies.push(ws);await wait(()=>ws.readyState===WebSocket.OPEN&&ws.latest);}
  const send=(i,m)=>lobbies[i].send(JSON.stringify(m));
  send(0,{type:'take-seat',seat:0,loadout:0});await wait(()=>lobbies[0].latest.room?.seats[0]);
  send(1,{type:'take-seat',seat:1,loadout:1});await wait(()=>lobbies[0].latest.room?.seats[1]);
  send(2,{type:'spectate'});await wait(()=>lobbies[0].latest.room?.spectatorCount===1);
  send(0,{type:'set-ready',ready:true});send(1,{type:'set-ready',ready:true});await wait(()=>lobbies[0].latest.room?.seats.slice(0,2).every(s=>s?.ready));
  send(0,{type:'start',adonisMode,inputDelay:automatic?0:inputDelay,inputDelayAuto:automatic,predictionReserve});await wait(()=>lobbies[0].latest.type==='start');
  if(adonisMode&&(lobbies[0].latest.room.adonisMode!==adonisMode||lobbies[0].latest.room.inputDelay!==(automatic?0:inputDelay)||lobbies[0].latest.room.inputDelayAuto!==automatic||lobbies[0].latest.room.predictionReserve!==predictionReserve))
    throw Error('Experimental timing requires the matching experiment/adonis launcher relay');
  for(let i=0;i<3;++i){const f=document.createElement('iframe');f.src='/';document.body.append(f);await new Promise(r=>f.onload=r);frames.push(f.contentWindow);}
  for(const w of frames){
   await w.openProbe(0,1,2,3,true);if(fallback)w.RTCPeerConnection=undefined;
   if(fullSpriteGeometry){if(!w.core._th09_probe_draw_mode)throw Error('Missing Draw comparison control');w.core._th09_probe_draw_mode(1);}
   // Install before connect, on the actual transport prototypes. A clean
   // startup ping followed by an impaired game would NOT test automatic D.
   w.startupWire={attempted:0,sent:0,dropped:0,pending:0,lanes:[]};
   for(const [name,prototype] of [['relay',w.WebSocket?.prototype],['rtc',w.RTCDataChannel?.prototype]]){
    if(!prototype)continue;
    const original=prototype.send;
    prototype.send=function(data){
     if(typeof data==='string')return original.call(this,data);
     const b=new Uint8Array(data),offset=b[0]===0xe7?2:0;
     if(b[offset]!==65||b[offset+1]!==68||b[offset+2]!==83||![2,3].includes(b[offset+4]))return original.call(this,data);
     const s=w.startupWire;++s.attempted;if(!s.lanes.includes(name))s.lanes.push(name);
     if(startupImpairment&&s.attempted%31===0){++s.dropped;return;}
     const channel=this,copy=new Uint8Array(b),delay=startupImpairment?25+b[offset+28]%37:0;++s.pending;
     w.setTimeout(()=>{--s.pending;if(channel.readyState==='open'||channel.readyState===1){original.call(channel,copy);++s.sent;}},delay);
    };
   }
  }
  const limit=600;
  for(let i=0;i<3;++i){const w=frames[i],{SharedNetplay}=await w.eval("import('/app/shared-netplay.mjs')");
   const spectator=i===2,options={netplayUrl:`ws://127.0.0.1:${port}/?room=th09mp-0999&run=1&players=2&${spectator?'spectator=rollbacktest':'player='+i}`,netplayPlayer:i%2,netplayPlayerCount:2,netplaySeed:12345,netplayDifficulty:3,netplayLoadouts:[{character:0},{character:1}],netplaySpectator:spectator,netplaySpectatorId:spectator?'rollbacktest':'',netplaySpectatorCount:1,netplayIceServers:[]};
   options.netplayAdonisMode=adonisMode;options.netplayInputDelay=automatic?0:inputDelay;
   options.netplayInputDelayAuto=automatic;options.netplayPredictionReserve=predictionReserve;
   w.core.eaglerOptions=options;w.play=new SharedNetplay(w.core,{onStatus:s=>{
    notices.push([i,s]);if(phaseEvents.length<64)phaseEvents.push({peer:i,atMs:performance.now()-diagnosticStarted,status:s});
   },onTiming:t=>{if(i===0&&t.phase==='ready')send(0,{type:'timing-result',serial:1,timing:t});},onClose:s=>{w.core._th09_loop_pause(1);closed.push([i,s]);},onResult:()=>{}});
   w.measure={work:[],gaps:[],last:0,presented:0,audioStart:0};
   w.core.onNetworkSpectatorFrame=(...args)=>w.play.publish(...args);w.core.onNetworkResult=()=>w.play.result();w.core.onGameFrame=(ok,ms)=>{w.play.frame();w.measure.callbackAt=performance.now();if(!w.measure.enabled)return;const m=w.measure,at=w.core._th09_game_metrics()/4,n=w.core.HEAPU32[at+9];m.work.push(ms);if(n!==m.presented){const now=performance.now();if(m.last)m.gaps.push(now-m.last);m.last=now;m.presented=n;}};
   w.core._th09_probe_frame_limit?.(limit);w.core._th09_loop_start();w.core._th09_loop_pause(1);await w.play.connect(options);
  }
  const stats=readStats;
  const started=performance.now();lastDiagnostic=started;let wrapped=false,gameplayStarted=null,faultInjected=false;
  while(performance.now()-started<45000){
   sampleDiagnostic();
   if(closed.some(([i])=>spectatorFault!=='disconnect'||i!==2))throw Error(JSON.stringify({closed,notices}));
   if(!wrapped&&frames.slice(0,2).every(w=>w.play.active)){
    wrapped=true;gameplayStarted=performance.now();
    phaseEvents.push({atMs:gameplayStarted-diagnosticStarted,status:'Both players active; fault injection and work measurements begin'});
    for(const w of frames){w.measure.enabled=true;w.measure.audioStart=w.core.SDL3?.audioContext?.currentTime||0;}
    for(const w of frames.slice(0,2)){w.core._th09_key(29,1);w.core._th09_key(44,1);const began=performance.now();
     const channels=fallback?[['relay',w.__eaglerPeerTransport.relay]]:[...w.__eaglerPeerTransport.peers.values()].flatMap(peer=>[['input',peer.inputDc],['control',peer.controlDc]]);
     for(const [lane,channel] of channels){
      const send=channel.send.bind(channel);let count=0;
      const counters={channel,peer:frames.indexOf(w),lane,attempted:0,sent:0,received:0,dropped:0,pending:0,
       lastSendAt:null,lastReceiveAt:null,maxTimerLatenessMs:0};lanes.push(counters);
      channel.addEventListener('message',()=>{++counters.received;counters.lastReceiveAt=performance.now();});
      const sendObserved=bytes=>{send(bytes);++counters.sent;counters.lastSendAt=performance.now();};
      channel.send=bytes=>{++counters.attempted;if(typeof bytes==='string'){sendObserved(bytes);return;}const copy=new Uint8Array(bytes),offset=copy[0]===0xe7?2:0;if(copy[offset]!==69||copy[offset+5]!==1){sendObserved(bytes);return;}const n=++count;if(lane==='input'&&n%9===0){++counters.dropped;return;}
       const age=performance.now()-began,delay=25+n%37+(age>=2500&&age<2900?800:0),due=performance.now()+delay;++counters.pending;
       setTimeout(()=>{--counters.pending;counters.maxTimerLatenessMs=Math.max(counters.maxTimerLatenessMs,performance.now()-due);if(channel.readyState==='open'||channel.readyState===WebSocket.OPEN)sendObserved(copy);},delay);};
     }
    }
   }
   if(wrapped&&spectatorFault&&!faultInjected&&performance.now()-gameplayStarted>=1500){
    faultInjected=true;const host=frames[0],relay=host.__eaglerPeerTransport.relay;
    if(spectatorFault==='disconnect')relay.close(1000,'test-only optional spectator upload loss');
    else {
     const get=Object.getOwnPropertyDescriptor(host.WebSocket.prototype,'bufferedAmount').get;
     const until=performance.now()+1200;
     Object.defineProperty(relay,'bufferedAmount',{configurable:true,get(){
      const actual=get.call(this);return performance.now()<until?Math.max(65536,actual):actual;
     }});
    }
   }
   if(frames.slice(0,2).every(w=>stats(w)[1]>=limit)&&
      (spectatorFault==='disconnect'?closed.some(([i])=>i===2):frames[2].core._th09_spectator_frame()>=limit))break;
   await new Promise(r=>setTimeout(r,25));
  }
  for(const w of frames)w.core._th09_loop_pause(1);
  const summary=frames.slice(0,2).map(stats),spectator=frames[2].core._th09_spectator_frame();
  const viewerStopped=spectatorFault==='disconnect';
  if(!summary.every(s=>(release?s[0]>=limit&&s[1]>=limit:s[0]===limit&&s[1]===limit&&s[4]===limit)&&(adonisMode===1?s[2]===0&&s[3]===0:s[2]>0))||
     (!viewerStopped&&(release?spectator<limit:spectator!==limit)))throw Error('incomplete '+JSON.stringify({summary,spectator,notices,closed}));
  if(spectatorFault&&!faultInjected)throw Error('spectator fault was not exercised');
  if(viewerStopped&&(!frames[0].play.spectatorOutputStopped||!closed.some(([i,s])=>i===2&&s.includes('spectator stream stopped'))))
    throw Error('spectator failure did not explicitly stop only the viewer');
  if(spectatorFault==='backpressure'&&!timeline.some(t=>t.peers[0].spectatorQueued>=16))throw Error('backpressure did not retain ordered input');
  const viewerAt=frames[2].core._th09_spectator_info()/4;
  const spectatorBudget=Array.from(frames[2].core.HEAPU32.subarray(viewerAt,viewerAt+8));
  if(spectatorBudget[0]!==1||spectatorBudget[5]>6)throw Error('spectator exceeded callback tick budget');
  const adonis=frames.slice(0,2).map(w=>{const at=w.core._th09_adonis_info()/4;return Array.from(w.core.HEAPU32.subarray(at,at+6));});
  const startup=frames.slice(0,2).map(w=>{if(!w.core._th09_startup_info)return [];const at=w.core._th09_startup_info()/4;return Array.from(w.core.HEAPU32.subarray(at,at+16));});
  const timings=frames.map(w=>w.play.timing),startupWire=frames.slice(0,2).map(w=>w.startupWire);
  const resolvedDelay=adonisMode?startup[0][10]:inputDelay;
  if(adonis.some(v=>v[0]!==adonisMode||v[1]!==resolvedDelay||(adonisMode===1&&v[2]!==0)))throw Error('wrong timing mode '+JSON.stringify(adonis));
  if(adonisMode){
   if(startup.some(s=>s[0]!==2||s[1]!==4||s[3]!==129||s[14]<96||s[15]<96||s[10]!==resolvedDelay||s[11]!==(adonisMode===2?Math.min(Math.max(0,s[9]-(automatic?1:0)),predictionReserve):0)))throw Error('incomplete native calibration '+JSON.stringify(startup));
   if(startupWire.some(w=>w.attempted<200||w.sent<190||!w.lanes.includes(fallback?'relay':'rtc')))throw Error('actual input-channel calibration not exercised');
   if(timings.some(t=>!t||t.inputDelay!==resolvedDelay||t.automatic!==automatic||t.fullDelay!==Math.max(1,Math.ceil(Math.floor(t.rttP95Us/2)*60/1_000_000))||
      (automatic?t.inputDelay!==Math.max(0,t.fullDelay-t.predictionReserve):t.inputDelay!==inputDelay)))throw Error('chosen D does not match policy '+JSON.stringify(timings));
   await wait(()=>lobbies[2].latest.room?.timing?.inputDelay===resolvedDelay);
   if(frames[2].__eaglerNetplayInputDelayFrames!==0)throw Error('spectator double delay');
  }
  const comparableFinalHashes=summary[0][0]===summary[1][0]&&summary[0][0]===spectator;
  const comparedWorlds=viewerStopped?frames.slice(0,2):frames;
  const hashes=comparedWorlds.map(w=>w.core._th09_network_hash()>>>0);if((!release||comparableFinalHashes)&&new Set(hashes).size!==1)throw Error('spectator divergence '+JSON.stringify(hashes));
  let replayBytes=null;
  if(!release){
   const replays=comparedWorlds.map(w=>{if(!w.core._th09_probe_save_replay())throw Error('Replay export failed after measured startup');return w.core.FS.readFile('/save/replay/th9_25.rpy');});
   if(replays.slice(1).some(b=>b.length!==replays[0].length||b.some((v,i)=>v!==replays[0][i])))throw Error('Measured startup Replay differs between players and confirmed spectator');
   replayBytes=replays[0].length;
  }
  const routes=frames.map(w=>w.__eaglerNetplayTransport),audio=frames.map(w=>({state:w.core.SDL3?.audioContext?.state,advancedSeconds:(w.core.SDL3?.audioContext?.currentTime||0)-w.measure.audioStart}));
  const elapsedMs=performance.now()-started;
  const setupMs=gameplayStarted===null?elapsedMs:gameplayStarted-started;
  const gameplayMs=gameplayStarted===null?null:performance.now()-gameplayStarted;
  // The 600-tick workload should take about ten seconds plus the injected
  // outage. A recovery pass must not spend an extra wall-clock tick per rewind.
  const sustainedCadencePassed=elapsedMs<=14000;
  const distribution=values=>{const a=values.slice().sort((x,y)=>x-y);return {count:a.length,total:a.reduce((sum,v)=>sum+v,0),p50:a[Math.floor(a.length*.5)],p95:a[Math.floor(a.length*.95)],p99:a[Math.floor(a.length*.99)],max:a.at(-1),over50ms:a.filter(v=>v>50).length};};
  const measured=frames.map(w=>({callbackWorkMs:distribution(w.measure.work),presentationGapMs:distribution(w.measure.gaps),wasmMemoryBytes:w.core.HEAPU8.length}));
  if(audio.slice(0,viewerStopped?2:3).some(a=>a.state!=='running'||a.advancedSeconds<1))throw Error('audio clock did not progress');
  const renderers=frames.map(w=>{const canvas=w.document.querySelector('canvas'),gl=canvas.getContext('webgl2')||canvas.getContext('webgl'),ext=gl.getExtension('WEBGL_debug_renderer_info');return ext?gl.getParameter(ext.UNMASKED_RENDERER_WEBGL):gl.getParameter(gl.RENDERER);});
  sampleDiagnostic();const transportDiagnostics=diagnostics();
  const calibratedBudgetPassed=gameplayMs!==null&&gameplayMs<=14000&&setupMs<=15000;
  for(const w of frames)w.play.close();for(const ws of lobbies)ws.close();return {summary,adonis,startup,startupWire,timings,resolvedDelay,spectator,spectatorBudget,spectatorFault,faultInjected,viewerStopped,hashes,replayBytes,comparableFinalHashes,routes,audio,elapsedMs,setupMs,gameplayMs,sustainedCadencePassed,calibratedBudgetPassed,gameplayCadencePassed:gameplayMs!==null&&gameplayMs<=14000,measured,renderers,notices,transportDiagnostics};
 },{port,fallback,release,fullSpriteGeometry,adonisMode,inputDelay,automatic,predictionReserve,startupImpairment,spectatorFault});
 assert.deepEqual(result.routes,[fallback?'relay':'rtc',fallback?'relay':'rtc','spectator']);assert.equal(errors.length,0,errors.join('\n'));
 if(hardware)for(const renderer of result.renderers)assert.doesNotMatch(renderer,/swiftshader|llvmpipe|microsoft basic render/i);
 const wasm=build.sha256;
 const relayTimeline=relaySamples();
 // RTC gameplay need not produce bidirectional binary traffic on one relay
 // socket: P1 uploads spectator frames, while the viewer only receives them.
 // Check the two observed directions across the participating sockets.
 const relayObservationPassed=!relayTiming ||
   (relayTimeline.some(s=>s.sockets.some(v=>v.received>0))&&relayTimeline.some(s=>s.sockets.some(v=>v.completed>0)));
 // Mandatory calibration is an explicit new phase. Retain the OLD combined
 // 14s boolean; new acceptance separates <=15s startup from the unchanged
 // <=14s gameplay budget. Neither aggregate gate means stutter-free.
 const timingPassed=adonisMode?result.calibratedBudgetPassed:result.sustainedCadencePassed;
 const passed=timingPassed&&relayObservationPassed;
 writeFileSync(resolve(root,`artifacts/multiplayer-tests/${label}-report.json`),JSON.stringify({passed,correctnessPassed:true,timingPassed,relayObservationPassed,wasm,browser:browser.version(),hardwareRequested:hardware,fullSpriteGeometry,parameters:{adonisMode,inputDelay,automatic,predictionReserve,startupImpairment,relayTiming},scope:'Local actual input-channel calibration, C++ timing mode, SharedNetplay, launcher relay result and confirmed spectator; desktop Chromium'+(release?'; production WASM smoke, no forced frame limit':''),result,errors,relayTimeline,relayLog},null,2));console.log(JSON.stringify(result));
 assert.ok(relayObservationPassed,'Relay observation was not exercised');
 assert.ok(timingPassed,`Timing budget exceeded: total=${result.elapsedMs} setup=${result.setupMs} gameplay=${result.gameplayMs} ms`);
}catch(error){
 let transportDiagnostics=null;
 try{transportDiagnostics=await page.evaluate(()=>globalThis.__th09TransportDiagnostics?.()??null);}catch{}
 writeFileSync(resolve(root,`artifacts/multiplayer-tests/${label}-failure.json`),JSON.stringify({error:error.stack,wasm:build.sha256,parameters:{adonisMode,inputDelay,automatic,predictionReserve,startupImpairment,relayTiming},errors,resourceErrors,relayLog,transportDiagnostics},null,2));throw error;
}
finally{await browser.close();netplay.close();await new Promise(r=>server.close(r));relay.kill();}
