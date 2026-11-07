import test from 'node:test';
import assert from 'node:assert/strict';
import {displayOptions,applyDisplay} from '../web/display.mjs';
import {runtimeOptions} from '../web/managed.mjs';

test('launcher display settings preserve exclusivity and stay out of normal runtime options',()=>{
 const requested={bossBar:'ring',phaseMarkers:true,phaseStars:true,numericPower:true};
 const normalized=displayOptions(requested);
 assert.equal(normalized.phaseTimeout,true);assert.equal(normalized.phaseStars,false);assert.equal(normalized.phaseMarkers,false);
 assert.throws(()=>runtimeOptions({th05MultiplayerDisplay:requested}),/Unsupported/);
 assert.deepEqual(runtimeOptions({th05MultiplayerDisplay:requested},true).th05MultiplayerDisplay,requested);
 for(const invalid of [null,[],{bossBar:'unknown'},{phaseTimeout:1},{invincible:true}])assert.throws(()=>displayOptions(invalid));
 const calls=[];applyDisplay({_th05_coop_display:(...args)=>calls.push(args)},requested);
 assert.deepEqual(calls,[[2,36]]);
});
