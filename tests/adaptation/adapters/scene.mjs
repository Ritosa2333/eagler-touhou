import {resolveAdaptationAdapter} from './registry.mjs';

/** Serializable mapping; one title module registers both normal and MP independently. */
export function validationSceneRequest({game,variant,action='read'}){
  const adapter=resolveAdaptationAdapter(game,variant);
  return{game,variant,action,contract:adapter?.controlledScene?{...adapter.controlledScene,runtime:adapter.runtime}:null};
}

/** Arm before title navigation or read the native receipt after world creation. */
export function controlValidationScene({game,variant,action='read',contract}){
  if(!['arm','read'].includes(action))throw Error(`Unknown validation-scene action ${action}`);
  if(!contract)return{available:false,reason:`No controlled scene adapter is registered for ${game}/${variant}`};
  if(contract.schema!=='eagler-touhou/controlled-input-scene/3')throw Error('Unsupported controlled scene contract');
  const mapping=contract.runtime,runtime=globalThis[mapping.global],core=runtime?.[mapping.core];
  const app=runtime?.[mapping.app],module=runtime?.[mapping.module];
  if(app==null||typeof core?.[contract.armExport]!=='function'||typeof core?.[contract.stateExport]!=='function'||
     typeof core?.[contract.armStateExport]!=='function'||typeof core?.[contract.writeStateExport]!=='function')
    return{available:false,reason:`${game}/${variant} Runtime has no declared pre-world controlled-scene interface`};
  let armAccepted=null;
  if(action==='arm'){
    const value=core[contract.armExport](app);
    if(value===true||value===1)armAccepted=true;
    else if(value===false||value===0)armAccepted=false;
    else throw Error('Controlled scene arm returned an invalid acceptance value');
  }
  const buffer=core.memory?.buffer??module?.HEAPU8?.buffer;
  const words=(exportName,count)=>{
    const pointer=core[exportName](app);
    if(!buffer||!Number.isInteger(pointer)||pointer<=0||pointer%4||pointer+count*4>buffer.byteLength)
      throw Error(`Controlled scene ${exportName} receipt points outside native memory`);
    return Array.from(new Uint32Array(buffer,pointer,count));
  };
  const snapshot=words(contract.stateExport,16);
  if(snapshot[0]!==1)throw Error(`Unsupported controlled scene snapshot ABI ${snapshot[0]}`);
  const lifecycle=words(contract.armStateExport,8);
  if(lifecycle[0]!==1)throw Error(`Unsupported controlled scene lifecycle ABI ${lifecycle[0]}`);
  const write=words(contract.writeStateExport,8);
  if(write[0]!==1)throw Error(`Unsupported controlled scene write receipt ABI ${write[0]}`);
  const flag=(value,label)=>{
    if(value!==0&&value!==1)throw Error(`Controlled scene ${label} flag must be 0 or 1`);
    return value===1;
  };
  if(![0,1,2,3].includes(lifecycle[1]))throw Error(`Controlled scene arm status ${lifecycle[1]} is invalid`);
  const status=lifecycle[1];
  return{available:true,action,armAccepted,armStatus:status,requestGeneration:lifecycle[2],requestPending:flag(lifecycle[3],'request-pending'),
    accepted:status===2?true:status===3?false:null,
    acceptedBeforeFirstUpdate:flag(lifecycle[4],'accepted-before-first-update'),
    firstTimelineWithheld:flag(lifecycle[5],'first-timeline-withheld'),
    updateStarted:flag(lifecycle[6],'update-started'),sessionWriteLocked:flag(lifecycle[7],'session-write-lock'),
    writePolicy:contract.writePolicy,
    writeReceipt:{locked:flag(write[1],'write-lock'),loadScope:flag(write[2],'write-load-scope'),attempts:write[3],
      ramOverrides:write[4],discardedBytes:write[5],committedBytes:write[6],rejectedWrites:write[7]},
    active:flag(snapshot[1],'active'),eligible:flag(snapshot[2],'eligible'),ready:flag(snapshot[3],'ready'),busy:flag(snapshot[4],'busy'),
    replay:flag(snapshot[5],'replay'),enemies:snapshot[6],bullets:snapshot[7],lasers:snapshot[8],stage:snapshot[9],
    enemyFrame:snapshot[10],controlFrame:snapshot[11],withheldTimelineUpdates:snapshot[12],timelineCount:snapshot[13],
    assisted:flag(snapshot[14],'assisted'),lifeState:snapshot[15],source:contract.source,
    scope:'controlled-input-world',restore:'retire-world'};
}

export function assertControlledSceneArmed(receipt){
  if(!receipt?.available||receipt.armAccepted!==true||receipt.armStatus!==1||!receipt.requestPending){
    const error=Error(`Controlled input request was not armed before title navigation: ${JSON.stringify(receipt)}`);
    error.evidence=receipt;throw error;
  }
  return receipt;
}

function assertControlledSceneReceipt(receipt){
  if(receipt?.accepted===false){
    const error=Error(`Controlled input world request was rejected: ${JSON.stringify(receipt)}`);
    error.evidence=receipt;throw error;
  }
  if(!receipt?.available||receipt.accepted!==true||!receipt.acceptedBeforeFirstUpdate||
     !receipt.firstTimelineWithheld||!receipt.updateStarted||!receipt.sessionWriteLocked||
     !receipt.active||!receipt.ready||receipt.replay||!receipt.assisted)
    {const error=Error(`Controlled input world lacks its pre-timeline acceptance receipt: ${JSON.stringify(receipt)}`);error.evidence=receipt;throw error;}
  const writes=receipt.writeReceipt;
  if(!writes?.locked||writes.loadScope||writes.committedBytes!==0)
    {const error=Error(`Controlled input world lacks a zero-commit disposable-write receipt: ${JSON.stringify(writes)}`);error.evidence=receipt;throw error;}
  if(receipt.writePolicy?.initialization==='ram-score-overlay'){
    if(writes.attempts<1||writes.ramOverrides<1||writes.discardedBytes<1)
      {const error=Error(`Controlled input initialization lacks its RAM score overlay receipt: ${JSON.stringify(writes)}`);error.evidence=receipt;throw error;}
  }else if(receipt.writePolicy?.initialization==='none'){
    if(writes.attempts!==0||writes.ramOverrides!==0||writes.discardedBytes!==0)
      {const error=Error(`Controlled input reported an undeclared initialization write: ${JSON.stringify(writes)}`);error.evidence=receipt;throw error;}
  }else{
    const error=Error(`Controlled input world has no supported initialization write policy: ${JSON.stringify(receipt.writePolicy)}`);error.evidence=receipt;throw error;
  }
  if(!Number.isInteger(receipt.timelineCount)||receipt.timelineCount<=0)
    throw Error('Controlled input world has no original ECL timeline to suppress');
  if(!Number.isInteger(receipt.withheldTimelineUpdates)||receipt.withheldTimelineUpdates<=0)
    throw Error('Controlled input world has not withheld an original ECL timeline update');
  return receipt;
}

export function assertControlledSceneWorld(receipt){
  assertControlledSceneReceipt(receipt);
  if(receipt.enemies||receipt.bullets||receipt.lasers)
    throw Error(`Controlled input world contains live hazards: ${receipt.enemies}/${receipt.bullets}/${receipt.lasers}`);
  return receipt;
}

/** Explicit fixture phases keep their own hazards/modal policy. Movement stays strict. */
export function assertControlledSceneStimulus(receipt,previous,{kind}={}){
  if(!['single-projectile','dialogue'].includes(kind))throw Error('Declare the controlled stimulus kind');
  assertControlledSceneReceipt(receipt);
  if(receipt.enemies||receipt.lasers||receipt.bullets>(kind==='single-projectile'?1:0))
    throw Error(`Unexpected hazards in ${kind} fixture: ${receipt.enemies}/${receipt.bullets}/${receipt.lasers}`);
  if(kind==='single-projectile'&&receipt.busy)throw Error('The projectile fixture has an unexpected modal owner');
  if(previous){
    if(receipt.stage!==previous.stage)throw Error('The controlled stimulus crossed native stages');
    for(const field of ['controlFrame','enemyFrame','withheldTimelineUpdates']){
      const delta=receipt[field]-previous[field];
      if(kind==='single-projectile'?delta!==1:delta<0||delta>1)
        throw Error(`The ${kind} fixture has an invalid ${field} boundary`);
    }
    assertWriteContinuity(receipt,previous);
  }
  return receipt;
}

function assertWriteContinuity(receipt,previous){
  for(const field of ['attempts','ramOverrides','discardedBytes','rejectedWrites'])
    if(receipt.writeReceipt[field]<previous.writeReceipt[field])throw Error(`Controlled input write receipt ${field} moved backwards`);
  if(receipt.writeReceipt.committedBytes!==previous.writeReceipt.committedBytes)
    throw Error('Controlled input write receipt changed committed bytes during the session');
}

export function assertControlledScene(receipt,previous){
  assertControlledSceneWorld(receipt);
  if(receipt.busy)throw Error('Controlled input movement world is occupied by a modal owner');
  if(previous){
    if(receipt.stage!==previous.stage)throw Error('Controlled input world changed stage');
    if(receipt.controlFrame!==previous.controlFrame+1||receipt.enemyFrame!==previous.enemyFrame+1){
      const error=Error(`Controlled input world did not advance exactly one original logic tick: controller ${previous.controlFrame}->${receipt.controlFrame}, enemy ${previous.enemyFrame}->${receipt.enemyFrame}`);
      error.evidence={previous,current:receipt};throw error;
    }
    if(receipt.withheldTimelineUpdates!==previous.withheldTimelineUpdates+1)
      throw Error('Controlled input world did not withhold the original timeline update');
    assertWriteContinuity(receipt,previous);
  }
  return receipt;
}

/** A paused controller advances while the original world stays frozen. */
export function assertControlledScenePauseBoundary(receipt,previous,{before,after}={}){
  assertControlledSceneWorld(receipt);
  if(!previous||after?.active!==true)
    throw Error('Controlled pause boundary requires independent native pause evidence');
  if(receipt.stage!==previous.stage)throw Error('Controlled input world changed stage');
  if(receipt.controlFrame!==previous.controlFrame+1)
    throw Error('Controlled pause boundary did not advance exactly one original controller tick');
  for(const key of ['enemyFrame','withheldTimelineUpdates'])
    if(receipt[key]!==previous[key]){
      const error=Error(`Controlled pause boundary advanced ${key}: ${previous[key]}->${receipt[key]}`);
      error.evidence={previous,current:receipt,pause:{before,after}};throw error;
    }
  assertWriteContinuity(receipt,previous);
  return receipt;
}
