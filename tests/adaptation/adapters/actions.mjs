import assert from 'node:assert/strict';
import {resolveAdaptationAdapter} from './registry.mjs';

export const STG_ACTIONS_SCHEMA='eagler-touhou/stg-actions/1';
export const STG_ACTIONS_WORD_COUNT=20;

/** Serializable contract for a title's read-only post-consumer snapshot. */
export function touchActionsRequest({game,variant,adapterRegistry={resolve:resolveAdaptationAdapter}}){
  assert.equal(typeof adapterRegistry?.resolve,'function','STG action requests require an adaptation registry');
  const adapter=adapterRegistry.resolve(game,variant);
  return{game,variant,contract:adapter?.actions?{...adapter.actions,runtime:adapter.runtime}:null};
}

/** Read a title-declared native snapshot without advancing or mutating Runtime state. */
export function readTouchActionState({game,variant,contract}){
  const schema='eagler-touhou/stg-actions/1',wordCount=20;
  if(!contract)return{available:false,reason:`No STG action observer is registered for ${game}/${variant}`};
  if(contract.schema!==schema||contract.wordCount!==wordCount||
     contract.sampleBoundary!=='after-player-consumer')throw Error('Unsupported STG action observer contract');
  const mapping=contract.runtime;
  if(!mapping?.global||!mapping?.core||!mapping?.module||!mapping?.app)throw Error('STG action observer has no Runtime owner mapping');
  const root=typeof window==='object'?window:globalThis;
  const runtime=root[mapping.global],core=runtime?.[mapping.core],module=runtime?.[mapping.module];
  const exported=core?.[contract.stateExport]??core?.[`_${contract.stateExport}`]??
    module?.[contract.stateExport]??module?.[`_${contract.stateExport}`];
  if(typeof exported!=='function')return{available:false,reason:`${game}/${variant} Runtime does not export ${contract.stateExport}`};
  const app=runtime?.[mapping.app];
  if(contract.stateApp&&app==null)return{available:false,reason:`${game}/${variant} action observer requires the Runtime app instance`};
  const pointer=contract.stateApp?exported.call(core,app):exported.call(core);
  const buffer=core?.memory?.buffer??module?.HEAPU8?.buffer;
  const bytes=wordCount*4;
  if(!Number.isInteger(pointer)||pointer<=0||pointer%4||!buffer||pointer+bytes>buffer.byteLength)
    throw Error(`${contract.stateExport} returned an invalid native snapshot pointer`);
  const words=Array.from(new Uint32Array(buffer,pointer,wordCount));
  const [version,validMask,phase,sequence,frame,stage,current,previous,focused,shootingCurrent,
    shootingPrevious,activePlayerShots,bombActive,bombTriggered,bombType,bombTimer,
    bombsRemaining,bombsUsed,bombsUsedStage,lifeState]=words;
  if(version!==1)throw Error(`Unsupported STG action snapshot ABI ${version}`);
  if(validMask&~0xff)throw Error('STG action snapshot has unknown validity bits');
  const masks=contract.inputMasks;
  for(const key of ['fire','focus','bomb','escape']){
    if(!Number.isSafeInteger(masks?.[key])||masks[key]<=0||masks[key]>0x80000000||!Number.isInteger(Math.log2(masks[key])))
      throw Error(`STG action contract has no single-bit ${key} mask`);
  }
  if(new Set(['fire','focus','bomb','escape'].map(key=>masks[key])).size!==4)throw Error('STG action masks must be distinct');
  if(phase===0){
    if(validMask!==0)throw Error('Unpublished STG action snapshot cannot mark fields valid');
    return{available:true,complete:false,reason:'No completed player-consumer snapshot yet',version,validMask,phase:'none',
      sequence:null,frame:null,stage:null,consumed:null,focused:null,shootingTimer:null,activePlayerShots:null,
      bomb:null,inventory:null,lifeState:null,words};
  }
  if(phase===2){
    if(validMask!==0x81)throw Error('Paused STG action snapshot must expose only tick/stage and life');
    return{available:true,complete:false,version,validMask,phase:'paused',sequence:null,frame,stage:stage|0,
      consumed:null,focused:null,shootingTimer:null,activePlayerShots:null,bomb:null,inventory:null,lifeState:lifeState|0,words};
  }
  if(phase!==1||validMask!==0xff)throw Error('Completed player-consumer snapshot has an invalid phase or validity mask');
  if(!sequence)throw Error('Completed player-consumer snapshot needs a nonzero consumer sequence');
  if([focused,bombActive,bombTriggered].some(value=>value!==0&&value!==1))
    throw Error('Completed player-consumer snapshot has a non-boolean action state');
  return{available:true,complete:true,version,validMask,phase:'consumer-run',sequence,frame,stage:stage|0,
    consumed:{current,previous,fire:Boolean(current&masks.fire),focus:Boolean(current&masks.focus),
      bomb:Boolean(current&masks.bomb),escape:Boolean(current&masks.escape)},
    focused:Boolean(focused),shootingTimer:{current:shootingCurrent|0,previous:shootingPrevious|0},
    activePlayerShots,bomb:{active:Boolean(bombActive),triggered:Boolean(bombTriggered),type:bombType|0,timer:bombTimer|0},
    inventory:{bombsRemaining: bombsRemaining|0,bombsUsed:bombsUsed|0,bombsUsedStage:bombsUsedStage|0},
    lifeState:lifeState|0,words};
}
