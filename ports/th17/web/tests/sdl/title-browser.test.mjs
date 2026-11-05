import test from 'node:test';
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {launchBrowser} from '../../../../../tools/browser/launch.mjs';
import {root,report,sha} from '../cpp/helpers.mjs';

test('TH17 original title assets compose through the shared SDL3/WebGL2 renderer at both scales',async()=>{
 const assets=['ascii.anm','title.anm','title_v.anm'];
 const files=new Map([['/module.mjs',resolve(root,'artifacts/sdl-title/th17-title.mjs')],['/th17-title.wasm',resolve(root,'artifacts/sdl-title/th17-title.wasm')],...assets.map(n=>['/assets/'+n,resolve(root,'reference/assets',n)])]);
 const html='<!doctype html><meta charset="utf-8"><title>TH17 development title fixture</title><style>body{margin:0;background:#222}canvas{display:block}</style><canvas id="canvas" width="640" height="480"></canvas><script type="module">import create from "/module.mjs";window.fixture=await create({canvas:document.querySelector("canvas")});</script>';
 const server=createServer(async(req,res)=>{try{const path=new URL(req.url,'http://localhost').pathname;res.setHeader('Cross-Origin-Opener-Policy','same-origin');res.setHeader('Cross-Origin-Embedder-Policy','require-corp');if(path==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}const file=files.get(path);if(!file){res.writeHead(404).end();return;}res.setHeader('Content-Type',path.endsWith('.mjs')?'text/javascript':path.endsWith('.wasm')?'application/wasm':'application/octet-stream');res.end(await readFile(file));}catch(e){res.writeHead(500).end(String(e));}});
 await new Promise(r=>server.listen(0,'127.0.0.1',r));let browser;const observations=[],errors=[];
 await mkdir(resolve(root,'artifacts/browser'),{recursive:true});
 try{
  browser=await launchBrowser();
  for(const scale of [1,2]){
   const context=await browser.newContext({viewport:{width:1280,height:960},hasTouch:scale===2,deviceScaleFactor:scale===2?2:1}),page=await context.newPage();page.on('pageerror',e=>errors.push(String(e)));
   await page.goto('http://127.0.0.1:'+server.address().port+'/');await page.waitForFunction(()=>window.fixture,{timeout:90000});
   await page.evaluate(async({assets,scale})=>{
    const c=window.fixture;c.FS.mkdir('/assets');for(const name of assets){const r=await fetch('/assets/'+name);if(!r.ok)throw Error('Missing '+name);c.FS.writeFile('/assets/'+name,new Uint8Array(await r.arrayBuffer()));}
    const decode=p=>new TextDecoder().decode(c.HEAPU8.subarray(p,c.HEAPU8.indexOf(0,p)));
    window.state=()=>{const p=c._title_state()>>>2;return Array.from(c.HEAP32.subarray(p,p+12));};
    window.step=(pressed=0,repeated=0)=>{if(!c._title_step(pressed,repeated))throw Error(decode(c._title_error())||'Title stopped '+state());return state();};
    window.ready=(screen,limit=200)=>{for(let n=0;n<limit;n++){const s=step();if(s[0]===screen&&s[1]===2)return s;}throw Error('Menu timeout '+state());};
    window.choose=(cursor,screen)=>{c._title_select(cursor);step(1);return ready(screen);};
    window.settle=(n=40)=>{for(let i=0;i<n;i++)step();};
    if(!c._title_initialize(scale))throw Error(decode(c._title_error()));ready(1);settle(40);
   },{assets,scale});
   const inspect=async kind=>{
    const result=await page.evaluate(()=>{const c=fixture,p=c._title_pixels(),s=c._title_graphics_stats()>>>2;return{state:state(),pixels:Array.from(c.HEAPU8.subarray(p,p+640*480*4)),stats:Array.from(c.HEAPU32.subarray(s,s+17)),canvas:[document.querySelector('canvas').width,document.querySelector('canvas').height]};});
    const bytes=Buffer.from(result.pixels);delete result.pixels;let visiblePixels=0;for(let i=0;i<bytes.length;i+=4)if(bytes[i]||bytes[i+1]||bytes[i+2])visiblePixels++;
    const observation={kind,scale,...result,visiblePixels,pixelSha256:sha(bytes)};observations.push(observation);await writeFile(resolve(root,'artifacts/browser/title-observed.json'),JSON.stringify({observations,errors},null,2)+'\n');await page.locator('canvas').screenshot({path:resolve(root,'artifacts/browser/title-'+scale+'x-'+kind+'.png')});
    assert.deepEqual(result.canvas,[640*scale,480*scale]);assert.ok(visiblePixels>60000,kind+' must contain a composed menu');assert.ok(result.stats[6]>0,'glBufferData streaming must be used');assert.equal(result.stats[7],0,'No sub-data streaming');assert.ok(result.stats[9]>0,'Sprite batches must use direct uploads');return observation;
   };
   await inspect('main');
   await page.evaluate(()=>{choose(0,5);settle();});await inspect('difficulty');
   await page.evaluate(()=>{choose(2,6);settle();step(0x20);settle();});await inspect('character');
   await page.evaluate(()=>{step(1);ready(7);settle();step(0x20);settle();});await inspect('animal');
   await page.evaluate(()=>{step(0x102);ready(6);step(0x102);ready(5);step(0x102);ready(1);settle(150);});await inspect('return');
   await page.evaluate(()=>{fixture._title_unlock();choose(2,5);choose(1,6);choose(2,7);choose(1,8);settle();});await inspect('practice');
   await page.evaluate(()=>{step(0x102);ready(7);step(0x102);ready(6);step(0x102);ready(5);step(0x102);ready(1);settle();choose(1,5);choose(0,6);choose(2,7);settle();});const extra=await inspect('extra');assert.equal(extra.state[8],4);
   await page.evaluate(()=>fixture._title_close());await context.close();
  }
  assert.deepEqual(errors,[]);report('sdl-title',{passed:true,completeGame:false,wasmSha256:sha(await readFile(resolve(root,'artifacts/sdl-title/th17-title.wasm'))),observations,scope:'Actual SDL3/WebGL2 composition of verified TH17 main, difficulty, character, animal and practice menus, cancel chains and unlocked Extra at 640x480 and 1280x960. Shared renderer is compiled unchanged with semantic game graphics interfaces. Desktop Chromium software GPU and emulated touch/DPR only; audio, game destinations, complete gameplay, native final-frame pixel parity and physical mobile hardware remain outside this development fixture.'});
  await writeFile(resolve(root,'artifacts/browser/title-observed.json'),JSON.stringify({observations,errors},null,2)+'\n');
 }finally{await browser?.close();await new Promise(r=>server.close(r));}
});
