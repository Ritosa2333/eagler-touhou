// Frozen production-release comparison through public spectator feed/tick exports.
// Covers the same tape in two authored builds; no development/original-game oracle.
import assert from 'node:assert/strict';
import {mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const root=fileURLToPath(new URL('../../',import.meta.url));
const dirs=[process.env.PC_REFERENCE,process.env.PC_BUILD].map(p=>{assert.ok(p,'Set PC_REFERENCE and PC_BUILD');return resolve(p);});
const frames=Number(process.env.FRAMES||7200),label=process.env.RUN_LABEL||'release-render';
assert.ok(Number.isInteger(frames)&&frames>=7200&&frames<=12000);assert.match(label,/^[\w-]+$/);
const out=resolve(root,'artifacts/multiplayer-tests');mkdirSync(out,{recursive:true});
const sha=p=>createHash('sha256').update(readFileSync(p)).digest('hex');
const identity={wasm:dirs.map(d=>sha(resolve(d,'th09.wasm'))),harness:sha(fileURLToPath(import.meta.url))};
for(const dir of dirs){const module=new WebAssembly.Module(readFileSync(resolve(dir,'th09.wasm')));assert.ok(!WebAssembly.Module.exports(module).some(e=>e.name.startsWith('th09_probe_')||e.name==='th09_title_open'));}
const browser=await launchBrowser({args:['--enable-gpu','--use-gl=angle','--use-angle=d3d11','--autoplay-policy=no-user-gesture-required']});
const hosts=[],pages=[],errors=[],checks=[];
const points=[...new Set([...Array.from({length:Math.floor(frames/120)},(_,i)=>(i+1)*120),203,204,401,430,479,481,847,848,901,918,950,979,981,frames])].sort((a,b)=>a-b);
try{
 for(let side=0;side<2;++side){
  const host=await presentationServer(0,{release:true,artifactDirectory:dirs[side]});hosts.push(host);
  const page=await browser.newPage();pages.push(page);page.on('pageerror',e=>errors.push({side,error:e.stack}));
  await page.goto(host.url);await page.evaluate(async()=>{
   await openProbe(0,1,2,3,true);
   if(!core._th09_spectator_begin(12345,3,0,1))throw Error(probeError());
   window.comparisonPhases=new Set();window.comparisonFrame=0;
  });
 }
 for(const end of points){
  const pair=[];
  for(const page of pages)pair.push(await page.evaluate(async end=>{
   const keys=(side,f)=>{const cycle=Math.floor(f/22)+side;return (f===0?0:(f<132?256:0)|(f%126<90?1:0)|(f%180<45?4:0)|(cycle%4===0?64:0)|(cycle%4===2?128:0))|((side===0&&(f===400||f===480)||side===1&&(f===900||f===980))?8:0);};
   for(let f=comparisonFrame;f<end;++f){
    if(!core._th09_spectator_feed(f,keys(0,f),keys(1,f),0,0,0,0,0,0)||core._th09_game_tick(1)!==1||core._th09_spectator_frame()!==f+1)throw Error('Public feed/tick failed at '+f+': '+probeError());
    comparisonPhases.add(core.HEAP32[core._th09_session_status()/4]);
   }
   comparisonFrame=end;window.comparisonImage=document.querySelector('canvas').toDataURL();
   const digest=await crypto.subtle.digest('SHA-256',new TextEncoder().encode(comparisonImage));
   const at=core._th09_session_status()/4,p=core._sdl_stats()/4;
   return {frame:end,imageSha256:Array.from(new Uint8Array(digest),b=>b.toString(16).padStart(2,'0')).join(''),hash:core._th09_network_hash()>>>0,session:Array.from(core.HEAP32.subarray(at,at+8)),phases:[...comparisonPhases],sdl:Array.from(core.HEAPU32.subarray(p,p+17))};
  },end));
  checks.push(pair);assert.equal(pair[0].hash,pair[1].hash,'production state at '+end);assert.deepEqual(pair[0].session,pair[1].session,'session at '+end);
  if(pair[0].imageSha256!==pair[1].imageSha256){
   for(let side=0;side<2;++side){const png=await pages[side].evaluate(()=>comparisonImage);writeFileSync(resolve(out,label+'-'+end+'-'+side+'.png'),Buffer.from(png.split(',')[1],'base64'));}
   throw Error('Exact production framebuffer divergence at '+end);
  }
 }
 const renderers=[];
 for(const page of pages)renderers.push(await page.evaluate(()=>{const gl=document.querySelector('canvas').getContext('webgl2'),ext=gl.getExtension('WEBGL_debug_renderer_info');if(gl.getError()!==gl.NO_ERROR)throw Error('GL error');return ext?gl.getParameter(ext.UNMASKED_RENDERER_WEBGL):gl.getParameter(gl.RENDERER);}));
 for(const renderer of renderers)assert.doesNotMatch(renderer,/swiftshader|llvmpipe|microsoft basic render/i);
 assert.equal(errors.length,0,JSON.stringify(errors));for(const c of checks.at(-1))assert.ok(c.phases.includes(3),'Results path was not exercised');
 for(let side=0;side<2;++side)assert.equal(sha(resolve(dirs[side],'th09.wasm')),identity.wasm[side]);
 const report={passed:true,identity,frames,browser:browser.version(),renderers,scope:'Two frozen authored production releases, public spectator feed/tick exports, identical endurance tape including pause edges; exact framebuffer PNG, network hash and session state. No development exports, rollback transport or timed performance claims.',checks,errors};
 writeFileSync(resolve(out,label+'-report.json'),JSON.stringify(report,null,2));console.log(JSON.stringify({...report,checks:undefined,checkpoints:checks.length,final:checks.at(-1)}));
}catch(error){writeFileSync(resolve(out,label+'-failure.json'),JSON.stringify({error:error.stack,identity,frames,checks,errors},null,2));throw error;}
finally{await browser.close();for(const host of hosts){host.netplay.close();await new Promise(r=>host.server.close(r));}}
