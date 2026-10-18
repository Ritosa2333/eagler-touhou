import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync,readFileSync} from 'node:fs';
import {launchBrowser} from '../th10_web/scripts/native/browser-launch.mjs';
const base=process.env.TEST_URL??'http://127.0.0.1:8095',out='th08_web/artifacts/architecture-candidate/validation';
const slot=Number(process.env.SLOT??1),occupied=process.env.OCCUPIED==='1',replayPath='replay/th8_'+String(slot).padStart(2,'0')+'.rpy';
assert(slot>=1&&slot<=15);
mkdirSync(out,{recursive:true});
const browser=await launchBrowser(),context=await browser.newContext({viewport:{width:640,height:480}}),page=await context.newPage(),report={passed:false,errors:[]};
page.on('pageerror',e=>report.errors.push(e.stack));
async function open(seed=false){
 await page.goto(base+'/runtime/th08/th08.html?manual=1');await page.waitForFunction(()=>window.__th08Runtime,null,{timeout:120000});await page.locator('canvas').click();
 await page.evaluate(async({bytes,path})=>{
  const r=__th08Runtime,c=r.core;if(bytes)await r.command({command:'write',path,bytes});await r.launch();c.sdl_touch_options(0,0,1);const p=c.allocate(64);let previous=0;
  const mapping=[[1,'KeyZ'],[2,'KeyX'],[8,'Escape'],[16,'ArrowUp'],[32,'ArrowDown'],[64,'ArrowLeft'],[128,'ArrowRight']];
  const step=(input=0)=>{for(const [bit,key]of mapping)if(!!(input&bit)!==!!(previous&bit)){new Uint8Array(c.memory.buffer,p,64).set(new TextEncoder().encode(key+'\0'));c.sdl_key(p,!!(input&bit));}previous=input;const result=c.sdl_loop_tick(r.app,1/60,17);if(result)throw Error('tick failed '+result+' '+r.status());};
  const wait=n=>{for(let i=0;i<n;i++)step();};const key=k=>{step(k);wait(42);};
  const diag=()=>Array.from(new Int32Array(c.memory.buffer,c.diagnostics(r.app),16));
  const menu=()=>c.menu_diagnostics?Array.from(new Int32Array(c.memory.buffer,c.menu_diagnostics(r.app),12)):[];
  const title=()=>[c.status(r.app,0),c.status(r.app,8),c.status(r.app,9)];
  const light=()=>{const gl=document.querySelector('canvas').getContext('webgl2'),b=new Uint8Array(300*60*4);gl.readPixels(50,370,300,60,gl.RGBA,gl.UNSIGNED_BYTE,b);let sum=0;for(let i=0;i<b.length;i+=4)sum+=b[i]+b[i+1]+b[i+2];return sum/(300*60*3);};
  window.probe={r,c,step,wait,key,diag,menu,title,light};wait(100);
 },{bytes:seed?Array.from(readFileSync(out+'/recorded-first-slot.rpy')):null,path:replayPath});
}
const snapshot=()=>page.evaluate(()=>({status:probe.r.status(),diag:probe.diag(),menu:probe.menu(),title:probe.title()}));
const key=async k=>{await page.evaluate(k=>probe.key(k),k);};
try{
 await open(occupied);for(let i=0;i<5;i++)await key(1);await page.evaluate(()=>probe.wait(240));
 report.start=await snapshot();assert.equal(report.start.title[0],2);
 report.before=await page.evaluate(()=>{probe.step();return probe.light();});await page.screenshot({path:out+'/pause-before.png'});
 await key(8);report.paused=await snapshot();report.dark=await page.evaluate(()=>{probe.wait(30);return probe.light();});await page.screenshot({path:out+'/pause-dark.png'});
 assert(report.paused.diag[6]>0);assert(report.dark<report.before*.8,`Pause must dim: ${report.before} -> ${report.dark}`);
 await key(8);report.resumed=await snapshot();assert.equal(report.resumed.diag[6],0);
 report.bright=await page.evaluate(()=>{probe.step();return probe.light();});assert(report.bright>report.dark*1.2);
 // Let a real run end with no continues, then save through the actual UI.
 for(let n=0;n<150;n++){
  const ended=await page.evaluate(async()=>{for(let i=0;i<300;i++){probe.step();if(probe.diag()[7]||probe.title()[0]!==2)return true;if(i%30===29)await new Promise(r=>setTimeout(r,0));}return false;});
  if(ended)break;
 }
 report.retry=await snapshot();console.log('retry',report.retry);
 if(report.retry.diag[7]){await page.evaluate(()=>probe.wait(100));await key(1);await page.evaluate(()=>probe.wait(100));}
 report.results=await snapshot();console.log('results',report.results);await page.screenshot({path:out+'/replay-result.png'});
 for(let i=0;i<24;i++){const s=await snapshot();if(s.title[0]===6&&s.menu[0]===10)break;await key(1);}
 assert.equal((await snapshot()).menu[0],10,'Reached save-replay question');await key(1);assert.equal((await snapshot()).menu[0],12,'Choose replay slot');
 for(let i=1;i<slot;i++)await key(32);
 await key(1);
 if(occupied){assert.equal((await snapshot()).menu[0],14,'Occupied row must ask before overwrite');await key(1);}
 assert.equal((await snapshot()).menu[0],13,'Name selected replay');
 for(let i=0;i<12;i++){if((await snapshot()).title[0]!==6)break;await key(1);}
 await page.evaluate(()=>probe.r.command({command:'sync'}));report.files=await page.evaluate(()=>probe.r.command({command:'list'}));
 assert(report.files.files.some(f=>f.path===replayPath),'Selected row saves corresponding filename');assert(!report.files.files.some(f=>f.path==='replay/th8_00.rpy'));
 const saved=await page.evaluate(path=>probe.r.command({command:'read',path}),replayPath);assert(saved.bytes.length>100);if(slot===1)writeFileSync(out+'/recorded-first-slot.rpy',Buffer.from(saved.bytes));
 await page.evaluate(()=>probe.r.stop());await open();
 const restored=await page.evaluate(path=>probe.r.command({command:'read',path}),replayPath);assert.deepEqual(restored.bytes,saved.bytes,'Recorded replay survives restart');
 for(const k of [16,16,16,16,16,1])await key(k);
 for(let i=1;i<slot;i++)await key(32);for(let i=0;i<3;i++)await key(1);
 report.playback=await snapshot();assert.equal(report.playback.title[0],2);assert(await page.evaluate(()=>new Uint32Array(probe.c.memory.buffer,probe.c.trace(probe.r.app),5)[4]&8),'Saved recording is playing');
 await page.evaluate(()=>probe.wait(600));assert.equal((await snapshot()).status[2],0);
 await page.evaluate(()=>probe.r.stop());assert.deepEqual(report.errors,[]);report.passed=true;console.log(JSON.stringify(report));
}catch(e){report.failure=e.stack;console.log(await snapshot().catch(()=>({})));await page.screenshot({path:out+'/replay-pause-failure.png'});throw e;}
finally{report.slot=slot;report.occupied=occupied;writeFileSync(out+`/replay-pause${slot===1?'':'-slot'+slot}.json`,JSON.stringify(report,null,2));await browser.close();}
