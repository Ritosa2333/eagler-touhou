const SCHEMA='th08/replay-capture-journal/1';
const digest=async bytes=>Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)))
  .map(value=>value.toString(16).padStart(2,'0')).join('');

/** One native step owns both its trace decision and the successful-consumption counters. */
export function createReplayCaptureJournal(controller){
  const {core,runtime}=controller;
  const state={applicationTicks:0,capturedRows:0,previous:null,started:false,done:false,stopReason:null,
    terminalScene:null,stages:[],finalScore:0,filters:{},tail:[]};
  let rows=[],ledger=[];
  const tick=()=>{
    const receipt=controller.driver.advanceOneTick();state.applicationTicks++;
    const pointer=core.trace(runtime.app),u32=new Uint32Array(core.memory.buffer,pointer,68),f32=new Float32Array(core.memory.buffer,pointer,68);
    const diagnostics=Array.from(new Int32Array(core.memory.buffer,core.diagnostics(runtime.app),16));
    const scene=core.status(runtime.app,0),counters=Array.from({length:7},(_,index)=>core.status(runtime.app,12+index));
    const replay={loaded:counters[0],frame:counters[1],inputCursor:counters[2],inputBytes:counters[3],
      timingCursor:counters[4],timingBytes:counters[5],stride:counters[6]};
    const sample={applicationTick:state.applicationTicks,advanced:receipt.status==='advanced',scene,stage:u32[1],
      traceFrame:u32[0],flags:u32[4],replay,captured:false,rowIndex:null,reason:null};
    const key=`${u32[1]}:${u32[0]}`;
    sample.reason=!sample.advanced?'driver-not-advanced':diagnostics[0]!==2||diagnostics[1]!==2?'owner-transition':
      (u32[4]&12)!==12?'inactive-replay':!u32[0]?'zero-trace-frame':key===state.previous?'duplicate-trace-frame':null;
    if(!sample.reason){
      sample.captured=true;sample.rowIndex=state.capturedRows++;state.previous=key;
      if(!state.stages.includes(u32[1]))state.stages.push(u32[1]);
      rows.push({applicationTick:state.applicationTicks,replay:{...replay},frame:u32[0],stage:u32[1],gameFrames:u32[2],controlFrames:u32[3],
        flags:u32[4],rng:u32[5],rngCalls:u32[6],player:[f32[7],f32[8]],playerState:u32[9],keys:u32[10],
        numbers:Array.from(new Uint8Array(core.memory.buffer,pointer+44,228))});
    }else state.filters[sample.reason]=(state.filters[sample.reason]??0)+1;
    ledger.push(sample);
    if(scene===2){state.finalScore=u32[13];if((u32[4]&8)!==0)state.started=true;}
    state.tail.push({...sample,receipt,trace:Array.from(u32.slice(0,14)),diagnostics});
    if(state.tail.length>32)state.tail.shift();
    if(state.started&&scene===1){state.done=true;state.stopReason='returned-to-title';state.terminalScene=scene;}
    else if(!sample.advanced){state.done=true;state.stopReason='driver-not-advanced';state.terminalScene=scene;}
  };
  const step=count=>{for(let index=0;index<count&&!state.done;index++)tick();};
  return{state,step,drain(){const batch={...state,rows,ledger,journal:{schema:SCHEMA,applicationTicks:state.applicationTicks,
    capturedRows:state.capturedRows,filters:{...state.filters}}};rows=[];ledger=[];return batch;}};
}

/** Reviewable cursor invariants, shared by the browser capture and its independent reader. */
export function createReplayLedgerInspector(){
  let applicationTick=0,capturedRows=0,previous=null,activeSamples=0;
  const problems=[];
  return{push(sample,row){
    const fail=reason=>{if(problems.length<16)problems.push({applicationTick:sample?.applicationTick,reason});};
    if(sample?.applicationTick!==++applicationTick)fail('Application tick sequence is missing, duplicated or reordered');
    if(sample?.captured){
      if(sample.rowIndex!==capturedRows++)fail('Captured row sequence is not contiguous');
      if(!row||row.applicationTick!==sample.applicationTick||row.stage!==sample.stage||row.frame!==sample.traceFrame||
        row.flags!==sample.flags||JSON.stringify(row.replay)!==JSON.stringify(sample.replay))fail('Trace row does not match its native cursor receipt');
      if(!sample.advanced||sample.reason!==null)fail('An unadvanced or filtered application step claimed a trace row');
    }
    const r=sample?.replay;
    if(sample?.scene!==2||(sample.flags&12)!==12||r?.loaded!==1){previous=null;return;}
    activeSamples++;
    if(!['frame','inputCursor','inputBytes','timingCursor','timingBytes','stride'].every(key=>Number.isSafeInteger(r[key])&&r[key]>=0)||
      ![2,6].includes(r.stride)||r.inputBytes<2||r.timingBytes<2||r.inputCursor!==r.frame*r.stride||
      r.inputCursor>r.inputBytes+r.stride-2||r.timingCursor!==Math.ceil(r.frame/30)||r.timingCursor>=r.timingBytes)
      fail('Replay cursor, stride or successful-consumption boundary is inconsistent');
    if(previous?.stage===sample.stage){
      const before=previous.replay,delta=r.frame-before.frame;
      // ReplayPlayback::after_update restarts Chain inside one application
      // update: dialogue batches end at frame % 3 == 2, fast mode at % 5 == 4.
      // These are native simulation steps, not missing application receipts.
      if(delta<0||delta>5||sample.traceFrame-previous.traceFrame!==delta||
        r.stride!==before.stride||r.inputBytes!==before.inputBytes||r.timingBytes!==before.timingBytes)
        fail('Replay consumption skipped, moved backwards or changed its selected block');
    }
    previous=sample;
  },finish(metadata){
    if(metadata?.schema!==SCHEMA||metadata.applicationTicks!==applicationTick||metadata.capturedRows!==capturedRows||!activeSamples)
      problems.push({reason:'Capture journal metadata does not match its observed rows'});
    return{schema:SCHEMA,status:problems.length?'incomplete':'passed',applicationTicks:applicationTick,capturedRows,activeSamples,problems};
  }};
}

export async function bootReplayCapture({presentationLab,fixture,bytes,startStage=0}){
  if(bytes===undefined){const response=await fetch(`/input/demo${fixture}.rpy`);if(!response.ok)throw Error('Missing named Replay fixture');
    bytes=Array.from(new Uint8Array(await response.arrayBuffer()));}
  const controller=await presentationLab.boot({replayBytes:bytes});controller.freeze();
  const {core,runtime}=controller,journal=createReplayCaptureJournal(controller);
  const press=(code,wait=40)=>{controller.key(code,true);journal.step(3);controller.key(code,false);journal.step(wait);};
  for(let index=0;index<12&&core.status(runtime.app,9)!==4;index++)press('ArrowDown');
  if(core.status(runtime.app,9)!==4)throw Error('Replay menu not reached');
  if(fixture===undefined){
    press('KeyZ');press('KeyZ');
    for(let index=0;index<10&&core.status(runtime.app,9)!==startStage;index++)press('ArrowDown');
    if(core.status(runtime.app,9)!==startStage)throw Error('Requested Replay stage is absent');
  }
  const navigation=[];
  for(let index=0;index<5&&core.status(runtime.app,0)!==2;index++){
    press('KeyZ');navigation.push([core.status(runtime.app,0),core.status(runtime.app,8),core.status(runtime.app,9)]);
  }
  if(core.status(runtime.app,0)!==2||(new Uint32Array(core.memory.buffer,core.trace(runtime.app),5)[4]&8)===0)
    throw Error('Real Replay playback did not start: '+JSON.stringify(navigation));
  controller.clearKeys();
  globalThis.replayVerifierCapture={run(count){journal.step(count);return journal.drain();}};
  return{...journal.drain(),navigation,replay:{bytes:bytes.length,sha256:await digest(new Uint8Array(bytes))},build:controller.identity};
}
