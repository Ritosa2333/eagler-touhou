import {resolveAdaptationAdapter} from './registry.mjs';

export function runBoundaryRequest({game,variant,action='read',runId,stage}){
  const adapter=resolveAdaptationAdapter(game,variant);
  return{game,variant,action,runId,stage,contract:adapter?.runBoundaries?{...adapter.runBoundaries,runtime:adapter.runtime}:null};
}

/** Test input requests use the normal supervisor; observations never write game state. */
export function controlRunBoundary({game,variant,action,runId,stage,contract}){
  if(!contract)return{available:false,reason:`No ordinary-run boundary adapter for ${game}/${variant}`};
  const runtime=globalThis[contract.runtime.global],core=runtime?.[contract.runtime.core],module=runtime?.[contract.runtime.module];
  const api=name=>core?.[name]??module?.[`_${name}`],app=runtime?.[contract.runtime.app];
  if(!runtime||app==null)return{available:false,reason:'The mapped ordinary Runtime is unavailable'};
  if(action!=='read'){
    const name=action==='arm'?contract.armExport:contract.transitionExport,fn=api(name);
    if(typeof fn!=='function')return{available:false,reason:`Runtime has no ${name}`};
    if(!['arm','stage','result'].includes(action))throw Error('Unknown ordinary-run request');
    if(action!=='arm'&&(!Number.isSafeInteger(runId)||runId<1||!Number.isSafeInteger(stage)||stage<0))
      throw Error('A transition request must name its observed native run and stage');
    return{available:true,accepted:(action==='arm'?fn(app):fn(app,action==='stage'?1:2,runId,stage))===1};
  }
  const fn=api(contract.stateExport);
  if(typeof fn!=='function')return{available:false,reason:`Runtime has no ${contract.stateExport}`};
  const p=fn(app),buffer=core.memory?.buffer??module?.HEAPU8?.buffer;
  if(!Number.isSafeInteger(p)||p<=0||p%8||!buffer||p+17*8>buffer.byteLength)throw Error('Invalid ordinary-run snapshot pointer');
  const values=Array.from(new Float64Array(buffer,p,17));
  if(values[0]!==1)throw Error('Unknown ordinary-run snapshot ABI');
  if(values[1]!==1)return{available:false,reason:'Ordinary-run snapshot is not available'};
  for(const n of [2,5,6,9,10,13,16])if(values[n]!==0&&values[n]!==1)throw Error(`Invalid ordinary-run boolean ${n}`);
  const flags=values[8],normal=(flags&contract.nonNormalFlags)===0&&values[9]===0;
  return{available:true,armed:!!values[2],runId:values[3],normal,practice:!!(flags&1)||!!values[9],replay:!!(flags&8),
    scene:values[4]===contract.gameScene&&values[5]===1&&values[6]===0?'gameplay':
      values[4]===contract.resultScene?'result':'transition',
    stage:values[7],cheatMovementUsed:!!values[10],playerPosition:{x:values[11],y:values[12]},
    resultLag:values[13]===1?{available:true,value:values[14],source:contract.sources.resultLag}:null,
    transitionCount:values[15],controlledWorld:!!values[16],native:values};
}
