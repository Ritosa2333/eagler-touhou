import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync,readdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {oracle,core,memory,report,root,d3dxScalar} from './helpers.mjs';
test('Original background model culling, billboard fog and ribbon geometry match C++',async()=>{
 const m=await oracle(),c=await core(),native=m.allocate(0x65c4),resource=m.allocate(0x12900),device=m.allocate(4),vt=m.allocate(0x160),arg=i=>m.u32(m.reg('ESP')+4+i*4);d3dxScalar(m);
 let f=0,base=0,p=[],q=[],bytes=[],file=0,anim=0,sprites=0,draws=[],vms=[],quads=[],checks=0,files=0,worldDraws=0,spriteDraws=0,ribbonDraws=0,dv=new DataView(c.memory.buffer);const heap=m.heap;
 const index=a=>(a-anim)/0x2a4,normalizeVm=b=>{b=b.slice();b.fill(0,0x224,0x228);return b;};
 m.u32(device,vt);m.u32(0x4b3108,device);m.u32(0x4dc550,resource);m.u32(0x4b3448,0x4b3178);m.u32(0x4b36d4,0);m.f32(0x4b36b8,1);
 for(const [slot,argc] of [[0x94,3],[0xa0,2]])m.u32(vt+slot,m.registerImport({dll:'stage',name:'matrix',argc,handler:()=>0}));
 for(const fn of [0x4396a0,0x401600,0x42ff00,0x42fec0])m.replace(fn,'stage-draw-state',()=>{if(fn===0x42ff00||fn===0x42fec0)draws.push([0,fn===0x42fec0?1:0]);return 0;},fn===0x401600?1:0);
 m.replace(0x401660,'stage-anm',()=>resource,1);m.replace(0x43c7a0,'stage-anm-load',()=>resource,2);m.replace(0x42c970,'stage-data',()=>{m.write(file,bytes);return file;},1);
 m.replace(0x401340,'stage-allocate',()=>{anim=m.allocate(arg(0));return anim;},2);
 m.replace(0x401560,'stage-animation-start',()=>{const v=arg(0),script=arg(1);m.view(v,0x2a4).fill(0);m.write(v+0x21a,[script&255,script>>>8]);m.write(v+0x214,[(script+1)&255,(script+1)>>>8]);m.u32(v+0x220,1);m.u32(v+0x1f0,0xffffffff);return 0;},2);
 m.replace(0x436f30,'animation-boundary',()=>{const v=arg(0),n=m.i32(v+0x100)+1;m.i32(v+0x100,n);if(n%97===0)m.u32(v+0x220,0);return 0;},1);
 m.replace(0x43b1a0,'world-sprite',()=>{draws.push([1,index(arg(0))]);vms.push(...normalizeVm(m.bytes(arg(0),0x2a4)));++worldDraws;return 0;},1);
 m.replace(0x43afe0,'screen-sprite',()=>{draws.push([2,index(arg(0))]);vms.push(...normalizeVm(m.bytes(arg(0),0x2a4)));++spriteDraws;return 0;},1);
 m.replace(0x43b5c0,'screen-quad',()=>{draws.push([3,index(arg(0))]);vms.push(...normalizeVm(m.bytes(arg(0),0x2a4)));const b=m.bytes(arg(1),112);for(let n=0;n<4;++n)if(!b[n*28+19])b.fill(0,n*28+16,n*28+20);quads.push(...b);++ribbonDraws;return 0;},2);
 function compare(label){
  dv=new DataView(c.memory.buffer);assert.equal(c.stage_renderer_size(f,0),draws.length*8,label+' events '+JSON.stringify(draws));draws.forEach((e,i)=>assert.deepEqual([dv.getInt32(c.stage_renderer_data(f,0)+i*8,true),dv.getInt32(c.stage_renderer_data(f,0)+i*8+4,true)],e,label+' event '+i));
  assert.deepEqual(memory(c,q[3],128),m.bytes(0x4b3178+0x4c,128),label+' camera matrices');const actual=memory(c,c.stage_renderer_data(f,1),c.stage_renderer_size(f,1)).slice();for(let i=0;i<actual.length;i+=0x2a4)actual.fill(0,i+0x224,i+0x228);for(let i=0;i<actual.length;++i)if(actual[i]!==vms[i]){const a=new DataView(actual.buffer),b=new DataView(Uint8Array.from(vms).buffer),at=i&~3,n=Math.floor(i/0x2a4);assert.fail(`${label} draw ${n} entry ${draws.filter(e=>e[0])[n]} field ${(i%0x2a4).toString(16)}: ${a.getFloat32(at,true)} vs ${b.getFloat32(at,true)}`);}assert.deepEqual(memory(c,c.stage_renderer_data(f,2),c.stage_renderer_size(f,2)),Uint8Array.from(quads),label+' ribbon vertices');
  const core=memory(c,c.background_part(base,2),c.background_count(base,0)*0x2a4).slice(),orig=m.bytes(anim,core.length);for(let i=0;i<core.length;i+=0x2a4){core.fill(0,i+0x224,i+0x228);orig.fill(0,i+0x224,i+0x228);}assert.deepEqual(core,orig,label+' retained model state');++checks;
 }
 try{for(const filename of readdirSync(resolve(root,'reference/assets')).filter(n=>n.endsWith('.std')).sort()){
  f=c.stage_renderer_fixture();base=c.stage_renderer_base(f);p=Array.from({length:3},(_,i)=>c.background_part(base,i));q=Array.from({length:5},(_,i)=>c.stage_renderer_part(f,i));bytes=readFileSync(resolve(root,'reference/assets',filename));m.heap=heap;file=m.allocate(bytes.length);m.view(native,0x65c4).fill(0);m.view(resource,256).fill(0);m.view(0x4b3178,240).fill(0);m.u32(native+16,0x4a7d90);m.u32(0x4a7ec4,0);m.u32(0x4a7dc4,0);m.i32(0x4a7e84,files%16);
  const data=c.allocate(bytes.length);memory(c,data,bytes.length).set(bytes);assert.equal(m.call(0x403830,{ecx:native}),0);assert.equal(c.background_load(base,data,bytes.length,files%16),1);c.release(data);c.stage_renderer_prepare(f);q=Array.from({length:5},(_,i)=>c.stage_renderer_part(f,i));dv=new DataView(c.memory.buffer);sprites=m.allocate(c.background_count(base,0)*0x44);
  [16,16,288,448].forEach((v,i)=>{dv.setUint32(q[0]+i*4,v,true);m.u32(0x4b3178+0xcc+i*4,v);});dv.setFloat32(q[0]+16,0,true);dv.setFloat32(q[0]+20,1,true);m.f32(0x4b3178+0xdc,0);m.f32(0x4b3178+0xe0,1);
  for(let i=0;i<c.background_count(base,0);++i){const vm=c.background_part(base,2)+i*0x2a4,at=anim+i*0x2a4,sp=q[2]+i*0x44,np=sprites+i*0x44;dv.setUint32(vm+0x1f8,3,true);m.u32(at+0x1f8,3);dv.setInt16(vm+0x1fc,[0,2,0x12][i%3],true);m.write(at+0x1fc,[i%3===0?0:i%3===1?2:0x12,0]);for(const off of [0x18,0x1c]){dv.setFloat32(vm+off,1,true);m.f32(at+off,1);}for(const off of [0x28,0x2c]){dv.setFloat32(vm+off,32,true);m.f32(at+off,32);}dv.setUint32(vm+0x1f0,0xc04090c0,true);m.u32(at+0x1f0,0xc04090c0);m.u32(at+0x224,np);
   const vals=[.125,.25,.875,.75,32,32];vals.forEach((v,n)=>{dv.setFloat32(sp+0x20+n*4,v,true);m.f32(np+0x20+n*4,v);});
  }
  for(let frame=0;frame<1200;++frame){m.call(0x402350,{ecx:native});c.background_step(base,0,0,1,0);if(frame%30)continue;
   memory(c,q[1],64).set(memory(c,p[1],64));for(let layer=0;layer<4;++layer){draws=[];vms=[];quads=[];m.call(0x4026c0,{ecx:native,args:[layer]});c.stage_renderer_draw(f,layer);compare(filename+'/'+frame+'/'+layer);}
  }c.stage_renderer_delete(f);f=0;++files;
 }report('stage-renderer',{files,checks,worldDraws,spriteDraws,ribbonDraws,scope:'Original STD model draw lifecycle and culling, projected billboard scaling/fog, all ribbon endpoints, UVs and draw order. ANM draw calls are recorded boundaries. Fully transparent ribbon RGB is ignored because it is discarded by alpha testing.'});
 }finally{if(f)c.stage_renderer_delete(f);m.close();}
});
