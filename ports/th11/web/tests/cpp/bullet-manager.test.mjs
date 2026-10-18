import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';
import {animationOracle} from './anm-oracle.mjs';
import {instruction as ins} from './ecl-bytecode.mjs';

test('TH11 complete projectile spawn, animation, update, cancellation and pool order match original',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m),{original,source}=o.load('bullet.anm');
 const data=c.allocate(source.length),em=c.allocate(0x214),manager=m.allocate(0x46d680),player=m.allocate(0x1000),nem=m.allocate(0x214),pause=m.allocate(0x100);
 const camera=m.allocate(0x100),device=m.allocate(4),vtable=m.allocate(0x180),batch=m.allocate(1024*168);let draws=[];
 m.u32(0x4c3288,device);m.u32(device,vtable);m.u32(0x4c37cc,camera);m.u32(camera+0xd4,640);m.u32(camera+0xd8,480);
 const arg=n=>m.u32(m.reg('ESP')+n*4),hook=(off,name,argc,handler)=>m.u32(vtable+off,m.registerImport({dll:'projectile-render-oracle',name,argc,handler}));
 for(const [off,argc]of [[0xe4,3],[0x104,3],[0x10c,4],[0x164,2],[0x114,4]])hook(off,'material state',argc,()=>0);
 hook(0x14c,'draw',5,()=>{assert.equal(arg(2),4);assert.equal(arg(5),28);draws.push(Buffer.from(m.bytes(arg(4),arg(3)*3*arg(5))));return 0;});
 memory(c,data,source.length).set(source);m.u32(0x4a8d68,manager);m.u32(0x4a8eb4,player);m.u32(manager+0x46d674,original);
 let f,events=[],collision=0,checks=0,frames=0,spawns=0;
 m.replace(0x431e00,'rectangular hit integration boundary',()=>collision);
 m.replace(0x431f70,'round hit integration boundary',()=>collision,1);
 m.replace(0x455b10,'visual effect integration boundary',()=>{const p=m.reg('EAX'),sp=m.reg('ESP');events.push([2,m.i32(sp+12),m.f32(p),m.f32(p+4)]);m.u32(m.u32(sp+8),0);return 0;},4);
 m.replace(0x40bb10,'graze counter integration boundary',()=>{events.push([3,0,0,0]);return 0;});
 m.replace(0x40baa0,'graze reward integration boundary',()=>{events.push([4,0,0,0]);return 0;},1);
 m.replace(0x44a260,'positional sound integration boundary',()=>{events.push([1,m.reg('EDI')|0,m.f32(m.reg('ESP')+4),0]);return 0;},1);
 m.replace(0x44a1e0,'centered sound integration boundary',()=>{events.push([0,m.reg('ESI')|0,0,0]);return 0;});
 const view=()=>new DataView(c.memory.buffer),read=p=>view().getUint32(p,true);
 const reset=(rate=1)=>{
  if(f)c.bm_delete(f);f=c.bm_create();assert.equal(c.bm_load(f,data,source.length),1);o.reset();
  m.view(manager,0x46d680).fill(0);m.u32(manager+0x46d674,original);m.u32(manager+0x10,manager+0x64);m.write(manager+0x46d216,Uint8Array.of(5,0));
  for(const [n,v]of [[0,rate],[3,20]])view().setFloat32(c.bm_data(f,n),v,true);
  view().setFloat32(c.bm_data(f,3)+4,400,true);m.f32(player+0x87c,20);m.f32(player+0x880,400);m.f32(0x4a7948,rate);
  for(const k of [1,2]){memory(c,c.bm_data(f,k),8).fill(0);view().setUint16(c.bm_data(f,k),12345,true);}
  m.u32(0x4a8e88,0);collision=0;c.bm_configure(f,collision,0,0);events=[];
 };
 const emitter=(type,color,aim=1)=>{
  const b=Buffer.alloc(0x214);b.writeInt16LE(type);b.writeInt16LE(color,2);b.writeFloatLE(16.25,4);b.writeFloatLE(90.75,8);b.writeFloatLE(.25,16);b.writeFloatLE(.12,20);b.writeFloatLE(2.4,24);b.writeFloatLE(1.2,28);b.writeInt16LE(3,0x1f8);b.writeInt16LE(2,0x1fa);b.writeInt16LE(aim,0x1fc);b.writeInt32LE(-1,0x208);return b;
 };
 const normalize=(input,base,get,cpp)=>{
  const b=Buffer.from(input),v=8;
  for(const off of [4,16,0x1c]){const p=b.readUInt32LE(v+off);if(p)b.writeUInt32LE((p-base)>>>0,v+off);}
  for(const off of [0x68,0xcc,0x118,0x144,0x190,0x1cc,0x218,0x244,0x270,0x29c,0x38c,0x3b0])b.writeUInt32LE(b.readUInt32LE(v+off)?1:0,v+off);
  const start=get(base+v+0x3a4);for(const off of [0x394,0x3a4,0x3a8]){const p=b.readUInt32LE(v+off);if(p)b.writeUInt32LE((p-start)>>>0,v+off);}
  b.writeUInt32LE(b.readUInt32LE(v+0x3ac)?1:0,v+0x3ac);b.writeUInt32LE(b.readUInt32LE(v+0x42c)?1:0,v+0x42c);b.writeUInt32LE(b.readUInt32LE(v+0x430)?1:0,v+0x430);
  for(const t of [0x464,0x478,...Array.from({length:11},(_,i)=>0x680+i*52),0x8f0])b.writeUInt32LE(b.readUInt32LE(t+12)?1:0,t+12);
  const next=b.readUInt32LE(0x4b8);if(next)b.writeUInt32LE(cpp?(next-c.bm_bullet(f,0))/0x910+1:(next-manager-0x64)/0x910+1,0x4b8);
  return b;
 };
 const compare=(index,label)=>{
  const p=c.bm_bullet(f,index),q=manager+0x64+index*0x910,a=normalize(memory(c,p,0x910),p,read,true),b=normalize(m.bytes(q,0x910),q,p=>m.u32(p),false);
  if(!a.equals(b)){let off=0;while(a[off]===b[off])++off;off&=~3;assert.fail(`${label} bullet ${index} offset 0x${off.toString(16)} cpp=${a.readUInt32LE(off).toString(16)} native=${b.readUInt32LE(off).toString(16)}`);}++checks;
 };
 const rng=()=>{assert.deepEqual(Buffer.from(memory(c,c.bm_data(f,1),8)),Buffer.from(m.bytes(0x4c2f00,8)),'script RNG');assert.deepEqual(Buffer.from(memory(c,c.bm_data(f,2),8)),Buffer.from(m.bytes(0x4c2ef8,8)),'visual RNG');};
 const checkEvents=()=>{const p=c.bm_data(f,4),n=c.bm_value(f,3),v=view();assert.deepEqual(Array.from({length:n},(_,i)=>[v.getInt32(p+i*16,true),v.getInt32(p+i*16+4,true),v.getFloat32(p+i*16+8,true),v.getFloat32(p+i*16+12,true)]),events);};
 const setEmitter=b=>{memory(c,em,b.length).set(b);m.write(nem,b);};
 const spawn=(index=0,layer=0)=>{const expected=m.call(0x408f20,{args:[manager,nem,index,layer,0]})|0;assert.equal(c.bm_spawn(f,em,index,layer,0),expected);assert.equal(c.bm_value(f,0),(m.u32(manager+0x10)-manager-0x64)/0x910);++spawns;return expected;};
 const tick=(count,label,paused=false)=>{m.u32(0x4a8e88,paused?pause:0);m.u32(pause+0x60,0x402);m.reg('ESI',manager);m.call(0x408d00);assert.equal(c.bm_update(f,+paused),1,`${label} error ${c.bm_value(f,2)}`);for(let i=0;i<count;++i)compare(i,label);checkEvents();rng();assert.equal(c.bm_value(f,1),m.u32(manager+0x5c));++frames;};
 const transform=(b,slot,op,a=0,v=0,x=0,y=0,concurrent=0)=>{const p=0x24+slot*24;b.writeFloatLE(a,p);b.writeFloatLE(v,p+4);b.writeInt32LE(x,p+8);b.writeInt32LE(y,p+12);b.writeUInt32LE(op,p+16);b.writeInt32LE(concurrent,p+20);};
 let drawChecks=0;const draw=(count,label)=>{for(let group=0;group<6;++group){draws=[];m.u32(o.manager+0x435620,0);m.u32(o.manager+0x7b5624,batch);m.u32(o.manager+0x7b5628,batch);m.u32(o.manager+0x4355bc,0);for(const off of [0x4355c0,0x4355c2,0x4355c6])m.view(o.manager+off,1)[0]=255;
   for(let i=0;i<4;++i)m.f32(0x4c91d8+i*28+12,1);m.reg('EAX',manager);m.call(0x408e20,{ecx:group});m.reg('ESI',o.manager);m.call(0x44fd10);
   assert.equal(c.bm_draw(f,group),1);const actual=Buffer.from(memory(c,c.bm_vertices(f),c.bm_vertex_bytes(f))),expected=Buffer.concat(draws);assert.equal(actual.length,expected.length,`${label} group ${group} vertex count`);
   for(let off=0;off<actual.length;off+=4)assert.equal(actual.readUInt32LE(off),expected.readUInt32LE(off),`${label} group ${group} vertex ${Math.floor(off/28)} word ${off%28/4}`);++drawChecks;
  }for(let i=0;i<count;++i)compare(i,label+' post draw');};
 try{
  for(let type=0;type<29;++type){const count=[17,28].includes(type)?4:type===23?3:[12,13,14,15,16,19,20,21,22,27].includes(type)?8:16;
   for(let color=0;color<count;++color){reset([1,.5,1.01][color%3]);const b=emitter(type,color,color%9);memory(c,em,b.length).set(b);m.write(nem,b);
    for(let index=0;index<3;++index){const expected=m.call(0x408f20,{args:[manager,nem,index,0,0]})|0;assert.equal(c.bm_spawn(f,em,index,0,0),expected,`spawn ${type}/${color}`);compare(index,`spawn ${type}/${color}`);++spawns;}rng();
    for(let frame=0;frame<8;++frame){collision=frame===3?2:frame===6?1:0;events=[];c.bm_configure(f,collision,c.bm_value(f,0),0);
     m.reg('ESI',manager);m.call(0x408d00);assert.equal(c.bm_update(f,0),1,`update error ${c.bm_value(f,2)}`);
     for(let index=0;index<3;++index)compare(index,`frame ${type}/${color}/${frame}`);checkEvents();rng();assert.equal(c.bm_value(f,1),m.u32(manager+0x5c));++frames;if(frame===2||frame===7)draw(3,`draw ${type}/${color}/${frame}`);
    }
   }
  }
  // Full emitter traversal, pool exhaustion, cursor wrap and reuse of a retired slot.
  reset();let b=emitter(4,6,3);b.writeInt16LE(100,0x1f8);b.writeInt16LE(21,0x1fa);b.writeUInt32LE(128,0x200);b.writeInt32LE(2,0x204);setEmitter(b);
  m.reg('ESI',nem);m.call(0x40a1f0,{limit:50000000});assert.equal(c.bm_fire(f,em),1);assert.equal(c.bm_value(f,0),0);checkEvents();rng();
  for(let i=0;i<2000;++i)compare(i,'full pool');assert.equal(spawn(),1);tick(2000,'paused full pool',true);tick(2000,'resumed full pool');
  for(const i of [17,1999]){const p=c.bm_bullet(f,i),q=manager+0x64+i*0x910;view().setUint32(p,read(p)|8,true);m.u32(q,m.u32(q)|8);}
  tick(2000,'retire full-pool slots');assert.equal(spawn(),0);compare(17,'reused slot');assert.equal(spawn(),0);compare(1999,'wrapped slot');rng();
  // Explicit cancel inside/outside the playfield and repeated cancellation.
  for(const pos of [[0,150],[-201,150],[0,457],[0,-9],[199,450]]){reset();b=emitter(4,6);b.writeFloatLE(pos[0],4);b.writeFloatLE(pos[1],8);setEmitter(b);spawn();
   for(let n=0;n<2;++n){m.reg('ESI',manager+0x64);m.call(0x40ae90);assert.equal(c.bm_cancel(f,0),1);compare(0,'cancel');checkEvents();}
   for(let n=0;n<24;++n)tick(1,'cancel animation');
  }
  // Appearance changes and grow-in interrupts execute actual bullet.anm scripts.
  for(const op of [0x800,0x1000000])for(const type of [0,12,17,18,23,25,28]){reset();b=emitter(4,6);transform(b,0,op,0,0,type,2);setEmitter(b);spawn();compare(0,`appearance ${op}/${type}`);for(let n=0;n<24;++n)tick(1,`appearance frame ${op}/${type}/${n}`);}
  for(let mode=0;mode<3;++mode){reset();b=emitter(4,6);transform(b,0,2,0,0,mode);setEmitter(b);spawn();compare(0,'grow in');for(let n=0;n<40;++n)tick(1,`grow in ${mode}/${n}`);}
  // A rejected spawn leaves its partly calculated launch state but not an active bullet.
  reset();b=emitter(0,3,7);setEmitter(b);m.f32(manager+0x60,1e6);c.bm_configure(f,0,0,1e6);assert.equal(spawn(),-1);compare(0,'excluded launch');rng();
  const ne=m.allocate(0x2678),nc=m.allocate(0x1024),ni=m.allocate(32),cc=c.allocate(0x1024),ci=c.allocate(32);let commandChecks=0;
  try{for(let sample=0;sample<144;++sample){reset();const ef=c.bm_enemy(f),ce=c.enemy_fixture_data(ef,0),cs=ce+0x103c,ns=ne+0x103c,slot=sample%8;
    memory(c,cc,0x1024).fill(0);view().setUint32(cc+4,ci,true);m.view(nc,0x1024).fill(0);m.u32(nc+4,ni);m.u32(nc+0x1014,ne);m.u32(ne,0x494074);m.u32(ne+4,nc);
    const code=ins(0x191,[slot]);memory(c,ci,code.length).set(code);m.write(ni,code);
    // Consume one random ECL float immediately before random bullet launch.
    // Both interpreters must advance the same stream, not synchronized copies.
    if(sample&1){m.u32(nc+0x1014,ne);const randomCode=ins(0x195,[slot,0,0],{refs:6});randomCode.writeFloatLE(-9999,20);randomCode.writeFloatLE(-9998,24);memory(c,ci,randomCode.length).set(randomCode);m.write(ni,randomCode);assert.equal(c.enemy_command(ef,cc),m.call(0x412e30,{ecx:ns})|0);rng();memory(c,ci,code.length).set(code);m.write(ni,code);}
    const position=[16.25,90.75,.1],offset=[-7.5,2.25,-.1],origin=[-18.3,60.7,sample%3?1:0];
    for(const [off,values]of [[0x34,position],[0x1414+slot*12,offset],[0x1474+slot*12,origin]])values.forEach((v,i)=>view().setFloat32(cs+off+i*4,v,true));
    view().setFloat32(cs+0x1590,sample%4===0?1e6:0,true);b=emitter(sample%29,0,sample%9);b.writeInt16LE(7,0x1f8);b.writeInt16LE(3,0x1fa);b.writeUInt32LE(128,0x200);b.writeInt32LE(7,0x204);memory(c,cs+0x374+slot*0x214,b.length).set(b);
    const initial=Buffer.from(memory(c,cs,0x163c));initial.writeUInt32LE(ne,0x1614);initial.writeUInt32LE(ne,0x168);for(const off of [0x160,0x1564,0x1578])initial.writeUInt32LE(0x4a7948,off);m.write(ns,initial);
    assert.equal(c.bm_enemy_fire(f,cc),m.call(0x412e30,{ecx:ns})|0,`enemy fire ${sample}`);assert.deepEqual(Buffer.from(memory(c,cs+0x374+slot*0x214,0x214)),Buffer.from(m.bytes(ns+0x374+slot*0x214,0x214)));rng();checkEvents();assert.equal(m.f32(manager+0x60),0);
    const count=c.bm_value(f,0);for(let i=0;i<count;++i)compare(i,`enemy fire ${sample}`);for(let frame=0;frame<4;++frame)tick(count,`enemy fire ${sample}/${frame}`);
    ++commandChecks;
  }}finally{c.release(cc);c.release(ci);}
  report('bullet-manager',{passed:true,checks,spawns,frames,drawChecks,commandChecks,scope:'Original projectile spawn and ANM execution, enemy fire command, pool exhaustion and reuse, pause/resume, cancellation, appearance transforms, grow-in interrupts, draw-group links, screen vertex bytes, timers and RNG. Player collision, graze reward, sound and external effect creation use ordered integration callbacks, not a complete player simulation.'});
 }finally{if(f)c.bm_delete(f);c.release(data);c.release(em);m.close();}
});
