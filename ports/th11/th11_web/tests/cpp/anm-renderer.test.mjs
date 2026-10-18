import test from 'node:test';
import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';

test('TH11 screen sprite submissions match original vertices and material states',async()=>{
 const c=await core(),m=await oracle(),fixture=c.render_create(),vm=c.allocate(0x434);
 const manager=m.allocate(0x7bd900),camera=m.allocate(0x100),device=m.allocate(4),vtable=m.allocate(0x180),nativeVm=m.allocate(0x434),sprite=m.allocate(64),texture=m.allocate(4),batch=m.allocate(1024*168);
 m.u32(0x4c3268,manager);m.u32(0x4c3288,device);m.u32(device,vtable);m.u32(0x4c37cc,camera);m.u32(camera+0xd4,640);m.u32(camera+0xd8,480);m.u32(sprite+8,texture);m.u32(texture,1);
 let draws=[],source=2,destination=1,filter=2,state=13579,checks=0;
 const random=()=>{state=(Math.imul(state,1664525)+1013904223)>>>0;return state/4294967296;};
 const arg=n=>m.u32(m.reg('ESP')+n*4);
 const hook=(offset,name,argc,handler)=>m.u32(vtable+offset,m.registerImport({dll:'render-oracle',name,argc,handler}));
 hook(0xe4,'state',3,()=>{if(arg(2)===19)source=arg(3);if(arg(2)===20)destination=arg(3);return 0;});
 hook(0x104,'texture',3,()=>0);hook(0x10c,'combiner',4,()=>0);hook(0x164,'layout',2,()=>0);
 hook(0x114,'sampler',4,()=>{if(arg(3)===6)filter=arg(4);return 0;});
 hook(0x14c,'draw',5,()=>{assert.equal(arg(2),4);assert.equal(arg(5),28);draws.push(Buffer.from(m.bytes(arg(4),arg(3)*3*arg(5))));return 0;});
 try{
  for(let mode=0;mode<4;++mode)for(let sample=0;sample<512;++sample){
   c.render_reset(fixture);draws=[];m.u32(manager+0x435620,0);m.u32(manager+0x7b5624,batch);m.u32(manager+0x7b5628,batch);m.u32(manager+0x4355bc,0);for(const off of [0x4355c0,0x4355c2,0x4355c6])m.view(manager+off,1)[0]=255;
   const x=(random()-.5)*16,y=(random()-.5)*16,tint=(random()*4294967296)>>>0,enabled=sample&1;
   c.render_configure(fixture,x,y,tint,enabled);m.f32(manager+0xb0,x);m.f32(manager+0xb4,y);m.u32(manager+0x7bd88c,tint);m.u32(manager+0x7bd890,enabled);
   const b=Buffer.alloc(0x434);b.writeFloatLE(sample%8===0?0:(random()-.5)*6.28,0x2c);
   b.writeFloatLE((random()-.5)*5,0x3c);b.writeFloatLE((random()-.5)*5,0x40);b.writeFloatLE(random()*128,0x4c);b.writeFloatLE(random()*128,0x50);
   for(const off of [0x54,0x58,0x70,0x74,0x78,0x7c,0x80,0x84,0x88,0x8c])b.writeFloatLE(random()*2-1,off);
   for(const off of [0x3dc,0x3e0,0x3e4,0x3f4,0x3f8,0x3fc])b.writeFloatLE((random()-.5)*12,off);
   b.writeFloatLE(random()*1000-180,0x3e8);b.writeFloatLE(random()*720-120,0x3ec);b.writeFloatLE(random(),0x3f0);
   b.writeUInt32LE((((sample%7)<<4)|(mode<<22)|((sample%3)<<18)|((Math.floor(sample/3)%3)<<20)|(sample&1?0x8000:0)|(sample&2?0x80000000:0)|3)>>>0,0x404);
   b.writeUInt32LE((random()*4294967296)>>>0,0x374);b.writeUInt32LE((random()*4294967296)>>>0,0x378);
   memory(c,vm,b.length).set(b);m.write(nativeVm,b);m.u32(nativeVm+0x3ac,sprite);
   for(let i=0;i<4;++i)m.f32(0x4c91d8+i*28+12,1);
   const result=c.render_draw(fixture,vm);m.reg('EAX',nativeVm);const expectedResult=m.call(0x451ef0,{ecx:manager});assert.equal(result|0,expectedResult|0);
   c.render_flush(fixture);m.reg('ESI',manager);m.call(0x44fd10);
   const actual=Buffer.from(memory(c,c.render_data(fixture),c.render_size(fixture))),expected=Buffer.concat(draws),context=`mode=${mode} sample=${sample}`;
   assert.equal(actual.length,expected.length,context+' vertex count');
   for(let i=0;i<actual.length;i+=4)assert.equal(actual.readUInt32LE(i),expected.readUInt32LE(i),`${context} vertex=${Math.floor(i/28)} word=${i%28/4} ${actual.readFloatLE(i)} vs ${expected.readFloatLE(i)}`);
   if(expected.length){assert.equal(c.render_state(fixture,0)+1,source,context+' source blend');assert.equal(c.render_state(fixture,1)+1,destination,context+' destination blend');assert.equal(c.render_state(fixture,2)+1,filter,context+' filter');}
   ++checks;
  }
  report('anm-renderer-screen',{passed:true,checks,scope:'original screen sprite modes 0-3; vertex bytes, culling, tint, blends 0-6 and filtering; GPU rasterization separate'});
 }finally{m.close();c.release(vm);c.render_delete(fixture);}
});
