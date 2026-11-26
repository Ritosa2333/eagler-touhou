import test from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {restoreRollback} from './rollback-client.mjs';
import {FrameQueue} from '../../ports/th01/web/netplay/queue.js';
import {RollbackQueue as TH04} from '../../ports/th04/web/netplay/rollback-queue.js';

const coreModule=await WebAssembly.compile(await readFile(new URL('./rollback-core.wasm',import.meta.url)));
for(const [game,Queue] of [['th01',FrameQueue],['th04',TH04]])for(const players of [2,3]){
 test(`${game} ${players} players: earlier corrections during asynchronous restore survive long sessions`,async()=>{
  const q=game==='th01'?new Queue(0,2,players,{rollback:true,coreModule}):new Queue(0,players,{coreModule,directionPrediction:3,predictionLead:8});
  if(game!=='th01')q.arm(0);
  for(let frame=0;frame<1200;frame++){
   q.capture(0);
   if(frame>=2)for(let slot=1;slot<players;slot++)q.receive(frame,0,slot);
   const row=q.peek();assert(row,`${game} frame ${frame}`);q.commit(row);q.prune();
  }
  // Simulate three predictions; the newest input starts restore, then an
  // older player's input arrives while the native worker is still restoring.
  for(let frame=1200;frame<1203;frame++){
   q.capture(0);if(players===3)q.receive(frame,0,2);
   const row=q.peek();assert(row);q.commit(row);
  }
  q.receive(1202,1,1);assert.equal(q.dirty,1202);
  const restored=[];
  const frame=await restoreRollback(q,async target=>{
   restored.push(target);
   await Promise.resolve();
   if(target===1202){q.receive(1200,2,1);assert.throws(()=>q.core.rewind(target),/无法恢复输入历史/,'the original stale target reproduces the reported error');}
  });
  assert.deepEqual(restored,[1202,1200]);assert.equal(frame,1200);assert.equal(q.frame,1200);
  assert.equal(q.peek()[1],2);q.commit(q.peek());
  q.receive(1201,0,1);q.refreshCorrection(true);
  assert.equal(q.dirty,null);
  for(let frame=1201;frame<1203;frame++){assert(q.peek());q.commit(q.peek());}
  q.prune();q.core.close();
 });
}
test('aborted restore never rewinds the input history',async()=>{
 let rewound=false;const queue={dirty:3,rewind(){rewound=true;}};
 assert.equal(await restoreRollback(queue,async()=>{},()=>false),null);assert.equal(rewound,false);
});
