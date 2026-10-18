// Real SDL game, natural enemy collision and normal input; no player-state writes.
import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {launchBrowser} from '../th10_web/scripts/native/browser-launch.mjs';
const arg=(name,fallback)=>process.argv.includes(name)?process.argv[process.argv.indexOf(name)+1]:fallback;
const base=arg('--base','http://127.0.0.1:8095'),label=arg('--label','after'),method=arg('--input','touch');
const out=resolve(import.meta.dirname,'../artifacts/touch-bomb');mkdirSync(out,{recursive:true});
const browser=await launchBrowser(),page=await browser.newPage({viewport:{width:390,height:844},isMobile:true,hasTouch:true}),errors=[];let frame=page;
page.on('pageerror',e=>errors.push(String(e)));let report={passed:false,base,label,method,physicalPhone:false};
try{
 if(method==='launcher'){
  await page.exposeFunction('pressBombFromTest',async()=>{await page.locator('#touchBomb').dispatchEvent('pointerdown',{pointerId:19,pointerType:'touch'});await page.locator('#touchBomb').dispatchEvent('pointerup',{pointerId:19,pointerType:'touch'});});
  await page.goto(base+'/?debug=1');await page.locator('.game[data-game="th08"]').tap();await page.locator('#launch').tap();
  await page.waitForFunction(()=>document.querySelector('#gameFrame')?.contentWindow?.__th08Runtime?.app,null,{timeout:120000});
  frame=page.frames().find(f=>f.url().includes('/runtime/th08/'));await frame.waitForFunction(()=>__th08Runtime.status()[9]>3);await frame.evaluate(()=>__th08Runtime.core.sdl_loop_stop());
  if(await page.locator('#touchHelpClose').isVisible())await page.locator('#touchHelpClose').tap();
 }else{
  await page.goto(base+'/runtime/th08/th08.html?manual=1',{waitUntil:'domcontentloaded'});
  await page.waitForFunction(()=>window.__th08Runtime||document.querySelector('#error')?.textContent,null,{timeout:120000});
  assert.equal(await page.locator('#error').textContent(),'');await page.locator('canvas').tap();
 }
 await frame.evaluate(async()=>{await __th08Runtime.launch();const c=__th08Runtime.core;c.sdl_touch_options(1,0,1);c.sdl_touch_mode(0);});
 const result=await frame.evaluate(async method=>{
  const r=__th08Runtime,c=r.core;
  const read=()=>{const p=c.trace(r.app),v=new DataView(c.memory.buffer),g=p+44;return {state:r.status(),frame:v.getUint32(p+8,true),life:v.getUint32(p+36,true),bombs:v.getFloat32(g+0x80,true),lives:v.getFloat32(g+0x74,true),deaths:v.getFloat32(g+0x64,true),used:v.getFloat32(g+0x84,true),input:v.getInt32(c.diagnostics(r.app)+44,true)};};
  function tick(){const code=c.sdl_loop_tick(r.app,1/60,0);if(code)throw Error('Tick failed '+code);}
  async function ticks(n){c.sdl_defer(1);try{for(let i=0;i<n;i++)tick();}finally{c.sdl_commit();c.sdl_defer(0);}await new Promise(done=>setTimeout(done,0));}
  await ticks(360);for(let n=0;n<5;n++){await r.command({command:'keyboard',code:'KeyZ',down:true});await ticks(3);await r.command({command:'keyboard',code:'KeyZ',down:false});await ticks(70);}
  if(read().state[0]!==2)throw Error('Not in gameplay '+JSON.stringify(read()));
  c.sdl_touch_controls(0,0,0,0,0,0);let hit=null;
  for(let n=0;n<12000&&!hit;n++){
   tick();const s=read();if(s.life===2&&s.bombs>0){hit=s;break;}
   if(n%120===119)await new Promise(done=>setTimeout(done,0));
  }
  if(!hit)throw Error('No natural hit observed');
  await ticks(method==='late'?40:2);const before=read();
  if(method==='keyboard')await r.command({command:'keyboard',code:'KeyX',down:true});
  else if(method==='launcher'){await window.pressBombFromTest();await new Promise(done=>setTimeout(done,50));}
  else if(method==='double-tap'){c.sdl_touch_gestures(0,1);c.sdl_touch(0,10,.5,.5);c.sdl_touch(2,10,.5,.5);c.sdl_touch(0,11,.5,.5);c.sdl_touch(2,11,.5,.5);}
  else await r.command({command:'touch-controls',controls:{fireEnabled:false,focusEnabled:false,bombSerial:1,escapeSerial:0,joystickX:0,joystickY:0}});
  const sequence=[];for(let n=0;n<6;n++){tick();sequence.push(read());}
  if(method==='keyboard')await r.command({command:'keyboard',code:'KeyX',down:false});
  const immediate=read();await ticks(90);const after=read();
  return {hit,before,immediate,after,sequence,deathbomb:immediate.bombs===before.bombs-2&&immediate.used===before.used+1&&immediate.life===3};
 },method);
 report={...report,...result,errors};
 await page.screenshot({path:resolve(out,label+'-'+method+'.png')});
 assert.deepEqual(errors,[]);
 if(method==='late'){assert.equal(result.deathbomb,false);assert.equal(result.after.used,result.hit.used,'Late bomb must not fire after respawn');assert.equal(result.after.lives,result.hit.lives-1);}
 else if(process.argv.includes('--expect-blocked')){assert.equal(result.deathbomb,false);assert(result.after.deaths>result.before.deaths);}
 else{assert.equal(result.deathbomb,true,'Bomb must reach the original last-spell handler');assert.equal(result.after.deaths,result.before.deaths);assert.equal(result.after.lives,result.before.lives);}
 report.passed=true;console.log(JSON.stringify(report));
}catch(error){report.error=error.stack;throw error;}finally{writeFileSync(resolve(out,label+'-'+method+'.json'),JSON.stringify(report,null,2)+'\n');await browser.close();}
