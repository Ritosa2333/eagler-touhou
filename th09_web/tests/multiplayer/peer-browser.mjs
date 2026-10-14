import assert from 'node:assert/strict';
import {existsSync,readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
import {startHostLoadProbe} from './host-load-probe.mjs';
const root=fileURLToPath(new URL('../../',import.meta.url));
const label=process.env.RUN_LABEL||'peer-browser';assert.match(label,/^[\w-]+$/);
const fixedDelay=Number(process.env.FIXED_DELAY_FRAMES||0),forceCorrection=process.env.FORCE_CORRECTION==='1';
const limit=Number(process.env.PEER_FRAMES||900),hardware=process.env.NATIVE_GPU==='1';
const cpuRate=Number(process.env.CPU_RATE??1),hostLoad=process.env.HOST_LOAD==='1';
assert.ok(cpuRate===1||cpuRate===4,'CPU_RATE must be 1 or 4');
const pauseTrace=process.env.PAUSE_TRACE==='1';
const adonisMode=Number(process.env.ADONIS_MODE||0),inputDelay=Number(process.env.INPUT_DELAY_FRAMES||0);
const captureTrace=process.env.CAPTURE_TRACE==='1';
assert.ok(!captureTrace||inputDelay===0,'Capture trace currently verifies zero-delay wire slots');
const deliveryClock=process.env.DELIVERY_CLOCK||'iteration';
assert.ok(['iteration','frontier'].includes(deliveryClock));
assert.ok(deliveryClock!=='frontier'||fixedDelay>0&&inputDelay===0,'Frontier delay requires fixed depth and zero input delay');
assert.ok(Number.isInteger(adonisMode)&&adonisMode>=0&&adonisMode<=2);
assert.ok(Number.isInteger(inputDelay)&&inputDelay>=0&&inputDelay<=9);
const inputTrace=process.env.PEER_INPUT_TRACE||(forceCorrection?'charged-strafe-alternating-focus':'legacy-zigzag');
assert.ok(['charged-strafe-alternating-focus','legacy-zigzag','endurance-tape'].includes(inputTrace));
assert.ok(!forceCorrection||inputTrace==='charged-strafe-alternating-focus');
assert.ok(Number.isInteger(fixedDelay)&&fixedDelay>=0&&fixedDelay<=8);
assert.ok(Number.isInteger(limit)&&limit>=300&&limit<=12000);
assert.ok(!forceCorrection||fixedDelay>0);
// At 4x an 8 ms recovery budget may admit only one historical tick per call.
// Predeclare enough finite calls for that case instead of mistaking a harness
// iteration ceiling for a failure to recover. This is not a time/FPS gate.
const maximumIterations=Math.max(4000,limit*(cpuRate===4?Math.max(5,fixedDelay+3):5));
const artifactDirectory=resolve(process.env.PC_BUILD||resolve(root,'artifacts/sdl3'));
const reportPath=resolve(root,'artifacts/multiplayer-tests',label+'-report.json');
const failurePath=resolve(root,'artifacts/multiplayer-tests',label+'-failure.json');
const cleanupPath=resolve(root,'artifacts/multiplayer-tests',label+'-cleanup.json');
assert.ok(!existsSync(reportPath)&&!existsSync(failurePath)&&!existsSync(cleanupPath),'Retained peer run already exists');
const sha=path=>createHash('sha256').update(readFileSync(path)).digest('hex');
const identityFiles={harness:fileURLToPath(import.meta.url),
  wasm:resolve(artifactDirectory,'th09-presentation.wasm'),
  loader:resolve(artifactDirectory,'th09-presentation.mjs'),
  manifest:resolve(artifactDirectory,'build.json'),
  presentationHarness:resolve(root,'tests/browser/presentation.html'),
  presentationServer:fileURLToPath(new URL('../../scripts/presentation-server.mjs',import.meta.url)),
  browserLauncher:fileURLToPath(new URL('../../../th10_web/scripts/native/browser-launch.mjs',import.meta.url))};
if(hostLoad)identityFiles.hostLoadProbe=fileURLToPath(new URL('./host-load-probe.mjs',import.meta.url));
const identity=Object.fromEntries(Object.entries(identityFiles).map(([name,path])=>[name,sha(path)]));
const parameters={fixedDelay,forceCorrection,limit,maximumIterations,hardware,pauseTrace,inputTrace,adonisMode,inputDelay,captureTrace,deliveryClock,cpuRate,hostLoad,
  cpuThrottle:{method:'CDP Emulation.setCPUThrottlingRate',rate:cpuRate,configuredBefore:'same-origin iframe creation and openProbe',scope:'All three sequential worlds share the throttled renderer. This is controlled recovery call cost, not real multiplayer or native rAF presentation cadence.'}};
let hostProbe=null,hostLoadSnapshot;
const finishHost=async()=>{if(hostProbe&&!hostLoadSnapshot)hostLoadSnapshot=await hostProbe.stop();return hostLoadSnapshot;};
let host,browser,page,cdp,retainedResult,browserVersion,partialCapture;
const errors=[],cleanupErrors=[];
try {
  host=await presentationServer(0,{artifactDirectory});
  browser=await launchBrowser({args:[...(hardware?['--enable-gpu','--use-gl=angle','--use-angle=d3d11']:['--enable-unsafe-swiftshader']),'--autoplay-policy=no-user-gesture-required']});
  page=await browser.newPage();page.on('pageerror',e=>errors.push(e.stack));
  hostProbe=hostLoad?startHostLoadProbe():null;hostProbe?.mark('controlled-peer-page-ready');
  await page.goto(host.url);
  cdp=await page.context().newCDPSession(page);
  await cdp.send('Emulation.setCPUThrottlingRate',{rate:cpuRate});
  // Keep the owning CDP session alive for the complete measured workload.
  browserVersion=browser.version();
  hostProbe?.mark('controlled-peer-throttle-configured');
  const result=await page.evaluate(async({fixedDelay,forceCorrection,limit,maximumIterations,hardware,pauseTrace,inputTrace,adonisMode,inputDelay,captureTrace,deliveryClock})=>{
    const frames=[],startedAt=performance.now(),timeOriginMs=performance.timeOrigin;
    for(let i=0;i<3;++i){const frame=document.createElement('iframe');frame.src='/';document.body.append(frame);await new Promise(r=>frame.onload=r);frames.push(frame.contentWindow);}
    for(const w of frames)await w.openProbe(0,1,2,3,true);
    const peers=frames.slice(0,2),pending=[],captures=[[],[]],deliveryFrontier=[0,0];let clock=0,sequence=0;
    const stats=w=>{const at=w.core._th09_rollback_info()/4;return Array.from(w.core.HEAPU32.subarray(at,at+8));};
    const error=w=>{const c=w.core,at=c._th09_error();return new TextDecoder().decode(c.HEAPU8.subarray(at,c.HEAPU8.indexOf(0,at)));};
    const send=(side,reliable,bytes)=>{
      const input=bytes[0]===69&&bytes[5]===1;
      if(captureTrace&&input){
        const view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength),first=view.getUint32(32,true),count=bytes[42];
        if(bytes.length!==44+count*12)throw Error('Unexpected input packet size');
        for(let i=0;i<count;++i){const f=first+i,at=44+i*12,bits=view.getUint16(at,true),trace=captures[side];
          if(f<trace.length){if(trace[f][5]!==bits)throw Error('Captured input was rewritten');continue;}
          if(f!==trace.length)throw Error('Wire capture gap');
          const current=stats(peers[side]);trace.push([clock,f,current[0],current[2],current[3],bits]);
        }
      }
      const n=++sequence;
      if(!fixedDelay&&input&&!reliable&&n%7===0)return;
      let delay=input?(fixedDelay||2+n%3):0;
      // Both lanes experience the same long spike, then recover.
      if(!fixedDelay&&input&&clock>=220&&clock<245)delay+=48;
      const view=input&&deliveryClock==='frontier'?new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength):null;
      const dueFrame=view&&bytes[42]?view.getUint32(32,true)+bytes[42]-1+fixedDelay:null;
      pending.push({to:1-side,due:clock+delay,dueFrame,bytes:new Uint8Array(bytes)});
      if(!fixedDelay&&input&&n%11===0)pending.push({to:1-side,due:clock+delay+5,bytes:new Uint8Array(bytes)});
    };
    for(let side=0;side<3;++side){const w=frames[side];const dc=reliable=>({readyState:'open',bufferedAmount:0,send:bytes=>{if(side<2)send(side,reliable,bytes);}});
      w.__eaglerPeerTransport={route:'rtc',localPlayer:side%2,playerCount:2,received:[],receivedHead:0,peers:new Map([[1-side%2,{inputOpen:true,controlOpen:true,inputDc:dc(false),controlDc:dc(true)}]])};
      if(!w.core._th09_adonis_configure(adonisMode)||!w.core._th09_rollback_begin(12345,side%2,3,0,1,0x1234,0x5678,0x901,inputDelay))throw Error('begin '+error(w));
    }
    const gameplayKeys=(side,f)=>inputTrace==='endurance-tape'?(f===0?0:
      (f<132?256:0)|(f%126<90?1:0)|(f%180<45?4:0)|((Math.floor(f/22)+side)%4===0?64:0)|((Math.floor(f/22)+side)%4===2?128:0)):
      inputTrace==='charged-strafe-alternating-focus'?
      (f<140?256:0)|(f%126<90?1:0)|((Math.floor(f/22)+side)%4===0?64:(Math.floor(f/22)+side)%4===2?128:0)|(f%2?4:0):
      (f<140?256:0)|(f%18<12?1:0)|((Math.floor(f/37)+side)%2?64:128)|(f%217===0?2:0);
    const keys=(side,f)=>gameplayKeys(side,f)|(pauseTrace&&(side===0&&(f===400||f===480)||side===1&&(f===900||f===980))?8:0);
    const physical=(w,bits)=>{for(const [bit,scan] of [[1,44],[2,45],[4,42],[8,1],[64,203],[128,205],[256,29]])w.core._th09_key(scan,+(!!(bits&bit)));};
    const deliver=()=>{
      for(let side=0;side<2;++side){deliveryFrontier[side]=Math.max(deliveryFrontier[side],stats(peers[side])[0]);if(deliveryFrontier[side]>=limit)++deliveryFrontier[side];}
      // Delaying by receiving logical frontier keeps the requested depth
      // meaningful when one recovery spans many callbacks. At the DEV limit,
      // continue delivering its finite tail without simulating extra frames.
      for(let i=pending.length-1;i>=0;--i)if(pending[i].dueFrame!=null?deliveryFrontier[pending[i].to]>=pending[i].dueFrame:pending[i].due<=clock){const packet=pending.splice(i,1)[0];peers[packet.to].__eaglerPeerTransport.received.push(packet.bytes);}
    };
    if(fixedDelay){
      // Finish the ordinary handshake on BOTH peers before either captures
      // frame zero. Otherwise an iteration-order offset changes the requested
      // depth by one frame, even though both worlds are behaving correctly.
      for(let attempt=0;attempt<64&&!peers.every(w=>stats(w)[6]);++attempt){deliver();for(const w of peers)if(!w.core._th09_rollback_pump())throw Error(error(w));++clock;}
      if(!peers.every(w=>stats(w)[6]))throw Error('Fixed-depth handshake incomplete');
    }
    for(const w of peers)w.core._th09_probe_frame_limit(limit);
    const ticks=[0,0],waits=[0,0],milliseconds=[0,0],samples=[[],[]],peakBullets=[0,0],peakEnemies=[0,0],phases=[new Set(),new Set()],pauseCalls=[0,0],preRecoveryCaptures=[0,0],partialRecoveryCalls=[0,0];let iterations=0;
    // Read only on failure, with no extra work in the tick or delivery path.
    globalThis.__th09ControlledPeerSnapshot=()=>({partial:true,frames:limit,iterations,clock,summary:peers.map(stats),ticks,waits,milliseconds,samples,captures,peakBullets,peakEnemies,phases:phases.map(s=>[...s]),pauseCalls,preRecoveryCaptures,partialRecoveryCalls,pendingPackets:pending.length,deliveryFrontier,observationClock:{timeOriginMs,startedAt,endedAt:performance.now(),includes:'partial controlled workload; not gameplay FPS'}});
    while(iterations++<maximumIterations){
      deliver();
      for(let side=0;side<2;++side){const w=peers[side],s=stats(w),capturedBefore=captures[side].length;physical(w,keys(side,s[4]));const begin=performance.now();let ok=1;
        if(s[0]<=limit){ok=w.core._th09_game_tick(hardware?1:0);if(!ok)throw Error(`peer ${side} frame ${s[0]}: ${error(w)}`);if(ok===2)++waits[side];++ticks[side];}
        else if(!w.core._th09_rollback_pump())throw Error(error(w));
        const work=performance.now()-begin,after=stats(w);milliseconds[side]+=work;
        if(after[0]<s[0])++partialRecoveryCalls[side];
        for(let i=capturedBefore;i<captures[side].length;++i)if(captures[side][i][3]<after[2])++preRecoveryCaptures[side];
        let bullets=null,enemies=null;if(w.core._th09_probe_load){const at=w.core._th09_probe_load()/4;bullets=w.core.HEAPU32[at]+w.core.HEAPU32[at+1];enemies=w.core.HEAPU32[at+2]+w.core.HEAPU32[at+3];peakBullets[side]=Math.max(peakBullets[side],bullets);peakEnemies[side]=Math.max(peakEnemies[side],enemies);}
        const phase=w.core.HEAP32[w.core._th09_session_status()/4];phases[side].add(phase);
        pauseCalls[side]+=w.core.HEAP32[w.core._th09_probe_status()/4+9]!==0;
        // With an active session, success includes exactly one fresh frontier
        // tick; a recovery-only/wait return includes none. This reconstructs
        // the restore distance even when its replay spans several callbacks.
        const restoredDepth=after[2]-s[2]===1?s[0]+after[3]-s[3]+(ok===1?1:0)-after[0]:0;
        if(restoredDepth<0||restoredDepth>8)throw Error('Invalid reconstructed restore distance');
        samples[side].push([s[0],after[0],after[2]-s[2],after[3]-s[3],work,ok,bullets,phase,restoredDepth,enemies]);
      }
      ++clock;
      const s=peers.map(stats);if(s.every(v=>v[0]===limit&&v[1]===limit))break;
      if(iterations%8===0)await new Promise(r=>setTimeout(r,0));
    }
    const summary=peers.map(stats);if(!summary.every(v=>v[0]===limit&&v[1]===limit&&v[4]===limit&&(adonisMode===1?v[2]===0&&v[3]===0:v[2]>0)))throw Error('incomplete '+JSON.stringify(summary));
    if(captureTrace)for(let side=0;side<2;++side){
      if(captures[side].length!==limit)throw Error('Extra/missing physical capture at frame limit');
      for(const c of captures[side])if(c[1]!==c[2]||c[5]!==keys(side,c[1]))throw Error('Input captured on a historical frame or with wrong physical intent');
      if(!preRecoveryCaptures[side])throw Error('Pre-recovery input publication was not exercised');
    }
    const adonis=peers.map(w=>{const at=w.core._th09_adonis_info()/4;return Array.from(w.core.HEAPU32.subarray(at,at+6));});
    if(adonis.some(v=>v[0]!==adonisMode||v[1]!==inputDelay||(adonisMode===1&&v[2]!==0)))throw Error('wrong timing/snapshot mode '+JSON.stringify(adonis));
    if(pauseTrace&&!pauseCalls.every(n=>n>20))throw Error('Pause path was not exercised');
    if(inputTrace==='endurance-tape'&&limit>=4400&&!phases.every(p=>p.has(3)))throw Error('Results path was not exercised '+JSON.stringify({frames:limit,phases:phases.map(p=>[...p]),pauseCalls,status:peers.map(w=>w.probeStatus())}));
    const costs=samples.map(rows=>{const steady=rows.filter(r=>r[0]>=180&&r[0]<limit),sorted=steady.map(r=>r[4]).sort((a,b)=>a-b);
      return {calls:steady.length,p50:sorted[Math.floor(sorted.length*.5)],p95:sorted[Math.floor(sorted.length*.95)],p99:sorted[Math.floor(sorted.length*.99)],max:sorted.at(-1),
        correctionCalls:steady.filter(r=>r[2]>0).length,requestedDeepCalls:steady.filter(r=>r[2]===1&&r[8]>=fixedDelay).length,completeDeepCalls:steady.filter(r=>r[2]===1&&r[3]>=fixedDelay&&r[5]===1).length,maxRequestedDepth:Math.max(...steady.map(r=>r[8])),maxResimulatedInCall:Math.max(...steady.map(r=>r[3]))};});
    if(forceCorrection&&!costs.every(c=>c.requestedDeepCalls>Math.min(100,(limit-180)/4)))throw Error('Sustained requested-depth recovery not exercised '+JSON.stringify(costs));
    const reference=frames[2];for(let f=0;f<limit;++f)if(!reference.core._th09_probe_rollback_step(f<inputDelay?0:keys(0,f-inputDelay),f<inputDelay?0:keys(1,f-inputDelay),0))throw Error('reference '+error(reference));
    const hashes=frames.map(w=>w.core._th09_network_hash()>>>0);
    if(new Set(hashes).size!==1)throw Error('reference divergence '+JSON.stringify(hashes));
    const audited=frames.map(w=>{const c=w.core,at=c._th09_probe_world()/4;return Array.from(c.HEAP32.subarray(at,at+48));});
    if(new Set(audited.map(a=>JSON.stringify(a))).size!==1)throw Error('world audit differs from exact-input reference');
    const images=frames.map(w=>{w.core._th09_game_draw();return w.document.querySelector('canvas').toDataURL();});
    if(new Set(images).size!==1)throw Error('framebuffer differs from exact-input reference');
    const replays=frames.map(w=>{if(!w.core._th09_probe_save_replay())throw Error('replay '+error(w));return w.core.FS.readFile('/save/replay/th9_25.rpy');});
    if(!replays.slice(1).every(b=>b.length===replays[0].length&&b.every((v,i)=>v===replays[0][i])))throw Error('replay differs from exact-input reference');
    const gl=frames[0].document.querySelector('canvas').getContext('webgl2'),ext=gl.getExtension('WEBGL_debug_renderer_info');
    return {frames:limit,iterations,summary,adonis,hashes,audited,framebuffersEqual:true,replayBytes:replays[0].length,ticks,waits,milliseconds,costs,peakBullets,peakEnemies,samples,phases:phases.map(s=>[...s]),pauseCalls,captures,preRecoveryCaptures,partialRecoveryCalls,
      sampleColumns:['nextBefore','nextAfter','correctionsDelta','resimulatedDelta','nativeCallMs','returnCode','aliveBullets','sessionPhase','requestedRestoreDepth','aliveEnemies'],
      observationClock:{timeOriginMs,startedAt,endedAt:performance.now(),includes:'iframe setup, all three worlds, oracle and readback; not a gameplay throughput timer'},
      renderer:ext?gl.getParameter(ext.UNMASKED_RENDERER_WEBGL):gl.getParameter(gl.RENDERER)};
  },{fixedDelay,forceCorrection,limit,maximumIterations,hardware,pauseTrace,inputTrace,adonisMode,inputDelay,captureTrace,deliveryClock});
  retainedResult=result;
  assert.equal(errors.length,0,errors.join('\n'));
  if(hardware)assert.doesNotMatch(result.renderer,/swiftshader|llvmpipe|microsoft basic render/i);
  const wasm=JSON.parse(readFileSync(resolve(artifactDirectory,'build.json'))).sha256;
  assert.equal(wasm,identity.wasm,'Build manifest does not describe the loaded Wasm');
  const identityAfter=Object.fromEntries(Object.entries(identityFiles).map(([name,path])=>[name,sha(path)]));
  assert.deepEqual(identityAfter,identity,'Controlled peer inputs changed during the run');
  hostProbe?.mark('controlled-peer-gameplay-and-oracles-finished');await finishHost();
  writeFileSync(reportPath,JSON.stringify({passed:true,wasm,harnessSha256:identity.harness,identity,identityAfter,browserVersion,parameters,scope:'Real TH09 per-frame history ring, packet/session and gameplay; injected '+deliveryClock+'-delay transport; three sequential worlds in one CPU-throttled Chromium renderer, not real RTC wall-clock pacing, native rAF presentation cadence or public TURN',result,errors,hostLoad:hostLoadSnapshot},null,2),{flag:'wx'});
  console.log(JSON.stringify({...result,samples:undefined,captures:undefined}));
} catch(error) {
  hostProbe?.mark('controlled-peer-failure');
  if(!retainedResult&&page){
    let partialTimer;
    try{
      retainedResult=await Promise.race([page.evaluate(()=>globalThis.__th09ControlledPeerSnapshot?.()??null),new Promise((_,reject)=>{partialTimer=setTimeout(()=>reject(Error('Partial controlled-peer snapshot timed out after 5000 ms')),5000);})]);
      partialCapture={status:retainedResult?'captured':'not-installed'};
    }catch(partialError){partialCapture={status:'failed',error:String(partialError)};}
    finally{clearTimeout(partialTimer);}
  }
  try{await finishHost();}catch(stopError){cleanupErrors.push({stage:'host-stop',error:String(stopError)});}
  writeFileSync(failurePath,JSON.stringify({error:error.stack,errors,identity,parameters,browserVersion,result:retainedResult,partialCapture,hostLoad:hostLoadSnapshot,cleanupErrors},null,2),{flag:'wx'});throw error;
} finally {
  for(const [stage,close] of [['host-stop',finishHost],['cdp-detach',()=>cdp?.detach()],['browser-close',()=>browser?.close()],['relay-close',()=>host?.netplay.close()],['server-close',()=>host?new Promise((accept,reject)=>host.server.close(error=>error?reject(error):accept())):undefined]]){
    try{await close();}catch(error){cleanupErrors.push({stage,error:String(error)});}
  }
  writeFileSync(cleanupPath,JSON.stringify({complete:cleanupErrors.length===0,errors:cleanupErrors},null,2),{flag:'wx'});
  if(cleanupErrors.length)process.exitCode=1;
}
