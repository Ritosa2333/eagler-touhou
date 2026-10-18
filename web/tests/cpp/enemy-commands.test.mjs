import test from 'node:test';
import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';
const SIZE=0x163c,OFF=0x103c;
const timers=[0x154,0x174+48,0x1c0+48,...[0x20c,0x248,0x284,0x2c0,0x2fc,0x338].map(x=>x+32),0x1558,0x156c];
function normalized(bytes){const b=Buffer.from(bytes);for(const o of timers)b.writeUInt32LE(0,o+12);b.writeUInt32LE(0,0x1614);b.writeUInt32LE(0,0x168);return b;}
test('TH11 enemy movement commands match the original dispatcher including references and RNG order',async()=>{
 const c=await core(),m=await oracle(),fixture=c.enemy_fixture_create(),enemy=c.enemy_fixture_data(fixture,0),p=enemy+OFF,boss=c.enemy_fixture_data(fixture,1),world=c.enemy_fixture_data(fixture,2),context=c.allocate(0x1024),instruction=c.allocate(128),n=m.allocate(0x2678),q=n+OFF,nb=m.allocate(0x2678),nc=m.allocate(0x1024),ni=m.allocate(128),nm=m.allocate(128),np=m.allocate(0x900);
 let checks=0,randomChecks=0,seed=17391;const random=()=>{seed=(Math.imul(seed,1664525)+1013904223)>>>0;return seed;};const rand=()=>random()/4294967296;
 m.u32(0x4a8d7c,nm);m.u32(nm+0x1c,nb);m.u32(0x4a8eb4,np);m.u32(n,0x494074);m.u32(n+4,nc);m.u32(nc+4,ni);m.u32(nc+0x1014,n);
 const contextBytes=Buffer.alloc(0x1024);contextBytes.writeUInt32LE(instruction,4);contextBytes.writeUInt32LE(enemy,0x1014);memory(c,context,contextBytes.length).set(contextBytes);
 const floatArgs=new Map([
  [0x118,[0,1]],[0x11a,[0,1]],[0x119,[2,3]],[0x11b,[2,3]],
  [0x11c,[0,1]],[0x11e,[0,1]],[0x11d,[2,3]],[0x11f,[2,3]],
  [0x120,[0,1,2,3]],[0x122,[0,1,2,3]],[0x121,[2,3,4]],[0x123,[2,3,4]],
  [0x124,[2]],[0x125,[2]],[0x126,[]],[0x127,[]],[0x128,[0,1,2]],[0x129,[0,1,2]],
  [0x12a,[0,1,2,3,4,5]],[0x12b,[0,1,2,3,4,5]],[0x12c,[0,1,2,3,4,5]],[0x12e,[0,1,2,3,4,5]],
  [0x12d,[2,3,4,5,6,7]],[0x12f,[2,3,4,5,6,7]],[0x130,[]],[0x131,[1,2,3,4,5,6]],[0x132,[1,2,3,4,5,6]],[0x133,[]]
 ]);
 try{
 for(const[op,args]of floatArgs)for(let example=0;example<252;++example){
  const state=Buffer.alloc(SIZE);for(const o of[0,0x34,0x68,0x9c]){for(let j=0;j<12;++j)state.writeFloatLE((rand()-.5)*(j<6?500:j===8?70:6),o+j*4);state.writeUInt32LE(example%4,o+48);}
  for(let o=0x124;o<0x154;o+=4)state.writeFloatLE((rand()-.5)*10,o);
  for(const[o,components]of[[0x174,3],[0x1c0,3],[0x20c,2],[0x248,2],[0x284,2],[0x2c0,2],[0x2fc,2],[0x338,2]]){for(let j=0;j<components*4;++j)state.writeFloatLE((rand()-.5)*20,o+j*4);state.writeInt32LE(3,o+components*16);state.writeInt32LE(4,o+components*16+4);state.writeFloatLE(4.25,o+components*16+8);state.writeUInt32LE(example%2,o+components*16+16);state.writeInt32LE(9,o+components*16+20);state.writeInt32LE(example%18,o+components*16+24);}
  const flags=(example%2?0x8000:0)|(example%3?0x2000:0);state.writeUInt32LE(flags,0x1580);state.writeFloatLE(0,0x14dc);state.writeFloatLE(220,0x14e0);state.writeFloatLE(380,0x14e4);state.writeFloatLE(440,0x14e8);
  const cs=Buffer.from(state),ns=Buffer.from(state);for(const o of timers){cs.writeUInt32LE(world+8,o+12);ns.writeUInt32LE(0x4a7948,o+12);}cs.writeUInt32LE(enemy,0x1614);ns.writeUInt32LE(n,0x1614);memory(c,p,SIZE).set(cs);m.write(q,ns);
  const w=Buffer.alloc(24);w.writeUInt16LE(random()&65535);w.writeFloatLE(example%2?.375:1,8);w.writeFloatLE((rand()-.5)*300,12);w.writeFloatLE(rand()*440,16);memory(c,world,24).set(w);m.write(0x4c2f00,w.subarray(0,8));m.write(0x4a7948,w.subarray(8,12));m.write(np+0x87c,w.subarray(12,24));
  const bossPosition=Buffer.alloc(12);for(let j=0;j<3;++j)bossPosition.writeFloatLE((rand()-.5)*200,j*4);memory(c,boss+OFF+0x34,12).set(bossPosition);m.write(nb+OFF+0x34,bossPosition);
  const ins=Buffer.alloc(128);ins.writeUInt16LE(op,4);ins.writeUInt16LE(128,6);ins[10]=255;ins[11]=8;ins.writeInt32LE(example%43-2,16);ins.writeInt32LE(example%18,20);let references=0;
  for(const index of args){let value=(rand()-.5)*8;if((example+index)%7===0)value=-999999;else if((example+index)%11===0)value=-1000000;
   if(example%4===3){value=[-9999,-9998,-9987,-9981,-9971,-9969][(example+index)%6];references|=1<<index;}
   ins.writeFloatLE(value,16+index*4);
  }
  // References to the same RNG variable reveal non-source-order argument reads.
  if(example%9===0){for(const index of args){ins.writeFloatLE(-9998,16+index*4);references|=1<<index;}}
  ins.writeUInt16LE(references,8);memory(c,instruction,128).set(ins);m.write(ni,ins);
  m.resetThreadFPU();const expected=m.call(0x412e30,{ecx:q});assert.equal(c.enemy_command(fixture,context),expected,`return ${op.toString(16)}/${example}`);
  const a=normalized(memory(c,p,SIZE)),b=normalized(m.bytes(q,SIZE));if(!a.equals(b)){let o=0;while(a[o]===b[o])++o;o&=~3;assert.fail(`command 0x${op.toString(16)} case ${example} offset 0x${o.toString(16)} cpp=${a.readFloatLE(o)} (${a.readUInt32LE(o).toString(16)}) native=${b.readFloatLE(o)} (${b.readUInt32LE(o).toString(16)})`);}++checks;
  assert.deepEqual(Buffer.from(memory(c,world,8)),Buffer.from(m.bytes(0x4c2f00,8)),`rng ${op.toString(16)}/${example}`);++randomChecks;
 }
 report('enemy-commands',{passed:true,checks,randomChecks,opcodes:[...floatArgs.keys()],scope:'all movement commands 0x118–0x133; complete enemy-state byte comparison, RNG state, sentinel inputs, global references, all interpolation modes and preserved center-command fallthrough'});
 }finally{c.release(context);c.release(instruction);c.enemy_fixture_delete(fixture);m.close();}
});
