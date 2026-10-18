import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,report} from './helpers.mjs';
test('Configuration defaults and accepted file bytes match the native configuration reader',async()=>{
 const m=await oracle(),c=await core(),f=c.configuration_create(),p=c.allocate(204),native=m.allocate(204),host=m.allocate(0x600),file=m.allocate(256);let bytes=new Uint8Array(204),rejected=false,checks=0;
 const arg=i=>m.u32(m.reg('ESP')+4+i*4);
 for(let n=0;n<3;++n)m.call(0x42b010,{ecx:0x4ace18+n*0x8e});m.call(0x41a8a2,{ecx:native});
 const defaults=m.bytes(native,204);assert.deepEqual(memory(c,c.configuration_data(f),204),defaults);++checks;
 m.replace(0x42c970,'configuration-read',()=>{const ptr=m.allocate(256);m.write(ptr,bytes);m.u32(m.reg('EDX'),bytes.length);return ptr;},1);
 m.replace(0x42c4e0,'configuration-write',()=>0,1);m.replace(0x42c5c0,'configuration-log',()=>{rejected=true;return 0;});
 for(const address of [0x401460,0x42d2b0,0x42d2d0,0x42d2c0,0x42ed30,0x42ed40,0x42ed50,0x42ed60])m.replace(address,'configuration-platform',()=>0);
 const imports=m.onImport;m.onImport=e=>{if(e.name==='CreateFileA'){m.ret(0xffffffff,7);return;}imports(e);};
 try{
  for(let n=0;n<512;++n){bytes=Uint8Array.from(defaults);for(let i=0;i<108;++i)bytes[i]=(i*37+n*11)&255;
   const limits=[3,2,3,2,6,2,3,3];for(let i=0;i<8;++i)bytes[0xac+i]=(n+i*3)%limits[i];bytes[0xba]=n%101;bytes[0xbb]=(n*7)%101;
   if(n%3===0)bytes[0xac+(n%8)]=limits[n%8];if(n%7===0)bytes[0xa4]^=8;
   rejected=false;m.call(0x42f7c0,{ecx:host,args:[file]});memory(c,p,204).set(bytes);assert.equal(c.configuration_load(f,p,204),Number(!rejected),'accept '+n);
   if(!rejected){assert.deepEqual(memory(c,c.configuration_data(f),204),m.bytes(0x4b3488,204),'file '+n);assert.equal(c.configuration_roundtrip(f),0,'preserve unknown bytes');}++checks;
  }
  for(const length of [0,1,107,203,205,4096])assert.equal(c.configuration_load(f,p,length),0);report('configuration',{checks,invalidSizes:6,bytes:204,scope:'Native defaults, file validation and preservation of native configuration fields. Renderer capability checks are platform boundaries.'});
 }finally{c.configuration_delete(f);m.close();}
});
