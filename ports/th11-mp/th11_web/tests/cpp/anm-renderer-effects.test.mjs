import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';import {graphicsOracle} from './graphics-oracle.mjs';
test('TH11 projected, fogged, world and effect submissions match the original',async()=>{
 const c=await core(),m=await oracle(),lib=graphicsOracle(m),fixture=c.render_create(),vm=c.allocate(0x434),geometry=c.allocate(4096),fog=c.allocate(28);
 const manager=m.allocate(0x7bd900),camera=m.allocate(0x120),device=m.allocate(4),vtable=m.allocate(0x1a0),nativeVm=m.allocate(0x434),sprite=m.allocate(64),texture=m.allocate(4),batch=m.allocate(1024*168),nativeGeometry=m.allocate(4096),scratch=m.allocate(512);
 m.u32(0x4c3268,manager);m.u32(0x4c3288,device);m.u32(device,vtable);m.u32(0x4c37cc,camera);m.u32(sprite+8,texture);m.u32(texture,1);
 const unit=Buffer.from(new Float32Array([-128,-128,0,0,0,128,-128,0,1,0,-128,128,0,0,1,128,128,0,1,1]).buffer);m.write(manager+0x4355d0,unit);
 let draws=[],matrices=new Map(),source=2,destination=1,filter=2,factor=0,depth=1,layout=0,topology=0,state=654987,checks=0;
 const random=()=>{state=(Math.imul(state,1664525)+1013904223)>>>0;return state/4294967296;},bits=n=>{const b=Buffer.alloc(4);b.writeFloatLE(n);return b.readUInt32LE();};
 const eq=(a,b,label)=>{assert.equal(a.length,b.length,label+' size');for(let i=0;i<a.length;i+=4)assert.equal(a.readUInt32LE(i),b.readUInt32LE(i),`${label} word=${i/4} ${a.readFloatLE(i)} vs ${b.readFloatLE(i)}`);};
 const arg=n=>m.u32(m.reg('ESP')+n*4),hook=(offset,name,argc,handler)=>m.u32(vtable+offset,m.registerImport({dll:'render-effects-oracle',name,argc,handler}));
 hook(0xe4,'state',3,()=>{if(arg(2)===19)source=arg(3);if(arg(2)===20)destination=arg(3);if(arg(2)===60)factor=arg(3);if(arg(2)===14)depth=arg(3);return 0;});
 hook(0x104,'texture',3,()=>0);hook(0x10c,'combiner',4,()=>0);hook(0x164,'layout',2,()=>{layout=arg(2);return 0;});hook(0x190,'stream',5,()=>0);
 hook(0x114,'sampler',4,()=>{if(arg(3)===6)filter=arg(4);return 0;});
 hook(0xb0,'transform',3,()=>{matrices.set(arg(2),Buffer.from(m.bytes(arg(3),64)));return 0;});
 hook(0x14c,'draw',5,()=>{topology=arg(2);const count=topology===4?arg(3)*3:arg(3)+2;assert.ok(count<1024);draws.push(Buffer.from(m.bytes(arg(4),count*arg(5))));return 0;});
 hook(0x144,'draw-world',4,()=>{topology=arg(2);assert.equal(arg(4),2);draws.push(unit);return 0;});
 try{for(let mode=4;mode<=13;++mode)for(let sample=0;sample<512;++sample){
  const context=`mode=${mode} sample=${sample}`;c.render_reset(fixture);draws=[];matrices.clear();m.u32(manager+0x435620,0);m.u32(manager+0x7b5624,batch);m.u32(manager+0x7b5628,batch);m.u32(manager+0x4355bc,0);m.u32(manager+0x4355c8,0);for(const off of [0x4355c0,0x4355c2,0x4355c6])m.view(manager+off,1)[0]=255;
  const cam=Buffer.alloc(188);cam.writeFloatLE(-600,8);cam.writeFloatLE(1,12);m.write(scratch,cam.subarray(0,12));m.write(scratch+16,Buffer.alloc(12));m.f32(scratch+32,0);m.f32(scratch+36,1);m.f32(scratch+40,0);
  m.call(lib.exports.get('D3DXMatrixLookAtLH'),{args:[scratch+64,scratch,scratch+16,scratch+32]});cam.set(m.bytes(scratch+64,64),24);
  m.call(lib.exports.get('D3DXMatrixPerspectiveFovLH'),{args:[scratch+128,...[1,4/3,1,10000].map(bits)]});cam.set(m.bytes(scratch+128,64),88);
  [0,0,640,480].forEach((v,k)=>cam.writeUInt32LE(v,152+k*4));cam.writeFloatLE(1,172);cam.writeFloatLE(500,176);cam.writeFloatLE(1200,180);cam.writeUInt32LE(0xff765432,184);
  memory(c,c.render_camera(fixture),188).set(cam);m.write(camera,cam.subarray(0,12));m.write(camera+0x30,cam.subarray(12,24));m.write(camera+0x4c,cam.subarray(24,88));m.write(camera+0x8c,cam.subarray(88,152));m.write(camera+0xcc,cam.subarray(152,176));m.f32(camera+0xfc,500);m.f32(camera+0x100,1200);m.u32(camera+0x114,0xff765432);
  const fogValues=[0,0,-600,50.5,84.25,118.75,500];memory(c,fog,28).set(Buffer.from(new Float32Array(fogValues).buffer));c.render_fog(fixture,fog);for(let i=0;i<3;++i){m.f32(0x4c3484+i*4,fogValues[i]);m.f32(0x4c3588+i*4,fogValues[i+3]);}m.f32(0x4c3580,500);
  const x=(random()-.5)*16,y=(random()-.5)*16,tint=(random()*4294967296)>>>0,enabled=sample&1;c.render_configure(fixture,x,y,tint,enabled);m.f32(manager+0xb0,x);m.f32(manager+0xb4,y);m.u32(manager+0x7bd88c,tint);m.u32(manager+0x7bd890,enabled);
  const b=Buffer.alloc(0x434);for(const off of [0x24,0x28,0x2c])b.writeFloatLE(sample%8===0?0:(random()-.5)*6.28,off);
  b.writeFloatLE((random()-.5)*5,0x3c);b.writeFloatLE((random()-.5)*5,0x40);b.writeFloatLE(random()*128,0x4c);b.writeFloatLE(random()*128,0x50);
  for(const off of [0x54,0x58,0x70,0x74,0x78,0x7c,0x80,0x84,0x88,0x8c])b.writeFloatLE(random()*2-1,off);
  for(const base of [0x2b4,0x2f4,0x334])for(let i=0;i<16;++i)b.writeFloatLE(i===15?1:(i%5===0?random():0),base+i*4);
  for(const off of [0x3dc,0x3e0,0x3e4,0x3f4,0x3f8,0x3fc])b.writeFloatLE((random()-.5)*12,off);
  b.writeFloatLE(random()*1000-500,0x3e8);b.writeFloatLE(random()*720-360,0x3ec);b.writeFloatLE(random()*1600-900,0x3f0);
  b.writeUInt32LE((((sample%7)<<4)|(mode<<22)|((sample%3)<<18)|((Math.floor(sample/3)%3)<<20)|(sample&1?0x8000:0)|(sample&2?0x80000000:0)|15)>>>0,0x404);b.writeUInt32LE((random()*4294967296)>>>0,0x374);b.writeUInt32LE((random()*4294967296)>>>0,0x378);
  b.writeInt32LE(2+sample%24,0x3b4);const mesh=Buffer.alloc(4096);for(let i=0;i<4096;i+=4)mesh.writeFloatLE(random()*10,i);memory(c,geometry,4096).set(mesh);m.write(nativeGeometry,mesh);
  if(mode===10&&sample&1){b.writeUInt32LE((mode<<22)>>>0,0x404);b.writeUInt32LE(0,0x374);}memory(c,vm,b.length).set(b);new DataView(c.memory.buffer).setUint32(vm+0x400,geometry,true);m.write(nativeVm,b);m.u32(nativeVm+0x400,nativeGeometry);m.u32(nativeVm+0x3ac,sprite);for(let i=0;i<4;++i)m.f32(0x4c91d8+i*28+12,1);
  const result=mode===10?c.render_ripple(fixture,vm):c.render_draw(fixture,vm);m.reg('EAX',nativeVm);const expectedResult=mode===10?m.call(0x452680,{ecx:nativeVm}):m.call(0x451ef0,{ecx:manager});assert.equal(result|0,expectedResult|0,context+' return');
  c.render_flush(fixture);m.reg('ESI',manager);m.call(0x44fd10);eq(Buffer.from(memory(c,c.render_data(fixture),c.render_size(fixture))),Buffer.concat(draws),context+' vertices');
  if(draws.length){assert.equal(c.render_state(fixture,0)+1,source,context+' source');assert.equal(c.render_state(fixture,1)+1,destination,context+' destination');assert.equal(c.render_state(fixture,2)+1,filter,context+' filter');assert.equal(c.render_pipeline(fixture,0),depth,context+' depth');}
  if(mode===8&&draws.length){eq(Buffer.from(memory(c,c.render_matrix(fixture,0),64)),matrices.get(256),context+' world');eq(Buffer.from(memory(c,c.render_matrix(fixture,1),64)),matrices.get(16),context+' texture matrix');assert.equal(c.render_pipeline(fixture,1)>>>0,factor,context+' factor');}
  eq(Buffer.from(memory(c,vm+0x2f4,64)),Buffer.from(m.bytes(nativeVm+0x2f4,64)),context+' transform');++checks;
 }report('anm-renderer-effects',{passed:true,checks,scope:'modes 4-13 including fog colors, matrices, meshes and invisible ripple callbacks; exact submitted bytes and material states'});
 }finally{m.close();for(const p of [vm,geometry,fog])c.release(p);c.render_delete(fixture);}
});
