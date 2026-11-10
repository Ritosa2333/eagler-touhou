import assert from 'node:assert/strict';
import {readFile,writeFile,mkdir} from 'node:fs/promises';
import {resolve} from 'node:path';
import {multiplayerFixture} from './multiplayer-fixture.mjs';
import {launchBrowser} from '../../../tools/browser/launch.mjs';

const source=JSON.parse(await readFile(new URL('../resources/player-art/source.json',import.meta.url),'utf8'));
const out=resolve(import.meta.dirname,'../artifacts/player-art');await mkdir(out,{recursive:true});
const site=await multiplayerFixture(3),browser=await launchBrowser(),page=await browser.newPage({viewport:{width:1320,height:1020}}),errors=[],report={characters:[],errors};
page.on('pageerror',error=>errors.push(error.message));
try{
 await page.goto(site.url);await page.waitForFunction(()=>events.filter(e=>e.event==='ready').length===3);
 for(let seat=0;seat<3;seat++)for(const [command,payload] of [['configure',{music:'none',language:'ja',sharedResources:[{path:'/data/gothic.ttf',url:'/font'}],options:{}}],['prepare-room',{}]]){
  const response=await page.evaluate(({seat,command,payload})=>window.command(seat,command,payload),{seat,command,payload});assert.equal(response.ok,true,JSON.stringify(response));
 }
 for(const [character,name] of ['reimu','marisa','mima','yuuka'].entries()){
  const result=await page.evaluate(character=>{
   const modules=[...document.querySelectorAll('iframe')].map(frame=>frame.contentWindow.Module);
   for(let seat=0;seat<3;seat++){
    const m=modules[seat];m._th05_coop_start(3,seat,1,741,character,character,character,0,0);m._th05_native_coop_stage(0,0);
    for(let n=0;n<80;n++){for(let i=0;i<3;i++)m._th05_coop_input(i,0,0,0,0);m._th05_tick(0);}
    for(let i=0;i<3;i++)m._th05_native_coop_probe(14,i,0);
   }
   const m=modules[0],textures=Array.from({length:3},(_,seat)=>Array.from({length:20},(_,frame)=>m._th05_native_coop_probe(13,seat,frame)>>>0));
   const variants=[0,1,2].map(seat=>m._th05_coop_value(seat,11));
   const alpha=[];
   for(const distance of [0,50,60,75,100,150]){
    m._th05_native_coop_probe(4,0,192*16);m._th05_native_coop_probe(4,1,(192+distance)*16);
    alpha.push([m._th05_coop_value(0,12),m._th05_coop_value(1,12)]);
   }
   for(const client of modules){for(let i=0;i<3;i++){client._th05_native_coop_probe(4,i,(128+i*64)*16);client._th05_native_coop_probe(5,i,320*16);}client._th05_coop_draw();client._th05_present();}
   return {textures,variants,alpha};
  },character);
  assert.deepEqual(result.variants,[0,1,2]);
  assert.deepEqual(result.alpha,[[255,51],[255,51],[255,91],[255,128],[255,128],[255,128]]);
  for(let seat=0;seat<3;seat++)for(let frame=0;frame<20;frame++){
   const file=frame<3?`player-${String(frame).padStart(2,'0')}.png`:frame<19?`shots-${String(frame-3).padStart(2,'0')}.png`:'ghost-32.png';
   const path=`${name}/${source.characterVariants[name][seat]}/${file}`;
   assert.equal(result.textures[seat][frame],source.textureHashes[path],path);
  }
  await page.screenshot({path:resolve(out,`${name}-three-colors.png`)});
  await page.evaluate(()=>{
   for(const iframe of document.querySelectorAll('iframe')){
    const m=iframe.contentWindow.Module;for(let seat=0;seat<3;seat++){m._th05_native_coop_probe(4,seat,192*16);m._th05_native_coop_probe(5,seat,320*16);}
    m._th05_coop_draw();m._th05_present();
   }
  });
  for(let seat=0;seat<3;seat++)await page.frames()[seat+1].locator('canvas').screenshot({path:resolve(out,`${name}-overlap-p${seat+1}.png`)});
  const ghost=await page.evaluate(()=>{
   const m=document.querySelector('iframe').contentWindow.Module;m._th05_native_coop_probe(1,2,0);
   const position=()=>[m._th05_coop_value(2,5),m._th05_coop_value(2,6)],before=position();
   for(let n=0;n<120;n++){for(let i=0;i<3;i++)m._th05_coop_input(i,i===2?15:0,1000,1000,1);m._th05_tick(0);}
   m._th05_coop_draw();m._th05_present();return {before,after:position()};
  });
  assert.deepEqual(ghost.before,[4096,5120]);assert.deepEqual(ghost.after,ghost.before);
  report.characters.push({name,textures:60,variants:result.variants,alpha:result.alpha,ghost});
 }
 assert.deepEqual(errors,[]);report.passed=true;await writeFile(resolve(out,'report.json'),JSON.stringify(report,null,2)+'\n');console.log('PASS 240 reviewed live textures, proximity alpha and fixed ghosts');
}finally{await browser.close();await site.close();}
