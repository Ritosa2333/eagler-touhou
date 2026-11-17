import test from 'node:test';import assert from 'node:assert/strict';import {spawnSync} from 'node:child_process';import {readFileSync} from 'node:fs';import {resolve} from 'node:path';import {WASI} from 'node:wasi';
import {oracle,root,report,sha} from './helpers.mjs';
test('Real-time slowdown counters match original presentation sampling, pauses, skips and clock rollback',async()=>{
 const file=resolve(root,'artifacts/cpp/frame-performance-test.wasm'),compiler=resolve(root,'../th10_web/tools/wasi-sdk-34.0-x86_64-windows/bin/clang++.exe');
 const built=spawnSync(compiler,['-std=c++17','-O2','-ffp-contract=off','-mexec-model=reactor','-Wl,--export-memory','-Wl,--strip-all','tests/cpp/performance-exports.cpp','cpp/game/FramePerformance.cpp','-o',file],{cwd:root,windowsHide:true,encoding:'utf8'});assert.equal(built.status,0,built.stderr);
 const wasi=new WASI({version:'preview1',args:[],env:{},preopens:{}}),{instance}=await WebAssembly.instantiate(readFileSync(file),{wasi_snapshot_preview1:wasi.wasiImport});wasi.initialize(instance);const c=instance.exports,m=await oracle();let now=0,checks=0;
 try{const native=m.allocate(0x38),world=m.allocate(0xb0),time=m.allocate(8);m.u32(0x4b76a8,native);m.u32(0x4b76b0,world);m.u32(0x4b7690,0);const stub=Buffer.from([0xf2,0x0f,0x10,0x05,0,0,0,0,0xc3]);stub.writeUInt32LE(time,4);m.write(0x462710,stub);const f64=(p,v)=>{const b=m.view(p,8),d=new DataView(b.buffer,b.byteOffset,8);if(v===undefined)return d.getFloat64(0,true);d.setFloat64(0,v,true);};
  // This routine returns the clock in XMM0, not EAX. Keep the original caller
  // and arithmetic unchanged and write exactly that register at the boundary.
  let last=0;for(const [duration,count,scene,flags,skip]of[[1,60,1,0,0],[1,30,1,0,0],[1,15,1,4,0],[1,20,1,16,0],[1,58,1,0,0],[1,57,1,0,0],[1,30,1,0,1],[1,10,0,0,0],[-.5,1,1,0,0],[1.5,60,1,0,0]]){
   for(let i=0;i<count;i++){now=last+duration*(i+1)/count;f64(time,now);m.u32(0x4b76b0,scene?world:0);m.u32(world+0x8c,flags);m.view(0x4b5cd4,1)[0]=skip;m.call(0x461c80);c.sample(now,scene,flags,skip);assert.equal(c.fps(),m.f32(native+0x30));assert.equal(c.delivered(),f64(native+0x20));assert.equal(c.requested(),f64(native+0x28));checks+=3;}last=now;
  }
  assert.ok(c.delivered()<c.requested());report('frame-performance',{passed:true,checks,fixtureSha256:sha(readFileSync(file)),delivered:c.delivered(),requested:c.requested(),scope:'Native 461c80 presentation sampling through original floating-point arithmetic. The controlled clock covers 60/30/15/58/57 FPS, paused/game-over/title exclusion, frame skip and clock rollback. Spell capture timing has its own oracle.'});
 }finally{m.close();}
});
