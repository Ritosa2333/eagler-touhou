import test from 'node:test';import assert from 'node:assert/strict';import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,report,root} from './helpers.mjs';import {animationOracle} from './anm-oracle.mjs';import {normalizeAnimation,firstDifference} from './shot-oracle.mjs';
test('TH11 player initialization and collision/item adapters preserve original character parameters',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m);let original=0,cases=0;
 m.replace(0x454360,'loaded player ANM resource',()=>original);m.replace(0x456b70,'update registration boundary',()=>0);m.replace(0x456c10,'draw registration boundary',()=>0);
 const dv=()=>new DataView(c.memory.buffer),read=p=>dv().getUint32(p,true),flt=(p,v)=>dv().setFloat32(p,v,true),put=(p,v)=>dv().setInt32(p,v,true);
 const fields=[[0,0x87c,12],[12,0x888,8],[20,0x890,16],[120,0x7c90,4],[96,0x8ba0,4]],bounds=[[76,0x8e4,36],[112,0x8cc,24],[136,0x8bcc,72]];
 try{for(const [combination,name]of ['pl00a','pl00b','pl00c','pl01a','pl01b','pl01c'].entries()){
  const resource=o.load(name.slice(0,4)+'.anm');original=resource.original;
  const anm=c.anm_create(),ad=c.allocate(resource.source.length);memory(c,ad,resource.source.length).set(resource.source);assert.equal(c.anm_open(anm,ad,resource.source.length),1);
  const bytes=readFileSync(resolve(root,'reference/assets',name+'.sht')),sht=c.sht_create(),sd=c.allocate(bytes.length);memory(c,sd,bytes.length).set(bytes);assert.equal(c.sht_open(sht,sd,bytes.length),1);
  const np=m.allocate(0x8d40),ns=m.allocate(bytes.length);m.view(np,0x8d40).fill(0);m.write(ns,bytes);m.u32(0x4c3240,ns);m.u32(0x4a8eb4,np);m.i32(0x4a5710,Math.floor(combination/3));m.i32(0x4a5714,combination%3);o.reset();
  const manager=c.anm_manager_create(),f=c.pf_create(sht,anm,anm,manager),s=c.pf_pointer(f,0),state=c.pf_pointer(f,1),body=c.pf_pointer(f,2),economy=c.pf_pointer(f,4),rate=c.anm_env_rate(manager),out=c.allocate(100);
  for(const vis of [0,1]){memory(c,c.anm_env_rng(manager,vis),8).fill(0);put(c.anm_env_rng(manager,vis),12345);}
  c.pf_input(f,0,0,combination,3);
  try{assert.equal(m.call(0x42f8a0,{args:[np]}),0);assert.equal(c.pf_action(f,0),1);
   const equal=(cp,p,n,label)=>assert.equal(firstDifference(Buffer.from(memory(c,cp,n)),Buffer.from(m.bytes(p,n))),'',name+' '+label);
   for(const [off,native,n]of fields)equal(s+off,np+native,n,'motion');for(const [off,native,n]of bounds)equal(state+off,np+native,n,'bounds');
   for(const [cp,native]of [[state+4,0x944],[state+44,0x8bb0],[c.pf_pointer(f,5),0x930]]){equal(cp,np+native,12,'timer');equal(cp+16,np+native+16,4,'timer flags');}
   equal(c.sht_header(sht),ns,0x268,'SHT character overrides');equal(economy+32,0x4a5748,8,'power limits');
   assert.equal(firstDifference(normalizeAnimation(memory(c,body,0x434),read,read(body+0x3a4)),normalizeAnimation(m.bytes(np+0x14,0x434),p=>m.u32(p),m.u32(np+0x3b8))),'',name+' body');
   c.pf_collision(f,out);assert.equal(read(out+24),m.u32(ns+4),name+' circular collision radius');equal(out,np+0x87c,8,'collision position');equal(out+8,np+0x8cc,8,'collision minimum');equal(out+16,np+0x8d8,8,'collision maximum');
   c.pf_items(f,out);equal(out,np+0x87c,12,'item position');equal(out+16,ns+8,4,'item attraction speed');
   for(const [off,native]of [[20,0x8bcc],[36,0x8be4],[52,0x8bfc]]){equal(out+off,np+native,8,'item rectangle minimum');equal(out+off+8,np+native+12,8,'item rectangle maximum');}
   ++cases;
  }finally{c.release(out);c.pf_delete(f);c.anm_manager_delete(manager);c.sht_delete(sht);c.release(sd);c.anm_delete(anm);c.release(ad);}
 }report('player-initialization',{passed:true,cases,scope:'Original 42f8a0 initializes all six configurations using real ANM initialization/binding. Resource retrieval and update/draw scheduler registration are explicit boundaries. Compares SHT overrides, fixed movement speeds, body state, power limits, collision/pickup bounds and timers; checks live-system adapter field mappings.'});
 }finally{m.close();}
});
