import {test} from 'node:test';
import assert from 'node:assert/strict';
import {boundedOperation} from './bounded-operation.mjs';
import {createInspectionGuard,settleInspections} from './inspection-serialization.mjs';
test('all independent inspections settle before error propagation',async()=>{
 let release;const completed=[];
 const pending=settleInspections([async()=>{throw Error('broken');},async()=>{await new Promise(r=>release=r);completed.push('persisted');}]);
 await new Promise(r=>setImmediate(r));assert.deepEqual(completed,[]);release();
 await assert.rejects(pending,error=>error instanceof AggregateError && error.errors.length===1);assert.deepEqual(completed,['persisted']);
});
test('synchronous throw also permits all other streams to finish',async()=>{
 let saved=false;
 await assert.rejects(settleInspections([()=>{throw Error('sync');},async()=>{saved=true;}]),AggregateError);
 assert.equal(saved,true);
});
test('timeout never permits a second concurrent writer',async()=>{
 const guard=createInspectionGuard();let release;
 const first=boundedOperation('inspect',()=>guard(0,()=>new Promise(r=>release=r)),10);
 await assert.rejects(first,/timed out/);await assert.rejects(guard(0,async()=>{}),/Concurrent inspection/);
 assert.equal(await guard(1,async()=>7),7);release();await new Promise(r=>setImmediate(r));assert.equal(await guard(0,async()=>8),8);
});
