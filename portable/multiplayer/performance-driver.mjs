// Test-only control of the REAL native rAF loop. No JS game tick scheduler,
// remote-input injection, invulnerability, frame skipping or synthetic FPS.
export function startPerformance(api,options={}){
 const {core,Module,app,netStatus,transportStatus,capturePeerInput,memoryStatus,canonical,status,audioStatus,errorDetail,networkError,lifecycle}=api;
 const initial=netStatus(),target=options.targetFrame,seat=options.seat??0;
 if(!Number.isInteger(target)||target<=initial[3]||target-initial[3]>36000)
  throw Error('Invalid bounded performance target');
 const gl=document.getElementById('screen').getContext('webgl2');
 if(!gl)throw Error('Actual WebGL2 context is required');
 const rendererInfo=gl.getExtension('WEBGL_debug_renderer_info');
 const report={done:false,passed:false,scope:'native rAF / actual default-framebuffer blits / wall-clock keyboard',
  initialFrame:initial[3],targetFrame:target,limit60:options.limit60!==false,
  rows:[],presents:[],longTasks:[],initialMemory:memoryStatus(),
  renderer:gl.getParameter(gl.RENDERER),unmaskedRenderer:rendererInfo?gl.getParameter(rendererInfo.UNMASKED_RENDERER_WEBGL):null,
  size:[gl.drawingBufferWidth,gl.drawingBufferHeight]};
 let drawFramebuffer=gl.getParameter(gl.DRAW_FRAMEBUFFER_BINDING);
 const bind=gl.bindFramebuffer,blit=gl.blitFramebuffer,keys=[];
 const start=performance.now();let finished=false,touchPhase=0,touchFrame=-1;
 report.touchTrace=[];
 if(options.directTouchP1&&seat===0){core.sdl_touch_mode(0);core.sdl_touch_options(1,1,1);}
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
  core.sdl_loop_stop();core.sdl_keys_clear();
  if(observer){recordLongTasks(observer.takeRecords());observer.disconnect();}
  gl.bindFramebuffer=bind;gl.blitFramebuffer=blit;
  for(const pointer of keys)core.files_free(pointer);
  report.elapsedMs=performance.now()-start;report.finalNet=netStatus();
  report.finalMemory=memoryStatus();report.transport=transportStatus();
  report.canonical=canonical();report.state=status();report.audio=audioStatus();
  report.lifecycle=lifecycle();report.nativeError=errorDetail();report.networkError=networkError();
  if(core.multiplayer_rollback_storage)report.storage=Array.from(new Float64Array(
   core.memory.buffer,core.multiplayer_rollback_storage(app),9));
  report.error=error||null;report.passed=!error;report.done=true;
 }
 Module.eaglerOptions={...(Module.eaglerOptions||{}),limitPresentationTo60:options.limit60!==false};
 Module.runtimePrepare=()=>{
  try{
   const n=netStatus();
   // A bounded native callback can complete multiple ticks. Stop only at its
   // callback boundary; the runner later uses a separate exact-input fence.
   if(n[3]>=target){finish(null);return 0;}
   const t=performance.now()-start,phase=Math.floor(t/180)+seat;
   if(t<(options.lateStartMs||0))return 0;
   core.sdl_key(keys[0],phase%2===0?1:0);core.sdl_key(keys[1],phase%2===1?1:0);
   core.sdl_key(keys[2],1);core.sdl_key(keys[3],Math.floor(t/730)%2);
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
  const n=netStatus();
  report.rows.push([performance.now()-start,milliseconds,n[3],n[4],core.memory.buffer.byteLength]);
  if(n[3]>=0&&!report.firstForward)report.firstForward={atMs:performance.now()-start,net:n,transport:transportStatus()};
  if(options.directTouchP1&&n[3]>=initial[3]+30&&n[3]<initial[3]+300){
   const players=status();report.touchTrace.push([n[3],players[12],players[13],players[14]]);
   // The one drag above stays in the lower half. A missing input may never
   // create a new target on the top collection line on any remote endpoint.
   if(players[12]===1&&players[14]<20000)finish('Remote absolute touch invented a top-line target: '+JSON.stringify(report.touchTrace.at(-1)));
  }
  if(report.rows.length>72000)finish('Bounded callback history exceeded');
 };
 core.sdl_loop_start(app);
 return {report,stop:()=>finish('Stopped before confirmed target')};
}
