import assert from 'node:assert/strict';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {writeFileSync} from 'node:fs';
const {default:p}=await import(pathToFileURL(resolve('launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js')).href);
const browser=await p.launch({executablePath:process.env.TH19_BROWSER,headless:true,protocolTimeout:240000,args:['--no-sandbox','--use-angle=swiftshader','--enable-unsafe-swiftshader']});
const report={menus:[],combat:[],pressure:[]};
try{
 const page=await browser.newPage();page.on('console',m=>console.log(m.text()));
 async function load(){await page.goto('http://127.0.0.1:8140/tests/harness.html?autolaunch=0');await page.waitForFunction(()=>window.__TH19_HARNESS?.configured,{timeout:180000});await page.evaluate(()=>window.__TH19_HARNESS.command('launch'));}
 for(const vs of [false,true]){
  await load();
  const row=await page.evaluate(vs=>{
   const c=window.__TH19_HARNESS.frame.contentWindow.Module;c._th19_loop_stop();
   const state=()=>Array.from(c.HEAP32.subarray(c._th19_achievement_status()/4,c._th19_achievement_status()/4+2));
   const press=key=>{c._th19_key(key,1);c._th19_test_ticks(1);c._th19_key(key,0);c._th19_test_ticks(120);};
   c._th19_test_ticks(600);if(vs)press(81);press(29);if(vs)press(29);
   press(82);press(82);const easy=state();for(let i=0;i<4;i++)press(81);const lunatic=state();press(29);press(27);const back=state();
   return {vs,easy,lunatic,back};
  },vs);
  assert.equal(row.easy[1],0);assert.equal(row.lunatic[1],3);assert.equal(row.back[1],3);report.menus.push(row);
 }
 await load();
 report.combat=await page.evaluate(()=>{
  const c=window.__TH19_HARNESS.frame.contentWindow.Module;c._th19_loop_stop();c._th19_network_begin(19019,3,0,1);
  const read=p=>Array.from(c.HEAP32.subarray(p/4,p/4+20));const result=[];
  for(const rank of [7,4,1]){c._th19_test_round_charge(1,rank);result.push({rank,after:read(c._th19_test_round_charge(2,0))});}
  for(const tier of [3,4]){const before=read(c._th19_test_round_charge(0,0));const after=read(c._th19_test_round_charge(3,tier));result.push({tier,before,after});}
  c._th19_network_end();return result;
 });
 for(const r of report.combat){if(r.rank!==undefined)assert.deepEqual(r.after.slice(0,4),[Math.max(0,r.rank-2),Math.max(0,r.rank-3),Math.max(0,r.rank-2),Math.max(0,r.rank-3)]);else{assert.equal(r.after[9],2);assert.deepEqual(r.after.slice(6,8),r.before.slice(6,8));assert.equal(r.after[8],2500);}}
 // Two independent modules consume identical dense-spirit inputs and compare hashes.
 const runs=[];
 for(let run=0;run<2;run++){
  await load();runs.push(await page.evaluate(()=>{
   const c=window.__TH19_HARNESS.frame.contentWindow.Module;c._th19_loop_stop();c._th19_network_begin(9919,3,0,1);let frame=0;const samples=[];
   for(let cycle=0;cycle<6;cycle++){
    const at=c._th19_test_spirit_pressure(200)/4;const initial=Array.from(c.HEAP32.subarray(at,at+6));console.log(JSON.stringify({cycle,initial,heap:c.HEAPU8.length}));const start=performance.now();
    for(let tick=0;tick<120;tick++,frame++){if(c._th19_network_step(frame,3,3,0,0,0,0,0,0,0)!==1)throw Error('pressure step');}
    const p=c._th19_test_spirit_pressure(0)/4;samples.push({frame,initial,state:Array.from(c.HEAP32.subarray(p,p+6)),hash:c._th19_network_hash()>>>0,heap:c.HEAPU8.length,ms:performance.now()-start});
   }
   c._th19_network_end();return samples;
  }));
 }
 assert.deepEqual(runs[0].map(r=>r.hash),runs[1].map(r=>r.hash));report.pressure=runs;report.passed=true;
}finally{writeFileSync('../th19-round-fixes-report.json',JSON.stringify(report,null,2));await browser.close();}
console.log(JSON.stringify(report));
