import test from 'node:test';import assert from 'node:assert/strict';import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,report,root} from './helpers.mjs';import {animationOracle} from './anm-oracle.mjs';import {graphicsOracle} from './graphics-oracle.mjs';import {firstDifference,normalizeAnimation} from './shot-oracle.mjs';
test('TH11 stage layer drawing matches original culling, VM transforms and submissions',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m);let checks=0,drawCount=0;
 const dv=()=>new DataView(c.memory.buffer),read=p=>dv().getUint32(p,true),put=(p,x)=>dv().setUint32(p,x>>>0,true);
 const equal=(a,b,label)=>{assert.equal(firstDifference(Buffer.from(a),Buffer.from(b)),'',label);++checks;};
 try{for(let number=1;number<=7;++number){const a=o.load(`stage0${number}.anm`),t=o.load('text.anm',true);graphicsOracle(m);o.reset();m.u32(0x4c3808,t.original);m.u32(0x4c342c,1);
  const std=readFileSync(resolve(root,`reference/assets/stage0${number}.std`)),raw=c.allocate(std.length),araw=c.allocate(a.source.length),traw=c.allocate(t.source.length),f=c.stage_create(),s=c.stage_data(f,0),manager=c.stage_data(f,1);
  memory(c,raw,std.length).set(std);memory(c,araw,a.source.length).set(a.source);memory(c,traw,t.source.length).set(t.source);
  for(const vis of [0,1]){memory(c,c.anm_env_rng(manager,vis),8).fill(0);put(c.anm_env_rng(manager,vis),12345);}
  assert.equal(c.anm_open(c.stage_data(f,2),araw,a.source.length),1);assert.equal(c.anm_open(c.stage_data(f,3),traw,t.source.length),1);assert.equal(c.stage_load(f,raw,std.length,number),1);
  for(const [i,value]of[32,16,384,448].entries())put(s+0x3040+0xcc+i*4,value);
  const ns=m.allocate(0x3158),file=m.allocate(std.length),vms=m.allocate(std.readUInt16LE(2)*0x434),entries=m.allocate(24),base=file+std.readUInt32LE(8);
  m.view(vms,std.readUInt16LE(2)*0x434).fill(0);m.view(entries,24).fill(0);m.write(file,std);m.write(ns,memory(c,s,0x3158));m.u32(ns+0x44,0x4a7948);
  for(const [off,p]of[[8,entries],[12,entries+8],[0x3034,entries+16],[0x10,file],[0x14,file+0x90],[0x18,file+std.readUInt32LE(4)],[0x1c,base],[0x178,a.original],[0x17c,vms]])m.u32(ns+off,p);
  for(let i=0;i<std.readUInt16LE(0);++i)m.u32(file+0x90+i*4,file+std.readUInt32LE(0x90+i*4));m.u32(0x4a8d60,ns);m.call(0x405030);
  const device=m.allocate(4),table=m.allocate(0x1a0),batch=m.allocate(1024*168);m.u32(0x4c3288,device);m.u32(device,table);
  let draws=[],snapshots=[],world=Buffer.alloc(64),uv=Buffer.alloc(64),depth=1,compare=8,fog=1,layout=0,factor=0;
  const arg=n=>m.u32(m.reg('ESP')+n*4),hook=(off,name,argc,handler)=>m.u32(table+off,m.registerImport({dll:'stage-draw',name,argc,handler}));
  const snapshot=()=>snapshots.push({depth,compare,fog,layout,factor,world:Buffer.from(world),uv:Buffer.from(uv)});
  hook(0xe4,'state',3,()=>{if(arg(2)===14)depth=arg(3);if(arg(2)===23)compare=arg(3);if(arg(2)===28)fog=arg(3);if(arg(2)===60)factor=arg(3);return 0;});
  hook(0x104,'texture',3,()=>0);hook(0x10c,'combiner',4,()=>0);hook(0x164,'layout',2,()=>{layout=arg(2);return 0;});hook(0x190,'stream',5,()=>0);hook(0xbc,'viewport',2,()=>0);hook(0x114,'sampler',4,()=>0);
  hook(0xb0,'transform',3,()=>{if(arg(2)===256)world=Buffer.from(m.bytes(arg(3),64));if(arg(2)===16)uv=Buffer.from(m.bytes(arg(3),64));return 0;});
  hook(0x14c,'draw',5,()=>{snapshot();const count=arg(2)===4?arg(3)*3:arg(3)+2;draws.push(Buffer.from(m.bytes(arg(4),count*arg(5))));return 0;});
  const unit=Buffer.from(new Float32Array([-128,-128,0,0,0,128,-128,0,1,0,-128,128,0,0,1,128,128,0,1,1]).buffer);m.write(o.manager+0x4355d0,unit);
  hook(0x144,'draw world',4,()=>{snapshot();draws.push(unit);return 0;});
  for(let i=0;i<4;++i)m.f32(0x4c91d8+i*28+12,1);for(let i=0;i<3;++i){m.f32(0x4c3484+i*4,0);m.f32(0x4c3588+i*4,160);}m.f32(0x4c3580,1000);
  try{for(let frame=0;frame<121;++frame){assert.equal(c.stage_tick(f),1);m.reg('EAX',ns);m.call(0x402aa0,{limit:6000000});
   if(frame%20===0){c.stage_render_reset(f);draws=[];snapshots=[];depth=1;compare=8;fog=1;factor=0xffffffff;m.u32(0x4c3c10,1);
    m.u32(o.manager+0x435620,0);m.u32(o.manager+0x7b5624,batch);m.u32(o.manager+0x7b5628,batch);m.u32(o.manager+0x4355bc,0);m.u32(o.manager+0x4355c8,0);for(const off of[0x4355c0,0x4355c2,0x4355c6])m.view(o.manager+off,1)[0]=255;
    m.write(0x4c36b4,m.bytes(ns+0x3040,0x118));
    for(let layer=0;layer<12;++layer){assert.equal(c.stage_layer(f,layer),1);m.call(0x404190,{args:[ns,layer],limit:3000000});m.reg('ESI',o.manager);m.call(0x44fd10);}
    const label=`stage ${number} frame ${frame}`;equal(memory(c,c.stage_render_data(f,0),c.stage_render_size(f,0)),Buffer.concat(draws),label+' vertices');
    // Batch sizes may differ; world-mode calls each retain their own material
    // state and matrices, and screen-mode bytes remain in submission order.
    const actual=[];for(let i=0;i<c.stage_render_size(f,2);++i){const p=c.stage_render_data(f,2)+i*148;if(read(p+12)===0)actual.push(p);}
    const expected=snapshots.filter(x=>x.layout===0x102);assert.equal(actual.length,expected.length,label+' world calls');
    for(let i=0;i<actual.length;++i){const p=actual[i],x=expected[i];assert.deepEqual([read(p),read(p+4)+1,read(p+8),read(p+16)],[x.depth,x.compare,x.fog,x.factor],label+' world pipeline');equal(memory(c,p+20,64),x.world,label+' world matrix');equal(memory(c,p+84,64),x.uv,label+' UV matrix');}drawCount+=snapshots.length;
    equal(memory(c,s+0x2fe4,12),m.bytes(ns+0x2fe4,12),label+' counts');equal(memory(c,c.stage_render_data(f,3),0x118),m.bytes(0x4c36b4,0x118),label+' active camera');
    for(let i=0;i<std.readUInt16LE(2);++i){const p=c.stage_data(f,5)+i*0x434,q=vms+i*0x434;equal(normalizeAnimation(memory(c,p,0x434),read,read(p+0x3a4)),normalizeAnimation(m.bytes(q,0x434),v=>m.u32(v),m.u32(q+0x3a4)),label+' object VM '+i);}
    for(let i=0,p=std.readUInt32LE(4);std.readInt16LE(p)>=0;++i,p+=16)assert.equal(c.stage_instance_flag(f,i),m.u32(file+p)>>>16,label+' instance flag '+i);
   }
  }}finally{c.stage_delete(f);for(const p of[raw,araw,traw])c.release(p);}
 }report('stage-draw',{passed:true,checks,drawCount,scope:'Native 404190 layer traversal with all seven STD/ANM asset sets, actual original VM drawing/D3DX, submitted vertices, world material matrices, camera, VM transforms and visibility counts/flags.'});
 }finally{m.close();}
});
