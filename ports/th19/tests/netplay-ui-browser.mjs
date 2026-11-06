import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import puppeteer from '../../../launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js';
const output=resolve('outputs/th19-netplay-ui');mkdirSync(output,{recursive:true});
const browser=await puppeteer.launch({executablePath:process.env.TH19_BROWSER,headless:true,args:['--no-sandbox','--use-angle=swiftshader','--enable-unsafe-swiftshader']});
try{
 const page=await browser.newPage();await page.setViewport({width:1100,height:900});
 await page.goto('http://127.0.0.1:8148/tests/harness.html');await page.waitForFunction(()=>window.__TH19_HARNESS?.frame?.contentWindow?.__th19Runtime,{timeout:120000});
 const results=[];
 for(const scene of ['pause','result','dialogue']){
  const result=await page.evaluate(async scene=>{
   const c=window.__TH19_HARNESS.frame.contentWindow.Module;c._th19_loop_stop();c._th19_network_end();c._th19_network_set_rules(0,1,1,4,4,1,1,0,0);c._th19_network_begin(190001,3,0,1);
   if(scene!=='pause')c._th19_test_vs_rules(10,1);
   const {Netplay}=await import('/runtime/netplay.mjs');
   let now=0;const queue=[],p=new Netplay(c);Object.assign(p,{side:0,delay:0,lastTick:0,budget:0,inputBudget:0,nextInputFrame:0});
   globalThis.__th19PeerTransport={peers:new Map([[1,{rttMs:100}]])};
   p.sample=()=>({keys:scene==='pause'&&p.nextInputFrame===1?1024:0,direct:0,x:0,y:0});
   p.send=bytes=>{if(bytes[5]===3)queue.push({at:now+50,frame:new DataView(bytes.buffer).getUint32(6,true)});};
   p.confirm=()=>{};p.compare=()=>{};p.prepareRollback();
   const duration=scene==='dialogue'?10000:3000,start=performance.now();
   for(now=8;now<=duration;now+=8){
    while(queue.length&&queue[0].at<=now)p.rollback.receive(queue.shift().frame,{keys:0,direct:0,x:0,y:0});
    p.pumpRollback(now);
   }
   const state=Array.from(c.HEAP32.subarray(c._th19_test_vs_rules(-1,0)/4,c._th19_test_vs_rules(-1,0)/4+16));
   return {scene,frames:p.frame,schedulingFps:p.frame*1000/duration,cpuWallMs:performance.now()-start,state,canPredict:c._th19_rollback_can_predict(0)};
  },scene);
  assert.ok(result.schedulingFps>55,JSON.stringify(result));assert.equal(result.canPredict,0,JSON.stringify(result));
  if(scene==='result')assert.ok(result.state[5]>=120&&result.state[5]<320);
  if(scene==='dialogue')assert.equal(result.state[10],10);
  results.push(result);await page.screenshot({path:resolve(output,scene+'.png')});
 }
 writeFileSync(resolve(output,'report.json'),JSON.stringify({passed:true,results},null,2)+'\n');console.log(JSON.stringify({passed:true,results}));
}finally{await browser.close();}

