import test from 'node:test';
import assert from 'node:assert/strict';
import {core,oracle,report} from './helpers.mjs';

test('TH11 displayed score and highest score follow original 41ef00',async()=>{
 const c=await core(),m=await oracle(),f=c.hud_create(),hud=m.allocate(0x4460);
 m.u32(0x4a8d84,hud);m.u32(0x4a5750,0);
 const ptr=c.hud_score(f),economy=c.hud_data(f,0),dv=()=>new DataView(c.memory.buffer);
 const set=(p,v)=>dv().setInt32(p,v,true),get=p=>dv().getInt32(p,true);
 let rng=0x128bc,checks=0;const rand=()=>rng=(Math.imul(rng,1664525)+1013904223)>>>0;
 try{
  assert.equal(m.i32(0x4a3b1c),1000000000);assert.equal(m.i32(0x4a3d60),1000000000);
  for(let sample=0;sample<100;++sample){
   const initial=rand()%999999999,high=rand()%999999999,cont=rand()%10,flags=rand()&0x3ff;
   for(const [off,value,address]of [[0,initial,hud+0x43e4],[4,0,hud+0x43e8],[8,high,0x4a56e0],[12,cont,0x4a573c],[16,1,0x4a5740],[20,flags,0x4a5758]]){set(ptr+off,value);m.i32(address,value);}
   m.i32(0x4a5720,sample%5);m.u32(hud+0x4420,sample&1?0x20:0);
   let current=initial;
   for(let frame=0;frame<200;++frame){
    if(frame%41===0)current=rand()%999999999;
    if(frame===160)current=999999999;
    if(frame===180)current=get(ptr)+1;
    set(economy,current);m.i32(0x4a56e4,current);
    c.hud_score_update(f);m.call(0x41ef00);
    for(const [off,address]of [[0,hud+0x43e4],[4,hud+0x43e8],[8,0x4a56e0],[12,0x4a573c],[16,0x4a5740],[20,0x4a5758]])assert.equal(get(ptr+off),m.i32(address),`sample ${sample} frame ${frame} field ${off}`);
    assert.equal(m.u32(0x4a5750),0,'no score extend below original cap');++checks;
   }
  }
  report('hud-score',{passed:true,checks,scope:'Original 41ef00 display interpolation, highest score/continue/flags, all difficulties and valid score range, both native extend sentinels exceed the gameplay cap.'});
 }finally{c.hud_delete(f);m.close();}
});
