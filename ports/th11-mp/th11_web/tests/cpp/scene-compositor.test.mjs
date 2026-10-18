import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';
import {animationOracle} from './anm-oracle.mjs';
import {normalizeAnimation,firstDifference} from './shot-oracle.mjs';
test('TH11 compositor target/clear order and real ANM vertices match original callbacks',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m),{source,original}=o.load('text.anm'),f=c.comp_create(),data=c.allocate(source.length);
 const device=m.allocate(4),table=m.allocate(0x180),batch=m.allocate(1024*168),copies=Array.from({length:3},()=>m.allocate(0x434)),camera=0x4c359c;
 m.u32(0x4c3288,device);m.u32(device,table);m.u32(0x4c342c,3);m.u32(0x4c3430,4);m.u32(0x4c3434,0);
 m.u32(0x4a8eac,0);m.u32(0x4a8ea8,0);m.u32(camera+0xd4,640);m.u32(camera+0xd8,480);m.f32(camera+0xe0,1);
 m.u32(o.manager+0x7b5624,batch);m.u32(o.manager+0x7b5628,batch);
 for(let i=0;i<4;++i)m.f32(0x4c91d8+i*28+12,1);
 const spriteBase=m.u32(original+0x118),textures=Array.from({length:4},(_,i)=>{const p=m.allocate(4);m.u32(p,i+1);return p;});
 for(let offset=0,index=0,chunk=0;;++chunk){for(let j=0;j<source.readUInt16LE(offset+4);++j)m.u32(spriteBase+(index++)*0x48+8,textures[chunk]);const next=source.readUInt32LE(offset+36);if(!next)break;offset+=next;}
 let events=[],draws=[],target=0,texture=0;
 const arg=n=>m.u32(m.reg('ESP')+n*4),hook=(offset,name,argc,handler)=>m.u32(table+offset,m.registerImport({dll:'compositor-oracle',name,argc,handler}));
 hook(0x94,'target',3,()=>{target=arg(3);events.push([0,target,0,0,0,0,640,480,0,0x3f800000]);return 0;});
 hook(0xac,'clear',7,()=>{const count=arg(2),rect=arg(3),box=count?[0,1,2,3].map(i=>m.u32(rect+i*4)):[0,0,640,480];events.push([1,target,arg(5),count,box[0],box[1],box[2]-box[0],box[3]-box[1],0,0x3f800000]);assert.equal(arg(4),3);return 0;});
 hook(0xbc,'viewport',2,()=>0);hook(0xe4,'state',3,()=>0);hook(0x104,'texture',3,()=>{texture=arg(3);return 0;});hook(0x10c,'combiner',4,()=>0);hook(0x114,'sampler',4,()=>0);hook(0x164,'layout',2,()=>0);
 hook(0x14c,'draw',5,()=>{const topology=arg(2)-1,count=arg(3),stride=arg(5),size=(topology===3?count*3:count+2)*stride;draws.push({target,texture,topology,count,stride,bytes:Buffer.from(m.bytes(arg(4),size))});return 0;});
 // Camera-matrix calculation is separately validated; this oracle exercises
 // the original compositor callbacks and unmodified ANM drawing/flush code.
 m.replace(0x42a970,'camera boundary',()=>0);
 const read=p=>new DataView(c.memory.buffer).getUint32(p,true),put=(p,x)=>new DataView(c.memory.buffer).setUint32(p,x,true);
 let frames=0,submissions=0;
 try{
  memory(c,data,source.length).set(source);assert.equal(c.comp_load(f,data,source.length),1);o.reset();
  for(let i=0;i<3;++i){m.u32(0x4c343c+i*4,copies[i]);m.reg('EAX',copies[i]);m.reg('EBX',i===1?82:81);m.reg('EDI',original);m.call(0x44acd0);
   const p=c.comp_data(f,0)+i*0x434,q=copies[i];assert.equal(firstDifference(normalizeAnimation(memory(c,p,0x434),read,read(p+0x3a4)),normalizeAnimation(m.bytes(q,0x434),x=>m.u32(x),m.u32(q+0x3a4))),'',`initial composite ${i}`);
  }
  assert.equal(c.comp_stage(f),1);m.view(copies[0]+0x37c,2).set([2,0]);m.call(0x44b4b0,{args:[copies[0]]});
  // Layer traversal is covered by scene-schedule with all 31 layers populated.
  o.reset();
  for(let sample=0;sample<32;++sample){
   c.comp_reset(f);events=[];draws=[];target=0;texture=0;
   const region=sample%2?[0,0,512,512]:[13,0,422,480],color=(0xff000000|Math.imul(sample,0x10203))>>>0;
   for(let i=0;i<4;++i){put(c.comp_data(f,5)+i*4,region[i]);m.u32(0x4c3780+i*4,region[i]);}put(c.comp_data(f,4),color);m.u32(0x4c3c40,color);
   const tint=(0xff000000|Math.imul(sample+1,0x70605))>>>0;put(c.comp_data(f,0)+2*0x434+0x374,tint);m.u32(copies[2]+0x374,tint);
   const passes=[[6,0x4290e0],[7,0x429220],[8,0x4293d0],[9,0x4292b0],[10,0x429420],[11,0x429340],[12,0x429470],[13,0x4293a0]];
   for(const[k,address]of passes){assert.equal(c.comp_pass(f,k),1);assert.equal(m.call(address,{ecx:0x4c3280}),1);}
   const actualEvents=Array.from({length:c.comp_size(f,0)},(_,i)=>Array.from({length:10},(_,j)=>read(c.comp_data(f,1)+i*40+j*4)));
   assert.deepEqual(actualEvents,events,`frame ${sample} targets/clears`);assert.equal(c.comp_size(f,2),draws.length);
   for(let i=0;i<draws.length;++i){const p=c.comp_data(f,3)+i*28,[t,tex,start,size,top,count,stride]=Array.from({length:7},(_,j)=>read(p+j*4)),expected=draws[i];
    assert.deepEqual([t,tex,top,count,stride],[expected.target,expected.texture,expected.topology,expected.count,expected.stride]);assert.equal(firstDifference(Buffer.from(memory(c,c.comp_data(f,2)+start,size)),expected.bytes),'',`frame ${sample} draw ${i}`);++submissions;
   }
   assert.equal(read(c.comp_data(f,0)+2*0x434+0x374),0xffffffff);++frames;
  }
  report('scene-compositor',{passed:true,frames,submissions,scope:'Actual native 4290e0/429220/4293d0/4292b0/429420/429340/429470/4293a0 callbacks, target and clear order, three real text.anm embedded VMs, byte-identical vertices and final tint reset. Native camera calculation is isolated; GPU pixels are not covered.'});
 }finally{c.comp_delete(f);c.release(data);m.close();}
});
