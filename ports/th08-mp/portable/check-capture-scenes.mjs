import assert from 'node:assert/strict';
import {writeFileSync} from 'node:fs';
import {launchBrowser} from '../th10_web/scripts/native/browser-launch.mjs';
const browser=await launchBrowser(),results={};
try {
 for(const [name,port]of [['previous',8088],['candidate',8095]]){
  const page=await browser.newPage();await page.addInitScript(()=>Date.now=()=>100000);
  await page.goto(`http://127.0.0.1:${port}/runtime/th08/th08.html?manual=1`);
  await page.waitForFunction(()=>window.__th08Runtime,null,{timeout:120000});await page.locator('canvas').click();
  results[name]=await page.evaluate(async()=>{
   const r=__th08Runtime,c=r.core;await r.launch();c.sdl_touch_options(0,0,1);const initialReadBytes=new Uint32Array(c.memory.buffer,c.sdl_stats(),9)[3];const p=c.allocate(32);
   const key=(name,down)=>{new Uint8Array(c.memory.buffer,p,32).set(new TextEncoder().encode(name+'\0'));c.sdl_key(p,down);};
   const tick=()=>{if(c.sdl_loop_tick(r.app,1/60,17))throw Error('Tick failed');};
   const wait=n=>{for(let i=0;i<n;++i)tick();};
   const press=name=>{key(name,1);tick();key(name,0);wait(42);};
   const gl=document.querySelector('canvas').getContext('webgl2');
   const hash=async()=>{const bytes=new Uint8Array(640*480*4);gl.readPixels(0,0,640,480,gl.RGBA,gl.UNSIGNED_BYTE,bytes);return Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),n=>n.toString(16).padStart(2,'0')).join('');};
   wait(100);for(let i=0;i<5;++i)press('KeyZ');wait(241);
   const rows=[await hash()];press('Escape');for(let i=0;i<6;++i){wait(5);rows.push(await hash());}
   press('Escape');tick();rows.push(await hash());
   const stats=Array.from(new Uint32Array(c.memory.buffer,c.sdl_stats(),c.sdl_capture_validation?17:9));
   return {rows,stats,initialReadBytes,validation:c.sdl_capture_validation?.()};
  });await page.close();
 }
 assert.deepEqual(results.candidate.rows,results.previous.rows,'Pause blur/dim/resume pixels must remain identical');
 assert(results.candidate.stats[16]>3,'Actual pause must use GPU filtering');
 assert.equal(results.candidate.stats[3],results.candidate.initialReadBytes,'Only startup driver validation may read GPU pixels');
 writeFileSync('th08_web/artifacts/architecture-candidate/validation/capture-scenes.json',JSON.stringify({passed:true,results},null,2));console.log({passed:true,previousReadBytes:results.previous.stats[3],candidateReadBytes:results.candidate.stats[3]});
}finally{await browser.close();}
