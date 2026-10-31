import test from 'node:test';
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,mkdir,writeFile,readdir} from 'node:fs/promises';
import {resolve,basename} from 'node:path';
import {launchBrowser} from '../../../../../tools/browser/launch.mjs';
import {root,report,sha} from '../cpp/helpers.mjs';

test('TH17 actual stage gameplay composes through SDL3 and the shared WebGL2 renderer',async()=>{
 const assets=(await readdir(resolve(root,'reference/assets'))).filter(n=>/\.(anm|ecl|msg|std|sht|rpy)$/.test(n));
 const definitions=JSON.parse(await readFile(resolve(root,'reference/sound-definitions.json'),'utf8'));
 assets.push(...new Set([...definitions.samples,'thbgm.fmt']));
 const fonts=(await readdir(resolve(root,'assets/sdl-native/fonts'))).filter(n=>n.endsWith('.bin'));
 const music=(await readdir(resolve(root,'assets/music'))).filter(n=>n.endsWith('.ogg'));
 const files=new Map(assets.map(n=>['/assets/'+n,resolve(root,'reference/assets',n)]));
 for(const n of fonts)files.set('/fonts/'+n,resolve(root,'assets/sdl-native/fonts',n));
 for(const n of music)files.set('/music/'+n,resolve(root,'assets/music',n));
 const html='<!doctype html><meta charset="utf-8"><canvas id="canvas" width="640" height="480"></canvas><script type="module">import create from "/module.mjs";window.fixture=await create({canvas:document.querySelector("canvas")});</script>';
 const server=createServer(async(req,res)=>{try{const name=new URL(req.url,'http://localhost').pathname;res.setHeader('Cross-Origin-Opener-Policy','same-origin');res.setHeader('Cross-Origin-Embedder-Policy','require-corp');if(name==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}const file=name==='/module.mjs'?resolve(root,'artifacts/sdl-stage/th17-stage.mjs'):name==='/th17-stage.wasm'?resolve(root,'artifacts/sdl-stage/th17-stage.wasm'):files.get(name);if(!file){res.writeHead(404).end();return;}res.setHeader('Content-Type',name.endsWith('.mjs')?'text/javascript':name.endsWith('.wasm')?'application/wasm':'application/octet-stream');res.end(await readFile(file));}catch(e){res.writeHead(500).end(String(e));}});
 await new Promise(r=>server.listen(0,'127.0.0.1',r));let browser;const observations=[],errors=[];
 await mkdir(resolve(root,'artifacts/browser'),{recursive:true});
 try{
  browser=await launchBrowser();
  for(const scale of [1,2]){
   const context=await browser.newContext({viewport:{width:1280,height:960},hasTouch:scale===2,deviceScaleFactor:scale===2?2:1}),page=await context.newPage();page.on('pageerror',e=>errors.push(String(e)));
   await page.goto('http://127.0.0.1:'+server.address().port+'/');await page.waitForFunction(()=>window.fixture,{timeout:90000});
   await page.evaluate(async paths=>{
    const c=window.fixture;for(const dir of ['/assets','/fonts','/music'])c.FS.mkdir(dir);
    for(let at=0;at<paths.length;at+=8)await Promise.all(paths.slice(at,at+8).map(async path=>{const r=await fetch(path);if(!r.ok)throw Error('Missing '+path);c.FS.writeFile(path,new Uint8Array(await r.arrayBuffer()));}));
    window.decode=p=>new TextDecoder().decode(c.HEAPU8.subarray(p,c.HEAPU8.indexOf(0,p)));
    window.step=(held=0,draw=1)=>{if(!c._stage_step(held,draw))throw Error(decode(c._stage_error())||'Stage fixture stopped');};
   },[...files.keys()]);
   const stages=process.env.TH17_RENDER_STAGE?process.env.TH17_RENDER_STAGE.split(',').map(Number):[1,2,3,4,5,6,7];
   for(const stage of stages){
    const result=await page.evaluate(({stage,scale,limit})=>{
     const c=fixture;if(!c._stage_initialize(stage,0,0,stage===7?4:1,scale,0))throw Error(decode(c._stage_error()));
     for(let frame=0;frame<limit;frame++)step(1|8);
     const p=c._stage_pixels(),s=c._stage_graphics_stats()>>>2,sp=c._stage_state()>>>2;return{state:Array.from(c.HEAP32.subarray(sp,sp+12)),pixels:Array.from(c.HEAPU8.subarray(p,p+640*480*4)),stats:Array.from(c.HEAPU32.subarray(s,s+17)),canvas:[document.querySelector('canvas').width,document.querySelector('canvas').height]};
    },{stage,scale,limit:Number(process.env.TH17_RENDER_FRAMES??181)});
    const bytes=Buffer.from(result.pixels);delete result.pixels;let visiblePixels=0,playfieldPixels=0;for(let i=0;i<bytes.length;i+=4)if(bytes[i]||bytes[i+1]||bytes[i+2]){visiblePixels++;const x=i/4%640,y=Math.floor(i/4/640);if(x>=32&&x<416&&y>=16&&y<464)playfieldPixels++;}
    const observation={stage,scale,...result,visiblePixels,playfieldPixels,pixelSha256:sha(bytes)};observations.push(observation);
    await writeFile(resolve(root,'artifacts/browser/stage-observed.json'),JSON.stringify({observations,errors},null,2)+'\n');
    await page.locator('canvas').screenshot({path:resolve(root,`artifacts/browser/stage-${stage}-${scale}x.png`)});
    assert.deepEqual(result.canvas,[640*scale,480*scale]);assert.ok(playfieldPixels>20000,'Stage '+stage+' playfield must contain its background');assert.ok(result.stats[6]>0);assert.equal(result.stats[7],0);
    await page.evaluate(()=>fixture._stage_close());
   }
   await context.close();
  }
  assert.deepEqual(errors,[]);report('sdl-stage',{passed:true,completeGame:false,wasmSha256:sha(await readFile(resolve(root,'artifacts/sdl-stage/th17-stage.wasm'))),observations,scope:'Actual TH17 StageGameplay and GameDraw, original ordered ANM layers, offscreen composition and ASCII HUD through unchanged shared SDL3/WebGL2 renderer at 640 and 1280 physical widths. Chromium software GPU with emulated touch/DPR; no physical mobile acceptance, full native-frame pixel equality, audio, Japanese font upload or complete application destination claim.'});
 }finally{await browser?.close();await new Promise(r=>server.close(r));}
});
