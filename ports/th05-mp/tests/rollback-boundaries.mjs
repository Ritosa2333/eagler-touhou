import assert from 'node:assert/strict';
import {mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {launchBrowser} from '../../../tools/browser/launch.mjs';
import {multiplayerFixture} from './multiplayer-fixture.mjs';
const out=resolve(process.env.TH05_TEST_OUTPUT??resolve(import.meta.dirname,'../artifacts/rollback-boundaries'));
await mkdir(out,{recursive:true});
const site=await multiplayerFixture(2),browser=await launchBrowser(),page=await browser.newPage(),errors=[];
page.on('pageerror',error=>errors.push(error.message));const report={cases:[]};
try {
 await page.goto(site.url);await page.waitForFunction(()=>events.filter(e=>e.event==='ready').length===2);
 for(let seat=0;seat<2;seat++)for(const [command,payload] of [['configure',{music:'none',language:'ja',sharedResources:[{path:'/data/gothic.ttf',url:'/font'}],options:{}}],['prepare-room',{}]]){
  const result=await page.evaluate(({seat,command,payload})=>window.command(seat,command,payload),{seat,command,payload});assert.equal(result.ok,true,JSON.stringify(result));
 }
 for(const kind of ['dialogue','stage-logo','stage-fade','midboss-create','midboss-retire','mai-branch','yuki-branch','mai-explosion','yuki-explosion','next-stage','next-stage5','ghost-stage5-0','ghost-stage5-1','ghost-stage5-2','ghost-heap-stage5-0','ghost-heap-stage5-1','ghost-heap-stage5-2','stage5-heap','clear-mai','clear-yuki','pause-restart','ghost-rescue','death','death-last','death-all']){
  const result=await page.evaluate(kind=>{
   const modules=[...document.querySelectorAll('iframe')].map(f=>f.contentWindow.Module),frames=kind.startsWith('clear-')?1800:720;
   const check=(v,message)=>{if(!v)throw Error(kind+': '+message);},branching=kind.endsWith('branch')||kind.endsWith('explosion')||kind.startsWith('clear-');
   for(let client=0;client<2;client++){
    const m=modules[client],stage=kind==='stage5-heap'?4:branching||(kind==='next-stage5'||kind.startsWith('ghost-stage5')||kind.startsWith('ghost-heap-stage5'))?3:0,warp=kind.startsWith('midboss')?1:branching?2:0;
    check(m._th05_coop_start(3,client,1,747,0,1,3,0,0),'start');if(kind==='stage5-heap')check(m._th05_native_coop_probe(24,0,client),'poison');check(m._th05_native_coop_stage(stage,warp),'stage');check(m._th05_native_coop_probe(24,0,-1),'poison reset');
    for(let seat=0;seat<3;seat++){m._th05_native_coop_probe(3,seat,128);m._th05_native_coop_probe(6,seat,10000);}
    if(kind==='midboss-retire'){check(m._th05_tick(0),'create');check(m._th05_native_coop_probe(16,0,0),'defeat');}
    if(branching){for(let seat=0;seat<3;seat++)m._th05_coop_input(seat,0,0,0,0);check(m._th05_tick(0),'boss intro');}
    if(branching)check(m._th05_native_coop_probe(kind.endsWith('explosion')?22:15,0,kind.startsWith('mai')||kind==='clear-mai'?1:0),'branch setup');
    if(kind.startsWith('clear-')){for(let f=0;f<400;f++){for(let seat=0;seat<3;seat++)m._th05_coop_input(seat,128,0,0,0);check(m._th05_tick(0),'branch intro');}check(m._th05_native_coop_probe(23,0,0),'defeat remaining boss');}
    if(kind==='dialogue')check(m._th05_native_coop_probe(17,0,0),'dialogue');
    if(kind==='stage-logo')check(m._th05_native_coop_probe(18,0,0),'logo');
    if(kind==='stage-fade')check(m._th05_native_coop_probe(19,0,0),'fade');
    if(kind.startsWith('ghost-heap-stage5')){check(m._th05_native_coop_probe(1,Number(kind.at(-1)),0),'carry ghost');check(m._th05_native_coop_probe(25,0,client),'reused stage heap');}
    if(kind.startsWith('ghost-stage5'))check(m._th05_native_coop_probe(1,Number(kind.at(-1)),0),'carry ghost');
    if(kind==='next-stage'||kind==='next-stage5'||kind.startsWith('ghost-stage5'))check(m._th05_native_coop_probe(12,0,0),'clear');
    if(kind==='ghost-rescue'){m._th05_native_coop_probe(1,2,0);m._th05_native_coop_probe(4,0,4096);m._th05_native_coop_probe(5,0,5120);}
    if(kind==='death-last'||kind==='death-all'){for(let seat=kind==='death-last'?2:0;seat<3;seat++){check(m._th05_native_coop_probe(0,seat,1),'last life');m._th05_native_coop_probe(6,seat,0);m._th05_native_coop_probe(8,seat,0);}}
    if(kind==='death'){m._th05_native_coop_probe(6,2,0);m._th05_native_coop_probe(8,2,0);}
   }
   const input=(seat,f)=>{
    let buttons=kind==='ghost-rescue'?seat===2&&f<10&&f%2===0?64:0:32;
    if(branching||kind==='dialogue'&&f>55)buttons|=128;
    if(kind==='pause-restart'){if(seat===0&&(f===70||f===90))buttons|=4096;if(seat===2&&f===180)buttons|=32768;}
    return [buttons,0,0,0];
   };
   const oracle=modules[0],corrected=modules[1],expected=[];check(oracle._th05_net_reset(0,0),'exact reset');
   for(let f=0;f<frames;f++){
    for(let seat=0;seat<3;seat++)check(oracle._th05_net_input(seat,f,...input(seat,f)),'exact input');check(oracle._th05_net_advance(),'exact tick');
    if(f%3===0)oracle._th05_coop_draw();expected.push(oracle._th05_coop_checksum()>>>0);
   }
   check(corrected._th05_net_reset(0,8),'rollback reset');let head=0,clock=0,comparisons=0;const pending=[],captured=new Set(),scenes=new Set();
   while((head<frames||pending.length||corrected._th05_net_confirmed()<frames-1)&&clock<frames*30){
    if(head<frames&&!captured.has(head)){
     captured.add(head);check(corrected._th05_net_input(1,head,...input(1,head)),'capture');
     for(const seat of [0,2])pending.push({seat,f:head,due:clock+1+(head*3+seat)%7});
    }
    for(let i=pending.length-1;i>=0;i--)if(pending[i].due<=clock){const p=pending.splice(i,1)[0];check(corrected._th05_net_input(p.seat,p.f,...input(p.seat,p.f)),'arrival');}
    const advanced=corrected._th05_net_advance();check(advanced>=0,'correction failed');if(advanced)head++;
    if(clock%2===0&&corrected._th05_net_stat(5)===head)corrected._th05_coop_draw();scenes.add(corrected._th05_input_scene());
    if(head>0&&corrected._th05_net_stat(5)===head&&corrected._th05_net_confirmed()>=head-1){
     check((corrected._th05_coop_checksum()>>>0)===expected[head-1],'confirmed checksum differs at '+head);comparisons++;
    }
    ++clock;
   }
   check(head===frames&&corrected._th05_net_confirmed()===frames-1,'drain');
   check((corrected._th05_coop_checksum()>>>0)===expected.at(-1),'final checksum');
   const stock=Array.from({length:3},(_,seat)=>[0,1,2,3,10].map(field=>corrected._th05_coop_value(seat,field)));
   if(kind.startsWith('ghost-stage5')||kind.startsWith('ghost-heap-stage5')){const ghost=Number(kind.at(-1));check(stock[ghost][0]===0&&stock[ghost][3]===1,'ghost stock must survive stage transition');}
   if(kind==='death-last')check(stock[2][0]===0&&stock[2][3]===1,'final life becomes a ghost');
   if(kind==='death-all')check(stock.every(s=>s[0]===0&&s[3]===1),'all final lives become ghosts');
   const stage=corrected._th05_native_coop_probe(26,0,0);
   if(kind.startsWith('ghost-stage5')||kind.startsWith('ghost-heap-stage5')||kind==='stage5-heap'||kind==='next-stage5'||kind.startsWith('clear-'))check(stage===4,'reached fifth stage');
   if(kind==='ghost-rescue')check(stock[2][3]===0&&stock[0][0]===2&&stock[2][0]===1,'rescue stock');
   return {kind,stage:stage+1,frames,comparisons,predicted:corrected._th05_net_stat(2),callbacks:clock,corrections:corrected._th05_net_stat(0),replayed:corrected._th05_net_stat(1),scenes:[...scenes],stock};
  },kind);
  if(process.env.TH05_BOUNDARY_BASELINE!=='1'&&['dialogue','stage-logo','stage-fade'].includes(kind))assert.ok(result.predicted>0,JSON.stringify(result));
  assert.ok(result.comparisons>0);report.cases.push(result);console.log('PASS '+kind);
 }
 assert.deepEqual(errors,[]);report.errors=errors;await writeFile(resolve(out,'boundaries.json'),JSON.stringify(report,null,2)+'\n');
}finally{await browser.close();await site.close();}
