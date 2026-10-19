import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,report} from './helpers.mjs';
import {assertBytesEqual} from '../../../../../tools/testing/byte-evidence.mjs';

test('CPU sprite geometry preserves continuous positions and original payload state',async()=>{
 const m=await oracle(),c=await core(),f=c.renderer_fixture(),p=Array.from({length:7},(_,i)=>c.renderer_part(f,i)),manager=m.allocate(0x12900),vm=m.allocate(0x2a4),sprite=m.allocate(0x44),device=m.allocate(4),vt=m.allocate(0x160),arg=i=>m.u32(m.reg('ESP')+4+4*i);
 let dv=new DataView(c.memory.buffer),drawn=[],checks=0,submitted=0,blend=5,depth=1,seed=0x81263714;
 const random=()=>{seed^=seed<<13;seed^=seed>>>17;seed^=seed<<5;return seed>>>0;};
 const real=(scale=1)=>(random()%10001-5000)/5000*scale;
 const nativeDraw=[0x43a950,0x43aa50,0x43ab50,0x43ad40,0x43afe0];
 const triangleOrder=[0,1,2,2,1,3];
 m.view(manager,0x12900).fill(0);m.write(manager+0x12884,[3,0,0xff,0xff]);m.u32(0x4dc550,manager);m.u32(0x4b3108,device);m.u32(device,vt);m.u32(0x4b3448,0x4b3178);
 m.u32(vt+0xf4,m.registerImport({dll:'renderer',name:'texture',argc:3,handler:()=>0}));m.u32(vt+0xc8,m.registerImport({dll:'renderer',name:'state',argc:3,handler:()=>{assert.equal(arg(1),20);blend=arg(2)===2?1:5;return 0;}}));
 m.replace(0x4396a0,'flush-boundary',()=>0);m.replace(0x439730,'quad-boundary',()=>{const b=m.bytes(arg(0),112);for(const n of triangleOrder)drawn.push(...b.subarray(n*28,(n+1)*28));return 0;},1);
 m.replace(0x42ff40,'depth-boundary',()=>{assert.equal(arg(0),14);depth=arg(1);return 0;},2);
 m.view(0x4dc558,112).fill(0);for(let n=0;n<4;++n){m.f32(0x4dc558+n*28+12,1);m.u32(0x4dc558+n*28+16,0xffffffff);}
 function u(off,value){dv.setUint32(p[0]+off,value,true);m.u32(vm+off,value);}
 function q(off,value){dv.setFloat32(p[0]+off,value,true);m.f32(vm+off,value);}
 function s(off,value){dv.setFloat32(p[1]+off,value,true);m.f32(sprite+off,value);}
 function assertExceptScreenXYExact(actual,expected,label){
  assert.equal(actual.length,expected.length,label+' byte length');
  assert.equal(actual.length%28,0,label+' complete SpriteVertex records');
  for(let vertex=0;vertex<actual.length/28;++vertex){
   const start=vertex*28+8,end=vertex*28+28;
   assertBytesEqual(actual.subarray(start,end),expected.subarray(start,end),label+' vertex '+vertex+' bytes excluding screen X/Y');
  }
 }
 function positions(bytes){
  const view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);
  return Array.from({length:4},(_,vertex)=>[0,1,2].map(axis=>view.getFloat32(vertex*28+axis*4,true)));
 }
 function uvs(bytes){
  const view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);
  return Array.from({length:4},(_,vertex)=>[view.getFloat32(vertex*28+20,true),view.getFloat32(vertex*28+24,true)]);
 }
 function submittedPositions(bytes){
  const view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);
  return Array.from({length:bytes.length/28},(_,vertex)=>[0,1,2].map(axis=>view.getFloat32(vertex*28+axis*4,true)));
 }
 function submittedUvs(bytes){
  const view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);
  return Array.from({length:bytes.length/28},(_,vertex)=>[view.getFloat32(vertex*28+20,true),view.getFloat32(vertex*28+24,true)]);
 }
 function setInput({position=[120,80],size=[16,10],scale=[1,1],rotation=0,anchor=0,shake=[0,0],uv=[0.125,0.25,0.875,0.75],view=[0,0,640,480],texture=1,color1=0xffffffff,flags=3,tint=0xffffffff,tintEnabled=true,depthDisabled=false}={}){
  dv=new DataView(c.memory.buffer);memory(c,p[0],0x2a4).fill(0);m.view(vm,0x2a4).fill(0);
  memory(c,p[1],0x44).fill(0);m.view(sprite,0x44).fill(0);m.view(manager,0x12900).fill(0);m.write(manager+0x12884,[3,0,0xff,0xff]);
  m.u32(0x4dc550,manager);m.u32(0x4b3108,device);m.u32(device,vt);m.u32(0x4b3448,0x4b3178);m.u32(vm+0x224,sprite);
  u(0x1f0,color1);u(0x1f4,0xffffffff);u(0x1f8,flags|((anchor&3)<<11)|(depthDisabled?0x2000:0));
  q(8,rotation);q(0x18,scale[0]);q(0x1c,scale[1]);q(0x28,size[0]);q(0x2c,size[1]);
  q(0x208,position[0]);q(0x20c,position[1]);q(0x210,0);q(0x30,0);q(0x34,0);
  dv.setUint32(p[1]+4,texture,true);m.u32(sprite+4,texture);s(0x20,uv[0]);s(0x24,uv[1]);s(0x28,uv[2]);s(0x2c,uv[3]);
  view.forEach((value,index)=>{dv.setUint32(p[2]+index*4,value,true);m.u32(0x4b3178+0xcc+index*4,value);});
  shake.forEach((value,index)=>{dv.setFloat32(p[3]+index*4,value,true);m.f32(manager+0x1c+index*4,value);});
  dv.setUint32(p[4],tint,true);m.u32(manager,tint);memory(c,p[5],1)[0]=+tintEnabled;m.u32(manager+4,+tintEnabled);
 }
 function runPair(kind,input,label,checkState=true){
  setInput(input);drawn=[];
  const originalReturn=m.call(nativeDraw[kind],{ecx:manager,args:[vm]})|0;
  const originalQuad=Uint8Array.from(m.bytes(0x4dc558,112)),originalSubmitted=Uint8Array.from(drawn);
  const originalTexture=m.u32(manager+0x12880),originalBlend=blend,originalDepth=depth;
  const candidateReturn=c.renderer_draw(f,kind)|0;
  const candidateQuad=memory(c,p[6],112).slice(),candidateSubmitted=memory(c,c.renderer_data(f),c.renderer_size(f)).slice();
  assert.equal(candidateReturn,originalReturn,label+' return value');
  assert.equal(candidateSubmitted.length,originalSubmitted.length,label+' submission length');
  if(checkState){
   assert.equal(c.renderer_state(f,0),originalTexture,label+' texture');
   assert.equal(c.renderer_state(f,1),originalBlend,label+' blend');
   assert.equal(c.renderer_state(f,2),originalDepth,label+' depth-write');
   assert.equal(c.renderer_state(f,3),m.bytes(manager+0x12884,1)[0],label+' blend mode');
   assert.equal(c.renderer_state(f,4),m.bytes(manager+0x12887,1)[0],label+' depth mode');
  }
  return {originalQuad,originalSubmitted,candidateQuad,candidateSubmitted};
 }
 function runNonDrawable({label,flags=3,color1=0xffffffff}){
  setInput({flags,color1});drawn=[];
  const originalQuadBefore=Uint8Array.from(m.bytes(0x4dc558,112));
  const originalReturn=m.call(nativeDraw[4],{ecx:manager,args:[vm]})|0;
  assert.equal(originalReturn,-1,label+' original return');
  assert.equal(drawn.length,0,label+' original submissions');
  assertBytesEqual(m.bytes(0x4dc558,112),originalQuadBefore,label+' original quad unchanged');
  const candidateQuadBefore=memory(c,p[6],112).slice();
  const candidateReturn=c.renderer_draw(f,4)|0;
  assert.equal(candidateReturn,-1,label+' candidate return');
  assert.equal(c.renderer_size(f),0,label+' candidate submissions');
  assertBytesEqual(memory(c,p[6],112),candidateQuadBefore,label+' candidate quad unchanged');
 }
 try{
  for(let scenario=0;scenario<9000;++scenario){
   dv=new DataView(c.memory.buffer);memory(c,p[0],0x2a4).fill(0);m.view(vm,0x2a4).fill(0);m.u32(vm+0x224,sprite);
   const flags=3|((scenario%4)<<4)|((scenario%4)<<11)|(scenario%3===0?0x2000:0)|(scenario%7===0?0x20000:0);
   let color1=random();if(!(color1>>>24))color1|=0x01000000;u(0x1f0,color1);u(0x1f4,random());u(0x1f8,flags);
   q(8,scenario%3===0?0:real(6.28));const scaleX=0.25+(random()%1251)/1000,scaleY=0.25+(random()%1251)/1000;q(0x18,scenario&1?-scaleX:scaleX);q(0x1c,scenario&2?-scaleY:scaleY);
   q(0x28,1+random()%64);q(0x2c,1+random()%64);q(0x208,320+real(32));q(0x20c,240+real(32));q(0x210,real(1));q(0x30,real(1));q(0x34,real(1));
   const tex=scenario%13+1;dv.setUint32(p[1]+4,tex,true);m.u32(sprite+4,tex);s(0x20,real(1));s(0x24,real(1));s(0x28,real(1));s(0x2c,real(1));
   const view=[0,0,640,480];view.forEach((value,index)=>{dv.setUint32(p[2]+index*4,value,true);m.u32(0x4b3178+0xcc+index*4,value);});
   const shake=[real(3),real(3)];shake.forEach((value,index)=>{dv.setFloat32(p[3]+index*4,value,true);m.f32(manager+0x1c+index*4,value);});
   const tint=random();dv.setUint32(p[4],tint,true);m.u32(manager,tint);memory(c,p[5],1)[0]=scenario&1;m.u32(manager+4,scenario&1);
   const kind=scenario%5;drawn=[];const original=m.call(nativeDraw[kind],{ecx:manager,args:[vm]})|0;const originalQuad=Uint8Array.from(m.bytes(0x4dc558,112)),originalSubmitted=Uint8Array.from(drawn);
   assert.equal(c.renderer_draw(f,kind)|0,original,'return '+scenario);dv=new DataView(c.memory.buffer);
   const candidateQuad=memory(c,p[6],112),candidateSubmitted=memory(c,c.renderer_data(f),c.renderer_size(f));
   assertExceptScreenXYExact(candidateQuad,originalQuad,'quad '+scenario+'/'+kind);assertExceptScreenXYExact(candidateSubmitted,originalSubmitted,'submit '+scenario);
   assert.equal(candidateSubmitted.length,168,'visible in-view sample submits six triangles '+scenario);
   assert.equal(c.renderer_state(f,0),m.u32(manager+0x12880),'texture '+scenario);assert.equal(c.renderer_state(f,1),blend,'blend '+scenario);assert.equal(c.renderer_state(f,2),depth,'depth '+scenario);assert.equal(c.renderer_state(f,3),m.bytes(manager+0x12884,1)[0],'blend mode '+scenario);assert.equal(c.renderer_state(f,4),m.bytes(manager+0x12887,1)[0],'depth mode '+scenario);
   ++checks;if(originalSubmitted.length)++submitted;
  }
  const nonDrawable=[{label:'flags-0',flags:0},{label:'flags-1',flags:1},{label:'flags-2',flags:2},{label:'alpha-0',flags:3,color1:0x00ffffff}];
  for(const sample of nonDrawable)runNonDrawable(sample);
  const normalUv=[[0.125,0.25],[0.875,0.25],[0.125,0.75],[0.875,0.75]],mirrorUv=[[0.875,0.25],[0.125,0.25],[0.875,0.75],[0.125,0.75]];
  const gridQuad=[[111.5,74.5,0],[127.5,74.5,0],[111.5,84.5,0],[127.5,84.5,0]];
  for(const [kind,expectedUv,label] of [[0,normalUv,'pixel-grid'],[1,mirrorUv,'pixel-grid-mirror']]){
   const result=runPair(kind,{position:[120,80],size:[16,10],scale:[1,1],rotation:0,anchor:0,shake:[0,0]},label);
   assert.deepEqual(positions(result.candidateQuad),gridQuad,label+' literal quad geometry');
   assert.deepEqual(submittedPositions(result.candidateSubmitted),[0,1,2,2,1,3].map(index=>gridQuad[index]),label+' literal triangle order');
   assert.deepEqual(uvs(result.candidateQuad),expectedUv,label+' literal quad UVs');
   assert.deepEqual(submittedUvs(result.candidateSubmitted),[0,1,2,2,1,3].map(index=>expectedUv[index]),label+' literal triangle UV order');
   assertBytesEqual(result.candidateQuad,result.originalQuad,label+' complete original quad bytes');
   assertBytesEqual(result.candidateSubmitted,result.originalSubmitted,label+' complete original submitted bytes');
  }
  const fractional={position:[120.25,80.75],size:[16,10],scale:[1.5,0.5],rotation:0,anchor:0,shake:[0.125,-0.25]};
  const roundedQuad=[[107.875,77.5,0],[131.875,77.5,0],[107.875,82.5,0],[131.875,82.5,0]];
  const continuousQuad=[[108.375,78,0],[132.375,78,0],[108.375,83,0],[132.375,83,0]];
  for(const [kind,expectedQuad,expectedUv,label] of [[0,roundedQuad,normalUv,'fractional-rounded'],[1,roundedQuad,mirrorUv,'fractional-mirror'],[4,continuousQuad,normalUv,'fractional-no-round']]){
   const result=runPair(kind,fractional,label);
   assert.deepEqual(positions(result.candidateQuad),expectedQuad,label+' literal quad geometry');
   assert.deepEqual(submittedPositions(result.candidateSubmitted),triangleOrder.map(index=>expectedQuad[index]),label+' literal triangle positions');
   assert.deepEqual(uvs(result.candidateQuad),expectedUv,label+' literal quad UVs');
   assert.deepEqual(submittedUvs(result.candidateSubmitted),triangleOrder.map(index=>expectedUv[index]),label+' literal triangle UV order');
   assertExceptScreenXYExact(result.candidateQuad,result.originalQuad,label+' original quad payload');
   assertExceptScreenXYExact(result.candidateSubmitted,result.originalSubmitted,label+' original triangle payload');
  }
  const edges=[
   {label:'left-touch',position:[-8,240],submitted:6},{label:'left-outside',position:[-8.25,240],submitted:0},
   {label:'right-touch',position:[648,240],submitted:6},{label:'right-outside',position:[648.25,240],submitted:0},
   {label:'top-touch',position:[320,-5],submitted:6},{label:'top-outside',position:[320,-5.25],submitted:0},
   {label:'bottom-touch',position:[320,485],submitted:6},{label:'bottom-outside',position:[320,485.25],submitted:0},
  ];
  for(const edge of edges){
   const result=runPair(4,{position:edge.position,size:[16,10],scale:[1,1],rotation:0,anchor:0,shake:[0,0]},edge.label,false);
   assert.equal(result.originalSubmitted.length,edge.submitted*28,edge.label+' original visible vertex count');
   assert.equal(result.candidateSubmitted.length,edge.submitted*28,edge.label+' expected visible vertex count');
   if(edge.submitted)assertExceptScreenXYExact(result.candidateSubmitted,result.originalSubmitted,edge.label+' original payload');
  }
  report('sprite-renderer',{checks,submitted,nonDrawableCases:nonDrawable.length,gridCases:2,fractionalCases:3,cullingCases:edges.length,paths:5,scope:'9,000 seeded in-view native comparisons exclude only screen X/Y bytes; Z and other vertex fields, ordered payload, draw return and render state remain exact across positive and negative scale. Four deterministic non-drawable cases verify return, empty submission and unchanged quads; pixel-grid cases retain complete native byte comparison; fractional literals verify continuous geometry and mirroring; four edge-touch/outside pairs verify culling.'});
 }finally{c.renderer_delete(f);m.close();}
});
