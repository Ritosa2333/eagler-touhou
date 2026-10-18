// Native gesture producer, two rollback worlds and an exact confirmed-input
// reference. This controlled tick loop is not RTC/WAN, game FPS or phone timing.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,existsSync,mkdirSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
import {buildTouchInputSchedule} from './touch-input-probe.mjs';
import {startHostLoadProbe} from './host-load-probe.mjs';

const root=fileURLToPath(new URL('../../',import.meta.url));
const label=process.env.RUN_LABEL||'touch-peer';assert.match(label,/^[\w-]+$/);
const artifactDirectory=resolve(process.env.PC_BUILD||resolve(root,'artifacts/sdl-adonis'));
const hardware=process.env.NATIVE_GPU==='1',mode=Number(process.env.ADONIS_MODE??2);
const delay=Number(process.env.INPUT_DELAY_FRAMES??2),limit=Number(process.env.PEER_FRAMES??900);
const touchProfile=process.env.TOUCH_PROFILE||'legacy',sourceHz=Number(process.env.TOUCH_SOURCE_HZ??60);
const deliveryClock=process.env.DELIVERY_CLOCK||'iteration',fixedDelay=Number(process.env.FIXED_DELAY_FRAMES??3);
const cpuRate=Number(process.env.CPU_RATE??1),hostLoad=process.env.HOST_LOAD==='1',maximumIterations=10000;
assert.ok([0,1,2].includes(mode)&&Number.isInteger(delay)&&delay>=0&&delay<=9);
assert.ok(Number.isInteger(limit)&&limit>=360&&limit<=7200&&[1,4].includes(cpuRate));
assert.ok(['legacy','direct','joystick'].includes(touchProfile)&&[60,120].includes(sourceHz));
assert.ok(['iteration','frontier'].includes(deliveryClock)&&Number.isInteger(fixedDelay)&&fixedDelay>=0&&fixedDelay<=8);
assert.ok(deliveryClock!=='frontier'||fixedDelay>0&&delay===0,'Frontier delivery requires a positive delay and D0');
const schedules=touchProfile==='legacy'?null:[0,1].map(side=>buildTouchInputSchedule({profile:touchProfile,sourceHz,durationMs:limit*1000/60,side}));
const parameters={mode,delay,limit,hardware,touchProfile,sourceHz,deliveryClock,fixedDelay,cpuRate,hostLoad,maximumIterations,
 inputClock:'Scheduled milliseconds converted from monotonic local capture slot / 60. This is a deterministic oracle stimulus, not independent wall-clock input.',
 transportClock:deliveryClock==='frontier'?'Receiving peer monotonic simulated frontier; bounded delivery-only tail after DEV frame limit':'One iteration advances after both peer tick attempts',
 cpuThrottle:{method:'CDP Emulation.setCPUThrottlingRate',rate:cpuRate,scope:'All three same-origin worlds share this throttled renderer and execute sequentially. No per-peer CPU isolation or gameplay Hz claim.'}};
const output=resolve(root,'artifacts/multiplayer-tests');mkdirSync(output,{recursive:true});
const reportPath=resolve(output,label+'-report.json'),failurePath=resolve(output,label+'-failure.json'),cleanupPath=resolve(output,label+'-cleanup.json');
assert.ok(!existsSync(reportPath)&&!existsSync(failurePath)&&!existsSync(cleanupPath),'Use a new run label');
const build=JSON.parse(readFileSync(resolve(artifactDirectory,'build.json')));
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const wasmPath=resolve(artifactDirectory,'th09-presentation.wasm');
assert.ok(existsSync(wasmPath),'Exact world/audit/framebuffer/Replay oracle requires a DEV artifact');
const identityFiles={harness:fileURLToPath(import.meta.url),touchProbe:fileURLToPath(new URL('./touch-input-probe.mjs',import.meta.url)),
 fixture:resolve(root,'tests/browser/presentation.html'),build:resolve(artifactDirectory,'build.json'),wasm:wasmPath,
 loader:resolve(artifactDirectory,'th09-presentation.mjs')};
if(hostLoad)identityFiles.hostLoadProbe=fileURLToPath(new URL('./host-load-probe.mjs',import.meta.url));
const identity=Object.fromEntries(Object.entries(identityFiles).map(([key,path])=>[key,sha(readFileSync(path))]));
assert.equal(build.sha256,identity.wasm,'DEV manifest does not describe selected Wasm');
const {server,netplay,url}=await presentationServer(0,{artifactDirectory});
let browser=null,page=null,cdp=null,hostProbe=null,hostLoadSnapshot=null,retainedResult=null;
const errors=[],cleanupErrors=[];let partialCapture=null;
const finishHost=async()=>{if(hostProbe&&!hostLoadSnapshot)hostLoadSnapshot=await hostProbe.stop();return hostLoadSnapshot;};
try{
 hostProbe=hostLoad?startHostLoadProbe():null;hostProbe?.mark('touch-peer-start');
 browser=await launchBrowser({args:[...(hardware?['--enable-gpu','--use-gl=angle','--use-angle=d3d11']:['--enable-unsafe-swiftshader']),'--autoplay-policy=no-user-gesture-required']});
 page=await browser.newPage();page.on('pageerror',e=>errors.push(e.stack));await page.goto(url);
 cdp=await page.context().newCDPSession(page);await cdp.send('Emulation.setCPUThrottlingRate',{rate:cpuRate});
 hostProbe?.mark('touch-peer-throttle-configured');
 const result=await page.evaluate(async({mode,delay,limit,schedules,touchProfile,deliveryClock,fixedDelay,maximumIterations})=>{
  const worlds=[],pending=[],tape=[],localTape=[[],[]],localByFrame=[new Map(),new Map()],confirmedHashes=[[],[]],referenceHashes=[];
  const deliveryFrontier=[0,0],events=[[],[]],samples=[[],[]],positions=[[],[]],motionSamples=[0,0],waits=[0,0];
  const timeOriginMs=performance.timeOrigin,startedAt=performance.now();let clock=0,iterations=0;
  for(let i=0;i<3;++i){const f=document.createElement('iframe');f.src='/';document.body.append(f);await new Promise(r=>f.onload=r);worlds.push(f.contentWindow);}
  for(const w of worlds)await w.openProbe(0,1,2,3,true);
  const stats=w=>{const at=w.core._th09_rollback_info()/4;return Array.from(w.core.HEAPU32.subarray(at,at+8));};
  const error=w=>{const c=w.core,p=c._th09_error();return new TextDecoder().decode(c.HEAPU8.subarray(p,c.HEAPU8.indexOf(0,p)));};
  globalThis.__th09TouchPeerSnapshot=()=>({partial:true,clock,iterations,summary:worlds.slice(0,2).map(stats),events,samples,positions,
   localTape,confirmedHashes,referenceHashes,tape,pendingPackets:pending.length,deliveryFrontier,waits,
   observationClock:{timeOriginMs,startedAt,endedAt:performance.now()}});
  const send=(side,bytes)=>{
   const copy=new Uint8Array(bytes),input=copy[0]===69&&copy[5]===1;
   if(copy.length===20&&copy[0]===84&&copy[1]===57&&copy[2]===72&&copy[3]===67){
    const v=new DataView(copy.buffer);confirmedHashes[side].push([v.getUint32(12,true),v.getUint32(16,true)]);
   }
   let dueFrame=null;
   if(input){
    if(copy.length<44||copy[1]!==57||copy[2]!==78||copy[3]!==80||copy[4]!==4||copy[6]!==side||copy[42]>32||copy.length!==44+copy[42]*12)throw Error('Local touch packet v4 schema');
    const v=new DataView(copy.buffer),first=v.getUint32(32,true);
    if(deliveryClock==='frontier'&&copy[42])dueFrame=first+copy[42]-1+fixedDelay;
    for(let i=0;i<copy[42];++i){
     const frame=first+i,at=44+i*12,word0=v.getUint32(at,true),xBits=v.getUint32(at+4,true),yBits=v.getUint32(at+8,true);
     const prior=localByFrame[side].get(frame);if(prior){if(prior.word0!==word0||prior.xBits!==xBits||prior.yBits!==yBits)throw Error('Local input history rewrite');continue;}
     if(frame!==localTape[side].length)throw Error('Local input capture gap');
     const rawHex=Array.from(copy.subarray(at,at+12),x=>x.toString(16).padStart(2,'0')).join('');
     const row={frame,buttons:v.getUint16(at,true),analogMode:copy[at+2],flags:copy[at+3],x:v.getFloat32(at+4,true),y:v.getFloat32(at+8,true),
      word0,xBits,yBits,rawHex,iteration:clock};
     localTape[side].push(row);localByFrame[side].set(frame,row);
    }
   }
   pending.push({to:1-side,due:clock+(input?fixedDelay:0),dueFrame,bytes:copy});
  };
  for(let side=0;side<2;++side){const w=worlds[side];
   const dc=()=>({readyState:'open',bufferedAmount:0,send:b=>send(side,b)});
   w.__eaglerPeerTransport={route:'rtc',localPlayer:side,playerCount:2,received:[],receivedHead:0,
    peers:new Map([[1-side,{inputOpen:true,controlOpen:true,inputDc:dc(),controlDc:dc()}]])};
   if(!w.core._th09_adonis_configure(mode)||!w.core._th09_rollback_begin(12345,side,3,0,1,0x1234,0x5678,0x901,delay))throw Error('begin '+error(w));
   w.core._th09_probe_frame_limit(limit);w.core._th09_touch_options(1,touchProfile==='joystick'?3:0,schedules?1.5:1,0,0);w.core._th09_touch_controls(1,0,0,0,0);
  }
  worlds[0].core.onNetworkSpectatorFrame=(...values)=>{
   if(values[0]!==tape.length)throw Error('Nonsequential confirmed output');tape.push(values);
  };
  const reference=worlds[2];if(!reference.core._th09_spectator_begin(12345,3,0,1))throw Error('reference begin '+error(reference));
  const deliver=()=>{
   for(let side=0;side<2;++side){deliveryFrontier[side]=Math.max(deliveryFrontier[side],stats(worlds[side])[0]);if(deliveryFrontier[side]>=limit)++deliveryFrontier[side];}
   for(let i=pending.length-1;i>=0;--i)if(pending[i].dueFrame!==null?deliveryFrontier[pending[i].to]>=pending[i].dueFrame:pending[i].due<=clock){
    const p=pending.splice(i,1)[0];worlds[p.to].__eaglerPeerTransport.received.push(p.bytes);
   }
  };
  for(let n=0;n<64&&!worlds.slice(0,2).every(w=>stats(w)[6]);++n){deliver();for(const w of worlds.slice(0,2))if(!w.core._th09_rollback_pump())throw Error(error(w));++clock;}
  if(!worlds.slice(0,2).every(w=>stats(w)[6]))throw Error('Handshake failed');
  const injected=[-1,-1],scheduleHeads=[0,0],observedFrontier=[0,0];
  const physical=(w,side,f)=>{
   const c=w.core;
   if(schedules){
    const source=schedules[side].events;
    while(scheduleHeads[side]<source.length&&source[scheduleHeads[side]].atMs<=f*1000/60+1e-7){
     const e=source[scheduleHeads[side]++];events[side].push({captureSlot:f,...e});
     if(e.kind==='key')c._th09_key(e.scan,e.down);
     else if(e.kind==='controls')c._th09_touch_controls(1,e.fire,e.focus,0,0);
     else if(e.kind==='pointer')c._th09_touch(e.type,e.id,e.x,e.y);
     else if(e.kind==='stick')c._th09_touch_stick(e.x*32767,e.y*32767);
     else if(e.kind==='cancel'){if(touchProfile==='joystick')c._th09_touch_stick(0,0);c._th09_touch_cancel();}
    }return;
   }
   c._th09_key(29,+(f>0&&f<130));c._th09_key(44,+(f%126<90));c._th09_key(42,+(f%180<45));
   if(side!==1)return;
   const event=(type,x,y)=>{c._th09_touch(type,71,x,y);events[side].push([f,type,x,y]);};
   if(f===150){event(0,.5,.7);event(1,.58,.7);}if(f===360)event(1,.38,.62);if(f===500)event(2,.38,.62);
   if(f===560){event(0,.5,.7);event(1,.6,.74);}if(f===700){c._th09_touch_cancel();events[side].push([f,'cancel']);}
  };
  while(++iterations<maximumIterations){
   deliver();
   for(let side=0;side<2;++side){const w=worlds[side],before=stats(w);
    if(before[4]<limit&&before[4]!==injected[side]){injected[side]=before[4];physical(w,side,before[4]);}
    const at=performance.now(),ok=w.core._th09_game_tick(1),ms=performance.now()-at;
    if(!ok)throw Error('P'+(side+1)+' '+before[0]+': '+error(w));
    const after=stats(w),corrections=after[2]-before[2],resimulated=after[3]-before[3];
    const restoredDepth=corrections===1?before[0]+resimulated+(ok===1?1:0)-after[0]:0;
    if(corrections>1||restoredDepth<0||restoredDepth>8)throw Error('Invalid reconstructed restore distance');
    samples[side].push([before[0],after[0],corrections,resimulated,ms,ok,restoredDepth]);if(ok===2)++waits[side];
    if(after[0]>observedFrontier[side]){observedFrontier[side]=after[0];const p=w.core._th09_touch_probe();
     positions[side].push([after[0],...new Float32Array(w.core.HEAPU8.buffer,p,8)]);}
   }
   ++clock;
   if(worlds.slice(0,2).every(w=>stats(w)[0]===limit&&stats(w)[1]===limit)&&tape.length===limit)break;
   if(iterations%8===0)await new Promise(r=>setTimeout(r,0));
  }
  const summary=worlds.slice(0,2).map(stats);
  if(!summary.every(v=>v[0]===limit&&v[1]===limit&&v[4]===limit)||tape.length!==limit)throw Error('Incomplete '+JSON.stringify({summary,tape:tape.length}));
  const checks=Math.floor(limit/120);
  for(let side=0;side<2;++side){if(confirmedHashes[side].length<checks||localTape[side].length<limit)throw Error('Incomplete input/hash trace');
   for(let i=0;i<checks;++i)if(confirmedHashes[side][i][0]!==120*(i+1))throw Error('Confirmed hash frame prefix');}
  if(JSON.stringify(confirmedHashes[0].slice(0,checks))!==JSON.stringify(confirmedHashes[1].slice(0,checks)))throw Error('Confirmed peer hashes differ');
  for(const values of tape){
   if(values[3])++motionSamples[0];if(values[6])++motionSamples[1];
   for(let side=0;side<2;++side){const actual=localTape[side][values[0]],motion=actual.analogMode===0?0:actual.analogMode===1?1:actual.flags&1?3:2;
    if(actual.buttons!==values[1+side]||motion!==values[3+side*3]||actual.x!==values[4+side*3]||actual.y!==values[5+side*3])throw Error('Confirmed input differs from actual local wire');}
   if(!reference.core._th09_spectator_feed(...values)||!reference.core._th09_game_tick(0))throw Error('exact reference '+error(reference));
   if((values[0]+1)%120===0)referenceHashes.push([values[0]+1,reference.core._th09_network_hash()>>>0]);
  }
  if(JSON.stringify(referenceHashes)!==JSON.stringify(confirmedHashes[0].slice(0,checks)))throw Error('Exact reference confirmed hash trace differs');
  const activity=localTape.map((rows,side)=>({rawEvents:events[side].length,analogActiveFrames:rows.filter(r=>r.analogMode!==0).length,
   targetChangedFrames:rows.filter((r,i)=>i>0&&r.analogMode!==0&&r.analogMode===rows[i-1].analogMode&&(r.xBits!==rows[i-1].xBits||r.yBits!==rows[i-1].yBits)).length,
   positionChangedObservations:positions[side].filter((r,i)=>i>0&&(r[1]!==positions[side][i-1][1]||r[2]!==positions[side][i-1][2])).length,
   newFrontierObservations:positions[side].length,maxRestoredDepth:Math.max(0,...samples[side].map(r=>r[6])),
   partialRecoveryCalls:samples[side].filter(r=>r[1]<r[0]).length,maxResimulatedInOneCall:Math.max(0,...samples[side].map(r=>r[3]))}));
  if(touchProfile==='legacy'){if(motionSamples[1]<200)throw Error('Raw drag did not reach native input');}
  else if(activity.some(a=>a.analogActiveFrames<100||a.targetChangedFrames<50||a.positionChangedObservations<20))throw Error('Continuous touch load was not exercised '+JSON.stringify(activity));
  const hashes=worlds.map(w=>w.core._th09_network_hash()>>>0);
  if(new Set(hashes).size!==1)throw Error('Reference world divergence '+JSON.stringify(hashes));
  const audits=worlds.map(w=>{const c=w.core,at=c._th09_probe_world()/4;return Array.from(c.HEAP32.subarray(at,at+48));});
  if(new Set(audits.map(v=>JSON.stringify(v))).size!==1)throw Error('Reference audit differs');
  const images=worlds.map(w=>{w.core._th09_game_draw();return w.document.querySelector('canvas').toDataURL();});
  if(new Set(images).size!==1)throw Error('Reference framebuffer differs');
  const replays=worlds.map(w=>{if(!w.core._th09_probe_save_replay())throw Error('Replay save');return Array.from(w.core.FS.readFile('/save/replay/th9_25.rpy'));});
  if(new Set(replays.map(v=>JSON.stringify(v))).size!==1)throw Error('Reference Replay bytes differ');
  const distribution=a=>{a=a.slice().sort((x,y)=>x-y);return {count:a.length,p50:a[Math.floor(a.length*.5)]??null,p95:a[Math.floor(a.length*.95)]??null,p99:a[Math.floor(a.length*.99)]??null,max:a.at(-1)??null,total:a.reduce((s,v)=>s+v,0)};};
  const measured=samples.map(rows=>({allCalls:distribution(rows.map(r=>r[4])),resimulated:rows.reduce((s,r)=>s+r[3],0),
   legacyFrameFiltered:touchProfile==='legacy'?{all:distribution(rows.filter(r=>r[0]>=150&&r[0]<limit).map(r=>r[4])),held:distribution(rows.filter(r=>r[0]>=200&&r[0]<340).map(r=>r[4]))}:null,
   method:'Sequential explicit game_tick call work including all waits/recovery; not rAF or gameplay Hz. Legacy frame filters retained only for legacy profile.'}));
  const gl=worlds[0].document.querySelector('canvas').getContext('webgl2'),ext=gl.getExtension('WEBGL_debug_renderer_info');
  return {summary,hashes,audits,events,tape,localTape,confirmedHashes,referenceHashes,checks,replay:replays[0],motionSamples,activity,measured,samples,positions,waits,iterations,deliveryFrontier,framebuffersEqual:true,
   sampleSchema:['beforeNext','afterNext','correctionDelta','resimulatedDelta','callWorkMs','returnCode','reconstructedRestoreDepth'],
   restoreMethod:'On one correction: beforeNext + resimulatedDelta + (returnCode===1 ? 1 : 0) - afterNext; final forward success is one fresh tick, budget-sliced recovery-only return adds none.',
   wireVerification:'Local retransmissions compare every byte of FrameInput, including touchUsed/unlimited/touchBomb flags and float bits. Confirmed spectator tuples independently compare buttons, motion kind including unlimited, x/y; spectator ABI has no separate touchUsed/touchBomb flags.',
   observationClock:{timeOriginMs,startedAt,endedAt:performance.now(),scope:'Includes setup, all three worlds and oracle; not gameplay throughput.'},
   renderer:ext?gl.getParameter(ext.UNMASKED_RENDERER_WEBGL):gl.getParameter(gl.RENDERER)};
 },{mode,delay,limit,schedules,touchProfile,deliveryClock,fixedDelay,maximumIterations});
 retainedResult=result;assert.equal(errors.length,0,errors.join('\n'));
 if(hardware)assert.doesNotMatch(result.renderer,/swiftshader|llvmpipe|microsoft basic render/i);
 const identityAfter=Object.fromEntries(Object.entries(identityFiles).map(([key,path])=>[key,sha(readFileSync(path))]));
 assert.deepEqual(identityAfter,identity,'Touch oracle inputs changed during run');hostProbe?.mark('touch-peer-oracles-finished');await finishHost();
 const report={passed:true,scope:'Two real TH09 rollback worlds and exact third-world input/hash/audit/framebuffer/Replay oracle; native ABI gestures; declared '+deliveryClock+' transport delay; all worlds in one CPU-throttled renderer; not RTC, wall-clock input response, Launcher bridge or phone evidence',
  wasm:identity.wasm,harnessSha256:identity.harness,identity,identityAfter,browserVersion:browser.version(),parameters,scheduleSha256:schedules?sha(JSON.stringify(schedules)):null,
  tapeSha256:sha(JSON.stringify(result.tape)),localTapeSha256:result.localTape.map(rows=>sha(JSON.stringify(rows.slice(0,limit).map(r=>[r.frame,r.rawHex])))),
  replaySha256:sha(Uint8Array.from(result.replay)),replayBytes:result.replay.length,result,errors,hostLoad:hostLoadSnapshot};
 writeFileSync(reportPath,JSON.stringify(report,null,2),{flag:'wx'});
 console.log(JSON.stringify({passed:true,identity,parameters,hashes:result.hashes,activity:result.activity,measured:result.measured}));
}catch(error){
 if(!retainedResult&&page){let partialTimer;
  try{retainedResult=await Promise.race([page.evaluate(()=>globalThis.__th09TouchPeerSnapshot?.()??null),new Promise((_,reject)=>{partialTimer=setTimeout(()=>reject(Error('Partial touch-peer snapshot timed out after 5000 ms')),5000);})]);
   partialCapture={status:retainedResult?'captured':'not-installed'};
  }catch(partialError){partialCapture={status:'failed',error:String(partialError)};}finally{clearTimeout(partialTimer);}
 }
 try{await finishHost();}catch(stopError){cleanupErrors.push({stage:'host-stop',error:String(stopError)});}
 writeFileSync(failurePath,JSON.stringify({error:error.stack,identity,parameters,errors,result:retainedResult,partialCapture,hostLoad:hostLoadSnapshot,cleanupErrors},null,2),{flag:'wx'});throw error;
}finally{
 for(const [stage,close] of [['host-stop',finishHost],['cdp-detach',()=>cdp?.detach()],['browser-close',()=>browser?.close()],['relay-close',()=>netplay.close()],['server-close',()=>new Promise((accept,reject)=>server.close(error=>error?reject(error):accept()))]]){
  try{await close();}catch(error){cleanupErrors.push({stage,error:String(error)});}
 }
 writeFileSync(cleanupPath,JSON.stringify({complete:cleanupErrors.length===0,errors:cleanupErrors},null,2),{flag:'wx'});
 if(cleanupErrors.length)process.exitCode=1;
}
