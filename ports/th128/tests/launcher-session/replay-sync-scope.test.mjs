import test from 'node:test';
import assert from 'node:assert/strict';
import{createReplayFileBridge}from'../../site/runtime/replay-file-bridge.mjs';
import{createReplayIDB}from'./replay-idb.mjs';
test('sync identifies committed Replay-store verification and never claims to flush the running native game',async()=>{
 const idb=createReplayIDB(),bridge=createReplayFileBridge({indexedDB:idb});
 try{
  const result=await bridge.execute('sync');
  assert.equal(result.storeOnly,true,'Missing limited-store sync scope');
  assert.equal(result.appliesToRunningGame,false,'Stored Replay sync cannot attest a running game flush');
  assert.equal(result.pendingWrites,0);assert.equal(result.verifiedFiles,0);assert.equal(idb.counts.opens,0);
 }finally{bridge.dispose();}
});
