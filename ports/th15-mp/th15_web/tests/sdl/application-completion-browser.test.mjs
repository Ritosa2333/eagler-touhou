import test from 'node:test';
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,writeFile,readdir} from 'node:fs/promises';
import {resolve} from 'node:path';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
import {root,sha,report} from '../cpp/helpers.mjs';
const assets=(await readdir(resolve(root,'reference/assets'))).filter(n=>!n.endsWith('.dat'));
const fonts=(await readdir(resolve(root,'assets/sdl-native/fonts'))).filter(n=>n.endsWith('.bin'));
const songs=(await readdir(resolve(root,'assets/music'))).filter(n=>n.endsWith('.ogg'));

test('Actual application completes normal ending and Extra results and saves each complete replay',{timeout:1200000},async()=>{
 const files=new Map([['/module.mjs',resolve(root,'artifacts/sdl-application/th15-application.mjs')],['/th15-application.wasm',resolve(root,'artifacts/sdl-application/th15-application.wasm')],...assets.map(n=>['/assets/'+n,resolve(root,'reference/assets',n)]),...fonts.map(n=>['/fonts/'+n,resolve(root,'assets/sdl-native/fonts',n)]),...songs.map(n=>['/music/'+n,resolve(root,'assets/music',n)])]);
 const html='<!doctype html><title>TH15 application integration development</title><canvas id="canvas" width="640" height="480"></canvas><script type="module">import create from "/module.mjs";window.fixture=await create({canvas:document.querySelector("canvas")});</script>';
 const server=createServer(async(req,res)=>{try{const path=new URL(req.url,'http://localhost').pathname;res.setHeader('Cross-Origin-Opener-Policy','same-origin');res.setHeader('Cross-Origin-Embedder-Policy','require-corp');if(path==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}const file=files.get(path);if(!file){res.writeHead(404).end();return;}res.setHeader('Content-Type',path.endsWith('.mjs')?'text/javascript':path.endsWith('.wasm')?'application/wasm':'application/octet-stream');res.end(await readFile(file));}catch(e){res.writeHead(500).end(String(e));}});
 await new Promise(r=>server.listen(0,'127.0.0.1',r));let browser;const errors=[],observations=[];
 try{
  browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']});const page=await browser.newPage();page.on('pageerror',e=>errors.push(String(e)));const url='http://127.0.0.1:'+server.address().port+'/';const prepare=async()=>{await page.goto(url);await page.waitForFunction(()=>window.fixture,{timeout:90000});
  await page.evaluate(async({assets,fonts,songs})=>{const c=fixture;for(const dir of ['assets','fonts','music'])c.FS.mkdir('/'+dir);for(const [dir,names] of [['assets',assets],['fonts',fonts],['music',songs]])for(const name of names){const response=await fetch('/'+dir+'/'+name);if(!response.ok)throw Error('Missing '+name);c.FS.writeFile('/'+dir+'/'+name,new Uint8Array(await response.arrayBuffer()));}c.FS.mkdir('/save');c.FS.mount(c.IDBFS,{},'/save');await new Promise((ok,no)=>c.FS.syncfs(true,e=>e?no(e):ok()));},{assets,fonts,songs});};

  const routes=[];
  for(const extra of [false,true]){
   await prepare();
   await page.evaluate(extra=>{const c=fixture;window.decode=p=>new TextDecoder().decode(c.HEAPU8.subarray(p,c.HEAPU8.indexOf(0,p)));window.state=()=>Array.from(c.HEAP32.subarray(c._application_state()/4,c._application_state()/4+16));window.step=(held=0,pressed=0)=>{if(!c._application_step(held,pressed,0,60,0))throw Error((decode(c._application_error())||'Application stopped')+' state='+state());return state();};window.frames=n=>{for(let i=0;i<n;i++)step();};window.key=k=>{step(k,k);step();};window.waitMenu=(screen,phase=2,limit=350)=>{for(let n=0;n<limit;n++){const s=step();if(s[0]===0&&s[8]===screen&&s[9]===phase)return s;}throw Error('Menu timed out '+screen+' '+state());};window.choose=(cursor,screen)=>{c._application_title_select(cursor);key(1);return waitMenu(screen);};if(!c._application_initialize(0))throw Error(decode(c._application_error()));waitMenu(1);
    if(extra){choose(4,11);for(let i=0;i<3;i++)key(32);for(let i=0;i<3;i++)key(128);for(const k of 'HEARTLAND'){c._application_keyboard_key(k.charCodeAt(0),1);frames(2);c._application_keyboard_key(k.charCodeAt(0),0);frames(2);}key(2);waitMenu(1);choose(1,6);key(1);waitMenu(7);}else{choose(0,5);choose(1,6);choose(1,7);}key(1);for(let n=0;n<250&&state()[0]!==1;n++)step();if(state()[0]!==1)throw Error('Game entry failed '+state());c._application_test_protection(100000);frames(100);
   },extra);
   const stages=extra?[7]:[1,2,3,4,5,6],route={extra,stages:[],endingFrames:0};
   for(const stage of stages){
    const before=await page.evaluate(()=>state());assert.equal(before[2],stage);assert.equal(before[0],1);console.log("completing stage",stage,"extra",extra,"state",before);
    const after=await page.evaluate(({final,stage})=>{if(!fixture._application_complete_stage())throw Error(decode(fixture._application_error()));for(let n=0;n<1000;n++){const s=step();if(final?s[0]!==1:s[2]!==stage){if(!final&&s[0]===1){fixture._application_test_protection(100000);frames(245);}return state();}}throw Error('Completion timed out '+state());},{final:stage===stages.at(-1),stage});
    route.stages.push({stage,after});
   }
   const result=await page.evaluate(()=>{let n=0;while(state()[0]===2&&n<30000){step(0x201,n%35===0?1:0);n++;}if(state()[0]!==0)throw Error('Ending did not return to results '+state());waitMenu(15);return {frames:n,state:state(),music:decode(fixture._application_music())};});route.endingFrames=result.frames;route.result=result;assert.equal(result.state[8],15);if(extra)assert.equal(result.frames,0);else assert.ok(result.frames>0);
   console.log('completion result',JSON.stringify(result));
   const canvas=await page.evaluate(()=>({count:document.querySelectorAll('canvas').length,html:document.body.innerHTML.slice(0,600),width:document.querySelector('canvas')?.width,height:document.querySelector('canvas')?.height}));console.log('completion canvas',canvas);assert.equal(canvas.count,1);
   await page.locator('canvas').screenshot({path:resolve(root,'artifacts/cpp/verification/application-completion-'+(extra?'extra':'normal')+'.png'),timeout:120000});
   const saved=await page.evaluate(()=>{key(1);key(16);key(64);key(1);waitMenu(16);key(1);waitMenu(16,3);key(1);waitMenu(16);const bytes=Array.from(fixture.FS.readFile('/save/replay/th15_01.rpy'));key(2);waitMenu(1);frames(20);return {bytes,state:state(),music:decode(fixture._application_music())};});assert.equal(saved.music,'th15_01.wav');assert.ok(saved.bytes.length>200);await writeFile(resolve(root,'artifacts/cpp/verification/application-completion-'+(extra?'extra':'normal')+'.rpy'),Buffer.from(saved.bytes));delete saved.bytes;route.saved=saved;routes.push(route);await page.evaluate(()=>fixture._application_close());console.log('completion route',extra?'Extra':'normal',JSON.stringify(route));
  }
  assert.deepEqual(errors,[]);report('sdl-application-completion',{passed:true,routes,wasmSha256:sha(await readFile(resolve(root,'artifacts/sdl-application/th15-application.wasm'))),scope:'Actual C++ application, original menus, resources, GPU, ending/staff, results and replay file writing. Controlled stage-completion entry and test-only invulnerability isolate scene lifecycle; this is not proof of natural full-run gameplay equivalence.'});
 }finally{await browser?.close();await new Promise(r=>server.close(r));}
});
