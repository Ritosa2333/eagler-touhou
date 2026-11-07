import test from 'node:test';import assert from 'node:assert/strict';import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,string,root,report,enableNativeMath,enableOriginalVectorMath} from './helpers.mjs';

test('TH17 screen cameras and animation draw order match original callbacks',async()=>{
 const c=await core(),m=await oracle();enableNativeMath(m);enableOriginalVectorMath(m);
 const a=c.anm_manager_create(),r=c.anm_renderer_create(),f=c.draw_schedule_create(a,r);let checks=0;
 try{
  // Run the original screen-camera initializer for each physical resolution.
  for(const [width,height,scale]of [[640,480,1],[1280,960,2],[960,720,1.5],[1920,1440,3]]){
   m.u32(0x524710,width);m.u32(0x524714,height);m.f32(0x524718,scale);m.view(0x524700,1)[0]=0;m.call(0x4424d0,{ecx:0x4b5ae0});c.screen_view_configure(f,width,height,scale);
   for(let i=0;i<4;i++){
    const native=0x4b5ae0+0x238+i*300;
    assert.deepEqual(Buffer.from(memory(c,c.screen_view_viewport(f,i),24)),Buffer.from(m.bytes(native+0xe0,24)),`${width} camera ${i} viewport`);
    for(let k=0;k<2;k++)assert.deepEqual(Buffer.from(memory(c,c.screen_view_matrix(f,i,k),64)),Buffer.from(m.bytes(native+0x60+k*64,64)),`${width} camera ${i} matrix ${k}`);checks+=3;
   }
  }
  c.screen_view_configure(f,640,480,1);
  const bytes=readFileSync(resolve(root,'reference/assets/ascii.anm')),p=c.allocate(bytes.length);memory(c,p,bytes.length).set(bytes);assert.equal(c.anm_manager_load(a,2,p,bytes.length),1,string(c,c.anm_manager_error(a)));c.deallocate(p);
  assert.equal(c.draw_schedule_populate(f,2,0),1,string(c,c.anm_manager_error(a)));
  // Read registration metadata from the original constructor and execute every
  // registered original callback. Only its final draw submission is captured.
  const source=readFileSync(resolve(root,'artifacts/cpp/analysis/jp/functions/00471950.asm'),'utf8');let previous=0;const callbacks=[];
  for(const match of source.matchAll(/CALL 0x004011f0/g)){const block=source.slice(previous,match.index);previous=match.index+match[0].length;const functions=[...block.matchAll(/(?:MOV dword ptr \[EAX \+ 0x8\],|PUSH )(0x475[0-9a-f]+)/g)],priorities=[...block.matchAll(/PUSH (0x[0-9a-f]+)/g)];callbacks.push({fn:parseInt(functions.at(-1)[1]),priority:parseInt(priorities.at(-1)[1])});}
  assert.equal(callbacks.length,39);callbacks.sort((a,b)=>a.priority-b.priority);
  const manager=m.allocate(0x200),device=m.allocate(4),vtable=m.allocate(0x100),layers=[],cameras=[];m.u32(0x509a20,manager);m.u32(0x4b5ae8,device);m.u32(device,vtable);
  m.u32(vtable+0xe4,m.registerImport({dll:'fixture',name:'draw schedule state boundary',argc:3,handler:()=>0}));
  m.replace(0x46d0a0,'draw schedule flush boundary',()=>0);m.replace(0x475ba0,'capture original ordered animation layer',()=>{layers.push(m.u32(m.reg('ESP')+4));return 1;},1);
  // This initializer has already executed above; a code hook also observes
  // Unicorn's cached translated blocks without changing their instructions.
  m.cpu.hook_add(m.uc.HOOK_CODE,()=>cameras.push('refresh'),null,0x442300,0x442300);m.replace(0x409460,'capture original camera selection',()=>{cameras.push(m.u32(m.reg('ESP')+4));return 0;},1);
  for(const callback of callbacks)m.call(callback.fn,{ecx:manager});
  assert.ok(c.draw_schedule_run(f)>=0,string(c,c.draw_schedule_error(f)));
  const events=Array.from(new Int32Array(c.memory.buffer,c.draw_schedule_events(f),c.draw_schedule_event_count(f))),actualCameras=[];
  for(let i=0;i<events.length;i+=2){if(events[i+1])actualCameras.push('refresh');actualCameras.push(events[i]);}
  assert.deepEqual(actualCameras,cameras,'Camera transitions and refresh order');
  const colors=[];for(let i=0;i<c.anm_renderer_calls(r);i++){const bytes=Buffer.from(memory(c,c.anm_renderer_history(r,i),c.anm_renderer_history_size(r,i)));for(let at=0;at<bytes.length;at+=28*6)colors.push(bytes.readUInt32LE(at+16)&255);}
  assert.deepEqual(colors,layers,'Transparent animation layer order');checks+=2;
  report('draw-schedule',{passed:true,resolutions:4,cameras:16,layers:layers.length,checks,originalFunctions:['0x4424d0','0x442300','0x471950 registration','0x475400..0x475820 callbacks'],scope:'Original camera initialization compares viewport and both matrix byte arrays at four resolutions. Original registered layer callbacks execute with draw/camera boundaries captured; portable scheduler submits actual ANM quad colors in the same layer order. Full stage composition and GPU rasterization require separate verification.'});
 }finally{c.draw_schedule_delete(f);c.anm_renderer_delete(r);c.anm_manager_delete(a);m.close();}
});
