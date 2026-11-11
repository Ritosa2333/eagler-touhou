// Same native runtime and complete checkpoints, with page-owned input/audio.
const send=(message,transfer=[])=>postMessage(message,transfer);
const audioContext={sampleRate:44100,state:'running',currentTime:0,addEventListener(){},removeEventListener(){},resume:async()=>{},close:async()=>{}};
let ready,stopped=false,loaded=false,frameSequence=0,replayMusic;
const transfers=new Set();
// Keep replay work bounded exactly like TH05. The page can keep receiving
// authoritative inputs while this Worker owns a short native batch.
const REPLAY_MAX_STEPS=4,REPLAY_BUDGET_MS=6;
const metrics={captureMs:0,captures:0,stepMs:0,steps:0,replaySteps:0,restoreMs:0,restores:0,confirmMs:0,confirmations:0,hashMs:0,hashes:0,presentMs:0,presentations:0,bitmapMs:0,bitmaps:0,nativeMs:0,nativeSteps:0,nativeMaxMs:0,statusMs:0,queueMs:0,commands:0};
const slowSteps=[];let detailAt=-Infinity,lastGameAt=-Infinity,lastGameStep=-30;
function timed(kind,count,action){const at=performance.now();try{return action();}finally{metrics[kind]+=performance.now()-at;metrics[count]++;}}
globalThis.th02WorkerHost={canvas:new OffscreenCanvas(640,400),audioContext,
 captureAudioState:()=>({music:replayMusic.capture()}),
 restoreAudioState(saved){const events=replayMusic.restore(saved?.music);send({type:'replay-audio-reset',events});},
 recordNative(costMs,frame,replay){metrics.nativeMs+=costMs;metrics.nativeSteps++;metrics.nativeMaxMs=Math.max(metrics.nativeMaxMs,costMs);if(costMs>30){slowSteps.push({frame,replay,costMs});if(slowSteps.length>120)slowSteps.shift();}},
 notify:message=>send({type:'notify',message}),music:event=>{replayMusic?.observe(event);send({type:'music',event});},
 sfx:{load:async()=>{},play:id=>send({type:'sfx',id}),stop(){},dispose(){},info:()=>({ready:true,mode:'confirmed-worker-events'})}
};
function status(force=false){const at=performance.now(),r=th02WorkerHost.runtime;
 const value={proof:r.replayProof(),ready:r.readyForRollback(),reason:r.predictionBlockReason(),touch:r.touchContext()};
 if(th02WorkerHost.replayOnly)value.scene=r.replayScene();
 if(force||metrics.steps-lastGameStep>=30||at-lastGameAt>=1000){value.game=r.snapshot();lastGameStep=metrics.steps;lastGameAt=at;}
 // Expensive display percentile sorting and fault detail are observational;
 // keep them out of every step/confirm RPC, while game policy stays current.
 if(at-detailAt>=250){detailAt=at;Object.assign(value,{rollback:r.rollbackInfo(),render:r.renderInfo(),diagnostics:r.diagnosticText(),fault:r.faultReport(),slowSteps:slowSteps.slice()});}
 metrics.statusMs+=performance.now()-at;value.performance={...metrics};return value;
}
function present(staged=false){
 if(transfers.size>=2)return false;
 if(!timed('presentMs','presentations',()=>th02WorkerHost.runtime.present()))return false;
 const bitmap=timed('bitmapMs','bitmaps',()=>th02WorkerHost.canvas.transferToImageBitmap()),sequence=++frameSequence;
 transfers.add(sequence);send({type:'frame',bitmap,sequence,staged,producedAt:performance.timeOrigin+performance.now()},[bitmap]);return true;
}
let chain=Promise.resolve();
onmessage=({data})=>{
 if(data.type==='frame-ack'){transfers.delete(data.sequence);return;}
 const receivedAt=performance.now();
 chain=chain.then(async()=>{
  if(stopped)return;
  if(!ready)ready=import('./runtime.js');await ready;
  const r=th02WorkerHost.runtime,started=performance.now();metrics.queueMs+=started-receivedAt;metrics.commands++;let result;
  if(data.type==='load'){if(loaded)throw Error('运行实例不能重新加载或更改旁观身份');loaded=true;th02WorkerHost.replayOnly=data.spectator===true;if(th02WorkerHost.replayOnly)replayMusic=(await import('../replay/music-state.js')).createMusicState();r.configure(...data.configure);result=await r.load(data.settings);}
  else if(data.type==='execute'){
   if(data.capture)timed('captureMs','captures',()=>r.capture(data.frame));
   timed('stepMs','steps',()=>r.step(data.inputs,{frame:data.frame,replay:data.replay,offlineMask:data.offlineMask}));if(data.replay)metrics.replaySteps++;
   if(data.hash)result=timed('hashMs','hashes',()=>r.checksum());
   if(data.stagePresentation&&!data.replay)present(true);
  }else if(data.type==='replay-slice'){
   if(!Array.isArray(data.plans)||!data.plans.length||data.plans.length>REPLAY_MAX_STEPS)throw Error('TH02 回滚补算批次无效');
   if(!Number.isSafeInteger(data.frame)||data.frame<0||data.plans.some((plan,index)=>!plan||plan.frame!==data.frame+index||!Array.isArray(plan.inputs)||!Number.isInteger(plan.offlineMask)))throw Error('TH02 回滚补算帧顺序无效');
   const sliceStarted=performance.now(),records=[];let applied=0,blocked=false,exited=false;
   for(const plan of data.plans){
    if(applied&&performance.now()-sliceStarted>=REPLAY_BUDGET_MS)break;
    // The page labels plans that were authoritative at dispatch time. A menu
    // or stage transition can block prediction while this batch is running.
    if(!plan.authoritative&&!r.canPredict()){blocked=true;break;}
    const stepStarted=performance.now();
    if(plan.capture)timed('captureMs','captures',()=>r.capture(plan.frame));
    const outcome=timed('stepMs','steps',()=>r.step(plan.inputs,{frame:plan.frame,replay:!plan.end,offlineMask:plan.offlineMask}));
    if(!plan.end)metrics.replaySteps++;
    const hash=plan.hash?timed('hashMs','hashes',()=>r.checksum()):null;
    records.push({frame:plan.frame,proof:r.replayProof(),hash,ready:hash?r.readyForRollback():undefined,stepMs:performance.now()-stepStarted});applied++;
    if(outcome==='exit'){exited=true;break;}
   }
   if(data.stagePresentation&&applied&&data.plans[applied-1].end)present(true);
   result={applied,records,blocked,exited,sliceMs:performance.now()-sliceStarted};
  }else if(data.type==='present')result=present();
  else if(data.type==='stop'){stopped=true;r.stop();return;}
  else {if(typeof r[data.type]!=='function')throw Error('Unknown TH02 worker command');
   const kinds={capture:['captureMs','captures'],step:['stepMs','steps'],restore:['restoreMs','restores'],confirm:['confirmMs','confirmations'],checksum:['hashMs','hashes']},kind=kinds[data.type];
   result=kind?timed(...kind,()=>r[data.type](...data.args)):await r[data.type](...data.args);
   if(data.type==='step'&&data.args[1]?.replay)metrics.replaySteps++;
  }
  // TH05 sends confirmations in order without waiting for another full state
  // response. Effects still commit only here, on the serial native boundary.
  if(data.type==='confirm'&&data.oneWay){if(result==='exit')send({type:'confirmed-exit'});return;}
  send({type:'result',id:data.id,result,status:status(['load','start'].includes(data.type)),executionMs:performance.now()-started});
 }).catch(error=>send({type:'error',id:data.id,message:error.stack||String(error)}));
};
