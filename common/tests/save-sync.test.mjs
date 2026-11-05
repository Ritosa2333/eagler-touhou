import test from 'node:test';
import assert from 'node:assert/strict';
import {createSaveSync} from '../browser/save-sync.mjs';

test('closed connection retries the same flush without repopulating memory',async()=>{
 const calls=[];
 const stale={transaction(){throw new DOMException('closing','InvalidStateError');},close(){}};
 const module={IDBFS:{dbs:{'/mp':stale},DB_STORE_NAME:'FILE_DATA'},FS:{syncfs(populate,done){
  calls.push(populate);done(module.IDBFS.dbs['/mp']?new DOMException('closing','InvalidStateError'):null);
 }}};
 await createSaveSync(()=>module)(false);
 assert.deepEqual(calls,[false,false]);
 assert.equal(module.IDBFS.dbs['/mp'],undefined);
});
test('permanent failures propagate and do not poison the next queued sync',async()=>{
 let calls=0;
 const failure=new DOMException('quota','QuotaExceededError');
 const module={FS:{syncfs(_,done){done(++calls===1?failure:null);}}};
 const sync=createSaveSync(()=>module);
 await assert.rejects(sync(false),error=>error===failure);
 await sync(false);
 assert.equal(calls,2);
});
test('recovery is bounded and leaves healthy cached connections alone',async()=>{
 let calls=0;
 const failure=new DOMException('closing','InvalidStateError');
 const healthy={transaction(){},close(){assert.fail('healthy connection closed');}};
 const stale={transaction(){throw failure;},close(){}};
 const module={IDBFS:{dbs:{'/mp':stale,'/normal':healthy},DB_STORE_NAME:'FILE_DATA'},FS:{syncfs(_,done){++calls;done(failure);}}};
 await assert.rejects(createSaveSync(()=>module)(true),error=>error===failure);
 assert.equal(calls,2);
 assert.equal(module.IDBFS.dbs['/normal'],healthy);
});
test('a sync error unrelated to a closed handle is never retried',async()=>{
 let calls=0;
 const failure=new DOMException('other invalid state','InvalidStateError');
 const module={IDBFS:{dbs:{'/mp':{transaction(){}}}},FS:{syncfs(_,done){++calls;done(failure);}}};
 await assert.rejects(createSaveSync(()=>module)(false),error=>error===failure);
 assert.equal(calls,1);
});
