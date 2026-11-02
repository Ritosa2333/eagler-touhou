import assert from 'node:assert/strict';
import {mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {multiplayerFixture} from './multiplayer-fixture.mjs';
import {launchBrowser} from '../../../tools/browser/launch.mjs';
assert.ok(process.env.TH05_BASELINE_RUNTIME,'Provide the saved baseline Runtime');
const out=resolve(process.env.TH05_TEST_OUTPUT??resolve(import.meta.dirname,'../artifacts/performance-comparison'));await mkdir(out,{recursive:true});
const current=process.env.TH05_TEST_RUNTIME;process.env.TH05_TEST_RUNTIME=process.env.TH05_BASELINE_RUNTIME;const before=await multiplayerFixture(1);
if(current)process.env.TH05_TEST_RUNTIME=current;else delete process.env.TH05_TEST_RUNTIME;
const after=await multiplayerFixture(1),browser=await launchBrowser(),pages=[],errors=[],report={cpuThrottle:4,scope:'Sequential paired desktop CPU samples; native logic and checkpoint/correction, plus CPU-side draw submission. Audio disabled during timing; no phone FPS claim.',cases:[]};
try{
 for(const site of [before,after]){const page=await browser.newPage();pages.push(page);page.on('pageerror',e=>errors.push(e.message));await page.goto(site.url);await page.waitForFunction(()=>events.some(e=>e.event==='ready'));
  for(const [command,payload] of [['configure',{music:'none',language:'ja',sharedResources:[{path:'/data/gothic.ttf',url:'/font'}],options:{}}],['prepare-room',{}]])assert.equal((await page.evaluate(({command,payload})=>window.command(0,command,payload),{command,payload})).ok,true);
  const cdp=await page.context().newCDPSession(page);await cdp.send('Emulation.setCPUThrottlingRate',{rate:4});
 }
 for(let round=0;round<3;round++)for(const stage of [2,3])for(const kind of ['logic','draw','rollback']){
  for(const version of round%2?[1,0]:[0,1]){
   const result=await pages[version].evaluate(({stage,kind})=>{
    const m=document.querySelector('iframe').contentWindow.Module;m.eaglerAudio={sound(){},select(){},action(){}};
    if(!m._th05_coop_start(3,0,3,747,0,1,3,0,0)||!m._th05_native_coop_stage(stage,2))throw Error('stage');
    for(let seat=0;seat<3;seat++){m._th05_native_coop_probe(6,seat,100000);m._th05_native_coop_probe(3,seat,128);}
    for(let f=0;f<1200;f++){for(let seat=0;seat<3;seat++)m._th05_coop_input(seat,0,0,0,0);m._th05_tick(0);}
    m._th05_net_reset(0,8);const samples=[],frames=900;let draws=0,vertices=0;
    for(let frame=0;frame<frames;frame++){
     const input=32|64|(frame%60<30?4:8);
     if(kind!=='rollback')for(let seat=0;seat<3;seat++)m._th05_coop_input(seat,input,0,0,0);
     const start=performance.now();
     if(kind==='rollback'){
      m._th05_net_input(0,frame,input,0,0,0);for(let seat=1;seat<3;seat++)if(frame>=6)m._th05_net_input(seat,frame-6,32|64|((frame-6)%60<30?4:8),0,0,0);
      let safety=0;while(m._th05_net_stat(6)<=frame){if(m._th05_net_advance()<0||++safety>16)throw Error('advance');}
     }else{if(!m._th05_tick(0))throw Error('tick');if(kind==='draw'){m._th05_coop_draw();m._th05_present();draws+=m._th05_graphics_stat(0);vertices+=m._th05_graphics_stat(1);}}
     samples.push(performance.now()-start);
    }
    samples.sort((a,b)=>a-b);return {stage:stage+1,kind,frames,p50Ms:samples[450],p95Ms:samples[855],maxMs:samples.at(-1),meanMs:samples.reduce((a,b)=>a+b,0)/frames,drawCalls:draws/frames,vertices:vertices/frames,hash:m._th05_coop_checksum()>>>0};
   },{stage,kind});
   result.round=round;result.version=version?'optimized':'baseline';report.cases.push(result);console.log(JSON.stringify(result));
  }
 }
 for(const item of report.cases.filter(c=>c.version==='optimized'))assert.equal(item.hash,report.cases.find(c=>c.version==='baseline'&&c.stage===item.stage&&c.kind===item.kind&&c.round===item.round).hash,'Paired timing runs must finish in the same game state');
 assert.deepEqual(errors,[]);report.errors=errors;await writeFile(resolve(out,'performance.json'),JSON.stringify(report,null,2)+'\n');
}finally{await browser.close();await before.close();await after.close();}
