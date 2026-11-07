import assert from 'node:assert/strict';
import {mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {launchBrowser} from '../../../tools/browser/launch.mjs';
import {multiplayerFixture} from './multiplayer-fixture.mjs';
const out=resolve(process.env.TH05_TEST_OUTPUT??resolve(import.meta.dirname,'../artifacts/rollback-performance'));
await mkdir(out,{recursive:true});
const site=await multiplayerFixture(1),browser=await launchBrowser(),page=await browser.newPage();
try {
 await page.goto(site.url);await page.waitForFunction(()=>events.some(e=>e.event==='ready'));
 for(const [command,payload] of [['configure',{music:'none',language:'ja',sharedResources:[{path:'/data/gothic.ttf',url:'/font'}],options:{}}],['prepare-room',{}]]){
  const response=await page.evaluate(({command,payload})=>window.command(0,command,payload),{command,payload});assert.equal(response.ok,true);
 }
 const cdp=await page.context().newCDPSession(page);await cdp.send('Emulation.setCPUThrottlingRate',{rate:4});
 const report={cpuThrottle:4,scope:'WASM simulation, checkpoint capture and correction; drawing measured separately by room tests',cases:[]};
 for(const stage of [2,3])for(const lag of [0,2,6]){
  const result=await page.evaluate(({stage,lag})=>{
   const m=document.querySelector('iframe').contentWindow.Module,frames=600;
   const check=(v,message)=>{if(!v)throw Error(message);};
   check(m._th05_coop_start(3,0,3,747,0,1,3,0,0),'start');check(m._th05_native_coop_stage(stage,2),'stage');
   for(let seat=0;seat<3;seat++)m._th05_native_coop_probe(6,seat,100000);
   for(let f=0;f<1200;f++){for(let seat=0;seat<3;seat++)m._th05_coop_input(seat,0,0,0,0);check(m._th05_tick(0),'warmup');}
   check(m._th05_net_reset(0,lag?8:0),'reset');
   const input=(seat,f)=>[32|64|(f%60<30?4:8),0,0,0],samples=[],pending=[],captured=new Set();let head=0,clock=0,maxBytes=0;
   while((head<frames||pending.length||m._th05_net_confirmed()<frames-1)&&clock<frames*30){
    if(head<frames&&!captured.has(head)){
     captured.add(head);m._th05_net_input(0,head,...input(0,head));
     for(let seat=1;seat<3;seat++)pending.push({seat,f:head,due:clock+lag});
    }
    for(let i=pending.length-1;i>=0;i--)if(pending[i].due<=clock){const p=pending.splice(i,1)[0];m._th05_net_input(p.seat,p.f,...input(p.seat,p.f));}
    const start=performance.now(),advanced=m._th05_net_advance();check(advanced>=0,'advance');samples.push(performance.now()-start);if(advanced)head++;
    maxBytes=Math.max(maxBytes,m._th05_net_stat(3));clock++;
   }
   check(head===frames&&m._th05_net_confirmed()===frames-1,'drain');
   samples.sort((a,b)=>a-b);const total=samples.reduce((a,b)=>a+b,0);
   return {stage:stage+1,players:3,frames,lagFrames:lag,calls:samples.length,corrections:m._th05_net_stat(0),replayed:m._th05_net_stat(1),maxBytes,p50Ms:samples[Math.floor(samples.length*.5)],p95Ms:samples[Math.floor(samples.length*.95)],maxCallMs:samples.at(-1),amortizedMsPerNewFrame:total/frames};
  },{stage,lag});report.cases.push(result);console.log(JSON.stringify(result));
 }
 await writeFile(resolve(out,'performance.json'),JSON.stringify(report,null,2)+'\n');
}finally{await browser.close();await site.close();}
