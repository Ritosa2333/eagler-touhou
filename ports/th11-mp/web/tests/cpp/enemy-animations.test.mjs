import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';import {animationOracle} from './anm-oracle.mjs';import {instruction as ins,bits} from './ecl-bytecode.mjs';
test('TH11 enemy animation commands run original animation resources and preserve manager order',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m),{source,original,heap}=o.load('enemy.anm');
 const data=c.allocate(source.length),cc=c.allocate(0x1024),ci=c.allocate(64),ne=m.allocate(0x2678),ns=ne+0x103c,nc=m.allocate(0x1024),ni=m.allocate(64),enemyManager=m.allocate(0x100),position=m.allocate(12),baseHeap=m.heap;
 memory(c,data,source.length).set(source);let f=0,checks=0,commands=0;
 const view=()=>new DataView(c.memory.buffer),read=p=>view().getUint32(p,true);
 const normalize=(input,get)=>{const b=Buffer.from(input);for(const off of [0x68,0xcc,0x118,0x144,0x190,0x1cc,0x218,0x244,0x270,0x29c,0x38c,0x3b0])b.writeUInt32LE(0,off);
  for(const off of [4,16,0x1c]){const p=b.readUInt32LE(off);if(p)b.writeUInt32LE(get(p),off);}for(const off of [8,12,20,24]){const p=b.readUInt32LE(off);if(p)b.writeUInt32LE(get(get(p)),off);}
  const base=b.readUInt32LE(0x3a4);for(const off of [0x394,0x3a4,0x3a8]){const p=b.readUInt32LE(off);if(p)b.writeUInt32LE((p-base)>>>0,off);}for(const off of [0x3ac,0x400,0x410,0x414,0x428,0x42c])b.writeUInt32LE(b.readUInt32LE(off)?1:0,off);return b;};
 const compare=label=>{
  const am=c.ea_manager(f),ef=c.ea_enemy(f),cs=c.enemy_fixture_data(ef,0)+0x103c,a=Buffer.from(memory(c,cs,0x163c)),b=Buffer.from(m.bytes(ns,0x163c));for(const off of [0x160,0x168,0x1564,0x1578,0x1614]){a.writeUInt32LE(0,off);b.writeUInt32LE(0,off);}assert.deepEqual(a,b,label+' enemy state');
  const states=o.states();assert.equal(c.anm_manager_count(am),states.length,label+' active count');for(const item of states){const p=c.anm_manager_find(am,item.id);assert.ok(p,label+' missing animation '+item.id);const left=normalize(memory(c,p,0x434),read),right=normalize(item.bytes,p=>m.u32(p));if(!left.equals(right)){let off=0;while(left[off]===right[off])++off;off&=~3;assert.fail(`${label} animation ${item.id} +0x${off.toString(16)} cpp=${left.readUInt32LE(off).toString(16)} native=${right.readUInt32LE(off).toString(16)}`);}++checks;}
  for(let k=0;k<2;++k)assert.deepEqual(Buffer.from(memory(c,c.anm_env_rng(am,k),8)),Buffer.from(m.bytes(k?0x4c2ef8:0x4c2f00,8)),label+' RNG');
 };
 const command=(op,args,options)=>{const b=ins(op,args,options);memory(c,ci,64).fill(0);memory(c,ci,b.length).set(b);m.write(ni,b);const expected=m.call(0x412e30,{ecx:ns})|0;assert.equal(c.ea_command(f,cc),expected,`opcode ${op.toString(16)}`);compare(`opcode ${op.toString(16)}`);++commands;};
 try{for(let sample=0;sample<48;++sample){if(f)c.ea_delete(f);f=c.ea_create();assert.equal(c.ea_load(f,data,source.length),1);const am=c.ea_manager(f),ef=c.ea_enemy(f),ce=c.enemy_fixture_data(ef,0),cs=ce+0x103c;
   m.heap=baseHeap;m.view(baseHeap,2000000).fill(0);o.reset();for(let k=0;k<2;++k){memory(c,c.anm_env_rng(am,k),8).fill(0);view().setUint16(c.anm_env_rng(am,k),12345,true);}
   memory(c,cc,0x1024).fill(0);view().setUint32(cc+4,ci,true);m.view(ne,0x2678).fill(0);m.view(nc,0x1024).fill(0);m.u32(ne,0x494074);m.u32(ne+4,nc);m.u32(nc+4,ni);m.u32(nc+0x1014,ne);m.u32(0x4a8d7c,enemyManager);m.u32(enemyManager+0x40,original);
   view().setUint32(cs+0x1580,sample&1?0x20:0,true);view().setInt32(cs+0x120,sample%5,true);const initial=Buffer.from(memory(c,cs,0x163c));initial.writeUInt32LE(ne,0x1614);initial.writeUInt32LE(ne,0x168);for(const off of [0x160,0x1564,0x1578])initial.writeUInt32LE(0x4a7948,off);m.write(ns,initial);
   command(0x102,[0]);command(0x106,[0,sample%8]);command(0x103,[1,8+sample%8]);command(0x108,[0,sample%8]);
   command(0x115,[0,bits(-9998)],{refs:2});command(0x113,[1,2]);
   // Position-bearing effects, both playfield and native world coordinates.
   for(const worldSpace of [false,true]){
    for(const [off,value]of [[0x34,-19.25],[0x38,171.5],[0x3c,.125]]){view().setFloat32(cs+off,value,true);m.f32(ns+off,value);}
    const flags=(read(cs+0x1580)&~0x400000)|(worldSpace?0x400000:0);view().setUint32(cs+0x1580,flags,true);m.u32(ns+0x1580,flags);
    for(const op of [0x107,0x110,0x111])command(op,[0,sample%8,bits(.375)]);
   }
   const id=read(cs+0xe0),p=c.anm_manager_find(am,id),q=o.states().find(v=>v.id===id).p;
   c.ea_position(f,id,16.25,170.75,.1,1);m.f32(position,16.25);m.f32(position+4,170.75);m.f32(position+8,.1);m.reg('EAX',q);m.call(0x456430,{edx:position});compare('position');
   for(let frame=0;frame<12;++frame){if(frame===2)command(0x10d,[0]);if(frame===4)command(0x112,[0,1]);if(frame===7)command(0x114,[]);if(frame===9)command(0x103,[1,-1]);assert.equal(c.anm_manager_update(am,0),1);o.update(false);compare(`sample ${sample} frame ${frame}`);}
  }report('enemy-animations',{passed:true,checks,commands,scope:'Real enemy.anm execution and ECL animation selection, creation, directional variants, rotation, interrupts, hiding, deferred deletion, positions, manager order and RNG.'});
 }finally{if(f)c.ea_delete(f);c.release(data);c.release(cc);c.release(ci);m.close();}
});
