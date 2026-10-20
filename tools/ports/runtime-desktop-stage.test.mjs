import test from 'node:test';
import assert from 'node:assert/strict';
import {createStageRecorder,requiredDesktopReadinessEvidence,requiredDesktopStartupStages,summarizeDesktopStartupEvidence} from './runtime-desktop-stage.mjs';

const passedStartupStages=()=>requiredDesktopStartupStages.map(stage=>({stage,epoch:1,status:'passed'}));

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

test('startup stage metadata separates native gameplay readiness from the six recorded startup stages',()=>{
  assert.deepEqual(requiredDesktopStartupStages,['host-navigation','ready','keyboard-bridge','configure','launch','first-frame']);
  assert.deepEqual(requiredDesktopReadinessEvidence,{id:'native-gameplay-preparation',source:'gameplayReadinessByEpoch',
    scope:'every-passed-first-frame-epoch',readyValue:true});
  const summary=summarizeDesktopStartupEvidence({stages:passedStartupStages(),gameplayReadinessByEpoch:[{epoch:1,ready:true,reasons:[]}]});
  assert.equal(summary.status,'passed');assert.equal(summary.initialStageResults.length,6);
  assert.deepEqual(summary.readinessEvidence.epochs,[{epoch:1,status:'ready',reasons:[]}]);
});

test('missing or failed startup and readiness evidence cannot be summarized as passed',()=>{
  const missingReadiness=summarizeDesktopStartupEvidence({stages:passedStartupStages(),gameplayReadinessByEpoch:[]});
  assert.equal(missingReadiness.status,'incomplete');assert.deepEqual(missingReadiness.incompleteReadiness,[1]);
  const notReady=summarizeDesktopStartupEvidence({stages:passedStartupStages(),gameplayReadinessByEpoch:[{epoch:1,ready:false,reasons:['native input unavailable']}]});
  assert.equal(notReady.status,'incomplete');
  const missingStage=summarizeDesktopStartupEvidence({stages:passedStartupStages().slice(1),gameplayReadinessByEpoch:[{epoch:1,ready:true}]});
  assert.equal(missingStage.status,'incomplete');assert.deepEqual(missingStage.missingStages,['host-navigation']);
  const failedStage=summarizeDesktopStartupEvidence({stages:passedStartupStages().map(item=>item.stage==='launch'?{...item,status:'failed'}:item),
    gameplayReadinessByEpoch:[{epoch:1,ready:true}]});
  assert.equal(failedStage.status,'failed');assert.deepEqual(failedStage.failedStages,['launch']);
});

test('readiness must be recorded once and ready for every Runtime first-frame epoch',()=>{
  const stages=[...passedStartupStages(),{stage:'first-frame',epoch:2,status:'passed'}];
  const summary=summarizeDesktopStartupEvidence({stages,gameplayReadinessByEpoch:[{epoch:1,ready:true,reasons:[]}]});
  assert.equal(summary.status,'incomplete');assert.deepEqual(summary.incompleteReadiness,[2]);
  const duplicate=summarizeDesktopStartupEvidence({stages:passedStartupStages(),gameplayReadinessByEpoch:[
    {epoch:1,ready:true,reasons:[]},{epoch:1,ready:true,reasons:[]},
  ]});
  assert.equal(duplicate.status,'incomplete');assert.equal(duplicate.readinessEvidence.epochs[0].status,'ambiguous');
});
