import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';import {firstDifference} from './shot-oracle.mjs';
test('TH11 fog color and distance interpolation match original arithmetic and state',async()=>{
 const c=await core(),m=await oracle(),p=c.allocate(0x8c),out=c.allocate(28),rate=c.allocate(4),q=m.allocate(0x8c),nativeOut=m.allocate(28);let checks=0;
 const normalize=b=>{b=Buffer.from(b);b.writeUInt32LE(0,0x7c);return b;};
 try{for(let mode=0;mode<18;++mode)for(const speed of [1,.375])for(const duration of [1,17,60]){
  const b=Buffer.alloc(0x8c);
  for(let k=0;k<4;++k){b.writeFloatLE(Math.fround((k+1)*31.891),k*28);b.writeFloatLE(Math.fround(2700.112/(k+1)),k*28+4);let color=0;for(let j=0;j<4;++j){const channel=Math.fround((k*19+j*41.27+5.81)%256);b.writeFloatLE(channel,k*28+8+j*4);color=(color|((Math.trunc(channel)&255)<<(j*8)))>>>0;}b.writeUInt32LE(color,k*28+24);}
  b.writeInt32LE(-1,0x70);b.writeUInt32LE(1,0x80);b.writeInt32LE(duration,0x84);b.writeInt32LE(mode,0x88);
  b.writeUInt32LE(rate,0x7c);memory(c,p,0x8c).set(b);b.writeUInt32LE(0x4a7948,0x7c);m.write(q,b);new DataView(c.memory.buffer).setFloat32(rate,speed,true);m.f32(0x4a7948,speed);
  for(let frame=0;frame<Math.ceil(duration/speed);++frame){c.fog_sample(p,out,rate);m.reg('EBX',q);m.call(0x4055d0,{args:[nativeOut]});
   const label=`mode=${mode} speed=${speed} duration=${duration} frame=${frame}`;
   assert.equal(firstDifference(Buffer.from(memory(c,out,28)),Buffer.from(m.bytes(nativeOut,28))),'',label);
   assert.equal(firstDifference(normalize(memory(c,p,0x8c)),normalize(m.bytes(q,0x8c))),'',label+' state');++checks;
  }
 }report('fog-interpolation',{passed:true,checks,modes:18,scope:'Native 4055d0 with real 4059d0/405a10/405a50/405b70 component arithmetic and color packing.'});
 }finally{c.release(p);c.release(out);c.release(rate);m.close();}
});
