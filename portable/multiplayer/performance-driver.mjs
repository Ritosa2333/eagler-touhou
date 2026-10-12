// Test-only control of the REAL native rAF loop. No JS game tick scheduler,
// remote-input injection, invulnerability, frame skipping or synthetic FPS.
export function startPerformance(api,options={}){
 const {core,Module,app,netStatus,transportStatus,capturePeerInput,memoryStatus,canonical,status,audioStatus,errorDetail,networkError,lifecycle}=api;
 const initial=netStatus(),target=options.targetFrame,seat=options.seat??0;
 const touchHz=options.touchMotionHz??0,lateStartMs=options.lateStartMs||0;
 if(!Number.isInteger(target)||target<=initial[3]||target-initial[3]>36000)
  throw Error('Invalid bounded performance target');
 if(!Number.isFinite(touchHz)||touchHz<0||touchHz>240||
    !Number.isFinite(lateStartMs)||lateStartMs<0||
    (touchHz>0&&options.directTouchP1))
  throw Error('Invalid or conflicting wall-clock touch options');
 const gl=document.getElementById('screen').getContext('webgl2');
 if(!gl)throw Error('Actual WebGL2 context is required');
 const rendererInfo=gl.getExtension('WEBGL_debug_renderer_info');
 const report={done:false,passed:false,schemaVersion:2,
  scope:'native rAF / actual default-framebuffer blits / wall-clock device stimulus',
  coverage:{input:'direct native device API; not the managed shell or full Launcher message path',
   presentation:'default-framebuffer blit observations, not final object motion or physical scanout',
   motionContinuity:'not measured: no final-pixel or stable presented-object export used'},
  initialFrame:initial[3],initialSessionWords:initial.slice(7,9),targetFrame:target,limit60:options.limit60!==false,
  rowSchema:['callbackEndMs','nativeCallbackMs','nativeLastSimulatedFrame','confirmedThrough','heapBytes',
   'observedFrameHighWater','nativeNextFrame','rollbackRequestFrame','nativeRecoveryYields',
   'nativeCorrecting','nativeCorrectionEnd','nativePredictionFrontier','nativeMissingInputs','nativeIntervalScaleMilli',
   'nativeHighRefresh','nativeInterpolate','nativeAlpha','nativePresented','nativePresentationPrimed'],
  progressScope:'high-water of actually observed native LastSimulatedFrame at callback ends; not a new native counter',
  rows:[],presents:[],longTasks:[],sessionIdentityChanges:[],initialMemory:memoryStatus(),
  renderer:gl.getParameter(gl.RENDERER),unmaskedRenderer:rendererInfo?gl.getParameter(rendererInfo.UNMASKED_RENDERER_WEBGL):null,
  size:[gl.drawingBufferWidth,gl.drawingBufferHeight]};
 const smoothnessStatus=()=>core.multiplayer_smoothness_status?Array.from(new Int32Array(
  core.memory.buffer,core.multiplayer_smoothness_status(app),10)):null;
 const presentationStatus=()=>core.multiplayer_presentation_status?Array.from(new Float64Array(
  core.memory.buffer,core.multiplayer_presentation_status(),6)):null;
 report.smoothnessStatusSchema=['schema','recoveryYields','correcting','correctionEnd','predictionFrontier',
  'missingInputs','intervalScaleMilli','forwardAdvanced','correctedPresentationPending','localCaptureAtNext'];
 report.initialSmoothness=smoothnessStatus();
 report.presentationStatusSchema=['schema','highRefresh','interpolate','alpha','presented','primed'];
 let drawFramebuffer=gl.getParameter(gl.DRAW_FRAMEBUFFER_BINDING);
 const bind=gl.bindFramebuffer,blit=gl.blitFramebuffer,keys=[];
 const start=performance.now();let finished=false,touchPhase=0,touchFrame=-1,observedFrame=initial[3];
 report.startedAtPerformanceMs=start; // Same-realm anchor for impairment timestamps only.
 let touchTimer=null,nextTouchIndex=0,touchActive=false;
 const touchPeriod=touchHz>0?1000/touchHz:0,touchId=191+seat;
 const motion=report.touchMotion={enabled:touchHz>0,targetHz:touchHz,startMs:lateStartMs,
  scope:'wall-clock normalized pointer path through native TouchController -> absolute DirectTarget input',
  latePolicy:'coalesce overdue pointer positions to the latest planned position; never replay a burst',
  unlimited:true,plannedEvents:0,accountedEvents:0,submittedEvents:0,coalescedEvents:0,
  skippedNotReadyEvents:0,unservicedAtStop:0,timerCallbacks:0,begins:0,moves:0,cancels:0,
  timerLatenessTotalMs:0,maxTimerLatenessMs:0,latestTargetAgeTotalMs:0,maxLatestTargetAgeMs:0,
  firstSubmittedMs:null,lastSubmittedMs:null,traceLimit:24000,traceDropped:0,
  traceSchema:['firstPlanIndex','lastPlanIndex','firstPlannedMs','lastPlannedMs','dispatchMs',
   'timerLatenessMs','latestTargetAgeMs','plannedInBatch','action','pointerX','pointerY',
   'nativeLastSimulatedFrame','localPlayerState','nativeSubmissionMs'],
  actions:{0:'not-ready',1:'begin',2:'move',3:'cancel-not-ready'},trace:[]};
 report.touchTrace=[];
 if(touchHz>0||(options.directTouchP1&&seat===0)){core.sdl_touch_mode(0);core.sdl_touch_options(1,1,1);}
 const recordLongTasks=entries=>{
  for(const entry of entries){
   if(report.longTasks.length>=256)break;
   report.longTasks.push({startMs:entry.startTime-start,durationMs:entry.duration,name:entry.name,
    attribution:Array.from(entry.attribution||[],a=>({name:a.name,type:a.containerType}))});
  }
 };
 const observer=typeof PerformanceObserver!=='undefined'&&PerformanceObserver.supportedEntryTypes.includes('longtask')
  ?new PerformanceObserver(list=>recordLongTasks(list.getEntries())):null;
 observer?.observe({type:'longtask'});
 for(const code of ['ArrowLeft','ArrowRight','KeyZ','ShiftLeft']){
  const bytes=new TextEncoder().encode(code+'\0'),pointer=core.files_allocate(bytes.length);
  new Uint8Array(core.memory.buffer,pointer,bytes.length).set(bytes);keys.push(pointer);
 }
 gl.bindFramebuffer=function(target,value){
  if(target===gl.FRAMEBUFFER||target===gl.DRAW_FRAMEBUFFER)drawFramebuffer=value;
  return bind.call(this,target,value);
 };
 gl.blitFramebuffer=function(...args){
  const result=blit.apply(this,args);
  if(drawFramebuffer===null)report.presents.push(performance.now()-start);
  return result;
 };
 function finish(error){
  if(finished)return;finished=true;
  if(touchTimer!==null){clearTimeout(touchTimer);touchTimer=null;}
  if(touchHz>0){
   motion.stopMs=performance.now()-start;
   motion.plannedEvents=motion.stopMs<lateStartMs?0:Math.floor((motion.stopMs-lateStartMs)/touchPeriod+1.e-9)+1;
   motion.accountedEvents=nextTouchIndex;
   motion.unservicedAtStop=Math.max(0,motion.plannedEvents-motion.accountedEvents);
   if(touchActive){++motion.cancels;touchActive=false;}
  }
  core.sdl_loop_stop();core.sdl_keys_clear();
  if(observer){recordLongTasks(observer.takeRecords());observer.disconnect();}
  gl.bindFramebuffer=bind;gl.blitFramebuffer=blit;
  for(const pointer of keys)core.files_free(pointer);
  report.elapsedMs=performance.now()-start;report.finalNet=netStatus();
  report.finalMemory=memoryStatus();report.transport=transportStatus();
  report.finalSmoothness=smoothnessStatus();
  report.finalPresentation=presentationStatus();
  report.canonical=canonical();report.state=status();report.audio=audioStatus();
  report.lifecycle=lifecycle();report.nativeError=errorDetail();report.networkError=networkError();
  if(core.multiplayer_rollback_storage)report.storage=Array.from(new Float64Array(
   core.memory.buffer,core.multiplayer_rollback_storage(app),9));
  report.error=error||null;report.passed=!error;report.done=true;
 }
 function scheduleTouch(){
  if(finished)return;
  const due=start+lateStartMs+nextTouchIndex*touchPeriod;
  touchTimer=setTimeout(pumpTouch,Math.max(0,Math.ceil(due-performance.now())));
 }
 function pumpTouch(){
  touchTimer=null;if(finished)return;
  try{
   const now=performance.now()-start;
   const lastIndex=Math.floor((now-lateStartMs)/touchPeriod+1.e-9);
   if(lastIndex<nextTouchIndex){scheduleTouch();return;}
   const firstIndex=nextTouchIndex,count=lastIndex-firstIndex+1;
   const firstPlanned=lateStartMs+firstIndex*touchPeriod,lastPlanned=lateStartMs+lastIndex*touchPeriod;
   // Lateness belongs to the FIRST overdue deadline. Measuring only the latest
   // coalesced position would hide a long event-loop stall behind <1 period.
   const lateness=Math.max(0,now-firstPlanned),targetAge=Math.max(0,now-lastPlanned);
   const angle=lastIndex*touchPeriod/2400*Math.PI*2+seat*.83;
   const x=.5+.18*Math.sin(angle),y=.5+.075*Math.sin(angle*.73+seat*.4);
   const n=netStatus(),players=status(),playerState=players[12+seat*12];
   const ready=!!n[9]&&!!players[3]&&!players[4]&&!players[5]&&playerState===1;
   ++motion.timerCallbacks;motion.timerLatenessTotalMs+=lateness;
   motion.maxTimerLatenessMs=Math.max(motion.maxTimerLatenessMs,lateness);
   motion.latestTargetAgeTotalMs+=targetAge;motion.maxLatestTargetAgeMs=Math.max(motion.maxLatestTargetAgeMs,targetAge);
   let action=0;const submissionStarted=performance.now();
   if(ready){
    action=touchActive?2:1;core.sdl_touch(touchActive?1:0,touchId,x,y);touchActive=true;
    ++motion.submittedEvents;motion.coalescedEvents+=count-1;
    if(action===1)++motion.begins;else ++motion.moves;
    if(motion.firstSubmittedMs===null)motion.firstSubmittedMs=now;motion.lastSubmittedMs=now;
   }else{
    motion.skippedNotReadyEvents+=count;
    if(touchActive){core.sdl_touch_cancel();touchActive=false;++motion.cancels;action=3;}
   }
   const row=[firstIndex,lastIndex,firstPlanned,lastPlanned,now,lateness,targetAge,count,action,x,y,
    n[3],playerState,performance.now()-submissionStarted];
   if(motion.trace.length<motion.traceLimit)motion.trace.push(row);else ++motion.traceDropped;
   motion.lastEvent=row;nextTouchIndex=lastIndex+1;motion.accountedEvents=nextTouchIndex;
   motion.plannedEvents=nextTouchIndex;scheduleTouch();
  }catch(error){finish('Wall-clock touch producer failed: '+String(error));}
 }
 Module.eaglerOptions={...(Module.eaglerOptions||{}),limitPresentationTo60:options.limit60!==false};
 Module.runtimePrepare=()=>{
  try{
   const n=netStatus();
   // A bounded native callback can complete multiple ticks. Stop only at its
   // callback boundary; the runner later uses a separate exact-input fence.
   if(n[3]>=target&&!smoothnessStatus()?.[2])return 0;
   const t=performance.now()-start,phase=Math.floor(t/180)+seat;
   if(t<lateStartMs)return 0;
   core.sdl_key(keys[0],phase%2===0?1:0);core.sdl_key(keys[1],phase%2===1?1:0);
   core.sdl_key(keys[2],1);core.sdl_key(keys[3],Math.floor(t/730)%2);
   if(touchHz>0){
    // Arrow input would cancel the native drag. Movement is produced only by
    // the independent wall-clock timer, never by this prepare callback.
    core.sdl_key(keys[0],0);core.sdl_key(keys[1],0);core.sdl_key(keys[3],0);
   }
   if(options.directTouchP1&&seat===0){
    core.sdl_key(keys[0],0);core.sdl_key(keys[1],0);core.sdl_key(keys[3],0);
    const players=status();
    if(n[9]&&players[12]===1){
     if(touchPhase===0){core.sdl_touch(0,91,.5,.5);touchPhase=1;touchFrame=n[3];}
     else if(touchPhase===1&&n[3]>touchFrame){core.sdl_touch(1,91,.58,.47);touchPhase=2;}
    }
   }
   return 1;
  }catch(error){finish(String(error));return 0;}
 };
 Module.runtimeFinish=(result,milliseconds)=>{
  if(result||core.application_error(app)){finish('Native loop failed: '+result+' / '+core.application_error(app));return;}
  const n=netStatus(),now=performance.now()-start;
  observedFrame=Math.max(observedFrame,n[3]);
  const smooth=smoothnessStatus(),present=presentationStatus();
  report.rows.push([now,milliseconds,n[3],n[4],core.memory.buffer.byteLength,observedFrame,n[2],n[5],
   ...(smooth?smooth.slice(1,7):[null,null,null,null,null,null]),
   ...(present?present.slice(1):[null,null,null,null,null])]);
  if((n[7]!==initial[7]||n[8]!==initial[8])&&report.sessionIdentityChanges.length<16)
   report.sessionIdentityChanges.push({atMs:now,sessionWords:n.slice(7,9)});
  if(n[3]>=0&&!report.firstForward)report.firstForward={atMs:now,net:n,transport:transportStatus()};
  if(options.directTouchP1&&n[3]>=initial[3]+30&&n[3]<initial[3]+300){
   const players=status();report.touchTrace.push([n[3],players[12],players[13],players[14]]);
   // The one drag above stays in the lower half. A missing input may never
   // create a new target on the top collection line on any remote endpoint.
   if(players[12]===1&&players[14]<20000)finish('Remote absolute touch invented a top-line target: '+JSON.stringify(report.touchTrace.at(-1)));
  }
  if(report.rows.length>72000)finish('Bounded callback history exceeded');
  // End at the actual callback boundary, after its final presentation. An
  // extra prepare can decline another tick without stopping beneath C++.
  if(n[3]>=target&&!smooth?.[2])finish(null);
 };
 if(touchHz>0)scheduleTouch();
 core.sdl_loop_start(app);
 return {report,stop:()=>finish('Stopped before confirmed target')};
}
