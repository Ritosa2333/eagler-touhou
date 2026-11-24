import assert from 'node:assert/strict';import test from 'node:test';import {pruneCoopReplaySlots} from '../runtime/coop-local-replays.mjs';
test('multiplayer replay slots retain 25 local files and replace repeated saves without pruning the new archive',()=>{
 const old=Array.from({length:25},(_,i)=>({id:String(i),savedAt:i}));
 assert.deepEqual(pruneCoopReplaySlots(old,{id:'4',savedAt:26}),[]);
 assert.deepEqual(pruneCoopReplaySlots(old,{id:'new',savedAt:30}),['0']);
 assert.deepEqual(pruneCoopReplaySlots([...old,{id:'older',savedAt:-1}],{id:'new',savedAt:30}),['0','older']);
});
import * as storage from '../runtime/coop-local-replays.mjs';

test('dedicated numbered replay slots replace only the selected tape for the current build',()=>{
 assert.equal(typeof storage.coopReplaySlotKey,'function');
 const build={coreSHA256:'a'.repeat(64),resourceSHA256:'b'.repeat(64)};
 assert.equal(storage.coopReplaySlotKey(build,0),storage.coopReplaySlotKey({...build},0));
 assert.notEqual(storage.coopReplaySlotKey(build,24),storage.coopReplaySlotKey(build,0));
 assert.notEqual(storage.coopReplaySlotKey({...build,coreSHA256:'d'.repeat(64)},0),storage.coopReplaySlotKey(build,0));
 for(const slot of [-1,25,1.25,null])assert.throws(()=>storage.coopReplaySlotKey(build,slot));
});
