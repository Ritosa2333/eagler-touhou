import assert from 'node:assert/strict';
import {mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {multiplayerFixture} from './multiplayer-fixture.mjs';
import {launchBrowser} from '../../../tools/browser/launch.mjs';
assert.ok(process.env.TH05_BASELINE_RUNTIME,'Provide the saved pre-optimization diagnostic Runtime');
const out=resolve(process.env.TH05_TEST_OUTPUT??resolve(import.meta.dirname,'../artifacts/performance-equivalence'));await mkdir(out,{recursive:true});
const current=process.env.TH05_TEST_RUNTIME;process.env.TH05_TEST_RUNTIME=process.env.TH05_BASELINE_RUNTIME;const before=await multiplayerFixture(1);
if(current)process.env.TH05_TEST_RUNTIME=current;else delete process.env.TH05_TEST_RUNTIME;
const after=await multiplayerFixture(1),browser=await launchBrowser(),pages=[],errors=[],report={cases:[],errors};
try{
 for(const site of [before,after]){const page=await browser.newPage();pages.push(page);page.on('pageerror',e=>errors.push(e.message));await page.goto(site.url);await page.waitForFunction(()=>events.some(e=>e.event==='ready'));
  for(const [command,payload] of [['configure',{music:'none',language:'ja',sharedResources:[{path:'/data/gothic.ttf',url:'/font'}],options:{}}],['prepare-room',{}]])assert.equal((await page.evaluate(({command,payload})=>window.command(0,command,payload),{command,payload})).ok,true);
 }
 for(const count of [2,3])for(let stage=0;stage<7;stage++)for(const warp of [0,2]){
  const results=[];
  for(const [version,page] of pages.entries()){
   const data=await page.evaluate(({count,stage,warp})=>{
    const m=document.querySelector('iframe').contentWindow.Module;
    if(!m._th05_coop_start_stock(count,0,stage===6?4:3,747,0,1,3,0,0,5,3)||!m._th05_native_coop_stage(stage,warp))throw Error('stage');
    for(let seat=0;seat<count;seat++){m._th05_native_coop_probe(3,seat,128);m._th05_native_coop_probe(6,seat,100000);}
    const checks=[];
    for(let f=0;f<720;f++){
     for(let seat=0;seat<count;seat++)m._th05_coop_input(seat,32|64|(f%60<30?4:8),0,0,0);
     if(!m._th05_tick(0))throw Error('tick');if(f%60===0)checks.push(m._th05_coop_checksum()>>>0);
    }
    m._th05_coop_display(0,32);m._th05_coop_draw();m._th05_present();return checks;
   },{count,stage,warp});results.push(data);
   await page.frameLocator('iframe').locator('canvas').screenshot({path:resolve(out,`${count}p-stage${stage+1}-warp${warp}-${version}.png`)});
   await page.evaluate(()=>{const m=document.querySelector('iframe').contentWindow.Module;m._th05_native_effect_probe(0);m._th05_coop_draw();m._th05_present();});
   await page.frameLocator('iframe').locator('canvas').screenshot({path:resolve(out,`${count}p-stage${stage+1}-warp${warp}-effect-${version}.png`)});
  }
  assert.deepEqual(results[1],results[0],JSON.stringify({count,stage,warp}));report.cases.push({count,stage:stage+1,warp,checks:results[0].length});console.log(`PASS ${count}P stage ${stage+1} warp ${warp}`);
 }
 assert.deepEqual(errors,[]);report.passed=true;await writeFile(resolve(out,'report.json'),JSON.stringify(report,null,2)+'\n');
}finally{await browser.close();await before.close();await after.close();}
