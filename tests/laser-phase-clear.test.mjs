import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import createCore from '../site/runtime/core.mjs';
const native=JSON.parse(fs.readFileSync(new URL('fixtures/laser-phase-clear-native.json',import.meta.url)));
assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
const snapshot=core=>{const at=core._th12_state(),size=core._th12_state_size();return JSON.parse(new TextDecoder().decode(core.HEAPU8.subarray(at,at+size)));};
async function start(row){
  const core=await createCore();
  const load=(raw,method,index=null)=>{const at=core._malloc(raw.length);try{core.HEAPU8.set(raw,at);assert.equal(index===null?method(at,raw.length):method(index,at,raw.length),1);}finally{core._free(at);}};
  load(fs.readFileSync(new URL('../local/retail/bullet.anm',import.meta.url)),core._th12_load_anm,0);
  const raw=Buffer.from(row.syntheticEclHex,'hex'),header=raw.readUInt32LE(36);
  raw.fill(0,40,header);raw.write('main',40);load(raw,core._th12_load_ecl);
  core._th12_start(0,0,1,0x1234);
  for(let tick=0;tick<row.inputTicksBeforeCapture;tick++)core._th12_tick(0,0);
  return core;
}
function compareItems(row,state){
  const items=state.items.filter(n=>n[2]===9);
  assert.equal(items.length,row.itemSpawns.length,row.name+' small point count');
  if(row.inputTicksBeforeCapture===0){
    for(let index=0;index<items.length;index++)
      assert.deepEqual(items[index].slice(4,6),row.itemSpawns[index].position.slice(0,2).map(n=>Number(n.toPrecision(9))),row.name+' spawn order '+index);
  }
}
test('actual ECL445 clears Timed beams and split Moving tails with original point/RNG counts',async()=>{
  for(const row of native.records.filter(n=>!n.protectedExtension)){
    const core=await start(row),state=snapshot(core);
    assert.deepEqual([state.rng.seed,state.rng.calls],row.birth.rng,row.name+' actual ANM RNG');
    compareItems(row,state);
    const roots=state.animations.chains[0].map(id=>state.animations.nodes.find(n=>n.id===id)).filter(n=>n.parent===0&&n.bank===0);
    const expected=row.effects.filter(n=>n.script===16);
    assert.equal(roots.length,expected.length,row.name+' cancel roots');
    for(let index=0;index<expected.length;index++){
      assert.equal(roots[index].script,expected[index].script);
      assert.equal(roots[index].render[5],expected[index].layer);
      assert.equal(roots[index].render[2],expected[index].flags);
      assert.deepEqual(roots[index].pose.slice(9,12),expected[index].rotation);
      // Snapshot uses nine significant digits; restore its float32 value
      // before the native viewport addition, including ties-to-even cases.
      assert.deepEqual(roots[index].pose.slice(3,6).map((n,i)=>Math.fround(Math.fround(n)+(i===0?224:i===1?16:0))),expected[index].enginePosition);
    }
    assert.equal(new Uint32Array(core.HEAPU8.buffer,core._th12_unsupported(),1024)[445],0);
  }
});
test('ECL512/513 preserve real ETEX200 protection and ECL445 resets it before clearing',async()=>{
  for(const row of native.records.filter(n=>n.protectedExtension)){
    const core=await start(row),state=snapshot(core);compareItems(row,state);
    assert.deepEqual([state.rng.seed,state.rng.calls],row.final.rng,row.name+' actual ANM RNG');
    if(row.commands[0]===445)assert.equal(state.lasers.nodes.length,0);
    else{
      assert.equal(state.lasers.nodes.length,2);
      assert.ok(state.lasers.nodes.every(n=>n.protection===10));
    }
  }
});
