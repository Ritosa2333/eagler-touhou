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
const relay=spawn(process.execPath,[relayPath],{windowsHide:true,env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:String(port),EAGLER_NETPLAY_STUN_URLS:''},stdio:['ignore','pipe','pipe']});
let relayLog='';relay.stderr.on('data',b=>relayLog+=b);
await new Promise((accept,reject)=>{relay.stdout.on('data',b=>{relayLog+=b;if(relayLog.includes('netplay relay listening'))accept();});relay.on('error',reject);relay.on('exit',code=>reject(Error('relay exited '+code+' '+relayLog)));});
const {server,netplay,url}=await presentationServer(0,{release,artifactDirectory});
const browser=await launchBrowser({args:[...(hardware?['--enable-gpu','--use-gl=angle','--use-angle=d3d11']:['--enable-unsafe-swiftshader']),'--autoplay-policy=no-user-gesture-required']});
const page=await browser.newPage(),errors=[];page.on('pageerror',e=>errors.push(e.stack));
if(release)assert.equal(build.exports.some(e=>e.name.startsWith('th09_probe_')),false);

try{
 await page.goto(url);
 const result=await page.evaluate(async ({port,fallback,release,fullSpriteGeometry,adonisMode,inputDelay})=>{
  const frames=[],notices=[],closed=[],lobbies=[];
  const wait=async condition=>{const began=performance.now();while(!condition()){if(performance.now()-began>10000)throw Error('lobby timeout');await new Promise(r=>setTimeout(r,10));}};
  for(const id of ['rollbackleft','rollbackright','rollbacktest']){const ws=new WebSocket(`ws://127.0.0.1:${port}/?room=th09mp-0999&lobby=${id}`);ws.onmessage=e=>{const m=JSON.parse(e.data);if(m.type==='error')throw Error(m.error);ws.latest=m;};lobbies.push(ws);await wait(()=>ws.readyState===WebSocket.OPEN&&ws.latest);}
  const send=(i,m)=>lobbies[i].send(JSON.stringify(m));
  send(0,{type:'take-seat',seat:0,loadout:0});await wait(()=>lobbies[0].latest.room?.seats[0]);
  send(1,{type:'take-seat',seat:1,loadout:1});await wait(()=>lobbies[0].latest.room?.seats[1]);
  send(2,{type:'spectate'});await wait(()=>lobbies[0].latest.room?.spectatorCount===1);
  send(0,{type:'set-ready',ready:true});send(1,{type:'set-ready',ready:true});await wait(()=>lobbies[0].latest.room?.seats.slice(0,2).every(s=>s?.ready));
  send(0,{type:'start',adonisMode,inputDelay});await wait(()=>lobbies[0].latest.type==='start');
  if(adonisMode&&(lobbies[0].latest.room.adonisMode!==adonisMode||lobbies[0].latest.room.inputDelay!==inputDelay))
    throw Error('Experimental timing requires the matching experiment/adonis launcher relay');
  for(let i=0;i<3;++i){const f=document.createElement('iframe');f.src='/';document.body.append(f);await new Promise(r=>f.onload=r);frames.push(f.contentWindow);}
  for(const w of frames){await w.openProbe(0,1,2,3,true);if(fallback)w.RTCPeerConnection=undefined;if(fullSpriteGeometry){if(!w.core._th09_probe_draw_mode)throw Error('Missing Draw comparison control');w.core._th09_probe_draw_mode(1);}}
  const limit=600;
  for(let i=0;i<3;++i){const w=frames[i],{SharedNetplay}=await w.eval("import('/app/shared-netplay.mjs')");
   const spectator=i===2,options={netplayUrl:`ws://127.0.0.1:${port}/?room=th09mp-0999&run=1&players=2&${spectator?'spectator=rollbacktest':'player='+i}`,netplayPlayer:i%2,netplayPlayerCount:2,netplaySeed:12345,netplayDifficulty:3,netplayLoadouts:[{character:0},{character:1}],netplaySpectator:spectator,netplaySpectatorId:spectator?'rollbacktest':'',netplaySpectatorCount:1,netplayIceServers:[]};
   options.netplayAdonisMode=adonisMode;options.netplayInputDelay=inputDelay;
   w.core.eaglerOptions=options;w.play=new SharedNetplay(w.core,{onStatus:s=>notices.push([i,s]),onClose:s=>closed.push([i,s]),onResult:()=>{}});
   w.measure={work:[],gaps:[],last:0,presented:0,audioStart:0};
   w.core.onNetworkSpectatorFrame=(...args)=>w.play.publish(...args);w.core.onNetworkResult=()=>w.play.result();w.core.onGameFrame=(ok,ms)=>{w.play.frame();if(!w.measure.enabled)return;const m=w.measure,at=w.core._th09_game_metrics()/4,n=w.core.HEAPU32[at+9];m.work.push(ms);if(n!==m.presented){const now=performance.now();if(m.last)m.gaps.push(now-m.last);m.last=now;m.presented=n;}};
   w.core._th09_probe_frame_limit?.(limit);w.core._th09_loop_start();w.core._th09_loop_pause(1);await w.play.connect(options);
  }
  const stats=w=>{const at=w.core._th09_rollback_info()/4;return Array.from(w.core.HEAPU32.subarray(at,at+8));};
  const started=performance.now();let wrapped=false;
  while(performance.now()-started<45000){
   if(closed.length)throw Error(JSON.stringify({closed,notices}));
   if(!wrapped&&frames.slice(0,2).every(w=>w.play.active)){
    wrapped=true;
    for(const w of frames){w.measure.enabled=true;w.measure.audioStart=w.core.SDL3?.audioContext?.currentTime||0;}
    for(const w of frames.slice(0,2)){w.core._th09_key(29,1);w.core._th09_key(44,1);const began=performance.now();
     const channels=fallback?[['relay',w.__eaglerPeerTransport.relay]]:[...w.__eaglerPeerTransport.peers.values()].flatMap(peer=>[['input',peer.inputDc],['control',peer.controlDc]]);
     for(const [lane,channel] of channels){
      const send=channel.send.bind(channel);let count=0;
      channel.send=bytes=>{if(typeof bytes==='string'){send(bytes);return;}const copy=new Uint8Array(bytes),offset=copy[0]===0xe7?2:0;if(copy[offset]!==69||copy[offset+5]!==1){send(bytes);return;}const n=++count;if(lane==='input'&&n%9===0)return;
       const age=performance.now()-began,delay=25+n%37+(age>=2500&&age<2900?800:0);setTimeout(()=>{if(channel.readyState==='open'||channel.readyState===WebSocket.OPEN)send(copy);},delay);};
     }
    }
   }
   if(frames.slice(0,2).every(w=>stats(w)[1]>=limit)&&frames[2].core._th09_spectator_frame()>=limit)break;
   await new Promise(r=>setTimeout(r,25));
  }
  for(const w of frames)w.core._th09_loop_pause(1);
  const summary=frames.slice(0,2).map(stats),spectator=frames[2].core._th09_spectator_frame();
  if(!summary.every(s=>(release?s[0]>=limit&&s[1]>=limit:s[0]===limit&&s[1]===limit&&s[4]===limit)&&(adonisMode===1?s[2]===0&&s[3]===0:s[2]>0))|| (release?spectator<limit:spectator!==limit))throw Error('incomplete '+JSON.stringify({summary,spectator,notices,closed}));
  const adonis=frames.slice(0,2).map(w=>{const at=w.core._th09_adonis_info()/4;return Array.from(w.core.HEAPU32.subarray(at,at+6));});
  if(adonis.some(v=>v[0]!==adonisMode||v[1]!==inputDelay||(adonisMode===1&&v[2]!==0)))throw Error('wrong timing mode '+JSON.stringify(adonis));
  const comparableFinalHashes=summary[0][0]===summary[1][0]&&summary[0][0]===spectator;
  const hashes=frames.map(w=>w.core._th09_network_hash()>>>0);if((!release||comparableFinalHashes)&&new Set(hashes).size!==1)throw Error('spectator divergence '+JSON.stringify(hashes));
  const routes=frames.map(w=>w.__eaglerNetplayTransport),audio=frames.map(w=>({state:w.core.SDL3?.audioContext?.state,advancedSeconds:(w.core.SDL3?.audioContext?.currentTime||0)-w.measure.audioStart}));
  const elapsedMs=performance.now()-started;
  // The 600-tick workload should take about ten seconds plus the injected
  // outage. A recovery pass must not spend an extra wall-clock tick per rewind.
  const sustainedCadencePassed=elapsedMs<=14000;
  const distribution=values=>{const a=values.slice().sort((x,y)=>x-y);return {count:a.length,total:a.reduce((sum,v)=>sum+v,0),p50:a[Math.floor(a.length*.5)],p95:a[Math.floor(a.length*.95)],p99:a[Math.floor(a.length*.99)],max:a.at(-1),over50ms:a.filter(v=>v>50).length};};
  const measured=frames.map(w=>({callbackWorkMs:distribution(w.measure.work),presentationGapMs:distribution(w.measure.gaps),wasmMemoryBytes:w.core.HEAPU8.length}));
  if(audio.some(a=>a.state!=='running'||a.advancedSeconds<1))throw Error('audio clock did not progress');
  const renderers=frames.map(w=>{const canvas=w.document.querySelector('canvas'),gl=canvas.getContext('webgl2')||canvas.getContext('webgl'),ext=gl.getExtension('WEBGL_debug_renderer_info');return ext?gl.getParameter(ext.UNMASKED_RENDERER_WEBGL):gl.getParameter(gl.RENDERER);});
  for(const w of frames)w.play.close();for(const ws of lobbies)ws.close();return {summary,adonis,spectator,hashes,comparableFinalHashes,routes,audio,elapsedMs,sustainedCadencePassed,measured,renderers,notices};
 },{port,fallback,release,fullSpriteGeometry,adonisMode,inputDelay});
 assert.deepEqual(result.routes,[fallback?'relay':'rtc',fallback?'relay':'rtc','spectator']);assert.equal(errors.length,0,errors.join('\n'));
 if(hardware)for(const renderer of result.renderers)assert.doesNotMatch(renderer,/swiftshader|llvmpipe|microsoft basic render/i);
 const wasm=build.sha256;
 // Save negative timing evidence as well as success. The original 14-second
 // cadence gate still fails; its metrics must not disappear with the throw.
 writeFileSync(resolve(root,`artifacts/multiplayer-tests/${label}-report.json`),JSON.stringify({passed:result.sustainedCadencePassed,correctnessPassed:true,wasm,browser:browser.version(),hardwareRequested:hardware,fullSpriteGeometry,parameters:{adonisMode,inputDelay},scope:'Local real transport, C++ timing mode, SharedNetplay, launcher relay, confirmed spectator; desktop Chromium'+(release?'; production WASM smoke, no forced frame limit':''),result,errors,relayLog},null,2));console.log(JSON.stringify(result));
 assert.ok(result.sustainedCadencePassed,'Simulation slowed by recovery: '+result.elapsedMs+' ms');
}catch(error){writeFileSync(resolve(root,`artifacts/multiplayer-tests/${label}-failure.json`),JSON.stringify({error:error.stack,errors,relayLog},null,2));throw error;}
finally{await browser.close();netplay.close();await new Promise(r=>server.close(r));relay.kill();}
