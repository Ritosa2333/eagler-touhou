import test from 'node:test';import assert from 'node:assert/strict';
import{core,oracle,memory,report}from'./helpers.mjs';import{animationOracle}from'./anm-oracle.mjs';import{graphicsOracle}from'./graphics-oracle.mjs';import{normalizeAnimation,firstDifference}from'./shot-oracle.mjs';
test('TH11 boss screen deformation matches native mesh, colors, phases and ANM strips',async()=>{
 const c=await core(),m=await oracle();graphicsOracle(m);const o=animationOracle(m),text=o.load('text.anm');
 const state=m.allocate(0x163c),mesh=m.allocate(24),spell=m.allocate(0x900),heap=m.heap;
 m.u32(0x4c3808,text.original);m.u32(0x4c342c,1);m.u32(0x4a8d6c,spell);
 m.replace(0x41271a,'end isolated original deformation suffix',()=>{m.reg('ESP',m.stack-4);return 0;});
 const view=()=>new DataView(c.memory.buffer),read=p=>view().getUint32(p,true);
 const compare=(a,b,label)=>{const delta=firstDifference(a,b);assert.equal(delta,'',label+' '+delta);};let frames=0,checks=0;
 try{for(let sample=0;sample<32;++sample){
  const f=c.deform_create(),p=c.allocate(text.source.length);try{
   memory(c,p,text.source.length).set(text.source);assert.equal(c.anm_open(c.deform_data(f,5),p,text.source.length),1);c.release(p);
   m.heap=heap;m.view(heap,200000).fill(0);o.reset();assert.equal(c.deform_initialize(f),1);
   m.reg('ESI',mesh);m.reg('EAX',17);m.call(0x40e530,{args:[17,0]});m.view(state,0x163c).fill(0);m.u32(state+0x1618,mesh);
   const cs=c.deform_data(f,0),manager=c.deform_data(f,4),target=[0,16,24,63,128,192,256,512][sample%8],spellId=sample%2?173:0;
   const write=(offset,value,float=true)=>{float?view().setFloat32(cs+offset,value,true):view().setUint32(cs+offset,value,true);float?m.f32(state+offset,value):m.u32(state+offset,value);};
   write(0x161c,target);write(0x1620,16);write(0x1624,[0xff603090,0xff000000,0xffffffff,0x114aff2a][sample%4],false);write(0x1628,sample*.073);write(0x162c,-sample*.137);m.i32(spell+0x8dc,spellId);
   for(let frame=0;frame<80;++frame){const label=`sample ${sample} frame ${frame}`;
    write(0x34,(sample%7-3)*60+frame*.125);write(0x38,80+sample*9+frame*.25);write(0x3c,sample%3?0:25);
    m.resetThreadFPU();m.reg('EBX',state);m.reg('ESI',state+0x34);m.call(0x412260,{limit:2000000});c.deform_update(f,spellId);
    compare(Buffer.from(memory(c,cs+0x161c,20)),Buffer.from(m.bytes(state+0x161c,20)),label+' state');
    compare(Buffer.from(memory(c,c.deform_data(f,1),17*17*28)),Buffer.from(m.bytes(m.u32(mesh+16),17*17*28)),label+' vertices');
    compare(Buffer.from(memory(c,c.deform_data(f,2),17*17*12)),Buffer.from(m.bytes(m.u32(mesh+20),17*17*12)),label+' positions');
    c.deform_copy(f);m.reg('EBX',mesh);m.call(0x40e890);
    for(const item of o.states()){const vm=c.anm_manager_find(manager,item.id);assert.ok(vm);const a=normalizeAnimation(memory(c,vm,0x434),read,read(vm+0x3a4)),b=normalizeAnimation(item.bytes,q=>m.u32(q),m.u32(item.p+0x3a4));a.writeUInt32LE(0,0x418);b.writeUInt32LE(0,0x418);compare(a,b,label+' animation');compare(Buffer.from(memory(c,read(vm+0x400),17*2*28)),Buffer.from(m.bytes(m.u32(item.p+0x400),17*2*28)),label+' strip');checks+=2;}
    assert.equal(c.anm_manager_update(manager,0),1);o.update(false);++frames;
   }
  }finally{c.deform_delete(f);}
 }
 report('screen-deformation',{passed:true,frames,checks,scope:'Original 40e530/40e6c0/40e890 and isolated 411750 deformation suffix; native 17x17 vertex/color/phase bytes and ANM strips. GPU render-target composition tested separately.'});
 }finally{m.close();}
});
