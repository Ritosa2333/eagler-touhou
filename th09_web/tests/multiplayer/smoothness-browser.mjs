// Two separate Chromium processes, real RTC, wall-clock keyboard input.
// No artificial outage by default: isolate sustained correction from outages.
import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {createHash} from 'node:crypto';
import {mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import net from 'node:net';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';

const root=fileURLToPath(new URL('../../',import.meta.url));
const label=process.env.RUN_LABEL||'smoothness';
assert.match(label,/^[\w-]+$/);
const integer=(key,fallback,min,max)=>{const n=Number(process.env[key]??fallback);assert.ok(Number.isInteger(n)&&n>=min&&n<=max,key);return n;};
const limit=integer('FRAMES',1800,300,18000),delay=integer('DELAY_MS',39,0,200),jitter=integer('JITTER_MS',5,0,100);
const slow=integer('CPU_RATE',1,1,8),dropEvery=integer('DROP_EVERY',0,0,1000);
const release=process.argv.includes('--release'),profile=process.env.PROFILE_CPU==='1';
const inputMode=process.env.INPUT_MODE||'wall-clock';assert.ok(['wall-clock','tape'].includes(inputMode));
const artifactDirectory=resolve(process.env.PC_BUILD||resolve(root,release?'artifacts/sdl-release':'artifacts/sdl3'));
const out=resolve(root,'artifacts/multiplayer-tests');mkdirSync(out,{recursive:true});
const sha=path=>createHash('sha256').update(readFileSync(path)).digest('hex');
const wasmPath=resolve(artifactDirectory,release?'th09.wasm':'th09-presentation.wasm');
const identity={wasm:sha(wasmPath),harness:sha(fileURLToPath(import.meta.url)),loader:sha(resolve(artifactDirectory,release?'th09.mjs':'th09-presentation.mjs'))};
const relayPath=resolve(process.env.EAGLER_RELAY_SOURCE||'D:/workspace/eagler/eagler-touhou/server/netplay-relay.mjs');
identity.relay=sha(relayPath);identity.shell=sha(resolve(root,'sdl-runtime/shared-netplay.mjs'));
const port=await new Promise(r=>{const s=net.createServer();s.listen(0,'127.0.0.1',()=>{const p=s.address().port;s.close(()=>r(p));});});
const relay=spawn(process.execPath,[relayPath],{windowsHide:true,env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:String(port),EAGLER_NETPLAY_STUN_URLS:''},stdio:['ignore','pipe','pipe']});
let relayLog='',host;const browsers=[],pages=[],sessions=[],errors=[];
const parameters={limit,delay,jitter,slow,dropEvery,release,profile,inputMode};
const distribution=values=>{const a=values.slice().sort((a,b)=>a-b);return {count:a.length,total:a.reduce((s,n)=>s+n,0),p50:a[Math.floor(a.length*.5)]??null,p95:a[Math.floor(a.length*.95)]??null,p99:a[Math.floor(a.length*.99)]??null,max:a.at(-1)??null,over25ms:a.filter(n=>n>25).length,over50ms:a.filter(n=>n>50).length};};
try {
 await new Promise((accept,reject)=>{const timer=setTimeout(()=>reject(Error('relay startup timeout')),10000);relay.stdout.on('data',b=>{relayLog=(relayLog+b).slice(-32768);if(relayLog.includes('netplay relay listening')){clearTimeout(timer);accept();}});relay.stderr.on('data',b=>relayLog=(relayLog+b).slice(-32768));relay.on('error',reject);relay.on('exit',code=>reject(Error('relay exit '+code)));});
 host=await presentationServer(0,{release,artifactDirectory});
 for(let side=0;side<2;++side){
  const browser=await launchBrowser({args:['--enable-gpu','--use-gl=angle','--use-angle=d3d11','--autoplay-policy=no-user-gesture-required']});browsers.push(browser);
  const page=await browser.newPage();pages.push(page);page.on('pageerror',e=>errors.push({side,error:e.stack}));
  await page.addInitScript(({delay,jitter,dropEvery,side})=>{
   const raf=window.requestAnimationFrame.bind(window);
   window.requestAnimationFrame=callback=>raf(timestamp=>{
    const previous=window.currentGameRafTimestamp;window.currentGameRafTimestamp=timestamp;
    try{return callback(timestamp);}finally{window.currentGameRafTimestamp=previous;}
   });
   const stats=window.impairment={matched:0,sent:0,dropped:0,pending:0,peakPending:0,planned:[],delivered:[],lanes:{}};
   let seed=0x915ab+side;
   const send=RTCDataChannel.prototype.send;
   RTCDataChannel.prototype.send=function(bytes){
    if(typeof bytes==='string')return send.call(this,bytes);
    const data=ArrayBuffer.isView(bytes)?new Uint8Array(bytes.buffer,bytes.byteOffset,bytes.byteLength):new Uint8Array(bytes);
    if(data[0]!==69||data[5]!==1)return send.call(this,bytes);
    window.observeLocalInput?.(data);
    const n=++stats.matched;stats.lanes[this.label]=(stats.lanes[this.label]||0)+1;
    if(this.label.includes('input')&&dropEvery&&n%dropEvery===0){++stats.dropped;return;}
    seed=(Math.imul(seed,1664525)+1013904223)>>>0;
    const planned=Math.max(0,delay+(seed/0x100000000*2-1)*jitter),began=performance.now(),copy=data.slice();
    stats.planned.push(planned);++stats.pending;stats.peakPending=Math.max(stats.peakPending,stats.pending);
    if(stats.pending>1024||stats.planned.length>200000)throw Error('impairment sample/queue overflow');
    setTimeout(()=>{--stats.pending;if(this.readyState==='open'){send.call(this,copy);++stats.sent;stats.delivered.push(performance.now()-began);}},planned);
   };
  },{delay,jitter,dropEvery,side});
  await page.goto(host.url);await page.evaluate(()=>openProbe(0,1,2,3,true));
  const cdp=await page.context().newCDPSession(page);sessions.push(cdp);
  if(side===1&&slow>1)await cdp.send('Emulation.setCPUThrottlingRate',{rate:slow});
 }
 await pages[0].evaluate(async port=>{
  const wait=async test=>{const start=performance.now();while(!test()){if(performance.now()-start>10000)throw Error('lobby timeout');await new Promise(r=>setTimeout(r,10));}};
  window.lobbies=[];
  for(const id of ['smoothnessleft','smoothnessright']){const ws=new WebSocket(`ws://127.0.0.1:${port}/?room=th09mp-0998&lobby=${id}`);ws.onmessage=e=>{const m=JSON.parse(e.data);if(m.type==='error')throw Error(m.error);ws.latest=m;};lobbies.push(ws);await wait(()=>ws.readyState===WebSocket.OPEN&&ws.latest);}
  lobbies[0].send(JSON.stringify({type:'take-seat',seat:0,loadout:0}));await wait(()=>lobbies[0].latest.room?.seats[0]);
  lobbies[1].send(JSON.stringify({type:'take-seat',seat:1,loadout:1}));await wait(()=>lobbies[0].latest.room?.seats[1]);
  for(const ws of lobbies)ws.send(JSON.stringify({type:'set-ready',ready:true}));await wait(()=>lobbies[0].latest.room?.seats.slice(0,2).every(s=>s?.ready));
  lobbies[0].send(JSON.stringify({type:'start'}));await wait(()=>lobbies[0].latest.type==='start');
 },port);
 for(let side=0;side<2;++side)await pages[side].evaluate(async ({side,port,limit,inputMode})=>{
  const {SharedNetplay}=await import('/app/shared-netplay.mjs');
  const options={netplayUrl:`ws://127.0.0.1:${port}/?room=th09mp-0998&run=1&players=2&player=${side}`,netplayPlayer:side,netplayPlayerCount:2,netplaySeed:12345,netplayDifficulty:3,netplayLoadouts:[{character:0},{character:1}],netplayIceServers:[],netplaySpectatorCount:0};
  const m=window.measure={samples:[],loads:[],notices:[],closed:[],inputTape:[],started:0,ended:0,limit,audioStart:0,finished:false};
  core.eaglerOptions=options;window.play=new SharedNetplay(core,{onStatus:s=>m.notices.push(s),onClose:s=>m.closed.push(s),onResult:()=>{m.finished=true;}});
  window.readStats=()=>Array.from(core.HEAPU32.subarray(core._th09_rollback_info()/4,core._th09_rollback_info()/4+8));
  const updateInput=now=>{
   if(inputMode==='tape')return;
   const age=now-m.started,cycle=Math.floor(age/360)+side;
   // Physical intent continues while simulation is late; no frame-driven drag.
   for(const [scan,down] of [[29,age<2200],[44,age%2100<1500],[42,age%3000<750],[203,cycle%4===0],[205,cycle%4===2]])core._th09_key(scan,+down);
  };
  if(inputMode==='tape'){
   // Keep the production keyboard sampler and protocol untouched. A new local
   // packet is sent synchronously by LocalCaptured, so set the NEXT frame's
   // hosted keys there, even if the callback will simulate several fresh ticks.
   // Validate every actual wire sample, not merely our intended key sequence.
   const keys=frame=>{const cycle=Math.floor(frame/22)+side;return frame===0?0:
    (frame<132?256:0)|(frame%126<90?1:0)|(frame%180<45?4:0)|(cycle%4===0?64:0)|(cycle%4===2?128:0);};
   window.observeLocalInput=data=>{
    const view=new DataView(data.buffer,data.byteOffset,data.byteLength),count=data[42];
    if(data.length<44||data[2]!==78||data[3]!==80||data[6]!==side||data.length!==44+count*12)throw Error('Unexpected input packet schema');
    const first=view.getUint32(32,true);
    for(let i=0;i<count;++i){const frame=first+i,at=44+i*12,buttons=view.getUint16(at,true);
     if(frame>limit+8||buttons!==keys(frame)||view.getUint16(at+2,true)!==0||view.getFloat32(at+4,true)!==0||view.getFloat32(at+8,true)!==0)
      throw Error(`Input tape mismatch P${side+1} frame ${frame}: ${buttons} != ${keys(frame)}`);
     if(frame===m.inputTape.length)m.inputTape.push(buttons);
     else if(frame>m.inputTape.length||m.inputTape[frame]!==buttons)throw Error('Input tape gap/rewrite');
    }
    const next=keys(m.inputTape.length);
    for(const [scan,mask] of [[29,256],[44,1],[42,4],[203,64],[205,128]])core._th09_key(scan,+!!(next&mask));
   };
  }
  core.onNetworkSpectatorFrame=(...args)=>play.publish(...args);core.onNetworkResult=()=>play.result();
  core.onGameFrame=(ok,ms)=>{
   play.frame();if(!play.active||m.ended)return;const now=performance.now();
   if(!m.started){m.started=now;m.audioStart=core.SDL3?.audioContext?.currentTime||0;}
   updateInput(now);const s=readStats(),at=core._th09_game_metrics()/4,n=core.HEAPU32[at+9],phase=sessionStatus()[0];
   if(!Number.isFinite(window.currentGameRafTimestamp))throw Error('Game callback outside measured requestAnimationFrame');
   m.samples.push([now-m.started,ok,ms,...s,n,phase,window.currentGameRafTimestamp]);
   if(core._th09_probe_load&&m.samples.length%30===0){const at=core._th09_probe_load()/4;m.loads.push([s[0],...core.HEAPU32.subarray(at,at+20)]);}
   if(m.samples.length>50000)throw Error('performance sample overflow');
   if(s[0]>=limit&&s[1]>=limit)m.ended=now;
  };
  core._th09_probe_frame_limit?.(limit);core._th09_loop_start();core._th09_loop_pause(1);await play.connect(options);
 },{side,port,limit,inputMode});
 if(profile)for(const cdp of sessions){await cdp.send('Profiler.enable');await cdp.send('Profiler.setSamplingInterval',{interval:500});await cdp.send('Profiler.start');}
 const began=Date.now(),timeout=limit/60*1000*3+20000;
 let status;
 while(Date.now()-began<timeout){
  status=await Promise.all(pages.map(p=>p.evaluate(()=>({stats:readStats(),ended:measure.ended,closed:measure.closed,finished:measure.finished}))));
  if(errors.length||status.some(s=>s.closed.length))throw Error('runtime failure '+JSON.stringify({errors,status}));
  if(status.every(s=>s.ended))break;
  if(status.some(s=>s.finished))throw Error('match ended before requested workload '+JSON.stringify(status));
  await new Promise(r=>setTimeout(r,100));
 }
 assert.ok(status.every(s=>s.ended),'incomplete workload '+JSON.stringify(status));
 if(profile)for(let side=0;side<2;++side){const result=await sessions[side].send('Profiler.stop');writeFileSync(resolve(out,`${label}-P${side+1}.cpuprofile`),JSON.stringify(result.profile));}
 const results=await Promise.all(pages.map(p=>p.evaluate(release=>{
  core._th09_loop_pause(1);const gl=document.querySelector('canvas').getContext('webgl2'),ext=gl.getExtension('WEBGL_debug_renderer_info');
  const renderer=ext?gl.getParameter(ext.UNMASKED_RENDERER_WEBGL):gl.getParameter(gl.RENDERER);
  const audio={state:core.SDL3?.audioContext?.state,advancedSeconds:(core.SDL3?.audioContext?.currentTime||0)-measure.audioStart};
  const audited=release?null:Array.from(core.HEAP32.subarray(core._th09_probe_world()/4,core._th09_probe_world()/4+48));
  return {stats:readStats(),hash:core._th09_network_hash()>>>0,audited,route:window.__eaglerNetplayTransport,renderer,audio,measure,impairment,memoryBytes:core.HEAPU8.length};
 },release)));
 assert.equal(errors.length,0);assert.ok(results.every(r=>r.route==='rtc'));
 for(const r of results){assert.doesNotMatch(r.renderer,/swiftshader|llvmpipe|microsoft basic render/i);assert.ok(r.impairment.matched>100&&r.impairment.sent>100,'RTC input impairment not exercised');assert.ok(r.audio.advancedSeconds>1);}
 const sameFrame=results[0].stats[0]===results[1].stats[0];
 if(inputMode==='tape')for(const r of results){assert.ok(r.measure.inputTape.length>=limit);r.inputTapeSha256=createHash('sha256').update(JSON.stringify(r.measure.inputTape.slice(0,limit))).digest('hex');}
 if(sameFrame){assert.equal(results[0].hash,results[1].hash);if(!release)assert.deepEqual(results[0].audited,results[1].audited);}
 const summary=results.map(r=>{
  const samples=r.measure.samples.filter(s=>s[3]>=180&&s[3]<limit),gaps=[],completionGaps=[],rafGaps=[];let prior;
  for(let i=1;i<samples.length;++i)if(samples[i][13]!==samples[i-1][13])rafGaps.push(samples[i][13]-samples[i-1][13]);
  for(const s of samples)if(!prior||s[11]!==prior[11]){if(prior){gaps.push(s[13]-prior[13]);completionGaps.push(s[0]-prior[0]);}prior=s;}
  const first=samples[0],last=samples.at(-1),elapsed=last[0]-first[0];
  return {logicHz:(last[3]-first[3])*1000/elapsed,elapsedMs:elapsed,firstFrame:first[3],lastFrame:last[3],callbacks:samples.length,waitCallbacks:samples.filter(s=>s[1]===2).length,corrections:last[5]-first[5],resimulated:last[6]-first[6],captures:last[7]-first[7],callbackWorkMs:distribution(samples.map(s=>s[2])),presentationGapMs:distribution(gaps),callbackCompletionGapMs:distribution(completionGaps),rafGapMs:distribution(rafGaps),plannedDelayMs:distribution(r.impairment.planned),deliveredDelayMs:distribution(r.impairment.delivered),scenes:[...new Set(samples.map(s=>s[12]))]};
 });
 assert.equal(sha(wasmPath),identity.wasm,'WASM changed during run');assert.equal(sha(relayPath),identity.relay);
 const report={passed:true,scope:'Two separate desktop Chromium processes; native GPU; real RTC with seeded application-send impairment on input and repair lanes; '+(inputMode==='tape'?'wire-verified frame-indexed hosted-key tape':'wall-clock synthetic keyboard')+'; audio clock only, not acoustic acceptance',presentationMeasurement:'RAF timestamps of callbacks with a new renderer submission; NOT physical scanout. Completion gaps are reported separately because varying callback work is not itself a missed vsync.',identity,parameters,browsers:browsers.map(b=>b.version()),sameFrame,summary,results,errors,relayLog};
 writeFileSync(resolve(out,label+'-report.json'),JSON.stringify(report,null,2));console.log(JSON.stringify({identity,parameters,sameFrame,summary,hashes:results.map(r=>r.hash)}));
}catch(error){
 const partial=await Promise.all(pages.map(async p=>{try{return await p.evaluate(()=>({measure:window.measure,impairment:window.impairment,stats:window.readStats?.()}));}catch{return null;}}));
 writeFileSync(resolve(out,label+'-failure.json'),JSON.stringify({error:error.stack,identity,parameters,partial,errors,relayLog},null,2));throw error;
}finally{
 for(const browser of browsers)await browser.close();if(host){host.netplay.close();await new Promise(r=>host.server.close(r));}relay.kill();
}
