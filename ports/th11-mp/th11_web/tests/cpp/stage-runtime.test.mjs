import test from 'node:test';import assert from 'node:assert/strict';import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,report,root} from './helpers.mjs';import {animationOracle} from './anm-oracle.mjs';import {graphicsOracle} from './graphics-oracle.mjs';import {firstDifference,normalizeAnimation} from './shot-oracle.mjs';
test('TH11 stage objects, embedded animations and both deformation meshes match original update',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m);let checks=0,frames=0;
 const dv=()=>new DataView(c.memory.buffer),read=p=>dv().getUint32(p,true),put=(p,x)=>dv().setUint32(p,x>>>0,true);
 const selected=process.env.TH11_STAGE_NUMBERS?process.env.TH11_STAGE_NUMBERS.split(',').map(Number):[1,2,3,4,5,6,7];
 try{for(const number of selected){const a=o.load(`stage0${number}.anm`),t=o.load('text.anm',true);graphicsOracle(m);m.u32(0x4c3808,t.original);m.u32(0x4c342c,1);
  const std=readFileSync(resolve(root,`reference/assets/stage0${number}.std`)),raw=c.allocate(std.length),araw=c.allocate(a.source.length),traw=c.allocate(t.source.length);
  memory(c,raw,std.length).set(std);memory(c,araw,a.source.length).set(a.source);memory(c,traw,t.source.length).set(t.source);
  const ns=m.allocate(0x3158),file=m.allocate(std.length),vms=m.allocate(std.readUInt16LE(2)*0x434),entries=m.allocate(24),heap=m.heap;
  for(const speed of [1,.5])for(const time of [0,6940,9700]){m.heap=heap;o.reset();m.view(ns,0x3158).fill(0);m.view(vms,std.readUInt16LE(2)*0x434).fill(0);m.view(entries,24).fill(0);m.write(file,std);
   const f=c.stage_create(),manager=c.stage_data(f,1),s=c.stage_data(f,0),rate=c.anm_env_rate(manager);dv().setFloat32(rate,speed,true);m.f32(0x4a7948,speed);
   for(const vis of [0,1]){memory(c,c.anm_env_rng(manager,vis),8).fill(0);put(c.anm_env_rng(manager,vis),12345);}
   assert.equal(c.anm_open(c.stage_data(f,2),araw,a.source.length),1);assert.equal(c.anm_open(c.stage_data(f,3),traw,t.source.length),1);assert.equal(c.stage_load(f,raw,std.length,number),1);
   const base=file+std.readUInt32LE(8);m.write(ns,memory(c,s,0x3158));m.u32(ns+0x44,0x4a7948);
   for(const [off,p] of [[8,entries],[12,entries+8],[0x3034,entries+16],[0x10,file],[0x14,file+0x90],[0x18,file+std.readUInt32LE(4)],[0x1c,base],[0x178,a.original],[0x17c,vms]])m.u32(ns+off,p);
   for(let i=0;i<std.readUInt16LE(0);++i)m.u32(file+0x90+i*4,file+std.readUInt32LE(0x90+i*4));m.u32(0x4a8d60,ns);m.u32(0x4c3c40,0);m.write(0x4c36b4,memory(c,c.stage_render_data(f,3),0x118));m.call(0x405030);
   c.timer_set(s+0x38,time,rate);m.reg('EAX',ns+0x38);m.call(0x406100,{args:[time]});
   const animation=(p,q,label)=>{const x=normalizeAnimation(memory(c,p,0x434),read,read(p+0x3a4)),y=normalizeAnimation(m.bytes(q,0x434),v=>m.u32(v),m.u32(q+0x3a4));x.writeUInt32LE(0,0x418);y.writeUInt32LE(0,0x418);assert.equal(firstDifference(x,y),'',label);++checks;};
   const equal=(p,q,n,label)=>{assert.equal(firstDifference(Buffer.from(memory(c,p,n)),Buffer.from(m.bytes(q,n))),'',label);++checks;};
   const compare=label=>{
    const x=Buffer.from(memory(c,s,0x3158)),y=Buffer.from(m.bytes(ns,0x3158));for(const b of [x,y])for(const off of [8,12,0x10,0x14,0x18,0x1c,0x30,0x44,0x8c,0xd8,0x164,0x178,0x17c,0x3000,0x3034])b.writeUInt32LE(0,off);
    y.writeUInt32LE(m.u32(ns+0x4c)-base,0x4c);y.writeUInt32LE(m.u32(ns+0x3010)?1:0,0x3010);
    for(const b of [x,y]){b.fill(0,0x180,0x2320);b.fill(0,0x2348,0x2fe4);}assert.equal(firstDifference(x,y),'',label+' state');++checks;
    for(let i=0;i<8;++i)animation(s+0x180+i*0x434,ns+0x180+i*0x434,label+' script VM '+i);
    for(let i=0;i<std.readUInt16LE(2);++i)animation(c.stage_data(f,5)+i*0x434,vms+i*0x434,label+' object VM '+i);
    for(let i=0;i<std.readUInt16LE(0);++i)assert.equal(c.stage_object_flag(f,i),m.bytes(m.u32(file+0x90+i*4)+3,1)[0]);
    equal(c.stage_render_data(f,3),0x4c36b4,0x118,label+' published camera');
    for(let i=0;i<3;++i)equal(c.anm_env_camera(manager,i),[0x4c36b4,0x4c36d8,0x4c37a4][i],12,label+' animation camera input '+i);
    const list=o.states();assert.equal(c.anm_manager_count(manager),list.length);for(const vm of list)animation(c.anm_manager_find(manager,vm.id),vm.p,label+' registered VM '+vm.id);
    for(const vis of [0,1])equal(c.anm_env_rng(manager,vis),vis?0x4c2ef8:0x4c2f00,8,label+' RNG');assert.equal(read(c.stage_data(f,4)),m.u32(0x4c3c40));
    const mesh=m.u32(ns+0x3010),cm=c.stage_mesh_data(f,0);assert.equal(!!cm,!!mesh);if(mesh){equal(cm,mesh,8,label+' mesh dimensions');const count=m.u32(mesh)*m.u32(mesh+4);equal(c.stage_mesh_data(f,1),m.u32(mesh+16),count*28,label+' mesh vertices');equal(c.stage_mesh_data(f,2),m.u32(mesh+20),count*12,label+' mesh positions');}
   };
   try{compare(`stage ${number} initial`);for(let frame=0;frame<180;++frame){const label=`stage=${number} speed=${speed} start=${time} frame=${frame}`;assert.equal(c.stage_tick(f),1,label);m.reg('EAX',ns);m.call(0x402aa0,{limit:6000000});compare(label);++frames;
     if(m.u32(ns+0x3010)){c.stage_mesh_copy(f);m.reg('EBX',m.u32(ns+0x3010));m.call(0x40e890);}
     o.update(false);assert.equal(c.anm_manager_update(manager,0),1);compare(label+' registry');
   }}finally{c.stage_delete(f);}
  }c.release(raw);c.release(araw);c.release(traw);
 }report('stage-runtime'+(process.env.TH11_STAGE_NUMBERS?'-partial':''),{passed:true,checks,frames,stages:selected,scope:'Native 405030 initialization and 402aa0/4040f0/404470 updates with real ANM assets, object and embedded VM state, registry state, both RNGs, deformation dimensions/base positions/vertices, slow timing and late script transitions.'});
 }finally{m.close();}
});
