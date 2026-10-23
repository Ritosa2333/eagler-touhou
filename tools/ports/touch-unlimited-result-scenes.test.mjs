import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {runTouchUnlimitedResultSceneGroup} from './touch-unlimited-result-scenes.mjs';

const replayBytes=Buffer.from('independent-normal-replay-bytes');
const replayHash=createHash('sha256').update(replayBytes).digest('hex');
const preparation={available:true,status:'ready',game:'th08',variant:'normal',runtimeEpoch:7,runId:'run-41',
  storageIdentity:{database:'acceptance-normal-41'},normal:true,isolatedStorage:true,
  identitySource:'host Runtime epoch + selected IDBFS mount',
  startReceipt:{available:true,status:'passed',source:'ordinary title flow'}};
const sources={runtimeIdentity:'host Runtime epoch + selected IDBFS mount',runId:'native ordinary-run generation sequence',
  runMarker:'native.motion.cheat_movement_used',touchTrace:'TouchMotionTraceBank.forward',
  stageTransition:'ordinary supervisor next-stage request',resultTransition:'ordinary supervisor game-results request',
  resultLag:'ResultScreen::state.hscr.lag',replayFinalizer:'BrowserExports.browser_save_replay->GameApplication::finalize_replay',
  replayParser:'ReplayMetadata::lag and ReplayMetadata::practice from the independently decoded v6 header'};

function fixture({stageReset=false,resultLag=100,replayLag=100,runSwap=false,traceStageSwap=false,traceUnavailable=false}={}){
  let sequence=0,pending=null;
  const rows=[];
  const state={available:true,game:'th08',variant:'normal',runtimeEpoch:7,runId:'run-41',storageIdentity:structuredClone(preparation.storageIdentity),
    normal:true,practice:false,replay:false,scene:'gameplay',stage:'stage-a',playerPosition:{x:4,y:5},cheatMovementUsed:false,
    identitySource:sources.runtimeIdentity,runIdSource:sources.runId,markerSource:sources.runMarker,resultLag:null};
  const contract={game:'th08',variant:'normal',sources,nonzeroTarget:current=>({x:current.playerPosition.x+2,y:current.playerPosition.y}),
    inspectReplay:async(bytes,{expectedSha256})=>({available:true,sha256:createHash('sha256').update(bytes).digest('hex'),
      metadata:{lag:replayLag,practice:false},source:sources.replayParser})};
  const io={
    setNeutralTarget:async()=>({available:true,accepted:true}),
    setUnlimitedTarget:async({target,requestId})=>{pending={target:structuredClone(target),requestId};return{available:true,accepted:true,target:structuredClone(target),requestId};},
    step:async count=>{
      for(let index=0;index<count;index++){
        if(pending){
          const before=structuredClone(state.playerPosition),after=structuredClone(pending.target);
          rows.push({sequence:++sequence,runId:state.runId,runtimeEpoch:state.runtimeEpoch,storageIdentity:structuredClone(state.storageIdentity),
            stage:traceStageSwap&&sequence>0?'stage-old':state.stage,mode:'unlimited',gameplayTick:true,before,
            target:structuredClone(pending.target),after,source:sources.touchTrace});
          state.playerPosition=after;
          if(after.x!==before.x||after.y!==before.y)state.cheatMovementUsed=true;
          pending=null;
        }
      }
      return{available:true,steps:count};
    },
    readState:async()=>structuredClone(state),
    readTouchTrace:async({afterSequence}={})=>{
      if(traceUnavailable)return{available:false,reason:'native trace not mounted'};
      return{available:true,nextSequence:sequence+1,firstAvailableSequence:rows[0]?.sequence??sequence+1,
        dropped:false,rows:rows.filter(row=>afterSequence===null||row.sequence>afterSequence).map(row=>structuredClone(row))};
    },
    requestStageTransition:async()=>{
      state.stage='stage-b';if(stageReset)state.cheatMovementUsed=false;if(runSwap)state.runId='run-42';
      return{available:true,accepted:true,source:sources.stageTransition};
    },
    requestResultTransition:async()=>{state.scene='result';state.resultLag={available:true,value:resultLag,source:sources.resultLag};
      return{available:true,accepted:true,source:sources.resultTransition};},
    saveReplay:async()=>({available:true,saved:true,path:'/acceptance/replay/tch10.rpy',sha256:replayHash,source:sources.replayFinalizer}),
    readReplayBytes:async()=>({available:true,bytes:replayBytes,storageIdentity:structuredClone(state.storageIdentity)}),
  };
  return{io,contract,rows,state};
}

test('shared TOUCH-10 scene verifies zero-control contrast, consumed nonzero movement, transitions, Result, and saved Replay',async()=>{
  const sample=fixture();
  const result=await runTouchUnlimitedResultSceneGroup(sample.io,{contract:sample.contract,preparation,maxWaitTicks:4});
  assert.equal(result.checkStatus,'passed');
  assert.equal(result.evidence.steps[0].row.before.x,result.evidence.steps[0].row.after.x);
  assert.equal(result.evidence.steps[0].marker,false);
  assert.notEqual(result.evidence.steps[1].row.before.x,result.evidence.steps[1].row.after.x);
  assert.equal(result.evidence.steps[1].marker,true);
  assert.deepEqual(result.evidence.transitions.map(item=>item.kind),['stage','result']);
  assert.equal(result.evidence.result.lag,100);
  assert.equal(result.evidence.replay.metadataLag,100);
  assert.equal(sample.io.setNeutralTarget!==undefined,true);
});

test('missing runtime or replay observations stay incomplete',async()=>{
  const missing=fixture();delete missing.io.readTouchTrace;
  assert.equal((await runTouchUnlimitedResultSceneGroup(missing.io,{contract:missing.contract,preparation})).checkStatus,'incomplete');
  const trace=fixture({traceUnavailable:true});
  assert.equal((await runTouchUnlimitedResultSceneGroup(trace.io,{contract:trace.contract,preparation})).checkStatus,'incomplete');
  const parser=fixture();delete parser.contract.inspectReplay;
  assert.equal((await runTouchUnlimitedResultSceneGroup(parser.io,{contract:parser.contract,preparation})).checkStatus,'incomplete');
});

test('zero-only movement cannot satisfy TOUCH-10',async()=>{
  const sample=fixture();sample.contract.nonzeroTarget=state=>state.playerPosition;
  await assert.rejects(runTouchUnlimitedResultSceneGroup(sample.io,{contract:sample.contract,preparation}),
    /nonzero touch target must differ/);
});

test('marker reset across stage or a run identity swap is rejected',async()=>{
  for(const options of [{stageReset:true},{runSwap:true}]){
    const sample=fixture(options);
    await assert.rejects(runTouchUnlimitedResultSceneGroup(sample.io,{contract:sample.contract,preparation}),
      options.stageReset?/marker was cleared across the stage transition/:/crossed gameplay run identities/);
  }
});

test('stale stage motion rows and incorrect Result or Replay rates are rejected',async()=>{
  const stale=fixture({traceStageSwap:true});
  await assert.rejects(runTouchUnlimitedResultSceneGroup(stale.io,{contract:stale.contract,preparation}),/another stage/);
  const wrongResult=fixture({resultLag:99});
  await assert.rejects(runTouchUnlimitedResultSceneGroup(wrongResult.io,{contract:wrongResult.contract,preparation}),
    /did not record 100%/);
  const wrongReplay=fixture({replayLag:99});
  await assert.rejects(runTouchUnlimitedResultSceneGroup(wrongReplay.io,{contract:wrongReplay.contract,preparation}),
    /did not store 100%/);
});
