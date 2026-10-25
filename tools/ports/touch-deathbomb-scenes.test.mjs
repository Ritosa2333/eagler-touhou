import assert from 'node:assert/strict';
import test from 'node:test';
import {runTouchDeathbombScene,assertTouchDeathbombEvidence} from './touch-deathbomb-scenes.mjs';

const contract={schema:'eagler-touhou/stg-deathbomb/1',wordCount:36,
  source:['PlayerBomb.cpp | update_player_bomb','PlayerLife.cpp | resolve_death','PlayerFrame.cpp | update_player_frame'],
  lifeStates:{alive:0,deathWindow:2,bombRecovery:3,respawning:1,movementBlocked:[1,2],movementAllowed:[0,3]},
  respawnPositionRule:'initial-player-position',respawnPositionSource:'Initial-position respawn component fixture',
  deathbombTypes:[2,3],bombCost:2,pulseTicks:3,inputMasks:{fire:1,bomb:2},
  retiringProjectileStates:[5],projectileRetirementSource:'Production-style cancellation animation component fixture',
  consumerLatencyTicks:0,consumerLatencySource:'Direct-consumption component fixture',
  geometry:{pointerId:9467,start:{x:.5,y:.75},nudge:{x:.505,y:.75},blockedMove:{x:.51,y:.75},recoveryMove:{x:.515,y:.74}}};
const budgets={eligibilityTicks:8,movementSettleTicks:3,collisionTicks:4,deathbombWindowTicks:8,
  recoveryTicks:72,lateDrainTicks:120,movementEpsilon:1e-4};

function makeIO({collision=true,rescue=true,lateCache=false,skipSequence=false,inventoryMismatch=false,commandLatencyTicks=0,wrongRetirement=false}={}){
  let sequence=1,fire=false,keyHeld=false,pointerDown=false,pointer={...contract.geometry.start};
  let position={...contract.geometry.start},requestPending=false,requestSequence=0,requestStatus=0;
  let projectileState=0,collisionSequence=0,collisionSource=0,collisionConsumerSequence=0;
  let collisionBefore=0,collisionAfter=0,collisionResult=0;
  let lifeState=0,lifeTimer=0,remaining=0,deathbomb=false,bombs=3,lives=2,deaths=0,bombsUsed=0;
  let bombActive=false,bombTriggered=false,bombType=0,bombConsumed=0,regularBombs=0;
  let pulseRemaining=0,seenBombSerial=0,bombSerial=0,lastBombTrigger=false,hitProcessed=false;
  let lateBombPending=false,lateRecoveryAge=0;
  let movementBlocked=false;
  const commandQueue=[];let consumedBomb=false;
  let consumerLifeState=0;
  let retirementAge=0;
  const controls={fireEnabled:false,bombSerial:0};
  const actions=()=>({available:true,complete:true,phase:'consumer-run',sequence,stage:1,
    consumed:{current:(controls.fireEnabled?1:0)|(consumedBomb?2:0),previous:0},
    lifeState:consumerLifeState,bomb:{active:bombActive,triggered:bombTriggered,type:bombType,timer:0},
    inventory:{bombsRemaining:bombs+(inventoryMismatch?1:0),bombsUsed}});
  const state=()=>({available:true,complete:true,fixture:{enabled:true,writeLocked:true,eligible:!controls.fireEnabled&&lifeState===0&&!hitProcessed},
    request:{status:requestStatus,sequence:requestSequence},projectile:{state:projectileState,
      liveCount:collisionSequence&&retirementAge<3?1:0,nativeState:collisionSequence&&retirementAge<3?(wrongRetirement?1:5):0},
    collision:{source:collisionSource,sequence:collisionSequence,consumerSequence:collisionConsumerSequence,
      beforeLife:collisionBefore,afterLife:collisionAfter,result:collisionResult},
    consumer:{sequence,valid:true},life:{state:lifeState,timer:lifeTimer,remainingTicks:remaining,deathbomb},
    bomb:{cooldown:0,active:bombActive,type:bombType,triggered:bombTriggered,consumed:bombConsumed},
    inventory:{bombs,regularBombs,lives,deaths,bombsUsed,bombsUsedStage:bombsUsed},position:{...position}});
  const moveToward=()=>{
    if(!pointerDown||movementBlocked)return;
    const dx=pointer.x-position.x,dy=pointer.y-position.y,distance=Math.hypot(dx,dy);
    if(distance>0){const amount=Math.min(.005,distance);position={x:position.x+dx/distance*amount,y:position.y+dy/distance*amount};}
  };
  return{
    async readState(){return state();},
    async readActions(){return actions();},
    async controls(value){
      assert.equal(value.fireEnabled,false,'The isolated projectile fixture must keep player shots out of its world');
      Object.assign(controls,value);fire=Boolean(controls.fireEnabled);bombSerial=controls.bombSerial;
      if(bombSerial>seenBombSerial){seenBombSerial=bombSerial;pulseRemaining=contract.pulseTicks;}
      return{accepted:true};
    },
    async directTouch(type,id,x,y){
      assert.equal(id,contract.geometry.pointerId);pointer={x,y};
      if(type==='down')pointerDown=true;
      if(type==='up')pointerDown=false;
      return{accepted:true};
    },
    async key(code,down){assert.equal(code,'KeyX');keyHeld=down;return{accepted:true};},
    async requestHit(){requestSequence++;requestStatus=1;requestPending=true;return{accepted:true,sequence:requestSequence};},
    async step(count){
      assert.equal(count,1);sequence++;
      commandQueue.push(keyHeld||pulseRemaining>0);
      const bombPressed=commandQueue.length>commandLatencyTicks?commandQueue.shift():false;
      consumedBomb=bombPressed;
      const beforeLife=lifeState;let taggedCollisionThisStep=false;
      lastBombTrigger=false;
      if(requestPending&&!hitProcessed){
        requestPending=false;hitProcessed=true;
        if(collision){
          taggedCollisionThisStep=true;
          projectileState=2;collisionSequence=1;collisionSource=1;collisionConsumerSequence=sequence;
          collisionBefore=0;collisionAfter=2;collisionResult=1;lifeState=2;remaining=4;deathbomb=true;lifeTimer=0;
        }else projectileState=3;
      }else if(lifeState===2){
        if(remaining>0&&bombPressed&&rescue){
          lifeState=3;remaining=0;deathbomb=false;bombActive=true;bombTriggered=true;lastBombTrigger=true;
          bombType=3;bombConsumed=2;bombs-=2;bombsUsed++;
        }else if(remaining>0){remaining--;deathbomb=remaining>0;if(remaining===0)deaths++;}
        else{
          if(lateCache&&bombPressed)lateBombPending=true;
          deathbomb=false;lifeTimer++;
          if(lifeTimer>=30){lifeState=1;lifeTimer=0;lives--;bombs=3;position={...contract.geometry.start};}
        }
      }else if(lifeState===1){
        lifeTimer++;
        if(lifeTimer>=30){lifeState=3;lifeTimer=240;remaining=15;lateRecoveryAge=0;}
      }
      if(lateCache&&lateBombPending&&lifeState===3){
        lateRecoveryAge++;
        if(lateRecoveryAge===2){lateBombPending=false;bombActive=true;bombTriggered=true;bombType=3;bombConsumed=2;bombs-=2;bombsUsed++;}
      }
      movementBlocked=lifeState===2||lifeState===1;
      moveToward();
      if(pulseRemaining>0)pulseRemaining--;
      if(skipSequence&&sequence>3)sequence++;
      consumerLifeState=taggedCollisionThisStep?beforeLife:lifeState;
      if(collisionSequence)retirementAge++;
    },
    async closeState(){return state();},
  };
}

test('last-window keyboard and touch trials prove a real hit, Bomb consumer, survival, and resumed movement',async()=>{
  for(const source of ['keyboard','touch']){
    const result=await runTouchDeathbombScene(makeIO(),{contract,source,timing:'last-window',budgets});
    assert.equal(result.checkStatus,'passed',`${source}: ${result.reason??''}`);
    assert.equal(result.complete,true);
    assert.equal(result.evidence.collision.collision.source,1);
    assert.equal(result.evidence.collision.collision.afterLife,contract.lifeStates.deathWindow);
    assert.equal(result.evidence.windowBoundary.life.remainingTicks,1);
    assert.equal(result.evidence.bombEvents.length,source==='touch'?1:0);
    assert.ok(result.evidence.movement.blocked.length>0);
    assert.ok(result.evidence.movement.recovered.length>0);
    assertTouchDeathbombEvidence(JSON.parse(JSON.stringify(result.evidence)),{contract});
  }
});

test('a queued command is scheduled early while the actual Bomb is consumed on the last native window tick',async()=>{
  for(const source of ['keyboard','touch']){
    const result=await runTouchDeathbombScene(makeIO({commandLatencyTicks:1}),{
      contract:{...contract,consumerLatencyTicks:1},source,timing:'last-window',budgets});
    assert.equal(result.checkStatus,'passed',result.reason);
    assert.equal(result.evidence.requestBoundary.life.remainingTicks,2);
    assertTouchDeathbombEvidence(JSON.parse(JSON.stringify(result.evidence)),{contract:{...contract,consumerLatencyTicks:1}});
    assert.equal(result.evidence.windowBoundary.life.remainingTicks,1);
  }
});

test('a hit projectile follows its real retirement animation while an active damaging projectile fails',async()=>{
  const passed=await runTouchDeathbombScene(makeIO(),{contract,source:'touch',timing:'last-window',budgets});
  assert.equal(passed.checkStatus,'passed',passed.reason);
  assert.equal(passed.evidence.projectileRetirement.atCollision.nativeState,5);
  assert.equal(passed.evidence.projectileRetirement.afterRecovery.liveCount,0);
  const failed=await runTouchDeathbombScene(makeIO({wrongRetirement:true}),{contract,source:'touch',timing:'last-window',budgets});
  assert.equal(failed.checkStatus,'failed');assert.match(failed.reason,/unexpected active projectile/);
});

test('late input is consumed outside the window but cannot activate or survive into respawn',async()=>{
  const result=await runTouchDeathbombScene(makeIO(),{contract,source:'touch',timing:'late',budgets});
  assert.equal(result.checkStatus,'passed',result.reason??'');
  assert.equal(result.evidence.windowBoundary.life.remainingTicks,0);
  assert.equal(result.evidence.recovery.life.state,contract.lifeStates.bombRecovery);
  assert.equal(result.evidence.recovery.inventory.deaths,1);
  assert.equal(result.evidence.recovery.inventory.lives,1);
  assert.ok(result.evidence.movement.blocked.length>0);
  assertTouchDeathbombEvidence(JSON.parse(JSON.stringify(result.evidence)),{contract});
});

for(const [name,mutate] of [
  ['no actual collision',e=>e.collision.collision.source=0],
  ['wrong input boundary',e=>e.requestBoundary.life.remainingTicks=3],
  ['duplicate consumer',e=>e.samples[2].consumerSequence=e.samples[1].consumerSequence],
  ['wrong native cost',e=>e.samples.find(row=>row.label==='last-window-bomb-input-consumer').bomb.consumed=1],
  ['life loss after rescue',e=>e.samples.at(-1).inventory.lives--],
  ['recovery teleport',e=>e.movement.recovered[0].position.x+=10],
  ['missing held pointer',e=>e.touch=e.touch.filter(row=>row.type!=='down')],
  ['pointer released before recovery',e=>e.touch.splice(2,0,{...e.touch[0],type:'up'})],
  ['missing input acknowledgement',e=>e.touch[0].result=null],
  ['missing cleanup acknowledgement',e=>e.cleanup[0].result=null],
  ['input cleanup failed',e=>e.cleanup[0].ok=false],
  ['unretired projectile',e=>e.samples.at(-1).projectile.liveCount=1],
])test(`saved Deathbomb evidence rejects ${name} despite stored passing verdict`,async()=>{
  const result=await runTouchDeathbombScene(makeIO(),{contract,source:'touch',timing:'last-window',budgets});
  assert.equal(result.checkStatus,'passed');const evidence=JSON.parse(JSON.stringify(result.evidence));mutate(evidence);
  assert.throws(()=>assertTouchDeathbombEvidence(evidence,{contract}));
});

test('saved late-input evidence rejects a Bomb cached until respawn',async()=>{
  const result=await runTouchDeathbombScene(makeIO(),{contract,source:'touch',timing:'late',budgets});
  assert.equal(result.checkStatus,'passed');const evidence=JSON.parse(JSON.stringify(result.evidence));
  evidence.samples.at(-1).bomb.active=true;evidence.samples.at(-1).actions.bomb.active=true;
  assert.throws(()=>assertTouchDeathbombEvidence(evidence,{contract}),/Late Bomb/);
});

test('an accepted button without a real collision cannot pass',async()=>{
  const result=await runTouchDeathbombScene(makeIO({collision:false}),{contract,source:'keyboard',timing:'last-window',budgets});
  assert.equal(result.checkStatus,'failed');
  assert.match(result.reason,/retired without hitting|did not produce a native player collision/);
  assert.equal(result.evidence.collision,null);
});

test('a Bomb that does not rescue or whose independent inventory views disagree fails',async()=>{
  const noRescue=await runTouchDeathbombScene(makeIO({rescue:false}),{contract,source:'keyboard',timing:'last-window',budgets});
  assert.equal(noRescue.checkStatus,'failed');
  assert.match(noRescue.reason,/did not transition|did not activate/);
  const mismatch=await runTouchDeathbombScene(makeIO({inventoryMismatch:true}),{contract,source:'touch',timing:'last-window',budgets});
  assert.equal(mismatch.checkStatus,'failed');
  assert.match(mismatch.reason,/inventory observations disagree/);
});

test('a late touch Bomb cached until after the first movable respawn sample fails',async()=>{
  const result=await runTouchDeathbombScene(makeIO({lateCache:true}),{contract,source:'touch',timing:'late',budgets});
  assert.equal(result.checkStatus,'failed');
  assert.match(result.reason,/activated after the first movable respawn sample/);
  assert.equal(result.evidence.recovery.life.state,contract.lifeStates.bombRecovery);
});

test('missing native input consumer data remains incomplete and does not invoke controls',async()=>{
  const io=makeIO();let controls=0;
  io.readActions=async()=>({available:false,reason:'no player consumer receipt'});
  const original=io.controls;io.controls=async(...args)=>{controls++;return original(...args);};
  const result=await runTouchDeathbombScene(io,{contract,source:'touch',timing:'late',budgets});
  assert.equal(result.checkStatus,'incomplete');
  assert.match(result.reason,/no player consumer receipt/);
  assert.equal(result.evidence.controls.length,0,'No gameplay controls should be sent without an initial consumer receipt');
  assert.equal(controls,1,'The only control call should be final neutral cleanup');
});

test('wrong boolean or skipped consumer sequences cannot be treated as observations',async()=>{
  const invalidBool=makeIO();invalidBool.readState=async()=>({...await makeIO().readState(),fixture:{enabled:1,writeLocked:true,eligible:true}});
  const invalidResult=await runTouchDeathbombScene(invalidBool,{contract,source:'touch',timing:'late',budgets});
  assert.equal(invalidResult.checkStatus,'incomplete');
  assert.match(invalidResult.reason,/fixture.enabled is not a native boolean/);

  const gap=makeIO({skipSequence:true});
  const gapResult=await runTouchDeathbombScene(gap,{contract,source:'keyboard',timing:'late',budgets});
  assert.equal(gapResult.checkStatus,'incomplete');
  assert.match(gapResult.reason,/sequence skipped or repeated/);
});
