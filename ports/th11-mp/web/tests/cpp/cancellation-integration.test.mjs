import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';import {animationOracle} from './anm-oracle.mjs';import {bits} from './ecl-bytecode.mjs';import {normalizeAnimation,firstDifference} from './shot-oracle.mjs';
test('TH11 cancellation connects real bullets, lasers, destructible enemies, point-item pools and effects',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m),{source,original}=o.load('bullet.anm');const data=c.allocate(source.length),cp=c.allocate(12);memory(c,data,source.length).set(source);
 const bm=m.allocate(0x46d680),em=m.allocate(0x100),enemies=m.allocate(96*0x2678),items=m.allocate(0x265e70),spell=m.allocate(0x900),player=m.allocate(0x8d40),sht=m.allocate(32),hud=m.allocate(0x4460),np=m.allocate(12),lm=m.allocate(0x480),la=m.allocate(0x204),sound=m.allocate(0x100),heap=m.heap,ca=c.allocate(0x204);
 m.u32(0x4a8e94,lm);m.u32(0x4a8e88,sound);m.view(sound,0x100).fill(0);
 m.replace(0x44a260,'cancellation sound boundary',()=>0,1);
 m.replace(0x432070,'cancellation player laser collision boundary',()=>0,3);
 const ef=c.enemy_fixture_create(),enemy=c.enemy_fixture_data(ef,0),context=c.allocate(0x1024),instruction=c.allocate(32),ne=m.allocate(0x2678),nc=m.allocate(0x1024),ni=m.allocate(32),safeHeap=m.heap;
 m.u32(0x4a8d68,bm);m.u32(0x4a8d7c,em);m.u32(0x4a8e90,items);m.u32(0x4a8d6c,spell);m.u32(0x4a8eb4,player);m.u32(0x4a8d84,hud);m.u32(player+0x92c,sht);m.i32(player+0x928,2);m.f32(player+0x880,400);m.f32(sht+8,8);
 const view=()=>new DataView(c.memory.buffer),read=p=>view().getUint32(p,true),put=(p,v)=>view().setUint32(p,v,true),flt=(p,v)=>view().setFloat32(p,v,true);
 let f=0,cases=0,checks=0,frames=0,animationChecks=0,laserChecks=0,laserSpawns=0,commandChecks=0;
 const normalizedItem=input=>{const b=Buffer.from(input);for(const off of [4,16,0x68,0xcc,0x118,0x144,0x190,0x1cc,0x218,0x244,0x270,0x29c,0x38c,0x3b0,0x45c])b.writeUInt32LE(0,off);const start=b.readUInt32LE(0x3a4);for(const off of [0x394,0x3a4,0x3a8])if(b.readUInt32LE(off))b.writeUInt32LE((b.readUInt32LE(off)-start)>>>0,off);b.writeUInt32LE(b.readUInt32LE(0x3ac)?1:0,0x3ac);return b;};
 const compare=label=>{
  let a=c.cx_laser_first(f),b=m.u32(lm+0x18),count=0;
  while(a||b){assert.ok(a&&b,label+' laser list');assert.ok(count++<256,label+' laser list capacity');
   const x=Buffer.from(memory(c,a+12,0x434)),y=Buffer.from(m.bytes(b+12,0x434));
   for(const off of[0x14,0x28,...Array.from({length:18},(_,i)=>0x70+i*52),0x424])for(const bytes of[x,y])bytes.writeUInt32LE(bytes.readUInt32LE(off)?1:0,off);
   assert.equal(firstDifference(x,y),'',`${label} laser ${count}`);++laserChecks;a=read(a+8);b=m.u32(b+8);
  }
  for(let i=0;i<2198;++i){const p=c.cx_item(f,i),q=items+0x14+i*0x478;if(read(p+0x464)||m.u32(q+0x464)){assert.equal(firstDifference(normalizedItem(memory(c,p,0x478)),normalizedItem(m.bytes(q,0x478))),'',`${label} item ${i}`);++checks;}}
  for(const [k,off]of [[0,0x265e68],[1,0x265e6c],[2,0x265e64]])assert.equal(c.cx_value(f,k),m.u32(items+off),`${label} counter ${k}`);
  const am=c.cx_anm(f),states=o.states();assert.equal(c.anm_manager_count(am),states.length,label+' effect count');
  for(const {id,p,bytes}of states){const q=c.anm_manager_find(am,id);assert.ok(q);const a=normalizeAnimation(memory(c,q,0x434),read,read(q+0x3a4)),b=normalizeAnimation(bytes,p=>m.u32(p),m.u32(p+0x3a4));assert.equal(firstDifference(a,b),'',`${label} effect ${id}`);++animationChecks;}
  for(let k=0;k<2;++k)assert.deepEqual(Buffer.from(memory(c,c.anm_env_rng(am,k),8)),Buffer.from(m.bytes(k?0x4c2ef8:0x4c2f00,8)),label+' rng');
 };
 try{for(let sample=0;sample<12;++sample){
  if(f)c.cx_delete(f);f=c.cx_create();assert.equal(c.cx_load(f,data,source.length),1);m.heap=safeHeap;o.reset();for(const [p,n]of [[bm,0x46d680],[em,0x100],[enemies,96*0x2678],[items,0x265e70],[spell,0x900],[lm,0x480]])m.view(p,n).fill(0);m.u32(bm+0x46d674,original);m.u32(lm+0x450,lm+16);m.u32(lm+0x458,0x10000);m.u32(lm+0x474,original);
  const am=c.cx_anm(f),rate=[1,.5,1.25][sample%3];flt(c.anm_env_rate(am),rate);m.f32(0x4a7948,rate);put(c.cx_player(f)+12,2);
  for(let k=0;k<2;++k){memory(c,c.anm_env_rng(am,k),8).fill(0);view().setUint16(c.anm_env_rng(am,k),12345,true);}
  for(let i=0;i<96;++i){const p=c.cx_enemy(f,i),q=enemies+i*0x2678;flt(p+0x1070,(i%12-6)*25);flt(p+0x1074,100+Math.floor(i/12)*32);flt(p+0x1078,i%3);put(p+0x25bc,i%3?0x800:0);put(p+0x252c,1200);put(p+0x2538,8400);put(p+0x253c,1200);put(p+0x2540,i%2);const b=Buffer.from(memory(c,p,0x2678));b.writeUInt32LE(q,0x11a4);b.writeUInt32LE(i<95?q+0x2678+0x11a4:0,0x11a8);b.writeUInt32LE(i?q-0x2678+0x11a4:0,0x11ac);m.write(q,b);}
  m.u32(em+0x68,enemies+0x11a4);
  for(let pass=0;pass<3;++pass){
   for(let i=0;i<24;++i){const type=i%2,p=Buffer.alloc(type?0x204:0x1e4);p.writeFloatLE((i%6-3)*72);p.writeFloatLE(48+Math.floor(i/6)*112,4);p.writeFloatLE(i%3,8);
    if(type){p.writeFloatLE((i%5-2)*.7,24);p.writeFloatLE(224,32);p.writeFloatLE(192,36);p.writeFloatLE(32,40);p.writeFloatLE(2,44);for(const[j,v]of[0,1,200,20].entries())p.writeInt32LE(v,48+j*4);p.writeInt32LE(-1,64);p.writeInt32LE(-1,68);p.writeInt16LE(4,76);p.writeInt16LE(i%16,78);p.writeUInt32LE(i%4===1?8:0,80);}
    else{p.writeFloatLE((i%5-2)*.7,12);p.writeFloatLE(224,16);p.writeFloatLE(192,20);p.writeFloatLE(800,24);p.writeFloatLE(32,28);p.writeFloatLE(2,32);p.writeInt16LE(4,36);p.writeInt16LE(i%16,38);p.writeInt32LE(-1,0x1dc);p.writeInt32LE(-1,0x1e0);}
    memory(c,ca,p.length).set(p);m.write(la,p);m.reg('EDI',la);assert.equal(c.cx_laser_spawn(f,ca,type),m.call(0x424df0,{args:[type]})|0);++laserSpawns;
   }
   for(let i=0;i<2000;++i){const b=Buffer.alloc(0x910);b.writeInt16LE(i%7===0?2:1,0x4b2);b.writeInt32LE(i%9===0?4:0,4);b.writeFloatLE((i%47-23)*8.25,0x43c);b.writeFloatLE(16+Math.floor(i/47)*10,0x440);b.writeFloatLE(4,0x45c);b.writeFloatLE(4,0x460);b.writeInt32LE(i%32===0?14:-1,0x4a4);memory(c,c.cx_bullet(f,i),b.length).set(b);m.write(bm+0x64+i*0x910,b);}
   const pos=[0,224,0],radius=[512,96,300][sample%3],protectedSpell=sample>=6,skip=sample%2,all=pass===2;pos.forEach((v,i)=>{flt(cp+i*4,v);m.f32(np+i*4,v);});m.u32(spell+0x8e0,+protectedSpell);m.u32(spell+0x8dc,160);
   if(pass===1){
    const ins=Buffer.alloc(32),op=[0x1a4,0x1a5,0x1be,0x1bf][sample%4];ins.writeUInt16LE(op,4);ins.writeUInt16LE(32,6);ins[10]=255;ins[11]=1;ins.writeFloatLE(radius,16);
    memory(c,instruction,32).set(ins);memory(c,context,0x1024).fill(0);put(context+4,instruction);memory(c,enemy+0x1070,12).set(memory(c,cp,12));
    m.write(ne,Buffer.from(memory(c,enemy,0x2678)));m.u32(ne,0x494074);m.u32(ne+4,nc);m.view(nc,0x1024).fill(0);m.u32(nc+4,ni);m.u32(nc+0x1014,ne);m.u32(ne+0x2650,ne);m.write(ni,ins);m.resetThreadFPU();
    assert.equal(c.cx_enemy_command(f,ef,context,+protectedSpell,160),m.call(0x412e30,{ecx:ne+0x103c,limit:50000000})|0);++commandChecks;
   }else{
    if(all){m.reg('EBX',skip);m.call(0x40b5c0,{limit:50000000});}else{m.reg('EBX',np);m.call(0x40af90,{args:[bits(radius),1,skip],limit:50000000});}
    assert.equal(c.cx_cancel(f,all?0:cp,radius,1,skip,+protectedSpell,160),1);
    if(all){m.reg('EBX',1);m.reg('EDI',skip);m.call(0x425040);}else{m.reg('ESI',np);m.call(0x424fe0,{args:[bits(radius),1,skip]});}
    assert.ok(c.cx_laser_cancel(f,all?0:cp,radius,1,skip)>=0);
   }++cases;
   for(let i=0;i<2000;++i){const a=Buffer.from(memory(c,c.cx_bullet(f,i),0x910)),b=Buffer.from(m.bytes(bm+0x64+i*0x910,0x910));for(const x of [a,b])x.writeUInt32LE(x.readUInt32LE(0x470)?1:0,0x470);assert.equal(firstDifference(a,b),'',`sample ${sample} pass ${pass} bullet ${i}`);++checks;}
   for(let i=0;i<96;++i){const a=c.cx_enemy(f,i),b=enemies+i*0x2678;assert.deepEqual(Buffer.from(memory(c,a+0x252c,24)),Buffer.from(m.bytes(b+0x252c,24)));++checks;}
   compare(`sample ${sample} pass ${pass}`);
  }
  for(let frame=0;frame<70;++frame){m.call(0x424d70,{ecx:lm});assert.equal(c.cx_laser_update(f),1);m.call(0x423550,{args:[items],limit:20000000});o.update(false);assert.equal(c.cx_step(f),1);compare(`sample ${sample} frame ${frame}`);++frames;}
 }
 report('cancellation-integration',{passed:true,cases,frames,checks,animationChecks,laserChecks,laserSpawns,commandChecks,scope:'Real native and C++ bullet and line/infinite laser cancellation, splitting, protected beams, ECL combined-cancellation commands 1a4/1a5/1be/1bf through script dispatch, linked destructible-enemy damage, shared type-8 item pool creation/overflow, delayed activation, ANM effects and shared RNG. No cancellation/effect/item callbacks mocked. Audio and laser-player collision are boundaries; controlled player state prevents pickup. Bomb lifecycle and stage orchestration excluded.'});
 }finally{if(f)c.cx_delete(f);c.enemy_fixture_delete(ef);for(const p of[data,cp,ca,context,instruction])c.release(p);m.close();}
});
