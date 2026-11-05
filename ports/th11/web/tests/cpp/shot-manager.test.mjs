import test from 'node:test';import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,report,root} from './helpers.mjs';import {animationOracle} from './anm-oracle.mjs';
function normalizeAnimation(bytes,read,base){const b=Buffer.from(bytes);
 for(const off of [0x68,0xcc,0x118,0x144,0x190,0x1cc,0x218,0x244,0x270,0x29c,0x38c,0x3b0])b.writeUInt32LE(0,off);
 for(const off of [4,16,0x1c]){const p=b.readUInt32LE(off);if(p)b.writeUInt32LE(read(p),off);}
 for(const off of [8,12,20,24]){const p=b.readUInt32LE(off);if(p)b.writeUInt32LE(read(read(p)),off);}
 for(const off of [0x394,0x3a4,0x3a8]){const p=b.readUInt32LE(off);if(p)b.writeUInt32LE((p-base)>>>0,off);}
 for(const off of [0x3ac,0x400,0x410,0x428,0x42c])b.writeUInt32LE(b.readUInt32LE(off)?1:0,off);
 b.writeUInt32LE(0,0x414);return b;
}
function firstDifference(a,b){for(let off=0;off<a.length;off+=4)if(a.readUInt32LE(off)!==b.readUInt32LE(off))return `0x${off.toString(16)} actual=0x${a.readUInt32LE(off).toString(16)} expected=0x${b.readUInt32LE(off).toString(16)}`;return '';}
test('TH11 shot pool, character callbacks and animations follow the original for all six shot resources',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m);let states=0,animations=0,frames=0,spawns=0,scenarios=0;
 let sounds=[];m.replace(0x44a260,'shot sound sink',()=>{sounds.push([m.reg('EDI')|0,m.f32(m.reg('ESP')+4)]);return 0;},1);
 const spawnCallbacks=[0,0x434e20,0x435210,0x435250],updateCallbacks=[0,0x434e30,0x4352a0,0x435330];
 const dv=()=>new DataView(c.memory.buffer),put=(p,v)=>dv().setInt32(p,v,true),flt=(p,v)=>dv().setFloat32(p,v,true),read=p=>dv().getUint32(p,true);
 try{for(const [characterIndex,name]of ['pl00a','pl00b','pl00c','pl01a','pl01b','pl01c'].entries()){
  if(process.env.TH11_SHOT_FILE&&name!==process.env.TH11_SHOT_FILE)continue;
  const {source:anmBytes,original}=o.load(name.slice(0,4)+'.anm'),a=c.anm_create(),ad=c.allocate(anmBytes.length);memory(c,ad,anmBytes.length).set(anmBytes);assert.equal(c.anm_open(a,ad,anmBytes.length),1);
  const bytes=readFileSync(resolve(root,'reference/assets',name+'.sht')),sht=c.sht_create(),data=c.allocate(bytes.length);memory(c,data,bytes.length).set(bytes);assert.equal(c.sht_open(sht,data,bytes.length),1);
  const ns=m.allocate(bytes.length);m.write(ns,bytes);const groups=bytes.readUInt16LE(2);
  for(let g=0;g<groups;++g){let off=bytes.readUInt32LE(0x268+g*8);m.u32(ns+0x268+g*8,ns+off);while(bytes.readInt8(off)>=0){m.u32(ns+off+0x24,spawnCallbacks[bytes.readUInt32LE(off+0x24)]);m.u32(ns+off+0x28,updateCallbacks[bytes.readUInt32LE(off+0x28)]);off+=0x34;}}
  const np=m.allocate(0x8d40),ne=m.allocate(0x2678),nem=m.allocate(0x100),special=m.allocate(0x100),hud=m.allocate(0x4460),heap=m.heap;
  for(let g=0;g<groups;++g){if(process.env.TH11_SHOT_GROUP&&g!==+process.env.TH11_SHOT_GROUP)continue;
   m.heap=heap;m.view(heap,4000000).fill(0);for(const [p,n]of [[np,0x8d40],[ne,0x2678],[nem,0x100],[special,0x100],[hud,0x4460]])m.view(p,n).fill(0);o.reset();sounds=[];
   m.u32(0x4a8eb4,np);m.u32(np+0x10,original);m.u32(np+0x92c,ns);m.u32(0x4a8d7c,nem);m.u32(0x4a8d64,special);m.u32(0x4a8d84,hud);
   const manager=c.anm_manager_create(),f=c.sm_create(sht,a,manager),p=c.sm_player(f),enemy=c.sm_enemy(f,1),rate=c.anm_env_rate(manager),timer=c.sm_timer(f);
   const seed=(g*9817+12345)&65535;for(const vis of [0,1]){memory(c,c.anm_env_rng(manager,vis),8).fill(0);put(c.anm_env_rng(manager,vis),seed);m.u32(vis?0x4c2ef8:0x4c2f00,seed);}
   const speed=[1,.5,1.5][g%3];flt(rate,speed);m.f32(0x4a7948,speed);c.timer_set(timer,0,rate);m.reg('EAX',np+0x930);m.call(0x406100,{args:[0]});
   m.i32(np+0x7c90,8);put(p+140,8);put(p+184,1);
   m.u32(nem+0x68,ne+0x119c);m.u32(ne+0x119c,ne);m.f32(ne+0x1070,3);m.f32(ne+0x1074,170);flt(enemy+0x1070,3);flt(enemy+0x1074,170);
   for(let i=0;i<8;++i){const option=c.anm_manager_spawn(manager,a,0,22,0),id=read(option);assert.equal(o.spawn(original,0,22),id);
    const native=o.states().find(s=>s.id===id).p;flt(option+0x3c4,-1.8+i*.06);m.f32(native+0x3c4,-1.8+i*.06);
    const x=(-4+i)*1000+g,y=39000+i*300-g,angle=-1.5707963+i*.025;
    put(p+12+i*16,x);put(p+16+i*16,y);flt(p+20+i*16,angle);put(p+24+i*16,id);
    m.i32(np+(i+1)*0xe4+0x74e8,x);m.i32(np+(i+1)*0xe4+0x74ec,y);m.f32(np+(i+1)*0xe4+0x7534,angle);m.u32(np+(i+1)*0xe4+0x753c,id);
   }
   const start=bytes.readUInt32LE(0x268+g*8);flt(p,-23.375);flt(p+4,310.25);flt(p+8,1.5);m.f32(np+0x87c,-23.375);m.f32(np+0x880,310.25);m.f32(np+0x884,1.5);
   function compare(label){
    for(let i=0;i<256;++i){const b=Buffer.from(memory(c,c.sm_shot(f,i),0x6c)),a=Buffer.from(m.bytes(np+0x96c+i*0x6c,0x6c));if(!a.readUInt32LE(0x68)&&!b.readUInt32LE(0x68))continue;for(const v of [a,b]){v.writeUInt32LE(0,12);for(const off of [0x54,0x68])v.writeUInt32LE(v.readUInt32LE(off)?1:0,off);}assert.equal(firstDifference(b,a),'',`${name} group${g} ${label} shot${i}`);++states;}
    assert.deepEqual(Buffer.from(memory(c,c.sm_lasers(f),36)),Buffer.from(m.bytes(np+0x8b8c,36)),label+' lasers');
    const native=o.states();assert.equal(c.anm_manager_count(manager),native.length,label+' animation count');
    for(const item of native){const cp=c.anm_manager_find(manager,item.id);assert.ok(cp);const b=normalizeAnimation(memory(c,cp,0x434),read,read(cp+0x3a4)),a=normalizeAnimation(item.bytes,p=>m.u32(p),m.u32(item.p+0x3a4));assert.equal(firstDifference(b,a),'',`${name} group${g} ${label} animation${item.id}`);++animations;}
    for(const vis of [0,1])assert.deepEqual(Buffer.from(memory(c,c.anm_env_rng(manager,vis),8)),Buffer.from(m.bytes(vis?0x4c2ef8:0x4c2f00,8)),label+' RNG');
   }
   try{
    for(let i=0;i<c.sht_count(sht,g);++i){m.reg('EAX',ns+start+i*0x34);m.call(0x433f90,{ecx:np+0x87c,args:[np,0]});assert.notEqual(c.sm_spawn(f,c.sht_shots(sht,g)+i*0x34,0,p),-2,`${name} g${g} i${i} spawn error ${c.sm_error(f)}`);++spawns;}compare('spawn');
    const soundBase=c.sm_sounds(f),actualSounds=Array.from({length:c.sm_sound_count(f)},(_,i)=>[dv().getInt32(soundBase+i*8,true),dv().getFloat32(soundBase+i*8+4,true)]);assert.deepEqual(actualSounds,sounds);
    for(let frame=0;frame<90;++frame){
     const x=3+Math.sin(frame*.07)*45,y=170+frame*.3;flt(enemy+0x1070,x);flt(enemy+0x1074,y);m.f32(ne+0x1070,x);m.f32(ne+0x1074,y);
     if(frame===18){put(enemy+0x25bc,1);m.u32(ne+0x25bc,1);}if(frame===23){put(enemy+0x25bc,0);m.u32(ne+0x25bc,0);}
     if(frame===32){put(p+188,1);m.i32(special+0x3c,1);}if(frame===45){c.timer_set(timer,-1,rate);m.reg('EAX',np+0x930);m.call(0x406100,{args:[-1]});}
     m.call(0x434440,{args:[np],limit:3000000});assert.equal(c.sm_update(f),1,`${name} ${g} frame${frame} error${c.sm_error(f)}`);
     compare('update'+frame);
     for(const overlay of [0,1]){o.update(overlay);assert.equal(c.anm_manager_update(manager,overlay),1);}
     compare('animation'+frame);++frames;
    }++scenarios;
   }finally{c.sm_delete(f);c.anm_manager_delete(manager);}
  }c.anm_delete(a);c.release(ad);c.sht_delete(sht);c.release(data);console.log(name+' shot groups verified');
 }report('shot-manager',{passed:true,scenarios,spawns,frames,states,animations,scope:'Original shot construction and update, real ANM interpreter/manager and RNG streams; six shipped shot tables, target motion/invalidations, laser release, special acceleration and offscreen removal. Enemy hit/damage resolution and full player input are separate.'});
 }finally{m.close();}
});
