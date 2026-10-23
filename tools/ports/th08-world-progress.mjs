const SCHEMA='eagler-touhou/th08-world-progress/1';

function finiteCounter(value){return Number.isSafeInteger(value)&&value>=0;}
function finiteCoordinate(value){return typeof value==='number'&&Number.isFinite(value);}

function read(snapshot,seat,label){
  const observation=snapshot?.observation,world=observation?.worldProgress;
  const errors=[];
  if(observation?.game!=='th08'||observation?.variant!=='multiplayer')
    errors.push(`${label}: expected a TH08 multiplayer observation`);
  if(world?.schema!==SCHEMA)errors.push(`${label}: expected ${SCHEMA} world observation`);
  if(!Number.isSafeInteger(seat)||seat<0||seat>2)errors.push(`${label}: invalid local seat ${seat}`);
  if(errors.length)return{world:null,errors};
  const {counters,state,localPlayer,netplay}=world;
  if(!counters||!state||!localPlayer||!netplay||!netplay.frontier)
    errors.push(`${label}: world observation is missing counters, state, local player, or network frontier`);
  if(errors.length)return{world,errors};
  if(localPlayer.seat!==seat)errors.push(`${label}: observation belongs to seat ${localPlayer.seat}, expected ${seat}`);
  for(const key of ['stage','enemySceneFrame','controlTick','lastCompletedNetworkFrame','confirmedNetworkFrame','nextNetworkFrame'])
    if(!finiteCounter(counters[key]))errors.push(`${label}: counters.${key} is unavailable`);
  for(const key of ['loading','paused','retrying','gameOver','dialogue','inTransition'])
    if(typeof state[key]!=='boolean')errors.push(`${label}: state.${key} is unavailable`);
  if(!finiteCounter(state.controlLoadState)||!finiteCounter(state.pauseState))
    errors.push(`${label}: control-load or pause state is unavailable`);
  if(typeof localPlayer.eligible!=='boolean'||!finiteCounter(localPlayer.inputButtons)||
      !localPlayer.position||!finiteCoordinate(localPlayer.position.x)||!finiteCoordinate(localPlayer.position.y))
    errors.push(`${label}: local seat eligibility, consumed input, or position is unavailable`);
  for(const key of ['configured','worldReady','frameOpen','waiting'])
    if(typeof netplay[key]!=='boolean')errors.push(`${label}: netplay.${key} is unavailable`);
  if(!finiteCounter(netplay.nextFrame)||!finiteCounter(netplay.generation)||
      !finiteCounter(netplay.lastCompletedFrame)||!finiteCounter(netplay.confirmedThrough))
    errors.push(`${label}: network frame counters are unavailable`);
  const frontier=netplay.frontier;
  if(!finiteCounter(frontier.nextFrame)||!finiteCounter(frontier.predictionFrontier)||
      typeof frontier.correcting!=='boolean'||frontier.correctionEnd!==null&&!finiteCounter(frontier.correctionEnd))
    errors.push(`${label}: network frontier/correction observation is unavailable`);
  if(netplay.rollbackFrame!==null&&!finiteCounter(netplay.rollbackFrame))
    errors.push(`${label}: rollback frame is invalid`);
  return{world,errors};
}

function activeWorldErrors(world,label){
  const errors=[];
  const {state,localPlayer,netplay}=world;
  if(state.loading||state.controlLoadState!==0||state.inTransition||state.paused||state.pauseState!==0||
      state.retrying||state.gameOver||state.dialogue||state.touchContext!=='gameplay')
    errors.push(`${label}: world is not in active gameplay`);
  if(!localPlayer.eligible)errors.push(`${label}: local player is not eligible`);
  if(!netplay.configured||!netplay.worldReady||netplay.frameOpen||netplay.rollbackFrame!==null||
      netplay.frontier.correcting||netplay.frontier.correctionEnd!==null||
      netplay.frontier.nextFrame!==netplay.nextFrame||netplay.frontier.predictionFrontier!==netplay.nextFrame)
    errors.push(`${label}: network has an open frame, active correction, or inconsistent frontier`);
  return errors;
}

function liveNetworkEvidence(world){
  const {netplay}=world;
  return{waiting:netplay.waiting,comparable:world.comparable,
    correctionIdle:!netplay.frameOpen&&netplay.rollbackFrame===null&&netplay.frontier.correcting===false&&
      netplay.frontier.correctionEnd===null&&netplay.frontier.nextFrame===netplay.nextFrame&&
      netplay.frontier.predictionFrontier===netplay.nextFrame,
    allFramesConfirmed:netplay.confirmedThrough>=netplay.lastCompletedFrame,
    lastCompletedFrame:netplay.lastCompletedFrame,confirmedThrough:netplay.confirmedThrough};
}

/** Require live, same-stage gameplay and advancement across all three native clocks. */
export function compareTh08LiveWorld(beforeSnapshot,afterSnapshot,{seat=0}={}){
  const first=read(beforeSnapshot,seat,'pre-disconnect baseline');
  const second=read(afterSnapshot,seat,'pre-disconnect progress sample');
  const errors=[...first.errors,...second.errors];
  if(errors.length)return{passed:false,errors};
  errors.push(...activeWorldErrors(first.world,'pre-disconnect baseline'),
    ...activeWorldErrors(second.world,'pre-disconnect progress sample'));
  const a=first.world.counters,b=second.world.counters;
  if(a.stage!==b.stage)errors.push(`stage changed before disconnect (${a.stage} -> ${b.stage})`);
  for(const key of ['enemySceneFrame','controlTick','lastCompletedNetworkFrame'])
    if(b[key]<=a[key])errors.push(`${key} did not advance before disconnect (${a[key]} -> ${b[key]})`);
  return{passed:errors.length===0,errors,evidence:{before:first.world,after:second.world,
    networkQualification:{before:liveNetworkEvidence(first.world),after:liveNetworkEvidence(second.world)}}};
}

/** A wait sample is ready only after all peer corrections and open frames have settled. */
export function th08DisconnectedWaitState(snapshot,{seat=0,label='disconnect wait sample'}={}){
  const {world,errors}=read(snapshot,seat,label);
  if(errors.length)return{ready:false,errors,blockers:[]};
  const blockers=[];
  const {counters,state,localPlayer,netplay}=world,frontier=netplay.frontier;
  if(state.loading||state.controlLoadState!==0||state.inTransition||state.paused||state.pauseState!==0||
      state.retrying||state.gameOver||state.dialogue||state.touchContext!=='gameplay')blockers.push('world-not-active-gameplay');
  if(!localPlayer.eligible)blockers.push('local-player-ineligible');
  if(!netplay.configured||!netplay.worldReady)blockers.push('world-not-ready');
  if(!netplay.waiting)blockers.push('network-not-waiting');
  if(netplay.frameOpen)blockers.push('network-frame-open');
  if(netplay.rollbackFrame!==null)blockers.push('rollback-active');
  if(frontier.correcting||frontier.correctionEnd!==null)blockers.push('correction-active');
  if(frontier.nextFrame!==netplay.nextFrame||frontier.predictionFrontier!==frontier.nextFrame)
    blockers.push('prediction-frontier-not-settled');
  return{ready:blockers.length===0,errors:[],blockers,world,tuple:th08WorldTuple(world)};
}

function th08WorldTuple(world){
  return{
    counters:{...world.counters},
    state:{loading:world.state.loading,controlLoadState:world.state.controlLoadState,
      pauseState:world.state.pauseState,paused:world.state.paused,retrying:world.state.retrying,
      gameOver:world.state.gameOver,dialogue:world.state.dialogue,touchContext:world.state.touchContext,
      touchContextRaw:world.state.touchContextRaw,
      inTransition:world.state.inTransition},
    localPlayer:{seat:world.localPlayer.seat,eligible:world.localPlayer.eligible,
      inputButtons:world.localPlayer.inputButtons,position:{...world.localPlayer.position},lifeState:world.localPlayer.lifeState},
    netplay:{configured:world.netplay.configured,worldReady:world.netplay.worldReady,waiting:world.netplay.waiting,
      frameOpen:world.netplay.frameOpen,rollbackFrame:world.netplay.rollbackFrame,
      lastCompletedFrame:world.netplay.lastCompletedFrame,confirmedThrough:world.netplay.confirmedThrough,
      nextFrame:world.netplay.nextFrame,generation:world.netplay.generation,mode:world.netplay.mode,
      frontier:{...world.netplay.frontier}},
  };
}

/** Check that gameplay froze across notification, settled, and final stability samples. */
export function compareTh08FrozenWorld(notificationSnapshot,settledSnapshot,stableSnapshot,{seat=0}={}){
  const samples=[notificationSnapshot,settledSnapshot,stableSnapshot].map((snapshot,index)=>
    th08DisconnectedWaitState(snapshot,{seat,label:`disconnect sample ${index+1}`}));
  const errors=[];
  for(let index=0;index<samples.length;index++){
    const sample=samples[index];
    if(sample.errors.length)errors.push(...sample.errors);
    else if(!sample.ready)errors.push(`disconnect sample ${index+1} is not a settled waiting world: ${sample.blockers.join(', ')}`);
  }
  if(samples.every(sample=>sample.tuple)){
    const baseline=JSON.stringify(samples[0].tuple);
    for(let index=1;index<samples.length;index++)
      if(JSON.stringify(samples[index].tuple)!==baseline)
        errors.push(`world state changed between disconnect sample 1 and ${index+1}`);
  }
  return{passed:errors.length===0,errors,evidence:samples.map(sample=>sample.tuple??null),blockers:samples.map(sample=>sample.blockers)};
}
