import test from 'node:test';
import assert from 'node:assert/strict';
import {OwnedReplayStore} from '../../site/runtime/sound.mjs';
import {createReplayIDB,replayFile} from './replay-idb.mjs';
function requireMethods(store){assert.equal(typeof store.importAtSlot,'function','Exact-slot Replay import is missing');assert.equal(typeof store.removeSlot,'function','CAS Replay remove is missing');}
test('strict exact-slot import preserves original file and metadata, leaving all other slots empty',async()=>{
 const idb=createReplayIDB(),store=new OwnedReplayStore(idb),file=replayFile();requireMethods(store);
 const receipt=await store.importAtSlot(7,file,{expectedDigest:null,fileName:'th128_07.rpy'});
 assert.equal(receipt.slot,7);assert.equal(receipt.strictDurability,true);assert.equal(receipt.strictFullReadback,true);
 assert.deepEqual(receipt.bytes,file);const rows=await store.observe();assert.equal(rows.filter(Boolean).length,1);assert.equal(rows[7].source,'original-rpy-import');
 assert.deepEqual(await store.export(7),file);assert.equal(idb.counts.writes,1);store.close();
});
test('a duplicate at the exact target revalidates without rewriting and never selects a different slot',async()=>{
 const idb=createReplayIDB(),store=new OwnedReplayStore(idb),file=replayFile();requireMethods(store);
 const first=await store.importAtSlot(24,file,{expectedDigest:null});const again=await store.importAtSlot(24,file,{expectedDigest:first.digest});
 assert.equal(again.slot,24);assert.equal(again.alreadyPresent,true);assert.equal(idb.counts.writes,1);
 await assert.rejects(store.importAtSlot(24,replayFile(9),{expectedDigest:null}),/conflict|changed|digest|occupied/i);
 assert.deepEqual(await store.export(24),file);store.close();
});
test('exact-slot import uses CAS to allow explicit replacement but stale identity leaves original bytes untouched',async()=>{
 const idb=createReplayIDB(),store=new OwnedReplayStore(idb);requireMethods(store);const first=await store.importAtSlot(0,replayFile(),{expectedDigest:null});
 await assert.rejects(store.importAtSlot(0,replayFile(10),{expectedDigest:'0'.repeat(64)}),/conflict|changed|digest/i);
 const changed=await store.importAtSlot(0,replayFile(10),{expectedDigest:first.digest});assert.notEqual(changed.digest,first.digest);assert.equal(changed.slot,0);store.close();
});
test('malformed or incompatible replay is refused before any database open or write',async()=>{
 const idb=createReplayIDB(),store=new OwnedReplayStore(idb);requireMethods(store);
 await assert.rejects(store.importAtSlot(1,new Uint8Array(40)),/replay|RPY|magic|identity/i);
 assert.equal(idb.counts.opens,0);assert.equal(idb.counts.writes,0);store.close();
});
test('cancel before the transaction mutates leaves the exact requested slot empty',async()=>{
 let current=true;
 const idb=createReplayIDB({beforeGet(){current=false;}}),store=new OwnedReplayStore(idb);requireMethods(store);
 await assert.rejects(store.importAtSlot(3,replayFile(),{expectedDigest:null,isCurrent:()=>current}),/cancel|owner|abort/i);
 assert.equal(idb.peek(3),undefined);assert.equal(idb.counts.writes,0);store.close();
});
test('strict durability denial and failed puts never report a successful file receipt',async()=>{
 for(const idb of [createReplayIDB({strict:false}),createReplayIDB({failWrite:()=>true})]){
  const store=new OwnedReplayStore(idb);requireMethods(store);await assert.rejects(store.importAtSlot(2,replayFile(),{expectedDigest:null}),/strict|durability|abort|write|failure/i);
  assert.equal(idb.peek(2),undefined);store.close();
 }
});
test('post-commit readback tampering is a failure rather than a strict receipt',async()=>{
 let corrupt=false;
 const idb=createReplayIDB({beforeCommit(){corrupt=true;},alterRead(row){if(corrupt&&row){row.bytes[40]^=1;}return row;}}),store=new OwnedReplayStore(idb);requireMethods(store);
 await assert.rejects(store.importAtSlot(0,replayFile(),{expectedDigest:null}),/digest|differ|readback|Stored replay/i);store.close();
});
test('CAS deletion validates old bytes, strict transaction and full absent readback',async()=>{
 const idb=createReplayIDB(),store=new OwnedReplayStore(idb);requireMethods(store);const first=await store.importAtSlot(9,replayFile(),{expectedDigest:null});
 await assert.rejects(store.removeSlot(9,{expectedDigest:'0'.repeat(64)}),/digest|conflict|changed/i);
 assert.deepEqual(await store.export(9),first.bytes);
 const deleted=await store.removeSlot(9,{expectedDigest:first.digest});
 assert.equal(deleted.slot,9);assert.equal(deleted.removed,true);assert.equal(deleted.strictDurability,true);assert.equal(deleted.strictFullReadback,true);
 assert.equal((await store.observe())[9],null);assert.equal(idb.counts.deletes,1);store.close();
});
test('all25 slots stay distinct and invalid target numbers never wrap or allocate another slot',async()=>{
 const idb=createReplayIDB(),store=new OwnedReplayStore(idb);requireMethods(store);
 for(let slot=0;slot<25;slot++)await store.importAtSlot(slot,replayFile(slot),{expectedDigest:null});
 assert.equal((await store.observe()).filter(Boolean).length,25);
 for(const slot of [-1,25,1.5,NaN])await assert.rejects(store.importAtSlot(slot,replayFile(),{expectedDigest:null}),/slot/i);
 assert.equal((await store.observe()).filter(Boolean).length,25);store.close();
});
