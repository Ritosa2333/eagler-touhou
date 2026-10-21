import {resolveAdaptationAdapter} from './registry.mjs';

export function deathbombRequest({game,variant}){
  const adapter=resolveAdaptationAdapter(game,variant);
  return{game,variant,contract:adapter?.deathbomb?{...adapter.deathbomb,runtime:adapter.runtime}:null};
}

/** A read-only view of a real projectile collision and the next consumer window. */
export function readDeathbombState({game,variant,contract}){
  if(!contract)return{available:false,reason:`No projectile/Deathbomb observer registered for ${game}/${variant}`};
  if(contract.schema!=='eagler-touhou/stg-deathbomb/1'||contract.wordCount!==36)
    throw Error('Unsupported Deathbomb observer contract');
  const runtime=globalThis[contract.runtime.global],core=runtime?.[contract.runtime.core],module=runtime?.[contract.runtime.module];
  const exported=core?.[contract.stateExport]??module?.[`_${contract.stateExport}`];
  if(typeof exported!=='function')return{available:false,reason:`Runtime has no ${contract.stateExport}`};
  const pointer=exported.call(core,runtime[contract.runtime.app]),buffer=core.memory?.buffer??module?.HEAPU8?.buffer;
  if(!Number.isSafeInteger(pointer)||pointer<=0||pointer%4||!buffer||pointer+144>buffer.byteLength)
    throw Error('Deathbomb observer returned an invalid native address');
  const w=Array.from(new Uint32Array(buffer,pointer,36)),floats=new Float32Array(buffer,pointer,36);
  if(w[0]!==1||w[35]!==0)throw Error('Unsupported Deathbomb snapshot ABI');
  if(w[1]!==1)return{available:true,complete:false,reason:'No current native Deathbomb snapshot',words:w};
  for(const n of [2,3,4,12,16,18,20])if(w[n]>1)throw Error(`Deathbomb snapshot boolean ${n} is invalid`);
  return{available:true,complete:true,fixture:{enabled:!!w[2],writeLocked:!!w[3],eligible:!!w[4]},
    request:{status:w[5],sequence:w[6]},projectile:{state:w[7],liveCount:w[30],nativeState:w[34]},
    collision:{source:w[8],sequence:w[9],consumerSequence:w[10],beforeLife:w[31]|0,afterLife:w[32]|0,result:w[33]|0},
    consumer:{sequence:w[11],valid:!!w[12]},life:{state:w[13]|0,timer:w[14]|0,remainingTicks:w[15]|0,deathbomb:!!w[16]},
    bomb:{cooldown:w[17]|0,active:!!w[18],type:w[19]|0,triggered:!!w[20],consumed:w[21]|0},
    inventory:{bombs:w[22]|0,regularBombs:w[23]|0,lives:w[24]|0,deaths:w[25]|0,bombsUsed:w[26]|0,bombsUsedStage:w[27]|0},
    position:{x:floats[28],y:floats[29]},words:w};
}

export function requestDeathbombHit({contract}){
  if(!contract)return{accepted:false,reason:'No projectile fixture registered'};
  const runtime=globalThis[contract.runtime.global],core=runtime?.[contract.runtime.core];
  if(typeof core?.[contract.hitExport]!=='function')return{accepted:false,reason:`Runtime has no ${contract.hitExport}`};
  return{accepted:core[contract.hitExport](runtime[contract.runtime.app])===1};
}
