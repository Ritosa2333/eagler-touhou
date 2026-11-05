import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,oracle,memory,report,root} from './helpers.mjs';
import {animationOracle} from './anm-oracle.mjs';
import {normalizeAnimation,firstDifference} from './shot-oracle.mjs';
test('TH11 converted laser shots match original static record, callbacks and pool lifetime',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m);let frames=0,checks=0,spawns=0;let sounds=[];
 m.replace(0x44a260,'sound output boundary',()=>{sounds.push([m.reg('EDI')|0,m.f32(m.reg('ESP')+4)]);return 0;},1);
 const dv=()=>new DataView(c.memory.buffer),put=(p,v)=>dv().setInt32(p,v,true),flt=(p,v)=>dv().setFloat32(p,v,true),read=p=>dv().getUint32(p,true);
 // These values come from the locked original PE's static ShotSpec, not SHT files.
 const spec=Buffer.from(m.bytes(0x4a3a50,0x34));
 assert.equal(spec.toString('hex'),'1e00140000000000000000000000c0410000c0410000000000004041000004000500180030504300805043000000000000000000');
 try{for(const name of ['pl00a','pl00b','pl00c','pl01a','pl01b','pl01c']){
  const {source,original}=o.load(name.slice(0,4)+'.anm'),a=c.anm_create(),ad=c.allocate(source.length);memory(c,ad,source.length).set(source);assert.equal(c.anm_open(a,ad,source.length),1);
  const bytes=readFileSync(resolve(root,'reference/assets',name+'.sht')),sht=c.sht_create(),sd=c.allocate(bytes.length);memory(c,sd,bytes.length).set(bytes);assert.equal(c.sht_open(sht,sd,bytes.length),1);
  const np=m.allocate(0x8d40),ne=m.allocate(0x2678),nem=m.allocate(0x100),special=m.allocate(0x100),hud=m.allocate(0x4460),origin=m.allocate(12),heap=m.heap;
  for(let scenario=0;scenario<14;++scenario){
   m.heap=heap;m.view(heap,4000000).fill(0);for(const [p,n]of [[np,0x8d40],[ne,0x2678],[nem,0x100],[special,0x100],[hud,0x4460]])m.view(p,n).fill(0);o.reset();sounds=[];
   m.u32(0x4a8eb4,np);m.u32(np+0x10,original);m.u32(0x4a8d7c,nem);m.u32(0x4a8d64,special);m.u32(0x4a8d84,hud);
   const manager=c.anm_manager_create(),f=c.sm_create(sht,a,manager),p=c.sm_player(f),enemy=c.sm_enemy(f,1),rate=c.anm_env_rate(manager);
   const speed=[.5,1,1.5][scenario%3];flt(rate,speed);m.f32(0x4a7948,speed);for(const vis of[0,1]){memory(c,c.anm_env_rng(manager,vis),8).fill(0);put(c.anm_env_rng(manager,vis),12345);}
   flt(p,0);flt(p+4,300);flt(p+8,.25);m.write(origin,memory(c,p,12));put(p+184,1);
   const target=scenario!==0,tx=[0,-224,224,-224.01,224.01,75,-75][scenario%7];
   m.u32(np+0x7c94,target?ne:0);m.f32(ne+0x1070,tx);m.f32(ne+0x1074,170);flt(enemy+0x1070,tx);flt(enemy+0x1074,170);
   const count=scenario===13?257:1;
   const compare=label=>{
    for(let i=0;i<Math.min(count,256);++i){const actual=Buffer.from(memory(c,c.sm_shot(f,i),0x6c)),expected=Buffer.from(m.bytes(np+0x96c+i*0x6c,0x6c));for(const b of[actual,expected]){b.writeUInt32LE(0,12);for(const off of[0x54,0x68])b.writeUInt32LE(b.readUInt32LE(off)?1:0,off);}assert.equal(firstDifference(actual,expected),'',name+' scenario'+scenario+' '+label+' shot'+i);++checks;}
    const native=o.states();assert.equal(c.anm_manager_count(manager),native.length,label+' animations');for(const item of native){const cp=c.anm_manager_find(manager,item.id);assert.ok(cp);assert.equal(firstDifference(normalizeAnimation(memory(c,cp,0x434),read,read(cp+0x3a4)),normalizeAnimation(item.bytes,p=>m.u32(p),m.u32(item.p+0x3a4))),'',name+' '+label+' animation'+item.id);++checks;}
    for(const vis of[0,1])assert.deepEqual(Buffer.from(memory(c,c.anm_env_rng(manager,vis),8)),Buffer.from(m.bytes(vis?0x4c2ef8:0x4c2f00,8)),label+' RNG');
   };
   try{
    for(let i=0;i<count;++i){const angle=Math.fround([-1.57,.3,3.1415927,-3.1415927][(scenario+i)%4]);m.f32(0x4a3a64,angle);m.reg('EAX',0x4a3a50);m.call(0x433f90,{ecx:origin,args:[np,0]});assert.equal(c.sm_converted(f,p,angle,target?1:0),i<256?i:-1);++spawns;}compare('spawn');
    const soundp=c.sm_sounds(f);assert.deepEqual(Array.from({length:c.sm_sound_count(f)},(_,i)=>[dv().getInt32(soundp+i*8,true),dv().getFloat32(soundp+i*8+4,true)]),sounds);
    for(let frame=0;frame<(scenario===13?2:150);++frame){
     if(frame===2&&scenario>=6&&scenario<=9){const flags=[1,0x20,0x400000,0x800000][scenario-6];put(enemy+0x25bc,flags);m.u32(ne+0x25bc,flags);}
     if(frame===1&&scenario>=10&&scenario<=12){const s=c.sm_shot(f,0),n=np+0x96c;const age=scenario===10?119:240;put(s+4,age);flt(s+8,age);m.i32(n+4,age);m.f32(n+8,age);if(scenario===12){put(s+0x48,2);m.i32(n+0x48,2);}}
     m.call(0x434440,{args:[np],limit:3000000});assert.equal(c.sm_update(f),1,'error '+c.sm_error(f));compare('tick'+frame);
     for(const overlay of[0,1]){o.update(overlay);assert.equal(c.anm_manager_update(manager,overlay),1);}compare('animation'+frame);++frames;
    }
   }finally{c.sm_delete(f);c.anm_manager_delete(manager);}
  }
  c.sht_delete(sht);c.release(sd);c.anm_delete(a);c.release(ad);
 }
 report('converted-laser',{passed:true,frames,checks,spawns,scope:'Original PE record 4a3a50, spawn 433f90/435030, update 434440/435080, six player ANM resources, real animation interpreter, target boundaries and invalid flags, half/normal/fast time, 120/240-frame branches, impacted state, sound events and full 256-slot pool. Laser-cut adapters and full bombs remain separate integration gates.'});
 }finally{m.close();}
});
