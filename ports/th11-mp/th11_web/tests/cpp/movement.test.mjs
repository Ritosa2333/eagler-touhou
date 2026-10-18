import test from 'node:test';import assert from 'node:assert/strict';import{core,oracle,memory,report}from'./helpers.mjs';
test('TH11 straight, circular and elliptical movement matches original per frame',async()=>{
 const c=await core(),m=await oracle(),p=c.allocate(52),q=m.allocate(52);let checks=0,seed=2375;const random=()=>{seed=(Math.imul(seed,1664525)+1013904223)>>>0;return seed/4294967296;};
 try{for(let example=0;example<240;++example){const b=Buffer.alloc(52);for(let i=0;i<12;++i)b.writeFloatLE((random()-.5)*(i<6?400:i===8?100:4),i*4);b.writeUInt32LE(example%4,48);memory(c,p,52).set(b);m.write(q,b);
  for(let frame=0;frame<80;++frame){for(const kind of [0,example%3===0?2:1]){m.resetThreadFPU();m.reg('ESI',q);m.reg('EBX',q);m.call([0x459540,0x459590,0x459670,0x459a30][kind]);c.movement(p,kind);assert.deepEqual(Buffer.from(memory(c,p,52)),Buffer.from(m.bytes(q,52)),`movement ${example}/${frame}/${kind}`);++checks;}}
 }
 for(let i=-20000;i<=20000;++i){const b=Buffer.alloc(52);b.writeFloatLE(i/100+.000001,0);b.writeFloatLE(i/100-.000001,4);memory(c,p,52).set(b);m.write(q,b);m.reg('ESI',q);m.call(0x459a30);c.movement(p,3);assert.deepEqual(Buffer.from(memory(c,p,52)),Buffer.from(m.bytes(q,52)),'round '+i);++checks;}
 report('movement',{passed:true,checks,trajectoryFrames:240*80,roundingCases:40001,scope:'velocity, straight/circular/rotated-elliptical position and native hundredth-coordinate snapping'});
 }finally{c.release(p);m.close();}
});
