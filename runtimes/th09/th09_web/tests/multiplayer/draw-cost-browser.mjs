// Paired old/new semantic Draw on the same checkpoint, alternating order.
// Full-state and exact framebuffer comparisons are outside timed sections.
import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync,readFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const root=fileURLToPath(new URL('../../',import.meta.url));
const label=process.env.RUN_LABEL||'pc-draw',burst=Number(process.env.BURST||8);
assert.ok([2,4,8].includes(burst));
const characters=(process.env.CHARACTERS||'0,1').split(',').map(Number);
const out=resolve(root,'artifacts/multiplayer-tests');mkdirSync(out,{recursive:true});
const {server,netplay,url}=await presentationServer();
const browser=await launchBrowser({args:['--enable-gpu','--use-gl=angle','--use-angle=d3d11','--autoplay-policy=no-user-gesture-required']});
const page=await browser.newPage(),errors=[];page.on('pageerror',e=>errors.push(e.stack));
try{
 await page.goto(url);await page.evaluate(chars=>openProbe(chars[0],chars[1],2,3),characters);
 const result=await page.evaluate(({burst})=>{
  const c=core,canvas=document.querySelector('canvas'),gl=canvas.getContext('webgl2'),ext=gl.getExtension('WEBGL_debug_renderer_info');
  const renderer=ext?gl.getParameter(ext.UNMASKED_RENDERER_WEBGL):gl.getParameter(gl.RENDERER);
  const samples={optimized:[],reference:[]},capture=[],restore=[];
  const step=(f,render)=>{const left=(sessionStatus()[0]===1&&f%8<4?1:0)|(f<180?256:0),right=(sessionStatus()[0]===1?1:0)|((Math.floor(f/80)%2)?64:128);if(!c._th09_probe_rollback_step(left,right,+render))throw Error(probeError());};
  for(let f=0;f<180;++f)step(f,true);
  const run=(f,mode)=>{
   c._th09_probe_draw_mode(mode);let update=0,draw=0;
   const start=performance.now();
   for(let n=0;n<burst;++n){step(f+n,n===burst-1);const p=c._th09_probe_tick_cost(),v=new Float64Array(c.HEAPU8.buffer,p,2);update+=v[0];draw+=v[1];}
   const total=performance.now()-start;samples[mode?'reference':'optimized'].push({update,draw,total});
  };
  let checkpoints=0,maxBytes=0;
  for(let f=180;f<1780;f+=burst){
   const first=checkpoints%2,started=performance.now(),bytes=c._th09_probe_checkpoint(0);capture.push(performance.now()-started);if(!bytes)throw Error('checkpoint failed');maxBytes=Math.max(maxBytes,bytes);
   run(f,first);const expected=c._th09_probe_state_hash(),image=canvas.toDataURL();
   const text=()=>{const p=c._th09_probe_hash_parts();return new TextDecoder().decode(c.HEAPU8.subarray(p,c.HEAPU8.indexOf(0,p))).split('\n');};
   const before=text(),start=performance.now();if(!c._th09_probe_restore())throw Error('restore failed');restore.push(performance.now()-start);
   run(f,1-first);const actual=c._th09_probe_state_hash();
   if(actual!==expected){const after=text(),i=before.findIndex((v,i)=>v!==after[i]);throw Error(`old/new full-state divergence frame ${f+burst}, part ${i}: ${before[i]} / ${after[i]}`);}
   if(image!==canvas.toDataURL())throw Error('old/new framebuffer divergence frame '+(f+burst));
   ++checkpoints;
  }
  const summarize=a=>{const v=a.slice().sort((a,b)=>a-b);return {count:v.length,total:a.reduce((a,b)=>a+b,0),p50:v[Math.floor(v.length*.5)],p95:v[Math.floor(v.length*.95)],p99:v[Math.floor(v.length*.99)],max:v.at(-1)};};
  const summary=Object.fromEntries(Object.entries(samples).map(([mode,values])=>[mode,Object.fromEntries(['update','draw','total'].map(k=>[k,summarize(values.map(v=>v[k]))]))]));
  return {renderer,checkpoints,burst,maxBytes,fullStateEqual:true,framebuffersEqual:true,summary,capture:summarize(capture),restore:summarize(restore),samples};
 },{burst});
 assert.equal(errors.length,0,errors.join('\n'));assert.doesNotMatch(result.renderer,/swiftshader|llvmpipe|microsoft basic render/i);
 const wasm=createHash('sha256').update(readFileSync(resolve(root,'artifacts/sdl3/th09-presentation.wasm'))).digest('hex');
 const report={passed:true,wasm,browser:browser.version(),characters,scope:'Paired same-checkpoint old/new CPU Draw; alternating order; hardware desktop; not network latency',result,errors};
 writeFileSync(resolve(out,label+'-report.json'),JSON.stringify(report,null,2));console.log(JSON.stringify({...report,result:{...result,samples:undefined}}));
}catch(e){writeFileSync(resolve(out,label+'-failure.json'),JSON.stringify({error:e.stack,errors},null,2));throw e;}
finally{await browser.close();netplay.close();await new Promise(r=>server.close(r));}
