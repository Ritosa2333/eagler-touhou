import test from 'node:test';
import assert from 'node:assert/strict';
import {runTouchDoubleTapBombScene} from './touch-double-tap-scenes.mjs';

const inputMasks={fire:1,focus:4,bomb:2,escape:8};
const options=enabled=>({inputMasks,launchOptions:{doubleTapBombEnabled:enabled,source:'runtime-launch-options'},
  gesture:{firstTapMaxMs:220,interTapMaxMs:320,bombPulseTicks:3,source:'TouchController::pointer | TouchController::sample'},
  geometry:{movementEpsilon:.01,primary:{id:41,start:{x:.48,y:.76},move:{x:.55,y:.76},bombMove:{x:.62,y:.76}},
    secondary:{id:42,point:{x:.8,y:.6}}},
  budgets:{baselineTicks:4,movementSettlingTicks:3,movementSettlingSource:'touchMovementContract.inputBoundary',
    triggerTicks:8,pulseDrainTicks:4}});

function makeIO({enabled=true,singleTapBomb=false,disabledDoubleTapBomb=false,repeatBombOnPulse=false,dropPrimaryOnBomb=false,
  noDoubleTapActivation=false,missingAction=false,omitCheckpoint=false,lateSecondTap=false,queuedMovementTicks=2,
  delayedSecondDownReceiptMs=0,cleanupPointerFailure=false,cleanupControlFailure=false,missingInitialControlsAck=false,
  timeoutFirstTouchBatch=false,timeoutActions=false}={}){
  let now=0,frame=0,sequence=0,primaryDown=false,primaryDropped=false,armedAt=null,secondaryDownAt=null,pendingBomb=false,pulse=0,
    activeTicks=0,bombsRemaining=3,bombsUsed=0,bombsUsedStage=0,controlCalls=0,touchBatchCalls=0,position={x:320,y:350},
    latest={available:true,complete:false,phase:'none',sequence:0};
  const history=[],pendingMoves=[];
  const snapshot=()=>({available:true,complete:true,phase:'consumer-run',sequence,frame,stage:1,
    consumed:{current:latest.current??0,previous:latest.previous??0,bomb:Boolean((latest.current??0)&inputMasks.bomb)},
    bomb:{active:activeTicks>0,triggered:activeTicks>0,type:1,timer:20-activeTicks},
    inventory:{bombsRemaining,bombsUsed,bombsUsedStage}});
  const observation=()=>({position:{...position},pause:{active:false},phase:'gameplay'});
  const step=async count=>{
    for(let n=0;n<count;n++){
      now+=16;frame++;sequence++;
      if(pendingBomb){pendingBomb=false;pulse=3;}
      const current=pulse>0?inputMasks.bomb:0;
      if(current&inputMasks.bomb){
        if(activeTicks===0||repeatBombOnPulse){bombsRemaining--;bombsUsed++;bombsUsedStage++;}
        activeTicks=Math.max(activeTicks,20);
      }
      if(activeTicks>0)activeTicks--;
      pulse=Math.max(0,pulse-1);
      for(let index=pendingMoves.length-1;index>=0;index--){
        pendingMoves[index]--;
        if(pendingMoves[index]<=0){
          if(primaryDown&&!primaryDropped)position={x:position.x+1,y:position.y};
          pendingMoves.splice(index,1);
        }
      }
      latest={current,previous:0};
      history.push({frame,sequence,current,bombsRemaining,bombsUsed,bombsUsedStage,active:activeTicks>0});
    }
  };
  const read=async()=>missingAction?{available:false,reason:'native action observer unavailable'}:snapshot();
  return{
    history,
    async controls(value){assert.equal(value.fireEnabled,false);assert.equal(value.focusEnabled,false);assert.equal(value.bombSerial,0);
      controlCalls++;
      if(missingInitialControlsAck&&controlCalls===1)return{};
      if(cleanupControlFailure&&history.length>0)return{acknowledged:false};return{acknowledged:true};},
    async step(count){await step(count);},
    async readActions(){if(timeoutActions)throw Object.assign(Error('native action RPC timed out'),{name:'TimeoutError'});return read();},
    async observe(){return observation();},
    async directTouchBatch(operations){
      touchBatchCalls++;
      if(timeoutFirstTouchBatch&&touchBatchCalls===1)throw Error('Timeout: direct-touch');
      const events=[],checkpoints=[];
      for(let operationIndex=0;operationIndex<operations.length;operationIndex++){
        const operation=operations[operationIndex];
        if(operation.type==='checkpoint'){
          const samples=[];
          if(omitCheckpoint!==operation.label)for(let tick=0;tick<operation.ticks;tick++){
            await step(1);samples.push({action:await read(),observation:observation()});
          }
          checkpoints.push({label:operation.label,samples});continue;
        }
        if(operation.id===42&&operation.type==='down'&&armedAt!==null&&lateSecondTap)now=Math.max(now,armedAt+400);
        const startedAtMs=now;
        now+=1;
        if(operation.id===41){
          if(operation.type==='down')primaryDown=true;
          if(operation.type==='move')pendingMoves.push(queuedMovementTicks);
          if(operation.type==='up')primaryDown=false;
        }
        if(operation.id===42){
          if(operation.type==='down'){
            secondaryDownAt=now;
            if(armedAt!==null&&now-armedAt<=320){
              if(enabled&&!noDoubleTapActivation)pendingBomb=true;
              if(disabledDoubleTapBomb)pendingBomb=true;
              armedAt=null;
              if(dropPrimaryOnBomb)primaryDropped=true;
            }
          }
          if(operation.type==='up'){
            if(secondaryDownAt!==null){
              if(singleTapBomb)pendingBomb=true;
              if(armedAt===null&&!pendingBomb)armedAt=now;
            }
            secondaryDownAt=null;
          }
        }
        const receiptDelay=delayedSecondDownReceiptMs&&operationIndex===3&&operations.length>5
          ?delayedSecondDownReceiptMs:0;
        now+=receiptDelay;
        events.push({operationIndex,type:operation.type,id:operation.id,startedAtMs,completedAtMs:now,
          acknowledged:!(cleanupPointerFailure&&operation.type==='up'&&operations.length===2&&operationIndex===0)});
      }
      return{available:true,acknowledged:true,events,checkpoints};
    },
  };
}

test('enabled double tap ignores the first click, consumes one native Bomb, and preserves the primary movement finger',async()=>{
  const io=makeIO(),result=await runTouchDoubleTapBombScene(io,options(true));
  assert.equal(result.checkStatus,'passed');
  const single=result.evidence.samples.find(sample=>sample.label==='single-tap-tick-1').action;
  assert.equal(single.consumed.bomb,false);
  assert.deepEqual(single.inventory,{bombsRemaining:3,bombsUsed:0,bombsUsedStage:0});
  assert.equal(result.evidence.bombObservation.observedBombsUsedDelta,1);
  assert.equal(result.evidence.bombObservation.expectedActivationCount,1);
  assert.ok(result.evidence.positionChecks.some(row=>row.label==='primary-finger-during-native-bomb'));
  assert.equal(result.evidence.cleanup.pointerReleased,true);
  assert.equal(result.evidence.cleanup.controlsNeutral,true);
});

test('disabled double tap option is a real paired-gesture negative control',async()=>{
  const result=await runTouchDoubleTapBombScene(makeIO({enabled:false}),options(false));
  assert.equal(result.checkStatus,'passed');
  assert.equal(result.evidence.bombObservation.observedBombsUsedDelta,0);
  assert.equal(result.evidence.samples.some(sample=>sample.action?.consumed?.bomb===true),false);
});

test('a single click that consumes Bomb cannot be hidden by a later successful double tap',async()=>{
  await assert.rejects(runTouchDoubleTapBombScene(makeIO({singleTapBomb:true}),options(true)),
    /Single tap: native consumer received Bomb input/);
});

test('a disabled option that still consumes the paired gesture fails closed',async()=>{
  await assert.rejects(runTouchDoubleTapBombScene(makeIO({enabled:false,disabledDoubleTapBomb:true}),options(false)),
    /Disabled double-tap case: native consumer received Bomb input/);
});

test('an enabled pair with complete observations but no native Bomb activation fails',async()=>{
  await assert.rejects(runTouchDoubleTapBombScene(makeIO({noDoubleTapActivation:true}),options(true)),
    /Enabled double-tap did not produce exactly one native Bomb/);
});

test('an inter-tap transport outside the title window is incomplete instead of a behavior verdict',async()=>{
  const result=await runTouchDoubleTapBombScene(makeIO({lateSecondTap:true}),options(true));
  assert.equal(result.checkStatus,'incomplete');
  assert.match(result.reason,/transport exceeded the title-declared/);
});

test('inter-tap timing uses the conservative RPC bracket, not its minimum possible delay',async()=>{
  const result=await runTouchDoubleTapBombScene(makeIO({delayedSecondDownReceiptMs:400}),options(true));
  assert.equal(result.checkStatus,'incomplete');
  assert.ok(result.evidence.gestureTiming.interTapMinElapsedMs<=320);
  assert.ok(result.evidence.gestureTiming.interTapMaxElapsedMs>320);
});

test('repeated inventory use during one held Bomb pulse is rejected',async()=>{
  await assert.rejects(runTouchDoubleTapBombScene(makeIO({repeatBombOnPulse:true}),options(true)),
    /more than one native Bomb use|repeated a native stage Bomb use/);
});

test('losing the primary pointer during the secondary double tap fails movement preservation',async()=>{
  await assert.rejects(runTouchDoubleTapBombScene(makeIO({dropPrimaryOnBomb:true}),options(true)),
    /primary movement finger did not produce native movement/);
});

test('missing native action observation is incomplete and cannot pass from the touch receipt',async()=>{
  const result=await runTouchDoubleTapBombScene(makeIO({missingAction:true}),options(true));
  assert.equal(result.checkStatus,'incomplete');
  assert.match(result.reason,/native action observer unavailable|completed player-consumer/);
});

test('unacknowledged control neutralization is incomplete',async()=>{
  const result=await runTouchDoubleTapBombScene(makeIO({missingInitialControlsAck:true}),options(true));
  assert.equal(result.checkStatus,'incomplete');
  assert.match(result.reason,/did not acknowledge neutral touch controls/);
});

test('a direct-touch transport timeout remains incomplete and still releases the pointer owner',async()=>{
  const result=await runTouchDoubleTapBombScene(makeIO({timeoutFirstTouchBatch:true}),options(true));
  assert.equal(result.checkStatus,'incomplete');
  assert.match(result.reason,/direct-touch-batch: Runtime IO timed out/);
  assert.equal(result.evidence.cleanup.pointerReleased,true);
  assert.ok(result.evidence.ioErrors.some(row=>row.label==='primary-movement-owner-direct-touch-batch'));
});

test('a native observer timeout remains incomplete instead of becoming a product failure',async()=>{
  const result=await runTouchDoubleTapBombScene(makeIO({timeoutActions:true}),options(true));
  assert.equal(result.checkStatus,'incomplete');
  assert.match(result.reason,/native-action-observation: Runtime IO timed out/);
  assert.ok(result.evidence.ioErrors.some(row=>row.label==='initial-native-baseline-native-action-observation'));
});

test('a missing checkpoint receipt is incomplete instead of assuming the click was consumed',async()=>{
  const result=await runTouchDoubleTapBombScene(makeIO({omitCheckpoint:'single-tap'}),options(true));
  assert.equal(result.checkStatus,'incomplete');
  assert.match(result.reason,/checkpoint single-tap/);
});

test('cleanup failure preserves the original incomplete scene reason and evidence',async()=>{
  await assert.rejects(runTouchDoubleTapBombScene(makeIO({missingAction:true,cleanupControlFailure:true}),options(true)),error=>{
    assert.match(error.message,/incomplete.*native action observer unavailable/);
    assert.match(error.message,/cleanup failed/);
    assert.equal(error.touchDoubleTapBombEvidence.sceneOutcome.checkStatus,'incomplete');
    assert.match(error.touchDoubleTapBombEvidence.sceneOutcome.reason,/native action observer unavailable/);
    assert.ok(error.touchDoubleTapBombEvidence.cleanup.errors.length>0);
    return true;
  });
});

test('movement assertions wait through the title-declared queued-consumption budget',async()=>{
  const result=await runTouchDoubleTapBombScene(makeIO({queuedMovementTicks:2}),options(true));
  assert.equal(result.checkStatus,'passed');
  assert.equal(result.evidence.positionChecks[0].after.x-result.evidence.positionChecks[0].before.x,1);
  assert.ok(result.evidence.positionChecks[1].settlingSamples.length>=2);
});
