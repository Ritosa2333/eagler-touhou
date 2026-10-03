// Real gesture producer, two rollback worlds and an exact confirmed-input
// reference. Iteration-delay transport isolates work; this is not RTC/WAN or
// input-to-photon timing. Compare frozen builds with the SAME mode and D.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,existsSync,mkdirSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';

const root=fileURLToPath(new URL('../../',import.meta.url));
const label=process.env.RUN_LABEL||'touch-peer';assert.match(label,/^[\w-]+$/);
const artifactDirectory=resolve(process.env.PC_BUILD||resolve(root,'artifacts/sdl-adonis'));
const hardware=process.env.NATIVE_GPU==='1',mode=Number(process.env.ADONIS_MODE??2);
const delay=Number(process.env.INPUT_DELAY_FRAMES??2),limit=900;
assert.ok([0,1,2].includes(mode)&&Number.isInteger(delay)&&delay>=0&&delay<=9);
const output=resolve(root,'artifacts/multiplayer-tests');mkdirSync(output,{recursive:true});
const reportPath=resolve(output,label+'-report.json');
assert.ok(!existsSync(reportPath)&&!existsSync(resolve(output,label+'-failure.json')),'Use a new run label');
const build=JSON.parse(readFileSync(resolve(artifactDirectory,'build.json')));
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const {server,netplay,url}=await presentationServer(0,{artifactDirectory});
const browser=await launchBrowser({args:[...(hardware?['--enable-gpu','--use-gl=angle','--use-angle=d3d11']:['--enable-unsafe-swiftshader']),'--autoplay-policy=no-user-gesture-required']});
const page=await browser.newPage(),errors=[];
page.on('pageerror',e=>errors.push(e.stack));
try{
 await page.goto(url);
 const result=await page.evaluate(async({mode,delay,limit})=>{
  const worlds=[],pending=[],tape=[];let clock=0;
  for(let i=0;i<3;++i){const f=document.createElement('iframe');f.src='/';document.body.append(f);await new Promise(r=>f.onload=r);worlds.push(f.contentWindow);}
  for(const w of worlds)await w.openProbe(0,1,2,3,true);
  const stats=w=>{const at=w.core._th09_rollback_info()/4;return Array.from(w.core.HEAPU32.subarray(at,at+8));};
  const error=w=>{const c=w.core,p=c._th09_error();return new TextDecoder().decode(c.HEAPU8.subarray(p,c.HEAPU8.indexOf(0,p)));};
  const send=(side,bytes)=>{
   const copy=new Uint8Array(bytes),input=copy[0]===69&&copy[5]===1;
   pending.push({to:1-side,due:clock+(input?3:0),bytes:copy});
  };
  for(let side=0;side<2;++side){const w=worlds[side];
   const dc=()=>({readyState:'open',bufferedAmount:0,send:b=>send(side,b)});
   w.__eaglerPeerTransport={route:'rtc',localPlayer:side,playerCount:2,received:[],receivedHead:0,
    peers:new Map([[1-side,{inputOpen:true,controlOpen:true,inputDc:dc(),controlDc:dc()}]])};
   if(!w.core._th09_adonis_configure(mode)||!w.core._th09_rollback_begin(12345,side,3,0,1,0x1234,0x5678,0x901,delay))throw Error('begin '+error(w));
   w.core._th09_probe_frame_limit(limit);w.core._th09_touch_options(1,0,1,0,0);w.core._th09_touch_controls(1,0,0,0,0);
  }
  worlds[0].core.onNetworkSpectatorFrame=(...values)=>{
   if(values[0]!==tape.length)throw Error('Nonsequential confirmed output');tape.push(values);
  };
  const reference=worlds[2];if(!reference.core._th09_spectator_begin(12345,3,0,1))throw Error('reference begin '+error(reference));
  const deliver=()=>{for(let i=pending.length-1;i>=0;--i)if(pending[i].due<=clock){const p=pending.splice(i,1)[0];worlds[p.to].__eaglerPeerTransport.received.push(p.bytes);}};
  for(let n=0;n<64&&!worlds.slice(0,2).every(w=>stats(w)[6]);++n){deliver();for(const w of worlds.slice(0,2))if(!w.core._th09_rollback_pump())throw Error(error(w));++clock;}
  if(!worlds.slice(0,2).every(w=>stats(w)[6]))throw Error('Handshake failed');
  const injected=[-1,-1],events=[[],[]],samples=[[],[]],motionSamples=[0,0],waits=[0,0];
  const physical=(w,side,f)=>{
   const c=w.core;
   c._th09_key(29,+(f>0&&f<130));c._th09_key(44,+(f%126<90));c._th09_key(42,+(f%180<45));
   if(side!==1)return;
   const event=(type,x,y)=>{c._th09_touch(type,71,x,y);events[side].push([f,type,x,y]);};
   if(f===150){event(0,.5,.7);event(1,.58,.7);}
   if(f===360)event(1,.38,.62);
   if(f===500)event(2,.38,.62);
   if(f===560){event(0,.5,.7);event(1,.6,.74);}
   if(f===700){c._th09_touch_cancel();events[side].push([f,'cancel']);}
  };
  let iterations=0;
  while(++iterations<10000){
   deliver();
   for(let side=0;side<2;++side){const w=worlds[side],before=stats(w);
    if(before[4]<limit&&before[4]!==injected[side]){injected[side]=before[4];physical(w,side,before[4]);}
    const at=performance.now(),ok=w.core._th09_game_tick(1),ms=performance.now()-at;
    if(!ok)throw Error(`P${side+1} ${before[0]}: ${error(w)}`);
    const after=stats(w);samples[side].push([before[0],after[0],after[2]-before[2],after[3]-before[3],ms]);if(ok===2)++waits[side];
   }
   ++clock;
   if(worlds.slice(0,2).every(w=>stats(w)[0]===limit&&stats(w)[1]===limit)&&tape.length===limit)break;
   if(iterations%8===0)await new Promise(r=>setTimeout(r,0));
  }
  const summary=worlds.slice(0,2).map(stats);
  if(!summary.every(v=>v[0]===limit&&v[1]===limit&&v[4]===limit)||tape.length!==limit)throw Error('Incomplete '+JSON.stringify({summary,tape:tape.length}));
  for(const values of tape){
   if(values[3])++motionSamples[0];if(values[6])++motionSamples[1];
   if(!reference.core._th09_spectator_feed(...values)||!reference.core._th09_game_tick(0))throw Error('exact reference '+error(reference));
  }
  if(motionSamples[1]<200)throw Error('Raw drag did not reach native input');
  const hashes=worlds.map(w=>w.core._th09_network_hash()>>>0);
  if(new Set(hashes).size!==1)throw Error('Reference world divergence '+JSON.stringify(hashes));
  const audits=worlds.map(w=>{const c=w.core,at=c._th09_probe_world()/4;return Array.from(c.HEAP32.subarray(at,at+48));});
  if(new Set(audits.map(v=>JSON.stringify(v))).size!==1)throw Error('Reference audit differs');
  const images=worlds.map(w=>{w.core._th09_game_draw();return w.document.querySelector('canvas').toDataURL();});
  if(new Set(images).size!==1)throw Error('Reference framebuffer differs');
  const replays=worlds.map(w=>{if(!w.core._th09_probe_save_replay())throw Error('Replay save');return Array.from(w.core.FS.readFile('/save/replay/th9_25.rpy'));});
  if(new Set(replays.map(v=>JSON.stringify(v))).size!==1)throw Error('Reference Replay bytes differ');
  const distribution=a=>{a=a.slice().sort((x,y)=>x-y);return {count:a.length,p50:a[Math.floor(a.length*.5)],p95:a[Math.floor(a.length*.95)],p99:a[Math.floor(a.length*.99)],max:a.at(-1),total:a.reduce((s,v)=>s+v,0)};};
  const measured=samples.map(rows=>({all:distribution(rows.filter(r=>r[0]>=150&&r[0]<limit).map(r=>r[4])),
   held:distribution(rows.filter(r=>r[0]>=200&&r[0]<340).map(r=>r[4])),resimulated:rows.reduce((s,r)=>s+r[3],0)}));
  const gl=worlds[0].document.querySelector('canvas').getContext('webgl2'),ext=gl.getExtension('WEBGL_debug_renderer_info');
  return {summary,hashes,audits,events,tape,replay:replays[0],motionSamples,measured,samples,waits,iterations,framebuffersEqual:true,
   renderer:ext?gl.getParameter(ext.UNMASKED_RENDERER_WEBGL):gl.getParameter(gl.RENDERER)};
 },{mode,delay,limit});
 assert.equal(errors.length,0,errors.join('\n'));
 if(hardware)assert.doesNotMatch(result.renderer,/swiftshader|llvmpipe|microsoft basic render/i);
 const report={passed:true,scope:'Two real TH09 worlds, native raw gesture producer, complete exact-input spectator/world/framebuffer/Replay reference; fixed three-iteration input delay, not wall-clock RTC or phone evidence',
  wasm:build.sha256,harnessSha256:sha(readFileSync(fileURLToPath(import.meta.url))),parameters:{mode,delay,limit,hardware},
  tapeSha256:sha(JSON.stringify(result.tape)),replaySha256:sha(Uint8Array.from(result.replay)),replayBytes:result.replay.length,result,errors};
 writeFileSync(reportPath,JSON.stringify(report,null,2));
 console.log(JSON.stringify({...report,result:{...result,tape:undefined,replay:undefined,samples:undefined,audits:undefined}}));
}catch(error){writeFileSync(resolve(output,label+'-failure.json'),JSON.stringify({error:error.stack,wasm:build.sha256,parameters:{mode,delay,limit,hardware},errors},null,2));throw error;}
finally{await browser.close();netplay.close();await new Promise(r=>server.close(r));}
