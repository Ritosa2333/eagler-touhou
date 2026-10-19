import test from 'node:test';
import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';
import {animationOracle} from './anm-oracle.mjs';
import {graphicsOracle} from './graphics-oracle.mjs';
import {firstDifference} from './shot-oracle.mjs';

test('TH11 enemy callbacks match original attraction, bullet transformation and enemy spawning',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m);
 const {source,original}=o.load('bullet.anm'),data=c.allocate(source.length),em=c.allocate(0x214),f=c.ecb_create(),bf=c.ecb_base(f);
 graphicsOracle(m);memory(c,data,source.length).set(source);assert.equal(c.bm_load(bf,data,source.length),1);
 const ep=c.ecb_data(f,0,0),manager=m.allocate(0x46d680),player=m.allocate(0x1000),enemy=m.allocate(0x163c),indicator=m.allocate(0x434),nem=m.allocate(0x214);
 m.view(manager,0x46d680).fill(0);m.u32(0x4a8d68,manager);m.u32(manager+0x46d674,original);m.u32(manager+0x10,manager+0x64);m.u32(0x4a8eb4,player);m.u32(0x4c343c,indicator);m.reg('ESI',indicator);m.call(0x401fd0);
 let events=[],calls=[],spawns=[],checks=0,cases=0,transformations=0;
 const arg=n=>m.u32(m.reg('ESP')+4*n),pos=p=>[m.f32(p),m.f32(p+4),m.f32(p+8)];
 m.replace(0x455b10,'cancel effect boundary',()=>{const p=m.reg('EAX');events.push([2,arg(3)|0,m.f32(p),m.f32(p+4)]);m.u32(arg(2),0);return 0;},4);
 m.replace(0x44a260,'positional sound boundary',()=>{events.push([1,m.reg('EDI')|0,m.f32(m.reg('ESP')+4),0]);return 0;},1);
 m.replace(0x410580,'forced player movement boundary',()=>{calls.push([0,0,...pos(m.reg('EDI')),0,0,0]);return 0;});
 m.replace(0x4108f0,'spawn enemy boundary',()=>{assert.equal(m.string(arg(1)),'MBossCard2_at2');spawns.push(Buffer.from(m.bytes(m.reg('EBX'),80)));return 0;},1);
 m.hooks.push(m.cpu.hook_add(m.uc.HOOK_CODE,()=>{if(arg(1)===indicator)calls.push([1,0,0,0,0,1,0,0]);},null,0x44b4b0,0x44b4b0));
 const dv=()=>new DataView(c.memory.buffer),read=p=>dv().getUint32(p,true),put=(p,v)=>dv().setUint32(p,v,true),flt=(p,v)=>dv().setFloat32(p,v,true);
 const normalize=(input,base,get)=>{const b=Buffer.from(input),v=8;
  for(const off of [4,16,0x1c]){const p=b.readUInt32LE(v+off);if(p)b.writeUInt32LE((p-base)>>>0,v+off);}
  for(const off of [0x68,0xcc,0x118,0x144,0x190,0x1cc,0x218,0x244,0x270,0x29c,0x38c,0x3b0])b.writeUInt32LE(b.readUInt32LE(v+off)?1:0,v+off);
  const start=get(base+v+0x3a4);for(const off of [0x394,0x3a4,0x3a8]){const p=b.readUInt32LE(v+off);if(p)b.writeUInt32LE((p-start)>>>0,v+off);}
  for(const off of [0x3ac,0x42c,0x430])b.writeUInt32LE(b.readUInt32LE(v+off)?1:0,v+off);
  for(const t of [0x464,0x478,...Array.from({length:11},(_,i)=>0x680+i*52),0x8f0])b.writeUInt32LE(b.readUInt32LE(t+12)?1:0,t+12);return b;
 };
 const loadBullet=(index,type,color,position,angle,active=1,state=1,marker=0)=>{
  const b=Buffer.alloc(0x214);b.writeInt16LE(type);b.writeInt16LE(color,2);position.forEach((x,j)=>b.writeFloatLE(x,4+4*j));b.writeFloatLE(angle,16);b.writeFloatLE(2.25,24);b.writeInt16LE(1,0x1f8);b.writeInt16LE(1,0x1fa);b.writeInt16LE(1,0x1fc);b.writeInt32LE(-1,0x208);
  memory(c,em,b.length).set(b);m.write(nem,b);assert.equal(c.bm_spawn(bf,em,0,0,0),m.call(0x408f20,{args:[manager,nem,0,0,0]})|0);
  const cp=c.bm_bullet(bf,index),np=manager+0x64+index*0x910;
  for(const [off,value]of [[0,(read(cp)&~1)|active],[0x4c0,marker]]){put(cp+off,value);m.u32(np+off,value);}dv().setInt16(cp+0x4b2,state,true);m.write(np+0x4b2,new Uint8Array([state,0]));
 };
 const table=Array.from({length:29},(_,i)=>m.u32(0x4a3ec0+i*208));
 try{
  for(const kind of [5,6,7,8,10,12])for(let sample=0;sample<72;++sample){
   o.reset();for(const k of [1,2]){memory(c,c.bm_data(bf,k),8).fill(0);put(c.bm_data(bf,k),12345);}flt(c.bm_data(bf,0),1);
   m.view(manager+0x64,2000*0x910).fill(0);for(let i=0;i<2000;++i)memory(c,c.bm_bullet(bf,i),0x910).fill(0);m.u32(manager+0x10,manager+0x64);c.bm_configure(bf,0,0,0);
   const e=Buffer.alloc(0x163c);e.writeFloatLE([0,15.3,-34.5][sample%3],0x34);e.writeFloatLE([224,165.4,400][sample%3],0x38);
   for(let j=0;j<12;++j)e.writeInt32LE(j*37+sample,0x124+j*4);
   e.writeInt32LE(sample%2,0x124);e.writeFloatLE([0,32,64,192,256.1,1e-6][sample%6],0x134);e.writeFloatLE(-1.2+sample*.05,0x138);e.writeFloatLE([0,.05,.7,3.1415927,-.1,9][sample%6],0x13c);e.writeFloatLE([0,1.25,-2.75][sample%3],0x140);e.writeFloatLE([0,32,64,256.1,128,32.0001][sample%6],0x1620);
   if(kind===7||kind===8){e.writeInt32LE(2,0x124);e.writeInt32LE(9,0x128);e.writeInt32LE(sample%9,0x158);e.writeFloatLE([0,Math.PI,-Math.PI,Math.PI/2,-Math.PI/2,.37,-2.73,-.77][sample%8],0x134);e.writeFloatLE([0,64,200,400,1000][Math.floor(sample/8)%5],0x138);e.writeFloatLE(sample%5===0?1e5:0,0x1590);
    for(let slot=0;slot<2;++slot){const off=0x374+slot*0x214;e.writeInt16LE(1,off);e.writeInt16LE(2,off+2);e.writeFloatLE(.35,off+16);e.writeFloatLE(.17,off+20);e.writeFloatLE(2.5,off+24);e.writeFloatLE(1.5,off+28);e.writeInt16LE(2,off+0x1f8);e.writeInt16LE(1,off+0x1fa);e.writeInt16LE(sample%9,off+0x1fc);e.writeInt32LE(128,off+0x200);e.writeInt32LE(2,off+0x204);e.writeInt32LE(-1,off+0x208);}e.writeFloatLE(-3.25,0x1420);e.writeFloatLE(7.5,0x1424);
   }
   memory(c,ep,e.length).set(e);m.write(enemy,e);const pp=sample%3===0?[e.readFloatLE(0x34),e.readFloatLE(0x38),0]:[17.25,400,0];pp.forEach((v,j)=>{flt(c.ecb_data(f,5,0)+j*4,v);m.f32(player+0x87c+j*4,v);flt(c.bm_data(bf,3)+j*4,v);});
   for(let i=0;i<96;++i){const type=kind===5?(i%3?table.indexOf(0x22):i%29):kind===6?(i%3?table.indexOf(0x40):i%29):kind===8?(i%3?table.indexOf(0xbf):i%29):1;
    const px=e.readFloatLE(0x34),py=e.readFloatLE(0x38),r=kind===5?e.readFloatLE(0x134)*.5:e.readFloatLE(0x1620)-32;
    const position=[[px,py,0],[px+r,py,0],[px+r*.99999,py,0],[px,py+r*1.00001,0],[px+r*.6,py+r*.8,0],[i*4-192,i*4,0]][i%6];
    const angle=[0,e.readFloatLE(0x138),e.readFloatLE(0x138)+e.readFloatLE(0x13c),e.readFloatLE(0x134)-e.readFloatLE(0x13c),-3.1415927,3.1415927][i%6];
    loadBullet(i,type,[5,6,8].includes(kind)?0:[2,6,13,0,1,3][i%6],position,angle,i%13?1:0,i%11?1:2,(i+sample)%3?0:1);
   }
   events=[];calls=[];spawns=[];c.bm_configure(bf,0,96,0);
   const expected=m.call(({5:0x418290,6:0x418460,7:0x418520,8:0x4188c0,10:0x4189d0,12:0x418d20})[kind],{ecx:enemy,limit:20000000})|0;assert.equal(c.ecb_run(f,kind,0),expected,`callback ${kind}/${sample}`);
   assert.equal(c.bm_value(bf,0),(m.u32(manager+0x10)-manager-0x64)/0x910,`pool cursor ${kind}/${sample}`);assert.equal(m.f32(manager+0x60),0);
   assert.equal(firstDifference(Buffer.from(memory(c,ep,e.length)),Buffer.from(m.bytes(enemy,e.length))),'',`enemy state ${kind}/${sample}`);
   for(let i=0;i<Math.max(96,c.bm_value(bf,0));++i){const cp=c.bm_bullet(bf,i),np=manager+0x64+i*0x910;assert.equal(firstDifference(normalize(memory(c,cp,0x910),cp,read),normalize(m.bytes(np,0x910),np,p=>m.u32(p))),'',`callback ${kind}/${sample} bullet ${i}`);++checks;}
   const soundData=c.bm_data(bf,4),v=dv(),actualEvents=Array.from({length:c.bm_value(bf,3)},(_,j)=>[v.getInt32(soundData+j*16,true),v.getInt32(soundData+j*16+4,true),v.getFloat32(soundData+j*16+8,true),v.getFloat32(soundData+j*16+12,true)]);assert.deepEqual(actualEvents,events,`effects ${kind}/${sample}`);transformations+=events.filter(v=>v[0]===1).length;
   const cp=c.ecb_data(f,3,0),view=dv(),actualCalls=Array.from({length:c.ecb_count(f,0)},(_,i)=>{const p=cp+i*32;return[view.getInt32(p,true),view.getInt32(p+4,true),...Array.from({length:6},(_,j)=>view.getFloat32(p+8+j*4,true))];});assert.deepEqual(actualCalls,calls,`world calls ${kind}/${sample}`);
   assert.deepEqual(Buffer.from(memory(c,c.ecb_data(f,4,0),c.ecb_count(f,1)*80)),Buffer.concat(spawns),`spawn parameters ${kind}/${sample}`);
   for(const k of [1,2])assert.deepEqual(Buffer.from(memory(c,c.bm_data(bf,k),8)),Buffer.from(m.bytes(k===1?0x4c2f00:0x4c2ef8,8)),`RNG ${kind}/${sample}`);++cases;
  }
  report('enemy-callbacks-bullets',{passed:true,cases,checks,transformations,scope:'Original callbacks 5/6/7/8/10/12: complete bullet state and actual emissions/ANM execution, boundary emitter coordinates, cancellation/effect order, both RNG streams, requested player motion, spawn script/parameters, indicator-update ordering. Player motion, indicator resource and spawned enemy execution are integration boundaries.'});
 }finally{c.ecb_delete(f);c.release(data);c.release(em);m.close();}
});
