import test from 'node:test';
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,readdir,mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const root=fileURLToPath(new URL('../../',import.meta.url));
test('Original assets: 2x raster, visible loading and movable Stage Clear', {timeout:600000}, async()=>{
 assert.ok(process.env.TH15_ORIGINAL_DAT,'Set TH15_ORIGINAL_DAT to the private original th15.dat');
 const fonts=(await readdir(resolve(root,'assets/sdl-native/fonts'))).filter(n=>n.endsWith('.bin'));
 const files=new Map([['/module.mjs',resolve(root,'artifacts/sdl-application/th15-application.mjs')],['/th15-application.wasm',resolve(root,'artifacts/sdl-application/th15-application.wasm')],['/th15.dat',process.env.TH15_ORIGINAL_DAT],...fonts.map(n=>['/fonts/'+n,resolve(root,'assets/sdl-native/fonts',n)])]);
 const server=createServer(async(req,res)=>{try{const path=new URL(req.url,'http://localhost').pathname;res.setHeader('Cross-Origin-Opener-Policy','same-origin');res.setHeader('Cross-Origin-Embedder-Policy','require-corp');if(path==='/'){res.setHeader('Content-Type','text/html');res.end('<canvas id="canvas" width="1280" height="960"></canvas><script type="module">import create from "/module.mjs";window.fixture=await create({canvas:document.querySelector("canvas")});</script>');return;}const file=files.get(path);if(!file){res.writeHead(404).end();return;}res.setHeader('Content-Type',path.endsWith('.mjs')?'text/javascript':path.endsWith('.wasm')?'application/wasm':'application/octet-stream');res.end(await readFile(file));}catch(e){res.writeHead(500).end(String(e));}});
 await new Promise(r=>server.listen(0,'127.0.0.1',r));let browser;const errors=[];const output=resolve(root,'artifacts/cpp/verification');await mkdir(output,{recursive:true});
 try{
 browser=await launchBrowser();const page=await browser.newPage({viewport:{width:1300,height:1000}});page.on('pageerror',e=>errors.push(String(e)));await page.goto('http://127.0.0.1:'+server.address().port+'/');await page.waitForFunction(()=>window.fixture,{timeout:90000});
 await page.evaluate(async fonts=>{const c=fixture;c.FS.mkdir('/fonts');c.FS.mkdir('/save');c.FS.mount(c.IDBFS,{},'/save');await new Promise((ok,no)=>c.FS.syncfs(true,e=>e?no(e):ok()));for(const name of fonts)c.FS.writeFile('/fonts/'+name,new Uint8Array(await(await fetch('/fonts/'+name)).arrayBuffer()));c.FS.writeFile('/th15.dat',new Uint8Array(await(await fetch('/th15.dat')).arrayBuffer()));c._th15_render_scale(2);c._th15_music_enabled(0);if(!c._th15_prepare_loading())throw Error('Loading failed');},fonts);
 await page.locator('canvas').screenshot({path:resolve(output,'loading-signature.png')});
 await page.evaluate(()=>{const c=fixture;c._application_render_scale(2);c._application_test_music_enabled(0);window.decode=p=>new TextDecoder().decode(c.HEAPU8.subarray(p,c.HEAPU8.indexOf(0,p)));window.state=()=>Array.from(c.HEAP32.subarray(c._application_state()/4,c._application_state()/4+16));window.life=()=>Array.from(new Float32Array(c.HEAPU8.buffer,c._application_test_lifecycle(),6));window.step=(held=0,pressed=0)=>{if(!c._application_step(held,pressed,0,60,0))throw Error(decode(c._application_error())+' state='+state());return state();};window.frames=n=>{for(let i=0;i<n;i++)step();};window.key=k=>{step(k,k);step();};window.waitMenu=(screen,phase=2,limit=350)=>{for(let n=0;n<limit;n++){const s=step();if(s[0]===0&&s[8]===screen&&s[9]===phase)return s;}throw Error('Menu timed out '+state());};window.choose=(cursor,screen)=>{c._application_title_select(cursor);key(1);return waitMenu(screen);};if(!c._application_initialize(0))throw Error(decode(c._application_error()));waitMenu(1);});
 await page.locator('canvas').screenshot({path:resolve(output,'title-2x.png')});
 for(const [cursor,screen,name] of [[3,12,'replay'],[4,11,'player-data']]){
  await page.evaluate(({cursor,screen})=>choose(cursor,screen),{cursor,screen});
  await page.locator('canvas').screenshot({path:resolve(output,name+'-2x.png')});
  await page.evaluate(()=>{key(2);waitMenu(1);});
 }
 const loading=await page.evaluate(()=>{choose(0,5);choose(1,6);choose(1,7);key(1);for(let n=0;n<250;n++){if(life()[4]===1)return {state:state(),life:life()};step();}throw Error('No published game loading frame');});assert.equal(loading.state[0],0);
 await page.locator('canvas').screenshot({path:resolve(output,'loading-game.png')});
 await page.evaluate(()=>{step();if(state()[0]!==1)throw Error('Game entry failed');fixture._application_test_protection(100000);frames(245);});
 await page.locator('canvas').screenshot({path:resolve(output,'game-2x.png')});
 const transition=await page.evaluate(()=>{if(!fixture._application_complete_stage())throw Error(decode(fixture._application_error()));const samples=[];for(let n=0;n<125;n++){const before=life();const s=step(n<20?128:0,n===0?128:0);samples.push({n,stage:s[2],x:life()[0],before:before[0],age:life()[3],pending:life()[4]});if(life()[4]===2)return samples;}throw Error('No stage loading frame');});
 assert.ok(transition.slice(0,20).every(s=>s.pending===0));assert.ok(transition.slice(0,20).every(s=>s.x>s.before),'Stage Clear must continue to update player motion');assert.ok(transition.at(-1).n>=118,'Outgoing clear waits for its own HUD timer');
 await page.locator('canvas').screenshot({path:resolve(output,'loading-next-stage.png')});
 const after=await page.evaluate(()=>{step();return state();});assert.equal(after[2],2);assert.equal(after[0],1);assert.deepEqual(errors,[]);
 await writeFile(resolve(output,'render-loading-lifecycle.json'),JSON.stringify({passed:true,loading,transition,after,scope:'Chromium, original archive, measured fonts, 1280x960 rendering, controlled stage completion; music disabled. Desktop execution does not establish mobile 60 FPS.'},null,2));
 }finally{await browser?.close();await new Promise(r=>server.close(r));}
});
