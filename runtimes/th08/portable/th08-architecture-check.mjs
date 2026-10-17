import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync} from 'node:fs';
import {launchBrowser} from '../th10_web/scripts/native/browser-launch.mjs';
const out='th08_web/artifacts/architecture-candidate/validation';mkdirSync(out,{recursive:true});
const browser=await launchBrowser(),context=await browser.newContext({viewport:{width:960,height:720}}),page=await context.newPage(),errors=[],messages=[];
page.on('pageerror',e=>errors.push(e.stack));page.on('console',m=>{if(m.type()==='error')messages.push(m.text());});
try{
 await page.goto('http://127.0.0.1:8095/runtime/th08/th08.html');await page.waitForFunction(()=>window.__th08Runtime||document.querySelector('#error').textContent,null,{timeout:120000});assert.equal(await page.locator('#error').textContent(),'');await page.locator('canvas').click();await page.evaluate(()=>__th08Runtime.launch());
 await page.waitForTimeout(6500);await page.screenshot({path:out+'/title.png'});
 console.log('title',await page.evaluate(()=>({state:__th08Runtime.status(),diag:Array.from(new Int32Array(__th08Runtime.core.memory.buffer,__th08Runtime.core.diagnostics(__th08Runtime.app),16))})));
 for(let i=0;i<5;i++){await page.keyboard.press('z',{delay:90});await page.waitForTimeout(600);}
 await page.waitForTimeout(4000);await page.screenshot({path:out+'/stage1.png'});
 const result=await page.evaluate(()=>{const r=__th08Runtime,c=r.core,a=c.sdl_audio_stats();return {state:r.status(),audio:Array.from(new Uint32Array(c.memory.buffer,a,12)),rms:new Float32Array(c.memory.buffer,a+40,1)[0],context:r.Module.SDL3.audioContext.state,heap:c.memory.buffer.byteLength};});
 assert.equal(result.state[2],0);assert.equal(result.audio[4],0);assert(result.rms>0);assert.equal(result.context,'running');
 await page.evaluate(()=>__th08Runtime.command({command:'sync'}));const files=await page.evaluate(()=>__th08Runtime.command({command:'list'}));assert(files.files.some(f=>f.path==='score.dat'));
 assert.equal(errors.length,0,errors.join('\n'));writeFileSync(out+'/smoke.json',JSON.stringify({passed:true,result,files,errors,messages},null,2));console.log(JSON.stringify({passed:true,result,files,messages}));
}catch(error){writeFileSync(out+'/failure.json',JSON.stringify({error:error.stack,errors,messages,body:await page.locator('body').innerText()},null,2));await page.screenshot({path:out+'/failure.png'});throw error;}
finally{await browser.close();}
