import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync,readdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,oracle,memory,string,root,report,enableNativeMath} from './helpers.mjs';
import {resourceLoader,nativePool,prepareBank,compareAnm,nativeInstances} from './anm-oracle.mjs';

test('TH17 animation templates and copied instances match original initialization for every bank',async()=>{
 const c=await core(),m=await oracle(),manager=c.anm_manager_create(),nativeManager=nativePool(m),loader=resourceLoader(m),native=m.allocate(0x600),vm=c.anm_vm_create();
 enableNativeMath(m);m.u32(0x4a0430,0x4b5918);m.f32(0x4b5918,1);
 const ascii=readFileSync(resolve(root,'reference/assets/ascii.anm')),ap=c.allocate(ascii.length);memory(c,ap,ascii.length).set(ascii);
 assert.equal(c.anm_manager_load(manager,0,ap,ascii.length),1,string(c,c.anm_manager_error(manager)));c.deallocate(ap);assert.equal(c.anm_manager_fallback(manager,0),1);
 const font=prepareBank(m,loader,'ascii.anm',ascii,nativeManager,0),text=m.allocate(0x19270);m.u32(0x4b7678,text);m.u32(text+0x19248,font);const heap=m.heap;let files=0,templates=0,bindings=0;
 try{for(const name of readdirSync(resolve(root,'reference/assets')).filter(n=>n.endsWith('.anm')).sort()){
  const bytes=readFileSync(resolve(root,'reference/assets',name)),p=c.allocate(bytes.length);memory(c,p,bytes.length).set(bytes);
  assert.equal(c.anm_manager_load(manager,1,p,bytes.length),1,name+': '+string(c,c.anm_manager_error(manager)));c.deallocate(p);m.heap=heap;
  const bank=prepareBank(m,loader,name,bytes,nativeManager,1),count=m.u32(bank+0x114),base=m.u32(bank+0x10c);
  for(let i=0;i<count;i++){
   const np=base+i*0x600,template=c.anm_manager_template(manager,1,i);compareAnm(c,m,template,np,name+' template '+i);templates++;
   for(let k=0;k<3;k++){const v=i*3.25-k*7;m.f32(native+0x5e4+k*4,v);new DataView(c.memory.buffer).setFloat32(c.anm_vm_translation(vm)+k*4,v,true);}
   m.call(0x407230,{ecx:bank,args:[native,i]});assert.equal(c.anm_manager_bind(manager,vm,1,i),1);
   compareAnm(c,m,vm,native,name+' binding '+i);assert.deepEqual(Buffer.from(memory(c,c.anm_vm_translation(vm),12)),Buffer.from(m.bytes(native+0x5e4,12)));bindings++;
  }
  files++;c.anm_manager_unload(manager,1);
 }
 report('anm-manager-templates',{passed:true,files,templates,bindings,originalFunctions:['0x4747f0','0x466de0','0x407230'],scope:'All ANM negative-time initialization and copied live-instance state. GPU texture allocation preserves dimensions only; rendered pixels and gameplay need separate checks.'});
 }finally{c.anm_vm_delete(vm);c.anm_manager_delete(manager);m.close();}
});

test('TH17 layer selection preserves the native material flags for all byte layer values',async()=>{
 const c=await core(),m=await oracle(),vm=c.anm_vm_create(),native=m.allocate(0x600);let checks=0;
 try{for(let layer=0;layer<256;layer++)for(const flags of [0,0xffffffff,0x818f4c20]){
  const visual=c.anm_vm_visual(vm);new DataView(c.memory.buffer).setUint32(visual+4,flags,true);m.u32(native+0x534,flags);
  c.anm_vm_set_layer(vm,layer);m.call(0x406530,{ecx:native,args:[layer]});assert.equal(new DataView(c.memory.buffer).getUint32(visual+4,true),m.u32(native+0x534),'layer '+layer);checks++;
 }report('anm-layers',{passed:true,checks,originalFunction:'0x406530'});
 }finally{c.anm_vm_delete(vm);m.close();}
});
function ins(op,args=[],time=0,mask=0){const b=Buffer.alloc(8+args.length*4);b.writeInt16LE(op);b.writeUInt16LE(b.length,2);b.writeInt16LE(time,4);b.writeUInt16LE(mask,6);args.forEach((a,i)=>typeof a==="object"?b.writeFloatLE(a.f,8+i*4):b.writeInt32LE(a,8+i*4));return b;}
function fixture(scripts){scripts=scripts.map(s=>Buffer.concat([s,ins(-1)]));const head=Buffer.alloc(124+scripts.length*8),entries=64+4,name=entries+scripts.length*8,sprite=name+4;head.writeUInt32LE(8);head.writeUInt16LE(1,4);head.writeUInt16LE(scripts.length,6);head.writeUInt16LE(16,10);head.writeUInt16LE(16,12);head.writeUInt16LE(1,14);head.writeUInt32LE(name,16);head.writeUInt32LE(sprite,64);head.write("@T",name);head.writeFloatLE(16,sprite+12);head.writeFloatLE(16,sprite+16);let off=head.length;for(let i=0;i<scripts.length;i++){head.writeInt32LE(i,entries+i*8);head.writeUInt32LE(off,entries+i*8+4);off+=scripts[i].length;}return Buffer.concat([head,...scripts]);}

test("TH17 animation manager executes child scripts, detached instances and replacement through original lifecycle",async()=>{
 const scripts=[
 Buffer.concat([ins(300,[0],-1),ins(304,[7],-1),ins(100,[10000,87],-1,1),ins(401,[{f:.125},{f:.25},{f:.375}],-1),ins(500,[1]),ins(505,[2,{f:12},{f:-18}]),ins(504,[3]),ins(0,[],20),ins(1)]),
 Buffer.concat([ins(300,[0],-1),ins(304,[12],-1),ins(509),ins(503,[4]),ins(415,[{f:0},{f:0},{f:.0625}]),ins(0,[],14),ins(1)]),
 Buffer.concat([ins(300,[0],-1),ins(304,[28],-1),ins(509),ins(407,[12,0,{f:24},{f:36},{f:0}]),ins(0,[],18),ins(1)]),
 Buffer.concat([ins(300,[0],-1),ins(304,[35],-1),ins(400,[{f:2},{f:3},{f:4}]),ins(0,[],8),ins(1)]),
 Buffer.concat([ins(300,[0],-1),ins(304,[35],-1),ins(509),ins(100,[10001,159],0,1),ins(0,[],5),ins(1)])
 ],bytes=fixture(scripts),c=await core(),m=await oracle(),manager=c.anm_manager_create(),nativeManager=nativePool(m),loader=resourceLoader(m),out=m.allocate(4),p=c.allocate(bytes.length);m.u32(0x4a0430,0x4b5918);m.f32(0x4b5918,1);enableNativeMath(m);let checks=0,frames=0;
 try{memory(c,p,bytes.length).set(bytes);assert.equal(c.anm_manager_load(manager,0,p,bytes.length),1,string(c,c.anm_manager_error(manager)));const nativeBank=prepareBank(m,loader,"fixture.anm",bytes,nativeManager,0);
 const compareAll=label=>{for(let alternate=0;alternate<2;alternate++){const instances=nativeInstances(m,nativeManager,alternate);assert.equal(c.anm_manager_count(manager,alternate),instances.length,label+" count "+alternate);for(let i=0;i<instances.length;i++){const handle=m.u32(instances[i]+0x538);assert.equal(c.anm_manager_order(manager,alternate,i),handle,label+" order "+i);const vm=c.anm_manager_find(manager,handle);assert.ok(vm);compareAnm(c,m,vm,instances[i],label+" vm "+handle);assert.deepEqual(Buffer.from(memory(c,c.anm_vm_translation(vm),12)),Buffer.from(m.bytes(instances[i]+0x5e4,12)),label+" translation");checks++;}}};
 for(let sample=0;sample<32;sample++){const handle=c.anm_manager_spawn(manager,0,0,7,0);assert.ok(handle,string(c,c.anm_manager_error(manager)));m.call(0x405a70,{ecx:nativeBank,args:[out,0,7,0]});assert.equal(handle,m.u32(out));compareAll("spawn "+sample);if(sample%2){const replacement=c.anm_manager_rebind(manager,handle,0);assert.ok(replacement,string(c,c.anm_manager_error(manager)));m.u32(out,handle);m.call(0x476910,{ecx:out,args:[0]});assert.equal(replacement,m.u32(out));compareAll("rebind "+sample);}
  for(let frame=0;frame<24;frame++){const rate=sample%3?.5:1;m.f32(0x4b5918,rate);for(let alternate=0;alternate<2;alternate++){assert.equal(c.anm_manager_update(manager,alternate,rate),1,string(c,c.anm_manager_error(manager)));m.call(alternate?0x475940:0x475830,{ecx:nativeManager});}compareAll("sample "+sample+" frame "+frame);frames++;}
 }report("anm-manager-lifecycle",{passed:true,checks,frames,originalFunctions:["0x405a70","0x476220","0x476370","0x476910","0x466de0","0x475830","0x475940"],scope:"Real child and detached allocation, inherited variables/rotation/translation, four queue orders, script progression, replacement and deferred parent/child destruction. Synthetic ANM scripts isolate lifecycle; actual drawing and full game scenarios remain separate."});
 }finally{c.deallocate(p);c.anm_manager_delete(manager);m.close();}
});

