import assert from 'node:assert/strict';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {mkdirSync,writeFileSync} from 'node:fs';
const root=resolve(import.meta.dirname,'../../..');
const {default:puppeteer}=await import(pathToFileURL(resolve(root,'launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js')).href);
const out=resolve(root,'archive/th19-chiyari-aura-20261107');mkdirSync(out,{recursive:true});
const browser=await puppeteer.launch({executablePath:process.env.TH19_BROWSER,headless:true,args:['--no-sandbox','--use-angle=swiftshader','--enable-unsafe-swiftshader']});
const report={passed:false,scenes:[],errors:[]};
try{
 const page=await browser.newPage();await page.setViewport({width:1280,height:960});page.on('pageerror',e=>report.errors.push(e.message));
 await page.goto('http://127.0.0.1:8140/tests/harness.html?autolaunch=0');
 await page.waitForFunction(()=>window.__TH19_HARNESS?.configured,{timeout:120000});
 await page.evaluate(async()=>{await window.__TH19_HARNESS.command('launch');window.c=window.__TH19_HARNESS.frame.contentWindow.Module;c._th19_loop_stop();});
 for(const side of [0,1]){
  await page.evaluate(side=>{c._th19_test_character(16);if(side)c._th19_test_cpu_character(16);c._th19_test_ticks(400);c._th19_test_aura_damage(side,1);if(side)c._th19_test_cpu_input(2);else c._th19_key(27,1);},side);
  let previous=100000;const pulses=[];
  for(let frame=1;frame<=30;frame++){
   const state=await page.evaluate(side=>{c._th19_test_ticks(1);const p=c._th19_test_aura_damage(side,0)/4;return Array.from(c.HEAP32.subarray(p,p+8));},side);
   if(state[0]!==previous){assert.equal(previous-state[0],4);pulses.push(frame);previous=state[0];}
   assert.equal(state[2],100000,'outside aura remains undamaged');assert.equal(state[6],0,'held-X test has no projectiles');
   report.scenes.push({side,frame,state});
  }
  assert(pulses.length>=4,'aura must produce repeated damage');for(let i=1;i<pulses.length;i++)assert.equal(pulses[i]-pulses[i-1],7);
  assert(report.scenes.at(-1).state[1]<100000,'expanded aura hits inside boundary');
  await page.screenshot({path:resolve(out,`held-x-p${side+1}.png`)});
 }
 // One-frame tap: allow its already-created one-frame area to finish; no
 // further aura pulses may be generated during the visual fade-out.
 const tail=await page.evaluate(()=>{c._th19_test_character(16);c._th19_test_ticks(400);c._th19_test_aura_damage(0,1);c._th19_key(27,1);c._th19_test_ticks(1);c._th19_key(27,0);c._th19_test_ticks(2);const read=()=>{const p=c._th19_test_aura_damage(0,0)/4;return Array.from(c.HEAP32.subarray(p,p+8));};const first=read();c._th19_test_ticks(25);return {first,last:read()};});
 assert.equal(tail.first[0],tail.last[0]);assert.equal(tail.last[4],0);report.tail=tail;
 const other=await page.evaluate(()=>{c._th19_test_character(0);c._th19_test_ticks(400);c._th19_test_aura_damage(0,1);c._th19_key(27,1);c._th19_test_ticks(30);const p=c._th19_test_aura_damage(0,0)/4;return Array.from(c.HEAP32.subarray(p,p+8));});
 assert.equal(other[0],100000,'other characters do not acquire Chiyari damage');report.other=other;
 assert.deepEqual(report.errors,[]);report.passed=true;console.log(JSON.stringify({passed:true,pulses:report.scenes.filter(s=>s.frame===30),tail}));
}finally{writeFileSync(resolve(out,'report.json'),JSON.stringify(report,null,2));await browser.close();}
