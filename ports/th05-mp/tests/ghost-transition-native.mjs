import assert from 'node:assert/strict';
import {mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {launchBrowser} from '../../../tools/browser/launch.mjs';
import {multiplayerFixture} from './multiplayer-fixture.mjs';
const out=resolve(process.env.TH05_TEST_OUTPUT??resolve(import.meta.dirname,'../artifacts/ghost-transition'));
await mkdir(out,{recursive:true});const site=await multiplayerFixture(2),browser=await launchBrowser(),page=await browser.newPage(),errors=[],report={cases:[]};
page.on('pageerror',e=>errors.push(e.message));
try{
 await page.goto(site.url);await page.waitForFunction(()=>events.filter(e=>e.event==='ready').length===2);
 for(let seat=0;seat<2;seat++)for(const [command,payload] of [['configure',{music:'none',language:'ja',sharedResources:[{path:'/data/gothic.ttf',url:'/font'}],options:{}}],['prepare-room',{}]])assert.equal((await page.evaluate(({seat,command,payload})=>window.command(seat,command,payload),{seat,command,payload})).ok,true);
 for(const count of [2,3])for(const characters of [[0,0,0],[1,2,3],[2,3,0],[3,1,2]])for(let ghost=0;ghost<count;ghost++){
  const row=await page.evaluate(({count,characters,ghost})=>{
   const modules=[...document.querySelectorAll('iframe')].map(f=>f.contentWindow.Module),frames=360,check=(v,label)=>{if(!v)throw Error(label);};
   for(let client=0;client<2;client++){
    const m=modules[client];m._th05_net_close();m.eaglerAudio={sound(){},select(){},action(){}};
    check(m._th05_coop_start(count,client,1,927,...characters,0,0),'start');check(m._th05_native_coop_stage(3,0),'fourth stage');
    for(let seat=0;seat<count;seat++){m._th05_native_coop_probe(3,seat,128);m._th05_native_coop_probe(6,seat,seat===ghost?0:10000);m._th05_coop_input(seat,0,0,0,0);}
    check(m._th05_native_coop_probe(0,ghost,1),'last life');check(m._th05_native_coop_probe(8,ghost,0),'collision');
    for(let n=0;n<40;n++)check(m._th05_tick(0),'death animation');
    check(m._th05_coop_value(ghost,0)===0&&m._th05_coop_value(ghost,3)===1,'life exhausted into ghost');
    // Model different allocator reuse on the two devices while carrying the
    // actual death result through the same stage teardown/initializer sequence.
    check(m._th05_native_coop_probe(25,0,client),'carry into fifth stage');
    check(m._th05_native_coop_probe(26,0,0)===4,'fifth stage');
   }
   const input=(seat,f)=>[32|(f%48<24?4:8),f%7===0?seat+1:0,0,1];
   const oracle=modules[0],actual=modules[1],expected=[];check(oracle._th05_net_reset(0,0),'oracle reset');
   for(let f=0;f<frames;f++){for(let seat=0;seat<count;seat++)check(oracle._th05_net_input(seat,f,...input(seat,f)),'oracle input');check(oracle._th05_net_advance(),'oracle frame');if(f%3===0)oracle._th05_coop_draw();expected.push(oracle._th05_coop_checksum()>>>0);}
   check(actual._th05_net_reset(0,8),'rollback reset');let head=0,clock=0,comparisons=0;const captured=new Set(),pending=[];
   while((head<frames||pending.length||actual._th05_net_confirmed()<frames-1)&&clock<frames*30){
    if(head<frames&&!captured.has(head)){captured.add(head);check(actual._th05_net_input(1,head,...input(1,head)),'local');for(let seat=0;seat<count;seat++)if(seat!==1)pending.push({seat,f:head,due:clock+1+(head+seat*3)%7});}
    for(let i=pending.length-1;i>=0;i--)if(pending[i].due<=clock){const p=pending.splice(i,1)[0];check(actual._th05_net_input(p.seat,p.f,...input(p.seat,p.f)),'arrival');}
    const advanced=actual._th05_net_advance();check(advanced>=0,'advance');if(advanced)head++;
    if(clock%2===0&&actual._th05_net_stat(5)===head)check(actual._th05_coop_draw(),'draw');
    if(head&&actual._th05_net_stat(5)===head&&actual._th05_net_confirmed()>=head-1){check((actual._th05_coop_checksum()>>>0)===expected[head-1],'confirmed mismatch at '+head);comparisons++;}clock++;
   }
   check(head===frames&&actual._th05_net_confirmed()===frames-1&&comparisons>0,'confirmed drain');check(actual._th05_coop_value(ghost,3)===1&&actual._th05_coop_value(ghost,0)===0,'ghost carried');
   const confirmedHashes=[];for(let frame=120;frame<=frames;frame+=120){const expected=oracle._th05_net_hash(frame)>>>0,observed=actual._th05_net_hash(frame)>>>0;check(observed===expected,'room checksum mismatch at '+frame);confirmedHashes.push({frame,hash:observed});}
   return {count,characters,ghost,frames,comparisons,confirmedHashes,rollbacks:actual._th05_net_stat(0),passed:true};
  },{count,characters,ghost});report.cases.push(row);console.log('PASS '+JSON.stringify(row));
 }
 assert.deepEqual(errors,[]);report.errors=errors;report.passed=true;await writeFile(resolve(out,'ghost-transition.json'),JSON.stringify(report,null,2)+'\n');
}finally{await browser.close();await site.close();}
