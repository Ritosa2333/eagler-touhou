import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync} from 'node:fs';
import {launchBrowser} from '../th10_web/scripts/native/browser-launch.mjs';
const out='th10_web/artifacts/architecture-candidate/validation',base=process.env.TH10_TEST_URL||'http://127.0.0.1:8093';mkdirSync(out,{recursive:true});
const browser=await launchBrowser(),context=await browser.newContext({viewport:{width:1000,height:800}}),page=await context.newPage(),errors=[],http=[],health=[];
page.on('pageerror',e=>errors.push(e.stack));page.on('response',r=>{if(r.status()>=400)http.push([r.status(),r.url()]);});
await page.exposeFunction('recordHealth',data=>health.push(data));
await page.addInitScript(()=>{window.addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1'&&e.data.event==='frame-health')window.recordHealth(e.data);});});
const key=async(code,delay=500)=>{await page.keyboard.down(code);await page.waitForTimeout(90);await page.keyboard.up(code);await page.waitForTimeout(delay);};
let frame;
try{
 await page.goto(base+'/?lang=chs&debug=1');await page.locator('.game[data-game="th10"]').click();await page.locator('#launch').click();
 await page.waitForFunction(()=>document.querySelector('#gameFrame')?.contentWindow?.__th10Runtime?.app,null,{timeout:120000});
 frame=page.frames().find(f=>f.url().includes('/runtime/th10/'));
 await frame.waitForFunction(()=>new Uint32Array(__th10Runtime.core.memory.buffer,__th10Runtime.core.sdl_stats(),6)[4]>=330,null,{timeout:30000});
 await page.screenshot({path:out+'/title.png'});
 console.log('title',await frame.evaluate(()=>({status:__th10Runtime.status(),fontErrors:__th10Runtime.core.sdl_fonts_errors(),heap:__th10Runtime.core.memory.buffer.byteLength,audio:Array.from(new Uint32Array(__th10Runtime.core.memory.buffer,__th10Runtime.core.sdl_audio_stats(),12)),context:Module.SDL3?.audioContext?.state})));
 for(let n=0;n<5;n++)await key('KeyZ',660);
 await frame.waitForFunction(()=>__th10Runtime.status()[1]===1,null,{timeout:30000});await page.waitForTimeout(6000);await page.screenshot({path:out+'/stage1.png'});
 const result=await frame.evaluate(()=>{const {core,Module}=__th10Runtime,a=core.sdl_audio_stats();return {state:__th10Runtime.status(),fontErrors:core.sdl_fonts_errors(),heap:core.memory.buffer.byteLength,audio:Array.from(new Uint32Array(core.memory.buffer,a,12)),rms:new Float32Array(core.memory.buffer,a+40,1)[0],context:Module.SDL3?.audioContext?.state};});
 assert.equal(result.state[2],0);assert.equal(result.fontErrors,0);assert(result.rms>0);assert.equal(result.context,'running');
 const pause=await frame.evaluate(async()=>{const r=__th10Runtime;r.core.sdl_loop_pause(1);const before=r.core.sdl_loop_time();await new Promise(resolve=>setTimeout(resolve,150));const after=r.core.sdl_loop_time();r.core.sdl_loop_pause(0);return {before,after};});assert.equal(pause.before,pause.after);
 await frame.evaluate(()=>__th10Runtime.command({command:'sync'}));const list=await frame.evaluate(()=>__th10Runtime.command({command:'list'}));assert(list.files.some(f=>f.path==='scoreth10c.dat'));
 const exported=await frame.evaluate(()=>__th10Runtime.command({command:'read',path:'scoreth10c.dat'}));assert(exported.bytes.length>100);
 const forbidden=await frame.evaluate(async()=>{try{await __th10Runtime.command({command:'read',path:'../fonts/simhei.ttf'});return false;}catch{return true;}});assert(forbidden);
 assert.equal(errors.length,0,errors.join('\n'));assert.equal(http.length,0,JSON.stringify(http));
 writeFileSync(out+'/smoke.json',JSON.stringify({passed:true,result,pause,files:list.files,health,errors,http},null,2));console.log(JSON.stringify({passed:true,result,files:list.files,health:health.slice(-3)}));
}catch(e){writeFileSync(out+'/failure.json',JSON.stringify({error:e.stack,errors,http,body:await page.locator('body').innerText()},null,2));await page.screenshot({path:out+'/failure.png'});throw e;}
finally{await browser.close();}
