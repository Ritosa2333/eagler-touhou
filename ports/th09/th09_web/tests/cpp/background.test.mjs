import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync,readdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {oracle,core,memory,report,root,d3dxScalar} from './helpers.mjs';
test('Every original STD background script and camera transition matches native execution',async()=>{
 const m=await oracle(),c=await core(),native=m.allocate(0x65c4),resource=m.allocate(256);let f=0,p=[],bytes=[],file=0,anim=0,side=0,events=[],checks=0,files=0,frames=0,dv=new DataView(c.memory.buffer),objectOffsets=[];
 const fields=Array.from({length:c.background_field_count()},(_,i)=>Array.from({length:3},(_,j)=>dv.getUint32(c.background_fields()+(i*3+j)*4,true))),arg=i=>m.u32(m.reg('ESP')+4+i*4),event=(...a)=>events.push([...a,...Array(4-a.length).fill(0)]);
 d3dxScalar(m);
 m.replace(0x401660,'background-resource',()=>resource,1);m.replace(0x43c7a0,'background-resource-load',()=>resource,2);m.replace(0x42c970,'STD-resource',()=>{m.write(file,bytes);return file;},1);
 function index(vm){if(vm>=anim&&vm<anim+bytes.readUInt16LE(2)*0x2a4)return (vm-anim)/0x2a4;if(vm>=native+0x2c&&vm<native+0x818)return 1000+(vm-native-0x2c)/0x2a4;return 2000+(vm-native-0xce0)/0x2a4;}
 const origAllocate=m.allocate.bind(m);m.replace(0x401340,'STD-animations',()=>{const n=arg(0),v=origAllocate(n);anim=v;return v;},2);
 m.replace(0x401560,'background-animation-start',()=>{const vm=arg(0),script=arg(1);event(0,index(vm),0,script);m.view(vm,0x2a4).fill(0);m.write(vm+0x21a,[script&255,script>>>8]);m.write(vm+0x214,[(script+1)&255,(script+1)>>>8]);m.u32(vm+0x220,1);m.u32(vm+0x1f0,0xffffffff);return 0;},2);
 m.replace(0x436f30,'background-animation-step',()=>{const vm=arg(0);event(1,index(vm));const n=m.i32(vm+0x100)+1;m.i32(vm+0x100,n);if(n%97===0)m.u32(vm+0x220,0);return 0;},1);
 m.u32(native+0x10,0x4a7d90);const heap=m.heap;
 function field(off,value){const entry=fields.find(a=>a[0]===off);assert.ok(entry);dv=new DataView(c.memory.buffer);if(entry[2]===1){memory(c,p[0]+entry[1],1)[0]=value;m.write(native+off,[value]);}else{dv.setInt32(p[0]+entry[1],value,true);m.i32(native+off,value);}}
 function compare(label){
  dv=new DataView(c.memory.buffer);assert.equal(c.background_invalid(f),0,label+' valid');for(const [o,a,n] of fields)assert.deepEqual(memory(c,p[0]+a,n),m.bytes(native+o,n),label+' field '+o.toString(16));
  assert.deepEqual(memory(c,p[1],36),m.bytes(0x4b3178+side*0xf0,36),label+' camera');assert.deepEqual(memory(c,p[1]+36,12),m.bytes(0x4b319c+side*0xf0,12),label+' camera direction');assert.deepEqual(memory(c,p[1]+48,16),m.bytes(0x4b31b4+side*0xf0,16),label+' camera offset/fov');
  assert.deepEqual(memory(c,c.background_part(f,2),c.background_count(f,0)*0x2a4),m.bytes(anim,c.background_count(f,0)*0x2a4),label+' primitives');objectOffsets.forEach((off,i)=>assert.equal(c.background_object_flags(f,i),m.bytes(file+off+3,1)[0],label+' active '+i));
  assert.equal(c.background_event_count(f),events.length,label+' events');events.forEach((e,i)=>assert.deepEqual(Array.from({length:4},(_,j)=>dv.getInt32(c.background_events(f)+i*16+j*4,true)),e,label+' event '+i));++checks;
 }
 try{for(const name of readdirSync(resolve(root,'reference/assets')).filter(n=>n.endsWith('.std')).sort()){
  bytes=readFileSync(resolve(root,'reference/assets',name));objectOffsets=Array.from({length:bytes.readUInt16LE(0)},(_,i)=>bytes.readUInt32LE(0x490+i*4));const times=[],labels=[];for(let i=bytes.readUInt32LE(8);i+20<=bytes.length;i+=20){const t=bytes.readInt32LE(i),op=bytes.readInt16LE(i+4);if(t<0)break;times.push(t);if(op===31)labels.push(bytes.readInt32LE(i+8));}
  for(let scenario=0;scenario<4;++scenario){side=scenario&1;f=c.background_fixture();p=[c.background_part(f,0),c.background_part(f,1)];dv=new DataView(c.memory.buffer);m.heap=heap;file=m.allocate(bytes.length);m.view(native,0x65c4).fill(0);m.u32(native+12,side);m.u32(native+16,0x4a7d90+56*side);m.view(0x4b3178,0x1e0).fill(0);m.view(resource,256).fill(0);m.i32(0x4a7e84,(files+scenario)%16);m.u32(0x4a7ec4,0);m.u32(0x4a7dc4+side*56,0);m.f32(0x4b36b8,1);m.u32(0x4b36d4,0);
   const data=c.allocate(bytes.length);memory(c,data,bytes.length).set(bytes);events=[];assert.equal(m.call(0x403830,{ecx:native}),0);assert.equal(c.background_load(f,data,bytes.length,(files+scenario)%16),1);c.release(data);compare(name+' load');
   const step=(frame,rate=1,force=false)=>{const flags=frame%71===70?0x1800:0,fieldFlags=frame%43===42?1:0;m.u32(0x4a7ec4,flags);m.u32(0x4a7dc4+side*56,fieldFlags);m.f32(0x4b36b8,rate);m.u32(0x4b36d4,force?32:0);events=[];m.call(0x402350,{ecx:native});c.background_step(f,flags,fieldFlags,rate,force);compare(name+'/'+scenario+'/'+frame);++frames;};
   for(let frame=0;frame<800;++frame){if(frame===101){field(0x1c,scenario%2+1);field(0x20,31);}if(frame===192){field(0xccc,1);field(0xcd0,57);}if(frame===350&&labels.length)field(0x18,labels[0]);step(frame,[1,.5,.99,1.5][scenario],scenario===3);}
   // Jump the clock to original instruction times, retaining the real next-op
   // cursor and running each boundary for three updates.
   for(const time of [...new Set(times)].sort((a,b)=>a-b)){const timer=fields.find(x=>x[0]===0xc58),at=p[0]+timer[1];dv.setInt32(at+8,time,true);dv.setFloat32(at+4,time,true);m.i32(native+0xc60,time);m.f32(native+0xc5c,time);for(let t=0;t<3;++t)step(time+t);}
   c.background_delete(f);f=0;
  }++files;
 }report('background',{files,checks,frames,scope:'All original STD files, model/primitive initialization, script commands, camera/curve interpolation, labels/waits/loops, sway, fog interpolation, freeze, overlays and boss animation timing. Original D3DX scalar normalization also executes. ANM execution is an explicit boundary; GPU rendering is not covered.'});
 }finally{if(f)c.background_delete(f);m.close();}
});
