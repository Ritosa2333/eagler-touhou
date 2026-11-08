import assert from 'node:assert/strict';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {mkdirSync,writeFileSync} from 'node:fs';
const root=resolve(import.meta.dirname,'../../..');
const {default:puppeteer}=await import(pathToFileURL(resolve(root,'launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js')).href);
const out=resolve(root,'archive/th19-biten-c3-contact-20261107');mkdirSync(out,{recursive:true});
const browser=await puppeteer.launch({executablePath:process.env.TH19_BROWSER,headless:true,args:['--no-sandbox','--use-angle=swiftshader','--enable-unsafe-swiftshader']});
const report={passed:false,sides:[],errors:[]};
try{
 const page=await browser.newPage();await page.setViewport({width:1280,height:960});page.on('pageerror',e=>report.errors.push(e.message));
 await page.goto('http://127.0.0.1:8140/tests/harness.html?autolaunch=0');
 await page.waitForFunction(()=>window.__TH19_HARNESS?.configured,{timeout:120000});
 await page.evaluate(async()=>{await window.__TH19_HARNESS.command('launch');window.c=window.__TH19_HARNESS.frame.contentWindow.Module;c._th19_loop_stop();});
 for(const side of [0,1]){
  await page.evaluate(side=>{c._th19_test_character(14);c._th19_test_cpu_character(14);c._th19_test_ticks(400);c._th19_test_biten_contact(side,1);},side);
  const samples=[];let checked=0,diagonal=0,warning=0;
  for(let frame=1;frame<=160;frame++){
   const rows=await page.evaluate(side=>{c._th19_test_ticks(1);let p=c._th19_test_biten_contact(side,0)/4;const n=c.HEAP32[p++];return Array.from({length:n},(_,i)=>Array.from(c.HEAP32.subarray(p+i*12,p+(i+1)*12)));},side);
   for(const row of rows){
    assert.equal(row[6],32);assert.equal(row[7],6);
    if(row[1]&2){warning++;continue;}
    assert(Math.abs(row[4]-row[5])<1000,`contact angle follows motion: ${row}`);
    assert.equal(row[8],1,'rod long-axis point hits');assert.equal(row[9],0,'point beside rod misses');assert.equal(row[10],0,'point beyond rod tip misses');checked++;
    if(Math.abs(Math.sin(row[5]/1e6))>.5)diagonal++;
   }
   if(frame%20===0)samples.push({frame,rows});
   if(frame===100)await page.screenshot({path:resolve(out,`c3-received-p${side+1}.png`)});
  }
  assert(checked>100);assert(diagonal>50);assert(warning>0);report.sides.push({side,checked,diagonal,warning,samples});
 }
 assert.deepEqual(report.errors,[]);report.passed=true;console.log(JSON.stringify({passed:true,sides:report.sides.map(({samples,...s})=>s)}));
}finally{writeFileSync(resolve(out,'report.json'),JSON.stringify(report,null,2));await browser.close();}
