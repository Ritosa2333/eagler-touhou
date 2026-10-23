import test from 'node:test';
import assert from 'node:assert/strict';
import vm from 'node:vm';
import {createStageRecorder,DesktopStartupPreconditionError,readDesktopHostEvents,recordDesktopGameplayReadiness,recordDesktopStartupEvidenceIssue,requiredAudioTitleReadinessEvidence,requiredDesktopReadinessEvidence,requiredDesktopStartupStages,summarizeDesktopStartupEvidence} from './runtime-desktop-stage.mjs';

const passedStartupStages=()=>requiredDesktopStartupStages.map(stage=>({stage,epoch:1,status:'passed'}));

test('terminal Host observations preserve protocol failures independently of startup completion',()=>{
  const host={events:[{event:'error',message:'audio startup failed'}],lifecycleEvents:[{type:'blur'}],
    hostKeyboardLifecycle:[],keyboardForwardEvents:[],keyboardMapEvents:[]};
  const result=vm.runInNewContext(`(${readDesktopHostEvents.toString()})()`,host);
  assert.deepEqual(JSON.parse(JSON.stringify(result)),host);
  assert.throws(()=>vm.runInNewContext(`(${readDesktopHostEvents.toString()})()`,{events:[]}),/ledger is unavailable/);
});

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

test('startup stages require host focus before ready and foreground restoration after configure',()=>{
  assert.deepEqual(requiredDesktopStartupStages,['host-navigation','host-focus-before-ready','ready','keyboard-bridge','foreground','configure','launch-foreground','launch','first-frame']);
  assert.deepEqual(requiredDesktopReadinessEvidence,{id:'native-gameplay-preparation',source:'gameplayReadinessByEpoch',
    scope:'every-passed-first-frame-epoch',readyValue:true});
  const summary=summarizeDesktopStartupEvidence({stages:passedStartupStages(),gameplayReadinessByEpoch:[{epoch:1,ready:true,reasons:[]}]});
  assert.equal(summary.status,'passed');assert.equal(summary.initialStageResults.length,9);
  assert.equal(summary.stageOrder.ready,true);
  assert.deepEqual(summary.readinessEvidence.epochs,[{epoch:1,status:'ready',reasons:[]}]);
});

test('unavailable initial desktop environment seals incomplete and cannot certify startup',async()=>{
  const record={},stage=createStageRecorder(record,async()=>{});
  await stage('host-navigation',{epoch:1},async()=>{});
  await assert.rejects(stage('host-focus-before-ready',{epoch:1},async()=>{
    throw new DesktopStartupPreconditionError('Native browser document remains hidden');
  }),DesktopStartupPreconditionError);
  assert.equal(record.stages[1].status,'incomplete');
  assert.match(record.stages[1].error,/remains hidden/);
  const summary=summarizeDesktopStartupEvidence({stages:record.stages});
  assert.equal(summary.status,'incomplete');
  assert.deepEqual(summary.incompleteStages,['host-focus-before-ready']);
  assert.deepEqual(summary.failedStages,[]);
  assert.ok(summary.missingStages.includes('launch'));
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
  const outOfOrder=passedStartupStages();
  [outOfOrder[1],outOfOrder[2]]=[outOfOrder[2],outOfOrder[1]];
  const reordered=summarizeDesktopStartupEvidence({stages:outOfOrder,gameplayReadinessByEpoch:[{epoch:1,ready:true}]});
  assert.equal(reordered.status,'failed');assert.equal(reordered.stageOrder.invalid,true);
  const launchedBeforeRestore=passedStartupStages();
  const launchIndex=launchedBeforeRestore.findIndex(item=>item.stage==='launch'),restoreIndex=launchedBeforeRestore.findIndex(item=>item.stage==='launch-foreground');
  [launchedBeforeRestore[launchIndex],launchedBeforeRestore[restoreIndex]]=[launchedBeforeRestore[restoreIndex],launchedBeforeRestore[launchIndex]];
  const launchOrder=summarizeDesktopStartupEvidence({stages:launchedBeforeRestore,gameplayReadinessByEpoch:[{epoch:1,ready:true}]});
  assert.equal(launchOrder.status,'failed');assert.equal(launchOrder.stageOrder.invalid,true);
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

test('one startup receipt stays unique while repeated transition readiness observations remain separately auditable',()=>{
  const record={};
  assert.throws(()=>recordDesktopGameplayReadiness(record,{epoch:1,label:'implicit purpose',
    readiness:{ready:true,reasons:[]},samples:1,durationMs:0,state:{scene:'gameplay'}}),/explicit startup or transition purpose/);
  recordDesktopGameplayReadiness(record,{purpose:'startup',epoch:1,label:'initial gameplay',
    readiness:{ready:true,reasons:[]},samples:3,durationMs:150,state:{scene:'gameplay'}});
  for(let index=0;index<4;index++)recordDesktopGameplayReadiness(record,{purpose:'transition',epoch:1,
    label:`restart transition ${index+1}`,readiness:{ready:true,reasons:[]},samples:2,durationMs:75,state:{scene:'gameplay'}});
  const summary=summarizeDesktopStartupEvidence({stages:passedStartupStages(),
    gameplayReadinessByEpoch:record.gameplayReadinessByEpoch});
  assert.equal(summary.status,'passed');
  assert.deepEqual(summary.readinessEvidence.epochs,[{epoch:1,status:'ready',reasons:[]}]);
  assert.equal(record.gameplayReadinessTransitions.length,4);
  assert.ok(record.gameplayReadinessTransitions.every(item=>item.purpose==='transition'));
  assert.equal(recordDesktopStartupEvidenceIssue(record,summary),null);
  assert.equal(record.incompleteReason,undefined);
});

test('duplicate startup receipts remain ambiguous and an unready transition cannot stand in for startup readiness',()=>{
  const record={};
  const add=(purpose,ready,label)=>recordDesktopGameplayReadiness(record,{purpose,epoch:1,label,
    readiness:{ready,reasons:ready?[]:['native gameplay not ready']},samples:2,durationMs:75,state:{scene:ready?'gameplay':'loading'}});
  add('startup',true,'initial gameplay');
  add('startup',true,'duplicate initial gameplay');
  add('transition',false,'restart transition did not become ready');
  const ambiguous=summarizeDesktopStartupEvidence({stages:passedStartupStages(),
    gameplayReadinessByEpoch:record.gameplayReadinessByEpoch});
  assert.equal(ambiguous.status,'incomplete');
  assert.equal(ambiguous.readinessEvidence.epochs[0].status,'ambiguous');
  record.incompleteReason='Existing transition failure';
  assert.match(recordDesktopStartupEvidenceIssue(record,ambiguous),/epoch 1 gameplay readiness ambiguous/);
  assert.match(record.incompleteReason,/Existing transition failure; Desktop startup evidence is incomplete: epoch 1 gameplay readiness ambiguous/);
  assert.equal(record.gameplayReadinessTransitions.length,1);
  assert.equal(record.gameplayReadinessTransitions[0].ready,false);
  const transitionOnly={};
  recordDesktopGameplayReadiness(transitionOnly,{purpose:'transition',epoch:1,label:'unready only',
    readiness:{ready:false,reasons:['loading']},samples:1,durationMs:75,state:{scene:'loading'}});
  const missingStartup=summarizeDesktopStartupEvidence({stages:passedStartupStages(),
    gameplayReadinessByEpoch:transitionOnly.gameplayReadinessByEpoch});
  assert.equal(missingStartup.status,'incomplete');
  assert.equal(missingStartup.readinessEvidence.epochs[0].status,'missing');
  assert.match(recordDesktopStartupEvidenceIssue(transitionOnly,missingStartup),/epoch 1 gameplay readiness missing/);
  assert.match(transitionOnly.incompleteReason,/epoch 1 gameplay readiness missing/);
  assert.equal(transitionOnly.gameplayReadinessByEpoch,undefined);
});

test('audio title scenes retain their own readiness source and all epoch gates',()=>{
  const args={stages:passedStartupStages(),readinessEvidence:requiredAudioTitleReadinessEvidence,
    gameplayReadinessByEpoch:[{epoch:1,ready:true}]};
  const missing=summarizeDesktopStartupEvidence({...args,readinessByEpoch:[]});
  assert.equal(missing.status,'incomplete');
  assert.equal(missing.readinessEvidence.source,'audioTitleReadinessByEpoch');
  const ready=summarizeDesktopStartupEvidence({...args,readinessByEpoch:[{epoch:1,ready:true}]});
  assert.equal(ready.status,'passed');
  assert.equal(ready.readinessEvidence.id,'native-audio-title-preparation');
  const ambiguous=summarizeDesktopStartupEvidence({...args,readinessByEpoch:[{epoch:1,ready:true},{epoch:1,ready:true}]});
  assert.equal(ambiguous.status,'incomplete');
  assert.equal(ambiguous.readinessEvidence.epochs[0].status,'ambiguous');
  assert.throws(()=>summarizeDesktopStartupEvidence({...args,readinessEvidence:{id:'title'}}),/explicit source/);
});
