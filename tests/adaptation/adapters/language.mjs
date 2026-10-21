import {resolveAdaptationAdapter} from './registry.mjs';

export function languageObservationRequest({game,variant='normal',action='read'}){
  const adapter=resolveAdaptationAdapter(game,variant);
  return{game,variant,action,contract:adapter?.language?{...adapter.language,runtime:adapter.runtime}:null};
}

/** Serialized into the real Runtime frame: observe or copy native records, never query a text key. */
export function readLanguageObservation({game,variant,action='read',contract}){
  if(!contract)return{available:false,reason:`No language observation adapter for ${game}/${variant}`};
  const runtime=globalThis[contract.runtime.global],core=runtime?.[contract.runtime.core];
  if(!core||[contract.enableExport,contract.sizeExport,contract.copyExport,contract.allocateExport,contract.freeExport]
    .some(name=>typeof core[name]!=='function'))return{available:false,reason:'Declared native language observation exports are absent'};
  if(action==='enable'){
    core[contract.enableExport](1);return{available:true,enabled:true};
  }
  if(action!=='read')throw Error(`Unknown language observation action ${action}`);
  const size=core[contract.sizeExport]();
  if(!Number.isSafeInteger(size)||size<=1||size>16*1024*1024)throw Error(`Invalid native language observation size ${size}`);
  const pointer=core[contract.allocateExport](size);
  if(!Number.isInteger(pointer)||pointer<=0)throw Error('Native language observation allocation failed');
  try{
    const buffer=core.memory?.buffer??runtime[contract.runtime.module]?.HEAPU8?.buffer;
    if(!buffer||pointer+size>buffer.byteLength)throw Error('Native language observation buffer is outside memory');
    const written=core[contract.copyExport](pointer,size),current=core.memory?.buffer??runtime[contract.runtime.module]?.HEAPU8?.buffer;
    if(!Number.isSafeInteger(written)||written<=0||written>=size||pointer+size>current.byteLength||new Uint8Array(current)[pointer+written]!==0)
      throw Error('Native language observation copy is truncated or lacks its NUL boundary');
    const trace=JSON.parse(new TextDecoder('utf-8',{fatal:true}).decode(new Uint8Array(current,pointer,written)));
    if(trace.schema!=='eagler-touhou/language-trace/1')throw Error('Unknown native language observation schema');
    return{available:true,trace};
  }finally{core[contract.freeExport](pointer);}
}
