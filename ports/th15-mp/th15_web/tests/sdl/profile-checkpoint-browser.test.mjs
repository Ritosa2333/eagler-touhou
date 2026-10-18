import test from 'node:test';
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,readdir,mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const root=fileURLToPath(new URL('../../',import.meta.url));
test('Profile real chapter/spell progression before and after bounded checkpoint compression', {timeout:600000}, async()=>{
 assert.ok(process.env.TH15_ORIGINAL_DAT,'Set TH15_ORIGINAL_DAT to the private original th15.dat');
 const fonts=(await readdir(resolve(root,'assets/sdl-native/fonts'))).filter(n=>n.endsWith('.bin'));
 const files=new Map([['/module.mjs',resolve(root,'artifacts/sdl-application/th15-application.mjs')],['/th15-application.wasm',resolve(root,'artifacts/sdl-application/th15-application.wasm')],['/th15.dat',process.env.TH15_ORIGINAL_DAT],...fonts.map(n=>['/fonts/'+n,resolve(root,'assets/sdl-native/fonts',n)])]);
 const server=createServer(async(req,res)=>{try{const path=new URL(req.url,'http://localhost').pathname;res.setHeader('Cross-Origin-Opener-Policy','same-origin');res.setHeader('Cross-Origin-Embedder-Policy','require-corp');if(path==='/'){res.setHeader('Content-Type','text/html');res.end('<canvas id="canvas" width="1280" height="960"></canvas><script type="module">import create from "/module.mjs";window.fixture=await create({canvas:document.querySelector("canvas")});</script>');return;}const file=files.get(path);if(!file){res.writeHead(404).end();return;}res.setHeader('Content-Type',path.endsWith('.mjs')?'text/javascript':path.endsWith('.wasm')?'application/wasm':'application/octet-stream');res.end(await readFile(file));}catch(e){res.writeHead(500).end(String(e));}});
 await new Promise(r=>server.listen(0,'127.0.0.1',r));let browser;const errors=[];const output=resolve(root,'artifacts/cpp/verification');await mkdir(output,{recursive:true});
 try{
 browser=await launchBrowser();const observations=[];
 const variants=process.env.TH15_PROFILE_BASELINE?[['baseline',process.env.TH15_PROFILE_BASELINE],['current',resolve(root,'artifacts/sdl-application')]]:[['current',resolve(root,'artifacts/sdl-application')]];
 for(const [label,build] of variants){
 files.set('/module.mjs',resolve(build,'th15-application.mjs'));files.set('/th15-application.wasm',resolve(build,'th15-application.wasm'));
 const page=await browser.newPage();await page.goto('http://127.0.0.1:'+server.address().port+'/');await page.waitForFunction(()=>window.fixture,{timeout:90000});
 await page.evaluate(async fonts=>{const c=fixture;c.FS.mkdir('/fonts');c.FS.mkdir('/save');for(const name of fonts)c.FS.writeFile('/fonts/'+name,new Uint8Array(await(await fetch('/fonts/'+name)).arrayBuffer()));c.FS.writeFile('/th15.dat',new Uint8Array(await(await fetch('/th15.dat')).arrayBuffer()));c._application_render_scale(2);c._application_test_music_enabled(0);window.decode=p=>new TextDecoder().decode(c.HEAPU8.subarray(p,c.HEAPU8.indexOf(0,p)));window.state=()=>Array.from(c.HEAP32.subarray(c._application_state()/4,c._application_state()/4+16));window.checkpoint=()=>Array.from(c.HEAP32.subarray(c._application_checkpoint_state()/4,c._application_checkpoint_state()/4+9));window.step=(held=0,pressed=0)=>{if(!c._application_step(held,pressed,0,60,0))throw Error(decode(c._application_error())+' state='+state());return state();};window.key=k=>{step(k,k);step();};window.waitMenu=screen=>{for(let n=0;n<350;n++){const s=step();if(s[0]===0&&s[8]===screen&&s[9]===2)return;}throw Error('Menu timed out '+state());};window.choose=(cursor,screen)=>{c._application_title_select(cursor);key(1);waitMenu(screen);};if(!c._application_initialize(0))throw Error(decode(c._application_error()));waitMenu(1);choose(0,5);choose(0,6);choose(1,7);key(1);while(state()[0]!==1)step();c._application_test_protection(100000);},fonts);
 const frames=[];for(let batch=0;batch<25;batch++)frames.push(...await page.evaluate(()=>{const values=[];for(let n=0;n<120;n++){const start=performance.now();step(0x201,n%30===0?1:0);const elapsed=performance.now()-start;const profile=Array.from(new Float64Array(fixture.HEAPU8.buffer,fixture._th15_probe_performance(),4));const cp=checkpoint();values.push({elapsed,profile,frame:state()[5],chapter:cp[4],bytes:cp[7]});}return values;}));
 const sorted=frames.map(f=>f.elapsed).sort((a,b)=>a-b),value={label,median:sorted[Math.floor(sorted.length*.5)],p95:sorted[Math.floor(sorted.length*.95)],p99:sorted[Math.floor(sorted.length*.99)],max:sorted.at(-1),slowest:[...frames].sort((a,b)=>b.elapsed-a.elapsed).slice(0,12),chapters:[...new Set(frames.map(f=>f.chapter))],checkpointBytes:frames.at(-1).bytes};
 assert.ok(value.checkpointBytes>96);console.log(JSON.stringify(value));observations.push(value);await writeFile(resolve(output,'profile-'+label+'.json'),JSON.stringify({value,frames,scope:'Desktop Chromium SwiftShader CPU submission timing, protected real stage progression, music disabled; not mobile hardware FPS.'},null,2));await page.close();
 }
 await writeFile(resolve(output,'profile-comparison.json'),JSON.stringify(observations,null,2));
 }finally{await browser?.close();await new Promise(r=>server.close(r));}
});
