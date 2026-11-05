import test from'node:test';import assert from'node:assert/strict';import{core,oracle,memory,report}from'./helpers.mjs';
test('TH11 emitter configuration commands preserve original parameters, rank thresholds and RNG order',async()=>{
 const c=await core(),m=await oracle(),fixture=c.enemy_fixture_create(),enemy=c.enemy_fixture_data(fixture,0),p=enemy+0x103c,w=c.enemy_fixture_data(fixture,2),cc=c.allocate(0x1024),ci=c.allocate(128),n=m.allocate(0x2678),q=n+0x103c,nc=m.allocate(0x1024),ni=m.allocate(128);
 m.u32(n,0x494074);m.u32(n+4,nc);m.u32(nc+4,ni);m.u32(nc+0x1014,n);new DataView(c.memory.buffer).setUint32(cc+4,ci,true);
 const specs=new Map([[0x190,[]],[0x192,[]],[0x193,[1,2]],[0x194,[1,2]],[0x195,[1,2]],[0x196,[]],[0x197,[]],[0x198,[]],[0x199,[6,7]],[0x19b,[]],[0x1a6,[1,2,3,4,5,6]],[0x1a7,[1,2,3,4,5,6,7,8,9,10]],[0x1a8,[1,2,3,4]],[0x1a9,[]],[0x1aa,[]],[0x1ab,[]],[0x1b3,[1,2,3,4,5,6,7,8]],[0x1b4,[]],[0x1b5,[1,2]],[0x1b6,[1]],[0x1b7,[1,2]]]);
 const ranks=[-2147483648,-1024,-601,-600,-599,-513,-512,-511,-201,-200,-199,0,199,200,201,511,512,513,599,600,601,1024,2147483647];let checks=0,seed=75131;const random=()=>{seed=(Math.imul(seed,1664525)+1013904223)>>>0;return seed;};
 try{for(const[op,floats]of specs)for(let example=0;example<432;++example){
  const b=Buffer.alloc(0x163c);for(let off=0;off<b.length;off+=4)b.writeUInt32LE(random(),off);for(let off=0x124;off<0x154;off+=4)b.writeFloatLE((random()/4294967296-.5)*12,off);
  b.writeUInt32LE(enemy,0x1614);memory(c,p,b.length).set(b);b.writeUInt32LE(n,0x1614);m.write(q,b);
  const world=Buffer.alloc(12);world.writeUInt16LE(random()&65535);world.writeFloatLE(1,8);memory(c,w,12).set(world);m.write(0x4c2f00,world.subarray(0,8));const rank=ranks[example%ranks.length],difficulty=example%7-1;new DataView(c.memory.buffer).setInt32(w+0x24,rank,true);new DataView(c.memory.buffer).setInt32(w+0x28,difficulty,true);m.i32(0x4a5744,rank);m.i32(0x4a5720,difficulty);
  const ins=Buffer.alloc(128);ins.writeUInt16LE(op,4);ins.writeUInt16LE(128,6);ins[10]=255;ins[11]=11;let refs=0;
  for(let j=0;j<11;++j){if(floats.includes(j)){ins.writeFloatLE((random()/4294967296-.5)*(op===0x1b7?2200:10),16+j*4);if(example%3===2){ins.writeFloatLE(j%2?-9998:-9981,16+j*4);refs|=1<<j;}}
   else{ins.writeUInt32LE(random(),16+j*4);if(example%3===2){ins.writeInt32LE(-10000,16+j*4);refs|=1<<j;}}}
  const set=(j,v)=>{ins.writeInt32LE(v,16+j*4);refs&=~(1<<j);};set(0,example%8);if(op===0x199)set(1,example%18);if(op===0x19b)set(1,(example>>2)%8);if(op===0x1b7&&example%7===0){ins.writeFloatLE(-990,20);refs&=~2;}
  ins.writeUInt16LE(refs,8);memory(c,ci,128).set(ins);m.write(ni,ins);m.resetThreadFPU();assert.equal(c.enemy_command(fixture,cc),m.call(0x412e30,{ecx:q})|0);
  const a=Buffer.from(memory(c,p,0x163c)),expected=Buffer.from(m.bytes(q,0x163c));a.writeUInt32LE(0,0x1614);expected.writeUInt32LE(0,0x1614);if(!a.equals(expected)){let off=0;while(a[off]===expected[off])++off;off&=~3;assert.fail(`emitter 0x${op.toString(16)}/${example} at 0x${off.toString(16)} cpp=${a.readUInt32LE(off).toString(16)} native=${expected.readUInt32LE(off).toString(16)}`);}
  assert.deepEqual(Buffer.from(memory(c,w,8)),Buffer.from(m.bytes(0x4c2f00,8)),`rng ${op}/${example}`);++checks;
 }report('enemy-emitter-commands',{passed:true,checks,opcodes:[...specs.keys()],scope:'8 emitter slots, 18 transforms per emitter, full state and RNG comparison; bullet spawning and motion not included'});
 }finally{c.release(cc);c.release(ci);c.enemy_fixture_delete(fixture);m.close();}
});
