import assert from 'node:assert/strict';
import {mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {multiplayerFixture} from './multiplayer-fixture.mjs';
import {launchBrowser} from '../../../tools/browser/launch.mjs';
const out=resolve(process.env.TH05_TEST_OUTPUT??resolve(import.meta.dirname,'../artifacts/team-bomb'));
await mkdir(out,{recursive:true});
const site=await multiplayerFixture(1),browser=await launchBrowser(),page=await browser.newPage(),errors=[],report={bombs:[],layers:[],errors};
page.on('pageerror',e=>errors.push(e.message));
try{
 await page.goto(site.url);await page.waitForFunction(()=>events.some(e=>e.event==='ready'));
 for(const [command,payload] of [['configure',{music:'none',language:'ja',sharedResources:[{path:'/data/gothic.ttf',url:'/font'}],options:{}}],['prepare-room',{}]]){
  const result=await page.evaluate(({command,payload})=>window.command(0,command,payload),{command,payload});assert.equal(result.ok,true);
 }
 for(const count of [2,3])for(let character=0;character<4;character++)for(let owner=0;owner<count;owner++){
  const result=await page.evaluate(({count,character,owner})=>{
   const m=document.querySelector('iframe').contentWindow.Module,check=(v,s)=>{if(!v)throw Error(s);};
   const p=(op,seat,value=0)=>m._th05_native_coop_probe(op,seat,value),field=(seat,n)=>p(31,seat,n),stock=(seat,n)=>m._th05_coop_value(seat,n);
   const chars=Array.from({length:3},(_,i)=>(character+i-owner+4)%4);
   const start=challenge=>{check(m._th05_coop_start(count,0,1,741,...chars,challenge,0),'start');check(m._th05_native_coop_stage(0,0),'stage');for(let seat=0;seat<count;seat++){p(14,seat);p(6,seat,0);p(7,seat,3);}};
   const tick=()=>{for(let seat=0;seat<count;seat++)m._th05_coop_input(seat,0,0,0,0);check(m._th05_tick(0),'tick');};
   start(0);const keeper=(owner+1)%count;p(6,keeper,999);p(7,owner,0);p(10,owner);
   check(!field(owner,4),'empty Bomb must not activate');for(let seat=0;seat<count;seat++)check(field(seat,0)===(seat===keeper?999:0),'empty Bomb must not protect');
   p(7,owner,3);const duration=field(owner,3);p(10,owner);check(field(owner,4)&&field(owner,5)===owner,'real caster');
   for(let seat=0;seat<count;seat++){
    check(stock(seat,1)===(seat===owner?2:3),'only caster spends Bomb');check(field(seat,0)===(seat===keeper?999:duration),'team protection and longer respawn immunity');
    p(8,seat);check(!field(seat,2),'protected collision');
   }
   for(let f=0;f<duration-1;f++)tick();
   for(let seat=0;seat<count;seat++){check(field(seat,0)===(seat===keeper?1000-duration:1),'protection countdown');p(8,seat);check(!field(seat,2),'last protected frame');}
   tick();check(field(owner,0)===0,'protection expires');p(8,owner);check(field(owner,2),'collision works after protection');tick();check(field(owner,1)>0,'normal death after expiration');
   start(0);const ghost=(owner+1)%count;p(1,ghost);p(10,owner);check(stock(ghost,3)===1&&stock(ghost,0)===0&&field(ghost,0)===0,'Bomb must not revive or protect ghost');
   start(1);p(10,owner);check(!field(owner,4)&&stock(owner,1)===3,'challenge rejects Bomb');for(let seat=0;seat<count;seat++)check(field(seat,0)===0,'challenge has no team shield');
   return {count,character,owner,duration,keeper,ghost,passed:true};
  },{count,character,owner});report.bombs.push(result);
 }
 for(let character=0;character<4;character++)for(let local=0;local<3;local++){
  const result=await page.evaluate(({character,local})=>{
   const frame=document.querySelector('iframe'),m=frame.contentWindow.Module,gl=frame.contentDocument.querySelector('canvas').getContext('webgl');
   m._th05_coop_start(3,local,1,741,character,character,character,0,0);m._th05_native_coop_stage(0,0);
   for(let n=0;n<80;n++){for(let seat=0;seat<3;seat++)m._th05_coop_input(seat,0,0,0,0);m._th05_tick(0);}
   m._th05_options(0,0);for(let seat=0;seat<3;seat++)m._th05_native_coop_probe(14,seat,0);
   const read=seat=>{const p=new Uint8Array(16*16*4),x=32+m._th05_coop_value(seat,5)/16,y=56+m._th05_coop_value(seat,6)/16;gl.readPixels(x-8,480-y-8,16,16,gl.RGBA,gl.UNSIGNED_BYTE,p);return Array.from(p);};
   const overlaps=[];
   for(let seat=0;seat<3;seat++){
    m._th05_native_coop_probe(32,seat,0);m._th05_coop_draw();m._th05_present();const actual=read(seat);
    m._th05_native_coop_probe(33,seat,0);const bulletOnTop=read(seat);
    m._th05_native_coop_probe(33,seat,1);const bodyOnTop=read(seat);
    overlaps.push({seat,correct:actual.every((v,i)=>v===bulletOnTop[i]),sensitive:actual.some((v,i)=>v!==bodyOnTop[i])});
   }
   return {character,local,overlaps};
  },{character,local});
  for(const overlap of result.overlaps){assert.ok(overlap.correct,JSON.stringify(result));assert.ok(overlap.sensitive,JSON.stringify(result));}report.layers.push(result);
 }
 assert.deepEqual(errors,[]);report.passed=true;await writeFile(resolve(out,'report.json'),JSON.stringify(report,null,2)+'\n');
 console.log('PASS 20 team Bomb cases and 36 real enemy-bullet/player overlaps');
}finally{await browser.close();await site.close();}
