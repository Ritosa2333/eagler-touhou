// Explicit diagnostic only. Every field is callback-local and published before
// runtimeFinish. The independent ABI never changes the old 19/25-word row.
export const callbackWorkPhases=Object.freeze([
 'forwardCapture','historyCapture','forwardUpdate','historyUpdate',
 'forwardSemanticDraw','historySemanticDraw','restore','confirmedCommit'
]);
export const callbackWorkTimingSchema=Object.freeze([
 'schema','enabled','valid','sequence','epoch','owner','generation',
 'frameBefore','frameAfter','exitReason','nativeInputAndStepMs','residualMs',
 'forwardElidedCalls','historyElidedCalls','diagnosticFlags','reserved',
 ...callbackWorkPhases.flatMap(name=>[name+'Calls',name+'Ms'])
]);

export function readCallbackWorkTiming(core,enabled=false){
 if(typeof enabled!=='boolean')throw TypeError('Callback work timing enabled must be boolean');
 if(!enabled)return null;
 const getter=core.sdl_callback_work_timing_status;
 if(typeof getter!=='function')throw Error('Callback work timing status export required');
 const pointer=getter();
 // Acquire the current buffer AFTER the native getter, even if memory grew.
 const buffer=core.memory.buffer,bytes=callbackWorkTimingSchema.length*8;
 if(!Number.isSafeInteger(pointer)||pointer<=0||pointer%8||
    bytes>buffer.byteLength||pointer>buffer.byteLength-bytes)
  throw Error('Callback work timing status pointer is outside current memory');
 const row=Array.from(new Float64Array(buffer,pointer,callbackWorkTimingSchema.length));
 if(row[0]!==1||row.some(value=>!Number.isFinite(value)))
  throw Error('Unsupported or non-finite callback work timing row');
 // Invalid diagnostic rows are evidence. Preserve their flags and residual;
 // the post-run analyzer rejects them, rather than making them legal zeroes.
 return row;
}

export function configureCallbackWorkTiming(core,{enabled=false,parentEnabled=false}={}){
 if(typeof enabled!=='boolean'||typeof parentEnabled!=='boolean')
  throw TypeError('Callback work timing options must be boolean');
 if(!enabled)return null;
 if(!parentEnabled)throw Error('Callback work timing requires the original callback timing');
 if(typeof core.sdl_callback_work_timing_set_enabled!=='function')
  throw Error('Callback work timing control export required');
 if(core.sdl_callback_work_timing_set_enabled(1)!==1)
  throw Error('Callback work timing option rejected');
 return readCallbackWorkTiming(core,true);
}

function recordObservationError(report,stage,error){
 if(report.callbackWorkTimingError)return;
 let message;
 try{message=String(error?.message??error).slice(0,512);}
 catch{message='Unprintable callback work timing error';}
 report.callbackWorkTimingError={stage,message};
}

// During a running measurement, malformed observation is a failed report,
// never a fabricated zero row or an exception that strands runtimeFinish.
export function observeCallbackWorkTiming(core,enabled,report,stage){
 if(typeof enabled!=='boolean')throw TypeError('Callback work timing enabled must be boolean');
 if(!enabled||report.callbackWorkTimingError)return null;
 try{return readCallbackWorkTiming(core,true);}
 catch(error){recordObservationError(report,stage,error);return null;}
}

export function stopCallbackWorkTiming(core,enabled,report){
 if(typeof enabled!=='boolean')throw TypeError('Callback work timing enabled must be boolean');
 if(!enabled)return;
 report.finalCallbackWorkTiming=observeCallbackWorkTiming(core,true,report,'finish');
 try{
  report.callbackWorkTimingDisableResult=core.sdl_callback_work_timing_set_enabled(0);
  if(report.callbackWorkTimingDisableResult!==0)throw Error('Callback work timing disable rejected');
 }catch(error){recordObservationError(report,'disable',error);}
}
