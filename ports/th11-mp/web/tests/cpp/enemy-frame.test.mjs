import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';import {animationOracle} from './anm-oracle.mjs';import {instruction as ins,resource,bits} from './ecl-bytecode.mjs';
test('TH11 enemy full frame preserves original script, damage, interrupt, collision and animation order',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m),{source,original}=o.load('enemy.anm');
 const data=c.allocate(source.length);memory(c,data,source.length).set(source);
 const code=resource({Main:[ins(40,[0]),ins(0x102,[0]),ins(0x106,[0,0]),ins(0x103,[1,8]),ins(10,[],{time:120})],Phase:[ins(40,[0]),ins(0x14b,[1200]),ins(0x103,[1,9]),ins(10,[],{time:100})]});
 const p=c.ecl_create(),cd=c.allocate(code.length),nf=m.allocate(code.length),program=m.allocate(0x1100),vt=m.allocate(16),name=c.allocate(32),nn=m.allocate(32);
 memory(c,cd,code.length).set(code);assert.equal(c.ecl_attach(p,cd,code.length),0);m.write(nf,code);m.u32(program,vt);m.u32(vt+4,m.registerImport({dll:'fixture',name:'header',argc:0,handler:()=>0}));assert.equal(m.call(0x45d900,{ecx:program,args:[nf]}),0);
 const ne=m.allocate(0x2678),ns=ne+0x103c,player=m.allocate(0x8be0),spell=m.allocate(0x900),special=m.allocate(0x60),hud=m.allocate(0x4460),em=m.allocate(0x100),baseHeap=m.heap;
 m.u32(0x4a8eb4,player);m.u32(0x4a8d6c,spell);m.u32(0x4a8d64,special);m.u32(0x4a8d84,hud);m.u32(0x4a8d7c,em);m.u32(em+0x40,original);m.u32(em+0x48,original);
 let damage=0,collision=0,tick=0,events=[],f=0,checks=0,animationChecks=0,frames=0,deathChecks=0,transitionChecks=0;
 const event=(kind,value=0,x=0,y=0)=>{events.push([kind,value,x,y]);return 0;};
 m.replace(0x4347f0,'controlled shot damage',()=>{event(0,damage,m.f32(ns+0x34),m.f32(ns+0x38));return damage;},2);
 m.replace(0x431f70,'controlled collision',()=>{event(1,collision,m.f32(m.reg('ESP')+4));return collision;},1);
 m.replace(0x44a260,'sound events',()=>event(2,m.reg('EDI'),m.f32(m.reg('ESP')+4)),1);
 m.replace(0x40ceb0,'score events',()=>event(3,m.i32(m.reg('ESP')+4)),1);
 m.replace(0x4101d0,'controlled item drop boundary',()=>{event(4,m.i32(ns+0x1508));m.view(ns+0x150c,48).fill(0);return 0;},1);
 const tickHook=m.registerImport({dll:'fixture',name:'tick callback',argc:0,handler:()=>{event(6,tick);return tick;}});
 const damageHook=m.registerImport({dll:'fixture',name:'damage callback',argc:0,handler:()=>{event(7,damage);return damage;}});
 const collisionHook=m.registerImport({dll:'fixture',name:'collision callback',argc:0,handler:()=>event(8)});
 const view=()=>new DataView(c.memory.buffer),get=a=>view().getUint32(a,true),put=(a,v)=>view().setUint32(a,v>>>0,true),float=(a,v)=>view().setFloat32(a,v,true);
 const normalizeAnimation=(input,read)=>{const b=Buffer.from(input);for(const off of [0x68,0xcc,0x118,0x144,0x190,0x1cc,0x218,0x244,0x270,0x29c,0x38c,0x3b0])b.writeUInt32LE(0,off);
  for(const off of [4,16,0x1c]){const p=b.readUInt32LE(off);if(p)b.writeUInt32LE(read(p),off);}for(const off of [8,12,20,24]){const p=b.readUInt32LE(off);if(p)b.writeUInt32LE(read(read(p)),off);}
  const base=b.readUInt32LE(0x3a4);for(const off of [0x394,0x3a4,0x3a8]){const p=b.readUInt32LE(off);if(p)b.writeUInt32LE((p-base)>>>0,off);}for(const off of [0x3ac,0x400,0x410,0x414,0x428,0x42c])b.writeUInt32LE(b.readUInt32LE(off)?1:0,off);return b;};
 const compareBytes=(a,b,label)=>{if(!a.equals(b)){let off=0;while(a[off]===b[off])++off;off&=~3;assert.fail(`${label} +0x${off.toString(16)} cpp=${a.readUInt32LE(off).toString(16)} native=${b.readUInt32LE(off).toString(16)}`);}++checks;};
 try{for(let sample=0;sample<96;++sample){
  if(f)c.ef_delete(f);f=c.ef_create();assert.equal(c.ea_load(f,data,source.length),1);
  const ef=c.ea_enemy(f),ce=c.enemy_fixture_data(ef,0),cs=ce+0x103c,world=c.enemy_fixture_data(ef,2),am=c.ea_manager(f),cf=c.ecl_file(p,0);
  m.heap=baseHeap;m.view(baseHeap,3000000).fill(0);o.reset();for(const address of [player,spell,special,hud])m.view(address,address===player?0x8be0:address===hud?0x4460:address===spell?0x900:0x60).fill(0);
  for(let k=0;k<2;++k){memory(c,c.anm_env_rng(am,k),8).fill(0);view().setUint16(c.anm_env_rng(am,k),12345,true);}
  memory(c,name,32).fill(0);memory(c,name,5).set(Buffer.from('Main\0'));assert.equal(c.enemy_script_start(ef,p,name,255),0);
  const rate=[1,.5,1.25][sample%3];float(world+8,rate);float(c.anm_env_rng(am,0)-4,rate);m.f32(0x4a7948,rate);
  float(cs+0x68,20);float(cs+0x6c,130);float(cs+0x34,20);float(cs+0x38,130);
  const flags=[0,0x80000,0x4000000,0x8000000,0x80,0x10,0x22,0x1000000,0x400000,0x800000][sample%10];put(cs+0x1580,flags);put(cs+0x14f0,sample%4===0?40:4000);put(cs+0x14f4,4000);put(cs+0x14ec,700);put(cs+0x1544,2);put(cs+0x1548,3);put(cs+0x154c,0);put(cs+0x1584,7);put(cs+0x1588,0);
  if(sample%8===1){put(cs+0x1630,1);put(cs+0x1634,1);put(cs+0x1638,1);}
  const initial=Buffer.from(memory(c,cs,0x163c));initial.writeUInt32LE(ne,0x168);initial.writeUInt32LE(ne,0x1614);for(const off of [0x160,0x1564,0x1578])initial.writeUInt32LE(0x4a7948,off);for(const [off,hook] of [[0x1630,tickHook],[0x1634,damageHook],[0x1638,collisionHook]])if(initial.readUInt32LE(off))initial.writeUInt32LE(hook,off);
  m.view(ne,0x2678).fill(0);m.write(ns,initial);m.u32(ne,0x494074);m.u32(ne+0x102c,program);m.reg('EAX',ne);m.call(0x40fed0);m.u32(ne+0x1024,255);m.write(nn,Buffer.from('Main\0'));m.reg('EAX',program);m.u32(ne+12,m.call(0x45db10,{args:[nn]}));
  // Real ECL and ANM execute. Only player-shot results, audio/score sinks and
  // item creation are controlled dependencies, so this is not a full-game test.
  for(let frame=0;frame<45;++frame){
   const label=`sample ${sample} frame ${frame}`;events=[];damage=frame%6===3?[1,4,5,17,99,500][sample%6]:0;collision=sample%3;tick=sample%8===1&&frame===40?1:0;
   const playerState=sample%5,spellFlags=sample%4===0?0:sample%4===1?1:sample%4===2?0x21:9,spellId=[0,0x9e,0xa1,0xa5,0xae,0xaf][sample%6],active=frame>=9&&frame<24?1:0,playerFlags=(sample%3===0?4:0)|((sample%6===1||sample%6===2)&&sample%4===1?2:0),invincibility=frame>=24&&frame<36?30:0;
   for(const [k,v] of [[0,damage],[1,collision],[2,playerState],[3,active],[4,spellId],[5,playerFlags],[6,spellFlags],[7,+(invincibility!==0||(playerFlags&2)!==0)],[8,tick]])c.ef_setting(f,k,v);
   m.i32(player+0x8bb4,invincibility);
   m.i32(player+0x928,playerState);m.i32(special+0x3c,active);m.u32(player+0x8bc4,playerFlags);m.u32(spell+0x8e0,spellFlags);m.i32(spell+0x8dc,spellId);
   const vx=[0,-.5,-.1,0,.1,.5,0][Math.floor(frame/4)%7];float(cs+0x74,vx);m.f32(ns+0x74,vx);
   // Explicit Cartesian motion and a timed phase use the original state layout.
   put(cs+0x98,1);m.u32(ns+0x98,1);
   if(sample%8===2&&frame===3){put(cs+0x1594,200);put(cs+0x1598,15);m.i32(ns+0x1594,200);m.i32(ns+0x1598,15);memory(c,name,6).set(Buffer.from('Phase\0'));m.write(nn,Buffer.from('Phase\0'));put(cs+0x159c,name);put(cs+0x15a0,name);m.u32(ns+0x159c,nn);m.u32(ns+0x15a0,nn);++transitionChecks;}
   put(cs+0x1580,get(cs+0x1580)&~0x4000);m.u32(ns+0x1580,m.u32(ns+0x1580)&~0x4000);m.resetThreadFPU();const expected=m.call(0x411750,{args:[ns],limit:10000000})|0,actual=c.ef_update(f);assert.equal(actual,expected,label+' return');
   const left=Buffer.from(memory(c,cs,0x163c)),right=Buffer.from(m.bytes(ns,0x163c));for(const off of [0x160,0x168,0x1564,0x1578,0x1614]){left.writeUInt32LE(0,off);right.writeUInt32LE(0,off);}for(const off of [0x159c,0x15a0,0x1630,0x1634,0x1638]){left.writeUInt32LE(left.readUInt32LE(off)?1:0,off);right.writeUInt32LE(right.readUInt32LE(off)?1:0,off);}compareBytes(left,right,label+' state');
   const cb=Buffer.from(memory(c,c.ef_events(f),c.ef_event_count(f)*16)),nb=Buffer.alloc(events.length*16);events.forEach(([k,v,x,y],i)=>{nb.writeInt32LE(k,i*16);nb.writeInt32LE(v,i*16+4);nb.writeFloatLE(x,i*16+8);nb.writeFloatLE(y,i*16+12);});assert.deepEqual(cb,nb,label+' events');
   for(const [k,value] of [[0,m.u32(player+0x7c94)===ne?1:0],[1,m.view(player+0x7c98,1)[0]],[2,m.i32(hud+0x4440)],[3,m.i32(hud+0x4444)],[4,m.i32(spell+0x8e0)],[5,m.i32(spell+0x8e4)],[6,m.i32(em+0x18)]])assert.equal(c.ef_value(f,k),value,label+' world '+k);
   const stateList=o.states();assert.equal(c.anm_manager_count(am),stateList.length,label+' animation count');for(const item of stateList){const a=c.anm_manager_find(am,item.id);assert.ok(a,label+' animation '+item.id);compareBytes(normalizeAnimation(memory(c,a,0x434),get),normalizeAnimation(item.bytes,p=>m.u32(p)),label+' animation '+item.id);++animationChecks;}
   for(let k=0;k<2;++k)assert.deepEqual(Buffer.from(memory(c,c.anm_env_rng(am,k),8)),Buffer.from(m.bytes(k?0x4c2ef8:0x4c2f00,8)),label+' rng');
   ++frames;if(actual){if(actual===1)++deathChecks;break;}
   assert.equal(c.anm_manager_update(am,0),1);o.update(false);
  }
 }
 report('enemy-frame',{passed:true,checks,animationChecks,frames,deathChecks,transitionChecks,scope:'Original full enemy frame with real ECL/ANM; controlled player-shot/collision and item/audio/score interfaces; no deformation or full-game replay claim.'});
 }finally{if(f)c.ef_delete(f);c.ecl_delete(p);c.release(data);c.release(cd);c.release(name);m.close();}
});
