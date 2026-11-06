import assert from 'node:assert/strict';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {mkdirSync,writeFileSync} from 'node:fs';
const arg=(name,fallback)=>process.argv.find(x=>x.startsWith('--'+name+'='))?.slice(name.length+3)||fallback;
const output=resolve(arg('output','artifacts/pressure'));mkdirSync(output,{recursive:true});
const report={passed:false,errors:[]},save=()=>writeFileSync(resolve(output,'pressure-report.json'),JSON.stringify(report,null,2)+'\n');save();
const {default:puppeteer}=await import(pathToFileURL(process.env.TH19_PUPPETEER_MODULE||resolve(import.meta.dirname,'../../../launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js')).href);
const browser=await puppeteer.launch({executablePath:process.env.TH19_BROWSER,headless:true,timeout:60000,protocolTimeout:240000,args:['--no-sandbox','--use-angle=swiftshader','--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']});
try{
 const page=await browser.newPage();page.on('pageerror',e=>report.errors.push(e.message));
 await page.goto(new URL('tests/harness.html',arg('url','http://127.0.0.1:8140/')).href,{waitUntil:'domcontentloaded',timeout:120000});
 await page.waitForFunction(()=>window.__TH19_HARNESS?.frame?.contentWindow?.__th19Runtime,{timeout:180000});
 report.samples=await page.evaluate(()=>{
  const c=window.__TH19_HARNESS.frame.contentWindow.Module;c._th19_loop_stop();c._th19_loop_pause(1);
  if(!c._th19_network_begin(19019,3,0,1))throw Error('pressure match');
  const read=()=>{const p=c._th19_test_world_pressure(0)/4;return Array.from(c.HEAP32.subarray(p,p+6));},samples=[],slots=[];let frame=0;
  for(let cycle=0;cycle<60;cycle++){
   const before=read();if(before[0]!==2000||before[3]!==2000)throw Error('bullet pool overflow');
   for(let tick=0;tick<200;tick++,frame++){
    if(tick%25===0){slots.push(c._th19_checkpoint_save());if(slots.length>8)c._th19_checkpoint_release(slots.shift());}
    if(c._th19_network_step(frame,0,0,0,0,0,0,0,0,+(tick===199))!==1)throw Error('pressure frame ended');
   }
   const state=read(),id=c._th19_checkpoint_save(),beforeHash=c._th19_network_hash();
   c._th19_network_step(frame,1,1,0,0,0,0,0,0,0);const afterHash=c._th19_network_hash();
   if(!c._th19_checkpoint_load(id)||c._th19_network_hash()!==beforeHash)throw Error('pressure checkpoint restore');
   c._th19_network_step(frame++,1,1,0,0,0,0,0,0,0);if(c._th19_network_hash()!==afterHash)throw Error('pressure resimulation');c._th19_checkpoint_release(id);
   samples.push({cycle,frame,heap:c.HEAPU8.length,bullets:[state[0],state[3]],enemies:[state[1],state[4]],dropped:[state[2],state[5]]});
  }
  for(const id of slots)c._th19_checkpoint_release(id);c._th19_network_end();return samples;
 });
 assert.equal(report.samples.length,60);assert(report.samples.at(-1).frame>=12000);
 const warm=Math.max(...report.samples.slice(10,30).map(s=>s.heap)),late=Math.max(...report.samples.slice(30).map(s=>s.heap));assert(late<=warm+64*1024*1024,'heap keeps growing after warmup');
 assert(report.samples.every(s=>s.bullets.every(n=>n===2000)&&s.enemies.every(n=>n<200)));
 assert.deepEqual(report.errors,[]);report.passed=true;save();console.log(JSON.stringify({passed:true,frames:report.samples.at(-1).frame,warmHeap:warm,lateHeap:late,cycles:60}));
}catch(error){report.failure=error.stack;save();throw error;}finally{await browser.close();}
