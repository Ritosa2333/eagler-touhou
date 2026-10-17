import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync} from 'node:fs';
import {launchBrowser} from '../th10_web/scripts/native/browser-launch.mjs';
const out='th10_web/artifacts/architecture-candidate/validation',base='http://127.0.0.1:8093';mkdirSync(out,{recursive:true});
const browser=await launchBrowser(),context=await browser.newContext({viewport:{width:390,height:844},isMobile:true,hasTouch:true}),page=await context.newPage(),errors=[],health=[];
page.on('pageerror',e=>errors.push(e.stack));await page.exposeFunction('recordHealth',data=>health.push(data));
await page.addInitScript(()=>{window.addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1'&&e.data.event==='frame-health')window.recordHealth(e.data);});});
try{
 await page.goto(base+'/?lang=chs&debug=1');await page.locator('.game[data-game="th10"]').tap();await page.locator('#launch').tap();await page.waitForFunction(()=>document.querySelector('#gameFrame')?.contentWindow?.__th10Runtime?.app,null,{timeout:120000});
 const frame=page.frames().find(f=>f.url().includes('/runtime/th10/'));await frame.waitForFunction(()=>new Uint32Array(__th10Runtime.core.memory.buffer,__th10Runtime.core.sdl_stats(),6)[4]>=330);
 for(let n=0;n<5;n++){await page.keyboard.down('KeyZ');await page.waitForTimeout(90);await page.keyboard.up('KeyZ');await page.waitForTimeout(660);}
 await frame.waitForFunction(()=>__th10Runtime.status()[1]===1);
 const cdp=await context.newCDPSession(page);await cdp.send('Emulation.setCPUThrottlingRate',{rate:4});
 await frame.evaluate(()=>{const r=__th10Runtime,c=r.core,prepare=r.Module.runtimePrepare;r.Module.runtimePrepare=()=>{const p=c.world_actor(c.application_world(r.app),1);if(p){const v=new DataView(c.memory.buffer);v.setInt32(p+0x430c,120,true);v.setInt32(p+0x4310,121,true);v.setFloat32(p+0x4314,121,true);}return prepare();};});
 await page.keyboard.down('KeyZ');await page.keyboard.down('ShiftLeft');await page.keyboard.down('ControlLeft');health.length=0;
 let state;for(let n=0;n<60;n++){await page.waitForTimeout(5000);state=await frame.evaluate(()=>__th10Runtime.status());assert.equal(state[2],0);if(n%6===0)console.log(JSON.stringify({seconds:(n+1)*5,state,health:health.at(-1)}));if(state[1]===2)break;}
 assert.equal(state[1],2,'Stage 1 did not complete');
 const output=await frame.evaluate(async()=>{const {core,Module}=__th10Runtime,s=Module.SDL3,analyser=s.audioContext.createAnalyser(),node=s.audio_playback.scriptProcessorNode;node.connect(analyser);const data=new Float32Array(analyser.fftSize);let rms=0;for(let n=0;n<8;n++){await new Promise(r=>setTimeout(r,30));analyser.getFloatTimeDomainData(data);rms=Math.max(rms,Math.sqrt(data.reduce((n,v)=>n+v*v,0)/data.length));}node.disconnect(analyser);return {rms,heap:core.memory.buffer.byteLength,fontErrors:core.sdl_fonts_errors(),audio:Array.from(new Uint32Array(core.memory.buffer,core.sdl_audio_stats(),12))};});
 assert(output.rms>0.0001);assert.equal(output.fontErrors,0);assert.equal(errors.length,0);
 const samples=health.map(h=>h.fps).sort((a,b)=>a-b),summary={physicalPhone:false,throttle:4,samples:samples.length,medianFps:samples[Math.floor(samples.length/2)],minOneSecondFps:Math.min(...samples),maxGapMs:Math.max(...health.map(h=>h.maxGapMs)),maxFrameMs:Math.max(...health.map(h=>h.frameMs)),stage:state[1]};
 writeFileSync(out+'/mobile-stage1.json',JSON.stringify({passed:true,summary,output,health,errors},null,2));await page.screenshot({path:out+'/mobile-stage2.png'});console.log(JSON.stringify({passed:true,summary,output}));
}catch(e){writeFileSync(out+'/mobile-failure.json',JSON.stringify({error:e.stack,errors,health},null,2));await page.screenshot({path:out+'/mobile-failure.png'});throw e;}finally{await browser.close();}
