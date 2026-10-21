import assert from 'node:assert/strict';

const SCHEMA='eagler-touhou/touch-deathbomb-scenes/1';
const MAX_PHASE_TICKS=2048;
const MAX_SCENE_TICKS=4096;

class SceneIncomplete extends Error {}

function incomplete(reason,evidence){return{checkStatus:'incomplete',reason,evidence};}
function isFinitePoint(value){return Number.isFinite(value?.x)&&Number.isFinite(value?.y)&&
  value.x>=0&&value.x<=1&&value.y>=0&&value.y<=1;}
function singleBit(value){return Number.isSafeInteger(value)&&value>0&&value<=0x80000000&&Number.isInteger(Math.log2(value));}
function nonemptySource(value){return typeof value==='string'?value.trim().length>0:
  Array.isArray(value)&&value.length>0&&value.every(item=>typeof item==='string'&&item.trim().length>0);}

function validateContract(contract,budgets){
  if(contract?.schema!=='eagler-touhou/stg-deathbomb/1'||contract.wordCount!==36)
    throw new SceneIncomplete('The title has no supported native Deathbomb snapshot contract');
  if(!nonemptySource(contract.source))throw new SceneIncomplete('Deathbomb contract has no production source anchor');
  const states=contract.lifeStates;
  for(const field of ['alive','deathWindow','bombRecovery','respawning'])
    if(!Number.isSafeInteger(states?.[field]))throw new SceneIncomplete(`Deathbomb contract has no lifeStates.${field}`);
  for(const field of ['movementBlocked','movementAllowed'])
    if(!Array.isArray(states?.[field])||!states[field].length||states[field].some(value=>!Number.isSafeInteger(value)))
      throw new SceneIncomplete(`Deathbomb contract has no lifeStates.${field}`);
  if(states.movementBlocked.includes(states.alive)||states.movementAllowed.includes(states.deathWindow)||
     !states.movementAllowed.includes(states.alive)||!states.movementAllowed.includes(states.bombRecovery)||
     states.movementBlocked.some(value=>states.movementAllowed.includes(value)))
    throw new SceneIncomplete('Deathbomb contract life-state movement sets are inconsistent');
  if(!['initial-player-position','declared-position'].includes(contract.respawnPositionRule)||!nonemptySource(contract.respawnPositionSource))
    throw new SceneIncomplete('Declare the original respawn position rule and source');
  if(contract.respawnPositionRule==='declared-position'&&
     ![contract.respawnPosition?.x,contract.respawnPosition?.y].every(Number.isFinite))
    throw new SceneIncomplete('The original respawn position is not finite');
  if(!Array.isArray(contract.deathbombTypes)||!contract.deathbombTypes.length||
     contract.deathbombTypes.some(value=>!Number.isSafeInteger(value))||
     new Set(contract.deathbombTypes).size!==contract.deathbombTypes.length)
    throw new SceneIncomplete('Deathbomb contract has no distinct native Deathbomb types');
  if(!Number.isSafeInteger(contract.bombCost)||contract.bombCost<1)
    throw new SceneIncomplete('Deathbomb contract has no positive Bomb inventory cost');
  if(!Array.isArray(contract.retiringProjectileStates)||!contract.retiringProjectileStates.length||
     contract.retiringProjectileStates.some(value=>!Number.isSafeInteger(value))||!nonemptySource(contract.projectileRetirementSource))
    throw new SceneIncomplete('Declare the production projectile retirement states and their source');
  if(!Number.isSafeInteger(contract.pulseTicks)||contract.pulseTicks<1||contract.pulseTicks>MAX_PHASE_TICKS)
    throw new SceneIncomplete('Deathbomb contract has no bounded input pulse length');
  if(!Number.isSafeInteger(contract.consumerLatencyTicks)||contract.consumerLatencyTicks<0||contract.consumerLatencyTicks>8||
     !nonemptySource(contract.consumerLatencySource))
    throw new SceneIncomplete('Declare the command-to-player consumer latency and its production source');
  if(!singleBit(contract.inputMasks?.fire)||!singleBit(contract.inputMasks?.bomb)||
     contract.inputMasks.fire===contract.inputMasks.bomb)
    throw new SceneIncomplete('Deathbomb contract needs distinct native fire and Bomb input masks');
  const geometry=contract.geometry;
  if(!Number.isSafeInteger(geometry?.pointerId)||geometry.pointerId<0||
     !['start','nudge','blockedMove','recoveryMove'].every(field=>isFinitePoint(geometry[field])))
    throw new SceneIncomplete('Deathbomb contract needs a pointer ID and four normalized movement points');
  if(['start','nudge','blockedMove','recoveryMove'].some((field,index,all)=>
    all.slice(index+1).some(other=>geometry[field].x===geometry[other].x&&geometry[field].y===geometry[other].y)))
    throw new SceneIncomplete('Deathbomb movement points must be distinct');
  if(!Number.isFinite(budgets?.movementEpsilon)||budgets.movementEpsilon<=0)
    throw new SceneIncomplete('Deathbomb scene needs a positive movement tolerance');
  const tickFields=['eligibilityTicks','movementSettleTicks','collisionTicks','deathbombWindowTicks','recoveryTicks','lateDrainTicks'];
  for(const field of tickFields){
    if(!Number.isSafeInteger(budgets?.[field])||budgets[field]<1||budgets[field]>MAX_PHASE_TICKS)
      throw new SceneIncomplete(`Deathbomb scene needs a bounded ${field}`);
  }
  if(budgets.movementSettleTicks<2)throw new SceneIncomplete('Movement settling needs at least two consumer samples');
  if(budgets.lateDrainTicks<contract.pulseTicks)
    throw new SceneIncomplete('Late-input drain budget is shorter than the declared Bomb pulse');
  const total=tickFields.reduce((sum,field)=>sum+budgets[field],0);
  if(total>MAX_SCENE_TICKS)throw new SceneIncomplete(`Deathbomb trial exceeds the ${MAX_SCENE_TICKS}-tick total budget`);
  return{states,geometry,total};
}

function requireState(value,label){
  if(value?.available===false)throw new SceneIncomplete(value.reason??`${label} is unavailable`);
  if(value?.complete!==true)throw new SceneIncomplete(value?.reason??`${label} is incomplete`);
  const required=[
    ['fixture.enabled',value.fixture?.enabled],['fixture.writeLocked',value.fixture?.writeLocked],
    ['fixture.eligible',value.fixture?.eligible],['request.status',value.request?.status],
    ['request.sequence',value.request?.sequence],['projectile.state',value.projectile?.state],
    ['projectile.liveCount',value.projectile?.liveCount],['projectile.nativeState',value.projectile?.nativeState],['collision.source',value.collision?.source],
    ['collision.sequence',value.collision?.sequence],['collision.consumerSequence',value.collision?.consumerSequence],
    ['collision.beforeLife',value.collision?.beforeLife],['collision.afterLife',value.collision?.afterLife],
    ['collision.result',value.collision?.result],['consumer.sequence',value.consumer?.sequence],
    ['consumer.valid',value.consumer?.valid],['life.state',value.life?.state],['life.remainingTicks',value.life?.remainingTicks],
    ['life.deathbomb',value.life?.deathbomb],['bomb.active',value.bomb?.active],['bomb.triggered',value.bomb?.triggered],
    ['bomb.type',value.bomb?.type],['bomb.consumed',value.bomb?.consumed],
    ['inventory.bombs',value.inventory?.bombs],['inventory.lives',value.inventory?.lives],
    ['inventory.deaths',value.inventory?.deaths],['inventory.regularBombs',value.inventory?.regularBombs],
    ['inventory.bombsUsed',value.inventory?.bombsUsed],['position.x',value.position?.x],['position.y',value.position?.y],
  ];
  const floatFields=new Set(['position.x','position.y']);
  const booleanFields=new Set(['fixture.enabled','fixture.writeLocked','fixture.eligible','consumer.valid',
    'life.deathbomb','bomb.active','bomb.triggered']);
  for(const [path,field] of required){
    if(field===undefined||field===null)throw new SceneIncomplete(`${label} is missing ${path}`);
    if(floatFields.has(path)){
      if(typeof field!=='number'||!Number.isFinite(field))throw new SceneIncomplete(`${label} ${path} is not finite`);
    }else if(booleanFields.has(path)){
      if(typeof field!=='boolean')throw new SceneIncomplete(`${label} ${path} is not a native boolean`);
    }else if(typeof field!=='number'||!Number.isSafeInteger(field)){
      throw new SceneIncomplete(`${label} ${path} is not a native integer`);
    }
  }
  return value;
}

function requireActions(value,label){
  if(value?.available===false)throw new SceneIncomplete(value.reason??`${label} is unavailable`);
  if(value?.complete!==true)throw new SceneIncomplete(value?.reason??`${label} is incomplete`);
  if(value.phase!=='consumer-run'||!Number.isSafeInteger(value.sequence)||value.sequence<1||
     !Number.isSafeInteger(value.consumed?.current)||!Number.isSafeInteger(value.lifeState)||
     !value.bomb||!value.inventory||!Number.isSafeInteger(value.inventory.bombsRemaining)||
     !Number.isSafeInteger(value.inventory.bombsUsed)||typeof value.bomb.active!=='boolean'||
     typeof value.bomb.triggered!=='boolean'||!Number.isSafeInteger(value.bomb.type))
    throw new SceneIncomplete(`${label} is not a complete player-consumer observation`);
  return value;
}

function rejected(result){return result===false||result?.accepted===false||result?.acknowledged===false||result?.ok===false;}
function delta(a,b){return Math.hypot(b.x-a.x,b.y-a.y);}

/**
 * Run one bounded real-input Deathbomb trial in the already-running Runtime.
 * The title supplies its state meanings, native bomb costs/types, input masks,
 * source anchors and canvas geometry; the shared scene owns timing and verdicts.
 * `source` selects the Bomb ingress; movement remains on a held direct-touch
 * pointer so both ingress paths exercise the same live movement condition.
 */
export async function runTouchDeathbombScene(io,{contract,source,timing,budgets}={}){
  const evidence={schema:SCHEMA,source,timing,contract:contract?structuredClone(contract):null,
    budgets:budgets?structuredClone(budgets):null,controls:[],touch:[],keys:[],samples:[],bombEvents:[],
    collision:null,movement:{before:null,initial:[],blocked:[],recovered:[]},cleanup:[],status:'incomplete'};
  const missing=['readState','readActions','controls','directTouch','key','requestHit','step']
    .filter(name=>typeof io?.[name]!=='function');
  if(missing.length)return incomplete(`Deathbomb IO is missing ${missing.join(', ')}`,evidence);
  if(!['keyboard','touch'].includes(source)||!['last-window','late'].includes(timing))
    return incomplete('Deathbomb trial requires a supported input source and timing case',evidence);
  let config;
  try{config=validateContract(contract,budgets);}
  catch(error){return incomplete(error.message,evidence);}

  let outcome=null,problem=null,heldPointer=false,keyHeld=false,positionAtPointer={...config.geometry.start};
  let consumerSequence=null,lastState=null,lastSample=null,bombSerial=0,respawnPosition=null;
  const call=async(label,fn)=>{
    try{return await fn();}
    catch(error){
      if(error?.name==='TimeoutError'||error?.code==='ETIMEDOUT'||error?.timeout===true)
        throw new SceneIncomplete(`${label} timed out before the required native observation`);
      throw error;
    }
  };
  const sample=async(label,advance=false)=>{
    if(advance)await call(`${label}-step`,()=>io.step(1));
    const actions=requireActions(await call(`${label}-actions`,()=>io.readActions()),`${label} actions`);
    const state=requireState(await call(`${label}-Deathbomb-state`,()=>io.readState()),`${label} state`);
    if(consumerSequence!==null){
      const next=(consumerSequence+1)>>>0;
      if(actions.sequence!==next)throw new SceneIncomplete(`${label}: player consumer sequence skipped or repeated`);
    }
    if(state.consumer.sequence!==actions.sequence||state.consumer.valid!==true)
      throw new SceneIncomplete(`${label}: Deathbomb and action observations do not identify the same completed player consumer`);
    const collisionAfterPlayer=state.collision.source===1&&state.collision.result===1&&
      state.collision.sequence===1&&state.collision.consumerSequence===actions.sequence&&
      state.collision.beforeLife===actions.lifeState&&state.collision.afterLife===state.life.state&&
      actions.lifeState===config.states.alive&&state.life.state===config.states.deathWindow;
    if(actions.lifeState!==state.life.state&&!collisionAfterPlayer)
      throw new SceneIncomplete(`${label}: independent life-state observations disagree without a tagged post-player collision`);
    if(actions.inventory.bombsRemaining!==state.inventory.bombs||actions.inventory.bombsUsed!==state.inventory.bombsUsed)
      throw new Error(`${label}: independent player/action inventory observations disagree`);
    if(actions.bomb.active!==state.bomb.active||actions.bomb.triggered!==state.bomb.triggered||actions.bomb.type!==state.bomb.type)
      throw new Error(`${label}: independent player/action Bomb observations disagree`);
    consumerSequence=actions.sequence;lastState=state;lastSample={state,actions};
    const row={label,consumerSequence,position:{...state.position},life:{...state.life},fixture:{...state.fixture},
      request:{...state.request},projectile:{...state.projectile},collision:{...state.collision},
      bomb:{...state.bomb},inventory:{...state.inventory},actions:{sequence:actions.sequence,stage:actions.stage,
        current:actions.consumed.current,lifeState:actions.lifeState,bomb:{...actions.bomb},inventory:{...actions.inventory}}};
    evidence.samples.push(row);return{state,actions,row};
  };
  const tick=label=>sample(label,true);
  const controls=async(label,values)=>{
    const payload={fireEnabled:false,focusEnabled:false,bombSerial,escapeSerial:0,joystickX:0,joystickY:0,touchSensitivity:100,...values};
    const result=await call(`${label}-touch-controls`,()=>io.controls(payload));
    evidence.controls.push({label,payload,result:result??null});
    if(rejected(result))throw new SceneIncomplete(`${label}: Runtime did not accept touch controls`);
    bombSerial=payload.bombSerial;return result;
  };
  const pointer=async(type,point,label)=>{
    const result=await call(`${label}-pointer`,()=>io.directTouch(type,config.geometry.pointerId,point.x,point.y));
    evidence.touch.push({label,type,id:config.geometry.pointerId,point:{...point},result:result??null});
    if(rejected(result))throw new SceneIncomplete(`${label}: Runtime did not accept the direct-touch event`);
    if(type==='down'||type==='move')positionAtPointer={...point};
    if(type==='down')heldPointer=true;if(type==='up')heldPointer=false;return result;
  };
  const sendBomb=async()=>{
    if(source==='keyboard'){
      const result=await call('keyboard-Bomb-down',()=>io.key('KeyX',true));
      keyHeld=true;evidence.keys.push({code:'KeyX',down:true,result:result??null});if(rejected(result))throw new SceneIncomplete('Runtime rejected the physical Bomb key-down');
    }else{
      const result=await controls('touch-Bomb-pulse',{bombSerial:(bombSerial+1)>>>0});
      evidence.bombEvents.push({source:'touch-controls-bombSerial',pulseTicks:contract.pulseTicks,serial:bombSerial,result:result??null});
    }
  };
  const releaseKey=async()=>{
    if(!keyHeld)return;const result=await call('keyboard-Bomb-up',()=>io.key('KeyX',false));
    evidence.keys.push({code:'KeyX',down:false,result:result??null});if(rejected(result))throw new SceneIncomplete('Runtime rejected the physical Bomb key-up');keyHeld=false;
  };
  const assertMovementBlocked=sample=>{
    const expected=sample.state.life.state===config.states.respawning?respawnPosition:evidence.movement.before;
    assert.ok(delta(expected,sample.state.position)<=budgets.movementEpsilon,
      'The player moved while life-state blocks movement during the Deathbomb window');
    evidence.movement.blocked.push(sample.row);
  };

  try{
    const initial=await sample('initial-native-baseline');
    respawnPosition=contract.respawnPositionRule==='initial-player-position'?{...initial.state.position}:{...contract.respawnPosition};
    evidence.movement.respawnPosition={...respawnPosition};
    if(!initial.state.fixture.enabled||!initial.state.fixture.writeLocked)
      throw new SceneIncomplete('Deathbomb trial requires the active locked controlled scene');
    if(initial.state.request.sequence!==0||initial.state.request.status!==0||initial.state.projectile.state!==0||
       initial.state.collision.source!==0||initial.state.collision.sequence!==0||initial.state.collision.consumerSequence!==0||
       initial.state.collision.beforeLife!==0||initial.state.collision.afterLife!==0||initial.state.collision.result!==0)
      throw new SceneIncomplete('Deathbomb trial must start in a fresh Runtime epoch');
    if(initial.state.projectile.liveCount!==0)
      throw new SceneIncomplete('Deathbomb fixture must begin without live projectiles');
    if(initial.state.inventory.bombs<contract.bombCost||initial.state.inventory.lives<1)
      throw new SceneIncomplete('Deathbomb trial needs sufficient real Bomb inventory and a life');
    const baselineInventory={...initial.state.inventory};
    let ready=initial;
    if(ready.state.fixture.eligible!==true||ready.state.life.state!==config.states.alive){
      for(let i=0;i<budgets.eligibilityTicks;i++){
        ready=await tick(`natural-eligibility-${i+1}`);
        if(ready.state.fixture.eligible===true&&ready.state.life.state===config.states.alive)break;
      }
    }
    if(ready.state.fixture.eligible!==true||ready.state.life.state!==config.states.alive)
      throw new SceneIncomplete('The native Deathbomb fixture never became eligible within the natural invincibility budget');

    await controls('neutral-actions-before-held-movement',{});
    await pointer('down',config.geometry.start,'movement-pointer-down');
    await pointer('move',config.geometry.nudge,'movement-pointer-nudge');
    const beforeMove=ready.state.position;let sawMovement=false,stable=0,previous=beforeMove,maxBaselineDelta=0;
    let settled=ready;
    for(let i=0;i<budgets.movementSettleTicks;i++){
      settled=await tick(`pre-hit-movement-${i+1}`);
      const amount=delta(previous,settled.state.position);maxBaselineDelta=Math.max(maxBaselineDelta,amount);
      if(delta(beforeMove,settled.state.position)>budgets.movementEpsilon)sawMovement=true;
      stable=amount<=budgets.movementEpsilon?stable+1:0;previous=settled.state.position;
      evidence.movement.initial.push(settled.row);
      assert.equal(settled.state.life.state,config.states.alive,'The player left the active state before the controlled hit');
    }
    if(!sawMovement)throw new SceneIncomplete('The held movement pointer produced no native player movement');
    if(stable<2)throw new SceneIncomplete('The pointer target did not settle before the fixture projectile was requested');
    evidence.movement.before={...settled.state.position};evidence.movement.maximumCalibrationDelta=maxBaselineDelta;
    if(settled.state.fixture.eligible!==true)throw new SceneIncomplete('The fixture became ineligible before the hit stimulus');

    const request=await call('native-hit-request',()=>io.requestHit());
    evidence.hitRequest=request??null;
    if(request?.accepted!==true)throw new Error(`The eligible native hit stimulus was rejected: ${request?.reason??'no acceptance receipt'}`);
    let collided=null;
    for(let i=0;i<budgets.collisionTicks;i++){
      const current=await tick(`real-projectile-collision-${i+1}`);
      if(current.state.projectile.state===3)throw new Error('The accepted real projectile retired without hitting the player');
      if(current.state.collision.sequence>0){collided=current;break;}
    }
    if(!collided)throw new Error('The accepted real projectile did not produce a native player collision within its collision budget');
    assert.equal(collided.state.request.sequence,1,'The fixture request was not recorded as exactly one accepted request');
    assert.equal(collided.state.request.status,1,'The fixture request did not retain its accepted status');
    assert.equal(collided.state.projectile.state,2,'The tracked projectile did not report its real collision state');
    assert.equal(collided.state.collision.source,1,'Collision did not come from the tracked BulletSystem projectile');
    assert.equal(collided.state.collision.sequence,1,'Fresh epoch collision receipt is not unique');
    assert.equal(collided.state.collision.beforeLife,config.states.alive,'Collision began from the wrong native life state');
    assert.equal(collided.state.collision.afterLife,config.states.deathWindow,'PlayerLife did not enter the declared hit state');
    assert.equal(collided.state.collision.result,1,'Native player collision did not return the damage result');
    assert.equal(collided.state.collision.consumerSequence,collided.actions.sequence,
      'The real collision receipt does not point to the player consumer earlier in the same native tick');
    const projectile=collided.state.projectile;
    assert.ok(projectile.liveCount===0||projectile.liveCount===1&&contract.retiringProjectileStates.includes(projectile.nativeState),
      'The tagged collision left an unexpected active projectile instead of its production retirement animation');
    assert.equal(collided.state.life.state,config.states.deathWindow,'The hit did not leave the player in the Deathbomb state');
    assert.ok(collided.state.life.remainingTicks>0,'The native collision did not open a Deathbomb window');
    evidence.collision=collided.row;

    const blockedAt={...collided.state.position};evidence.movement.before=blockedAt;
    await pointer('move',config.geometry.blockedMove,'move-while-deathbomb-blocked');
    let window=collided;
    if(timing==='last-window'){
      for(let i=0;i<budgets.deathbombWindowTicks&&
          !(window.state.life.deathbomb===true&&window.state.life.remainingTicks===1+contract.consumerLatencyTicks);i++){
        window=await tick(`wait-last-deathbomb-tick-${i+1}`);
        if(window.state.life.state!==config.states.deathWindow)throw new Error('The player left the Deathbomb state before the declared last tick');
        assertMovementBlocked(window);
        if(window.state.life.remainingTicks===0)throw new Error('The declared last Deathbomb tick was skipped');
      }
      if(window.state.life.deathbomb!==true||window.state.life.remainingTicks!==1+contract.consumerLatencyTicks)
        throw new SceneIncomplete('The native last Deathbomb tick was not observed within its budget');
    }else{
      for(let i=0;i<budgets.deathbombWindowTicks&&window.state.life.remainingTicks>0;i++){
        window=await tick(`wait-late-deathbomb-window-close-${i+1}`);
        if(window.state.life.state!==config.states.deathWindow)throw new Error('The player left the Deathbomb state before the late input boundary');
        assertMovementBlocked(window);
      }
      if(window.state.life.remainingTicks!==0||window.state.life.deathbomb!==false)
        throw new SceneIncomplete('The natural end of the Deathbomb window was not observed');
    }
    evidence.requestBoundary=window.row;

    await sendBomb();
    for(let n=0;n<contract.consumerLatencyTicks;n++){
      window=await tick(`${timing}-Bomb-queued-before-consumer-${n+1}`);
      assert.equal(Boolean(window.actions.consumed.current&contract.inputMasks.bomb),false,
        'Bomb was consumed before its declared native consumer boundary');
      assert.equal(window.state.inventory.bombsUsed,baselineInventory.bombsUsed,'Queued Bomb activated before its consumer boundary');
      assertMovementBlocked(window);
    }
    evidence.windowBoundary=window.row;
    if(timing==='last-window')assert.equal(window.state.life.remainingTicks,1,'The actual Bomb consumer was not scheduled at the last original window tick');
    const bombSample=await tick(`${timing}-bomb-input-consumer`);
    if(source==='keyboard')await releaseKey();
    const bombWasConsumed=Boolean(bombSample.actions.consumed.current&contract.inputMasks.bomb);
    if(timing==='last-window'){
      assert.equal(bombWasConsumed,true,'The native player consumer did not receive the last-window Bomb input');
      assert.equal(bombSample.state.life.state,config.states.bombRecovery,'The native Bomb pattern did not transition into the declared recovery state');
      assert.equal(bombSample.state.bomb.active,true,'The Deathbomb did not activate its native Bomb pattern');
      assert.equal(bombSample.state.bomb.triggered,true,'The Deathbomb did not report a native trigger');
      assert.ok(contract.deathbombTypes.includes(bombSample.state.bomb.type),
        `Native Bomb type ${bombSample.state.bomb.type} is not a declared Deathbomb type`);
      const expectedCost=Math.min(contract.bombCost,baselineInventory.bombs);
      assert.equal(bombSample.state.bomb.consumed,expectedCost,'Deathbomb consumed the wrong number of Bombs');
      assert.equal(bombSample.state.inventory.bombs,baselineInventory.bombs-expectedCost,'Bomb inventory did not reflect the native Deathbomb cost');
      assert.equal(bombSample.state.inventory.regularBombs,baselineInventory.regularBombs,'Deathbomb was counted as an ordinary Bomb');
      assert.equal(bombSample.state.inventory.bombsUsed,baselineInventory.bombsUsed+1,'Native Bomb-use count did not advance exactly once');
      assert.equal(bombSample.state.inventory.lives,baselineInventory.lives,'A valid last-window Deathbomb cost a life');
      assert.equal(bombSample.state.inventory.deaths,baselineInventory.deaths,'A valid last-window Deathbomb counted as a death');
      if(source==='touch'){
        for(let i=1;i<contract.pulseTicks;i++){
          const pulse=await tick(`deathbomb-touch-pulse-drain-${i}`);
          assert.equal(pulse.state.inventory.bombsUsed,baselineInventory.bombsUsed+1,'One touch pulse triggered multiple Bombs');
          assert.equal(pulse.state.inventory.lives,baselineInventory.lives,'Deathbomb pulse drain changed lives');
          assert.equal(pulse.state.inventory.deaths,baselineInventory.deaths,'Deathbomb pulse drain counted a death');
        }
      }
    }else{
      assert.equal(bombWasConsumed,true,'Late Bomb input did not reach the native player consumer');
      assert.equal(bombSample.state.life.state,config.states.deathWindow,'Late Bomb input unexpectedly rescued the player');
      assert.equal(bombSample.state.bomb.active,false,'Late Bomb input activated a delayed Bomb');
      assert.equal(bombSample.state.bomb.triggered,false,'Late Bomb input retained a delayed trigger');
      assert.equal(bombSample.state.inventory.bombs,baselineInventory.bombs,'Late Bomb input changed the Bomb inventory');
      assert.equal(bombSample.state.inventory.bombsUsed,baselineInventory.bombsUsed,'Late Bomb input counted as a Bomb use');
      assert.equal(bombSample.state.inventory.lives,baselineInventory.lives,'Late Bomb input changed lives before natural death');
      assert.equal(bombSample.state.inventory.deaths,baselineInventory.deaths+1,'Late input was not evaluated at the natural death boundary');
      let lateDrainObserved=1;
      for(let i=1;i<contract.pulseTicks;i++){
        if(lateDrainObserved>=budgets.lateDrainTicks)throw new SceneIncomplete('Late-input pulse exceeded its drain budget');
        const drain=await tick(`late-bomb-pulse-drain-${i}`);
        lateDrainObserved++;
        assert.equal(drain.state.bomb.active,false,'A late Bomb pulse was cached for a later frame');
        assert.equal(drain.state.bomb.triggered,false,'A late Bomb pulse triggered after the Deathbomb window');
        assert.equal(drain.state.inventory.bombs,baselineInventory.bombs,'Late pulse drain changed Bomb inventory');
        assert.equal(drain.state.inventory.bombsUsed,baselineInventory.bombsUsed,'Late pulse drain counted a Bomb');
      }
      let recovery=null;
      for(let i=0;i<budgets.recoveryTicks&&lateDrainObserved<budgets.lateDrainTicks;i++){
        const next=await tick(`late-natural-respawn-${i+1}`);
        lateDrainObserved++;
        if(config.states.movementAllowed.includes(next.state.life.state)){recovery=next;break;}
        if(!config.states.movementBlocked.includes(next.state.life.state))
          throw new SceneIncomplete(`Unexpected life state ${next.state.life.state} during late-input recovery`);
        assertMovementBlocked(next);
        assert.equal(next.state.bomb.active,false,'A late Bomb became active during death/respawn');
        assert.equal(next.state.bomb.triggered,false,'A late Bomb triggered during death/respawn');
      }
      if(!recovery)throw new SceneIncomplete('The late-input trial did not reach a movable native recovery state within its bounded recovery/drain budgets');
      assert.ok(recovery.state.inventory.deaths>baselineInventory.deaths,'The late-input trial did not record the natural death');
      assert.ok(recovery.state.inventory.lives<baselineInventory.lives,'The late-input trial did not cross the life-loss boundary');
      assert.equal(recovery.state.bomb.active,false,'A late Bomb was replayed after respawn');
      assert.equal(recovery.state.bomb.triggered,false,'A late Bomb triggered after respawn');
      evidence.recovery=recovery.row;
    }

    // Input is sampled before player simulation. Give the owner one real poll
    // of the restored life state before submitting the next physical delta.
    const recovered=await tick('post-recovery-input-owner-sample');
    if(!config.states.movementAllowed.includes(recovered.state.life.state))
      throw new SceneIncomplete('Native player has not reached a movement-enabled state after the Bomb trial');
    const beforeRecoveryMove={...recovered.state.position};
    await pointer('move',config.geometry.recoveryMove,'held-primary-recovery-move');
    let moved=false,prior=beforeRecoveryMove;
    for(let i=0;i<budgets.movementSettleTicks;i++){
      const next=await tick(`recovered-movement-${i+1}`),movement=delta(prior,next.state.position);
      if(movement>maxBaselineDelta+budgets.movementEpsilon)
        throw new Error('Movement jumped beyond the measured pre-hit per-tick envelope after recovery');
      if(timing==='late'){
        assert.equal(next.state.bomb.active,false,'A late Bomb was activated after the first movable respawn sample');
        assert.equal(next.state.bomb.triggered,false,'A late Bomb trigger reappeared after the first movable respawn sample');
        assert.equal(next.state.inventory.bombs,baselineInventory.bombs,'A late Bomb changed inventory after respawn');
        assert.equal(next.state.inventory.bombsUsed,baselineInventory.bombsUsed,'A late Bomb was counted after respawn');
        assert.equal(next.state.inventory.deaths,baselineInventory.deaths+1,'Late input changed the natural death count after respawn');
        assert.equal(next.state.inventory.lives,baselineInventory.lives-1,'Late input changed the natural life count after respawn');
      }
      if(delta(beforeRecoveryMove,next.state.position)>budgets.movementEpsilon)moved=true;
      evidence.movement.recovered.push(next.row);prior=next.state.position;
    }
    if(!moved)throw new Error('The held primary movement pointer did not resume movement after recovery');
    assert.equal(lastState.projectile.liveCount,0,'The tagged projectile did not retire within the completed window and recovery scene');
    evidence.projectileRetirement={atCollision:{...collided.state.projectile},afterRecovery:{...lastState.projectile}};
    if(timing==='last-window'){
      assert.equal(lastState.inventory.bombs,baselineInventory.bombs-Math.min(contract.bombCost,baselineInventory.bombs));
      assert.equal(lastState.inventory.bombsUsed,baselineInventory.bombsUsed+1);
      assert.equal(lastState.inventory.lives,baselineInventory.lives);
      assert.equal(lastState.inventory.deaths,baselineInventory.deaths);
    }
    outcome={checkStatus:'passed',complete:true,scope:`One real hit | ${source} Bomb ingress | ${timing} window | held primary movement`};
  }catch(error){
    problem=error;
    evidence.failure={name:error?.name??'Error',message:String(error?.message??error),code:error?.code??null};
    outcome=error instanceof SceneIncomplete?incomplete(error.message,evidence):
      {checkStatus:'failed',reason:error.message,evidence};
  }finally{
    const cleanup=async(label,fn)=>{try{const result=await fn();evidence.cleanup.push({label,ok:!rejected(result),result:result??null});
      if(rejected(result))throw new Error(`${label} was rejected`);}catch(error){evidence.cleanup.push({label,ok:false,error:String(error?.message??error)});}};
    if(keyHeld)await cleanup('Bomb-key-up',()=>io.key('KeyX',false));
    if(heldPointer)await cleanup('movement-pointer-up',()=>io.directTouch('up',config.geometry.pointerId,positionAtPointer.x,positionAtPointer.y));
    await cleanup('neutral-controls',()=>io.controls({fireEnabled:false,focusEnabled:false,bombSerial,escapeSerial:0,joystickX:0,joystickY:0,touchSensitivity:100}));
  }
  if(outcome?.checkStatus==='passed'&&evidence.cleanup.some(row=>!row.ok))
    {evidence.failure={name:'CleanupError',message:'Deathbomb trial passed its gameplay checks but could not release every input owner'};
      outcome=incomplete(evidence.failure.message,evidence);}
  evidence.status=outcome.checkStatus;
  if(problem&&outcome?.checkStatus!=='passed')return outcome;
  return{...outcome,evidence};
}
