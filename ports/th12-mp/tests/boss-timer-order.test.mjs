import test from'node:test';import assert from'node:assert/strict';import fs from'node:fs';
import createCore from'../artifacts/diagnostic-runtime/core.mjs';import{omitPlayerEntryClear}from'./helpers/passive-player.mjs';
const native=JSON.parse(fs.readFileSync(new URL('fixtures/boss-timer-order-original.json',import.meta.url)));
function read(core,fn,size){const p=core[fn](),length=core[size]();return JSON.parse(new TextDecoder().decode(core.HEAPU8.subarray(p,p+length)));}
test('countdown uses the actual Enemy18 interrupt check before its phase-clock tail and retains timeout values',async()=>{
 for(const record of native.records){const c=await createCore(),raw=Buffer.from(record.syntheticEclHex,'hex'),ptr=c._malloc(raw.length);c.HEAPU8.set(raw,ptr);assert.equal(c._th12_load_ecl(ptr,raw.length),1);c._free(ptr);c._th12_start(0,0,1,1234);omitPlayerEntryClear(c,7);
  for(const f of record.frames){if(f.inputTick>=0)c._th12_tick(0,0);const label=`${record.kind} ${f.inputTick}`,state=read(c,'_th12_state','_th12_state_size'),hud=read(c,'_th12_boss_hud','_th12_boss_hud_size');assert.equal(state.enemies[0].phaseAge,f.phaseAge,label);assert.deepEqual([hud.timer.seconds,hud.timer.hundredths],f.timer,label);}
 }
});
