import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,string,report,enableNativeMath} from './helpers.mjs';
test('TH17 every item award matches original scoring, power, life/bomb fragments and ordered services',async()=>{
 const c=await core(),m=await oracle(),owner=m.allocate(0x19100),pool=m.allocate(0xe4b988),item=pool+0x14,hud=m.allocate(0x200),bomb=m.allocate(0x60),header=m.allocate(32),sentinel=m.allocate(16);enableNativeMath(m);
 for(const [at,p]of [[0x4b77d0,owner],[0x4b76b8,pool],[0x4b76ac,hud],[0x4b7688,bomb]])m.u32(at,p);m.u32(owner+0x1903c,header);m.f32(header+8,5);m.f32(0x4b5918,0);m.i32(owner+0x18db0,1);m.u32(item,sentinel);
 for(let k=0;k<6;k++)m.f32(owner+0x190a4+k*4,k<3?-10000:10000);
 let events=[],cases=0;const sp=()=>m.reg('ESP'),event=(kind,a=0,b=0,p=0)=>events.push(kind,a>>>0,b>>>0,p?m.u32(p):0,p?m.u32(p+4):0,p?m.u32(p+8):0);
 m.replace(0x449630,'observe collection option rebuild',()=>{event(0);return 0;});m.replace(0x42f8a0,'observe collection notification',()=>{assert.equal(m.i32(sp()+4),0);event(1,m.i32(sp()+8));return 0;},2);
 m.replace(0x4654f0,'observe collection immediate audio',()=>{event(2,m.i32(sp()+4));return 0;},1);m.replace(0x465450,'observe collection queued audio',()=>{event(3,m.i32(sp()+4));return 0;},2);
 m.replace(0x450b70,'observe collection score popup',()=>{event(4,m.i32(sp()+8),m.i32(sp()+12),m.u32(sp()+4));return 0;},3);
 for(const [at,id]of [[0x42fc60,5],[0x42fd50,6]])m.replace(at,'observe collection HUD '+id,()=>{event(id,m.i32(sp()+4),m.i32(sp()+8));return 0;},2);
 const po=[[0,0x4b5a40],[1,0x4b5a44],[6,0x4b5a30],[7,0x4b5a38],[8,0x4b5a4c],[9,0x4b5a50]],so=[0x4b59fc,0x4b5a24,0x4b5a2c,0x4b5a20,0x4b5a48,0x4b5a00,0x4b5a34];
 try{for(let character=0;character<3;character++){const f=c.item_collection_create(character);try{m.i32(0x4b59f4,character);
  for(let kind=1;kind<=14;kind++)for(let sample=0;sample<80;sample++){
   const pp=c.item_collection_field(f,0),ss=c.item_collection_field(f,1),pos=c.item_collection_field(f,2),ip=c.item_collection_field(f,3),dv=new DataView(c.memory.buffer);
   const pv=[[0,[-1,0,2,7,8,9][sample%6]],[1,[0,1,2,3,4,5,10][sample%7]],[6,[0,99,100,199,299,399,400,401][sample%8]],[7,100],[8,[0,2,7,8,9][sample%5]],[9,[0,1,2,3,4][sample%5]]];
   for(const [i,value]of pv){dv.setInt32(pp+i*4,value,true);m.i32(po.find(x=>x[0]===i)[1],value);}
   const sv=[[0,1771,999999998,999999999][sample%4],[0,999,1000,1000000,1000450,1000999,9999000,9999999][sample%8],10000000,sample,sample%10,sample%3?0:4,400];
   sv.forEach((value,i)=>{dv.setInt32(ss+i*4,value,true);m.i32(so[i],value);});
   const p=[Math.fround((sample%9-4)*17.125),Math.fround([-16,127.999,128,128.001,147.999,148,148.001,320.75,449.875,472.25][sample%10]),Math.fround(sample%3*.25)],it=[Math.fround(sample*1.25),Math.fround(300-sample*.125),Math.fround(sample?-sample*.0625:0)];
   for(let k=0;k<3;k++){dv.setFloat32(pos+k*4,p[k],true);m.f32(owner+0x610+k*4,p[k]);}memory(c,ip,104).fill(0);m.view(item+0xc10,104).fill(0);
   for(let k=0;k<3;k++){dv.setFloat32(ip+k*4,it[k],true);m.f32(item+0xc10+k*4,it[k]);}dv.setInt32(ip+72,sample%2?3:1,true);m.i32(item+0xc58,sample%2?3:1);m.i32(item+0xc5c,kind);m.view(sentinel,16).fill(0);events=[];
   assert.equal(m.call(0x433410,{ecx:pool}),1);assert.deepEqual(events.slice(-6),[2,37,0,0,0,0]);events.length-=6;
   assert.equal(c.item_collection_award(f,kind),1,string(c,c.item_collection_error(f)));const view=new DataView(c.memory.buffer),label=character+'/'+kind+'/'+sample;
   for(const [i,at]of po)assert.equal(view.getInt32(pp+i*4,true),m.i32(at),label+' player '+i);so.forEach((at,i)=>assert.equal(view.getInt32(ss+i*4,true),m.i32(at),label+' score '+i));
   assert.deepEqual(Array.from(new Uint32Array(c.memory.buffer,c.item_collection_events(f),c.item_collection_event_count(f))),events,label+' ordered services');cases++;
  }
 }finally{c.item_collection_delete(f);}}
 report('item-collection-awards',{passed:true,cases,characters:3,kinds:14,originalFunctions:['0x433410','0x434200','0x434400','0x443700'],scope:'All original award branches reached through unchanged native full item update; 3 characters, power/score caps, height thresholds, Extra life-fragment tier table, three-fragment bomb extension, all six cancellation-point types and duplicate popup/HUD behavior. HUD/audio/option rebuild captured at service boundaries. Full movement/animation and animal spirit lifecycle are separate.'});
 }finally{m.close();}
});
