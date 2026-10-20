import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';
import createFixture from '../artifacts/laser-tests/core.mjs';
const native=JSON.parse(fs.readFileSync(new URL('./native/laser-original.json',import.meta.url)));
const close=(a,b,label)=>assert.ok(Math.abs(a-b)<.00005,`${label}: ${a} vs original ${b}`);
function decode(row){
 const bytes=Buffer.from(row.profileHex,'hex'),f=o=>bytes.readFloatLE(o),i=o=>bytes.readInt32LE(o),h=o=>bytes.readInt16LE(o);
 const kind=row.factoryKind,p=Array(32).fill(0);p[0]=f(0);p[1]=f(4);p[2]=f(8);p[23]=-1;
 if(kind===0){p[3]=f(12);p[4]=f(20);p[5]=f(16);p[6]=f(24);p[7]=f(28);p[8]=f(32);p[13]=f(40);p[18]=i(44);p[20]=h(36);p[21]=h(38);p[23]=i(0x1e0);p[24]=i(0x1e4);}
 if(kind===1){p[9]=f(12);p[10]=f(16);p[11]=f(20);p[3]=f(24);p[12]=f(28);p[5]=f(32);p[4]=f(36);p[7]=f(40);p[8]=f(44);p[14]=i(48);p[15]=i(52);p[16]=i(56);p[17]=i(60);p[22]=i(72);p[13]=f(76);p[20]=h(80);p[21]=h(82);p[18]=i(84);p[23]=i(64);p[24]=i(68);}
 if(kind===2){p[3]=f(12);p[7]=f(16);p[8]=f(20);p[20]=h(24);p[21]=h(26);p[19]=i(28);p[13]=f(32);p[18]=i(36);p[23]=i(0x1d8);p[24]=i(0x1dc);}
 return p;
}
function spawn(c,row){c._test_reset();c.HEAPF32.set(decode(row),c._test_parameters()/4);assert.ok(c._test_spawn(row.factoryKind));}
function state(c){const p=c._test_state();const n=c.HEAPF32[p/4];return Array.from({length:n},(_,i)=>[...c.HEAPF32.slice(p/4+1+i*16,p/4+17+i*16)]);}
function compare(c,frame,label){const rows=state(c);assert.equal(rows.length,frame.count,label);for(let i=0;i<rows.length;i++){const a=rows[i],b=frame.nodes[i];assert.deepEqual(a.slice(0,4),[b.id,b.factoryKind,b.phase,b.age],`${label}:${i} identity/phase/age`);for(let j=0;j<3;j++)close(a[j+4],b.position[j],`${label} pos${j}`);for(let j=0;j<4;j++)close(a[j+7],b.angleLengthWidthSpeed[j],`${label} geometry${j}`);close(a[11],b.headOffset,`${label} headOffset`);assert.equal(a[12],b.retirePending,`${label} pending`);assert.equal(a[13],b.cancellable,`${label} cancellable`);}}
test('three native laser classes match 12 complete original 75-tick lifecycles and clears',async()=>{
 const c=await createFixture();let callbacks=0;for(const r of native.records){spawn(c,r);for(const frame of r.frames){const label=`kind${r.factoryKind} ${r.clearMethod} ${frame.frame}`;if(frame.afterClear)c._test_clear({all:1,rectangle:2,circle:3}[r.clearMethod],20,0,r.clearMethod==='circle'?10:30,20);else if(frame.frame>=0){c._test_tick();callbacks++;}compare(c,frame,label);}assert.equal(c._test_unsupported(),0);}assert.equal(callbacks,900);
});
test('laser point/radius query matches all 60 original boundary/rotation samples',async()=>{
 const c=await createFixture();for(const r of native.geometry)assert.equal(c._test_query(r.factoryKind,...r.point,r.angle,100,10,r.radius),r.result,JSON.stringify(r));
});
test('embedded animation hooks bind/interrupt/birth in native order, and Curve main has no per-frame bytecode tick',async()=>{
 const c=await createFixture();for(const kind of [0,1,2]){const r=native.records.find(r=>r.factoryKind===kind&&r.clearMethod==='none');spawn(c,r);assert.equal(c._test_events(),6);assert.deepEqual(Array.from({length:6},(_,i)=>[c._test_event(i,0),c._test_event(i,2)]),[[0,0],[1,0],[2,0],[0,1],[1,1],[2,1]]);c._test_tick();const extra=Array.from({length:c._test_events()-6},(_,i)=>[c._test_event(i+6,0),c._test_event(i+6,2)]);assert.deepEqual(extra,kind===0?[[2,0]]:kind===1?[[2,0],[2,1]]:[[2,1]]);}
});
test('ECL laser ID clear removes every matching TimedLaser and preserves other lookup IDs',async()=>{
 const c=await createFixture(),r=native.records.find(r=>r.factoryKind===1);spawn(c,r);c._test_spawn(1);assert.equal(c._test_find(123),1);c._test_clear_id(123);assert.equal(c._test_find(123),0);c._test_tick();assert.equal(state(c).length,0);
});
test('unrecovered laser extension is observable and cannot silently turn into a neighboring-title rule',async()=>{
 const c=await createFixture(),r=native.records.find(r=>r.factoryKind===0);spawn(c,r);c._test_extension(0,0x2000000,12);c._test_tick();assert.equal(c._test_unsupported(),1);assert.equal(state(c).length,1);
});
test('native line hurt/graze boundaries, five rotations and player state gate match original437a80',async()=>{
 const c=await createFixture(),source=JSON.parse(fs.readFileSync(new URL('./native/laser-line-original.json',import.meta.url)));
 for(const r of source.cases){const p=r.player;const result=c._test_line(r.angle,r.transverseSize,r.longitudinalSize,p.position[0],p.position[1],p.hitWidth,p.hitHeight,p.state,p.invulnerability,p.flags,+p.dialogue);assert.equal(result&3,r.nativeReturn,JSON.stringify(r));assert.equal(!!(result&4),!!r.calls438370);assert.equal(!!(result&8),r.nativeReturn===1);}
});
test('typed 602/603/611 factory parameters match 72 original ECL503/524/525 constructor buffers',async()=>{
 const c=await createFixture(),source=JSON.parse(fs.readFileSync(new URL('./native/laser-factory-original.json',import.meta.url)));
 for(const r of source.records){c._test_reset();const q=r.input,p=Array(32).fill(0);[p[4],p[5],p[6],p[7]]=q.laser600;[p[14],p[15],p[16],p[17],p[18]]=q.laser601;p[3]=q.angle;p[8]=q.speed;p[13]=q.initialOffset;p[20]=q.type;p[21]=q.color;[p[23],p[24]]=q.sounds;c.HEAPF32.set(p,c._test_parameters()/4);
  const out=c._test_factory(r.factoryKind,q.enemy[0],q.enemy[1],q.offset[0],q.offset[1],q.origin[0],q.origin[1],+q.fixedOrigin,q.lookupId),got=[...c.HEAPF32.slice(out/4,out/4+32)],expected=decode({factoryKind:r.factoryKind,profileHex:r.factory.profileHex});
  const fields=[0,1,2,3,7,8,13,18,20,21,22,23,24,...(r.factoryKind===0?[4,5,6]:r.factoryKind===1?[4,5,9,10,11,12,14,15,16,17]:[19])];for(const field of fields)close(got[field],expected[field],`${r.opcode} ${JSON.stringify(q)} field${field}`);
 }
});
