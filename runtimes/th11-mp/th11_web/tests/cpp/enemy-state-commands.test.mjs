import test from 'node:test';import assert from 'node:assert/strict';import{core,oracle,memory,report}from'./helpers.mjs';
const SIZE=0x163c,OFF=0x103c;
test('TH11 enemy state commands match original health, interrupts, drops, boss slots and rank decisions',async()=>{
 const c=await core(),m=await oracle(),f=c.enemy_fixture_create(),e=c.enemy_fixture_data(f,0),p=e+OFF,b=c.enemy_fixture_data(f,1),w=c.enemy_fixture_data(f,2),cw=c.enemy_fixture_data(f,4),events=c.enemy_fixture_data(f,5),cc=c.allocate(0x1024),ci=c.allocate(128),n=m.allocate(0x2678),q=n+OFF,nb=m.allocate(0x2678),nc=m.allocate(0x1024),ni=m.allocate(128),manager=m.allocate(128),hud=m.allocate(0x4500);
 let checks=0,eventChecks=0,seed=7917;const random=()=>{seed=(Math.imul(seed,1664525)+1013904223)>>>0;return seed;};let nativeEvents=[];
 m.u32(0x4a8d7c,manager);m.u32(0x4a8d84,hud);m.u32(n,0x494074);m.u32(n+4,nc);
 m.replace(0x456650,'hide animation',()=>{nativeEvents.push([m.u32(m.reg('EAX')),0]);return 0;});m.replace(0x456610,'show animation',()=>{nativeEvents.push([m.u32(m.reg('EAX')),1]);return 0;});
 const ops=[0x116,0x1f4,0x1ba,0x140,0x141,0x142,0x143,0x144,0x145,0x146,0x147,0x148,0x14a,0x14b,0x14c,0x14e,0x14f,0x154,0x155,0x15a,0x15b,0x15d,0x15e,0x15f,0x160,0x161,0x162,0x163,0x164,0x168,0x169,0x16c,0x16e,0x16f,0x170,0x171,0x172,0x174,0x1bb,0x1bc,0x1bd];
 const floats=new Map([[0x140,[0,1]],[0x141,[0,1]],[0x144,[0,1,2,3]],[0x148,[0,1]],[0x15a,[0]],[0x15b,[1]],[0x15d,[0,1,2]],[0x15e,[0,1,2,3,4]],[0x15f,[0,1,2]],[0x164,[0,1,2,3,4]],[0x16f,[0]]]);
 const rankCases=[-2147483648,-1024,-513,-512,-401,-400,-399,-201,-200,-199,0,199,200,201,511,512,513,599,600,601,1024,2147483647];
 function normalize(bytes,owner,instruction,rate){const out=Buffer.from(bytes);out.writeUInt32LE(0,0x1614);for(const o of[0x160,0x1564,0x1578])if(out.readUInt32LE(o)===rate)out.writeUInt32LE(1,o);for(let o=0x159c;o<0x1614;o+=16)for(const x of[o,o+4]){const ptr=out.readUInt32LE(x);if(ptr>=instruction&&ptr<instruction+128)out.writeUInt32LE(ptr-instruction+1,x);}return out;}
 try{
 for(const op of ops)for(let example=0;example<336;++example){
  const state=Buffer.alloc(SIZE);for(let o=0;o<SIZE;o+=4)state.writeUInt32LE(random(),o);
  for(let o=0x134;o<0x154;o+=4)state.writeFloatLE((random()/4294967296-.5)*120,o);
  state.writeUInt32LE(((example%2?0x80000:0)|(random()&0xf7f7ffff))>>>0,0x1580);state.writeInt32LE(example%8,0x158c);state.writeInt32LE(example%2?10000:-701,0x14f4);
  for(const o of[0x154,0x1558,0x156c]){state.writeInt32LE(-1,o);state.writeInt32LE(0,o+4);state.writeFloatLE(0,o+8);state.writeUInt32LE(example%2,o+16);}
  for(let o=0x1594;o<0x1614;o+=16){state.writeInt32LE(123,o);state.writeInt32LE(321,o+4);state.writeUInt32LE(0,o+8);state.writeUInt32LE(0,o+12);}
  const cs=Buffer.from(state),ns=Buffer.from(state);cs.writeUInt32LE(e,0x1614);ns.writeUInt32LE(n,0x1614);for(const o of[0x160,0x1564,0x1578]){cs.writeUInt32LE(w+8,o);ns.writeUInt32LE(0x4a7948,o);}memory(c,p,SIZE).set(cs);m.write(q,ns);
  const world=Buffer.alloc(0x44);world.writeUInt16LE(random()&65535);world.writeFloatLE(.75,8);const rank=rankCases[example%rankCases.length],difficulty=example%7-1;world.writeInt32LE(rank,0x24);world.writeInt32LE(difficulty,0x28);memory(c,w,world.length).set(world);m.write(0x4c2f00,world.subarray(0,8));m.f32(0x4a7948,.75);m.i32(0x4a5744,rank);m.i32(0x4a5720,difficulty);
  const commands=Buffer.alloc(72);for(let j=0;j<8;++j){commands.writeUInt32LE(example%3?b+OFF:0,j*4);m.u32(manager+0x1c+j*4,example%3?nb:0);}commands.writeUInt32LE(random(),32);m.u32(manager+0x3c,commands.readUInt32LE(32));for(let j=36;j<72;j+=4)commands.writeUInt32LE(random(),j);m.write(hud+0x4400,commands.subarray(36,68));m.u32(hud+0x43fc,commands.readUInt32LE(68));memory(c,cw,72).set(commands);new DataView(c.memory.buffer).setUint32(w+0x44,commands.readUInt32LE(0),true);
  const ins=Buffer.alloc(128);ins.writeUInt16LE(op,4);ins.writeUInt16LE(128,6);ins[10]=255;ins[11]=8;let refs=0;
  for(let j=0;j<8;++j){if(floats.get(op)?.includes(j)){ins.writeFloatLE((random()/4294967296-.5)*100000,16+j*4);if(example%3===2){ins.writeFloatLE(j%2?-9998:-9985,16+j*4);refs|=1<<j;}}
   else {ins.writeUInt32LE(random(),16+j*4);if(example%3===2){ins.writeInt32LE(-10000,16+j*4);refs|=1<<j;}}
  }
  const setInt=(index,value)=>{ins.writeInt32LE(value,16+index*4);refs&=~(1<<index);};
  if(op===0x147)setInt(0,example%13);if(op===0x14a)setInt(0,example%13);if(op===0x14c)setInt(0,example%10-2);if(op===0x14e||op===0x155)setInt(0,example%8);if(op===0x15b)setInt(0,example%4);
  if(op>=0x15d&&op<=0x164){refs|=1;if(floats.has(op))ins.writeFloatLE(example%2?-9981:0,16);else ins.writeInt32LE(example%2?-9985:0,16);}
  if(op>=0x1bb&&op<=0x1bd)setInt(0,example%(op===0x1bb?13:2));
  ins.writeUInt16LE(refs,8);memory(c,ci,128).set(ins);m.write(ni,ins);
  const ctx=Buffer.alloc(0x1024);ctx.writeFloatLE(51.25,0);ctx.writeFloatLE(71.5,8);ctx.writeUInt32LE(ci,4);ctx.writeUInt32LE(e,0x1014);memory(c,cc,ctx.length).set(ctx);ctx.writeUInt32LE(ni,4);ctx.writeUInt32LE(n,0x1014);m.write(nc,ctx);
  nativeEvents=[];new DataView(c.memory.buffer).setUint32(events,0,true);m.resetThreadFPU();const expected=m.call(0x412e30,{ecx:q})|0;assert.equal(c.enemy_command(f,cc),expected,`return ${op.toString(16)}/${example}`);
  const actual=normalize(memory(c,p,SIZE),e,ci,w+8),native=normalize(m.bytes(q,SIZE),n,ni,0x4a7948);
  if(op>=0x1bb&&op<=0x1bd){const offset=0x1630+(op-0x1bb)*4,table=[0x4a3d64,0x4a3d98,0x4a3da0][op-0x1bb],index=ins.readInt32LE(16);assert.equal(native.readUInt32LE(offset),m.u32(table+index*4),'original callback table');native.writeUInt32LE(index,offset);}
  if(!actual.equals(native)){let o=0;while(actual[o]===native[o])++o;o&=~3;assert.fail(`state 0x${op.toString(16)}/${example} offset 0x${o.toString(16)} cpp=${actual.readUInt32LE(o).toString(16)} native=${native.readUInt32LE(o).toString(16)}`);}
  assert.deepEqual(Buffer.from(memory(c,cc,4)),Buffer.from(m.bytes(nc,4)),`time ${op}/${example}`);assert.deepEqual(Buffer.from(memory(c,cc+8,4096)),Buffer.from(m.bytes(nc+8,4096)),`locals ${op}/${example}`);
  assert.deepEqual(Buffer.from(memory(c,w,12)),Buffer.from(Buffer.concat([Buffer.from(m.bytes(0x4c2f00,8)),Buffer.from(m.bytes(0x4a7948,4))])),`rng/rate ${op}/${example}`);
  const output=Buffer.from(memory(c,cw,72));for(let j=0;j<8;++j){const ptr=m.u32(manager+0x1c+j*4);assert.equal(output.readUInt32LE(j*4),ptr===n?p:ptr===nb?b+OFF:0,`boss ${op}/${example}/${j}`);}assert.equal(output.readUInt32LE(32),m.u32(manager+0x3c));assert.deepEqual(output.subarray(36,68),Buffer.from(m.bytes(hud+0x4400,32)));assert.equal(output.readUInt32LE(68),m.u32(hud+0x43fc));
  const ev=Buffer.from(memory(c,events,516)),actualEvents=Array.from({length:ev.readUInt32LE(0)},(_,j)=>[ev.readUInt32LE(4+j*8),ev.readUInt32LE(8+j*8)]);assert.deepEqual(actualEvents,nativeEvents,`visibility ${op}/${example}`);eventChecks+=actualEvents.length;++checks;
 }
 // Native replay-section setter keeps elapsed frames when the section is
 // unchanged and resets them only on an actual transition.
 const section=c.enemy_fixture_data(f,6);
 for(const old of [0,1,23,24,-1])for(const next of [old,0,2,24]){
  const view=new DataView(c.memory.buffer);view.setInt32(section,old,true);view.setInt32(section+4,371,true);
  m.i32(0x4a5730,old);m.i32(0x4a5738,371);
  const ins=Buffer.alloc(128);ins.writeUInt16LE(0x158,4);ins.writeUInt16LE(20,6);ins.writeInt32LE(next,16);
  memory(c,ci,128).set(ins);m.write(ni,ins);
  assert.equal(c.enemy_command(f,cc),m.call(0x412e30,{ecx:q})|0);
  assert.equal(new DataView(c.memory.buffer).getInt32(section,true),m.i32(0x4a5730));
  assert.equal(new DataView(c.memory.buffer).getInt32(section+4,true),m.i32(0x4a5738));++checks;
 }
 ops.push(0x158);
 report('enemy-state-commands',{passed:true,checks,eventChecks,opcodes:ops,scope:'full enemy state, local variables, RNG, timers, replay section transitions, health HUD, boss registry and animation visibility callback order; callback rendering is tested separately'});
 }finally{c.release(cc);c.release(ci);c.enemy_fixture_delete(f);m.close();}
});
