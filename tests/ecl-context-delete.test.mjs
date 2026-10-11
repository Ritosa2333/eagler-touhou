import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import createCore from '../site/runtime/core.mjs';
import {omitPlayerEntryClear} from './helpers/passive-player.mjs';
const native=JSON.parse(fs.readFileSync(new URL('fixtures/ecl-context-delete-original.json',import.meta.url)));
function snapshot(core){const at=core._th12_state(),size=core._th12_state_size();return JSON.parse(new TextDecoder().decode(core.HEAPU8.subarray(at,at+size)));}
test('original main ECL1/return removes its Enemy, while an async ECL1/return disposes only that context',async()=>{
 assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 for(const row of native.records){
  const core=await createCore(),raw=Buffer.from(row.syntheticEclHex,'hex');
  const names=36+raw.readUInt16LE(16)*4;
  // Keep the multi-routine name table contiguous when replacing the five
  // character Probe name with the four character main name.
  raw.write(raw.readUInt16LE(16)===2?'main\0Worker\0':'main\0',names);
  const at=core._malloc(raw.length);core.HEAPU8.set(raw,at);
  assert.equal(core._th12_load_ecl(at,raw.length),1);core._free(at);
  core._th12_start(0,0,1,1234);omitPlayerEntryClear(core);
  for(const frame of row.frames){
   if(frame.inputTick>=0)core._th12_tick(0,0);
   const actual=snapshot(core),label=row.kind+' input'+frame.inputTick;
   assert.equal(actual.enemies.length,frame.enemies.length,label+' surviving enemies');
   if(!frame.enemies.length)continue;
   const expected=frame.enemies[0].contexts.filter(c=>c.routine).map(c=>[
    c.routine==='Probe'?'main':c.routine,c.offset,c.time]);
   const contexts=actual.ecl.filter(c=>c.routine).map(c=>[c.routine,c.byteOffset,c.time]);
   assert.deepEqual(contexts,expected,label+' surviving scheduled contexts');
  }
 }
});
