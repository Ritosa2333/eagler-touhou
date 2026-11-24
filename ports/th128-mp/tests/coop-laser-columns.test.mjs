import test from 'node:test';import assert from 'node:assert/strict';import {readFile} from 'node:fs/promises';
// Actual native geometry/callback acceptance is in coop-campaign.integration.mjs (EX).
// Regression for the source receipt: non-host seats must own their original laser ANM/collision callbacks.
test('the MP projection owns the EX selector-3 column callback and independent contact presentation',async()=>{
 const patch=await readFile(new URL('../native/world-coop.patch',import.meta.url),'utf8'),mp=await readFile(new URL('../native/CoopSeats.inc',import.meta.url),'utf8');
 assert.ok(patch.includes('coopphysics::columnContact('));assert.ok(mp.includes('laserEffectSnapshots'));assert.ok(mp.includes('laserColumnContactRequired'));
});