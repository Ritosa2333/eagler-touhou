import test from 'node:test';
import assert from 'node:assert/strict';
import {OwnedReplayStore} from '../../site/runtime/sound.mjs';
import {createReplayFileBridge} from '../../site/runtime/replay-file-bridge.mjs';
import {createReplayIDB,replayFile} from './replay-idb.mjs';
test('two actual stores racing one empty slot produce one winner and never overwrite it',async()=>{
 const idb=createReplayIDB(),a=new OwnedReplayStore(idb),b=new OwnedReplayStore(idb),first=replayFile(101),second=replayFile(102);
 const values=await Promise.allSettled([a.importAtSlot(3,first,{expectedDigest:null}),b.importAtSlot(3,second,{expectedDigest:null})]);
 assert.equal(values.filter(v=>v.status==='fulfilled').length,1);assert.equal(values.filter(v=>v.status==='rejected').length,1);
 assert.equal(idb.counts.writes,1);assert.deepEqual(await a.export(3),values.find(v=>v.status==='fulfilled').value.bytes);a.close();b.close();
});
test('missing, stale and corrupted stored records fail real read and sync validation',async()=>{
 const idb=createReplayIDB(),store=new OwnedReplayStore(idb),saved=await store.importAtSlot(1,replayFile(),{expectedDigest:null});
 idb.seed(1,{...idb.peek(1),digest:'0'.repeat(64)});
 const bridge=createReplayFileBridge({indexedDB:idb});
 await assert.rejects(bridge.execute('read',{path:'replay/th128_01.rpy'}),/Stored replay bytes differ/);
 await assert.rejects(bridge.execute('sync'),/Stored replay bytes differ/);
 assert.equal(idb.counts.writes,1);assert(saved.digest!=='0'.repeat(64));bridge.dispose();store.close();
});
test('a read connection that opens after bridge disposal is closed instead of leaking into the retired owner',async()=>{
 const idb=createReplayIDB(),store=new OwnedReplayStore(idb);await store.importAtSlot(0,replayFile(),{expectedDigest:null});store.close();
 let opening,release;const opened=new Promise(resolve=>{opening=resolve;});let closed=0;
 const delayed={databases:()=>idb.databases(),open(name){
  const req={onsuccess:null,onerror:null,onupgradeneeded:null,result:null,error:null};opening();
  release=()=>{const db=idb._dbs.get(name);req.result={...db,transaction:db.transaction.bind(db),close(){closed++;}};req.onsuccess?.();};return req;
 }};
 const bridge=createReplayFileBridge({indexedDB:delayed}),pending=bridge.execute('list');await opened;bridge.dispose();release();
 await assert.rejects(pending,error=>error.name==='AbortError');assert.equal(closed,1,'Late DB connection was not closed');
});
test('reopening one store after close cannot adopt a late result from its earlier open',async()=>{
 let release;const db={createObjectStore(){},closeCount:0,close(){this.closeCount++;}},idb={open(){const r={};release=()=>{r.result=db;r.onsuccess?.();};return r;}};
 const store=new OwnedReplayStore(idb),pending=store.open();store.close();release();
 await assert.rejects(pending,/closed|cancel|owner|open/i);assert.equal(db.closeCount,1);
});
