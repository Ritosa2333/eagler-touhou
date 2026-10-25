import assert from 'node:assert/strict';
import {assertBombResource} from './bomb-resource-contract.mjs';

const DEFAULT_TICKS=64;
const PLAYER_POSITION_EPSILON=0.01;

class ActionSceneIncomplete extends Error {}

function validMask(mask){return Number.isSafeInteger(mask)&&mask>0&&mask<=0x80000000&&Number.isInteger(Math.log2(mask));}
function validateIO(io){
  const required=['controls','directTouch','step','observe','readActions'];
  const missing=required.filter(name=>typeof io?.[name]!=='function');
  return missing;
}
function validateMasks(masks){
  for(const name of ['fire','focus','bomb','escape'])assert.ok(validMask(masks?.[name]),`STG action scene requires a single-bit ${name} mask`);
  assert.equal(new Set(['fire','focus','bomb','escape'].map(name=>masks[name])).size,4,'STG action masks must be distinct');
}
function positionOf(observation,label){
  const position=observation?.position;
  assert.ok(Number.isFinite(position?.x)&&Number.isFinite(position?.y),`${label} needs the native player position`);
  return{x:position.x,y:position.y};
}
function ensureConsumer(state,label){
  if(state?.available===false)throw new ActionSceneIncomplete(state.reason??'Native STG action observer is unavailable');
  assert.equal(state?.available,true,`${label}: native STG action snapshot is unavailable`);
  assert.equal(state?.complete,true,`${label}: no completed player-consumer snapshot`);
  assert.equal(state.phase,'consumer-run',`${label}: expected an unpaused consumer sample`);
}
function assertAction(state,mask,expected,label){
  ensureConsumer(state,label);
  const name=mask.name;
  assert.equal(Boolean(state.consumed?.[name]),expected,`${label}: consumed ${name} state differs`);
  assert.equal(Boolean(state.consumed.current&mask.value),expected,`${label}: raw consumed input mask differs`);
}

/** Recompute actual shots, one Bomb, paused world frames and released consumers. */
export function assertTouchActionEvidence(e,{inputMasks=e?.inputMasks,escapePulseTicks=e?.escapePulseTicks,bombResource=e?.bombResource}={}){
  assert.deepEqual(e.bombResource,bombResource);const bombCost=assertBombResource(bombResource);
  assert.equal(e.schema,'eagler-touhou/touch-action-scenes/1');assert.deepEqual(e.inputMasks,inputMasks);validateMasks(inputMasks);
  assert.equal(e.escapePulseTicks,escapePulseTicks);
  const active=e.samples.filter(row=>row.action?.complete===true);assert.ok(active.length>5);
  let previous=null;
  for(const [index,row] of active.entries()){
    ensureConsumer(row.action,'saved action');
    if(previous&&row.action.sequence===previous.action.sequence){
      assert.equal(row.runtime.pause.active,true,'A repeated player receipt is only valid during native pause');
      assert.deepEqual(row.action,previous.action,'Paused player receipt changed without a new consumer');
      assert.equal(row.runtime.frames[e.worldFrameKey],previous.runtime.frames[e.worldFrameKey],'Paused world frame changed');
    }else{if(previous)assert.equal(row.action.sequence,previous.action.sequence+1);previous=row;}
    for(const name of ['fire','focus','bomb','escape'])assert.equal(!!(row.action.consumed.current&inputMasks[name]),row.action.consumed[name]);
  }
  const matching=prefix=>active.filter(row=>row.label.startsWith(prefix+'-tick-'));
  const last=prefix=>{const rows=matching(prefix);assert.ok(rows.length,`Missing ${prefix}`);return rows.at(-1);};
  const on=last('focus-on'),off=last('focus-off'),fire=last('fire-on'),released=last('fire-off');
  assert.equal(on.action.focused,true);assert.equal(on.action.consumed.focus,true);assert.equal(off.action.focused,false);assert.equal(off.action.consumed.focus,false);
  assert.equal(fire.action.consumed.fire,true);assert.equal(released.action.consumed.fire,false);
  assert.ok([...matching('fire-on'),...matching('fire-projectile')].some(row=>row.action.activePlayerShots>off.action.activePlayerShots),'Fire created no native shot');
  assert.equal(released.action.bomb.active,false);assert.equal(released.action.bomb.triggered,false);
  const bomb=last('ordinary-bomb'),inventory=released.action.inventory;
  assert.ok(bomb.action.consumed.current&inputMasks.bomb);assert.equal(bomb.action.bomb.active,true);assert.equal(bomb.action.bomb.triggered,true);
  for(const row of active.filter(row=>row.action.sequence>=bomb.action.sequence)){
    assert.equal(row.action.inventory.bombsRemaining,inventory.bombsRemaining-bombCost);assert.equal(row.action.inventory.bombsUsed,inventory.bombsUsed+1);
    assert.equal(row.action.inventory.bombsUsedStage,inventory.bombsUsedStage+1);
  }
  const moved=last('bomb-with-movement-pointer');assert.equal(moved.action.bomb.active,true);
  assert.ok(Math.hypot(moved.runtime.position.x-released.runtime.position.x,moved.runtime.position.y-released.runtime.position.y)>PLAYER_POSITION_EPSILON,'Bomb cancelled movement');
  const transition=label=>{const rows=e.pauseTransitions.filter(row=>row.label===label);assert.equal(rows.length,1);return rows[0];};
  const paused=transition('escape-paused'),expired=transition('old-escape-pulse-expired'),resume=transition('escape-resume-requested'),resumed=transition('resumed');
  assert.equal(resume.serial,paused.serial+1);assert.equal(resumed.serial,resume.serial);assert.equal(expired.samples.length,escapePulseTicks+1);
  const key=e.worldFrameKey;assert.ok(typeof key==='string'&&key);assert.equal(e.samples[0].runtime.restartFrameKey,key);
  const frame=paused.sample.runtime.frames[key];assert.ok(Number.isFinite(frame));
  for(const row of [paused.sample,...expired.samples]){
    assert.equal(row.runtime.pause.active,true);assert.equal(row.runtime.frames[key],frame,'Native world advanced during pause');
    assert.ok(row.action.complete!==true||row.action.sequence<=paused.consumerSequenceBefore);
  }
  assert.equal(resumed.sample.runtime.pause.active,false);assert.ok(resumed.sample.action.sequence>paused.consumerSequenceBefore);
  const cleanup=e.cleanup;assert.deepEqual(cleanup.errors,[]);assert.equal(cleanup.pointerReceipt?.acknowledged,true);assert.equal(cleanup.controlReceipt?.acknowledged,true);
  assert.ok(cleanup.samples.length>=escapePulseTicks+1);const final=cleanup.samples.at(-1);ensureConsumer(final.action,'cleanup');
  assert.equal(final.action.consumed.current&Object.values(inputMasks).reduce((a,b)=>a|b,0),0,'Action input stayed held after cleanup');
  assert.ok(final.runtime.frames[key]>frame,'Native world did not resume');
}

/**
 * Exercise common STG action edges through the already-running Runtime.
 * IO owns only Launcher controls, real pointer delivery, bounded manual ticks,
 * and read-only observations; no expected action state comes from controls.
 */
export async function runTouchActionSceneGroup(io,{inputMasks,timeoutTicks=DEFAULT_TICKS,scenarioSet='core',escapePulseTicks,worldFrameKey,bombResource}={}){
  const missing=validateIO(io);
  const evidence={schema:'eagler-touhou/touch-action-scenes/1',scenarioSet,timeoutTicks,escapePulseTicks:escapePulseTicks??null,
    worldFrameKey:worldFrameKey??null,bombResource:bombResource?structuredClone(bombResource):null,inputMasks:inputMasks?structuredClone(inputMasks):null,controls:[],samples:[],positionChecks:[],pauseTransitions:[],cleanup:null};
  if(missing.length)return{checkStatus:'incomplete',reason:`Touch action IO is missing ${missing.join(', ')}`,evidence};
  validateMasks(inputMasks);
  if(!bombResource)return{checkStatus:'incomplete',reason:'Declare the native ordinary Bomb resource cost and unit',evidence};
  const bombCost=assertBombResource(bombResource);
  if(!Number.isInteger(timeoutTicks)||timeoutTicks<1||timeoutTicks>120)throw new RangeError('Touch action timeoutTicks must be between 1 and 120');
  if(!Number.isInteger(escapePulseTicks)||escapePulseTicks<1||escapePulseTicks>=120)
    return{checkStatus:'incomplete',reason:'The title must declare a bounded Escape pulse duration in controller ticks.',evidence};

  const masks=Object.fromEntries(['fire','focus','bomb','escape'].map(name=>[name,{name,value:inputMasks[name]}]));
  const input={fireEnabled:false,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0,touchSensitivity:100};
  const pointerId=9417;
  let pointerDown=false;
  let previousSequence=null;
  let pointerX=.58,pointerY=.75;
  let result=null,failure=null,cleanupError=null;
  const record=async label=>{
    const action=await io.readActions();
    const runtime=await io.observe();
    const entry={label,action,runtime:{pause:runtime?.pause??null,position:runtime?.position??null,phase:runtime?.phase??null,
      worldProgress:runtime?.worldProgress??null,frames:runtime?.frames??null,restartFrameKey:runtime?.capabilities?.restartFrame??null}};
    evidence.samples.push(entry);
    return entry;
  };
  const setControls=async(label,fields)=>{
    Object.assign(input,fields);
    const acknowledgement=await io.controls({...input});
    if(acknowledgement?.acknowledged===false)throw new Error(`${label}: Runtime did not acknowledge touch controls`);
    evidence.controls.push({label,controls:{...input},acknowledgement:acknowledgement??null});
  };
  const tick=async label=>{await io.step(1);return record(label);};
  const nextConsumer=async(label,afterSequence,{predicate=()=>true,maxTicks=8}={})=>{
    let lastSequence=afterSequence;
    assert.ok(Number.isSafeInteger(lastSequence)&&lastSequence>=0,`${label}: needs a prior consumer sequence`);
    for(let index=0;index<maxTicks;index++){
      const sample=await tick(`${label}-tick-${index+1}`);
      if(sample.action?.available===false)throw new ActionSceneIncomplete(sample.action.reason??'Native STG action observer is unavailable');
      if(sample.action?.complete){
        assert.equal(sample.action.sequence,lastSequence+1,`${label}: completed consumer sequence skipped or repeated a player update`);
        lastSequence=sample.action.sequence;
        previousSequence=lastSequence;
        if(predicate(sample.action))return sample;
      }
    }
    throw new ActionSceneIncomplete(`${label}: no qualifying player-consumer sample within ${maxTicks} manual ticks`);
  };

  try{
    await setControls('neutral-baseline',{});
    let first=await record('initial-consumer');
    for(let index=0;!first.action?.complete&&index<8;index++)first=await tick(`initial-consumer-tick-${index+1}`);
    if(first.action?.available===false)throw new ActionSceneIncomplete(first.action.reason??'Native STG action observer is unavailable');
    ensureConsumer(first.action,'initial consumer');
    if(first.action.inventory.bombsRemaining<bombCost)throw new ActionSceneIncomplete(`Prepare at least ${bombCost} native ${bombResource.unit} before the ordinary Bomb scene`);
    previousSequence=first.action.sequence;
    assert.equal(first.action.bomb.active,false,'Action scene requires no active Bomb before testing a new press');
    assert.equal(first.action.bomb.triggered,false,'Action scene requires no pending Bomb trigger before testing a new press');
    const initialPosition=positionOf(first.runtime,'Initial sample');

    await io.directTouch('down',pointerId,.5,.75);pointerDown=true;pointerX=.5;
    await io.directTouch('move',pointerId,.58,.75);pointerX=.58;
    const focusOn=await setAndRead('focus-on',{focusEnabled:true},masks.focus,true);
    assert.equal(focusOn.action.focused,true,'Focus owner did not enter focused movement state');
    const focusOff=await setAndRead('focus-off',{focusEnabled:false},masks.focus,false);
    assert.equal(focusOff.action.focused,false,'Focus owner did not return to unfocused movement state');

    const fireStartShots=focusOff.action.activePlayerShots;
    const fireOn=await setAndRead('fire-on',{fireEnabled:true},masks.fire,true);
    let fired=fireOn;
    for(let index=0;index<timeoutTicks&&fired.action.activePlayerShots<=fireStartShots;index++)
      fired=await nextConsumer('fire-projectile',previousSequence,{maxTicks:1});
    assert.ok(fired.action.activePlayerShots>fireStartShots,
      `Fire input did not create an active native player shot within ${timeoutTicks} ticks`);
    const neutralBeforeBomb=await setAndRead('fire-off',{fireEnabled:false},masks.fire,false);
    assert.equal(neutralBeforeBomb.action.bomb.active,false,'Ordinary Bomb baseline must be inactive immediately before its press');
    assert.equal(neutralBeforeBomb.action.bomb.triggered,false,'Ordinary Bomb baseline must have no pending trigger');
    const bombsBefore=neutralBeforeBomb.action.inventory;
    const playerBeforeBomb=positionOf((await io.observe()),'Before bomb');
    await setControls('bomb-press',{bombSerial:input.bombSerial+1});
    const bomb=await readNext('ordinary-bomb',previousSequence,state=>Boolean(state.consumed.current&masks.bomb.value));
    assertAction(bomb.action,masks.bomb,true,'ordinary bomb');
    assert.equal(bomb.action.bomb.active,true,'Ordinary bomb did not become active');
    assert.equal(bomb.action.bomb.triggered,true,'Ordinary bomb trigger was not recorded');
    assert.equal(bomb.action.inventory.bombsRemaining,bombsBefore.bombsRemaining-bombCost,'Ordinary bomb did not spend its declared native resource cost');
    assert.equal(bomb.action.inventory.bombsUsed,bombsBefore.bombsUsed+1,'Ordinary bomb use count did not advance by one');
    assert.equal(bomb.action.inventory.bombsUsedStage,bombsBefore.bombsUsedStage+1,'Stage bomb use count did not advance by one');
    await io.directTouch('move',pointerId,.70,.75);pointerX=.70;
    let movingBomb=bomb;
    for(let index=0;index<4;index++)movingBomb=await nextConsumer('bomb-with-movement-pointer',previousSequence,{maxTicks:1,
      predicate:state=>state.bomb.active});
    const playerDuringBomb=positionOf(movingBomb.runtime,'Bomb movement sample');
    assert.ok(Math.hypot(playerDuringBomb.x-playerBeforeBomb.x,playerDuringBomb.y-playerBeforeBomb.y)>PLAYER_POSITION_EPSILON,
      'The held movement pointer did not move the native player while the ordinary bomb was active');
    evidence.positionChecks.push({label:'movement-pointer-during-bomb',before:playerBeforeBomb,after:playerDuringBomb,
      bombActive:movingBomb.action.bomb.active,pointerId});

    let sequenceBeforePause=previousSequence;
    await setControls('escape-pause-press',{escapeSerial:input.escapeSerial+1});
    let paused=null;
    for(let index=0;index<8;index++){
      const sample=await tick(`escape-pause-tick-${index+1}`);
      if(sample.action?.available===false)throw new ActionSceneIncomplete(sample.action.reason??'Native STG action observer is unavailable');
      if(sample.runtime?.pause?.active===true){paused=sample;break;}
      ensureConsumer(sample.action,'Unpaused input processing before pause');
      assert.equal(sample.action.sequence,previousSequence+1,'Player consumer skipped a Tick before native pause began');
      previousSequence=sequenceBeforePause=sample.action.sequence;
    }
    assert.ok(paused,'The Escape action did not open the native pause menu');
    evidence.pauseTransitions.push({label:'escape-paused',serial:input.escapeSerial,sample:paused,
      consumerSequenceBefore:sequenceBeforePause});

    const pulseExpiryTicks=escapePulseTicks+1;
    const pulseExpirySamples=[];
    for(let index=0;index<pulseExpiryTicks;index++){
      const sample=await tick(`old-escape-pulse-expiry-tick-${index+1}`);
      if(sample.action?.available===false)throw new ActionSceneIncomplete(sample.action.reason??'Native STG action observer is unavailable');
      assert.equal(sample.runtime?.pause?.active,true,
        `Pause ended before a second Escape pulse after the declared ${escapePulseTicks}-tick pulse expired`);
      if(sample.action?.complete===true&&Number.isSafeInteger(sample.action.sequence)&&sample.action.sequence>sequenceBeforePause)
        assert.fail('The player consumer advanced while the authoritative pause state remained active');
      pulseExpirySamples.push(sample);
    }
    evidence.pauseTransitions.push({label:'old-escape-pulse-expired',serial:input.escapeSerial,
      requiredControllerTicks:pulseExpiryTicks,samples:pulseExpirySamples});

    const resumeSerial=input.escapeSerial+1;
    await setControls('escape-resume-press',{escapeSerial:resumeSerial});
    evidence.pauseTransitions.push({label:'escape-resume-requested',serial:resumeSerial,
      consumerSequenceBefore:sequenceBeforePause});
    let resumed=null;
    for(let index=0;index<timeoutTicks;index++){
      const sample=await tick(`escape-resume-tick-${index+1}`);
      if(sample.action?.available===false)throw new ActionSceneIncomplete(sample.action.reason??'Native STG action observer is unavailable');
      const consumer=sample.action;
      if(consumer?.complete===true&&Number.isSafeInteger(consumer.sequence)&&consumer.sequence>sequenceBeforePause){
        assert.equal(consumer.sequence,previousSequence+1,
          'The resumed player-consumer sequence skipped or repeated a player update');
        assert.equal(sample.runtime?.pause?.active,false,
          'The player consumer advanced before the authoritative pause state cleared');
        previousSequence=consumer.sequence;
        if(consumer.phase==='consumer-run'){
          resumed=sample;
          break;
        }
      }
    }
    assert.ok(resumed,`A second Escape pulse did not clear pause and produce a new player-consumer sample within ${timeoutTicks} logical ticks`);
    ensureConsumer(resumed.action,'resumed player consumer');
    assert.ok(resumed.action.sequence>sequenceBeforePause,
      'The resumed player consumer did not advance beyond the pre-pause sequence');
    evidence.pauseTransitions.push({label:'resumed',serial:resumeSerial,
      consumerSequenceBefore:sequenceBeforePause,sample:resumed});

    evidence.initialPosition=initialPosition;evidence.finalSequence=resumed.action.sequence;
    result={checkStatus:'passed',reason:null,cleanupControls:input,evidence};

    async function setAndRead(label,fields,mask,expected){
      const before=previousSequence;
      await setControls(label,fields);
      const sample=await readNext(label,before,state=>Boolean(state.consumed.current&mask.value)===expected);
      assertAction(sample.action,mask,expected,label);
      previousSequence=sample.action.sequence;
      return sample;
    }
    async function readNext(label,before,predicate){
      const sample=await nextConsumer(label,before,{predicate});
      previousSequence=sample.action.sequence;
      return sample;
    }
  }catch(error){
    if(error instanceof ActionSceneIncomplete)result={checkStatus:'incomplete',reason:error.message,cleanupControls:input,evidence};
    else failure=error;
  }finally{
    Object.assign(input,{fireEnabled:false,focusEnabled:false,joystickX:0,joystickY:0,touchSensitivity:100});
    const cleanupControls={...input};
    let pointerReleased=!pointerDown,controlNeutral=false,pointerReceipt=null,controlReceipt=null;
    const cleanupFailures=[];
    try{
      if(pointerDown){pointerReceipt=await io.directTouch('up',pointerId,pointerX,pointerY);pointerReleased=true;}
    }catch(error){cleanupFailures.push(error);}
    try{
      const acknowledgement=await io.controls(cleanupControls);
      controlReceipt=acknowledgement??null;
      controlNeutral=acknowledgement?.acknowledged!==false;
      if(!controlNeutral)cleanupFailures.push(new Error('Runtime did not acknowledge neutral touch action controls'));
    }catch(error){cleanupFailures.push(error);}
    const cleanupSamples=[];
    if(!failure&&result?.checkStatus==='passed'&&worldFrameKey)try{
      for(let n=0;n<escapePulseTicks+2;n++){await io.step(1);cleanupSamples.push(await record(`cleanup-input-drain-${n+1}`));}
    }catch(error){cleanupFailures.push(error);}
    evidence.cleanup={pointerReleased,controlsNeutral:controlNeutral,controls:cleanupControls,pointerReceipt,controlReceipt,samples:cleanupSamples,
      errors:cleanupFailures.map(error=>error.message)};
    if(cleanupFailures.length)cleanupError=cleanupFailures.length===1?cleanupFailures[0]:new AggregateError(cleanupFailures,'Touch action cleanup failed');
    if(failure){
      failure.touchActionCleanupControls=input;
      if(cleanupError)failure.touchActionCleanupError=cleanupError.message;
    }
  }
  if(failure){failure.touchActionCleanupControls=input;failure.touchActionEvidence=evidence;if(cleanupError)failure.touchActionCleanupError=cleanupError.message;throw failure;}
  if(cleanupError){const error=new Error(`Touch action cleanup failed: ${cleanupError.message}`,{cause:cleanupError});
    error.touchActionCleanupControls=input;error.touchActionEvidence=evidence;throw error;}
  return result;
}
