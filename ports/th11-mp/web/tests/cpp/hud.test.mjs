import test from 'node:test';
import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';
import {animationOracle} from './anm-oracle.mjs';

test('TH11 HUD life fragments, communication, boss timers and indicator match original callbacks',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m);
 const front=o.load('front.anm'),text=o.load('ascii.anm',true),logo=o.load('st01logo.anm',true);
 m.u32(front.original,5);m.u32(text.original,2);m.u32(logo.original,27);
 const hud=m.allocate(0x4460),enemy=m.allocate(0x100),boss=m.allocate(0x2678),player=m.allocate(0x1000),spell=m.allocate(0x1000),supervisor=m.allocate(0x19000),control=m.allocate(0x80),msg=m.allocate(0xac);
 for(const [address,value]of [[0x4a8d84,hud],[0x4a8d7c,enemy],[0x4a8eb4,player],[0x4a8d6c,spell],[0x4a8d58,supervisor],[0x4a8e88,control]])m.u32(address,value);
 m.u32(supervisor+0x184ac,text.original);m.u32(msg+0x10,0x4a7948);
 let sounds=[];m.replace(0x44a1e0,'record countdown sound',()=>{sounds.push(m.reg('ESI'));return 0;});
 m.replace(0x41d380,'keep dialogue active at HUD boundary',()=>0,1);
 const heap=m.heap,dv=()=>new DataView(c.memory.buffer),put=(p,v)=>dv().setInt32(p,v,true),get=p=>dv().getInt32(p,true),flt=(p,v)=>dv().setFloat32(p,v,true);
 function norm(bytes,read){const b=Buffer.from(bytes);for(const off of [0x68,0xcc,0x118,0x144,0x190,0x1cc,0x218,0x244,0x270,0x29c,0x38c,0x3b0])b.writeUInt32LE(0,off);
  const base=b.readUInt32LE(0x3a4);for(const off of [0x394,0x3a4,0x3a8]){const p=b.readUInt32LE(off);if(p)b.writeUInt32LE((p-base)>>>0,off);}
  for(const off of [4,16,0x1c]){const p=b.readUInt32LE(off);if(p)b.writeUInt32LE(read(p),off);}
  for(const off of [8,12,20,24]){const p=b.readUInt32LE(off);if(p)b.writeUInt32LE(read(read(p)),off);}
  for(const off of [0x3ac,0x400,0x410,0x428,0x42c])b.writeUInt32LE(b.readUInt32LE(off)?1:0,off);
  b.writeUInt32LE(0,0x414);return b;
 }
 let frames=0,checks=0,cases=0;
 try{for(let st=1;st<=7;++st)for(const rate of [1,.375]){
  m.heap=heap;o.reset();m.f32(0x4a7948,rate);
  for(const [p,n]of [[hud,0x4460],[enemy,0x100],[boss,0x2678],[player,0x1000],[spell,0x1000],[control,0x80]])m.view(p,n).fill(0);
  m.u32(hud+0x444c,front.original);m.u32(hud+0x43ec,logo.original);
  m.i32(hud+0x4440,-1);m.i32(hud+0x4448,-1);m.i32(hud+0x43c8,-1);m.u32(hud+0x43d4,0x4a7948);m.u32(hud+0x43d8,1);
  m.i32(0x4a5728,st);m.i32(0x4a5720,st%5);m.u32(0x4a5758,0);m.u32(0x4c37d8,0);m.u32(0x4c37e4,1);m.i32(0x4a573c,0);m.i32(0x4a5718,2);m.i32(0x4a571c,0);
  const f=c.hud_create(),e=c.hud_data(f,0),input=c.hud_data(f,1),b=c.hud_data(f,2),state=c.hud_data(f,3),manager=c.hud_data(f,12);
  try{
   for(const [i,file]of [front,text,logo].entries()){const p=c.allocate(file.source.length);memory(c,p,file.source.length).set(file.source);assert.equal(c.hud_load(f,i,p,file.source.length),1);c.release(p);}
   flt(c.hud_data(f,9),rate);put(e+24,st%5);put(input+16,st);
   for(const v of [0,1]){memory(c,c.anm_env_rng(manager,v),8).fill(0);dv().setUint16(c.anm_env_rng(manager,v),12345,true);}
   assert.equal(c.hud_start(f,0,1),1);m.call(0x41a6d0);
   function compare(label){
    for(const [k,offset,n]of [[4,0x10,9],[5,0x25e4,2],[6,0x2e4c,4],[7,0x3f1c,1]])for(let i=0;i<n;++i){
     const a=norm(memory(c,c.hud_data(f,k)+i*0x434,0x434),p=>get(p)>>>0),b=norm(m.bytes(hud+offset+i*0x434,0x434),p=>m.u32(p));
     let off=-1;for(let j=0;j<a.length;j+=4)if(a.readUInt32LE(j)!==b.readUInt32LE(j)){off=j;break;}
     assert.equal(off,-1,`${label} VM ${k}/${i} offset=${off.toString(16)} actual=${off<0?'':a.readUInt32LE(off).toString(16)} expected=${off<0?'':b.readUInt32LE(off).toString(16)}`);++checks;
    }
    assert.equal(get(state)>>>0,m.u32(hud+0x4420),label+' HUD flags');assert.equal(c.hud_value(f,0)|0,m.i32(hud+0x4448),label+' seconds');
    assert.equal(c.hud_health(f,0),m.f32(hud+0x43f0),label+' health interpolation');assert.equal(c.hud_health(f,1),m.f32(hud+0x43f4),label+' target health');
    const native=o.states();assert.equal(c.anm_manager_count(manager),native.length,label+' manager count');
    for(const vm of native){const p=c.anm_manager_find(manager,vm.id);assert.ok(p,label+' missing VM '+vm.id);assert.equal(get(p+0x3a0)>>>16,m.u32(vm.p+0x3a0)>>>16,label+' label script');}
    for(const v of [0,1])assert.deepEqual(Buffer.from(memory(c,c.anm_env_rng(manager,v),8)),Buffer.from(m.bytes(v?0x4c2ef8:0x4c2f00,8)),label+' RNG');
   }
   compare(`stage${st} rate${rate} init`);
   for(let frame=0;frame<150;++frame){
    const label=`stage${st} rate${rate} frame${frame}`;
    if(frame%10===0){const count=Math.floor(frame/10)%10,fragments=Math.floor(frame/10)%5;c.hud_lives(f,count,fragments);m.reg('EDI',hud);m.call(0x41a060,{edx:count,args:[fragments]});}
    const section=frame<60?1:24,active=frame>=20&&frame<140,seconds=frame<80?15-Math.floor(frame/5):30-Math.floor(frame/5);
    const x=[-100,-64,-1,0,1,100][Math.floor(frame/5)%6],y=[430,416,410,400,399,64,65,80,81,0][Math.floor(frame/3)%10];
    const comm=[0,9000,9999,10000,10500,11000,11500,12000][Math.floor(frame/6)%8];
    const hidden=frame>=130?1:0,dialogue=frame>=100&&frame<105,sp=frame>=70;
    flt(input,x);flt(input+4,y);put(input+20,section);put(input+24,seconds);memory(c,input+13,2).set([+dialogue,+sp]);
    put(e+12,comm);c.hud_boss(f,+active);put(c.hud_data(f,10),hidden);put(c.hud_data(f,11),Math.floor(frame/15)%11);
    const bx=[-193,-192,-160,-10,0,10,160,192,193][frame%9],hp=1000-frame*5,threshold=[2500,1500,700,400,200,199,399,1000][Math.floor(frame/4)%8];
    flt(b+0x34,bx);put(b+0x14f0,hp);put(b+0x14f4,1000);put(b+0x14f8,threshold);put(b+0x1580,frame>=125?0x20:0);
    m.f32(player+0x87c,x);m.f32(player+0x880,y);m.i32(0x4a56f4,comm);m.i32(0x4a5730,section);
    m.u32(enemy+0x1c,active?boss:0);m.u32(enemy+0x3c,hidden);m.u32(spell+0x8e0,+sp);m.u32(hud+0x4438,dialogue?msg:0);m.i32(hud+0x4440,seconds);m.i32(hud+0x43fc,Math.floor(frame/15)%11);
    m.f32(boss+0x1070,bx);m.i32(boss+0x252c,hp);m.i32(boss+0x2530,1000);m.i32(boss+0x2534,threshold);m.u32(boss+0x25bc,frame>=125?0x20:0);
    sounds=[];m.call(0x41b380,{args:[hud]});o.update(false);o.update(true);assert.equal(c.hud_update(f),1,label+' update');
    compare(label);assert.deepEqual(Array.from({length:c.hud_value(f,2)},(_,i)=>get(c.hud_data(f,13)+i*4)),sounds,label+' sounds');++frames;
   }
   ++cases;
  }finally{c.hud_delete(f);}
 }
 report('hud',{passed:true,cases,frames,checks,scope:'Original 41a6d0 setup, 41a060 lives and complete 41b380 update with real shipped ANM scripts; all seven stages, ordinary and slow rates, fragments, communication thresholds, boss-name/star lifetimes, countdown sounds, indicator hysteresis. Dialogue is held at its separately tested boundary; glyph rendering is a separate gate.'});
 }finally{m.close();}
});
