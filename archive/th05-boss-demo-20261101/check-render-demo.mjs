import {createServer} from 'node:http';
import {readFile,writeFile} from 'node:fs/promises';
import {resolve,extname} from 'node:path';
import assert from 'node:assert/strict';
import {launchBrowser} from '../../tools/browser/launch.mjs';
import {ORIGINAL_BGM,musicFileName} from '../../ports/th05/web/audio-layout.mjs';
const ops=import.meta.dirname,assets=resolve(ops,'../../../../eagler-touhou/ports/th05/artifacts'),runtime=resolve(assets,'test-runtime-boss');
const html=`<!doctype html><script>let serial=0;const waiting=new Map();window.events=[];window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/th05.data')).arrayBuffer()});addEventListener('message',e=>{events.push(e.data);if(waiting.has(e.data.request)){waiting.get(e.data.request)(e.data);waiting.delete(e.data.request);}});window.command=(command,payload={})=>new Promise(done=>{const request='a'+(++serial);waiting.set(request,done);document.querySelector('iframe').contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th05',epoch:9,command,request,...payload},location.origin);});</script><iframe style="border:0;width:640px;height:480px" src="/runtime/th05.html?runtimeEpoch=9&managedData=1"></iframe>`;
const server=createServer(async(req,res)=>{try{const p=new URL(req.url,'http://test').pathname;const body=p==='/'?html:await readFile(p==='/th05.data'?resolve(assets,'content/th05.data'):p==='/font.otf'?resolve(assets,'../../../dependencies/unifont-15.1.05/unifont-15.1.05.otf'):p.startsWith('/music/')?resolve(assets,'content',p.slice(1)):resolve(runtime,p.slice('/runtime/'.length)));res.writeHead(200,{'Content-Type':p==='/'?'text/html':({'.mjs':'text/javascript','.html':'text/html','.wasm':'application/wasm'})[extname(p)]??'application/octet-stream'});res.end(body);}catch(error){res.writeHead(404);res.end(error.message);}});
await new Promise(done=>server.listen(0,'127.0.0.1',done));const browser=await launchBrowser(),page=await browser.newPage({viewport:{width:660,height:500}}),errors=[],report={backgrounds:[],stages:[]};page.on('pageerror',e=>errors.push(e.message));
await page.addInitScript(()=>{let serial=0;const waiting=new Map();window.requestAnimationFrame=callback=>{const id=++serial;waiting.set(id,callback);return id;};window.cancelAnimationFrame=id=>waiting.delete(id);window.pumpFrame=time=>{const current=[...waiting.values()];waiting.clear();for(const callback of current)callback(time);};});
try{
 await page.goto('http://127.0.0.1:'+server.address().port);await page.waitForFunction(()=>events.some(e=>e.event==='ready'),null,{polling:50});const command=async(name,payload={})=>{const r=await page.evaluate(({name,payload})=>command(name,payload),{name,payload});assert.equal(r.ok,true,JSON.stringify(r));};
 const resources=['op','st00','st02','st03'].map(name=>musicFileName(ORIGINAL_BGM.tracks[name+'-m86'].url)).map(name=>({path:'/music/'+name,url:'/music/'+name}));await command('configure',{music:'ogg',resources,sharedResources:[{path:'/data/gothic.ttf',url:'/font.otf'}],options:{thpracEnabled:true,oggDecodeMode:'stream',limitPresentationTo60:false}});await command('launch');const frame=page.frames()[1];await frame.locator('#canvas').click({force:true,position:{x:4,y:4}});
 report.frames=[];
 for(const stage of [0,2,3,4,5,6]){
  await frame.evaluate(stage=>{Module._th05_native_stage_start(stage,2);Module._th05_key(90,1);Module._th05_key(85,1);Module._th05_key(85,0);},stage);
  for(let part=0;part<8;part++){
   const result=await frame.evaluate(part=>{for(let n=0;n<112;n++)Module._th05_tick(0);const effect=part>=4?Module._th05_native_effect_probe(part===7?1:0):null;Module._th05_tick(1);const gl=Module.canvas.getContext('webgl'),read=()=>{const out=new Uint8Array(640*480*4);gl.readPixels(0,0,640,480,gl.RGBA,gl.UNSIGNED_BYTE,out);return out;};let t=performance.now();Module._th05_native_draw_probe(0);const oldCost=performance.now()-t,oldDraws=Module._th05_graphics_stat(0),old=read();t=performance.now();Module._th05_native_draw_probe(1);const newCost=performance.now()-t,newDraws=Module._th05_graphics_stat(0),next=read();let differences=0;for(let n=0;n<old.length;n++)if(old[n]!==next[n])differences++;return {effect,differences,oldCost,newCost,oldDraws,newDraws};},part);report.frames.push({stage:stage+1,part,...result});if(part>=4)assert.equal(result.effect,1,'native explosion/bomb probe');assert.equal(result.differences,0,JSON.stringify({stage,part,...result}));
  }
 }
 report.demos=[];
 for(const number of [1,2,3,4,5]){
  const sequences=[];
  for(const stride of [1,4]){assert.equal(await frame.evaluate(number=>{Module._th05_clear();return Module._th05_native_demo_start(number);},number),1);const samples=await frame.evaluate(stride=>{const out=[];for(let n=0;n<5200;n++){if(!Module._th05_tick(+(n%stride===stride-1)))throw Error('Demo tick');if(n%128===127||!Module._th05_native_demo_info(0)){out.push({frame:n,state:Array.from({length:8},(_,i)=>Module._th05_native_demo_info(i)),scene:Module._th05_scene()});if(!Module._th05_native_demo_info(0))break;}}return out;},stride);sequences.push(samples);await page.waitForTimeout(10);}
  assert.deepEqual(sequences[0],sequences[1],'Demo render cadence '+number);report.demos.push({number,samples:sequences[0]});
 }
 assert.deepEqual(errors,[]);report.passed=true;report.errors=errors;await writeFile(resolve(ops,'render-demo.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify(report.frames));console.log('DEMO cadence parity PASS');
}finally{await browser.close();await new Promise(done=>server.close(done));}
