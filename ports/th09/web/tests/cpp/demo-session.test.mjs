import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync,readdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,oracle,memory,report,root} from './helpers.mjs';
test('Attract replay uses the native demo flag and leaves on every original menu key',async()=>{
 const c=await core(),m=await oracle(),f=c.session_create(),name=c.allocate(128),data=c.allocate(16000000);let checks=0;
 const source=n=>readFileSync(resolve(root,'reference/assets',n));
 for(const n of readdirSync(resolve(root,'reference/assets')).filter(n=>/\.(anm|sht|ecl|msg|std|bmp|png)$/.test(n))){const b=source(n);memory(c,name,128).fill(0);memory(c,name,128).set(new TextEncoder().encode(n));memory(c,data,b.length).set(b);c.session_file(f,name,data,b.length);}
 m.replace(0x404980,'dialogue-busy',()=>1);for(const address of [0x4a7dac,0x4a7de4]){const p=m.allocate(160);m.view(p,160).fill(0);m.u32(address,p);}m.u32(0x4a7ea8,2);m.u32(0x4a7db8,1);m.u32(0x4a7df0,1);
 const demo=source('demorpy0.rpy');memory(c,data,demo.length).set(demo);
 try{for(const key of [1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384]){
   assert.equal(c.session_demo(f,data,demo.length),1);assert.equal(c.session_step(f,0,0,0),1);assert.equal(c.session_value(f,0),1);
   m.u32(0x4b3690,0);m.u32(0x4a7eb4,0);m.u32(0x4a811c,0);m.u32(0x4a7ec4,14);m.u32(0x4a7ecc,0);m.u32(0x4a7ed0,0);m.write(0x4acf3a,new Uint8Array([key&255,key>>>8]));m.call(0x41aa5f,{ecx:0x4a7d90});assert.equal(m.u32(0x4b3690),1,'Original demo exit '+key);
   assert.equal(c.session_step(f,0,0,key),1);assert.equal(c.session_value(f,0),5,'C++ demo exit '+key);++checks;
  }report('demo-session',{checks,scope:'All 15 native menu bits interrupt a running attract replay; separately verified replay simulation remains unchanged.'});
 }finally{c.session_delete(f);m.close();}
});
