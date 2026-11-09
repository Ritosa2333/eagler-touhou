import assert from 'node:assert/strict';
import {mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {multiplayerFixture} from './multiplayer-fixture.mjs';
import {launchBrowser} from '../../../tools/browser/launch.mjs';
const out=resolve(process.env.TH05_TEST_OUTPUT??resolve(import.meta.dirname,'../artifacts/initial-stock'));await mkdir(out,{recursive:true});
const site=await multiplayerFixture(1),browser=await launchBrowser(),page=await browser.newPage(),errors=[],report={cases:[],screens:[]};
page.on('pageerror',e=>errors.push(e.message));
try{
 await page.goto(site.url);await page.waitForFunction(()=>events.some(e=>e.event==='ready'));
 for(const [command,payload] of [['configure',{music:'none',language:'ja',sharedResources:[{path:'/data/gothic.ttf',url:'/font'}],options:{}}],['prepare-room',{}]])assert.equal((await page.evaluate(({command,payload})=>window.command(0,command,payload),{command,payload})).ok,true);
 for(const count of [2,3])for(const difficulty of [1,4])for(const [spares,bombs] of [[0,0],[2,3],[5,3],[5,0]]){
  const result=await page.evaluate(({count,difficulty,spares,bombs})=>{
   const m=document.querySelector('iframe').contentWindow.Module,stock=()=>Array.from({length:count},(_,seat)=>[m._th05_coop_value(seat,0),m._th05_coop_value(seat,1)]);
   if(!m._th05_coop_start_stock(count,0,difficulty,741,0,1,1,0,0,spares,bombs))throw Error('stock start');
   const initial=stock();m._th05_native_coop_stage(difficulty===4?6:0,0);
   for(let seat=0;seat<count;seat++)m._th05_native_coop_probe(1,seat,0);m._th05_coop_draw();
   for(let tick=0;tick<60;tick++){for(let seat=0;seat<count;seat++)m._th05_coop_input(seat,seat===0&&tick===0?32:0,0,0,0);if(!m._th05_tick(0))throw Error('restart tick');}
   return {count,difficulty,spares,bombs,initial,restarted:stock()};
  },{count,difficulty,spares,bombs});
  const expected=Array.from({length:count},()=>[spares+1,bombs]);assert.deepEqual(result.initial,expected);assert.deepEqual(result.restarted,expected);report.cases.push(result);
 }
 for(const characters of [[0,1,1],[0,0,0],[1,1,1],[2,2,2],[3,3,3]]){
  const draw=await page.evaluate(characters=>{
   const m=document.querySelector('iframe').contentWindow.Module;m._th05_coop_start_stock(3,0,1,741,...characters,0,0,5,3);m._th05_native_coop_stage(0,0);
   for(let tick=0;tick<80;tick++)m._th05_tick(0);
   for(let seat=0;seat<3;seat++)m._th05_native_coop_probe(3,seat,[1,64,128][seat]);
   const before=m._th05_coop_checksum();m._th05_coop_display(0,32);m._th05_coop_draw();m._th05_present();
   return {before,after:m._th05_coop_checksum(),variants:[0,1,2].map(seat=>m._th05_coop_value(seat,11))};
  },characters);
  assert.equal(draw.after,draw.before,'Display settings must preserve the game state');
  const name='numeric-'+characters.join('-')+'.png';await page.frameLocator('iframe').locator('canvas').screenshot({path:resolve(out,name)});report.screens.push({name,characters,...draw});
 }
 await page.evaluate(()=>{const m=document.querySelector('iframe').contentWindow.Module;m._th05_coop_display(0,0);m._th05_coop_draw();m._th05_present();});
 await page.frameLocator('iframe').locator('canvas').screenshot({path:resolve(out,'gauge.png')});
 assert.deepEqual(errors,[]);report.passed=true;report.errors=errors;await writeFile(resolve(out,'report.json'),JSON.stringify(report,null,2)+'\n');console.log('PASS 16 native stock/restart cases, Extra, five numeric HUD captures and gauge toggle');
}finally{await browser.close();await site.close();}
