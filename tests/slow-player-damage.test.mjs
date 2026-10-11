import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {initializeCandidate,restoreStage,state} from '../tools/replay-verifier/capture-candidate.mjs';

test('real slow BossDead shooting retains original score, RNG and shot/Spell clocks',async()=>{
 const original=JSON.parse(fs.readFileSync(new URL('./fixtures/slow-player-damage-original.json',import.meta.url)));
 assert.equal(original.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 assert.equal(original.rows.length,341);
 const core=await initializeCandidate(6),bytes=Buffer.from(original.mainBase64,'base64'),at=core._malloc(bytes.length);
 try{core.HEAPU8.set(bytes,at);assert.equal(core._th12_load_ecl(at,bytes.length),1);}finally{core._free(at);}
 restoreStage(core,original,{number:6,seed:original.seed,initial:original.initial});
 for(const row of original.rows){
  if(row.tick>=0)core._th12_tick(1,0);
  const s=state(core);
  assert.deepEqual({tick:row.tick,rng:[s.rng.seed,s.rng.calls],rate:s.gameSpeed,score:s.economy.resources[3],
   shotClock:s.player.fireFrame,serialTicks:s.spellState.serialTicks,spellAge:s.spellState.age},row,`tick${row.tick}`);
 }
});
