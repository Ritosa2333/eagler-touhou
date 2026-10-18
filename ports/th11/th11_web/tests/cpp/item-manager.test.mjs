import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';import {animationOracle} from './anm-oracle.mjs';import {bits} from './ecl-bytecode.mjs';
test('TH11 item pool, spawn, cancellation delays, motion and attraction match native frames',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m),{source,original}=o.load('bullet.anm');
 const data=c.allocate(source.length),pos=c.allocate(12),drop=c.allocate(60);memory(c,data,source.length).set(source);
 const nm=m.allocate(0x265e70),player=m.allocate(0x8c40),sht=m.allocate(32),hud=m.allocate(0x4460),bm=m.allocate(0x46d700),np=m.allocate(12),nd=m.allocate(60),baseHeap=m.heap;
 m.u32(0x4a8e90,nm);m.u32(0x4a8eb4,player);m.u32(0x4a8d84,hud);m.u32(0x4a8d68,bm);m.u32(bm+0x46d674,original);m.u32(player+0x92c,sht);
 // Effects/audio are recorded boundaries. Item spawn/update and real ANM execute.
 let events=[];m.replace(0x455b10,'item effect sink',()=>{const sp=m.reg('ESP'),v=m.reg('EAX');events.push([1,m.u32(sp+12),m.f32(v),m.f32(v+4)]);return 0;},4);
 m.replace(0x44a1e0,'nonpositional item sound',()=>{events.push([0,m.reg('ESI'),m.f32(np),0]);return 0;});
 m.replace(0x44a260,'positional item sound',()=>{events.push([0,m.reg('EDI'),m.f32(m.reg('ESP')+4),0]);return 0;},1);
 const get=a=>new DataView(c.memory.buffer).getUint32(a,true),put=(a,v)=>new DataView(c.memory.buffer).setUint32(a,v>>>0,true),float=(a,v)=>new DataView(c.memory.buffer).setFloat32(a,v,true);
 const normalize=(data,read)=>{const b=Buffer.from(data);for(const off of [4,16,0x68,0xcc,0x118,0x144,0x190,0x1cc,0x218,0x244,0x270,0x29c,0x38c,0x3b0,0x45c])b.writeUInt32LE(0,off);const base=b.readUInt32LE(0x3a4);for(const off of [0x394,0x3a4,0x3a8])if(b.readUInt32LE(off))b.writeUInt32LE((b.readUInt32LE(off)-base)>>>0,off);b.writeUInt32LE(b.readUInt32LE(0x3ac)?1:0,0x3ac);return b;};
 const compare=(a,b,label)=>{if(!a.equals(b)){let off=0;while(a[off]===b[off])++off;off&=~3;assert.fail(`${label} +0x${off.toString(16)} cpp=${a.readUInt32LE(off).toString(16)} native=${b.readUInt32LE(off).toString(16)}`);}};
 let f=0,checks=0,frames=0,spawnCount=0,scatterCount=0;
 const point=(x,y,z=0)=>{for(const [i,v] of [x,y,z].entries()){float(pos+i*4,v);m.f32(np+i*4,v);}};
 const spawn=(type,x,y,angle=-1.5707963705062866,speed=2.2)=>{point(x,y);const color=0xfedcab89;m.reg('EAX',np);m.call(0x424230,{ecx:type,edx:nm,args:[color,bits(angle),bits(speed)]});assert.equal(c.im_spawn(f,type,pos,color,angle,speed),0,'spawn type '+type);++spawnCount;};
 const check=label=>{for(let i=0;i<2198;++i){const a=c.im_item(f,i),b=nm+0x14+i*0x478;if(!get(a+0x464)&&!m.u32(b+0x464))continue;compare(normalize(memory(c,a,0x478),get),normalize(m.bytes(b,0x478),a=>m.u32(a)),label+' item '+i);++checks;}for(const [k,off]of [[0,0x265e64],[1,0x265e68],[2,0x265e6c]])assert.equal(c.im_value(f,k),m.u32(nm+off),label+' counter '+k);for(let k=0;k<2;++k)assert.deepEqual(Buffer.from(memory(c,c.im_data(f,k+1),8)),Buffer.from(m.bytes(k?0x4c2ef8:0x4c2f00,8)),label+' rng');};
 try{for(let sample=0;sample<12;++sample){
  if(f)c.im_delete(f);f=c.im_create();assert.equal(c.im_load(f,data,source.length),1);m.view(nm,0x265e70).fill(0);m.heap=baseHeap;m.view(baseHeap,1000000).fill(0);o.reset();events=[];
  for(let k=0;k<2;++k){memory(c,c.im_data(f,k+1),8).fill(0);new DataView(c.memory.buffer).setUint16(c.im_data(f,k+1),12345,true);}
  const rate=[1,.5,1.25][sample%3];float(c.im_data(f,3),rate);m.f32(0x4a7948,rate);
  const cp=c.im_data(f,0);m.i32(0x4a56e8,sample%2?400:0);m.i32(0x4a5748,400);put(cp+76,sample%2?400:0);put(cp+80,400);
  for(let type=1;type<=11;++type)if(type!==8)spawn(type,-230+type*41,20+type*13,.1*type-2,2.2);
  for(let i=0;i<1200;++i)spawn(8,(i%37-18)*8,60+Math.floor(i/37)*8,-1.5707963705062866,.6);check(`sample ${sample} spawn`);
  // Scatter includes the initial RNG draw even for an empty drop list.
  for(let n=0;n<3;++n){const d=Buffer.alloc(60);for(let i=0;i<12;++i)d.writeInt32LE(n===0?0:(i+n)%4,4+i*4);d.writeFloatLE(31.5,52);d.writeFloatLE(17.25,56);memory(c,drop,60).set(d);m.write(nd,d);point(-18.5,143.75,3);m.reg('EDI',np);m.call(0x4101d0,{args:[nd]});assert.equal(c.im_scatter(f,drop,pos),1);assert.deepEqual(Buffer.from(memory(c,drop,60)),Buffer.from(m.bytes(nd,60)));++scatterCount;check(`sample ${sample} scatter ${n}`);}
  for(let frame=0;frame<135;++frame){
   const state=sample%3===0?2:frame>80?4:1,y=sample%4===0?90:400,focused=frame%2,communication=sample%2?10000:0,force=frame>=55&&frame<62;
   float(cp,0);float(cp+4,y);put(cp+12,state);float(cp+16,8);m.f32(player+0x87c,0);m.f32(player+0x880,y);m.i32(player+0x928,state);m.f32(sht+8,8);
   for(const [offset,nativeOffset,size]of [[20,0x8bcc,-1],[36,0x8be4,40],[52,0x8bfc,28]]){for(const [j,v]of [-size,y-size,size,y+size].entries()){float(cp+offset+j*4,v);m.f32(player+nativeOffset+(j>=2?j*4+4:j*4),v);}}
   memory(c,cp+68,1)[0]=focused;memory(c,cp+69,1)[0]=force;put(cp+72,communication);m.u32(0x4c93c0,focused?8:0);m.u32(hud+0x4438,force?1:0);m.i32(0x4a56f4,communication);
   if(frame===65&&sample%2===0){c.im_attract(f);m.call(0x4245e0);}
   m.resetThreadFPU();m.call(0x423550,{args:[nm],limit:20000000});assert.equal(c.im_update(f),1,`sample ${sample} frame ${frame} error ${c.im_value(f,3)}`);check(`sample ${sample} frame ${frame}`);++frames;
  }
 }
 report('item-manager',{passed:true,checks,frames,spawnCount,scatterCount,scope:'Original pooled item creation, RNG scatter, delayed cancel stars, falling/attraction, real bullet.anm; excludes pickup reward logic and draw submissions.'});
 }finally{if(f)c.im_delete(f);c.release(data);c.release(pos);c.release(drop);m.close();}
});
