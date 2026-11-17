import assert from 'node:assert/strict';
import {mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {multiplayerFixture} from './multiplayer-fixture.mjs';
import {launchBrowser} from '../../../tools/browser/launch.mjs';

const out=resolve(process.env.TH05_TEST_OUTPUT??resolve(import.meta.dirname,'../artifacts/player-pixels'));
await mkdir(out,{recursive:true});
const site=await multiplayerFixture(1),browser=await launchBrowser(),page=await browser.newPage(),errors=[],report={characters:[],errors};
page.on('pageerror',e=>errors.push(e.message));
try{
 await page.goto(site.url);await page.waitForFunction(()=>events.some(e=>e.event==='ready'));
 for(const [command,payload] of [['configure',{music:'none',language:'ja',sharedResources:[{path:'/data/gothic.ttf',url:'/font'}],options:{}}],['prepare-room',{}]]){
  const response=await page.evaluate(({command,payload})=>window.command(0,command,payload),{command,payload});assert.equal(response.ok,true);
 }
 for(let character=0;character<4;character++){
  const result=await page.evaluate(character=>{
   const frame=document.querySelector('iframe'),m=frame.contentWindow.Module,gl=frame.contentDocument.querySelector('canvas').getContext('webgl');
   const check=(v,label)=>{if(!v)throw Error(label);};
   const pixels=(x,y,w,h)=>{const p=new Uint8Array(w*h*4);gl.readPixels(x,480-y-h,w,h,gl.RGBA,gl.UNSIGNED_BYTE,p);return p;};
   const hash=p=>{let h=2166136261;for(let i=0;i<p.length;i++)if(i%4!==3)h=Math.imul(h^p[i],16777619);return h>>>0;};
   check(m._th05_coop_start(3,0,1,741,character,character,character,0,0),'start');check(m._th05_native_coop_stage(0,0),'stage');
   for(let n=0;n<80;n++){for(let s=0;s<3;s++)m._th05_coop_input(s,0,0,0,0);m._th05_tick(0);}
   for(let s=0;s<3;s++)m._th05_native_coop_probe(14,s,0);
   const points=[];
   for(const [x,y] of [[192*16,320*16],[192*16+7,320*16+15]]){
    m._th05_native_coop_probe(4,0,x);m._th05_native_coop_probe(5,0,y);m._th05_options(1,0);check(m._th05_native_coop_probe(27,0,0),'hitbox draw');
    const p=pixels(0,0,640,480),lit=[];for(let n=0;n<640*480;n++)if(p[n*4]||p[n*4+1]||p[n*4+2])lit.push([n%640,479-Math.floor(n/640),...p.slice(n*4,n*4+3)]);
    lit.sort((a,b)=>a[1]-b[1]||a[0]-b[0]);points.push(lit);
   }
   m._th05_options(0,0);m._th05_native_coop_probe(27,0,0);const hidden=hash(pixels(0,0,640,480));
   const shots=[],markers=[];
   for(let seat=0;seat<3;seat++){
    m._th05_native_coop_probe(4,seat,192*16);m._th05_native_coop_probe(5,seat,320*16);
    for(let image=0;image<16;image++){
     const expected=m._th05_native_coop_probe(30,seat,3+image)>>>0;check(m._th05_native_coop_probe(28,seat,image),'shot draw');
     shots.push({seat,image,expected,actual:hash(pixels(224-8,376-8,16,16))});
    }
   }
   for(const numeric of [0,1]){
    check(m._th05_native_coop_probe(29,0,numeric),'sidebar draw');
    for(let seat=0;seat<3;seat++)markers.push({numeric,seat,expected:m._th05_native_coop_probe(30,seat,3)>>>0,actual:hash(pixels(numeric?472:464,168+seat*32,16,16))});
   }
   return {points,hidden,shots,markers};
  },character);
  for(const point of result.points)assert.deepEqual(point,[[223,375,255,255,255],[224,375,255,255,255],[223,376,255,255,255],[224,376,255,255,255]],`character ${character} hitbox`);
  let black=2166136261;for(let n=0;n<640*480*3;n++)black=Math.imul(black,16777619);assert.equal(result.hidden,black>>>0);
  for(const shot of result.shots)assert.equal(shot.actual,shot.expected,JSON.stringify({character,shot}));
  for(const marker of result.markers)assert.equal(marker.actual,marker.expected,JSON.stringify({character,marker}));
  report.characters.push({character,...result});
 }
 assert.deepEqual(errors,[]);report.passed=true;await writeFile(resolve(out,'report.json'),JSON.stringify(report,null,2)+'\n');
 console.log('PASS centered 2x2 hitboxes, 192 rendered shot frames and 24 sidebar markers');
}finally{await browser.close();await site.close();}
