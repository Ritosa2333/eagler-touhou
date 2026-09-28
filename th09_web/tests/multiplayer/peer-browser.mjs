import assert from 'node:assert/strict';
import {readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const root=fileURLToPath(new URL('../../',import.meta.url));
const label=process.env.RUN_LABEL||'peer-browser';assert.match(label,/^[\w-]+$/);
const fixedDelay=Number(process.env.FIXED_DELAY_FRAMES||0),forceCorrection=process.env.FORCE_CORRECTION==='1';
const limit=Number(process.env.PEER_FRAMES||900),hardware=process.env.NATIVE_GPU==='1';
const pauseTrace=process.env.PAUSE_TRACE==='1';
const inputTrace=process.env.PEER_INPUT_TRACE||(forceCorrection?'charged-strafe-alternating-focus':'legacy-zigzag');
assert.ok(['charged-strafe-alternating-focus','legacy-zigzag','endurance-tape'].includes(inputTrace));
assert.ok(!forceCorrection||inputTrace==='charged-strafe-alternating-focus');
assert.ok(Number.isInteger(fixedDelay)&&fixedDelay>=0&&fixedDelay<=8);
assert.ok(Number.isInteger(limit)&&limit>=300&&limit<=12000);
assert.ok(!forceCorrection||fixedDelay>0);
const artifactDirectory=resolve(process.env.PC_BUILD||resolve(root,'artifacts/sdl3'));
const {server,netplay,url}=await presentationServer(0,{artifactDirectory});
const browser=await launchBrowser({args:[...(hardware?['--enable-gpu','--use-gl=angle','--use-angle=d3d11']:['--enable-unsafe-swiftshader']),'--autoplay-policy=no-user-gesture-required']});
const page=await browser.newPage(),errors=[];
page.on('pageerror',e=>errors.push(e.stack));
try {
  await page.goto(url);
  const result=await page.evaluate(async({fixedDelay,forceCorrection,limit,hardware,pauseTrace,inputTrace})=>{
    const frames=[];
    for(let i=0;i<3;++i){const frame=document.createElement('iframe');frame.src='/';document.body.append(frame);await new Promise(r=>frame.onload=r);frames.push(frame.contentWindow);}
    for(const w of frames)await w.openProbe(0,1,2,3,true);
    const peers=frames.slice(0,2),pending=[];let clock=0,sequence=0;
    const stats=w=>{const at=w.core._th09_rollback_info()/4;return Array.from(w.core.HEAPU32.subarray(at,at+8));};
    const error=w=>{const c=w.core,at=c._th09_error();return new TextDecoder().decode(c.HEAPU8.subarray(at,c.HEAPU8.indexOf(0,at)));};
    const send=(side,reliable,bytes)=>{
      const input=bytes[0]===69&&bytes[5]===1;
      const n=++sequence;
      if(!fixedDelay&&input&&!reliable&&n%7===0)return;
      let delay=input?(fixedDelay||2+n%3):0;
      // Both lanes experience the same long spike, then recover.
      if(!fixedDelay&&input&&clock>=220&&clock<245)delay+=48;
      pending.push({to:1-side,due:clock+delay,bytes:new Uint8Array(bytes)});
      if(!fixedDelay&&input&&n%11===0)pending.push({to:1-side,due:clock+delay+5,bytes:new Uint8Array(bytes)});
    };
    for(let side=0;side<3;++side){const w=frames[side];const dc=reliable=>({readyState:'open',bufferedAmount:0,send:bytes=>{if(side<2)send(side,reliable,bytes);}});
      w.__eaglerPeerTransport={route:'rtc',localPlayer:side%2,playerCount:2,received:[],receivedHead:0,peers:new Map([[1-side%2,{inputOpen:true,controlOpen:true,inputDc:dc(false),controlDc:dc(true)}]])};
      if(!w.core._th09_rollback_begin(12345,side%2,3,0,1,0x1234,0x5678,0x901))throw Error('begin '+error(w));
    }
    const gameplayKeys=(side,f)=>inputTrace==='endurance-tape'?(f===0?0:
      (f<132?256:0)|(f%126<90?1:0)|(f%180<45?4:0)|((Math.floor(f/22)+side)%4===0?64:0)|((Math.floor(f/22)+side)%4===2?128:0)):
      inputTrace==='charged-strafe-alternating-focus'?
      (f<140?256:0)|(f%126<90?1:0)|((Math.floor(f/22)+side)%4===0?64:(Math.floor(f/22)+side)%4===2?128:0)|(f%2?4:0):
      (f<140?256:0)|(f%18<12?1:0)|((Math.floor(f/37)+side)%2?64:128)|(f%217===0?2:0);
    const keys=(side,f)=>gameplayKeys(side,f)|(pauseTrace&&(side===0&&(f===400||f===480)||side===1&&(f===900||f===980))?8:0);
    const physical=(w,bits)=>{for(const [bit,scan] of [[1,44],[2,45],[4,42],[8,1],[64,203],[128,205],[256,29]])w.core._th09_key(scan,+(!!(bits&bit)));};
    const deliver=()=>{for(let i=pending.length-1;i>=0;--i)if(pending[i].due<=clock){const packet=pending.splice(i,1)[0];peers[packet.to].__eaglerPeerTransport.received.push(packet.bytes);}};
    if(fixedDelay){
      // Finish the ordinary handshake on BOTH peers before either captures
      // frame zero. Otherwise an iteration-order offset changes the requested
      // depth by one frame, even though both worlds are behaving correctly.
      for(let attempt=0;attempt<64&&!peers.every(w=>stats(w)[6]);++attempt){deliver();for(const w of peers)if(!w.core._th09_rollback_pump())throw Error(error(w));++clock;}
      if(!peers.every(w=>stats(w)[6]))throw Error('Fixed-depth handshake incomplete');
    }
    for(const w of peers)w.core._th09_probe_frame_limit(limit);
    const ticks=[0,0],waits=[0,0],milliseconds=[0,0],samples=[[],[]],peakBullets=[0,0],phases=[new Set(),new Set()],pauseCalls=[0,0];let iterations=0;
    while(iterations++<Math.max(4000,limit*5)){
      deliver();
      for(let side=0;side<2;++side){const w=peers[side],s=stats(w);physical(w,keys(side,s[4]));const begin=performance.now();let ok=1;
        if(s[0]<=limit){ok=w.core._th09_game_tick(hardware?1:0);if(!ok)throw Error(`peer ${side} frame ${s[0]}: ${error(w)}`);if(ok===2)++waits[side];++ticks[side];}
        else if(!w.core._th09_rollback_pump())throw Error(error(w));
        const work=performance.now()-begin,after=stats(w);milliseconds[side]+=work;
        let bullets=null;if(w.core._th09_probe_load){const at=w.core._th09_probe_load()/4;bullets=w.core.HEAPU32[at]+w.core.HEAPU32[at+1];peakBullets[side]=Math.max(peakBullets[side],bullets);}
        const phase=w.core.HEAP32[w.core._th09_session_status()/4];phases[side].add(phase);
        pauseCalls[side]+=w.core.HEAP32[w.core._th09_probe_status()/4+9]!==0;
        samples[side].push([s[0],after[0],after[2]-s[2],after[3]-s[3],work,ok,bullets,phase]);
      }
      ++clock;
      const s=peers.map(stats);if(s.every(v=>v[0]===limit&&v[1]===limit))break;
      if(iterations%8===0)await new Promise(r=>setTimeout(r,0));
    }
    const summary=peers.map(stats);if(!summary.every(v=>v[0]===limit&&v[1]===limit&&v[4]===limit&&v[2]>0))throw Error('incomplete '+JSON.stringify(summary));
    if(pauseTrace&&!pauseCalls.every(n=>n>20))throw Error('Pause path was not exercised');
    if(inputTrace==='endurance-tape'&&limit>=4400&&!phases.every(p=>p.has(3)))throw Error('Results path was not exercised');
    const costs=samples.map(rows=>{const steady=rows.filter(r=>r[0]>=180&&r[0]<limit),sorted=steady.map(r=>r[4]).sort((a,b)=>a-b);
      return {calls:steady.length,p50:sorted[Math.floor(sorted.length*.5)],p95:sorted[Math.floor(sorted.length*.95)],p99:sorted[Math.floor(sorted.length*.99)],max:sorted.at(-1),
        correctionCalls:steady.filter(r=>r[2]>0).length,completeDeepCalls:steady.filter(r=>r[2]===1&&r[3]>=fixedDelay&&r[5]===1).length,maxResimulatedInCall:Math.max(...steady.map(r=>r[3]))};});
    if(forceCorrection&&!costs.every(c=>c.completeDeepCalls>Math.min(100,(limit-180)/4)))throw Error('Sustained requested-depth recovery not exercised '+JSON.stringify(costs));
    const reference=frames[2];for(let f=0;f<limit;++f)if(!reference.core._th09_probe_rollback_step(keys(0,f),keys(1,f),0))throw Error('reference '+error(reference));
    const hashes=frames.map(w=>w.core._th09_network_hash()>>>0);
    if(new Set(hashes).size!==1)throw Error('reference divergence '+JSON.stringify(hashes));
    const audited=frames.map(w=>{const c=w.core,at=c._th09_probe_world()/4;return Array.from(c.HEAP32.subarray(at,at+48));});
    if(new Set(audited.map(a=>JSON.stringify(a))).size!==1)throw Error('world audit differs from exact-input reference');
    const images=frames.map(w=>{w.core._th09_game_draw();return w.document.querySelector('canvas').toDataURL();});
    if(new Set(images).size!==1)throw Error('framebuffer differs from exact-input reference');
    const replays=frames.map(w=>{if(!w.core._th09_probe_save_replay())throw Error('replay '+error(w));return w.core.FS.readFile('/save/replay/th9_25.rpy');});
    if(!replays.slice(1).every(b=>b.length===replays[0].length&&b.every((v,i)=>v===replays[0][i])))throw Error('replay differs from exact-input reference');
    const gl=frames[0].document.querySelector('canvas').getContext('webgl2'),ext=gl.getExtension('WEBGL_debug_renderer_info');
    return {frames:limit,iterations,summary,hashes,audited,framebuffersEqual:true,replayBytes:replays[0].length,ticks,waits,milliseconds,costs,peakBullets,samples,phases:phases.map(s=>[...s]),pauseCalls,
      renderer:ext?gl.getParameter(ext.UNMASKED_RENDERER_WEBGL):gl.getParameter(gl.RENDERER)};
  },{fixedDelay,forceCorrection,limit,hardware,pauseTrace,inputTrace});
  assert.equal(errors.length,0,errors.join('\n'));
  if(hardware)assert.doesNotMatch(result.renderer,/swiftshader|llvmpipe|microsoft basic render/i);
  const wasm=JSON.parse(readFileSync(resolve(artifactDirectory,'build.json'))).sha256;
  const harnessSha256=createHash('sha256').update(readFileSync(fileURLToPath(import.meta.url))).digest('hex');
  writeFileSync(resolve(root,'artifacts/multiplayer-tests',label+'-report.json'),JSON.stringify({passed:true,wasm,harnessSha256,parameters:{fixedDelay,forceCorrection,limit,hardware,pauseTrace,inputTrace},scope:'Real TH09 per-frame history ring, packet/session and gameplay; injected iteration-delay transport; three sequential worlds in one Chromium process, not real RTC wall-clock pacing or public TURN',result,errors},null,2));
  console.log(JSON.stringify({...result,samples:undefined}));
} catch(error) {
  writeFileSync(resolve(root,'artifacts/multiplayer-tests',label+'-failure.json'),JSON.stringify({error:error.stack,errors},null,2));throw error;
} finally {await browser.close();netplay.close();await new Promise(r=>server.close(r));}
