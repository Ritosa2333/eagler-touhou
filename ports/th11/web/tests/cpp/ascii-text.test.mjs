import test from 'node:test';
import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';
import {animationOracle} from './anm-oracle.mjs';

test('TH11 ASCII atlas text matches original glyph vertices, shadows and filtering',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m),file=o.load('ascii.anm');
 const f=c.ascii_create(),data=c.allocate(file.source.length),str=c.allocate(256);
 const supervisor=m.allocate(0x184c0),device=m.allocate(4),vtable=m.allocate(0x180),batch=m.allocate(1024*168),manager=o.manager;
 memory(c,data,file.source.length).set(file.source);assert.equal(c.ascii_load(f,data,file.source.length),1);
 m.reg('ESI',supervisor+0x14);m.call(0x401fd0);m.u32(supervisor+0x184ac,file.original);m.u32(supervisor+0x184a4,9);
 m.reg('EAX',supervisor+0x14);m.call(0x44ab40,{ecx:0,edx:file.original});
 m.u32(0x4c3288,device);m.u32(device,vtable);
 for(const [camera,x,y,w,h]of [[0x4c359c,0,0,640,480],[0x4c3484,32,16,384,448]]){
  m.u32(camera+0xcc,x);m.u32(camera+0xd0,y);m.u32(camera+0xd4,w);m.u32(camera+0xd8,h);
 }
 m.replace(0x42a970,'camera matrix boundary (screen coordinates)',()=>0);
 let draws=[],filter=2,checks=0;const arg=n=>m.u32(m.reg('ESP')+n*4);
 const hook=(offset,name,argc,handler=()=>0)=>m.u32(vtable+offset,m.registerImport({dll:'ascii-oracle',name,argc,handler}));
 hook(0xbc,'viewport',2);hook(0xe4,'state',3);hook(0x104,'texture',3);hook(0x10c,'combiner',4);hook(0x164,'layout',2);
 hook(0x114,'sampler',4,()=>{if(arg(3)===6)filter=arg(4);return 0;});
 hook(0x14c,'draw',5,()=>{assert.equal(arg(2),4);assert.equal(arg(5),28);draws.push(Buffer.from(m.bytes(arg(4),arg(3)*3*28)));return 0;});
 try{
  const texts=['TH11 Test 0123!?\nAbc xyz','Music 0123 /+-\nExtra','0123456789/+-*%$. 12\n12. 34','0123456789/.s 12\n34'];
  for(let font=0;font<4;++font)for(let sample=0;sample<80;++sample){
   const sx=[1,.6,1.25,-.75][sample%4],sy=[1,.6,1.125][sample%3],color=[0xffffffff,0x8080a0ff,0x01010203,0][sample%4]>>>0;
   const x=[30.125,120.5,330.75,-20.2,600.1][sample%5],y=[20.125,205.5,435.7,-10.1][sample%4],flags=sample%8,pass=(flags>>2)&1;
   c.ascii_reset(f);memory(c,str,256).fill(0);memory(c,str,Buffer.byteLength(texts[font])).set(Buffer.from(texts[font]));
   assert.equal(c.ascii_add(f,str,x,y,sx,sy,color,font,flags),1);
   const request=supervisor+0x87c;m.view(request,0x130).fill(0);m.write(request,Buffer.from(texts[font]+'\0'));
   for(const [off,value]of [[0x100,x],[0x104,y],[0x110,sx],[0x114,sy]])m.f32(request+off,value);
   for(const [off,value]of [[0x10c,color],[0x11c,(flags>>1)&1],[0x120,font],[0x124,flags&1],[0x128,pass]])m.u32(request+off,value);
   m.u32(supervisor+0x1847c,1);m.u32(0x4c37cc,0x4c3484);
   draws=[];m.u32(manager+0x435620,0);m.u32(manager+0x7b5624,batch);m.u32(manager+0x7b5628,batch);m.u32(manager+0x4355bc,0);
   for(const off of [0x4355c0,0x4355c2,0x4355c6])m.view(manager+off,1)[0]=255;
   for(let i=0;i<4;++i)m.f32(0x4c91d8+i*28+12,1);
   assert.equal(c.ascii_draw(f,pass),1);m.call(0x401670,{args:[supervisor,pass]});m.reg('ESI',manager);m.call(0x44fd10);
   const actual=Buffer.from(memory(c,c.ascii_data(f),c.ascii_size(f))),expected=Buffer.concat(draws),label=`font${font} sample${sample}`;
   assert.equal(actual.length,expected.length,label+' vertex count');
   for(let i=0;i<actual.length;i+=4)assert.equal(actual.readUInt32LE(i),expected.readUInt32LE(i),`${label} vertex${Math.floor(i/28)} word${i%28/4}: ${actual.readFloatLE(i)} vs ${expected.readFloatLE(i)}`);
   if(expected.length)assert.equal(c.ascii_filter(f)+1,filter,label+' filter');++checks;
  }
  report('ascii-text',{passed:true,checks,scope:'Original 401670 plus 44fe30/44f880 submissions using shipped ascii.anm: all four fonts, punctuation/spacing/newlines, shadow alpha, fractional/negative scales, viewports and sampler selection. GPU rasterization separate.'});
 }finally{c.ascii_delete(f);c.release(data);c.release(str);m.close();}
});
