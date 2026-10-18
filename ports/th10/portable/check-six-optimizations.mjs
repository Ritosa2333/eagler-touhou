// Actual GL calls, pixels and gameplay from both the previous and candidate
// builds. Desktop/mobile viewport results are not physical-phone benchmarks.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {launchBrowser} from '../th10_web/scripts/native/browser-launch.mjs';
const game=process.argv.includes('--th08')?'th08':'th10';
const oldBase=process.env.OLD_URL??(game==='th08'?'http://127.0.0.1:8088':'http://127.0.0.1:8096');
const newBase=process.env.TEST_URL??(game==='th08'?'http://127.0.0.1:8095':'http://127.0.0.1:8093');
const out=game+'_web/artifacts/architecture-candidate/validation';mkdirSync(out,{recursive:true});
const browser=await launchBrowser(),runs={};
try {
 for(const [label,base] of [['previous',oldBase],['candidate',newBase]]) {
  const manifest=await(await fetch(base+'/manifest.json')).json();
  const context=await browser.newContext({viewport:{width:390,height:844},isMobile:true,hasTouch:true});
  const page=await context.newPage(),errors=[];page.on('pageerror',e=>errors.push(e.stack));
  await page.addInitScript(()=>{Date.now=()=>100000;});
  await page.goto(base+`/runtime/${game}/${game}.html?manual=1&language=ja`);
  await page.waitForFunction(game=>window['__'+game+'Runtime'],game,{timeout:120000});
  await page.locator('canvas').click();
  await page.evaluate(async({game,replay})=>{
   const r=window['__'+game+'Runtime'],c=r.core;
   let app;
   if(game==='th08') {
    await r.command({command:'write',path:'replay/th8_01.rpy',bytes:replay});await r.launch();app=r.app;c.sdl_touch_options(0,0,1);
   } else {r.Module.runtimePrepare=()=>0;app=c.sdl_game_open(false,Date.now()&65535);}
   const allocate=c.allocate??c.graphics_allocate,p=allocate(64);let previous=new Set();
   const tick=(keys=[])=>{const next=new Set(keys);for(const key of new Set([...previous,...next]))if(previous.has(key)!==next.has(key)){new Uint8Array(c.memory.buffer,p,64).set(new TextEncoder().encode(key+'\0'));c.sdl_key(p,next.has(key));}previous=next;if(c.sdl_loop_tick(app,1/60,17))throw Error('C++ tick failed');};
   const wait=n=>{for(let i=0;i<n;i++)tick();},key=k=>{for(let i=0;i<3;i++)tick([k]);wait(42);};
   if(game==='th08') {wait(100);for(let i=0;i<10&&c.status(app,9)!==4;i++)key('ArrowUp');key('KeyZ');key('KeyZ');for(let i=1;i<9;i++)key('ArrowDown');key('KeyZ');key('KeyZ');}
   else {wait(330);for(let i=0;i<5;i++)key('KeyZ');wait(100);}
   const gl=document.querySelector('canvas').getContext('webgl2'),info=gl.getExtension('WEBGL_debug_renderer_info');
   const counters={replacements:0,subUpdates:0,draws:0,bytes:0,attrib:0,compile:0,texture:0,framebuffer:0},data=gl.bufferData,sub=gl.bufferSubData,draw=gl.drawArrays,instanced=gl.drawArraysInstanced;
   gl.bufferData=function(target,bytes,...rest){counters.replacements++;if(typeof bytes!=='number')counters.bytes+=(rest[2]??bytes.length-(rest[1]??0))*bytes.BYTES_PER_ELEMENT;return data.call(this,target,bytes,...rest);};
   gl.bufferSubData=function(...args){counters.subUpdates++;return sub.apply(this,args);};
   gl.drawArrays=function(...args){counters.draws++;return draw.apply(this,args);};
   gl.drawArraysInstanced=function(...args){counters.draws++;return instanced.apply(this,args);};
   for(const [name,key]of [['vertexAttribPointer','attrib'],['compileShader','compile'],['bindTexture','texture'],['bindFramebuffer','framebuffer']]){const original=gl[name];gl[name]=function(...args){++counters[key];return original.apply(this,args);};}
   window.probe={r,c,app,tick,gl,counters,gpu:info?gl.getParameter(info.UNMASKED_RENDERER_WEBGL):gl.getParameter(gl.RENDERER)};
  },{game,replay:game==='th08'?Array.from(readFileSync('th08_web/artifacts/cpp/campaign/extra/original.rpy')):null});
  const rows=[],times=[],total=game==='th08'?10000:14000;
  for(let start=0;start<total;start+=500) {
   const row=await page.evaluate(async({game,start,count})=>{
    const {c,app,tick,gl}=probe,times=[];
    for(let i=0;i<count;i++) {
     if(game==='th10'){const p=c.world_actor(c.application_world(app),1);if(p){const v=new DataView(c.memory.buffer);v.setInt32(p+0x430c,120,true);v.setInt32(p+0x4310,121,true);v.setFloat32(p+0x4314,121,true);}}
     const time=performance.now();tick(game==='th10'?['KeyZ','ShiftLeft']:[]);times.push(performance.now()-time);
     if(i%50===49)await new Promise(resolve=>setTimeout(resolve,0));
    }
    const pixels=new Uint8Array(gl.drawingBufferWidth*gl.drawingBufferHeight*4);gl.readPixels(0,0,gl.drawingBufferWidth,gl.drawingBufferHeight,gl.RGBA,gl.UNSIGNED_BYTE,pixels);
    const hash=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',pixels)),n=>n.toString(16).padStart(2,'0')).join('');
    const state=game==='th08'?Array.from(new Uint32Array(c.memory.buffer,c.trace(app),68)):Array.from(new Int32Array(c.memory.buffer,c.sdl_game_status(),10));
    return {frame:start+count,hash,state,times,error:gl.getError()};
   },{game,start,count:Math.min(500,total-start)});
   times.push(...row.times);delete row.times;rows.push(row);
   if(start%3000===0)console.log(JSON.stringify({game,label,frame:row.frame,state:row.state.slice(0,6)}));
  }
  const result=await page.evaluate(()=>({counters:probe.counters,gpu:probe.gpu,stats:Array.from(new Uint32Array(probe.c.memory.buffer,probe.c.sdl_stats(),17))}));
  times.sort((a,b)=>a-b);result.frameMs={median:times[Math.floor(times.length*.5)],p95:times[Math.floor(times.length*.95)],p99:times[Math.floor(times.length*.99)]};
  assert.deepEqual(errors,[]);assert(rows.every(r=>r.error===0));
  runs[label]={wasm:manifest.execution.sha256,rows,...result,errors};
  await page.screenshot({path:out+'/gles-'+label+'.png'});await context.close();
 }
 assert.deepEqual(runs.candidate.rows,runs.previous.rows,'Same input must produce identical gameplay and rendered pixels');
 
 assert.equal(runs.candidate.counters.subUpdates,0,'No segmented vertex uploads in the production Web renderer');
 assert(runs.candidate.counters.replacements>100);assert(runs.candidate.counters.draws>100);
 assert(runs.candidate.counters.attrib<runs.previous.counters.attrib/10,'Fixed layouts should eliminate repeated attribute setup');
 assert.equal(runs.candidate.counters.compile,0,'No shader compilation inside gameplay');
 const report={passed:true,game,physicalPhone:false,runs};writeFileSync(out+'/six-optimizations.json',JSON.stringify(report,null,2));
 console.log(JSON.stringify({passed:true,game,frames:runs.candidate.rows.at(-1).frame,previous:runs.previous.counters,candidate:runs.candidate.counters,timing:{previous:runs.previous.frameMs,candidate:runs.candidate.frameMs}}));
} catch(error) {writeFileSync(out+'/six-optimizations-failure.json',JSON.stringify({error:error.stack,runs},null,2));throw error;}
finally {await browser.close();}
