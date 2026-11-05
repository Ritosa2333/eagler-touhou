import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';import {animationOracle} from './anm-oracle.mjs';import {instruction as ins,resource,bits} from './ecl-bytecode.mjs';
function spawn(op,name,args,time=0,refs=0){const text=Buffer.from(name+'\0'),n=(text.length+3)&~3,b=Buffer.alloc(20+n+args.length*4);ins(op,[],{time,refs,count:args.length+1}).copy(b);b.writeUInt16LE(b.length,6);b.writeUInt32LE(n,16);text.copy(b,20);args.forEach((v,i)=>b.writeUInt32LE(v>>>0,20+n+i*4));return b;}
test('TH11 enemy manager matches native nested spawn, lifecycle, clear and conditional creation',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m),{source,original}=o.load('enemy.anm');
 const data=c.allocate(source.length);memory(c,data,source.length).set(source);
 const player=m.allocate(0x8be0),spell=m.allocate(0x900),special=m.allocate(0x60),hud=m.allocate(0x4460),em=m.allocate(0x100),params=m.allocate(80),cp=c.allocate(80),name=m.allocate(32),cn=c.allocate(32),baseHeap=m.heap;
 m.u32(0x4a8eb4,player);m.u32(0x4a8d6c,spell);m.u32(0x4a8d64,special);m.u32(0x4a8d84,hud);m.u32(0x4a8d7c,em);
 // Actual enemy constructors, ECL, ANM and destructors execute. The unfinished
 // player-shot, item, audio and score systems remain explicit test boundaries.
 m.replace(0x4347f0,'controlled empty shots',()=>0,2);m.replace(0x431f70,'controlled empty collision',()=>0,1);
 let nativeRewards=[];
 m.replace(0x44a260,'sound sink',()=>0,1);m.replace(0x40ceb0,'score boundary',()=>{nativeRewards.push([3,m.i32(m.reg('ESP')+4),0,0]);return 0;},1);
 m.replace(0x438440,'score popup boundary',()=>{const p=m.u32(m.reg('ESP')+4);nativeRewards.push([9,m.reg('EAX')|0,m.f32(p),m.f32(p+4)]);return 0;},2);
 m.replace(0x4101d0,'item sink',()=>0,1);
 m.replace(0x424230,'primary item sink',()=>0,3);
 const get=a=>new DataView(c.memory.buffer).getUint32(a,true),put=(a,v)=>new DataView(c.memory.buffer).setUint32(a,v>>>0,true);
 const list=(first,read)=>{const result=[];for(let node=first;node;node=read(node+4)){assert.ok(result.length<100,'enemy list cycle');result.push(read(node));}return result;};
 const compare=(a,b,label)=>{if(!a.equals(b)){let off=0;while(a[off]===b[off])++off;off&=~3;assert.fail(`${label} +0x${off.toString(16)} cpp=${a.readUInt32LE(off).toString(16)} native=${b.readUInt32LE(off).toString(16)}`);}};
 let checks=0,frames=0,spawns=0,clears=0,f=0,p=0;
 try{for(let sample=0;sample<36;++sample){
  if(f){c.em_delete(f);f=0;}if(p)c.ecl_delete(p);p=c.ecl_create();m.heap=baseHeap;m.view(baseHeap,5000000).fill(0);o.reset();nativeRewards=[];
  for(const [a,n] of [[em,0x100],[player,0x8be0],[spell,0x900],[special,0x60],[hud,0x4460]])m.view(a,n).fill(0);
  const variant=[0x100,0x101,0x104,0x105,0x109,0x10a,0x10b,0x10c,0x10e,0x10f][sample%10],world=variant>=0x10e,rate=[1,.5,1.25][sample%3];
  const refs=sample%2?6:0,args=[bits(refs?-9999:-23.25),bits(refs?-9998:72.5),...(world?[bits(5.5)]:[]),350,600,1];
  const boss=sample%6===1,clear=sample%4===0;
  const code=resource({Main:[ins(40,[0]),ins(0x102,[0]),ins(0x106,[0,0]),...(boss?[ins(0x14c,[0])]:[]),spawn(variant,'Worker',args,0,refs),spawn(variant,'Worker',args,6,refs),spawn(variant,'Worker',args,11,refs),...(clear?[ins(sample%8?0x159:0x173,sample%8?[]:[[0,3,5][sample%3]],{time:20})]:[]),ins(10,[],{time:60})],
   Worker:[ins(40,[0]),ins(0x172,[3]),ins(0x102,[1]),ins(0x106,[0,[0,5,10,15][sample%4]]),ins(0x11c,[bits(.8),bits(.6)]),...(sample%5===0?[spawn(0x101,'Leaf',[bits(17),bits(40),200,12,4])]:[]),ins(0x124,[8,0,bits(.4)],{time:8}),ins(10,[],{time:35})],
   Leaf:[ins(40,[0]),ins(0x102,[0]),ins(0x103,[0,8]),ins(0x1c2,[7777],{time:3}),ins(0x149,[],{time:5}),ins(10,[],{time:12})]});
  const cd=c.allocate(code.length),nf=m.allocate(code.length),program=m.allocate(0x1100),vt=m.allocate(16);memory(c,cd,code.length).set(code);assert.equal(c.ecl_attach(p,cd,code.length),0);c.release(cd);m.write(nf,code);m.u32(program,vt);m.u32(vt+4,m.registerImport({dll:'fixture',name:'header',argc:0,handler:()=>0}));assert.equal(m.call(0x45d900,{ecx:program,args:[nf]}),0);
  f=c.em_create(p);assert.equal(c.ea_load(f,data,source.length),1);c.em_setup(f,sample%5,rate);
  const am=c.ea_manager(f),cf=c.ecl_file(p,0);for(let k=0;k<2;++k){memory(c,c.anm_env_rng(am,k),8).fill(0);new DataView(c.memory.buffer).setUint16(c.anm_env_rng(am,k),12345,true);}
  m.u32(em+0x64,program);for(const off of [0x40,0x44,0x48])m.u32(em+off,original);
  m.f32(0x4a7948,rate);m.u32(0x4a5720,sample%5);m.u32(0x4a5710,0);m.u32(0x4a5714,0);m.u32(player+0x928,1);m.f32(0x4c3484,0);m.f32(0x4c3488,0);
  m.u32(em+0x50,0xffffffff);m.u32(em+0x5c,0x4a7948);m.u32(em+0x60,1);
  const block=Buffer.alloc(80);block.writeFloatLE(11,0);block.writeFloatLE(80,4);block.writeInt32LE(1000,12);block.writeInt32LE(sample%2?4:1,16);block.writeInt32LE(1500,20);block.writeUInt32LE(sample%2,24);for(let k=0;k<12;++k)block.writeUInt32LE(k<4?sample*100+k:bits(k+.25),32+k*4);m.write(params,block);memory(c,cp,80).set(block);m.write(name,Buffer.from('Main\0'));memory(c,cn,5).set(Buffer.from('Main\0'));
  m.resetThreadFPU();m.reg('EBX',params);m.call(0x4108f0,{args:[name]});assert.ok(c.em_spawn(f,cn,cp),`spawn sample ${sample}, opcode ${c.em_error_opcode(f).toString(16)}`);
  const check=label=>{
   const left=list(c.em_first(f),get),right=list(m.u32(em+0x68),a=>m.u32(a));assert.equal(left.length,right.length,label+' count');assert.equal(c.em_value(f,0),m.u32(em+0x70),label+' active');assert.equal(c.em_value(f,1),m.u32(em+0x74),label+' total');assert.equal(c.em_value(f,3),m.u32(em+0x54),label+' timer');
   for(let i=0;i<left.length;++i){const a=left[i],b=right[i],cs=a+0x103c,ns=b+0x103c,A=Buffer.from(memory(c,cs,0x163c)),B=Buffer.from(m.bytes(ns,0x163c));
    for(const off of [0x160,0x168,0x16c,0x170,0x238,0x1564,0x1578,0x1614]){A.writeUInt32LE(0,off);B.writeUInt32LE(0,off);}compare(A,B,label+' enemy '+i);
    assert.equal(get(a+8),m.u32(b+8),label+' script time');assert.equal(get(a+12)?get(a+12)-cf:0,m.u32(b+12)?m.u32(b+12)-nf:0,label+' instruction');assert.equal(get(a+0x1024),m.u32(b+0x1024),label+' difficulty');++checks;
   }
   assert.equal(c.anm_manager_count(am),o.states().length,label+' animations');for(let k=0;k<2;++k)assert.deepEqual(Buffer.from(memory(c,c.anm_env_rng(am,k),8)),Buffer.from(m.bytes(k?0x4c2ef8:0x4c2f00,8)),label+' rng');
   const dv=new DataView(c.memory.buffer),events=c.ef_events(f),rewards=Array.from({length:c.ef_event_count(f)},(_,i)=>{const p=events+i*16;return[dv.getInt32(p,true),dv.getInt32(p+4,true),dv.getFloat32(p+8,true),dv.getFloat32(p+12,true)];}).filter(e=>e[0]===3||e[0]===9);assert.deepEqual(rewards,nativeRewards,label+' rewards');
  };
  check('sample '+sample+' initial');spawns+=c.em_value(f,1);
  for(let frame=0;frame<145;++frame){
   const label=`sample ${sample} frame ${frame}`;
   if(frame===10&&sample%3===0){const left=list(c.em_first(f),get),right=list(m.u32(em+0x68),a=>m.u32(a));const mask=[0,0x80,0x100,0x400,0x40000,0x20];for(let i=0;i<left.length;++i){put(left[i]+0x25bc,get(left[i]+0x25bc)|mask[i%6]);m.u32(right[i]+0x25bc,m.u32(right[i]+0x25bc)|mask[i%6]);}m.call(0x412880);assert.equal(c.em_clear(f),1);++clears;check(label+' clear');}
   m.resetThreadFPU();m.call(0x411120,{args:[em]});assert.equal(c.em_update(f),1,label+' manager update, opcode '+c.em_error_opcode(f).toString(16));check(label);++frames;
   assert.equal(c.anm_manager_update(am,0),1);o.update(false);
  }
  assert.equal(c.em_value(f,0),0,`sample ${sample} all completed enemies removed`);
 }
 report('enemy-manager',{passed:true,samples:36,frames,checks,initialSpawns:spawns,clears,scope:'Native constructors, nested ECL spawning, per-frame manager/destructor/ANM lifecycles and clear; controlled shot/collision/item/audio/score dependencies, not complete gameplay.'});
 }finally{if(f)c.em_delete(f);if(p)c.ecl_delete(p);c.release(cp);c.release(cn);c.release(data);m.close();}
});
