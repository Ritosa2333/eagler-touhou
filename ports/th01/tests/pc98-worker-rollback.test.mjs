import test from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {restoreRollback} from '../../../common/pc98/rollback-client.mjs';
import {FrameQueue} from '../web/netplay/queue.js';
import {RollbackQueue as TH02} from '../../th02/web/netplay/rollback-queue.js';
import {RollbackQueue as TH04} from '../../th04/web/netplay/rollback-queue.js';

const coreModule=await WebAssembly.compile(await readFile(new URL('../../../common/pc98/rollback-core.wasm',import.meta.url)));
for(const [game,Queue] of [['th01',FrameQueue],['th02',TH02],['th04',TH04]])for(const players of [2,3]){
 test(`${game} ${players} players: a late Worker input keeps the correction available until restore`,async()=>{
  const q=game==='th01'?new Queue(0,2,players,{rollback:true,coreModule}):new Queue(0,players,{coreModule,directionPrediction:3,predictionLead:8});
  if(game!=='th01')q.arm(0);
  try{
   for(let frame=0;frame<2;frame++){q.capture(0);q.commit(q.peek());q.prune();}
   q.capture(0);
   const predicted=q.peek();assert.equal(predicted[1],0);
   // The Worker already owns this row when the authoritative inputs arrive.
   await Promise.resolve();
   for(let slot=1;slot<players;slot++)q.receive(2,slot,slot);
   q.commit(predicted);assert.equal(q.dirty,2);
   q.prune();
   assert.equal(await restoreRollback(q,async()=>{}),2);
   assert.deepEqual(q.peek(),Array.from({length:players},(_,slot)=>slot));
  }finally{q.core.close();}
 });
}
