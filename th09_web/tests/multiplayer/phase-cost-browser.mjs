// Frozen desktop comparison: base simulation, then 8-tick correction bursts.
// No network waits, CPU throttling, or hash/readback work inside timed regions.
import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync,readFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const root=fileURLToPath(new URL('../../',import.meta.url));
const artifactDirectory=resolve(process.env.PC_BUILD||resolve(root,'artifacts/sdl3'));
const label=process.env.RUN_LABEL||'pc-phase';
const out=resolve(root,'artifacts/multiplayer-tests');mkdirSync(out,{recursive:true});
const {server,netplay,url}=await presentationServer(0,{artifactDirectory});
const hardware=process.env.NATIVE_GPU==='1';
const browser=await launchBrowser({args:[...(hardware?['--enable-gpu','--use-gl=angle','--use-angle=d3d11']:['--enable-unsafe-swiftshader']),'--autoplay-policy=no-user-gesture-required']});
const page=await browser.newPage(),errors=[];page.on('pageerror',e=>errors.push(e.stack));
try{
 await page.goto(url);await page.evaluate(()=>openProbe(0,1,2,3));
 const result=await page.evaluate(()=>{
  const c=core,canvas=document.querySelector('canvas'),gl=canvas.getContext('webgl2')||canvas.getContext('webgl');
  const ext=gl.getExtension('WEBGL_debug_renderer_info'),renderer=ext?gl.getParameter(ext.UNMASKED_RENDERER_WEBGL):gl.getParameter(gl.RENDERER);
  const phases={base:[],forward:[],resim:[]};let phase=null;
  const step=(f,render)=>{const left=(sessionStatus()[0]===1&&f%8<4?1:0)|(f<180?256:0),right=(sessionStatus()[0]===1?1:0)|((Math.floor(f/80)%2)?64:128);if(!c._th09_probe_rollback_step(left,right,+render))throw Error(probeError());if(phase){const at=c._th09_probe_tick_cost();phases[phase].push(Array.from(new Float64Array(c.HEAPU8.buffer,at,2)));}};
  const timed=fn=>{const start=performance.now();fn();return performance.now()-start;};
  const samples={baseEightTicks:[],capture:[],restore:[],forwardEightTicks:[],resimEightTicks:[],correction:[]},audits=[];
  for(let f=0;f<180;++f)step(f,f%8===7);
  // Base control: no checkpoint, network session or historical replay.
  phase='base';
  for(let f=180;f<500;f+=8)samples.baseEightTicks.push(timed(()=>{for(let n=0;n<8;++n)step(f+n,n===7);}));
  let bytes=0;
  for(let f=500;f<2100;f+=8){
   const capture=timed(()=>{const b=c._th09_probe_checkpoint(0);assertCheckpoint(b);bytes=Math.max(bytes,b);});
   phase='forward';const forward=timed(()=>{for(let n=0;n<8;++n)step(f+n,n===7);});
   const expected=c._th09_probe_state_hash(),image=canvas.toDataURL();
   const restore=timed(()=>{if(!c._th09_probe_restore())throw Error('restore failed');});
   phase='resim';const resim=timed(()=>{for(let n=0;n<8;++n)step(f+n,n===7);});
   if(c._th09_probe_state_hash()!==expected)throw Error('state divergence at '+f);
   if(canvas.toDataURL()!==image)throw Error('framebuffer divergence at '+f);
   const at=c._th09_probe_world()/4;audits.push(Array.from(c.HEAP32.subarray(at,at+48)));
   samples.capture.push(capture);samples.restore.push(restore);samples.forwardEightTicks.push(forward);samples.resimEightTicks.push(resim);samples.correction.push(capture+restore+resim);
  }
  function assertCheckpoint(b){if(!b)throw Error('checkpoint failed');}
  const summarize=a=>{const s=a.slice().sort((a,b)=>a-b);return {count:s.length,total:a.reduce((a,b)=>a+b,0),p50:s[Math.floor(s.length*.5)],p95:s[Math.floor(s.length*.95)],p99:s[Math.floor(s.length*.99)],max:s.at(-1)};};
  return {phases:Object.fromEntries(Object.entries(phases).map(([k,v])=>[k,{ticks:v.length,updateMs:v.reduce((n,x)=>n+x[0],0),drawMs:v.reduce((n,x)=>n+x[1],0)}])),renderer,frames:2100,bytes,summary:Object.fromEntries(Object.entries(samples).map(([k,v])=>[k,summarize(v)])),samples,audits};
 });
 assert.equal(errors.length,0,errors.join('\n'));
 if(hardware)assert.doesNotMatch(result.renderer,/swiftshader|llvmpipe|microsoft basic render/i,'hardware lane fell back to software');
 const wasm=createHash('sha256').update(readFileSync(resolve(artifactDirectory,'th09-presentation.wasm'))).digest('hex');
 const report={passed:true,wasm,browser:browser.version(),hardwareRequested:hardware,scope:'Desktop 1P+CPU versus, full semantic Draw, 200 single-checkpoint 8-tick corrections; CPU cost excludes oracle/readback; not live network latency',result,errors};
 writeFileSync(resolve(out,label+'-report.json'),JSON.stringify(report,null,2));console.log(JSON.stringify({wasm,browser:report.browser,renderer:result.renderer,summary:result.summary,phases:result.phases}));
}catch(e){writeFileSync(resolve(out,label+'-failure.json'),JSON.stringify({error:e.stack,errors},null,2));throw e;}
finally{await browser.close();netplay.close();await new Promise(r=>server.close(r));}
