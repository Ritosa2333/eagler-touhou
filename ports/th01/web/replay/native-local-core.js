import {NativeSimulation} from '../netplay/runtime-core.js';
import {NATIVE_COMMANDS,validNativeCommand} from './native-command-schema.js';
import {pointBytes,toBase64} from './tape.js';
import {AUDITED_CORE} from './native-state.js';
export function nativeLocalOpening(core){
  const at=core.th_score_data(),size=core.th_score_size();
  return {core:AUDITED_CORE,version:'native-local/1',scores:toBase64(new Uint8Array(core.memory.buffer,at,size))};
}
export function loadLocalOpening(core,opening){
  if(opening?.version!=='native-local/1'||opening.core!==AUDITED_CORE||typeof opening.scores!=='string'||opening.scores.length>8192)throw Error('原生单人开局身份无效');
  const data=pointBytes({data:opening.scores});if(data.length!==core.th_score_size())throw Error('原生单人成绩数据长度无效');
  new Uint8Array(core.memory.buffer,core.th_score_data(),data.length).set(data);core.th_score_load(data.length);
}
export class NativeLocalSimulation extends NativeSimulation{
  consumed(){this.frame++;this.previous=Array(this.settings.players).fill(0);}
  command(command){
    if(!validNativeCommand(command))throw Error('原生单人命令无效');
    if(command.name==='th_score_load'){
      const data=pointBytes({data:command.scores});if(data.length!==this.core.th_score_size()||command.args[0]!==data.length)throw Error('原生成绩命令长度无效');
      new Uint8Array(this.core.memory.buffer,this.core.th_score_data(),data.length).set(data);
    }
    this.core[command.name](...command.args);this.consumed();
  }
}
// Wrap a plain facade, not the non-configurable WebAssembly export object.
// Calls and their return values are unchanged; recording errors cannot escape.
export function localExportFacade(core,onCommand,beforeCommand=()=>true){
  return Object.fromEntries(Object.entries(core).map(([name,value])=>[name,NATIVE_COMMANDS.has(name)?(...args)=>{
    try{if(beforeCommand(name,args)===false)return;}catch{}
    let command;
    try{
      command={name,args:args.map(n=>Number(n)|0)};
      if(name==='th_score_load')command.scores=toBase64(new Uint8Array(core.memory.buffer,core.th_score_data(),core.th_score_size()));
    }catch{command=undefined;}
    const result=value(...args);try{onCommand(command);}catch{}return result;
  }:value]));
}
