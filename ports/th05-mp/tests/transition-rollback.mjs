import assert from 'node:assert/strict';
import {mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {launchBrowser} from '../../../tools/browser/launch.mjs';
import {multiplayerFixture} from './multiplayer-fixture.mjs';

const out=resolve(process.env.TH05_TEST_OUTPUT??resolve(import.meta.dirname,'../artifacts/transition-rollback'));
await mkdir(out,{recursive:true});
const site=await multiplayerFixture(1),browser=await launchBrowser(),page=await browser.newPage(),errors=[];
page.on('pageerror',error=>errors.push(error.message));
const spares=2,bombs=3;
const report={spares,bombs,cases:[]};
try {
 await page.goto(site.url);await page.waitForFunction(()=>events.some(e=>e.event==='ready'));
 for(const [command,payload] of [['configure',{music:'none',language:'ja',sharedResources:[{path:'/data/gothic.ttf',url:'/font'}],options:{}}],['prepare-room',{}]]){
  const result=await page.evaluate(({command,payload})=>window.command(0,command,payload),{command,payload});assert.equal(result.ok,true,JSON.stringify(result));
 }
 for(const count of [2,3])for(const kind of ['dialogue','stage-logo','stage-fade']){
  const frames=kind==='dialogue'?360:kind==='stage-logo'?120:35,stage=0,warp=0;
  const result=await page.evaluate(({count,stage,warp,frames,spares,bombs,kind})=>{
   const m=document.querySelector('iframe').contentWindow.Module;
   const check=(value,message)=>{if(!value)throw Error(message);};
   check(m._th05_coop_start_stock(count,0,stage===6?4:1,741,stage%4,(stage+1)%4,(stage+2)%4,0,0,spares,bombs),'start');
   check(m._th05_native_coop_stage(stage,warp),'stage');
   for(let seat=0;seat<count;seat++){m._th05_native_coop_probe(3,seat,128);m._th05_native_coop_probe(6,seat,10000);}
   if(kind==='dialogue'){
    check(m._th05_native_coop_probe(17,0,0),'dialogue');
    for(let f=0;f<55;f++){for(let seat=0;seat<count;seat++)m._th05_coop_input(seat,0,0,0,0);check(m._th05_tick(0),'text warmup');}
   }else check(m._th05_native_coop_probe(kind==='stage-logo'?18:19,0,0),'animation');
   check(m._th05_native_rollback_checkpoint(0),'oracle checkpoint');
   const input=(seat,f)=>[(f%50<25?4:8),f%7===0?seat*5:0,0,f%7===0?1:0];
   const exactSounds=[],actualSounds=[];let sounds=exactSounds;
   const originalAudio=m.eaglerAudio;m.eaglerAudio={sound:n=>sounds.push(['sound',n]),select:n=>sounds.push(['music',n]),action:(...v)=>sounds.push(['action',...v])};
   try {
    check(m._th05_net_reset(0,0),'exact reset');const oracle=[];
    for(let f=0;f<frames;f++){
     for(let seat=0;seat<count;seat++)check(m._th05_net_input(seat,f,...input(seat,f)),'exact input');
     check(m._th05_net_advance(),'exact tick');oracle.push(m._th05_net_world_hash()>>>0);
    }
    const finalWorld=oracle.at(-1),finalGame=m._th05_coop_checksum()>>>0;
    m._th05_net_close();check(m._th05_native_rollback_checkpoint(1),'oracle restore');
    sounds=actualSounds;check(m._th05_net_reset(0,8),'rollback reset');
    let head=0,clock=0,comparisons=0,stalls=0,maxBytes=0,maxHistory=0;const pending=[],capture=new Set();
    while((head<frames||pending.length||m._th05_net_confirmed()<frames-1)&&clock<frames*30){
     if(head<frames&&!capture.has(head)){
      capture.add(head);check(m._th05_net_input(0,head,...input(0,head)),'local capture');
      for(let seat=1;seat<count;seat++)pending.push({seat,f:head,due:clock+1+(head*7+seat*3)%7});
     }
     // Arrival order deliberately differs from input-frame order.
     for(let i=pending.length-1;i>=0;i--)if(pending[i].due<=clock){const p=pending.splice(i,1)[0];check(m._th05_net_input(p.seat,p.f,...input(p.seat,p.f)),'remote input');}
     const advanced=m._th05_net_advance();if(advanced)head++;else stalls++;
     if(m._th05_net_stat(5)===head&&m._th05_net_confirmed()>=head-1&&head>0){
      const actual=m._th05_net_world_hash()>>>0;if(actual!==oracle[head-1])throw Error('world byte mismatch '+JSON.stringify({count,stage,warp,head,clock,actual,expected:oracle[head-1]}));comparisons++;
     }
     maxBytes=Math.max(maxBytes,m._th05_net_stat(3));maxHistory=Math.max(maxHistory,m._th05_net_stat(4));++clock;
    }
    check(head===frames&&m._th05_net_confirmed()===frames-1,'did not drain confirmed history');
    check((m._th05_net_world_hash()>>>0)===finalWorld,'final world bytes');check((m._th05_coop_checksum()>>>0)===finalGame,'final game checksum');
    check(JSON.stringify(exactSounds)===JSON.stringify(actualSounds),'audio repeated, lost or reordered');
    return {kind,count,stage,warp,frames,comparisons,stalls,rollbacks:m._th05_net_stat(0),resimulated:m._th05_net_stat(1),predicted:m._th05_net_stat(2),maxBytes,maxHistory,audioEvents:actualSounds.length,finalGame};
   }finally{m._th05_net_close();m.eaglerAudio=originalAudio;}
  },{count,stage,warp,frames,spares,bombs,kind});
  assert.ok(result.rollbacks>0,JSON.stringify(result));assert.ok(result.comparisons>0);assert.ok(result.maxHistory<=8);report.cases.push(result);
  console.log(`PASS ${count}P ${kind}: ${result.rollbacks} corrections, ${result.maxBytes} bytes/checkpoint`);
 }
 assert.deepEqual(errors,[]);report.errors=errors;await writeFile(resolve(out,'transitions.json'),JSON.stringify(report,null,2)+'\n');
 console.log('PASS '+report.cases.length+' dense world-byte/audio rollback oracles');
}finally{await browser.close();await site.close();}
