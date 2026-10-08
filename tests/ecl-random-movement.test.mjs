import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import createCore from '../site/runtime/core.mjs';

const fixture=JSON.parse(fs.readFileSync(new URL('fixtures/ecl-random-movement-native.json',import.meta.url)));
const wasmBinary=fs.readFileSync(new URL('../site/runtime/core.wasm',import.meta.url));
const snapshot=core=>{
  const pointer=core._th12_state(),size=core._th12_state_size();
  return JSON.parse(new TextDecoder().decode(core.HEAPU8.subarray(pointer,pointer+size)));
};
const near=(actual,expected,label,tolerance=.000004)=>assert.ok(Math.abs(actual-expected)<tolerance,`${label}: ${actual} != ${expected}`);

for(const op of [312,313])test(`native ECL${op} preserves random direction branches, RNG16 bias and interpolation`,async()=>{
  assert.equal(fixture.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
  assert.equal(fixture.wholeGameOracle,false);assert.equal(fixture.creationGuardPrimed,false);
  const core=await createCore({wasmBinary});
  for(const row of fixture.cases.filter(row=>row.op===op)){
    const bytes=Buffer.from(row.syntheticEclHex,'hex'),pointer=core._malloc(bytes.length);
    try{core.HEAPU8.set(bytes,pointer);assert.equal(core._th12_load_ecl(pointer,bytes.length),1,row.name);}finally{core._free(pointer);}
    // These are explicit synthetic initial conditions, matching the native
    // fixture before its Enemy constructor resolves the player-relative bias.
    const initial=new Int32Array([row.playerX*128,400*128,100,2,2,0,2000000,0,0,0,0,0,0,0,0,0,0]);
    const header=core._malloc(initial.byteLength);
    try{core.HEAPU8.set(new Uint8Array(initial.buffer),header);assert.equal(core._th12_start_replay(0,0,1,row.seed,header,initial.length),1);}finally{core._free(header);}
    core._th12_oracle_fixture(7); // Same passive damage/body boundaries as the native slice.
    const birth=snapshot(core),enemy=birth.enemies[0],selected=row.selected;
    assert.deepEqual([birth.rng.seed,birth.rng.calls],[selected.rngSeed,selected.rngCalls],row.name+' constructor RNG');
    assert.deepEqual(enemy.movement.slice(28,32),selected.clamp,row.name+' clamp parameters');
    const angleIndex=op===312?4:18;
    near(enemy.movement[angleIndex],selected.angle,row.name+' selected angle',.000001);
    for(const frame of row.frames){
      core._th12_tick(0,0);const actual=snapshot(core),motion=actual.enemies[0],label=row.name+' input'+frame.inputTick;
      assert.equal(actual.frame,frame.inputTick+1,label+' clock');
      assert.deepEqual([actual.rng.seed,actual.rng.calls],[frame.rngSeed,frame.rngCalls],label+' RNG');
      for(const field of ['position','absolute','relative'])
        for(let axis=0;axis<2;axis++)near(motion[field][axis],frame[field][axis],label+' '+field+axis,.00002);
      for(const [field,component,index] of [['angle','absolute',2],['speed','absolute',3],['relativeAngle','relative',2],['relativeSpeed','relative',3]])
        near(motion[component][index],frame[field],label+' '+field);
    }
  }
});
