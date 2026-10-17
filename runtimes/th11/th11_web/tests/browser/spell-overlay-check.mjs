import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';

const root=fileURLToPath(new URL('../../',import.meta.url)),out=resolve(root,'artifacts/sdl3/browser/spell-overlay');mkdirSync(out,{recursive:true});
const wasmSha256=createHash('sha256').update(readFileSync(resolve(root,'artifacts/sdl3/th11-harness.wasm'))).digest('hex');
const tracks=JSON.parse(readFileSync(resolve(root,'assets/sdl-native/music-verification.json'),'utf8'));
const files=new Map([['/th11.mjs',resolve(root,'artifacts/sdl3/th11-harness.mjs')],['/th11-harness.wasm',resolve(root,'artifacts/sdl3/th11-harness.wasm')],['/th11.dat',resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat')]]);
for(const t of tracks)files.set('/music/'+t.file,resolve(root,'assets/sdl-native/music',t.file));
for(const name of ['font0.bin','font1.bin','font2.bin','font3.bin','cp932.bin','blend4444.bin'])files.set('/fonts/'+name,resolve(root,'assets/sdl-native/fonts',name));
const server=createServer((req,res)=>{const name=new URL(req.url,'http://localhost').pathname;res.setHeader('Cross-Origin-Opener-Policy','same-origin');res.setHeader('Cross-Origin-Embedder-Policy','require-corp');if(name==='/'){res.setHeader('Content-Type','text/html');res.end('<!doctype html><title>TH11 audio test</title><canvas id="canvas" width="640" height="480"></canvas>');return;}if(!files.has(name)){res.writeHead(404).end();return;}res.setHeader('Content-Type',name.endsWith('.mjs')?'text/javascript':name.endsWith('.wasm')?'application/wasm':'application/octet-stream');res.end(readFileSync(files.get(name)));});
await new Promise(r=>server.listen(0,'127.0.0.1',r));const url='http://127.0.0.1:'+server.address().port;
let browser;const errors=[];
try{
 browser=await launchBrowser({args:[...(process.env.NATIVE_GPU==='1'?['--enable-gpu','--use-gl=angle','--use-angle=d3d11']:['--enable-unsafe-swiftshader']),'--autoplay-policy=no-user-gesture-required']});const page=await browser.newPage({viewport:{width:680,height:520}});
 page.on('pageerror',e=>errors.push(e.stack));await page.goto(url);await page.evaluate(()=>{window.glLost=false;document.querySelector('canvas').addEventListener('webglcontextlost',()=>{window.glLost=true;});});
 await page.evaluate(async tracks=>{const {default:create}=await import('/th11.mjs');const c=window.core=await create({canvas:document.querySelector('canvas')});c.FS.writeFile('/th11.dat',new Uint8Array(await fetch('/th11.dat').then(r=>r.arrayBuffer())));c.FS.mkdir('/music');await Promise.all(tracks.map(async t=>c.FS.writeFile('/music/'+t.file,new Uint8Array(await fetch('/music/'+t.file).then(r=>r.arrayBuffer())))));c.FS.mkdir('/fonts');for(const name of ['font0.bin','font1.bin','font2.bin','font3.bin','cp932.bin','blend4444.bin'])c.FS.writeFile('/fonts/'+name,new Uint8Array(await fetch('/fonts/'+name).then(r=>r.arrayBuffer())));window.error=()=>{const p=c._th11_error();return new TextDecoder().decode(c.HEAPU8.subarray(p,c.HEAPU8.indexOf(0,p)));};if(!c._th11_initialize())throw Error(error());},tracks);
 const replay=Array.from(readFileSync(resolve(root,'reference/replays/th11_ud0189.rpy')));
 await page.evaluate(bytes=>{const c=core;c.FS.writeFile('/save/replay/th11_01.rpy',Uint8Array.from(bytes));if(!c._th11_return_title())throw Error(error());const tick=(held,n=1)=>{for(let i=0;i<n;++i)if(!c._th11_tick(held))throw Error(error());};const press=held=>{tick(held);tick(0);};tick(0,100);press(32);press(32);press(1);tick(0,40);press(1);tick(0,16);press(1);tick(0,35);if(c._th11_phase()!==1)throw Error('Replay did not start');},replay);
 const snapshots=[];
 for(let frames=0;frames<9000;frames+=300){
  const state=await page.evaluate(()=>{for(let i=0;i<300;++i)if(!core._th11_tick(0))throw Error(error());if(window.glLost)throw Error('WebGL context lost during accelerated test');return {phase:core._th11_phase(),frame:core._th11_frame(),writes:core._th11_text_writes()};});
  assert.equal(state.phase,1);snapshots.push(state);
  if(frames>=3900)await page.locator('canvas').screenshot({path:resolve(out,`frame-${state.frame}.png`)});
 }
 assert.deepEqual(errors,[]);const result={passed:true,physicalDevice:false,hardwareGpu:process.env.NATIVE_GPU==='1',wasmSha256,snapshots,scope:'Original replay, actual SDL3/WebGL2 rendering through stage-one spells. Screenshots for visual inspection; native text comparisons in spell-overlay.test.mjs.'};writeFileSync(resolve(out,'report.json'),JSON.stringify(result,null,2));console.log(JSON.stringify(result));
}catch(e){throw e;}
finally{if(browser)await browser.close();await new Promise(r=>server.close(r));}
