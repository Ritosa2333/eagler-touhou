import assert from 'node:assert/strict';
import {resolveAdaptationAdapter} from './registry.mjs';
import {touchMovementContract} from './movement.mjs';
import {assertControlledScene,assertControlledSceneArmed} from './scene.mjs';
import {assertTouchMotionSamples,assertTouchReleaseSamples,isTouchMotionTickEligible,
  mapFloat32TouchDragTarget,predictTouchMotionStep,TOUCH_TRACE_POSITION_EPSILON as epsilon} from '../../../tools/ports/touch-motion-contract.mjs';

const definitions={
  'shared-touch-limited':{mode:1,checks:['TOUCH-01','TOUCH-14']},
  'shared-touch-limited-restored':{mode:1,checks:['TOUCH-01','TOUCH-14']},
  'shared-touch-unlimited':{mode:2,checks:['TOUCH-02']},
  'shared-touch-joystick-free':{mode:'joystick-free',checks:['TOUCH-04']},
  'shared-touch-two-finger-focus':{mode:1,scenarioSet:'input-ownership',focusMode:'two-finger',checks:['TOUCH-07']},
};
export const motionCompleteChecks=id=>[...(definitions[id]?.checks??[])];
const same=(a,b,label)=>assert.deepEqual(a,b,label);
const near=(a,b,label)=>assert.ok(Number.isFinite(a)&&Number.isFinite(b)&&Math.abs(a-b)<=epsilon,label);
const position=row=>({x:row.afterX,y:row.afterY});

function sceneRows(rows,native,label){
  assert.ok(Array.isArray(rows)&&rows.length>=3,`${label}: three native ticks are required`);
  for(const row of rows)same(row,native.get(row.sequence),`${label}: scene row differs from the native ledger`);
  for(let i=1;i<rows.length;i++)assert.equal(rows[i].sequence,rows[i-1].sequence+1,`${label}: native sequence gap`);
  return rows;
}

function calibration(evidence,native,policy){
  const result={};
  for(const [label,focused] of [['fast',false],['slow',true]]){
    const value=evidence.calibration?.[label];
    const rows=sceneRows(value.rows,native,`${label} keyboard`);
    const direction=rows[0].beforeX<=320?'ArrowRight':'ArrowLeft',sign=direction==='ArrowRight'?1:-1;
    assert.equal(value?.direction,direction,'Calibration must use the independent horizontal keyboard path');
    assert.equal(rows.length,3,'Keyboard calibration needs exactly three held movement ticks');
    const scale={x:rows[0].scaleX,y:rows[0].scaleY};
    assert.ok(scale.x>0&&scale.y>0,'Keyboard native axis scales are absent');
    const speed=sign*(rows[0].afterX-rows[0].beforeX)/scale.x;
    assert.ok(speed>epsilon,'Keyboard reference speed must be positive');
    for(const row of rows){
      assert.equal(row.mode,0,'Touch must be inactive during keyboard calibration');
      assert.equal(!!(row.flags&4),focused,'Keyboard focus reference differs');
      near(row.afterY,row.beforeY,'Horizontal keyboard reference moved vertically');
      near(row.afterX-row.beforeX,sign*speed*scale.x,'Keyboard reference speed changed');
      near(row.speed,speed,'Reported native speed differs from actual keyboard displacement');
      near(row.scaleX,scale.x,'Keyboard X scale changed');near(row.scaleY,scale.y,'Keyboard Y scale changed');
    }
    near(value.speed,speed,'Stored calibration differs from actual displacement');
    same(value.scale,scale,'Stored calibration scale differs');
    const transitions=evidence.keyboardTransitions??[];
    const down=transitions.find(row=>row.code===direction&&row.down===true&&
      row.acknowledgement?.nativeTraceTotal===value.startSequence);
    assert.ok(down?.acknowledgement?.acknowledged&&down.acknowledgement.changed&&down.acknowledgement.stableTraceBoundary,
      'Keyboard calibration has no acknowledged native press boundary');
    const neutral=sceneRows(value.neutralRows,native,`${label} keyboard release`);
    for(const row of neutral){
      near(row.beforeX,row.afterX,'Keyboard release retained movement');near(row.beforeY,row.afterY,'Keyboard release retained movement');
      assert.equal(row.mode,0,'Keyboard release retained touch movement');
    }
    if(policy){
      const boundary=down.acknowledgement.nextMovementInputBoundary;
      assert.equal(boundary?.semantics,policy.semantics,'Keyboard calibration input semantics differ');
      assert.equal(boundary?.observationSource,policy.observationSource,'Keyboard calibration observer differs');
      assert.equal(boundary?.observationPhase,policy.observationPhase,'Keyboard calibration phase differs');
      assert.equal(boundary.traceTotal,value.startSequence,'Keyboard boundary belongs to another trace tick');
      const release=value.releaseAcknowledgements?.[0];
      assert.ok(release?.acknowledged&&release.changed&&release.stableTraceBoundary,'Keyboard reference has no acknowledged release');
      const held=release.nextMovementInputBoundary;
      assert.equal(held?.traceTotal,rows.at(-1).sequence,'Held keyboard observation belongs to another native tick');
      assert.ok(held.value&policy.directionMasks[direction],'Held keyboard reference did not queue the selected direction');
      assert.equal(!!(held.value&policy.focusMask),focused,'Held keyboard reference queued the wrong focus state');
    }
    result[label]={speed,scale};
  }
  assert.ok(result.slow.speed<result.fast.speed,'Focus must reduce the independent keyboard speed');
  return result;
}

function dragMatrix(mode){
  const rows=[];
  const add=(prefix,sensitivity,focused)=>{
    for(const [suffix,delta,release] of [
      ['micro-up',{x:.3,y:.4},'up'],['diagonal-up',{x:6,y:8},'up'],
      ['diagonal-reverse-cancel',{x:-6,y:-8},'cancel'],
    ])rows.push({label:`${prefix}-${sensitivity}-${suffix}`,sensitivity,focused,delta,release});
  };
  for(const sensitivity of [100,200,300])add('sensitivity',sensitivity,false);
  if(mode===1){
    for(const sensitivity of [100,200,300])add('focused-sensitivity',sensitivity,true);
    rows.push({label:'focus-restored-fast-diagonal-up',sensitivity:100,focused:false,delta:{x:6,y:8},release:'up'},
      {label:'focus-restored-fast-diagonal-reverse-cancel',sensitivity:100,focused:false,delta:{x:-6,y:-8},release:'cancel'});
  }
  return rows;
}

function assessDrags(evidence,native,reference,movement,mode){
  for(const expected of dragMatrix(mode)){
    const matches=evidence.scenarios.filter(row=>row.label===expected.label&&row.requestedDelta);
    assert.equal(matches.length,1,`Missing or duplicate required drag ${expected.label}`);
    const row=matches[0],speed=reference[expected.focused?'slow':'fast'];
    same(row.requestedDelta,expected.delta,'Pointer command differs from the fixed scene specification');
    assert.equal(row.sensitivityPercent,expected.sensitivity,'Sensitivity matrix changed');
    assert.equal(row.focused,expected.focused,'Focus matrix changed');assert.equal(row.release,expected.release,'Release matrix changed');
    same(row.pointerOrigin,{x:.5,y:.5},'Pointer origin differs');
    same(row.pointerDestination,{x:.5+expected.delta.x/640,y:.5+expected.delta.y/480},'Pointer destination differs');
    const down=row.downRows?.find(sample=>sample.mode===mode);
    assert.ok(down,'Native pointer-down consumption is missing');
    same(down,native.get(down.sequence),'Pointer-down is not in the native ledger');
    const start={x:down.beforeX,y:down.beforeY};same(row.startPosition,start,'Drag start differs from the native position');
    near(down.afterX,start.x,'Pointer-down moved X');near(down.afterY,start.y,'Pointer-down moved Y');
    const sensitivity=expected.sensitivity/100,focusFactor=mode===1&&expected.focused?speed.speed/reference.fast.speed:1;
    const target=movement.inputCommand?.coordinate==='float32-normalized-pointer'
      ?mapFloat32TouchDragTarget(start,expected.delta,{inputSpace:{width:640,height:480},sensitivity,focusFactor,
        pixelsPerUnit:movement.inputCommand.pixelsPerUnit})
      :{x:start.x+expected.delta.x*sensitivity*focusFactor,y:start.y+expected.delta.y*sensitivity*focusFactor};
    const samples=sceneRows(row.movementRows,native,expected.label);
    near(samples[0].beforeX,start.x,'First movement starts at another position');near(samples[0].beforeY,start.y,'First movement starts at another position');
    assertTouchMotionSamples(samples,{mode,target,speed:speed.speed,sensitivity,scale:speed.scale,
      precision:movement.precision,focused:expected.focused});
    if(mode===2){near(samples[0].afterX,target.x,'Unlimited target was delayed');near(samples[0].afterY,target.y,'Unlimited target was delayed');}
    const released=sceneRows(row.releaseRows,native,`${expected.label} release`);
    assertTouchReleaseSamples(released,{position:position(samples.at(-1)),afterSequence:row.releaseBoundarySequence});
    for(const sample of released.slice(expected.release==='cancel'?1:0))
      assert.equal(!!(sample.flags&4),expected.release==='cancel'?false:expected.focused,'Release retained the wrong focus owner');
  }
}

function assessJoystick(evidence,native,reference,movement){
  const expected=[];
  for(const angle of [23,37,71,203,217,251])for(const amplitude of [.25,.5,1]){
    const prefix=angle===23&&amplitude===.25?'focus-calibration':angle===37&&amplitude===.25?'focus-restored-fast':'free-joystick';
    for(const reverse of [false,true])expected.push({angle,amplitude,prefix,reverse,focused:prefix==='focus-calibration'});
  }
  for(const reverse of [false,true])expected.push({angle:43,amplitude:1,prefix:'radial-speed-boundary',reverse,focused:false});
  assert.equal(evidence.scenarios.length,expected.length,'Free joystick direction/amplitude matrix is incomplete');
  for(const trial of expected){
    const label=`${trial.prefix}-angle-${trial.angle}-amplitude-${trial.amplitude}-focus-${trial.focused?'on':'off'}-${trial.reverse?'reverse':'forward'}`;
    const matches=evidence.scenarios.filter(row=>row.label===label);assert.equal(matches.length,1,`Missing free joystick vector ${label}`);
    const row=matches[0],factor=trial.reverse?-1:1,radians=trial.angle*Math.PI/180;
    const command={joystickX:factor*Math.round(Math.cos(radians)*trial.amplitude*32767),joystickY:factor*Math.round(Math.sin(radians)*trial.amplitude*32767)};
    same(row.input,command,'Joystick command differs from the fixed independent vector');
    const x=command.joystickX/32767,y=command.joystickY/32767,norm=Math.max(1,Math.hypot(x,y));
    const speed=reference[trial.focused?'slow':'fast'];
    const samples=sceneRows(row.movementRows,native,label);assert.equal(samples.length,3,'Joystick must retain all three commanded ticks');
    const targets=[];let start=row.startPosition;
    near(samples[0].beforeX,start.x,'Joystick initial X differs');near(samples[0].beforeY,start.y,'Joystick initial Y differs');
    for(let i=0;i<3;i++){
      const target={x:start.x+speed.speed*x/norm,y:start.y+speed.speed*y/norm};targets.push(target);
      start=predictTouchMotionStep(start,target,{mode:1,speed:speed.speed,scale:speed.scale,precision:movement.precision});
    }
    assertTouchMotionSamples(samples,{mode:1,targets,speed:speed.speed,scale:speed.scale,precision:movement.precision,focused:trial.focused});
    const released=sceneRows(row.neutralRows,native,`${label} neutral`);
    assertTouchReleaseSamples(released,{position:position(samples.at(-1)),afterSequence:samples.at(-1).sequence});
  }
}

function assessMotionCheck(driver,entry,product,definition){
  assert.equal(driver.schema,'eagler-touhou/runtime-desktop-probes/1');
  const targets=driver.targets?.filter(row=>row.game===product.game&&row.variant===product.variant)??[];
  assert.equal(targets.length,1,'Motion evidence requires one matching selected product');
  const target=targets[0];
  assert.ok(target.checks.includes(entry)&&target.checks.filter(row=>row.id===entry.id).length===1,'Motion claim is not unique in this target');
  assert.equal(entry.status,'passed');
  const observation=entry.observation,evidence=observation?.evidence;
  const adapter=resolveAdaptationAdapter(product.game,product.variant);assert.ok(adapter?.movement,'Register title movement semantics');
  const movement=touchMovementContract(product.game,product.variant);
  same(observation.movementContract,movement,'Recorded movement contract differs from the current title declaration');
  assert.equal(observation.clock?.mode,'native-manual','Motion must use completed native logical ticks');
  assert.ok(target.gameplayReadinessByEpoch?.some(row=>row.epoch===observation.epoch&&row.ready===true),'No real gameplay readiness for this epoch');
  assert.equal(observation.input,'runtime-touch-protocol');assert.equal(evidence?.schema,'eagler-touhou/touch-motion-scenes/1');
  assert.equal(evidence.mode,definition.mode);assert.equal(evidence.scenarioSet,definition.scenarioSet??'all','A selected subset cannot complete the motion requirement');
  if(definition.focusMode){
    assert.equal(evidence.focusMode,definition.focusMode);assert.equal(observation.transition?.pagehide,true);
    const configs=target.launchOptionsByEpoch.filter(row=>row.epoch===observation.epoch);assert.equal(configs.length,1);
    assert.equal(configs[0].options.touchFocusMode,definition.focusMode);assert.equal(configs[0].acknowledgement.ok,true);
    const requests=new Set();
    for(const command of evidence.pointerCommands){
      assert.equal(command.result.protocol,'eagler-touhou/1');assert.equal(command.result.game,product.game);
      assert.equal(command.result.epoch,observation.epoch);assert.equal(command.result.ok,true);
      assert.ok(typeof command.result.request==='string'&&command.result.request&&!requests.has(command.result.request));requests.add(command.result.request);
    }
  }
  same(evidence.inputSpace,{width:640,height:480});same(evidence.precision,movement.precision);
  same(evidence.inputCommand,movement.inputCommand??null);same(evidence.queuedMovementPolicy,movement.keyboardReleasePolicy??null);
  assert.equal(evidence.inputBoundary,movement.inputBoundary);assert.equal(evidence.inputBoundarySource,movement.inputBoundarySource);
  if(adapter.controlledScene){
    assertControlledSceneArmed(observation.controlledScene?.request);
    let previous=assertControlledScene(observation.controlledScene?.acceptance);
    for(const receipt of observation.controlledScene.receipts)previous=assertControlledScene(receipt,previous);
  }
  assertTouchMotionSceneEvidence(evidence,movement,definition.mode);
}

/** Shared numeric specification for recorded raw ticks; no browser or game assets. */
export function assertTouchMotionSceneEvidence(evidence,movement,mode){
  const trace=evidence.trace;assert.ok(trace?.available&&trace.abiVersion===2&&trace.localSeat===0,'Motion requires native v2 local-seat observations');
  assert.ok(Array.isArray(evidence.nativeRows)&&evidence.nativeRows.length===trace.finalTotal-trace.initialTotal,'Native tick ledger is truncated');
  const native=new Map();
  for(const [i,row] of evidence.nativeRows.entries()){
    assert.ok(isTouchMotionTickEligible(row),'Motion ledger contains an ineligible tick');
    assert.equal(row.localSeat,0,'Motion ledger changed the local-seat stream');
    assert.equal(row.executionKind,0,'Motion ledger changed the forward execution stream');
    assert.equal(row.sequence,trace.initialTotal+i+1,'Native tick ledger has a gap');
    if(i){
      const previous=evidence.nativeRows[i-1];
      assert.equal(row.frame,(previous.frame+1)>>>0,'Native logical frame gap');
      near(row.beforeX,previous.afterX,'Native ledger X discontinuity');near(row.beforeY,previous.afterY,'Native ledger Y discontinuity');
    }
    native.set(row.sequence,row);
  }
  const reference=calibration(evidence,native,movement.keyboardReleasePolicy);
  if(evidence.scenarioSet==='input-ownership')assessTwoFinger(evidence,native,reference,movement);
  else if(mode==='joystick-free')assessJoystick(evidence,native,reference,movement);
  else assessDrags(evidence,native,reference,movement,mode);
}

function assessTwoFinger(e,native,reference,movement){
  assert.equal(e.focusMode,'two-finger');
  const matches=e.scenarios.filter(row=>row.label==='multipointer-primary-owner-two-finger');assert.equal(matches.length,1);
  const s=matches[0],factor=reference.slow.speed/reference.fast.speed;near(s.focusFactor,factor,'Two-finger focus ratio differs from the physical keyboard');
  assert.notEqual(s.primaryId,s.secondaryId);const start=s.startPosition;
  const commands=e.pointerCommands.filter(row=>[s.primaryId,s.secondaryId].includes(row.id));
  const expected=[['down',s.primaryId,.5,.5],['move',s.primaryId,.5,.5+4/480],['down',s.secondaryId,.25,.25],
    ['move',s.secondaryId,.29,.29],['move',s.primaryId,.5,.5+6/480],['up',s.secondaryId,.29,.29],
    ['move',s.primaryId,.5,.5+(2-2*factor)/480],['up',s.primaryId,.5,.5+(2-2*factor)/480]];
  assert.equal(commands.length,expected.length,'Two-finger command sequence is incomplete');
  for(const [index,row] of commands.entries()){
    const [type,id,x,y]=expected[index];assert.equal(row.type,type);assert.equal(row.id,id);near(row.x,x,'Pointer X');near(row.y,y,'Pointer Y');
    assert.equal(row.result?.acknowledged,true,'Runtime did not acknowledge the actual Pointer command');
    assert.ok(Number.isSafeInteger(row.beforeSequence)&&native.has(row.beforeSequence));
    if(index)assert.ok(row.beforeSequence>=commands[index-1].beforeSequence);
  }
  const boundRows=(rows,label,min=3)=>{assert.ok(Array.isArray(rows)&&rows.length>=min,`Missing ${label}`);
    for(const [index,row] of rows.entries()){
      same(row,native.get(row.sequence),`${label}: sample is not in the native ledger`);
      if(index)assert.equal(row.sequence,rows[index-1].sequence+1);
    }return rows;};
  const standing=(rows,point,focus,label,min=3)=>{
    const speed=reference[focus?'slow':'fast'];
    for(const row of boundRows(rows,label,min)){
      assert.equal(row.mode,1);assert.equal(Boolean(row.flags&4),focus);near(row.speed,speed.speed,`${label}: native speed`);
      near(row.scaleX,speed.scale.x,`${label}: X scale`);near(row.scaleY,speed.scale.y,`${label}: Y scale`);
      near(row.targetX,point.x,`${label}: target X`);near(row.targetY,point.y,`${label}: target Y`);
      near(row.afterX,point.x,`${label}: stable X`);near(row.afterY,point.y,`${label}: stable Y`);
      near(row.beforeX,point.x,`${label}: stationary X`);near(row.beforeY,point.y,`${label}: stationary Y`);
    }
  };
  standing(s.primaryDownRows,start,false,'Primary down',1);
  const segments=s.observations;assert.deepEqual(segments.map(row=>row.label),['primary-initial-move','primary-move-with-second-finger','primary-recenter']);
  const shifted=(point,dy,focusFactor=1)=>movement.inputCommand?.coordinate==='float32-normalized-pointer'
    ?mapFloat32TouchDragTarget(point,{x:0,y:dy},{inputSpace:e.inputSpace,sensitivity:1,focusFactor,pixelsPerUnit:movement.inputCommand.pixelsPerUnit})
    :{x:point.x,y:point.y+dy*focusFactor};
  const p1=shifted(start,4),p2=shifted(p1,2,factor);
  for(const [index,target,focused] of [[0,p1,false],[1,p2,true],[2,start,false]]){
    const rows=boundRows(segments[index].rows,segments[index].label),speed=reference[focused?'slow':'fast'];
    assertTouchMotionSamples(rows,{mode:1,target,speed:speed.speed,scale:speed.scale,precision:movement.precision,focused});
  }
  const policy=movement.keyboardReleasePolicy,queued=movement.inputBoundary==='queued-for-next-movement'||!!policy;
  if(queued)assert.equal(policy?.touchFocusTransitions?.semantics,'action-queue-then-next-movement');
  else assert.equal(movement.inputBoundary,'synchronous-consumption');
  for(const [name,point,wasFocused] of [['secondaryDown',p1,false],['secondaryUp',p2,true]]){
    const before=s[name+'InputBefore'],first=s[name+'FirstTick'],rows=boundRows(s[name+'Rows'],name,queued?3:2);
    if(!queued){
      assert.ok(before==null&&first==null,'Synchronous input cannot claim a queued focus transition');
      standing(rows,point,!wasFocused,name+' synchronous focus',2);continue;
    }
    same(first.sample,rows[0],`${name}: first native Tick differs`);same(first.consumedInput,before,`${name}: consumed queue differs`);
    assert.equal(before.traceTotal,rows[0].sequence-1);assert.equal(first.nextMovementInput.traceTotal,rows[0].sequence);
    for(const queue of [before,first.nextMovementInput]){
      assert.equal(queue.observationSource,policy.observationSource);assert.equal(queue.observationPhase,policy.observationPhase);
    }
    assert.equal(Boolean(before.value&policy.focusMask),wasFocused);
    assert.equal(Boolean(first.nextMovementInput.value&policy.focusMask),!wasFocused);
    standing([rows[0]],point,wasFocused,name+' queued Tick',1);standing(rows.slice(1),point,!wasFocused,name+' settled',2);
  }
  // This owner-preservation phase collects two settled ticks; release uses three.
  standing(s.secondaryMoveRows,p1,true,'Secondary move does not own movement',2);
  const released=boundRows(s.releaseRows,'Primary release');
  assertTouchReleaseSamples(released,{position:start,afterSequence:s.releaseBoundarySequence});
  for(const row of released)assert.equal(Boolean(row.flags&4),false,'Released pointers retained focus');
  const boundaries=[s.primaryDownRows[0].sequence,segments[0].rows[0].sequence,s.secondaryDownRows[0].sequence,s.secondaryMoveRows[0].sequence,
    segments[1].rows[0].sequence,s.secondaryUpRows[0].sequence,segments[2].rows[0].sequence,released[0].sequence];
  for(let i=0;i<commands.length;i++)assert.equal(boundaries[i],commands[i].beforeSequence+1,'Pointer command is not adjacent to its native Tick');
}

/** Recompute commands, calibration, native positions and release; ignore stored verdict/oracle fields. */
export function assessRuntimeTouchMotionEvidence({driver,entry,product}={}){
  const definition=definitions[entry?.id];
  if(!definition)return{available:false,valid:false,kind:null};
  try{
    assessMotionCheck(driver,entry,product,definition);
    if(definition.mode===2){
      const target=driver.targets.find(row=>row.game===product.game&&row.variant===product.variant);
      const restored=target.checks.find(row=>row.id==='shared-touch-limited-restored');
      assert.ok(restored&&restored.observation.epoch>entry.observation.epoch,'Unlimited mode needs a later restored limited epoch');
      assert.equal(restored.observation.transition?.pagehide,true,'Previous Runtime must retire before restoring limited movement');
      assessMotionCheck(driver,restored,product,definitions[restored.id]);
    }
    return{available:true,valid:true,kind:'native-touch-motion',checks:definition.checks};
  }catch(error){return{available:true,valid:false,kind:'native-touch-motion',checks:definition.checks,reason:error.message};}
}
