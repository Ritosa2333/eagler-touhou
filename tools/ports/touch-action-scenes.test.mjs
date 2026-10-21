import test from 'node:test';
import assert from 'node:assert/strict';
import {runTouchActionSceneGroup} from './touch-action-scenes.mjs';

const inputMasks={fire:1,focus:4,bomb:2,escape:8};

function makeIO({focusStuck=false,bombInventoryStuck=false,escapeStuck=false,delayBombPress=false,skipSequenceOnDelayedBomb=false,
  initialBombActive=false,initialBombTriggered=false,dropSecondEscapeSerial=false}={}){
  const controlsHistory=[];
  const processedEscapeSerials=[];
  let controls={fireEnabled:false,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0,touchSensitivity:100};
  let pointerDown=false,position={x:320,y:360},pauseActive=false;
  let sequence=0,frame=0,previousButtons=0,seenBombSerial=0,seenEscapeSerial=0,bombTicks=0,bombDelayUsed=false;
  let bombsRemaining=3,bombsUsed=0,bombsUsedStage=0,shots=0;
  let action={available:true,complete:false,phase:'none',sequence:0};
  const snapshot=()=>({available:true,complete:true,phase:'consumer-run',sequence,frame,stage:1,
    consumed:{current:action.current??0,previous:action.previous??0,
      fire:Boolean((action.current??0)&inputMasks.fire),focus:Boolean((action.current??0)&inputMasks.focus),
      bomb:Boolean((action.current??0)&inputMasks.bomb),escape:Boolean((action.current??0)&inputMasks.escape)},
    focused:!focusStuck&&Boolean(controls.focusEnabled),shootingTimer:{current:controls.fireEnabled?sequence:0,previous:0},
    activePlayerShots:shots,bomb:{active:action.bombActiveOverride??bombTicks>0,
      triggered:action.bombTriggeredOverride??Boolean(action.triggered),type:1,timer:bombTicks},
    inventory:{bombsRemaining,bombsUsed,bombsUsedStage},lifeState:0});
  return{
    controlsHistory,
    processedEscapeSerials,
    async controls(next){
      for(const key of ['fireEnabled','focusEnabled','bombSerial','escapeSerial','joystickX','joystickY','touchSensitivity'])
        assert.ok(Object.hasOwn(next,key),`control snapshot lacks ${key}`);
      const old=controlsHistory.at(-1)?.controls;
      if(old){assert.ok(next.bombSerial>=old.bombSerial,'bomb serial moved backwards');assert.ok(next.escapeSerial>=old.escapeSerial,'escape serial moved backwards');}
      controls={...next};
      if(dropSecondEscapeSerial&&next.escapeSerial>=2)controls.escapeSerial=1;
      controlsHistory.push({controls:{...controls},requested:{...next}});return{acknowledged:true};
    },
    async directTouch(type,id,x,y){
      assert.equal(id,9417);
      if(type==='down')pointerDown=true;
      if(type==='up')pointerDown=false;
      if(type==='move'||type==='down'||type==='up')position={x:position.x+(x-.5)*20,y:position.y+(y-.75)*20};
    },
    async step(count){
      for(let i=0;i<count;i++){
        frame++;
        if(controls.escapeSerial>seenEscapeSerial){
          seenEscapeSerial=controls.escapeSerial;processedEscapeSerials.push(seenEscapeSerial);
          if(!escapeStuck){
            if(!pauseActive)pauseActive=true;
            else if(seenEscapeSerial>=2)pauseActive=false;
          }
        }
        if(pauseActive){action={available:true,complete:false,phase:'paused',sequence:null,frame,stage:1};continue;}
        let bombPressed=false,delayedBombRow=false;
        if(controls.bombSerial>seenBombSerial){
          if(delayBombPress&&!bombDelayUsed){bombDelayUsed=true;delayedBombRow=true;}
          else{bombPressed=true;seenBombSerial=controls.bombSerial;}
        }
        if(bombPressed){if(!bombInventoryStuck){bombsRemaining--;bombsUsed++;bombsUsedStage++;}bombTicks=8;}
        const current=(controls.fireEnabled?inputMasks.fire:0)|(controls.focusEnabled?inputMasks.focus:0)|(bombPressed?inputMasks.bomb:0);
        sequence+=delayedBombRow&&skipSequenceOnDelayedBomb?2:1;
        if(controls.fireEnabled)shots++;
        if(pointerDown)position={x:position.x+1,y:position.y};
        action={available:true,complete:true,phase:'consumer-run',sequence,frame,stage:1,
          current,previous:previousButtons,triggered:bombPressed,
          bombActiveOverride:sequence===1&&initialBombActive?true:undefined,
          bombTriggeredOverride:sequence===1&&initialBombTriggered?true:undefined};
        previousButtons=current;
        if(bombTicks>0)bombTicks--;
      }
    },
    async observe(){return{position:{...position},pause:{active:pauseActive},worldProgress:{frame}};},
    async readActions(){return action.complete?snapshot():action;},
  };
}

test('the shared action scene keeps complete controls, monotonic edge serials and exposes final cleanup controls',async()=>{
  const io=makeIO();
  const result=await runTouchActionSceneGroup(io,{inputMasks,timeoutTicks:24,escapePulseTicks:3});
  assert.equal(result.checkStatus,'passed');
  assert.equal(result.evidence.cleanup.controlsNeutral,true);
  assert.equal(result.evidence.cleanup.pointerReleased,true);
  assert.deepEqual(result.cleanupControls,io.controlsHistory.at(-1).controls);
  assert.equal(result.cleanupControls.bombSerial,1);
  assert.equal(result.cleanupControls.escapeSerial,2);
  assert.equal(result.cleanupControls.fireEnabled,false);
  assert.equal(result.cleanupControls.focusEnabled,false);
  assert.ok(io.controlsHistory.length>=8);
  const pulseExpiry=result.evidence.pauseTransitions.find(item=>item.label==='old-escape-pulse-expired');
  assert.equal(pulseExpiry.requiredControllerTicks,4);
  assert.ok(pulseExpiry.samples.every(sample=>sample.runtime.pause.active===true));
  assert.deepEqual(io.processedEscapeSerials,[1,2]);
  const serialChanges=io.controlsHistory.flatMap((item,index)=>{
    const previous=io.controlsHistory[index-1]?.controls.escapeSerial??0;
    return item.controls.escapeSerial>previous?[item.controls.escapeSerial]:[];
  });
  assert.deepEqual(serialChanges,[1,2]);
  const resumed=result.evidence.pauseTransitions.find(item=>item.label==='resumed');
  assert.ok(resumed.sample.action.sequence>resumed.consumerSequenceBefore);
});

test('a native focus transition failure remains a hard failure and carries cleanup serials',async()=>{
  const io=makeIO({focusStuck:true});
  await assert.rejects(runTouchActionSceneGroup(io,{inputMasks,escapePulseTicks:3}),error=>{
    assert.match(error.message,/Focus owner did not enter focused movement state/);
    assert.equal(error.touchActionCleanupControls.bombSerial,0);
    assert.equal(error.touchActionCleanupControls.escapeSerial,0);
    return true;
  });
  assert.equal(io.controlsHistory.at(-1).controls.focusEnabled,false);
});

test('bomb resource loss and a missing pause transition cannot pass the action scene',async()=>{
  await assert.rejects(runTouchActionSceneGroup(makeIO({bombInventoryStuck:true}),{inputMasks,escapePulseTicks:3}),/did not spend one native inventory unit/);
  await assert.rejects(runTouchActionSceneGroup(makeIO({escapeStuck:true}),{inputMasks,escapePulseTicks:3}),/did not open the native pause menu/);
});

test('one queued consumer row is retained while the Bomb edge predicate arrives on the next row',async()=>{
  const result=await runTouchActionSceneGroup(makeIO({delayBombPress:true}),{inputMasks,escapePulseTicks:3});
  assert.equal(result.checkStatus,'passed');
  const bombRows=result.evidence.samples.filter(sample=>sample.label.startsWith('ordinary-bomb-tick-'));
  assert.equal(bombRows.length,2);
  assert.equal(bombRows[0].action.sequence+1,bombRows[1].action.sequence);
  assert.equal(bombRows[0].action.consumed.bomb,false);
  assert.equal(bombRows[1].action.consumed.bomb,true);
});

test('a genuinely skipped consumer sequence fails and preserves the sampled row',async()=>{
  await assert.rejects(runTouchActionSceneGroup(makeIO({delayBombPress:true,skipSequenceOnDelayedBomb:true}),{inputMasks,escapePulseTicks:3}),error=>{
    assert.match(error.message,/completed consumer sequence skipped or repeated/);
    const row=error.touchActionEvidence.samples.find(sample=>sample.label==='ordinary-bomb-tick-1');
    assert.ok(row,'the rejected sequence row remains in failure evidence');
    return true;
  });
});

test('an already-active or pending native Bomb invalidates the intended baseline',async()=>{
  await assert.rejects(runTouchActionSceneGroup(makeIO({initialBombActive:true}),{inputMasks,escapePulseTicks:3}),/no active Bomb before testing/);
  await assert.rejects(runTouchActionSceneGroup(makeIO({initialBombTriggered:true}),{inputMasks,escapePulseTicks:3}),/no pending Bomb trigger/);
});

test('a failed final control acknowledgement cannot leave a passed scene',async()=>{
  const io=makeIO();
  const controls=io.controls.bind(io);
  const directTouch=io.directTouch.bind(io);
  let pointerReleased=false;
  io.directTouch=async(...args)=>{await directTouch(...args);if(args[0]==='up')pointerReleased=true;};
  io.controls=async snapshot=>{
    await controls(snapshot);
    if(pointerReleased&&!snapshot.fireEnabled&&!snapshot.focusEnabled&&snapshot.bombSerial===1&&snapshot.escapeSerial===2)
      return{acknowledged:false};
    return{acknowledged:true};
  };
  await assert.rejects(runTouchActionSceneGroup(io,{inputMasks,escapePulseTicks:3}),/Touch action cleanup failed/);
  assert.equal(io.controlsHistory.at(-1).controls.bombSerial,1);
  assert.equal(io.controlsHistory.at(-1).controls.escapeSerial,2);
});

test('missing observer and invalid action masks are explicit',async()=>{
  const unavailable={controls(){},directTouch(){},step(){},observe(){},readActions(){return{available:false,reason:'missing export'};}};
  const result=await runTouchActionSceneGroup(unavailable,{inputMasks,escapePulseTicks:3});
  assert.equal(result.checkStatus,'incomplete');
  assert.match(result.reason,/missing export/);
  await assert.rejects(runTouchActionSceneGroup(makeIO(),{inputMasks:{...inputMasks,bomb:1},escapePulseTicks:3}),/must be distinct/);
});

test('pause persists after the first Escape pulse expires and clears only on the second serial',async()=>{
  const io=makeIO();
  const controls={fireEnabled:false,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0,touchSensitivity:100};
  await io.controls({...controls,escapeSerial:1});
  await io.step(1);
  assert.equal((await io.observe()).pause.active,true);
  await io.step(4);
  assert.equal((await io.observe()).pause.active,true,'elapsed controller ticks cannot resume the fixture');
  assert.deepEqual(io.processedEscapeSerials,[1]);
  await io.controls({...controls,escapeSerial:2});
  await io.step(1);
  assert.equal((await io.observe()).pause.active,false);
  assert.deepEqual(io.processedEscapeSerials,[1,2]);
});

test('dropping the second Escape serial leaves pause active and prevents a scene pass',async()=>{
  const io=makeIO({dropSecondEscapeSerial:true});
  await assert.rejects(runTouchActionSceneGroup(io,{inputMasks,timeoutTicks:8,escapePulseTicks:3}),error=>{
    assert.match(error.message,/second Escape pulse did not clear pause/);
    assert.deepEqual(io.processedEscapeSerials,[1]);
    assert.ok(error.touchActionEvidence.samples.filter(sample=>sample.label.startsWith('old-escape-pulse-expiry-'))
      .every(sample=>sample.runtime.pause.active===true));
    return true;
  });
});

test('missing Escape pulse duration is incomplete instead of using an assumed wait',async()=>{
  const result=await runTouchActionSceneGroup(makeIO(),{inputMasks});
  assert.equal(result.checkStatus,'incomplete');
  assert.match(result.reason,/declare a bounded Escape pulse duration/);
});
