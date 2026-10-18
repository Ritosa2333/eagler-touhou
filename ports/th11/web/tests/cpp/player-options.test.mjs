import test from 'node:test';import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,report,root} from './helpers.mjs';import {animationOracle} from './anm-oracle.mjs';
import {normalizeAnimation,firstDifference} from './shot-oracle.mjs';
test('TH11 option reconstruction matches original power transitions and all six formations',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m);let cases=0,options=0,animations=0;
 const dv=()=>new DataView(c.memory.buffer),put=(p,v)=>dv().setInt32(p,v,true),flt=(p,v)=>dv().setFloat32(p,v,true),read=p=>dv().getUint32(p,true);
 try{for(const [combination,name]of ['pl00a','pl00b','pl00c','pl01a','pl01b','pl01c'].entries()){
  const resource=o.load(name.slice(0,4)+'.anm'),anm=c.anm_create(),ad=c.allocate(resource.source.length);memory(c,ad,resource.source.length).set(resource.source);assert.equal(c.anm_open(anm,ad,resource.source.length),1);
  const bytes=readFileSync(resolve(root,'reference/assets',name+'.sht')),sht=c.sht_create(),sd=c.allocate(bytes.length);memory(c,sd,bytes.length).set(bytes);assert.equal(c.sht_open(sht,sd,bytes.length),1);
  const np=m.allocate(0x8d40),ns=m.allocate(bytes.length);m.write(ns,bytes);const heap=m.heap,max=bytes.readInt32LE(0x20),step=bytes.readInt32LE(0x24);
  for(const focused of [0,1])for(const mode of combination===4?[0,1,2,3,4]:[0]){
   m.heap=heap;m.view(heap,3000000).fill(0);m.view(np,0x8d40).fill(0);o.reset();m.u32(np+0x10,resource.original);m.u32(np+0x92c,ns);m.u32(0x4a8eb4,np);m.i32(0x4a5710,Math.floor(combination/3));m.i32(0x4a5714,combination%3);m.i32(0x4a574c,step);m.i32(0x4a5748,max*step);m.i32(np+0x888,-6543);m.i32(np+0x88c,39000);m.i32(np+0x8d20,focused);m.i32(np+0x8bac,mode);
   const manager=c.anm_manager_create(),f=c.pm_create(anm,anm,manager),s=c.pm_state(f);put(s+12,-6543);put(s+16,39000);put(s+88,focused);put(s+108,mode);flt(s+124,1.7);m.f32(np+0x8c14,1.7);
   for(const vis of [0,1]){memory(c,c.anm_env_rng(manager,vis),8).fill(0);put(c.anm_env_rng(manager,vis),12345);}
   function compare(label){
    assert.equal(read(s+120),m.u32(np+0x7c90),label+' count');assert.equal(read(s+124),m.u32(np+0x8c14),label+' direction');
    for(let i=0;i<8;++i){const b=Buffer.from(memory(c,c.pm_option(f,i),0xe4)),a=Buffer.from(m.bytes(np+0x7570+i*0xe4,0xe4)),callback=a.readUInt32LE(0xdc);a.writeUInt32LE(callback===0x433690?1:callback===0x4337a0?2:callback,0xdc);assert.equal(firstDifference(b,a),'',label+` option${i}`);++options;}
    const native=o.states();assert.equal(c.anm_manager_count(manager),native.length,label+' animation count');for(const item of native){const cp=c.anm_manager_find(manager,item.id);assert.ok(cp);assert.equal(firstDifference(normalizeAnimation(memory(c,cp,0x434),read,read(cp+0x3a4)),normalizeAnimation(item.bytes,p=>m.u32(p),m.u32(item.p+0x3a4))),'',label+` animation${item.id}`);++animations;}
    for(const vis of [0,1])assert.deepEqual(Buffer.from(memory(c,c.anm_env_rng(manager,vis),8)),Buffer.from(m.bytes(vis?0x4c2ef8:0x4c2f00,8)),label+' RNG');
   }
   const levels=[0,...Array.from({length:max},(_,i)=>i+1),max,...Array.from({length:max+1},(_,i)=>max-i),max,0,max,Math.max(max-1,0),max];
   try{for(const [n,level]of levels.entries()){
    const power=level*step;m.i32(0x4a56e8,power);m.reg('EBX',np);m.call(0x432cc0);assert.equal(c.pm_rebuild(f,sht,power,step,max*step,combination),1,`error ${c.pm_value(f,1)}`);
    const label=`${name} focus${focused} mode${mode} transition${n} power${power}`;compare(label);++cases;
    for(let frame=0;frame<3;++frame){for(const overlay of [0,1]){o.update(overlay);assert.equal(c.anm_manager_update(manager,overlay),1);}compare(label+` animation frame${frame}`);}
   }}finally{c.pm_delete(f);c.anm_manager_delete(manager);}
  }
  c.anm_delete(anm);c.release(ad);c.sht_delete(sht);c.release(sd);
 }report('player-options',{passed:true,cases,options,animations,scope:'Original 432cc0, shipped SHT tables and real ANM execution; every legal power level, increase/decrease, repeated full power, focused/unfocused and all Marisa/B modes. Complete player lifecycle remains separate.'});
 }finally{m.close();}
});
