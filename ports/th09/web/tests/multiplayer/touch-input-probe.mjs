// Synthetic native-ABI touch stimulus. No DOM/Launcher bridge or physical phone claim.
// Event deadlines use Page performance.now, never game callbacks, captures or F.
export const TOUCH_INPUT_SCHEMA='th09-touch-input-v1';

export function buildTouchInputSchedule({profile='direct',sourceHz=60,durationMs=120000,side=0,pattern='lifecycle'}={}){
 if(!['direct','joystick'].includes(profile)||!['lifecycle','continuous'].includes(pattern)||![60,120].includes(sourceHz)||![0,1].includes(side)||
    !Number.isFinite(durationMs)||durationMs<6000||durationMs>120000)throw Error('Invalid touch source configuration');
 const events=[],startMs=2500+137*side,periodMs=5200;
 const add=(atMs,kind,details={})=>{if(atMs<=durationMs)events.push({atMs,kind,...details,order:events.length});};
 add(0,'key',{scan:29,down:1,label:'dialogue-skip'});add(2200,'key',{scan:29,down:0,label:'dialogue-skip-end'});
 const move=(a,b,from,to,epoch,label)=>{
  const emit=t=>{const u=(t-a)/(b-a),x=from[0]+(to[0]-from[0])*u,y=from[1]+(to[1]-from[1])*u;
   add(t,profile==='direct'?'pointer':'stick',{type:1,id:71+side,x,y,epoch,label,coalesceKey:epoch+':'+label});};
  for(let k=Math.floor(a*sourceHz/1000)+1;k*1000/sourceHz<b-1e-7;++k)emit(k*1000/sourceHz);
  emit(b);
 };
 if(pattern==='continuous'){
  const legMs=1200,route=[0,1,2,3,0,3,2,1,0],epoch=1;
  const points=profile==='direct'?[[.42,.64],[.58,.64],[.58,.76],[.42,.76]]:
   [[.65,-.3],[.3,.65],[-.65,.3],[-.3,-.65]];
  const point=(at,xy,label,type=1)=>add(at,profile==='direct'?'pointer':'stick',{
   ...(profile==='direct'?{type,id:71+side}:{}),x:xy[0],y:xy[1],epoch,label});
  let endpoint=points[0];
  for(let leg=0;startMs+leg*legMs<durationMs;++leg){
   const t=startMs+leg*legMs,i=leg%(route.length-1),from=points[route[i]],to=points[route[i+1]];
   const fire=leg%4===3?0:1,focus=leg%2;
   add(t,'controls',{fire,focus,label:fire?(focus?'fire-focus':'fire-fast'):'fire-off'});
   // Native controls clear stick axes. Reassert the boundary point immediately,
   // outside MOVE coalescing, without lifting or creating another gesture.
   point(t,from,leg===0?'contact':'turn',leg===0?0:1);
   move(t,t+legMs,from,to,epoch,'continuous-leg-'+leg);
   const u=Math.min(1,(durationMs-t)/legMs);
   endpoint=from.map((value,axis)=>value+(to[axis]-value)*u);
  }
  // Finish a partial grid interval at its actual interpolated position. Do not
  // accelerate the last leg or leave a held gesture/vector after its deadline.
  if(events.at(-1).atMs<durationMs)point(durationMs,endpoint,'end-move');
  point(durationMs,profile==='direct'?endpoint:[0,0],'end-up',2);
  add(durationMs,'cancel',{epoch,label:'end-cancel'});
  add(durationMs,'controls',{fire:0,focus:0,label:'end-neutral'});
  events.sort((a,b)=>a.atMs-b.atMs||a.order-b.order);
  if(events.length>20000)throw Error('Touch schedule exceeds bounded event capacity');
  return {schema:TOUCH_INPUT_SCHEMA,profile,sourceHz,durationMs,side,startMs,periodMs:legMs*(route.length-1),sensitivity:1.5,
   pattern,cadence:'Absolute Page-clock grid throughout one held gesture; leg/control edges and terminal neutral retain their own deadlines.',
   events:events.map(({order,...e},sequence)=>({sequence,...e}))};
 }
 for(let cycle=0;startMs+cycle*periodMs<=durationMs;++cycle){
  const t=startMs+cycle*periodMs,epoch=cycle*3+1;
  const contact=(at,e)=>add(at,profile==='direct'?'pointer':'stick',profile==='direct'?
   {type:0,id:71+side,x:.5,y:.7,epoch:e,label:'contact'}:{x:.65,y:-.3,epoch:e,label:'contact'});
  const release=(at,e,label,x=.43,y=.7)=>add(at,profile==='direct'?'pointer':'stick',profile==='direct'?
   {type:2,id:71+side,x,y,epoch:e,label}:{x:0,y:0,epoch:e,label});
  add(t,'controls',{fire:1,focus:0,label:'fire-fast'});contact(t,epoch);
  move(t,t+1200,profile==='direct'?[.5,.7]:[.65,-.3],profile==='direct'?[.58,.66]:[.3,.65],epoch,'drag');
  add(t+1200,'controls',{fire:1,focus:1,label:'fire-focus'});
  move(t+1200,t+2100,profile==='direct'?[.58,.66]:[.3,.65],profile==='direct'?[.43,.7]:[-.65,-.3],epoch,'reverse');
  add(t+2100,'marker',{epoch,label:profile==='direct'?'hold-stationary-finger':'hold-stick-vector'});
  release(t+2600,epoch,'up');add(t+2600,'controls',{fire:1,focus:0,label:'fire-fast'});
  contact(t+2950,epoch+1);
  move(t+2950,t+4050,profile==='direct'?[.5,.7]:[.65,-.3],profile==='direct'?[.42,.64]:[-.5,.55],epoch+1,'recontact-drag');
  add(t+4050,'cancel',{epoch:epoch+1,label:'cancel'});
  add(t+4350,'controls',{fire:1,focus:1,label:'fire-focus'});contact(t+4350,epoch+2);
  move(t+4350,t+5050,profile==='direct'?[.5,.7]:[.65,-.3],profile==='direct'?[.56,.72]:[.25,-.7],epoch+2,'recontact-after-cancel');
  release(t+5050,epoch+2,'up',.56,.72);add(t+5050,'controls',{fire:1,focus:0,label:'fire-fast'});
 }
 events.sort((a,b)=>a.atMs-b.atMs||a.order-b.order);
 if(events.length>20000)throw Error('Touch schedule exceeds bounded event capacity');
 return {schema:TOUCH_INPUT_SCHEMA,profile,sourceHz,durationMs,side,startMs,periodMs,sensitivity:1.5,
  cadence:'Absolute Page-clock grid during moving legs; contact/control/release/hold edges retain their own deadlines.',
  events:events.map(({order,...e},sequence)=>({sequence,...e}))};
}

// This function is serialized into a Page by Playwright: keep it self-contained.
export function installTouchInputProbe({enabled=true,plan}={}){
 if(!enabled)return;
 if(!plan||plan.schema!=='th09-touch-input-v1'||!Array.isArray(plan.events)||plan.events.length>20000)
  throw Error('Invalid touch probe plan');
 const limits={events:20000,captures:24000,positions:24000,errors:32,coalescedRanges:4096,commandsPerWake:64};
 const attempts=new Map(),byFrame=new Map(),captures=[],positions=[],errors=[],coalesced=[],pending=[];
 const overflow={errors:0,captures:0,positions:0,coalescedRanges:0},events=plan.events;
 const nextGroupEnd=new Uint32Array(events.length);
 for(let i=events.length-1;i>=0;--i)nextGroupEnd[i]=events[i].coalesceKey&&events[i+1]?.coalesceKey===events[i].coalesceKey?nextGroupEnd[i+1]:i+1;
 let core=null,startedAt=null,endedAt=null,timer=null,running=false,nextEvent=0,pendingHead=0;
 let firstResponseFrame=null,nextWireFrame=0,frontier=0,lastPresentation=0,lastMove=null,lastControls=null,lastKey=null;
 let packets=0,duplicates=0,changedHistory=0,captureGaps=0,rawCalls=0,wakes=0,maxWakeCommands=0,sourceExhausted=false;
 let analogActiveFrames=0,targetChanges=0,newFrontierCallbacks=0,newPresentedCallbacks=0,forwardFrames=0,positionChanges=0;
 const age=t=>startedAt===null?null:t-startedAt;
 const note=text=>{if(errors.length<limits.errors)errors.push(text);else ++overflow.errors;};
 const distribution=values=>{const a=values.filter(Number.isFinite).sort((a,b)=>a-b);return {count:a.length,
  p50:a[Math.floor(a.length*.5)]??null,p95:a[Math.floor(a.length*.95)]??null,p99:a[Math.floor(a.length*.99)]??null,max:a.at(-1)??null};};
 const dueEnd=ms=>{let lo=0,hi=events.length;while(lo<hi){const m=(lo+hi)>>>1;if(events[m].atMs<=ms)lo=m+1;else hi=m;}return lo;};
 const stop=(now=performance.now())=>{if(startedAt===null||endedAt!==null)return;clearTimeout(timer);timer=null;running=false;endedAt=now;};
 const apply=e=>{
  const row={sequence:e.sequence,submittedAtMs:age(performance.now()),submissionCompletedAtMs:null,
   captureFrame:null,firstWireAtMs:null,presentationObservedAtMs:null,presentationFrontier:null,
   unchangedWireObservations:0,association:null,controlsSequenceAtSubmit:lastControls,touchStateBefore:null,touchStateAfter:null};
  const movement=['pointer','stick','cancel'].includes(e.kind);
  const readTouchState=()=>{const p=core._th09_touch_state()/4;return Array.from(core.HEAP32.subarray(p,p+4));};
  if(movement)row.touchStateBefore=readTouchState();
  attempts.set(e.sequence,row);
  if(e.kind==='key'){core._th09_key(e.scan,e.down);lastKey=e.sequence;++rawCalls;}
  else if(e.kind==='controls'){core._th09_touch_controls(1,e.fire,e.focus,0,0);lastControls=e.sequence;++rawCalls;}
  else if(e.kind==='pointer'){core._th09_touch(e.type,e.id,e.x,e.y);lastMove=e.sequence;++rawCalls;}
  else if(e.kind==='stick'){core._th09_touch_stick(e.x*32767,e.y*32767);lastMove=e.sequence;++rawCalls;}
  else if(e.kind==='cancel'){if(plan.profile==='joystick'){core._th09_touch_stick(0,0);++rawCalls;}core._th09_touch_cancel();lastMove=e.sequence;++rawCalls;}
  else if(e.kind==='marker')lastMove=e.sequence;
  else throw Error('Unknown touch event kind');
  if(movement)row.touchStateAfter=readTouchState();
  row.submissionCompletedAtMs=age(performance.now());
 };
 const wake=()=>{
  if(!running)return;++wakes;timer=null;
  try{
   const end=dueEnd(age(performance.now()));let commands=0;
   while(nextEvent<end){
    let index=nextEvent;
    // Coalesce only adjacent MOVE rows of the same gesture/leg. Lifecycle,
    // focus and direction-leg boundaries are never silently replayed as MOVE.
    if(events[index].coalesceKey){
     const groupEnd=Math.min(end,nextGroupEnd[index]);
     if(groupEnd-index>1){
      if(coalesced.length>=limits.coalescedRanges){++overflow.coalescedRanges;note('Coalesced range capacity exceeded');stop();return;}
      coalesced.push({firstSequence:index,lastSequence:groupEnd-2,observedAtMs:age(performance.now()),reason:'source-timer-late-same-gesture-moves'});
     }
     index=groupEnd-1;
    }
    if(commands>=limits.commandsPerWake){note('Source wake command cap exceeded; remaining due intents were not submitted');stop();return;}
    apply(events[index]);nextEvent=index+1;++commands;
   }
   maxWakeCommands=Math.max(maxWakeCommands,commands);
   if(nextEvent<events.length)timer=setTimeout(wake,Math.max(0,startedAt+events[nextEvent].atMs-performance.now()));
   else {sourceExhausted=true;running=false;}
  }catch(error){note(String(error?.stack||error));stop();}
 };
 window.touchInputProbe={
  start(runtime,side){
   if(startedAt!==null)throw Error('Touch probe is single-session');
   if(side!==plan.side||!runtime||!['_th09_touch','_th09_touch_cancel','_th09_touch_controls','_th09_touch_options',
      '_th09_touch_stick','_th09_touch_state','_th09_touch_probe','_th09_key','_th09_rollback_info','_th09_adonis_info','_th09_game_metrics']
      .every(name=>typeof runtime[name]==='function'))throw Error('Touch probe requires public TH09 native ABI');
   core=runtime;const at=core._th09_rollback_info()/4,ad=core._th09_adonis_info()/4;
   if(core.HEAPU32[ad]!==0||core.HEAPU32[ad+1]!==0)throw Error('Touch wall-clock lane requires rollback mode 0, D0');
   firstResponseFrame=core.HEAPU32[at+4];frontier=core.HEAPU32[at];
   lastPresentation=core.HEAPU32[core._th09_game_metrics()/4+9];
   core._th09_touch_cancel();core._th09_touch_stick(0,0);
   for(const scan of [29,44,42,203,205])core._th09_key(scan,0);
   core._th09_touch_options(1,plan.profile==='direct'?0:3,plan.sensitivity,0,0);
   core._th09_touch_controls(1,0,0,0,0);
   startedAt=performance.now();running=true;wake();
   return {startedAt,firstResponseFrame,mode:0,inputDelay:0};
  },
  onLocalInput(data,now=performance.now()){
   let bytes;
   if(ArrayBuffer.isView(data))bytes=new Uint8Array(data.buffer,data.byteOffset,data.byteLength);
   else if(data instanceof ArrayBuffer)bytes=new Uint8Array(data);
   else {note('Local input observation is not bytes');return;}
   if(bytes[0]===0xe7)bytes=bytes.subarray(2);
   if(bytes.length<6||bytes[0]!==69||bytes[5]!==1)return;
   if(bytes.length<44||bytes[1]!==57||bytes[2]!==78||bytes[3]!==80||bytes[4]!==4||
      bytes[6]!==plan.side||bytes[42]>32||bytes.length!==44+12*bytes[42]){note('Local touch packet v4 schema/seat mismatch');return;}
   ++packets;const view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength),first=view.getUint32(32,true);
   for(let i=0;i<bytes[42];++i){
    const frame=first+i,at=44+i*12,word0=view.getUint32(at,true),xBits=view.getUint32(at+4,true),yBits=view.getUint32(at+8,true);
    const prior=byFrame.get(frame);
    if(prior){++duplicates;if(prior.word0!==word0||prior.xBits!==xBits||prior.yBits!==yBits){++changedHistory;note('Full 12-byte local history changed at frame '+frame);}continue;}
    if(captures.length>=limits.captures){++overflow.captures;note('Touch capture capacity exceeded');return;}
    const rawHex=Array.from(bytes.subarray(at,at+12),x=>x.toString(16).padStart(2,'0')).join('');
    if(frame!==nextWireFrame){++captureGaps;note('Local tape gap/order at '+frame+' expected '+nextWireFrame);}
    nextWireFrame=Math.max(nextWireFrame,frame+1);
    const buttons=view.getUint16(at,true),analogMode=bytes[at+2],flags=bytes[at+3],x=view.getFloat32(at+4,true),y=view.getFloat32(at+8,true);
    if(analogMode>2||(flags&~7)||!Number.isFinite(x)||!Number.isFinite(y))note('Invalid native touch wire sample');
    const previous=captures.at(-1),motionChanged=!!previous&&(previous.analogMode!==analogMode||previous.flags!==flags||
     previous.xBits!==xBits||previous.yBits!==yBits);
    const targetChanged=!!previous&&analogMode!==0&&previous.analogMode===analogMode&&
     (previous.xBits!==xBits||previous.yBits!==yBits);
    const record={frame,buttons,analogMode,flags,x,y,word0,xBits,yBits,rawHex,
     observedAt:now,firstWireAtMs:age(now),latestMoveSequence:lastMove,latestControlsSequence:lastControls,latestKeySequence:lastKey,
     analogActive:analogMode!==0,motionChanged,targetChanged,associatedSequence:null};
    captures.push(record);byFrame.set(frame,record);if(analogMode!==0)++analogActiveFrames;if(targetChanged)++targetChanges;
    if(startedAt===null||endedAt!==null||frame<firstResponseFrame||lastMove===null)continue;
    const event=events[lastMove],attempt=attempts.get(lastMove);
    if(!attempt||attempt.captureFrame!==null||event.kind==='marker')continue;
    const expectsNone=event.kind==='cancel'||event.kind==='pointer'&&event.type===2||event.kind==='stick'&&event.x===0&&event.y===0;
    const modeMatches=expectsNone?analogMode===0:analogMode===(plan.profile==='direct'?2:1)&&!!(flags&2);
    if(modeMatches&&motionChanged){
     attempt.captureFrame=frame;attempt.firstWireAtMs=age(now);
     attempt.association={method:'latest-submitted-movement-event-before-changed-native-wire',
      readyGameplayAtSubmit:attempt.touchStateBefore?.[0]===1&&!!attempt.touchStateBefore?.[1],
      controlsChangedSinceSubmit:attempt.controlsSequenceAtSubmit!==lastControls,
      causalEffectVerified:false};
     record.associatedSequence=lastMove;pending.push(attempt);
    }else ++attempt.unchangedWireObservations;
   }
  },
  onFrame(stats,presentationCount,phase,now=performance.now()){
   if(startedAt===null||endedAt!==null)return;
   if(!stats||stats.length<8||!Number.isInteger(stats[0])||!Number.isFinite(presentationCount)){note('Invalid native frame observation');return;}
   const next=stats[0],fresh=next>frontier,rendered=presentationCount>lastPresentation;
   if(presentationCount<lastPresentation)note('Presentation counter regressed');
   if(fresh){
    ++newFrontierCallbacks;forwardFrames+=next-frontier;if(rendered)++newPresentedCallbacks;
    if(positions.length<limits.positions){
     const at=core._th09_touch_probe()/4,st=core._th09_touch_state()/4;
     const values=Array.from(new Float32Array(core.HEAPU8.buffer,at*4,8)),state=Array.from(core.HEAP32.subarray(st,st+4));
     const prior=positions.at(-1);if(prior&&(prior.values[0]!==values[0]||prior.values[1]!==values[1]))++positionChanges;
     positions.push({frontier:next,observedAtMs:age(now),presentationCount,rendered,phase,values,touchState:state,latestMoveSequence:lastMove});
    }else ++overflow.positions;
    if(rendered)while(pendingHead<pending.length&&pending[pendingHead].captureFrame<next){
     const attempt=pending[pendingHead++];attempt.presentationObservedAtMs=age(now);attempt.presentationFrontier=next;
    }
   }
   frontier=Math.max(frontier,next);lastPresentation=presentationCount;
  },
  stop,
  snapshot(){
   const end=endedAt??(startedAt===null?null:performance.now()),count=end===null?0:dueEnd(age(end));
   let range=0;
   const rows=events.slice(0,count).map(e=>{
    while(range<coalesced.length&&coalesced[range].lastSequence<e.sequence)++range;
    const omitted=coalesced[range]&&coalesced[range].firstSequence<=e.sequence&&coalesced[range].lastSequence>=e.sequence;
    const a=attempts.get(e.sequence),status=a?(e.kind==='marker'?'marker-observed':a.presentationObservedAtMs!==null?'render-progress-after-conditional-wire':
     a.captureFrame!==null?'captured-not-presented':'submitted-without-attributable-changed-wire'):omitted?'coalesced-before-submit':'not-submitted';
    return {...e,...(a||{}),status};
   });
   const futureEvents=events.slice(count).map(e=>({...e,status:'future-censored-not-due'}));
   const counts={plannedEvents:events.length,dueEvents:rows.length,futureCensoredEvents:futureEvents.length,
    plannedNativeEvents:events.filter(e=>e.kind!=='marker').length,dueNativeEvents:rows.filter(e=>e.kind!=='marker').length,
    submittedEvents:rows.filter(e=>e.submittedAtMs!==undefined).length,rawNativeCalls:rawCalls,
    coalescedMoves:rows.filter(e=>e.status==='coalesced-before-submit').length,notSubmitted:rows.filter(e=>e.status==='not-submitted').length,
    attributedCaptures:rows.filter(e=>e.captureFrame!==undefined&&e.captureFrame!==null).length,
    renderProgressObservedEvents:rows.filter(e=>e.status==='render-progress-after-conditional-wire').length,
    submittedWithoutAttributableChangedWire:rows.filter(e=>e.status==='submitted-without-attributable-changed-wire').length,
    packets,duplicates,changedHistory,captureGaps,actualInputFrames:captures.length,analogActiveFrames,targetChanges,
    newFrontierCallbacks,newPresentedCallbacks,forwardFrames,positionChangedObservations:positionChanges};
   const presented=rows.filter(e=>e.status==='render-progress-after-conditional-wire');
   return {schema:'th09-touch-input-v1',enabled:true,valid:errors.length===0&&!Object.values(overflow).some(Boolean),
    scope:'Synthetic native ABI, Page-clock deadlines; no DOM/Launcher/iOS bridge, physical phone, hardware-input or photon timing.',
    clock:{name:'Page performance.now',timeOrigin:performance.timeOrigin,startedAt,endedAt:end},
    configuration:{...plan,events:undefined},limits,overflow,errors,counts,
    wireObservation:{boundary:'Serialized native local input first observed at the transport send-intent hook, before original send and configured client delay/drop.',
     timing:'firstWireAtMs is this observation time. It is not successful send API completion, network delivery, or the exact native capture time.'},
    source:{wakes,maxWakeCommands,sourceExhausted,configuredEvents:events.length,coalescedRanges:coalesced,
     clockIndependence:'setTimeout absolute deadlines; never scheduled from game frame, capture, frontier or transport progress.',
     latenessMs:distribution(rows.filter(e=>e.submittedAtMs!==undefined).map(e=>e.submittedAtMs-e.atMs))},
    response:{method:'Conditional render-progress proxy: latest submitted movement event, first subsequent changed native movement wire, then first newly advanced frontier with renderer submission. This does not establish visible movement or per-event gameplay effect.',
     confounding:'Focus can change joystick velocity; death/context changes can write neutral input; clipping, overwritten moves and target convergence can produce no visible movement. First wire observation can follow a capture delayed by backpressure: ordering it after an event does not prove capture followed that event. The submission touch state and changed-controls marker are metadata, not proof of causality. Normalized finger coordinates are not the absolute wire target.',
     denominator:'All configured planned events remain in events + futureEvents. dueEvents and futureCensoredEvents are separate. Coalesced, blocked, clipped, unchanged and unsubmitted events cannot be inferred successful. Hold markers do not manufacture a wire edge.',
     plannedToRenderProgressMs:distribution(presented.map(e=>e.presentationObservedAtMs-e.atMs)),
     submitToFirstWireMs:distribution(presented.map(e=>e.firstWireAtMs-e.submittedAtMs)),
     firstWireToRenderProgressMs:distribution(presented.map(e=>e.presentationObservedAtMs-e.firstWireAtMs))},
    events:rows,futureEvents,captures,positions,positionSchema:['x','y','velocityX','velocityY','motionEnabled','targetMode','motionX','motionY'],
    touchStateSchema:['context','ready','activeGesture','inTitle'],firstResponseFrame,frontier};
  }
 };
}

