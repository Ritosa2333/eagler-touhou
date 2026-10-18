import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {launchBrowser} from '../th10_web/scripts/native/browser-launch.mjs';
const out='th08_web/artifacts/architecture-candidate/validation';mkdirSync(out,{recursive:true});
const route=process.argv.includes('--extra')?'extra':process.argv.includes('--finalb')?'finalb-practice':'character0-final',replay=readFileSync('th08_web/artifacts/cpp/campaign/'+route+'/original.rpy');
const cached=process.argv.includes('--cached-baseline')?JSON.parse(readFileSync('th08_web/artifacts/sdl3/replay-baseline-'+route+'.json')):null;
if(cached)assert.equal(cached.passed,true);
const browser=await launchBrowser(),all=cached?{baseline:cached.native}:{};
try{for(const native of cached?[true]:[false,true]){
 const context=await browser.newContext(),page=await context.newPage(),errors=[];page.on('pageerror',e=>errors.push(e.stack));await page.addInitScript(()=>{Date.now=()=>100000;});
 if(!native){
  await page.route('**/probe.html',r=>r.fulfill({contentType:'text/html',body:'<!doctype html><canvas id="screen" width="640" height="480"></canvas>'}));
  await page.route('**/probe.mjs',r=>r.fulfill({contentType:'text/javascript',body:readFileSync('portable/th08-probe-page.mjs')}));
  await page.route('**/probe-game.wasm',r=>r.fulfill({contentType:'application/wasm',body:readFileSync('th08_web/artifacts/sdl-release/site/game.wasm')}));
  await page.route('**/probe-replay.rpy',r=>r.fulfill({body:replay}));
  await page.goto('http://127.0.0.1:8092/probe.html');await page.evaluate(async()=>{const{createProbe}=await import('/probe.mjs');window.probe=await createProbe({sdl:true,replay:true});});
 }else{
  await page.goto('http://127.0.0.1:8095/runtime/th08/th08.html?manual=1');await page.waitForFunction(()=>window.__th08Runtime||document.querySelector('#error').textContent,null,{timeout:120000});await page.locator('canvas').click();
  await page.evaluate(async bytes=>{const r=__th08Runtime,c=r.core;await r.command({command:'write',path:'replay/th8_01.rpy',bytes});await r.launch();c.sdl_touch_options(0,0,1);
   const pointer=c.allocate(64),mapping=[[1,'KeyZ'],[2,'KeyX'],[4,'ShiftLeft'],[8,'Escape'],[16,'ArrowUp'],[32,'ArrowDown'],[64,'ArrowLeft'],[128,'ArrowRight'],[256,'ControlLeft']];let previous=0;
   const step=(input=0)=>{for(const[bit,key]of mapping)if(!!(input&bit)!==!!(previous&bit)){new Uint8Array(c.memory.buffer,pointer,64).set(new TextEncoder().encode(key+'\0'));c.sdl_key(pointer,!!(input&bit));}previous=input;if(c.sdl_loop_tick(r.app,1/60,17))throw Error('Native game failed '+r.status());};
   const wait=n=>{for(let i=0;i<n;i++)step();},key=k=>{for(let i=0;i<3;i++)step(k);wait(42);};
   const title=()=>({scene:c.status(r.app,0),screen:c.status(r.app,8),cursor:c.status(r.app,9)});
   const select=cursor=>{for(let i=0;i<10&&title().cursor!==cursor;i++)key(16);if(title().cursor!==cursor)throw Error('Menu cursor');key(1);};
   const trace=()=>Array.from(new Uint32Array(c.memory.buffer,c.trace(r.app),68));window.probe={c,r,step,wait,key,title,select,trace};wait(100);
  },Array.from(replay));
 }
 await page.evaluate(()=>{probe.select(4);probe.key(1);probe.key(1);probe.key(1);});
 const rows=[];let ticks=0,done=false;
 for(let batch=0;batch<400&&!done;batch++){
  const result=await page.evaluate(async native=>{const rows=[];let done=false,used=0;for(let i=0;i<300;i++){probe.step();used++;if(i%30===0){const c=native?probe.c:probe.core,app=native?probe.r.app:probe.runtime;rows.push({state:Array.from(new Uint32Array(c.memory.buffer,c.trace(app),68)),scene:probe.title().scene});}if(i%30===29)await new Promise(r=>setTimeout(r,0));if(probe.title().scene===1){done=true;break;}}return {rows,done,used};},native);
  rows.push(...result.rows);ticks+=result.used;done=result.done;if(batch%20===0)console.log(JSON.stringify({native,route,ticks,last:rows.at(-1)?.state.slice(0,11)}));
 }
 assert(done,'Replay must finish');assert.equal(errors.length,0);all[native?'native':'baseline']={ticks,rows,errors};await page.screenshot({path:out+'/replay-'+route+'-'+native+'.png'});await context.close();
 }
 assert.equal(all.native.ticks,all.baseline.ticks);assert.deepEqual(all.native.rows,all.baseline.rows,'Replay state, RNG, player and score must remain identical');
 const build=JSON.parse(readFileSync('th08_web/artifacts/architecture-candidate/site/manifest.json'));
 writeFileSync(out+'/replay-'+route+'.json',JSON.stringify({passed:true,route,wasm:build.execution.sha256,ticks:all.native.ticks,samples:all.native.rows.length,stages:[...new Set(all.native.rows.map(r=>r.state[1]))],baseline:'2.2.1 release'+(cached?' (previously verified trace cache)':''),native:all.native}));console.log(JSON.stringify({passed:true,route,wasm:build.execution.sha256,ticks:all.native.ticks,samples:all.native.rows.length}));
}catch(error){writeFileSync(out+'/replay-'+route+'-failure.json',JSON.stringify({error:error.stack,all}));throw error;}finally{await browser.close();}
