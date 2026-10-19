import test from 'node:test';import assert from 'node:assert/strict';import {core,oracle,memory,report}from'./helpers.mjs';
test('TH11 configuration defaults and original validation boundary',async()=>{
 const c=await core(),m=await oracle(),p=c.allocate(60),b=c.allocate(60),bindings=c.allocate(18),np=m.allocate(60),raw=m.allocate(64),host=m.allocate(0x600);let logs=0,bytes=Buffer.alloc(60),checks=0;
 m.view(host,0x600).fill(0);
 m.replace(0x46a720,'memset',()=>{const sp=m.reg('ESP'),q=m.u32(sp+4);m.view(q,m.u32(sp+12)).fill(m.u32(sp+8)&255);return q;});
 m.replace(0x458400,'config file bytes',()=>{m.u32(m.u32(m.reg('ESP')+4),bytes.length);m.view(raw,64).fill(0);m.write(raw,bytes);return raw;},2);
 m.replace(0x458a10,'config validation log',()=>{++logs;return 0;});m.replace(0x458670,'platform apply',()=>0,1);
 try{
  m.reg('ESI',np);m.call(0x420d80);c.config_reset(p,0);assert.deepEqual(Buffer.from(memory(c,p,60)),Buffer.from(m.bytes(np,60)),'defaults after original CRT initializer 48abf0');++checks;
  for(let sample=0;sample<64;++sample){const keys=Buffer.from(Array.from({length:18},(_,i)=>(sample*13+i*7)&255));memory(c,bindings,18).set(keys);m.write(0x4c93dc,keys);m.reg('ESI',np);m.call(0x420d80);c.config_reset(p,bindings);assert.deepEqual(Buffer.from(memory(c,p,60)),Buffer.from(m.bytes(np,60)));++checks;}
  const base=Buffer.from(memory(c,p,60)),samples=[base];
  for(const[offset,limit]of [[0x1a,2],[0x1b,3],[0x1c,2],[0x1d,4],[0x1e,3],[0x1f,3]])for(const value of[0,limit-1,limit,255]){const s=Buffer.from(base);s[offset]=value;samples.push(s);}
  for(const offset of[0,1,2,3]){const s=Buffer.from(base);s[offset]^=0x40;samples.push(s);}
  samples.push(Buffer.from(base.subarray(0,59)),Buffer.concat([base,Buffer.from([0])]));
  // The original permits all volume bytes; the display/audio path clamps or
  // interprets them separately rather than changing the imported file.
  for(const value of[0,80,100,127,128,255]){const s=Buffer.from(base);s[32]=s[33]=value;samples.push(s);}
  for(bytes of samples){logs=0;m.call(0x429eb0,{args:[host]});memory(c,b,60).fill(0);memory(c,b,Math.min(60,bytes.length)).set(bytes.subarray(0,60));const before=Buffer.from(memory(c,p,60)),actual=c.config_open(p,b,bytes.length);assert.equal(actual,logs?0:1,'native validation');if(actual)assert.deepEqual(Buffer.from(memory(c,p,60)),Buffer.from(m.bytes(0x4c3448,60)));else assert.deepEqual(Buffer.from(memory(c,p,60)),before,'invalid import is transactional');++checks;}
  report('config',{passed:true,checks,scope:'Original default bytes for 64 controller maps; native 429eb0 file length, signature and mode validation including volume edge bytes. Invalid imports preserve the existing configuration.'});
 }finally{c.release(p);c.release(b);c.release(bindings);m.close();}
});
