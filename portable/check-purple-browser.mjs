// Private retail DATA is served locally, never copied into source or evidence.
import {createServer} from 'node:http';
import {readFileSync,existsSync,mkdirSync,writeFileSync} from 'node:fs';
import {resolve,extname} from 'node:path';
import assert from 'node:assert/strict';
import {launchBrowser} from '../th10_web/scripts/native/browser-launch.mjs';
const root=resolve(import.meta.dirname,'..'),out=resolve(root,'th10_web/artifacts/purple-check',process.env.TH10_PRACTICE_SECTION?'section-'+process.env.TH10_PRACTICE_SECTION:'baseline');
const data=process.env.TH10_RETAIL_DATA,fonts=process.env.TH10_SHARED_FONTS;
assert(data&&fonts,'Set TH10_RETAIL_DATA and TH10_SHARED_FONTS');mkdirSync(out,{recursive:true});
const parent=`<!doctype html><style>html,body{margin:0}iframe{width:100vw;height:100vh;border:0}</style><script>window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/retail.data')).arrayBuffer()});</script><iframe src='/runtime/th10.html?managedData=1&runtimeEpoch=1&gameGeneration=private-test'></iframe>`;
const server=createServer((req,res)=>{try{const path=new URL(req.url,'http://local').pathname;let bytes,file;
 if(path==='/')bytes=Buffer.from(parent);else if(path==='/retail.data')file=data;
 else if(/^\/(unifont\.otf|msgothic\.ttc)$/.test(path))file=resolve(fonts,path.slice(1));
 else if(path.startsWith('/runtime/')&&!path.includes('..'))file=resolve(root,'build-eagler',path.slice(9));else throw Error('path');
 if(file){assert(existsSync(file),file);bytes=readFileSync(file);}res.setHeader('Content-Type',file?({'.mjs':'text/javascript','.wasm':'application/wasm','.json':'application/json','.html':'text/html'}[extname(file)]||'application/octet-stream'):'text/html');res.end(bytes);
 }catch(e){res.statusCode=404;res.end(String(e));}});
await new Promise(r=>server.listen(0,'127.0.0.1',r));const browser=await launchBrowser();
const errors=[],evidence={};
const disabled=process.env.THPRAC_OFF==='1';
try{
 const page=await browser.newPage({viewport:{width:960,height:720}});page.on('pageerror',e=>errors.push(String(e)));
 await page.goto(`http://127.0.0.1:${server.address().port}/`);const frame=page.frames().find(f=>f.url().includes('/runtime/'));
 await frame.waitForFunction(()=>window.__th10Runtime,{timeout:60000});
 await frame.evaluate(async disabled=>{const r=window.__th10Runtime;await r.command({command:'configure',language:'lang_en',music:'none',options:{thpracEnabled:!disabled,thpracLocale:'en'},sharedResources:[{path:'/unifont.otf',url:'/unifont.otf'},{path:'/msgothic.ttc',url:'/msgothic.ttc'}]});await r.launch();},disabled);
 await frame.evaluate(()=>window.__th10Runtime.core.sdl_loop_stop());
 const wait=ms=>frame.evaluate(n=>{const r=window.__th10Runtime;for(let i=0;i<n;i++){const result=r.core.sdl_loop_tick(r.app,1/60,17);if(result)throw Error('native tick '+result+' / error '+r.core.application_error(r.app));}},Math.ceil(ms*60/1000));
 await wait(3000);
 const status=()=>frame.evaluate(()=>window.__th10Runtime.status());
 const press=async(code)=>{await frame.evaluate(code=>window.__th10Runtime.command({command:'keyboard',code,down:true}),code);await wait(70);await frame.evaluate(code=>window.__th10Runtime.command({command:'keyboard',code,down:false}),code);await wait(70);};
 const title=()=>frame.evaluate(()=>{const r=window.__th10Runtime,p=r.core.application_title(r.app);return p?Array.from(new Int32Array(r.core.memory.buffer,p+0x1c,3)):null;});
 evidence.initial=await status();
 if(disabled){
  for(let i=0;i<12&&(await status())[0]!==7;i++){await press('KeyZ');await wait(600);}await wait(2500);
  evidence.game=await status();assert.equal(evidence.game[0],7);assert.equal(evidence.game[2],0);assert.deepEqual(errors,[]);
  await page.screenshot({path:resolve(out,'ordinary-off.png')});evidence.passed=true;writeFileSync(resolve(out,'browser-off.json'),JSON.stringify(evidence,null,2)+'\n');console.log(JSON.stringify(evidence));
 }else{
 await press('F12');await wait(100);await page.screenshot({path:resolve(out,'advanced-desktop.png')});
 // Native pointer FIFO, quick down/up in the same display callback.
 await frame.evaluate(()=>{const c=window.__th10Runtime.core;c.sdl_thprac_mouse(1,80,125);c.sdl_thprac_mouse(2,80,125);});await wait(100);await page.screenshot({path:resolve(out,'advanced-pointer.png')});await press('F12');
 for(let i=0;i<10&&(await title())?.[0]!==2;i++){await press('KeyZ');await wait(600);}
 await wait(600);assert.equal((await title())?.[0],2,'native main menu');
 for(let i=0;i<8&&(await title())[2]!==2;i++){await press((await title())[2]<2?'ArrowDown':'ArrowUp');await wait(100);}
 assert.equal((await title())[2],2,'native Practice entry');
 await press('KeyZ');await wait(600);await press('KeyZ');await wait(600);await press('KeyZ');await wait(600);await press('KeyZ');await wait(600);
 evidence.practice=await frame.evaluate(()=>{const r=window.__th10Runtime;return Array.from(new Int32Array(r.core.memory.buffer,r.core.practice_status(r.app),7));});
 for(let i=0;i<3&&!evidence.practice[0];i++){await press('KeyZ');await wait(600);evidence.practice=await frame.evaluate(()=>{const r=window.__th10Runtime;return Array.from(new Int32Array(r.core.memory.buffer,r.core.practice_status(r.app),7));});}
 await page.screenshot({path:resolve(out,'practice-desktop.png')});
 assert.equal(evidence.practice[0],1,'original title/character/shot/stage flow must reach practice tuning');
 if(process.env.TH10_PRACTICE_SECTION){
  const {sections}=await import('../th10_web/sdl-runtime/practice-sections.mjs');const id=Number(process.env.TH10_PRACTICE_SECTION);const section=sections.find(s=>s.id===id),stage=section?.stage??Math.floor((id-10000)/100)-1;
  evidence.section=id;await frame.evaluate(({id,stage})=>{const r=window.__th10Runtime,words=[1,stage,0,id,1,0,0,0,9,100,10000,130,-1,0,1];const p=r.core.graphics_allocate(120);try{new Float64Array(r.core.memory.buffer,p,15).set(words);if(!r.core.practice_configure(r.app,p,15,true))throw Error('practice config rejected');}finally{r.core.graphics_free(p);}}, {id,stage});
 }
 await press('KeyZ');await wait(2500);evidence.game=await status();assert.equal(evidence.game[2],0,'native runtime error');
 assert.equal(evidence.game[0],7,'practice session must be live');
 evidence.worldError=await frame.evaluate(()=>{const r=window.__th10Runtime;return r.core.world_error(r.core.application_world(r.app));});assert.equal(evidence.worldError,0);
 if(!evidence.section){
  evidence.replay=await frame.evaluate(()=>{const r=window.__th10Runtime,c=r.core,alloc=text=>{const bytes=new TextEncoder().encode(text+'\0'),p=c.graphics_allocate(bytes.length);new Uint8Array(c.memory.buffer,p,bytes.length).set(bytes);return p;};const file=alloc('th10_01.rpy'),name=alloc('Purple');try{if(!c.world_save_replay(c.application_world(r.app),file,name))throw Error('native Replay save');const bytes=r.Module.FS.readFile('/savesth10/jp/replay/th10_01.rpy'),text=new TextDecoder().decode(bytes);return {bytes:bytes.length,prac:text.includes('PRAC'),purpleVersion:text.includes('2.2.2.7')};}finally{c.graphics_free(file);c.graphics_free(name);}});assert(evidence.replay.prac&&evidence.replay.purpleVersion,'native Replay must retain purple PRAC metadata');
 }
 await press('Backspace');await press('KeyU');await press('Tab');await wait(100);await page.screenshot({path:resolve(out,'game-overlay.png')});
 await page.setViewportSize({width:430,height:932});await press('F12');await wait(100);await page.screenshot({path:resolve(out,'advanced-mobile.png')});
 if(!evidence.section){
  const tap=async(x,y)=>{await frame.evaluate(({x,y})=>{const c=window.__th10Runtime.core;c.sdl_touch(0,17,x/640,y/480);c.sdl_touch(2,17,x/640,y/480);},{x,y});await wait(100);};
  await tap(16,173);await page.screenshot({path:resolve(out,'blind-mobile.png')});await tap(16,173);
  await tap(30,146);await press('F12');await wait(300);
  const position=()=>frame.evaluate(()=>{const r=window.__th10Runtime,w=r.core.application_world(r.app),p=r.core.world_actor(w,1);return Array.from(new Float32Array(r.core.memory.buffer,p+0x3c0,2));});
  const before=await position();await press('ArrowUp');const after=await position();evidence.flippedMovement={before,after};assert(after[1]>before[1],'native SSS flips only the vertical movement step');
  await page.screenshot({path:resolve(out,'flipped-mobile.png')});await press('F12');await tap(30,480-146);await press('F12');
 }
 await frame.evaluate(()=>window.__th10Runtime.command({command:'touch-cancel'}));assert.equal((await status())[2],0);
 assert.deepEqual(errors,[]);evidence.errors=errors;evidence.passed=true;writeFileSync(resolve(out,evidence.section?'browser-section-'+evidence.section+'.json':'browser.json'),JSON.stringify(evidence,null,2)+'\n');console.log(JSON.stringify(evidence));
 }
}finally{await browser.close();await new Promise(r=>server.close(r));}
