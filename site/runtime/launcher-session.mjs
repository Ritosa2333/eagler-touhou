// Eagler single-player session transport. Page hooks, resources and actual presentation
// remain explicit; this module never loads a game or claims an absent capability.
import{touchOptions}from'./touch-options.mjs';
import{installLauncherGamepadInput}from'./gamepad-input.mjs';
const PROTOCOL='eagler-touhou/1',GAME='th128';
const THPRAC_LOCALES=new Map([['zh','zh'],['zh-CN','zh'],['ja','ja'],['ja-JP','ja'],['en','en'],['en-US','en']]);
const KEYS=new Set(['ArrowUp','ArrowDown','ArrowLeft','ArrowRight','KeyZ','KeyX','KeyC','ControlLeft','KeyR','Enter','Escape','ShiftLeft','ShiftRight','Tab','Backspace','F1','F2','F3','F4','F5','F6','F7','F12']);
const own=(value,key)=>Object.hasOwn(value,key),record=value=>value!==null&&typeof value==='object'&&!Array.isArray(value)
 &&(Object.getPrototypeOf(value)===Object.prototype||Object.getPrototypeOf(value)===null);
const invalid=message=>Object.assign(new Error(message),{errno:'invalid-field'});
export function mountLauncherSession({window=globalThis.window,document=window?.document,location=window?.location,hooks={}}={}){
 if(!window||!location||!window.parent||window.parent===window)return null;
 const values=new URLSearchParams(location.search??'').getAll('runtimeEpoch');
 if(values.length!==1||!(/^[1-9][0-9]*$/).test(values[0]))return null;
 const epoch=Number(values[0]);if(!Number.isSafeInteger(epoch))return null;
 let parentOrigin;
 try{const referrer=new URL(document?.referrer);if(!['https:','http:'].includes(referrer.protocol)||referrer.origin==='null')return null;parentOrigin=referrer.origin;}catch{return null;}
 const parent=window.parent,controller=new AbortController(),requests=new Map();
 let phase='booting',config=null,launchSerial=null,presented=false,closed=false,busy=false,resourcesBusy=false,fileBusy=false;
 const detachGamepad=parentOrigin===location.origin?installLauncherGamepadInput({target:window,protocol:PROTOCOL,game:GAME,epoch,origin:location.origin,isReady:()=>phase==='running'}):null;
 const post=body=>parent.postMessage({...body,protocol:PROTOCOL,game:GAME,epoch},parentOrigin);
 const reply=(request,ok,body={})=>{
  if(request===undefined)return;const response={...body,request,ok};
  requests.set(request,response);
  const completed=[...requests].filter(([,value])=>value!==null);
  for(let i=0;i<completed.length-256;i++)requests.delete(completed[i][0]);
  post(response);
 };
 const fail=(request,errno,message)=>reply(request,false,{errno,error:String(message)});
 const hook=async(name,value)=>{if(typeof hooks[name]!=='function')throw Object.assign(new Error('Runtime '+name+' bridge is unavailable'),{errno:'unavailable'});return hooks[name](value);};
 function detach(){window.removeEventListener('message',receive);window.removeEventListener('pagehide',pagehide);detachGamepad?.();}
 async function stop(request,clean){
  if(closed)return false;closed=true;phase='exited';controller.abort();detach();
  try{await hook('exit',{signal:controller.signal,clean});}catch(error){if(clean)fail(request,error.errno??'internal-error',error?.message??error);if(clean&&request===undefined)throw error;return false;}
  if(clean){reply(request,true);post({event:'exit',code:0,status:'success'});}
  return true;
 }
 function noteFrame(actual,phaseAllowed){
  if(actual!==true||closed||presented||!phaseAllowed)return false;
  presented=true;post({event:'first-frame',...(launchSerial===null?{}:{launchSerial})});return true;
 }
 const pagehide=()=>{void stop(undefined,false);};
 function configureValue(data){
  if(!['ogg','none','midi'].includes(data.music))throw invalid('configure.music is invalid');
  const value={music:data.music};
  if(own(data,'language')){if(typeof data.language!=='string'||!data.language||data.language.length>64)throw invalid('configure.language is invalid');value.language=data.language;}
  for(const key of ['resources','runtimeResources','sharedResources']){
   if(!own(data,key))continue;if(!Array.isArray(data[key])||data[key].length>4096)throw invalid('configure.'+key+' must be a bounded array');value[key]=data[key];
  }
  if(own(data,'runtimePack'))value.runtimePack=data.runtimePack;
  if(own(data,'options')){
   if(!record(data.options))throw invalid('configure.options must be an object');
   try{touchOptions(data.options);}catch(error){throw invalid(error.message);}
   value.options={...data.options};
   if(own(value.options,'thpracLocale')){
    const locale=THPRAC_LOCALES.get(value.options.thpracLocale);
    if(!locale)throw invalid('configure.options.thpracLocale is unavailable');
    value.options.thpracLocale=locale;
   }
  }
  return value;
 }
 async function execute(data){
  const {command,request}=data;
  if(command==='exit'){await stop(request,true);return;}
  if(closed)return;
  if(['list','read','write','remove','sync'].includes(command)){
   if(fileBusy||busy||!['ready','configured','running'].includes(phase)||(phase==='running'&&['write','remove'].includes(command))){
    fail(request,'invalid-state','Replay file command is unavailable in this phase');return;
   }
   fileBusy=true;
   try{
    const result=await hook('files',{command,data,phase,signal:controller.signal});if(!closed)reply(request,true,result);
   }catch(error){if(!closed)fail(request,error.errno??'internal-error',error.message??error);}
   finally{fileBusy=false;}
   return;
  }
  const touchCommands=['direct-touch','touch-controls','touch-cancel'];
  const supported=['configure','resources','launch','keyboard','keyboard-clear',...touchCommands];
  if(!supported.includes(command)){fail(request,'unsupported-command','Runtime does not implement '+command);return;}
  if(command==='resources'&&phase==='running'&&hooks.acceptLiveResources?.()===true){
   if(resourcesBusy){fail(request,'invalid-state','A resource receipt is already pending');return;}
   resourcesBusy=true;
   try{
    if(!Array.isArray(data.resources)||data.resources.length>4096)throw invalid('resources must be a bounded array');
    await hook('resources',data.resources);if(!closed)reply(request,true);
   }catch(error){if(!closed)fail(request,error.errno??'internal-error',error.message??error);}
   finally{resourcesBusy=false;}
   return;
  }
  if(phase==='booting'||busy||fileBusy&&!['keyboard','keyboard-clear',...touchCommands].includes(command)){fail(request,'invalid-state','Runtime is not ready for '+command);return;}
  if(touchCommands.includes(command)){
   if(phase!=='running'){fail(request,'invalid-state','Runtime input is not running');return;}
   try{
    if(command==='direct-touch'){
     if(!['down','move','up','cancel'].includes(data.type)||!Number.isSafeInteger(data.id)||Math.abs(data.id)>2147483647
      ||![data.x,data.y].every(v=>typeof v==='number'&&Number.isFinite(v)&&Math.abs(v)<=16))throw invalid('direct-touch input is invalid');
     await hook('directTouch',{type:data.type,id:data.id,x:data.x,y:data.y});
    }else if(command==='touch-controls'){
     const value={};for(const key of ['fireEnabled','focusEnabled']){if(typeof data[key]!=='boolean')throw invalid('touch-controls '+key);value[key]=data[key];}
     for(const key of ['bombSerial','escapeSerial']){if(!Number.isSafeInteger(data[key])||data[key]<0||data[key]>2147483647)throw invalid('touch-controls '+key);value[key]=data[key];}
     for(const key of ['joystickX','joystickY']){if(!Number.isSafeInteger(data[key])||Math.abs(data[key])>32767)throw invalid('touch-controls '+key);value[key]=data[key];}
     if(own(data,'touchSensitivity')){if(!Number.isInteger(data.touchSensitivity)||data.touchSensitivity<100||data.touchSensitivity>300)throw invalid('touch-controls sensitivity');value.touchSensitivity=data.touchSensitivity;}
     for(const key of ['thpracMobilePanelsEnabled','thpracMobilePanelsVisible'])if(own(data,key)){if(typeof data[key]!=='boolean')throw invalid('touch-controls '+key);value[key]=data[key];}
     await hook('touchControls',value);
    }else await hook('touchCancel');
    if(!closed)reply(request,true);
   }catch(error){if(!closed)fail(request,error.errno??'internal-error',error.message??error);}
   return;
  }
  if(command==='keyboard'||command==='keyboard-clear'){
   if(phase!=='running'){fail(request,'invalid-state','Runtime input is not running');return;}
   try{
    if(command==='keyboard'){
     if(typeof data.down!=='boolean'||!KEYS.has(data.code)||own(data,'repeat')&&typeof data.repeat!=='boolean'
      ||own(data,'inputSource')&&!['physical','virtual'].includes(data.inputSource))throw invalid('keyboard input is invalid');
     await hook('keyboard',{down:data.down,code:data.code,repeat:data.repeat??false,inputSource:data.inputSource??'physical'});
    }else await hook('keyboardClear');
    if(!closed)reply(request,true);
   }catch(error){if(!closed)fail(request,error.errno??'internal-error',error.message);}
   return;
  }
  if(command==='launch'&&phase!=='configured'||command!=='launch'&&phase!=='ready'&&phase!=='configured'){
   fail(request,'invalid-state','Invalid runtime phase for '+command);return;
  }
  const before=phase;busy=true;
  try{
   if(command==='configure'){const next=configureValue(data);await hook('configure',next);if(closed)return;config=next;phase='configured';}
   else if(command==='resources'){
    if(!Array.isArray(data.resources)||data.resources.length>4096)throw invalid('resources must be a bounded array');
    await hook('resources',data.resources);if(closed)return;
   }else{
    if(own(data,'launchSerial')&&(!Number.isSafeInteger(data.launchSerial)||data.launchSerial<1))throw invalid('launchSerial is invalid');
    phase='loading';launchSerial=data.launchSerial??null;
    await hooks.keyboardClear?.();if(closed)return;
    await hook('launch',{config,launchSerial,signal:controller.signal});if(closed)return;phase='running';
   }
   reply(request,true,command==='launch'?{launchSerial}:{});
  }catch(error){if(!closed){phase=before;fail(request,error.errno??'internal-error',error.message??error);}}
  finally{busy=false;}
 }
 function receive(event){
  if(closed||event.source!==parent||event.origin!==parentOrigin)return;
  const value=event.data;
  if(!record(value)||value.protocol!==PROTOCOL||value.game!==GAME||value.epoch!==epoch||typeof value.command!=='string')return;
  if(own(value,'request')&&(typeof value.request!=='string'||value.request.length<1||value.request.length>128))return;
  if(own(value,'request')){
   if(requests.has(value.request)){const response=requests.get(value.request);if(response)post(response);return;}
   if(value.command!=='exit'&&[...requests.values()].filter(row=>row===null).length>=64){post({request:value.request,ok:false,errno:'unavailable',error:'Runtime has too many pending requests'});return;}
   requests.set(value.request,null);
  }
  let data;
  try{data=structuredClone(value);}catch{fail(value.request,'invalid-field','Runtime command cannot be detached');return;}
  void execute(data);
 }
 window.addEventListener('message',receive);window.addEventListener('pagehide',pagehide);
 return {
  get epoch(){return epoch;},get phase(){return phase;},get signal(){return controller.signal;},
  markReady(){if(closed||phase!=='booting')return false;phase='ready';post({event:'ready'});return true;},
  noteStartupPresented(actual){return noteFrame(actual,phase==='loading');},
  notePresented(actual){return noteFrame(actual,phase==='running');},
  requestExit(){return phase==='running'?stop(undefined,true):false;},
  runtimeInfo(value){if(!closed)post({...value,event:'runtime-info'});},
  health(frame,audio){if(closed||!presented)return;post({...frame,event:'frame-health'});if(audio)post({...audio,event:'audio-health'});},
  reportError(error){if(!closed)post({event:'error',message:String(error?.message??error)});},
  dispose(){return stop(undefined,false);}
 };
}
