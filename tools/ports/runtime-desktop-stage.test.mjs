import test from 'node:test';
import assert from 'node:assert/strict';
import {createStageRecorder} from './runtime-desktop-stage.mjs';

test('stage recorder saves a timestamped running stage before work and its result after work',async()=>{
  const record={},snapshots=[];let time=0;
  const save=async()=>snapshots.push(JSON.parse(JSON.stringify(record)));
  const stage=createStageRecorder(record,save,{now:()=>new Date(1000+time*1000),monotonic:()=>1000*time++});
  const result=await stage('ready',{epoch:2},async()=>{
    assert.equal(snapshots.at(-1).stages[0].status,'running');
    return{event:'ready'};
  });
  assert.deepEqual(result,{event:'ready'});
  assert.equal(record.stages[0].status,'passed');
  assert.equal(record.stages[0].epoch,2);
  assert.equal(record.stages[0].startedAt,'1970-01-01T00:00:01.000Z');
  assert.equal(record.stages[0].finishedAt,'1970-01-01T00:00:02.000Z');
  assert.equal(record.stages[0].durationMs,1000);
  assert.equal(snapshots.length,2);
});

test('stage recorder persists a failed transition before propagating its error',async()=>{
  const record={},snapshots=[];
  const save=async()=>snapshots.push(JSON.parse(JSON.stringify(record)));
  const stage=createStageRecorder(record,save);
  await assert.rejects(stage('launch',{epoch:4},async()=>{throw Error('launch failed');}),/launch failed/);
  assert.equal(snapshots[0].stages[0].status,'running');
  assert.equal(snapshots[1].stages[0].status,'failed');
  assert.match(snapshots[1].stages[0].error,/launch failed/);
  assert.ok(snapshots[1].stages[0].finishedAt);
});
