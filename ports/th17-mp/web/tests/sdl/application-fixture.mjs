import {createServer} from 'node:http';
import {readFile,readdir,mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import assert from 'node:assert/strict';
import {root,sha} from '../cpp/helpers.mjs';

export async function applicationServer(extraFiles={}){
 const files=new Map();
 const runtime=new Map([['/module.mjs',readFileSyncRuntime('th17-application.mjs')],['/th17-application.wasm',readFileSyncRuntime('th17-application.wasm')]]);
 function readFileSyncRuntime(name){return readFile(resolve(root,process.env.TH17_APPLICATION_RUNTIME||'artifacts/sdl-application',name));}
 for(const name of(await readdir(resolve(root,'reference/assets'))).filter(n=>/\.(anm|ecl|msg|std|sht|rpy|wav|fmt|txt|png)$/.test(n)))files.set('/assets/'+name,resolve(root,'reference/assets',name));
 for(const [dir,source,extension]of[['fonts','assets/sdl-native/fonts','.bin'],['music','assets/music','.ogg']])for(const name of(await readdir(resolve(root,source))).filter(n=>n.endsWith(extension)))files.set('/'+dir+'/'+name,resolve(root,source,name));
 const html='<!doctype html><meta charset="utf-8"><style>body{margin:0;background:#222}canvas{display:block}</style><canvas id="canvas"></canvas><script type="module">import create from "/module.mjs";window.fixture=await create({canvas:document.querySelector("canvas")});</script>';
 const server=createServer(async(req,res)=>{try{
  const name=new URL(req.url,'http://localhost').pathname;
  res.setHeader('Cross-Origin-Opener-Policy','same-origin');res.setHeader('Cross-Origin-Embedder-Policy','require-corp');
  if(name==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}
  const file=extraFiles[name]??files.get(name),compiled=runtime.get(name);
  if(!file&&!compiled){res.writeHead(404).end(name);return;}
  res.setHeader('Content-Type',name.endsWith('.mjs')?'text/javascript':name.endsWith('.wasm')?'application/wasm':'application/octet-stream');res.end(compiled?await compiled:await readFile(file));
 }catch(e){res.writeHead(500).end(String(e));}});
 await new Promise(r=>server.listen(0,'127.0.0.1',r));
 return {url:'http://127.0.0.1:'+server.address().port,paths:[...files.keys()],close:()=>new Promise(r=>server.close(r))};
}
export async function applicationPage(browser,server,scale=1){
 const page=await browser.newPage({viewport:{width:1280,height:960},deviceScaleFactor:1});
 await page.goto(server.url);await page.waitForFunction(()=>window.fixture,{timeout:90000});
 await page.evaluate(async({paths,scale})=>{
  const c=fixture;for(const dir of['/assets','/fonts','/music'])c.FS.mkdir(dir);
  for(let at=0;at<paths.length;at+=8)await Promise.all(paths.slice(at,at+8).map(async path=>{const r=await fetch(path);if(!r.ok)throw Error('Missing '+path);c.FS.writeFile(path,new Uint8Array(await r.arrayBuffer()));}));
  window.decode=p=>new TextDecoder().decode(c.HEAPU8.subarray(p,c.HEAPU8.indexOf(0,p)));
  const words=(p,n)=>Array.from(c.HEAP32.subarray(p>>>2,(p>>>2)+n));
  window.state=()=>words(c._application_state(),16);window.pause=()=>words(c._application_pause_state(),7);window.world=()=>words(c._application_world_state(),24);window.ending=()=>words(c._application_ending_state(),6);
  window.step=(pressed=0,repeated=0,held=pressed)=>{if(!c._application_step(held,pressed,repeated,60,0))throw Error(decode(c._application_error())+' state='+state()+' pause='+pause()+' ending='+ending());return state();};
  window.settle=(n=40)=>{for(let i=0;i<n;i++)step();};
  window.waitFor=(condition,limit=480)=>{for(let i=0;i<limit;i++){step();if(condition())return;}throw Error('Application timeout '+state()+' pause='+pause()+' ending='+ending());};
  window.ready=(screen,limit=480)=>{waitFor(()=>{const s=state();return s[0]===0&&s[8]===screen&&s[9]===2;},limit);return state();};
  window.choose=(cursor,screen)=>{c._application_title_select(cursor);step(1);return ready(screen);};
  window.initialize=()=>{if(!c._application_initialize(0,scale))throw Error(decode(c._application_error()));ready(1);settle(30);};initialize();
  window.returnMain=()=>{step(0x100);waitFor(()=>pause()[1]===6);step(0x10000);waitFor(()=>state()[0]===0);ready(1);settle(30);};
 },{paths:server.paths,scale});return page;
}
export async function inspectApplication(page,kind,observations){
 const value=await page.evaluate(()=>{const c=fixture,p=c._application_pixels(),s=c._application_graphics_stats()>>>2;let visible=0;for(let i=0;i<640*480*4;i+=4)if(c.HEAPU8[p+i]||c.HEAPU8[p+i+1]||c.HEAPU8[p+i+2])visible++;return{state:state(),pause:pause(),ending:ending(),music:decode(c._application_music()),stats:Array.from(c.HEAPU32.subarray(s,s+17)),graphicsError:document.querySelector('canvas').getContext('webgl2').getError(),visible};});
 await mkdir(resolve(root,'artifacts/browser'),{recursive:true});const screenshot=resolve(root,'artifacts/browser/application-'+kind+'.png');await page.locator('canvas').screenshot({path:screenshot});
 const result={kind,...value,screenshotSha256:sha(await readFile(screenshot))};observations.push(result);await writeFile(resolve(root,'artifacts/browser/application-modes-observed.json'),JSON.stringify(observations,null,2)+'\n');
 assert.equal(value.graphicsError,0,kind+' graphics errors');assert.equal(value.stats[7],0,kind+' retains WebGL full-buffer replacement');assert.ok(value.visible>20000,kind+' visible content');return result;
}
