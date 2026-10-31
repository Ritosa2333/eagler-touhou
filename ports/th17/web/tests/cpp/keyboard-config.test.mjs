import test from 'node:test';import assert from 'node:assert/strict';import {core,oracle,memory,report} from './helpers.mjs';
test('TH17 browser key mapping and configuration bytes agree with the original',async()=>{
 const c=await core(),m=await oracle(),p=c.allocate(256),settings=c.game_config_create(),native=m.allocate(0x800),bindings=m.allocate(20);let keys=new Uint8Array(256),cases=0,loaded=null;
 try{
  m.replace(0x4019b0,'platform XInput',()=>0);m.replace(0x401690,'platform joystick merge',()=>m.u32(m.reg('ESP')+4),1);m.replace(0x404d20,'separately verified input edges',()=>0);
  const previous=m.onImport;m.onImport=e=>{if(e.name==='GetKeyboardState'){m.write(m.u32(m.reg('ESP')+4),keys);m.ret(1,1);}else previous(e);};m.u32(0x5226d0,1);m.u32(0x4b6210,0);
  let seed=0x39edabba;const random=()=>seed=(Math.imul(seed,1664525)+1013904223)>>>0;
  for(let sample=0;sample<1280;sample++){keys.fill(0);if(sample<256)keys[sample]=128;else for(let j=0;j<12;j++)keys[random()&255]=128;memory(c,p,256).set(keys);m.call(0x401ab0);assert.equal(c.keyboard_keys(p),m.reg('EAX'),'physical keys '+sample);cases++;}
  m.replace(0x48172f,'platform log synchronization',()=>0);m.replace(0x4025a0,'platform configuration log',()=>0);m.replace(0x402310,'platform configuration write',()=>0);
  m.replace(0x4020d0,'platform configuration read',()=>{m.u32(m.reg('EDX'),loaded?.length??0);if(!loaded)return 0;const address=m.allocate(100);m.write(address,loaded);return address;},1);
  const snapshot=()=>Buffer.from(memory(c,c.game_config_bytes(settings),100));
  for(let sample=0;sample<64;sample++){
   const values=Buffer.alloc(20);for(let i=0;i<10;i++)values.writeUInt16LE((sample+i*7)&65535,i*2);m.write(0x4b3464,values);memory(c,p,20).set(values);c.game_config_reset(settings,p);loaded=null;m.call(0x441d20,{ecx:native});assert.deepEqual(snapshot(),Buffer.from(m.bytes(0x4b5cb4,100)),'default config '+sample);
   loaded=snapshot();for(let i=4;i<24;i++)loaded[i]=random()&255;for(let i=0;i<6;i++)loaded[0x1c+i]=random()%[2,3,2,6,3,3][i];loaded[0x29]=random()&255;memory(c,p,100).set(loaded);assert.equal(c.game_config_open(settings,p,100),1);m.call(0x441d20,{ecx:native});assert.deepEqual(snapshot(),Buffer.from(m.bytes(0x4b5cb4,100)),'loaded config '+sample);
   for(const [offset,limit]of [[0x1c,2],[0x1d,3],[0x1e,2],[0x1f,6],[0x20,3],[0x21,3]]){const bad=Buffer.from(loaded);bad[offset]=limit;memory(c,p,100).set(bad);assert.equal(c.game_config_open(settings,p,100),0,'range '+offset);}
   cases+=8;
  }
  report('keyboard-config',{passed:true,cases,originalFunctions:['0x401ab0','0x441d20'],scope:'All 256 individual virtual keys and 1024 combined key sets, native configuration defaults, valid byte-preserving imports and six original range guards. OS joystick, file IO and log operations are isolated.'});
 }finally{c.game_config_delete(settings);c.deallocate(p);m.close();}
});
