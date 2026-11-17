import assert from 'node:assert/strict';
import {mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {multiplayerFixture} from './multiplayer-fixture.mjs';
import {launchBrowser} from '../../../tools/browser/launch.mjs';
const out=resolve(process.env.TH05_TEST_OUTPUT??resolve(import.meta.dirname,'../artifacts/performance-profile'));await mkdir(out,{recursive:true});
const site=await multiplayerFixture(1),browser=await launchBrowser(),page=await browser.newPage(),errors=[],report={cpuThrottle:4,cases:[]};
page.on('pageerror',e=>errors.push(e.message));
try{
 await page.goto(site.url);await page.waitForFunction(()=>events.some(e=>e.event==='ready'));
 for(const [command,payload] of [['configure',{music:'none',language:'ja',sharedResources:[{path:'/data/gothic.ttf',url:'/font'}],options:{}}],['prepare-room',{}]])assert.equal((await page.evaluate(({command,payload})=>window.command(0,command,payload),{command,payload})).ok,true);
 const cdp=await page.context().newCDPSession(page);await cdp.send('Emulation.setCPUThrottlingRate',{rate:4});await cdp.send('Profiler.enable');await cdp.send('Profiler.setSamplingInterval',{interval:250});
 for(const stage of [2,3])for(const kind of ['logic','draw','rollback']){
  await page.evaluate(stage=>{const m=document.querySelector('iframe').contentWindow.Module;if(!m._th05_coop_start(3,0,3,747,0,1,3,0,0)||!m._th05_native_coop_stage(stage,2))throw Error('stage');for(let i=0;i<3;i++){m._th05_native_coop_probe(6,i,100000);m._th05_native_coop_probe(3,i,128);}for(let f=0;f<1200;f++){for(let seat=0;seat<3;seat++)m._th05_coop_input(seat,0,0,0,0);m._th05_tick(0);}m._th05_net_reset(0,8);},stage);
  await cdp.send('Profiler.start');
  const result=await page.evaluate(async({stage,kind})=>{
   const m=document.querySelector('iframe').contentWindow.Module,samples=[],frames=600;let drawCalls=0,vertices=0;
   for(let frame=0;frame<frames;frame++){
    const input=32|64|(frame%60<30?4:8);
    if(kind!=='rollback')for(let seat=0;seat<3;seat++)m._th05_coop_input(seat,input,0,0,0);
    const start=performance.now();
    if(kind==='rollback'){
     m._th05_net_input(0,frame,input,0,0,0);for(let seat=1;seat<3;seat++)if(frame>=6)m._th05_net_input(seat,frame-6,32|64|((frame-6)%60<30?4:8),0,0,0);
     let safety=0;while(m._th05_net_stat(6)<=frame){if(m._th05_net_advance()<0||++safety>16)throw Error('advance');}
    }else{
     if(!m._th05_tick(0))throw Error('tick');
     if(kind==='draw'){m._th05_coop_draw();m._th05_present();drawCalls+=m._th05_graphics_stat(0);vertices+=m._th05_graphics_stat(1);}
    }
    samples.push(performance.now()-start);if(frame%20===0)await new Promise(r=>setTimeout(r,0));
   }
   samples.sort((a,b)=>a-b);return {stage:stage+1,kind,frames,p50Ms:samples[300],p95Ms:samples[570],maxMs:samples.at(-1),meanMs:samples.reduce((a,b)=>a+b,0)/frames,drawCalls:drawCalls/frames,vertices:vertices/frames,corrections:m._th05_net_stat(0),replayed:m._th05_net_stat(1)};
  },{stage,kind});
  const {profile}=await cdp.send('Profiler.stop');await writeFile(resolve(out,`stage${stage+1}-${kind}.cpuprofile`),JSON.stringify(profile));
  const counts=new Map();for(const id of profile.samples??[])counts.set(id,(counts.get(id)??0)+1);
  result.hot=profile.nodes.map(n=>({function:n.callFrame.functionName,samples:counts.get(n.id)??0})).filter(n=>n.samples).sort((a,b)=>b.samples-a.samples).slice(0,20);
  report.cases.push(result);console.log(JSON.stringify(result));
 }
 assert.deepEqual(errors,[]);report.errors=errors;await writeFile(resolve(out,'performance.json'),JSON.stringify(report,null,2)+'\n');
}finally{await browser.close();await site.close();}
