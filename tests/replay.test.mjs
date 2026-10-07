import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';import path from 'node:path';import {parseReplay} from '../src/replay.mjs';import {allowedPath} from '../src/storage.mjs';
const root=path.resolve(import.meta.dirname,'..');
test('all supplied retail fixtures decode with complete bounded input streams',()=>{
  const corpus=JSON.parse(fs.readFileSync(path.join(root,'tools/replay-verifier/corpus.json')));
  const supplied=fs.readdirSync(path.join(root,'../th12/replay')).filter(n=>/\.rpy$/i.test(n)),demos=fs.readdirSync(path.join(root,'local/retail')).filter(n=>/^demo\d\.rpy$/.test(n));
  assert.deepEqual(corpus.fixtures.map(f=>f.id).sort(),[...supplied,...demos].sort());for(const f of corpus.fixtures){const r=parseReplay(fs.readFileSync(path.join(root,'site/replays',f.id)));assert.equal(r.character,f.character);assert.equal(r.shot,f.shot);assert.equal(r.difficulty,f.difficulty);for(const s of r.stages){assert.ok(r.input(s.number,s.frames-1));assert.equal(r.input(s.number,s.frames),null);}}
});
test('malformed and oversized replays fail before large allocations',()=>{
  const b=fs.readFileSync(path.join(root,'site/replays/demo2.rpy'));assert.throws(()=>parseReplay(b.subarray(0,35)));const n=Buffer.from(b);n.writeUInt32LE(0x7fffffff,32);assert.throws(()=>parseReplay(n));const wrong=Buffer.from(b);wrong[0]=0;assert.throws(()=>parseReplay(wrong));
});
test('all current stage headers separate 16-bit lives/bombs from their fragments',()=>{
  const corpus=JSON.parse(fs.readFileSync(path.join(root,'tools/replay-verifier/corpus.json')));let count=0;
  for(const fixture of corpus.fixtures){const replay=parseReplay(fs.readFileSync(path.join(root,'site/replays',fixture.id))),view=new DataView(replay.data.buffer);
    for(const stage of replay.stages){const initial=stage.initial;
      assert.equal(initial.lives,view.getUint16(stage.offset+24,true));assert.equal(initial.lifeFragments,view.getUint16(stage.offset+26,true));
      assert.equal(initial.bombs,view.getUint16(stage.offset+28,true));assert.equal(initial.bombFragments,view.getUint16(stage.offset+30,true));
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
