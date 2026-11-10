import assert from 'node:assert/strict';
import {multiplayerFixture} from './multiplayer-fixture.mjs';
import {launchBrowser} from '../../../tools/browser/launch.mjs';

const fixture=await multiplayerFixture(1),browser=await launchBrowser(),page=await browser.newPage();
const errors=[];
page.on('pageerror',error=>errors.push(error.message));
try{
 await page.goto(fixture.url);
 await page.waitForFunction(()=>events.some(event=>event.event==='ready'));
 for(const [command,payload] of [
  ['configure',{music:'none',language:'ja',sharedResources:[{path:'/data/gothic.ttf',url:'/font'}],options:{}}],
  ['prepare-room',{}],
 ])assert.equal((await page.evaluate(([name,payload])=>window.command(0,name,payload),[command,payload])).ok,true);
 const result=await page.evaluate(()=>{
  const m=document.querySelector('iframe').contentWindow.Module;
  const check=(ok,why)=>{if(!ok)throw Error(why);};
  const setup=(players,limit,character=0)=>{
   m._th05_net_close();
   check(m._th05_coop_start_stock(players,0,1,747,character,1,3,0,0,2,3),'start');
   for(let frame=0;frame<160;frame++)check(m._th05_tick(0),'stage intro');
   check(m._th05_net_reset(0,limit),'netplay reset');
  };
  const input=(seat,frame,buttons=0)=>check(m._th05_net_input(seat,frame,buttons,0,0,0),'input');
  const tick=()=>{const result=m._th05_net_advance();check(result>=0,'tick');return result;};
  const result=[];
  for(const players of [2,3]){
   const immediate=[];
   for(let character=0;character<4;character++){
    setup(players,8,character);
    const before=m._th05_coop_value(0,5),bombs=m._th05_coop_value(0,1);
    input(0,0,8|32);
    check(tick()===1&&m._th05_coop_value(0,5)>before,'local movement and fire must run on the capture frame');
    input(0,1,16);
    const bombTick=tick(),bombStock=m._th05_coop_value(0,1);
    immediate.push({character,advanced:bombTick,bombSpent:bombs-bombStock,confirmed:m._th05_net_confirmed()});
   }
   const frames=90,lag=5,change=38;
   const buttons=(seat,frame)=>seat===0?(frame>=10&&frame<28?8:0):frame<change?4:8;
   const remote=frame=>{for(let seat=1;seat<players;seat++)input(seat,frame,buttons(seat,frame));};
   setup(players,0);
   for(let frame=0;frame<frames;frame++){input(0,frame,buttons(0,frame));remote(frame);check(tick(),'exact tick');}
   const oracle=m._th05_coop_checksum()>>>0;
   setup(players,8);
   // Seed the first held direction, then keep each peer exactly five frames
   // behind. The only necessary correction is the actual direction change.
   let head=0,stalls=0;
   for(let clock=0;clock<frames+90&&(head<frames||m._th05_net_confirmed()<frames-1);clock++){
    if(clock===0)remote(0);
    else if(clock>lag&&clock-lag<frames)remote(clock-lag);
    if(head<frames){input(0,head,buttons(0,head));if(tick())head++;else stalls++;}
    else tick();
   }
   result.push({players,immediate,stalls,corrections:m._th05_net_stat(0),head,confirmed:m._th05_net_confirmed(),matchesOracle:(m._th05_coop_checksum()>>>0)===oracle});
  }
  m._th05_net_close();
  return result;
 });
 console.log(JSON.stringify(result));
 assert.deepEqual(errors,[]);
 for(const {immediate,corrections,head,confirmed,matchesOracle} of result){
  assert.ok(immediate.every(row=>row.advanced===1&&row.bombSpent===1&&row.confirmed===-1),'Bomb works on the local frame before any remote confirmation');
  assert.equal(corrections,1,'A steady held direction needs no repeated corrections');
  assert.equal(head,90);
  assert.equal(confirmed,89);
  assert.equal(matchesOracle,true);
 }
}catch(error){if(errors.length)console.error(errors);throw error;}
finally{await browser.close();await fixture.close();}
