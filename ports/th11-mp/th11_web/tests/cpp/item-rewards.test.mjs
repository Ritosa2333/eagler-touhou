import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';import {animationOracle} from './anm-oracle.mjs';
test('TH11 item pickups preserve native score, power, rank, lives and notification order',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m),{source,original}=o.load('bullet.anm');
 const data=c.allocate(source.length);memory(c,data,source.length).set(source);const f=c.im_create();assert.equal(c.im_load(f,data,source.length),1);
 const nm=m.allocate(0x265e70),player=m.allocate(0x8c40),sht=m.allocate(32),hud=m.allocate(0x4460),bm=m.allocate(0x46d700);
 m.view(nm,0x265e70).fill(0);m.view(player,0x8c40).fill(0);m.view(hud,0x4460).fill(0);
 m.u32(0x4a8e90,nm);m.u32(0x4a8eb4,player);m.u32(0x4a8d84,hud);m.u32(0x4a8d68,bm);m.u32(bm+0x46d674,original);m.u32(player+0x92c,sht);
 const map=[0x4a56e4,0x4a56e8,0x4a56f0,0x4a56f4,0x4a5718,0x4a571c,0x4a5720,0x4a5744,0x4a5748,0x4a574c,0x4a5754,0x4a576c];
 const economy=c.im_economy(f),cp=c.im_data(f,0),ci=c.im_item(f,0),ni=nm+0x14;
 const put=(a,v)=>new DataView(c.memory.buffer).setInt32(a,v,true),flt=(a,v)=>new DataView(c.memory.buffer).setFloat32(a,v,true);
 let events=[],checks=0;
 m.replace(0x44a1e0,'life sound sink',()=>{events.push([0,m.reg('ESI'),0,0,0]);return 0;});
 m.replace(0x44a260,'item sound sink',()=>{events.push([0,m.reg('EDI'),m.f32(m.reg('ESP')+4),0,1]);return 0;},1);
 m.replace(0x438440,'item score popup sink',()=>{const sp=m.reg('ESP'),p=m.u32(sp+4);events.push([2,m.reg('EAX')|0,m.f32(p),m.f32(p+4),m.u32(sp+8)]);return 0;},2);
 m.replace(0x41f010,'item HUD notification sink',()=>{events.push([4,m.reg('EAX'),0,0,0]);return 0;},1);
 m.replace(0x432cc0,'player options rebuild sink',()=>{events.push([5,0,0,0,0]);return 0;});
 m.replace(0x41a060,'life HUD update sink',()=>{events.push([6,m.reg('EDX')|0,(m.u32(m.reg('ESP')+4)<<16)>>16,0,0]);return 0;},1);
 const states=[];
 for(let k=0;k<80;++k)states.push([
  [0,999999998,999999999][k%3],[0,1,49,99,100,199,299,399,400,401][k%10],
  [0,1000,3999999,99999000,199999900][k%5],[0,1,99,9999,10000,20000][k%6],
  [0,2,8,9,10][k%5],[0,1,4,5][k%4],k%7-1,[-1100,-1024,0,1000,1024,1100][k%6],
  400,[50,100][k%2],[0,99,100,89899,89900,99999][k%6],100000000]);
 try{for(const [s,state]of states.entries())for(let type=0;type<=12;++type){
  for(let i=0;i<map.length;++i){put(economy+i*4,state[i]);m.i32(map[i],state[i]);}
  put(cp+76,state[1]);put(cp+80,state[8]);put(cp+12,1);m.i32(player+0x928,1);
  for(const [i,v]of [-100000,-100000,100000,100000].entries()){flt(cp+20+i*4,v);m.f32(player+0x8bcc+(i>=2?i*4+4:i*4),v);}
  memory(c,ci,0x478).fill(0);m.view(ni,0x478).fill(0);
  for(const [off,v]of [[0x464,99],[0x468,type]]){put(ci+off,v);m.i32(ni+off,v);}
  for(const [off,v]of [[0x434,-37.5],[0x438,302.25]]){flt(ci+off,v);m.f32(ni+off,v);}
  events=[];m.call(0x423550,{args:[nm],limit:1000000});assert.equal(c.im_update(f),1,`case ${s} type ${type}`);
  const actual=Array.from({length:map.length},(_,i)=>new DataView(c.memory.buffer).getInt32(economy+i*4,true));
  assert.deepEqual(actual,map.map(a=>m.i32(a)),`state ${s} type ${type}`);
  const eventBase=c.im_data(f,4),view=new DataView(c.memory.buffer),actualEvents=[];
  for(let i=0;i<c.im_value(f,4);++i){const p=eventBase+i*20;actualEvents.push([view.getInt32(p,true),view.getInt32(p+4,true),view.getFloat32(p+8,true),view.getFloat32(p+12,true),view.getUint32(p+16,true)]);}
  assert.deepEqual(actualEvents,events,`event order ${s} type ${type}`);assert.equal(view.getInt32(ci+0x464,true),0);++checks;
 }
 report('item-rewards',{passed:true,checks,scope:'Native item update and original score, power, point-value and life helpers; compared counters and ordered effects at limits. Audio, popup/HUD and player-option rendering are recorded interfaces.'});
 }finally{c.im_delete(f);c.release(data);m.close();}
});
