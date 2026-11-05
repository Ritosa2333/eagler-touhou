// Exact rendered-output comparison of two frozen authored-code builds.
// No original executable, reconstruction oracle, or timed performance claims.
import assert from 'node:assert/strict';
import {mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {launchBrowser} from '../../../../../tools/browser/launch.mjs';
const root=fileURLToPath(new URL('../../',import.meta.url));
const dirs=[process.env.PC_REFERENCE,process.env.PC_BUILD].map(p=>{assert.ok(p,'Set PC_REFERENCE and PC_BUILD');return resolve(p);});
const label=process.env.RUN_LABEL||'render-equivalence',frames=Number(process.env.FRAMES||1800);
const characters=(process.env.CHARACTERS||'0,1').split(',').map(Number);
assert.ok(Number.isInteger(frames)&&frames>=300&&frames%30===0);
const out=resolve(root,'artifacts/multiplayer-tests');mkdirSync(out,{recursive:true});
const sha=p=>createHash('sha256').update(readFileSync(p)).digest('hex');
const identity={wasm:dirs.map(d=>sha(resolve(d,'th09-presentation.wasm'))),harness:sha(fileURLToPath(import.meta.url))};
const hardware=process.env.NATIVE_GPU==='1',hosts=[],pages=[],errors=[],checks=[];
const browser=await launchBrowser({args:[...(hardware?['--enable-gpu','--use-gl=angle','--use-angle=d3d11']:['--enable-unsafe-swiftshader']),'--autoplay-policy=no-user-gesture-required']});
try{
 for(let side=0;side<2;++side){
  const host=await presentationServer(0,{artifactDirectory:dirs[side]});hosts.push(host);
  const page=await browser.newPage();pages.push(page);page.on('pageerror',e=>errors.push({side,error:e.stack}));
  await page.goto(host.url);await page.evaluate(chars=>openProbe(chars[0],chars[1],2,3),characters);
 }
 for(let first=0;first<frames;first+=30){
  const pair=[];
  for(const page of pages)pair.push(await page.evaluate(async first=>{
   const keys=(f,side)=>{const cycle=Math.floor(f/22)+side;return f===0?0:(f<132?256:0)|(f%126<90?1:0)|(f%180<45?4:0)|(cycle%4===0?64:0)|(cycle%4===2?128:0);};
   for(let f=first;f<first+30;++f)if(!core._th09_probe_rollback_step(keys(f,0),keys(f,1),1))throw Error(probeError());
   window.comparisonImage=document.querySelector('canvas').toDataURL();
   const digest=await crypto.subtle.digest('SHA-256',new TextEncoder().encode(comparisonImage));
   const at=core._th09_probe_world()/4,p=core._sdl_stats()/4,l=core._th09_probe_load()/4;
   return {frame:first+30,imageSha256:Array.from(new Uint8Array(digest),b=>b.toString(16).padStart(2,'0')).join(''),hash:core._th09_network_hash()>>>0,audit:Array.from(core.HEAP32.subarray(at,at+48)),sdl:Array.from(core.HEAPU32.subarray(p,p+17)),load:Array.from(core.HEAPU32.subarray(l,l+20))};
  },first));
  checks.push(pair);
  assert.equal(pair[0].hash,pair[1].hash,'network state at '+(first+30));
  assert.deepEqual(pair[0].audit,pair[1].audit,'world inventory at '+(first+30));
  if(pair[0].imageSha256!==pair[1].imageSha256){
   for(let side=0;side<2;++side){const image=await pages[side].evaluate(()=>comparisonImage);writeFileSync(resolve(out,`${label}-${first+30}-${side?'candidate':'reference'}.png`),Buffer.from(image.split(',')[1],'base64'));}
   throw Error('Exact framebuffer divergence at '+(first+30));
  }
 }
 const renderers=await Promise.all(pages.map(p=>p.evaluate(()=>{const gl=document.querySelector('canvas').getContext('webgl2'),ext=gl.getExtension('WEBGL_debug_renderer_info');return ext?gl.getParameter(ext.UNMASKED_RENDERER_WEBGL):gl.getParameter(gl.RENDERER);} )));
 if(hardware)for(const renderer of renderers)assert.doesNotMatch(renderer,/swiftshader|llvmpipe|microsoft basic render/i);
 assert.equal(errors.length,0,JSON.stringify(errors));
 for(let i=0;i<2;++i)assert.equal(sha(resolve(dirs[i],'th09-presentation.wasm')),identity.wasm[i]);
 const report={passed:true,identity,frames,characters,renderers,browser:browser.version(),scope:'Two frozen authored-code builds, identical deterministic input tape; exact framebuffer PNG, canonical network hash and 48-field world audit every 30 frames; not performance acceptance',checks,errors};
 writeFileSync(resolve(out,label+'-report.json'),JSON.stringify(report,null,2));console.log(JSON.stringify({passed:true,identity,frames,characters,renderers,checkpoints:checks.length,final:checks.at(-1)}));
}catch(error){writeFileSync(resolve(out,label+'-failure.json'),JSON.stringify({error:error.stack,identity,frames,characters,checks,errors},null,2));throw error;}
finally{await browser.close();for(const host of hosts){host.netplay.close();await new Promise(r=>host.server.close(r));}}
