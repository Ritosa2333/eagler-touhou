import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';
import createCore from '../site/runtime/core.mjs';
import {startReplayStage} from '../src/replay-session.mjs';
import {readCoreAnimationPoses} from '../src/renderer.mjs';
const word=n=>{const b=Buffer.alloc(4);b.writeInt32LE(n);return b;},float=n=>{const b=Buffer.alloc(4);b.writeFloatLE(n);return b;};
function ins(op,args=[],time=0,mask=0){const b=Buffer.alloc(16+args.length*4);b.writeInt32LE(time);b.writeUInt16LE(op,4);b.writeUInt16LE(b.length,6);b.writeUInt16LE(mask,8);b[10]=63;b[11]=args.length;Buffer.concat(args).copy(b,16);return b;}
function program(code){const b=Buffer.alloc(48);b.write('SCPT');b.writeUInt16LE(1,4);b.writeUInt16LE(1,16);b.writeUInt32LE(48,36);b.write('main\0',40);const h=Buffer.alloc(16);h.write('ECLH');return Buffer.concat([b,h,...code,ins(10)]);}
function load(c,bytes,method,index=null){const p=c._malloc(bytes.length);c.HEAPU8.set(bytes,p);assert.equal(index===null?c[method](p,bytes.length):c[method](index,p,bytes.length),1);c._free(p);}
function snapshot(c){const p=c._th12_state(),n=c._th12_state_size();return JSON.parse(new TextDecoder().decode(c.HEAPU8.subarray(p,p+n)));}
const hold=(time=0)=>ins(83,[word(100000)],time),header={number:1,seed:0x3456,initial:{x:768,y:49920,power:143,lives:2,bombs:2,score:1234567,pointValue:12345000,lifeFragments:4,bombFragments:2,ufoColors:[0,0,0],rank:42,nativeGlobals:[314,3,1,9876]}};

test('real core preserves native birth/input0 guard and reads input1 without a replay offset',async()=>{
  const native=JSON.parse(fs.readFileSync(new URL('fixtures/replay-input-order-original.json',import.meta.url))),c=await createCore();
  load(c,program([ins(300,[float(0),float(100)]),ins(300,[float(11),float(100)],1),ins(300,[float(22),float(100)],2),hold(2)]),'_th12_load_ecl');
  startReplayStage(c,{character:2,shot:1,difficulty:1},header);
  const compare=index=>{const s=snapshot(c),e=s.enemies[0],context=s.ecl.find(r=>r.enemy===e.id),expected=native.startup[index];assert.deepEqual(e.position.slice(0,2),expected.position);assert.equal(e.age,expected.age);assert.equal(context.time,expected.contextTime);assert.equal(e.flags&0x20000,expected.flags&0x20000);};
  compare(0);c._th12_tick(0x81,0x81);compare(2);c._th12_tick(0x40,0x40);compare(4);c._th12_tick(0,0);compare(6);
});
test('retail stage values and player position are restored before the synchronous main script',async()=>{
  const c=await createCore();load(c,program([ins(300,[float(-9991),float(-9990)],0,3),hold()]),'_th12_load_ecl');startReplayStage(c,{character:2,shot:1,difficulty:1},header);
  const s=snapshot(c);assert.deepEqual(s.enemies[0].position.slice(0,2),[6,390]);
  assert.deepEqual([s.player.xFixed,s.player.yFixed,s.player.graze],[768,49920,9876]);assert.deepEqual(s.player.replayInitialGlobals,header.initial.nativeGlobals);
  assert.deepEqual(s.economy.resources.slice(0,11),[143,400,100,1234567,12345000,20000000,2,2,4,2,42]);assert.deepEqual([s.rng.seed,s.rng.calls],[0x3456,0]);
});
test('Bomb responds to the held input domain, including a held button without a new press',async()=>{
  const c=await createCore();load(c,program([hold()]),'_th12_load_ecl');c._th12_start(0,0,1,1234);
  c._th12_tick(0,2);assert.equal(snapshot(c).economy.resources[7],2,'pressed-only is not the native Bomb domain');
  c._th12_tick(2,0);assert.equal(snapshot(c).economy.resources[7],1);assert.equal(Boolean(snapshot(c).player.bombState.active),true);
  c._th12_tick(2,0);assert.equal(snapshot(c).economy.resources[7],1,'active Bomb blocks duplicate starts');
});
function anmInstruction(op,values=[]){const b=Buffer.alloc(8+values.length*4);b.writeUInt16LE(op);b.writeUInt16LE(b.length,2);values.forEach((n,i)=>b.writeInt32LE(n,8+i*4));return b;}
function bulletBank(rotate){
  const scripts=Array.from({length:36},(_,i)=>Buffer.concat([...(i===35?[anmInstruction(3,[0]),anmInstruction(67,[1]),anmInstruction(82,[rotate?1:0]),anmInstruction(49,[0,0,float(.375).readInt32LE()])]:[]),anmInstruction(63),anmInstruction(65535)]));
  const h=Buffer.alloc(64+4+36*8);h.writeUInt32LE(7);h.writeUInt16LE(1,4);h.writeUInt16LE(36,6);h.writeUInt16LE(32,10);h.writeUInt16LE(32,12);h.writeUInt32LE(h.length,64);
  const sprite=Buffer.alloc(20);sprite.writeFloatLE(8,12);sprite.writeFloatLE(8,16);let cursor=h.length+sprite.length;
  scripts.forEach((s,i)=>{h.writeInt32LE(i,68+i*8);h.writeUInt32LE(cursor,72+i*8);cursor+=s.length;});return Buffer.concat([h,sprite,...scripts]);
}
test('hostile draw-time headings match original40a150 and preserve gameplay state',async()=>{
  const native=JSON.parse(fs.readFileSync(new URL('fixtures/bullet-heading-native.json',import.meta.url)));
  for(const row of native.rows){const c=await createCore();load(c,bulletBank(row.rotate),'_th12_load_anm',0);
    load(c,program([ins(500,[word(0)]),ins(502,[word(0),word(0),word(0)]),ins(504,[word(0),float(row.angle),float(0)]),ins(505,[word(0),float(1),float(1)]),ins(506,[word(0),word(1),word(1)]),ins(507,[word(0),word(1)]),ins(501,[word(0)]),hold()]),'_th12_load_ecl');c._th12_start(0,0,1,1234);
    const before=snapshot(c),bullet=before.bullets.find(b=>!b.friendly),pose=readCoreAnimationPoses(c).find(p=>p.owner===0x10000000+bullet.id);
    assert.ok(pose);assert.equal(pose.a.rotation,row.rotation,`angle=${row.angle} rotate=${row.rotate}`);assert.equal(pose.a.rotate,true);assert.deepEqual(snapshot(c),before,'draw reader cannot advance animation or motion');
  }
});
