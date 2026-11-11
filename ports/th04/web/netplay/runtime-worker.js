// Shared TH04 runtime, with serial native operations and page-owned audio.
const send=(message,transfer=[])=>postMessage(message,transfer);
const audioContext={sampleRate:44100,state:'running',currentTime:0,
 addEventListener(){},removeEventListener(){},resume:async()=>{},close:async()=>{}};
let ready,stopped=false,loaded=false,frameSequence=0,sequence=0,played=0,replayMusic;
const transfers=new Set();
const REPLAY_MAX_STEPS=4,REPLAY_BUDGET_MS=6;
globalThis.th04WorkerHost={canvas:new OffscreenCanvas(640,400),audioContext,
 captureAudioState:()=>({sequence,played,music:replayMusic.capture()}),restoreAudioState(saved){const events=replayMusic.restore(saved?.music);sequence=saved.sequence;played=saved.played;send({type:'replay-audio-reset',events,sequence});},
 notify:message=>{
  if(message.event==='runtime-error')try{message.fault=th04WorkerHost.runtime?.faultReport();}catch{/* Preserve the original failure. */}
  send({type:'notify',message});
 },music:event=>{replayMusic?.observe(event);send({type:'music',event});},
 se:{load:async()=>{},confirm(event){
  const count=(event.sequence-sequence)&65535;
  if(count>8)throw Error('音效事件超过桥接队列容量');
  for(let i=0;i<count;i++){const id=event.ring[(sequence+i)&7];if(!Number.isInteger(id)||id<0||id>=17)throw Error('原生音效编号无效');}
  sequence=event.sequence;played+=count;send({type:'se',event});
 },stop:()=>send({type:'se-stop'}),dispose(){},
 info:()=>({sequence,played,status:'浏览器音效：确认事件交给页面',loaded:17})}
};
let lastDiagnostics=-Infinity,lastGameAt=-Infinity,lastGameStep=-30,simulatedSteps=0;
function status(force=false){
 const r=th04WorkerHost.runtime,now=performance.now();
 // Control, prediction and replay proof must follow every completed command.
 // UI-only diagnostics match the room's 250 ms refresh, including during replay.
 const state={proof:r.replayProof(),ready:r.readyForRollback(),reason:r.predictionBlockReason(),touch:r.touchContext()};
 if(th04WorkerHost.replayOnly)state.scene=r.replayScene();
 if(force||simulatedSteps-lastGameStep>=30||now-lastGameAt>=1000){state.game=r.snapshot();lastGameStep=simulatedSteps;lastGameAt=now;}
 if(force||now-lastDiagnostics>=250){
  Object.assign(state,{rollback:r.rollbackInfo(),render:r.renderInfo(),diagnostics:r.diagnosticText(),fault:r.faultReport({cpu:false})});
  lastDiagnostics=now;
 }
 return state;
}
function present(staged=false){
 if(transfers.size>=2)return false;
 if(!th04WorkerHost.runtime.present())return false;
 const bitmap=th04WorkerHost.canvas.transferToImageBitmap(),sequence=++frameSequence;
 transfers.add(sequence);send({type:'frame',bitmap,sequence,staged,producedAt:performance.timeOrigin+performance.now()},[bitmap]);return true;
}
let chain=Promise.resolve();
onmessage=({data})=>{
 if(data.type==='frame-ack'){transfers.delete(data.sequence);return;}
 chain=chain.then(async()=>{
  if(stopped)return;
  if(!ready)ready=import('./runtime.js');await ready;
  const r=th04WorkerHost.runtime,started=performance.now();let result;
  if(data.type==='load'){if(loaded)throw Error('运行实例不能重新加载或更改旁观身份');loaded=true;th04WorkerHost.replayOnly=data.spectator===true;if(th04WorkerHost.replayOnly)replayMusic=(await import('../replay/music-state.js')).createMusicState();r.configure(...data.configure);result=await r.load(data.settings);}
  else if(data.type==='execute'){
   if(data.capture)r.capture(data.frame);
   r.step(data.inputs,{frame:data.frame,replay:data.replay,offlineMask:data.offlineMask});
   simulatedSteps++;
   if(data.hash)result=r.checksum();
   if(data.stagePresentation&&!data.replay)present(true);
  }else if(data.type==='replay-slice'){
   if(!Array.isArray(data.plans)||!data.plans.length||data.plans.length>REPLAY_MAX_STEPS)throw Error('TH04 回滚补算批次无效');
   if(!Number.isSafeInteger(data.frame)||data.frame<0||data.plans.some((plan,index)=>!plan||plan.frame!==data.frame+index||!Array.isArray(plan.inputs)||!Number.isInteger(plan.offlineMask)))throw Error('TH04 回滚补算帧顺序无效');
   const sliceStarted=performance.now(),records=[];let applied=0,blocked=false,exited=false;
   for(const plan of data.plans){
    if(applied&&performance.now()-sliceStarted>=REPLAY_BUDGET_MS)break;
    if(!plan.authoritative&&!r.canPredict()){blocked=true;break;}
    const stepStarted=performance.now();
    if(plan.capture)r.capture(plan.frame);
    const outcome=r.step(plan.inputs,{frame:plan.frame,replay:!plan.end,offlineMask:plan.offlineMask});
    simulatedSteps++;
    const hash=plan.hash?r.checksum():null;
    records.push({frame:plan.frame,proof:r.replayProof(),hash,ready:hash?r.readyForRollback():undefined,stepMs:performance.now()-stepStarted});applied++;
    if(outcome==='exit'){exited=true;break;}
   }
   if(data.stagePresentation&&applied&&data.plans[applied-1].end)present(true);
   result={applied,records,blocked,exited,sliceMs:performance.now()-sliceStarted};
  }else if(data.type==='present')result=present();
  else if(data.type==='stop'){stopped=true;r.stop();return;}
  else {if(typeof r[data.type]!=='function')throw Error('Unknown TH04 worker command');result=await r[data.type](...data.args);if(data.type==='step')simulatedSteps++;}
  if(data.type==='confirm'&&data.oneWay){if(result==='exit')send({type:'confirmed-exit'});return;}
  send({type:'result',id:data.id,result,status:status(['load','start','replayLoad','faultReport'].includes(data.type)),executionMs:performance.now()-started});
 }).catch(error=>{
  stopped=true;let fault;try{fault=th04WorkerHost.runtime?.faultReport();}catch{/* Preserve the command failure. */}
  send({type:'error',id:data.id,message:error.stack||String(error),fault});
 });
};
