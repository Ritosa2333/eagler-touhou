import assert from 'node:assert/strict';
import {multiplayerFixture} from './multiplayer-fixture.mjs';
import {readFile,writeFile,mkdir} from 'node:fs/promises';
import {resolve,extname} from 'node:path';
import {launchBrowser} from '../../../tools/browser/launch.mjs';

const repo=resolve(import.meta.dirname,'../../..'),root=resolve(repo,'ports/th05-mp');
const out=resolve(process.env.TH05_TEST_OUTPUT??resolve(root,'artifacts/multiplayer-native'));await mkdir(out,{recursive:true});
const site=await multiplayerFixture(3);
const browser=await launchBrowser(),page=await browser.newPage({viewport:{width:1320,height:1020}}),errors=[],report={};
page.on('pageerror',e=>errors.push(e.message));
try{
 await page.goto(site.url);await page.waitForFunction(()=>events.filter(e=>e.event==='ready').length===3,null,{timeout:30000});
 for(let seat=0;seat<3;seat++){
  for(const [command,payload] of [['configure',{music:'none',language:'ja',sharedResources:[{path:'/data/gothic.ttf',url:'/font'}],options:{}}],['prepare-room',{}]]){
   const response=await page.evaluate(async({seat,command,payload})=>{if(command==='configure'&&seat===1){payload.language='lang_zh-hans';payload.runtimePack=await(await fetch('/language.json')).json();for(const file of payload.runtimePack.files)file.bytes=new Uint8Array(file.bytes);}return window.command(seat,command,payload);},{seat,command,payload});assert.equal(response.ok,true,JSON.stringify(response));
  }
 }
 report.modifiers=await page.evaluate(()=>{
  return [...document.querySelectorAll('iframe')].map(f=>{
   const w=f.contentWindow,m=w.Module,previous=w.__eaglerGamepadInputBridge;let reads=0;
   try {
    for(let n=0;n<8;n++){
     reads=0;m._th05_clear();
     w.__eaglerGamepadInputBridge={sample:()=>{++reads;if(reads===2){m._th05_key(162,1);return 256;}return 0;}};
     if(!m._th05_step())throw Error('modifier frame failed');
     m._th05_clear();
    }
    return m._th05_alive();
   } finally {w.__eaglerGamepadInputBridge=previous;m._th05_clear();}
  });
 });
 assert.deepEqual(report.modifiers,[1,1,1]);
 report.resources=await page.evaluate(()=>{
  const m=document.querySelector('iframe').contentWindow.Module;if(!m._th05_coop_start(3,0,1,741,0,1,2))throw Error('start');
  const probe=(op,seat,value)=>{if(!m._th05_native_coop_probe(op,seat,value))throw Error('probe');};
  const value=(seat,field)=>m._th05_coop_value(seat,field),result={initial:[0,1,2].map(i=>[value(i,0),value(i,1),value(i,2)])};
  probe(2,1,3);result.largePower=[0,1,2].map(i=>value(i,2));
  probe(3,0,128);probe(2,0,0);result.fullPowerSupport=[0,1,2].map(i=>value(i,2));
  probe(2,2,4);result.teamBomb=[0,1,2].map(i=>value(i,1));
  probe(1,2,0);probe(2,0,5);result.teamRevive=[0,1,2].map(i=>[value(i,0),value(i,3)]);
  // P3's ghost requests a life from P1 with five fresh focus edges.
  probe(1,2,0);probe(4,0,4096);probe(5,0,5120);probe(4,2,4096);probe(5,2,5120);probe(7,0,5);
  m._th05_native_coop_stage(0,0); // Remove title-logo timing before edge tests.
  probe(1,2,0);probe(4,0,4096);probe(5,0,5120);probe(4,2,4096);probe(5,2,5120);probe(7,0,5);
  for(let n=0;n<10;n++){
   for(let i=0;i<3;i++)m._th05_coop_input(i,i===2&&n%2===0?64:0,0,0,0);
   m._th05_tick(0);
  }
  result.rescue=[0,1,2].map(i=>[value(i,0),value(i,1),value(i,3)]);
  m._th05_coop_start(2,0,1,741,0,1,2,0);m._th05_native_coop_stage(0,0);probe(1,0,0);probe(1,1,0);m._th05_coop_draw();
  result.allGhostMenu=m._th05_input_scene();
  for(let n=0;n<60;n++){m._th05_coop_input(0,n===0?32:0,0,0,0);m._th05_coop_input(1,0,0,0,0);m._th05_tick(0);}
  result.allGhostRestart=[value(0,0),value(1,0),value(0,3),value(1,3)];
  m._th05_coop_start(2,0,1,741,0,1,2,1);m._th05_native_coop_stage(0,0);probe(6,0,0);probe(8,0,0);
  for(let n=0;n<50;n++){m._th05_coop_input(0,0,0,0,0);m._th05_coop_input(1,0,0,0,0);m._th05_tick(0);}
  probe(10,0,0);result.challenge=[value(0,0),value(0,1),value(0,10)];
  return result;
 });
 assert.deepEqual(report.resources.initial,[[3,3,1],[3,3,1],[3,3,1]]);
 assert.deepEqual(report.resources.largePower,[1,21,1]);assert.deepEqual(report.resources.fullPowerSupport,[128,21,3]);assert.deepEqual(report.resources.teamBomb,[4,4,4]);
 assert.deepEqual(report.resources.teamRevive,[[4,0],[4,0],[1,0]]);assert.deepEqual(report.resources.rescue,[[2,2,0],[3,3,0],[1,1,0]]);
 assert.deepEqual(report.resources.allGhostMenu,0);assert.deepEqual(report.resources.allGhostRestart,[3,3,0,0]);
 assert.deepEqual(report.resources.challenge,[3,3,1]);
 report.cases=[];
 for(const count of [2,3])for(let stage=0;stage<7;stage++)for(const warp of [0,2]){
  const result=await page.evaluate(({count,stage,warp})=>{
   const modules=[...document.querySelectorAll('iframe')].slice(0,count).map(f=>f.contentWindow.Module);
   for(let seat=0;seat<count;seat++){
    const m=modules[seat];if(!m._th05_coop_start(count,seat,stage===6?4:1,739,stage%4,(stage+1)%4,(stage+2)%4)||!m._th05_native_coop_stage(stage,warp))throw Error('stage start');
    m._th05_coop_display(seat,seat===0?1:seat===1?2:0);
    for(let i=0;i<count;i++){m._th05_native_coop_probe(3,i,128);m._th05_native_coop_probe(6,i,10000);}
   }
   let checks=0;
   for(let tick=0;tick<480;tick++){
    for(const m of modules){
     for(let seat=0;seat<count;seat++)m._th05_coop_input(seat,32|(tick%160<80?(seat%2?4:8):(seat%2?8:4)),seat===1&&tick%43===0?5:0,0,1);
     if(!m._th05_tick(0))throw Error('native frame failed');
    }
    if(tick%60===0){const hashes=modules.map(m=>m._th05_coop_checksum()>>>0);if(hashes.some(h=>h!==hashes[0]))throw Error('state mismatch '+JSON.stringify({count,stage,warp,tick,hashes}));checks++;}
   }
   for(const m of modules){m._th05_coop_draw();m._th05_present();}
   return {count,stage,warp,frames:480,checks,hash:modules[0]._th05_coop_checksum()>>>0,players:Array.from({length:count},(_,seat)=>Array.from({length:7},(_,field)=>modules[0]._th05_coop_value(seat,field)))};
  },{count,stage,warp});report.cases.push(result);console.log(`Native ${count}P stage ${stage+1} warp ${warp}: ${result.hash}`);
 }
 await page.screenshot({path:resolve(out,'native-hud-styles.png')});
 report.ending=await page.evaluate(()=>{
  const modules=[...document.querySelectorAll('iframe')].map(f=>f.contentWindow.Module),scenes=[];
  for(const m of modules){m._th05_native_coop_stage(5,0);m._th05_native_coop_probe(12,0,0);m._th05_tick(0);}
  for(let n=0;n<20000;n++){
   const values=modules.map(m=>m._th05_scene());if(values.some(v=>v!==values[0]))throw Error('Ending scenes differ');if(!scenes.includes(values[0]))scenes.push(values[0]);
   if(modules.every(m=>m._th05_coop_finished()))return {frames:n,scenes};
   for(const m of modules){for(let seat=0;seat<3;seat++)m._th05_coop_input(seat,n%2?0:32,0,0,0);m._th05_tick(0);}
  }
  throw Error('Cooperative ending did not finish: '+JSON.stringify(scenes));
 });
 assert.ok(report.ending.scenes.includes(5));assert.ok(report.ending.scenes.includes(9));
 await page.screenshot({path:resolve(out,'native-three-player.png')});assert.deepEqual(errors,[]);report.errors=errors;
 report.yumeko=await page.evaluate(()=>{
  const m=document.querySelector('iframe').contentWindow.Module;
  if(!m._th05_coop_start(3,0,3,741,0,1,2)||!m._th05_native_coop_stage(4,2))throw Error('Yumeko stage start');
  return m._th05_native_coop_probe(34,0,0);
 });
 assert.equal(report.yumeko,1);
 report.bossPatterns=await page.evaluate(()=>{
  const m=document.querySelector('iframe').contentWindow.Module;
  return [0,1,3,5,6].map(stage=>{
   if(!m._th05_coop_start(3,0,stage===6?4:3,741,0,1,2)||!m._th05_native_coop_stage(stage,2))throw Error('Boss pattern stage start');
   return m._th05_native_coop_probe(35,0,0);
  });
 });
 assert.deepEqual(report.bossPatterns,[1,1,1,1,1]);
 report.stage3Fairy=await page.evaluate(()=>{
  const m=document.querySelector('iframe').contentWindow.Module;
  if(!m._th05_coop_start(3,0,3,741,0,1,2)||!m._th05_native_coop_stage(2,0))throw Error('Stage 3 fairy fixture start');
  return m._th05_native_coop_probe(36,0,0);
 });
 assert.equal(report.stage3Fairy,1);
 report.enemyWaves=await page.evaluate(()=>{
  const m=document.querySelector('iframe').contentWindow.Module;
  return [0,1,2,3,4,5,6].map(stage=>{
   if(!m._th05_coop_start(3,0,stage===6?4:3,741,0,1,2)||!m._th05_native_coop_stage(stage,0))throw Error('Enemy wave fixture start');
   return m._th05_native_coop_probe(37,0,0);
  });
 });
 assert.deepEqual(report.enemyWaves,[204,283,229,352,229,84,291]);
 await writeFile(resolve(out,'native.json'),JSON.stringify(report,null,2)+'\n');console.log('PASS native shared-world resources, rescue and '+report.cases.length+' synchronized stage cases');
}catch(error){console.error(report,errors);await page.screenshot({path:resolve(out,'native-failure.png')});throw error;}
finally{await browser.close();await site.close();}
