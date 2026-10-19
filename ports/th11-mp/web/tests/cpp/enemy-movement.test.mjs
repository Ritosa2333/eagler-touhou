import test from 'node:test';import assert from 'node:assert/strict';import{core,oracle,memory,report}from'./helpers.mjs';
const S=0x163c,OFF=0x103c;
const interpolations=[[0x174,3],[0x1c0,3],[0x20c,2],[0x248,2],[0x284,2],[0x2c0,2]];
function normalized(b){b=Buffer.from(b);for(const[o,n]of interpolations)b.writeUInt32LE(0,o+n*16+12);b.writeUInt32LE(0,0x160);b.writeUInt32LE(0,0x1614);return b;}
test('TH11 enemy movement, interpolation, clamps and visibility match original update prefix',async()=>{
 const c=await core(),m=await oracle(),f=c.enemy_fixture_create(),p=c.enemy_fixture_data(f,0)+OFF,w=c.enemy_fixture_data(f,2),q=m.allocate(S);let checks=0,visibilityChecks=0,seed=9577;
 const random=()=>{seed=(Math.imul(seed,1664525)+1013904223)>>>0;return seed/4294967296;};
 const compare=label=>{const a=normalized(memory(c,p,S)),b=normalized(m.bytes(q,S));if(!a.equals(b)){let o=0;while(a[o]===b[o])++o;assert.fail(`${label} offset 0x${(o&~3).toString(16)} cpp=${a.readFloatLE(o&~3)} native=${b.readFloatLE(o&~3)}`);}++checks;};
 const load=(b,rate)=>{const cb=Buffer.from(b),nb=Buffer.from(b);for(const[o,n]of interpolations){cb.writeUInt32LE(w+8,o+n*16+12);nb.writeUInt32LE(0x4a7948,o+n*16+12);}cb.writeUInt32LE(w+8,0x160);nb.writeUInt32LE(0x4a7948,0x160);memory(c,p,S).set(cb);m.write(q,nb);new DataView(c.memory.buffer).setFloat32(w+8,rate,true);m.f32(0x4a7948,rate);};
 try{
  // Native script callback is outside the scope of this prefix comparison.
  // Return -1 after movement; distinguish callback reached from early despawn.
  let reached=false;m.replace(0x45d420,'movement prefix boundary',()=>{reached=true;return -1;},1);
  const delta=Buffer.alloc(12);delta.writeFloatLE(.37);delta.writeFloatLE(-.41,4);delta.writeFloatLE(.25,8);memory(c,w+24,12).set(delta);m.write(0x4c3574,delta);
  for(let example=0;example<252;++example){const b=Buffer.alloc(S);for(const o of [0x34,0x68,0x9c]){for(let i=0;i<12;++i)b.writeFloatLE((random()-.5)*(i<6?100:i===8?60:3),o+i*4);b.writeUInt32LE(o===0x34?0:example%4,o+48);}
   b.writeFloatLE(32,0x14d4);b.writeFloatLE(32,0x14d8);b.writeFloatLE(0,0x14dc);b.writeFloatLE(224,0x14e0);b.writeFloatLE(360,0x14e4);b.writeFloatLE(420,0x14e8);const flags=(example%2?0x2000:0)|(example%3?0:0x400000);b.writeUInt32LE(flags,0x1580);
   for(let k=0;k<interpolations.length;++k){const[o,n]=interpolations[k];if((example+k)%3===0)continue;for(let j=0;j<n*4;++j)b.writeFloatLE((random()-.5)*(k<2?120:2),o+j*4);b.writeInt32LE(-1,o+n*16);b.writeUInt32LE(1,o+n*16+16);b.writeInt32LE(17+(example%5),o+n*16+20);b.writeInt32LE(example%18,o+n*16+24);}
   load(b,example%2?.375:1);
   for(let frame=0;frame<48;++frame){const dv=new DataView(c.memory.buffer);dv.setUint32(p+0x1580,dv.getUint32(p+0x1580,true)&~0x4000,true);m.u32(q+0x1580,m.u32(q+0x1580)&~0x4000);reached=false;m.resetThreadFPU();m.call(0x411750,{args:[q]});const actual=c.enemy_movement(f,0);assert.equal(actual,reached?0:-1,`visibility ${example}/${frame}`);++visibilityChecks;compare(`movement ${example}/${frame}`);if(actual===-1)break;}
  }
  for(const x of [-209,-208,-207,-192,0,192,207,208,209])for(const y of [-17,-16,-15,0,224,448,463,464,465])for(const flags of [0,4,8,12,0x1000,0x1004,0x1008,0x100c]){const b=Buffer.alloc(S);b.writeFloatLE(x,0x68);b.writeFloatLE(y,0x6c);b.writeFloatLE(32,0x14d4);b.writeFloatLE(32,0x14d8);b.writeUInt32LE(flags,0x1580);load(b,1);reached=false;m.call(0x411750,{args:[q]});assert.equal(c.enemy_movement(f,0),reached?0:-1,`edge ${x}/${y}/${flags}`);++visibilityChecks;compare('edge');}
  // combine alone with fractional bounds and already circular current motion.
  for(let i=0;i<1000;++i){const b=Buffer.alloc(S);for(const o of[0x34,0x68,0x9c]){for(let j=0;j<12;++j)b.writeFloatLE((random()-.5)*500,o+j*4);b.writeUInt32LE(i%4,o+48);}for(let o=0x14dc;o<0x14ec;o+=4)b.writeFloatLE((random()-.5)*400,o);b.writeUInt32LE(i%2?0x2000:0,0x1580);load(b,1);m.reg('ESI',q);m.call(0x411610);c.enemy_movement(f,1);compare(`combine ${i}`);}
  report('enemy-movement',{passed:true,checks,visibilityChecks,scope:'original 0x411750 prefix through movement/visibility, with script callback intercepted; 0x411610 combine including clamp and circular current state'});
 }finally{c.enemy_fixture_delete(f);m.close();}
});