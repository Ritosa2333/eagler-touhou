import test from 'node:test';
import assert from 'node:assert/strict';
import {assertTouchMotionSceneEvidence} from './touch-motion-evidence.mjs';

const movement={precision:{unitsPerPixel:1,rounding:'float',scalePlacement:'before-limit'}};

const twoFingerPolicy={semantics:'queued-for-next-movement',observationSource:'fixture.native-input',observationPhase:'post-update',
  directionMasks:{ArrowRight:128,ArrowLeft:64},focusMask:4,touchFocusTransitions:{semantics:'action-queue-then-next-movement',maxMovementTicks:1}};
function twoFingerFixture(){
  const e=fixture();e.nativeRows=e.nativeRows.slice(0,12);e.scenarios=[];e.pointerCommands=[];e.scenarioSet='input-ownership';e.focusMode='two-finger';
  for(const value of Object.values(e.calibration)){
    const down=e.keyboardTransitions.find(row=>row.acknowledgement.nativeTraceTotal===value.startSequence);
    down.acknowledgement.nextMovementInputBoundary={semantics:twoFingerPolicy.semantics,observationSource:twoFingerPolicy.observationSource,
      observationPhase:twoFingerPolicy.observationPhase,traceTotal:value.startSequence};
    value.releaseAcknowledgements=[{acknowledged:true,changed:true,stableTraceBoundary:true,nextMovementInputBoundary:{traceTotal:value.rows.at(-1).sequence,
      value:128|(value===e.calibration.slow?4:0)}}];
  }
  let sequence=12,{afterX:x,afterY:y}=e.nativeRows.at(-1);const start={x,y};
  const tick=(target,focused=false,mode=1)=>{
    const speed=focused?2:4,dy=Math.max(-speed,Math.min(speed,target.y-y));
    const row={sequence:++sequence,frame:sequence-1,abiVersion:2,localSeat:0,executionKind:0,mode,flags:focused?5:1,
      beforeX:x,beforeY:y,targetX:target.x,targetY:target.y,afterX:x,afterY:y+dy,speed,scaleX:1,scaleY:1,sensitivity:1};
    e.nativeRows.push(row);y=row.afterY;return row;
  };
  const command=(type,id,px,py)=>e.pointerCommands.push({type,id,x:px,y:py,beforeSequence:sequence,result:{acknowledged:true}});
  const rows=(point,focus=false,n=3,mode=1)=>Array.from({length:n},()=>tick(point,focus,mode));
  const s={label:'multipointer-primary-owner-two-finger',focusMode:'two-finger',focusFactor:.5,primaryId:701,secondaryId:702,startPosition:start,observations:[]};e.scenarios.push(s);
  command('down',701,.5,.5);s.primaryDownRows=rows(start,false,1);
  const p1={x,y:start.y+4},p2={x,y:start.y+5};
  command('move',701,.5,.5+4/480);s.observations.push({label:'primary-initial-move',rows:rows(p1)});
  const transition=(name,point,focus)=>{
    const before={traceTotal:sequence,value:focus?4:0,observationSource:twoFingerPolicy.observationSource,observationPhase:twoFingerPolicy.observationPhase};
    s[name+'InputBefore']=before;const first=tick(point,focus);
    s[name+'FirstTick']={sample:first,consumedInput:before,nextMovementInput:{...before,traceTotal:sequence,value:focus?0:4}};
    s[name+'Rows']=[first,...rows(point,!focus,2)];
  };
  command('down',702,.25,.25);transition('secondaryDown',p1,false);
  command('move',702,.29,.29);s.secondaryMoveRows=rows(p1,true,2);
  command('move',701,.5,.5+6/480);s.observations.push({label:'primary-move-with-second-finger',rows:rows(p2,true)});
  command('up',702,.29,.29);transition('secondaryUp',p2,true);
  command('move',701,.5,.5+1/480);s.observations.push({label:'primary-recenter',rows:rows(start)});
  s.releaseBoundarySequence=sequence;command('up',701,.5,.5+1/480);s.releaseRows=rows(start,false,3,0);
  e.trace.finalTotal=sequence;return e;
}

test('two-finger proof independently checks primary ownership, queued focus, actual movement and both releases',()=>{
  const e=twoFingerFixture();assertTouchMotionSceneEvidence(e,{...movement,keyboardReleasePolicy:twoFingerPolicy},1);
  for(const mutate of [
    value=>value.pointerCommands[3].id=701,
    value=>value.pointerCommands[2].result.acknowledged=false,
    value=>value.scenarios[0].secondaryMoveRows[1].afterX+=1,
    value=>value.scenarios[0].secondaryMoveRows.pop(),
    value=>value.scenarios[0].secondaryDownFirstTick.nextMovementInput.value=0,
    value=>value.scenarios[0].secondaryUpRows[1].flags=5,
    value=>value.scenarios[0].observations[1].rows[0].speed=4,
    value=>value.scenarios[0].releaseRows[0].afterY+=1,
  ]){const invalid=structuredClone(e);mutate(invalid);assert.throws(()=>assertTouchMotionSceneEvidence(invalid,{...movement,keyboardReleasePolicy:twoFingerPolicy},1));}
});

test('the same two-finger proof accepts declared synchronous consumption without inventing a queued transition',()=>{
  const e=twoFingerFixture(),s=e.scenarios[0];
  for(const [name,focus] of [['secondaryDown',true],['secondaryUp',false]]){
    delete s[name+'InputBefore'];delete s[name+'FirstTick'];
    for(const row of s[name+'Rows']){row.flags=focus?5:1;row.speed=focus?2:4;}
  }
  const synchronous={...movement,inputBoundary:'synchronous-consumption'};
  assertTouchMotionSceneEvidence(e,synchronous,1);
  const invalid=structuredClone(e);invalid.scenarios[0].secondaryDownRows[0].flags=1;
  assert.throws(()=>assertTouchMotionSceneEvidence(invalid,synchronous,1));
});

// Analytic 3-4-5 vectors: micro length .5 and diagonal length 10. The fixture
// supplies raw observations, never calls the checker/predictor to produce them.
function fixture(mode=1){
  const e={nativeRows:[],keyboardTransitions:[],calibration:{},scenarios:[],
    trace:{available:true,abiVersion:2,localSeat:0,initialTotal:0,finalTotal:0}};
  let x=100,y=100,sequence=0;
  const tick=({dx=0,dy=0,targetX=x,targetY=y,speed=4,focused=false,nativeMode=0,sensitivity=1}={})=>{
    const row={sequence:++sequence,frame:sequence-1,abiVersion:2,localSeat:0,executionKind:0,
      mode:nativeMode,flags:focused?5:1,beforeX:x,beforeY:y,targetX,targetY,afterX:x+dx,afterY:y+dy,
      speed,scaleX:1,scaleY:1,sensitivity};
    e.nativeRows.push(row);x=row.afterX;y=row.afterY;return row;
  };
  for(const [label,speed,focused] of [['fast',4,false],['slow',2,true]]){
    const startSequence=sequence;
    e.keyboardTransitions.push({code:'ArrowRight',down:true,acknowledgement:{acknowledged:true,changed:true,stableTraceBoundary:true,nativeTraceTotal:startSequence}});
    const rows=Array.from({length:3},()=>tick({dx:speed,speed,focused}));
    const neutralRows=Array.from({length:3},()=>tick({speed}));
    e.calibration[label]={direction:'ArrowRight',startSequence,rows,neutralRows,speed,scale:{x:1,y:1}};
  }
  const drag=(label,sensitivityPercent,focused,kind)=>{
    const reverse=kind==='diagonal-reverse-cancel',micro=kind==='micro-up',sign=reverse?-1:1;
    const requestedDelta={x:sign*(micro?.3:6),y:sign*(micro?.4:8)};
    const scale=sensitivityPercent/100*(mode===1&&focused?.5:1),speed=focused?2:4;
    const startPosition={x,y},target={x:x+requestedDelta.x*scale,y:y+requestedDelta.y*scale};
    const downRows=[tick({nativeMode:mode,speed,focused,sensitivity:sensitivityPercent/100})];
    let remaining=(micro?.5:10)*scale;
    const movementRows=[];
    do{
      const travel=mode===2?remaining:Math.min(remaining,speed);
      movementRows.push(tick({dx:sign*.6*travel,dy:sign*.8*travel,targetX:target.x,targetY:target.y,
        speed,focused,nativeMode:mode,sensitivity:sensitivityPercent/100}));
      remaining=Math.max(0,remaining-travel);
    }while(remaining>1e-10||movementRows.length<3);
    const releaseBoundarySequence=sequence;
    const releaseRows=Array.from({length:3},(_,index)=>tick({speed,focused:focused&&(!reverse||index===0),sensitivity:sensitivityPercent/100}));
    e.scenarios.push({label,sensitivityPercent,focused,release:reverse?'cancel':'up',requestedDelta,startPosition,
      pointerOrigin:{x:.5,y:.5},pointerDestination:{x:.5+requestedDelta.x/640,y:.5+requestedDelta.y/480},
      downRows,movementRows,releaseRows,releaseBoundarySequence,
      expectedTarget:{x:-999,y:-999},oracle:{passed:true}});
  };
  for(const sensitivity of [100,200,300])for(const kind of ['micro-up','diagonal-up','diagonal-reverse-cancel'])
    drag(`sensitivity-${sensitivity}-${kind}`,sensitivity,false,kind);
  if(mode===1){
    for(const sensitivity of [100,200,300])for(const kind of ['micro-up','diagonal-up','diagonal-reverse-cancel'])
      drag(`focused-sensitivity-${sensitivity}-${kind}`,sensitivity,true,kind);
    for(const kind of ['diagonal-up','diagonal-reverse-cancel'])drag(`focus-restored-fast-${kind}`,100,false,kind);
  }
  e.trace.finalTotal=sequence;
  return e;
}

test('full limited and unlimited matrices recompute raw ticks instead of stored oracle fields',()=>{
  for(const mode of [1,2])assert.doesNotThrow(()=>assertTouchMotionSceneEvidence(fixture(mode),movement,mode));
});

const corruptions={
  'speed above original keyboard cap':e=>{const r=e.scenarios[1].movementRows[0];r.afterX+=1;},
  'micro movement snapped to a direction':e=>{const r=e.scenarios[0].movementRows[0];r.afterY=r.beforeY;},
  'missing sensitivity case':e=>{e.scenarios=e.scenarios.filter(row=>!row.label.startsWith('sensitivity-300'));},
  'missing focused sensitivity':e=>{e.scenarios=e.scenarios.filter(row=>!row.label.startsWith('focused-sensitivity-200'));},
  'forged target and oracle':e=>{const r=e.scenarios[1];for(const tick of r.movementRows)tick.targetX+=1;r.expectedTarget.x+=1;},
  'altered pointer request':e=>{e.scenarios[1].pointerDestination.x+=.01;},
  'fake independent keyboard speed':e=>{e.calibration.fast.speed=5;},
  'missing keyboard press':e=>{e.keyboardTransitions=[];},
  'missing native tick':e=>{e.nativeRows.splice(6,1);},
  'duplicated native tick':e=>{e.nativeRows[7].sequence=e.nativeRows[6].sequence;},
  'foreign seat stream':e=>{e.nativeRows[0].localSeat=1;},
  'scene detached from native ledger':e=>{e.scenarios[0].movementRows=structuredClone(e.scenarios[0].movementRows);e.scenarios[0].movementRows[0].frame++;},
  'sticky pointer release':e=>{e.scenarios[0].releaseRows[0].mode=1;},
  'late movement after release':e=>{e.scenarios[0].releaseRows[2].afterX+=1;},
  'missing reverse cancellation':e=>{e.scenarios=e.scenarios.filter(row=>!row.label.endsWith('reverse-cancel'));},
  'missing focus restore':e=>{e.scenarios=e.scenarios.filter(row=>!row.label.startsWith('focus-restored-fast'));},
};
for(const [name,mutate] of Object.entries(corruptions))test(`motion evidence rejects ${name}`,()=>{
  const evidence=fixture();mutate(evidence);
  assert.throws(()=>assertTouchMotionSceneEvidence(evidence,movement,1));
});
