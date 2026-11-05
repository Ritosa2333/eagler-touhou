import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';import {animationOracle} from './anm-oracle.mjs';import {bits} from './ecl-bytecode.mjs';
test('TH11 pooled items render original vertices including offscreen indicators and dormant cancel stars',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m),{source,original}=o.load('bullet.anm');
 const data=c.allocate(source.length),pos=c.allocate(12),f=c.im_create();memory(c,data,source.length).set(source);assert.equal(c.im_load(f,data,source.length),1);
 const nm=m.allocate(0x265e70),np=m.allocate(12),bm=m.allocate(0x46d700),camera=m.allocate(0x100),device=m.allocate(4),vtable=m.allocate(0x180),batch=m.allocate(4096*168);
 m.view(nm,0x265e70).fill(0);m.u32(0x4a8e90,nm);m.u32(0x4a8d68,bm);m.u32(bm+0x46d674,original);m.i32(0x4a56e8,0);m.i32(0x4a5748,400);
 m.u32(0x4c3288,device);m.u32(device,vtable);m.u32(0x4c37cc,camera);m.u32(camera+0xd4,640);m.u32(camera+0xd8,480);o.reset();
 const arg=n=>m.u32(m.reg('ESP')+n*4),hook=(off,argc,handler)=>m.u32(vtable+off,m.registerImport({dll:'item-render-oracle',name:'GPU boundary '+off,argc,handler}));let draws=[];
 for(const [off,argc]of [[0xe4,3],[0x104,3],[0x10c,4],[0x164,2],[0x114,4]])hook(off,argc,()=>0);
 hook(0x14c,5,()=>{assert.equal(arg(2),4);assert.equal(arg(5),28);draws.push(Buffer.from(m.bytes(arg(4),arg(3)*3*arg(5))));return 0;});
 m.replace(0x455b10,'item effect boundary',()=>0,4);m.replace(0x44a1e0,'item sound boundary',()=>0);
 const view=()=>new DataView(c.memory.buffer),put=(p,v)=>view().setUint32(p,v,true),flt=(p,v)=>view().setFloat32(p,v,true);
 const normalize=input=>{const b=Buffer.from(input);for(const off of [4,16,0x68,0xcc,0x118,0x144,0x190,0x1cc,0x218,0x244,0x270,0x29c,0x38c,0x3b0,0x45c])b.writeUInt32LE(0,off);const base=b.readUInt32LE(0x3a4);for(const off of [0x394,0x3a4,0x3a8])if(b.readUInt32LE(off))b.writeUInt32LE((b.readUInt32LE(off)-base)>>>0,off);b.writeUInt32LE(b.readUInt32LE(0x3ac)?1:0,0x3ac);return b;};
 let count=0,checks=0,vertices=0;
 try{
  for(let type=1;type<=11;++type){flt(pos,0);flt(pos+4,100);flt(pos+8,0);m.f32(np,0);m.f32(np+4,100);m.f32(np+8,0);m.reg('EAX',np);m.call(0x424230,{ecx:type,edx:nm,args:[0xabcdefab,bits(-1.5),bits(2.2)]});assert.equal(c.im_spawn(f,type,pos,0xabcdefab,-1.5,2.2),0);++count;}
  const indexes=[...Array.from({length:10},(_,i)=>i),150];
  const ys=[-100,-40,-32,-16,-8.001,-8,0,10,400,464,480,-12,15];
  for(let frame=0;frame<ys.length;++frame){
   for(const [j,i]of indexes.entries()){const a=c.im_item(f,i),b=nm+0x14+i*0x478,x=(j-5)*31.25,y=ys[(frame+j)%ys.length];flt(a+0x434,x);flt(a+0x438,y);m.f32(b+0x434,x);m.f32(b+0x438,y);}
   draws=[];m.u32(o.manager+0x435620,0);m.u32(o.manager+0x7b5624,batch);m.u32(o.manager+0x7b5628,batch);m.u32(o.manager+0x4355bc,0);for(const off of [0x4355c0,0x4355c2,0x4355c6])m.view(o.manager+off,1)[0]=255;
   for(let i=0;i<4;++i)m.f32(0x4c91d8+i*28+12,1);m.reg('EAX',nm);m.call(0x423f40);m.reg('ESI',o.manager);m.call(0x44fd10);assert.equal(c.im_draw(f),1);
   const actual=Buffer.from(memory(c,c.im_vertices(f),c.im_vertex_bytes(f))),expected=Buffer.concat(draws);assert.deepEqual(actual,expected,`frame ${frame} screen vertices`);vertices+=actual.length/28;
   for(const i of indexes){const actual=normalize(memory(c,c.im_item(f,i),0x478)),expected=normalize(m.bytes(nm+0x14+i*0x478,0x478));if(!actual.equals(expected)){let off=0;while(actual[off]===expected[off])++off;off&=~3;assert.fail(`frame ${frame} item ${i} offset ${off.toString(16)} actual ${actual.readUInt32LE(off).toString(16)} expected ${expected.readUInt32LE(off).toString(16)}`);}++checks;}
  }
  report('item-rendering',{passed:true,checks,vertices,scope:'All eleven item types including a delayed uninitialized cancel star; original draw loop and ANM sprite/alpha mutations, ordered screen vertex bytes through the common ZunGraphics interface.'});
 }finally{c.im_delete(f);c.release(data);c.release(pos);m.close();}
});
