// One installed launch owner per document. Runtime readiness and actual presentation
// are separate from the browser's completion of a module import.
let installed=null;
export function installPageLaunchContext(context){
 if(installed&&installed!==context)throw new Error('A page launch context is already installed');
 installed=context;return context;
}
export function getPageLaunchContext(){return installed;}
export function createPageLaunchContext({config={},onPresented=()=>{}}={}){
 const controller=new AbortController(),settings=structuredClone(config),waiters=[];
 let runtime=null,ready=false,failure=null,stopped=false;
 const notify=()=>{while(waiters.length){const {resolve,reject}=waiters.shift();if(failure)reject(failure);else if(ready)resolve(runtime);}};
 const stopRuntime=()=>{if(runtime&&!stopped){stopped=true;runtime.exit();}};
 const context={
  get runtime(){return ready&&!failure?runtime:null;},get config(){return structuredClone(settings);},get signal(){return controller.signal;},
  bindRuntime(value){
   if(!value||['keyboard','clearInput','exit'].some(name=>typeof value[name]!=='function'))throw new TypeError('Runtime page hooks are incomplete');
   if(runtime&&runtime!==value)throw new Error('A runtime is already bound to this launch');
   runtime=value;if(failure){stopRuntime();return false;}return true;
  },
  finish(){if(failure)return false;if(!runtime)throw new Error('Runtime cannot be ready before hooks exist');ready=true;notify();return true;},
  fail(error){if(failure)return;failure=error instanceof Error?error:new Error(String(error));ready=false;notify();},
  waitRuntime(){if(failure)return Promise.reject(failure);if(ready)return Promise.resolve(runtime);return new Promise((resolve,reject)=>waiters.push({resolve,reject}));},
  notePresented(actual){if(actual===true&&ready&&!failure&&!controller.signal.aborted)onPresented(true);},
  abort(){if(controller.signal.aborted)return;controller.abort();failure??=new DOMException('Runtime launch owner exited','AbortError');ready=false;try{stopRuntime();}finally{notify();}}
 };
 return context;
}
