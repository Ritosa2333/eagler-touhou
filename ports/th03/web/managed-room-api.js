let managedEndTask;
export const managedAPI={
 async start({music}){
  $('start').click();const deadline=performance.now()+180000;
  while(starting){if(performance.now()>deadline)throw Error('梦时空启动超时');await new Promise(resolve=>setTimeout(resolve,50));}
  if(!sync?.running)throw Error($('status').textContent||'梦时空同步尚未启动');
  if(music==='none')localBgm?.setVolume?.(0);
 },
 health(){const frame=sync?.queue.frame??0,confirmed=sync?.queue.confirmed??0;
  Object.assign(globalThis,{__eaglerNetplayLanActive:!!sync?.running,__eaglerNetplayLanFrame:frame,__eaglerNetplayLanConfirmed:confirmed,__eaglerNetplayLanRollback:sync?.costs?.restores??0,__eaglerNetplayLanResimulated:sync?.costs?.replays??0,__eaglerNetplayInputDelayFrames:sync?.queue.inputDelay??0});
  const gameFrame=nativeState&&emulator?.module?.HEAPU8?new DataView(emulator.module.HEAPU8.buffer).getUint32(nativeState.at+nativePause.meta.fields.ticks,true):undefined;
  return {gameFrame,executionThread:emulator?.module?.executionThread??'page',frames:presentationCount,running:!!sync?.running,frame,confirmed,players:nativePause?.markers()??[],audio:{state:emulator?.context?.state??'running'}};
 },
 end(){if(managedEndTask)return managedEndTask;managedEndTask=(async()=>{try{await netplay?.managedTransport?.finish();replayRecorder?.finish('本局结束');await replayRecorder?.sealPublisher();managedAPI.stop();globalThis.__eaglerPC98Emit({event:'exit',code:0});}catch(error){managedAPI.stop();globalThis.__eaglerPC98Emit({event:'error',message:error.message});}})();return managedEndTask;},
 stop(){embeddedRuntime.stop();},
};
