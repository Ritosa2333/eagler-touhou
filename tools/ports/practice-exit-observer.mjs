/** Observe the production completion callback before it retires the native owner. */
export function installPracticeExitObserver({runtime}){
  const key='__eaglerPracticeExitObserver',owner=globalThis[runtime.global],module=owner?.[runtime.module],core=owner?.[runtime.core];
  if(globalThis[key]||typeof module?.runtimeFinish!=='function'||typeof core?.status!=='function')throw Error('Practice completion observer cannot bind this native owner');
  const original=module.runtimeFinish,state={module,original,entries:[],dropped:0};
  const copy=(name,count)=>{const app=globalThis[runtime.global]?.[runtime.app],pointer=core[name](app),memory=core.memory?.buffer??module.HEAPU8?.buffer;
    if(!memory||!Number.isInteger(pointer)||pointer<=0||pointer%8||pointer+count*8>memory.byteLength)throw Error('Practice completion snapshot outside native memory');
    return{available:true,words:Array.from(new Float64Array(memory,pointer,count))};};
  state.wrapper=function(...args){
    try{
      const app=globalThis[runtime.global]?.[runtime.app],invalid=core.status(app,2),faults=core.status(app,4);
      if(args[0]||invalid||faults){
        if(state.entries.length>=8)state.dropped++;
        else state.entries.push({result:args[0],invalid,faults,app,status:Array.from({length:12},(_,i)=>core.status(app,i)),
          practice:copy('practice_snapshot_v1',128),initial:copy('gameplay_initial_snapshot_v1',80),
          url:globalThis.location?.href??null,timeOrigin:performance.timeOrigin});
      }
    }catch(error){if(state.entries.length<8)state.entries.push({captureError:String(error)});else state.dropped++;}
    return original.apply(this,args);
  };
  module.runtimeFinish=state.wrapper;globalThis[key]=state;return{available:true,armed:true};
}

export function releasePracticeExitObserver(){
  const key='__eaglerPracticeExitObserver',state=globalThis[key];
  if(!state)return{available:false,restored:false,entries:[],dropped:0};
  if(state.module.runtimeFinish!==state.wrapper)throw Error('Practice completion owner changed before observer cleanup');
  state.module.runtimeFinish=state.original;delete globalThis[key];
  return{available:true,restored:true,entries:state.entries,dropped:state.dropped};
}
