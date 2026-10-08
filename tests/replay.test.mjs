import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';import path from 'node:path';import {parseReplay,replayInputEnded,decryptBytes} from '../src/replay.mjs';import {startReplayStage} from '../src/replay-session.mjs';import {allowedPath} from '../src/storage.mjs';
const root=path.resolve(import.meta.dirname,'..');
test('all supplied retail fixtures decode with complete bounded input streams',()=>{
  const corpus=JSON.parse(fs.readFileSync(path.join(root,'tools/replay-verifier/corpus.json')));
  const supplied=fs.readdirSync(path.join(root,'../th12/replay')).filter(n=>/\.rpy$/i.test(n)),demos=fs.readdirSync(path.join(root,'local/retail')).filter(n=>/^demo\d\.rpy$/.test(n));
  assert.deepEqual(corpus.fixtures.map(f=>f.id).sort(),[...supplied,...demos].sort());for(const f of corpus.fixtures){const r=parseReplay(fs.readFileSync(path.join(root,'site/replays',f.id)));assert.equal(r.character,f.character);assert.equal(r.shot,f.shot);assert.equal(r.difficulty,f.difficulty);for(const s of r.stages){assert.ok(r.input(s.number,s.frames-1));assert.equal(r.input(s.number,s.frames),null);}}
});
test('malformed and oversized replays fail before large allocations',()=>{
  const b=fs.readFileSync(path.join(root,'site/replays/demo2.rpy'));assert.throws(()=>parseReplay(b.subarray(0,35)));const n=Buffer.from(b);n.writeUInt32LE(0x7fffffff,32);assert.throws(()=>parseReplay(n));const wrong=Buffer.from(b);wrong[0]=0;assert.throws(()=>parseReplay(wrong));
});
test('all current stage headers use native signed16 power/life/bomb counters and separate fragments',()=>{
  const corpus=JSON.parse(fs.readFileSync(path.join(root,'tools/replay-verifier/corpus.json')));let count=0;
  for(const fixture of corpus.fixtures){const replay=parseReplay(fs.readFileSync(path.join(root,'site/replays',fixture.id))),view=new DataView(replay.data.buffer);
    for(const stage of replay.stages){const initial=stage.initial;
      assert.equal(initial.power,view.getInt16(stage.offset+16,true));
      assert.equal(initial.lives,view.getInt16(stage.offset+24,true));assert.equal(initial.lifeFragments,view.getInt16(stage.offset+26,true));
      assert.equal(initial.bombs,view.getInt16(stage.offset+28,true));assert.equal(initial.bombFragments,view.getInt16(stage.offset+30,true));
      assert.ok(initial.lives<=9&&initial.bombs<=5&&initial.lifeFragments<=4&&initial.bombFragments<=4);
      const {ufoColors,rank,nativeGlobals}=initial;assert.deepEqual(initial,fixture.stages.find(s=>s.number===stage.number).initial);
      assert.deepEqual(ufoColors,[32,36,40].map(at=>view.getInt32(stage.offset+at,true)));assert.equal(rank,view.getInt32(stage.offset+44,true));assert.deepEqual(nativeGlobals,[56,60,64,68].map(at=>view.getInt32(stage.offset+at,true)));count++;
    }
  }
  assert.equal(count,corpus.fixtures.reduce((sum,f)=>sum+f.stages.length,0));
  const replay=parseReplay(fs.readFileSync(path.join(root,'site/replays/th12_03.rpy'))),initial=replay.stages.find(s=>s.number===5).initial;
  assert.equal(initial.lives,6);assert.equal(initial.lifeFragments,4);assert.equal(initial.bombs,2);assert.equal(initial.bombFragments,2);
});
test('storage restricts operations to native user file names',()=>{
  assert.equal(allowedPath('/savesth12/replay/th12_import_1.rpy'),'/savesth12/replay/th12_import_1.rpy');for(const path of ['/savesth12/../th12.dat','/savesth12/th12.exe','/other/replay/th12_01.rpy','/savesth12/replay/../th12.cfg'])assert.throws(()=>allowedPath(path));
});

// Build fresh synthetic literal records instead of borrowing a deleted or
// copied stale user replay for signed-field/sentinel edge cases.
function encrypted(input,key,step,block){
  const initialKey=key,out=new Uint8Array(input),untouched=(input.length%block<block/4?input.length%block:0)+(input.length&1);
  let cursor=0,remaining=input.length-untouched;
  while(remaining>0){const n=Math.min(block,remaining);let k=0;
    for(let parity=1;parity<=2;parity++)for(let pos=n-parity;pos>=0;pos-=2){out[cursor+k++]=input[cursor+pos]^key;key=(key+step)&255;}
    cursor+=n;remaining-=n;
  }
  assert.deepEqual(decryptBytes(out,initialKey,step,block),input,'synthetic encrypted block roundtrip');
  return out;
}
function literalReplay(){
  const frames=3,raw=Buffer.alloc(112+160+frames*6+1),view=new DataView(raw.buffer,raw.byteOffset,raw.byteLength),stage=112;
  raw.write('TEST    ');raw.writeUInt32LE(1,88);raw.writeUInt32LE(2,92);raw.writeUInt32LE(1,96);raw.writeUInt32LE(1,100);
  raw.writeUInt16LE(2,stage);raw.writeUInt16LE(0x1234,stage+2);raw.writeUInt32LE(frames,stage+4);raw.writeUInt32LE(frames*6+1,stage+8);
  raw.writeInt32LE(123456,stage+12);raw.writeInt16LE(-143,stage+16);raw.writeUInt16LE(0x7777,stage+18);raw.writeInt32LE(12345000,stage+20);
  [-1,-2,-3,-4].forEach((n,i)=>raw.writeInt16LE(n,stage+24+i*2));[1,2,0].forEach((n,i)=>raw.writeInt32LE(n,stage+32+i*4));
  raw.writeInt32LE(42,stage+44);raw.writeInt32LE(768,stage+48);raw.writeInt32LE(49920,stage+52);
  [314,3,1,9876].forEach((n,i)=>raw.writeInt32LE(n,stage+56+i*4));
  [[0x81,0x81,0],[0xffff,0,0],[0xffff,0xffff,0xffff]].forEach((words,frame)=>words.forEach((word,i)=>view.setUint16(stage+160+frame*6+i*2,word,true)));
  const bits=[];for(const byte of raw){bits.push(1);for(let bit=7;bit>=0;bit--)bits.push((byte>>bit)&1);}bits.push(...Array(14).fill(0));
  const compressed=new Uint8Array(Math.ceil(bits.length/8));bits.forEach((bit,index)=>compressed[index>>3]|=bit<<(7-(index&7)));
  const inner=encrypted(compressed,0x7d,0x3a,0x40),outer=encrypted(inner,0x5e,0xe1,0x800),file=Buffer.alloc(36+outer.length);
  file.writeUInt32LE(0x72323174);file.writeUInt16LE(4,4);file.writeUInt32LE(outer.length,28);file.writeUInt32LE(raw.length,32);Buffer.from(outer).copy(file,36);
  return file;
}
test('fresh synthetic replay preserves signed short fields and a held-onlyffff input',()=>{
  const replay=parseReplay(literalReplay()),stage=replay.stages[0];
  assert.deepEqual([stage.initial.power,stage.initial.lives,stage.initial.lifeFragments,stage.initial.bombs,stage.initial.bombFragments],[-143,-1,-2,-3,-4]);
  assert.deepEqual(replay.input(2,0),{held:0x81,pressed:0x81,released:0});
  assert.equal(replayInputEnded(replay.input(2,1)),false);assert.equal(replayInputEnded(replay.input(2,2)),true);
  assert.equal(replayInputEnded(replay.input(2,3)),true);
});
test('end marker helper agrees with original43b950 three-word decisions',()=>{
  const native=JSON.parse(fs.readFileSync(path.join(root,'tests/fixtures/replay-input-order-original.json')));
  assert.equal(native.originalReplayLoaded,false);assert.equal(native.wholeGameOracle,false);
  for(const record of native.records.filter(r=>r.name.includes('ffff'))){const[held,pressed,released]=record.triples[0],read=record.steps.find(s=>s.step==='read0');
    assert.equal(replayInputEnded({held,pressed,released}),read.events.includes('replay-end-menu'),record.name);
  }
  const startup=native.startup,birth=startup[0],input0=startup[1],firstVisit=startup[2],secondVisit=startup[4];
  assert.deepEqual(input0.input.slice(3),[0x81,0]);assert.equal(input0.inputCursor,1);
  assert.equal(firstVisit.age,birth.age);assert.deepEqual(firstVisit.position,birth.position);
  assert.equal(firstVisit.contextTime,birth.contextTime);assert.equal(birth.flags&0x20000,0x20000);assert.equal(firstVisit.flags&0x20000,0);
  assert.deepEqual(secondVisit.position,[11,100]);
});

function fixtureCore(){
  const core={HEAPU8:new Uint8Array(256),calls:[],_malloc:n=>{core.calls.push(['malloc',n]);return 64;},_free:p=>core.calls.push(['free',p])};
  core._th12_start_replay=(...args)=>{const[c,s,d,seed,p,n]=args,view=new DataView(core.HEAPU8.buffer);
    core.calls.push(['start',c,s,d,seed,Array.from({length:n},(_,i)=>view.getInt32(p+i*4,true))]);return 1;};
  for(const method of ['_th12_start','_th12_set_initial','_th12_set_replay_economy'])core[method]=()=>{throw Error('late restore forbidden');};
  return core;
}
test('replay session sends all17 native fields atomically before the core owns main birth',()=>{
  const replay=parseReplay(literalReplay()),stage=replay.stages[0],before=structuredClone(stage),core=fixtureCore();
  startReplayStage(core,replay,stage);
  assert.deepEqual(core.calls,[['malloc',68],['start',2,1,1,0x1234,[768,49920,-143,-1,-3,123456,12345000,-2,-4,1,2,0,42,314,3,1,9876]],['free',64]]);
  assert.deepEqual(stage,before,'original stage record is read-only');
});
test('missing or invalid atomic replay initialization fails explicitly and frees the allocated header',()=>{
  const replay=parseReplay(literalReplay()),stage=replay.stages[0];
  const old=fixtureCore();delete old._th12_start_replay;assert.throws(()=>startReplayStage(old,replay,stage),/更新核心/);assert.deepEqual(old.calls,[]);
  const failed=fixtureCore();failed._th12_start_replay=()=>0;assert.throws(()=>startReplayStage(failed,replay,stage),/初始化失败/);assert.deepEqual(failed.calls,[['malloc',68],['free',64]]);
  const escaped=fixtureCore();escaped._malloc=()=>250;assert.throws(()=>startReplayStage(escaped,replay,stage),/分配失败/);assert.deepEqual(escaped.calls,[['free',250]]);
  for(const invalid of[{...stage,seed:-1},{...stage,initial:{...stage.initial,power:1.5}},
    {...stage,initial:{...stage.initial,nativeGlobals:[0,0,0]}},{...stage,initial:{...stage.initial,ufoColors:[0,0]}}]){
    const core=fixtureCore();assert.throws(()=>startReplayStage(core,replay,invalid),/无效/);assert.deepEqual(core.calls,[]);
  }
});
