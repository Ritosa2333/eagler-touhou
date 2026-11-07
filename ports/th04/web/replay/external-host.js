import {sha256} from './sha256.js';
// Browser-only clock and input host for ordinary single-player emulators.
export const HOST_VERSION='pc98-external-jspi/1';
export const CLOCK_BYTES=9,CLOCK_CHUNK=4096;
const RealDate=Date,realPerformance=globalThis.performance;
const realNow=()=>realPerformance.now();

export async function inspectExternalMemory(module,{scope,patch}={}){
  let bytes=module.HEAPU8;
  if(scope==='pc98-guest-ram/1'){
    const sig=new TextEncoder().encode(patch.signature),view=new DataView(bytes.buffer),matches=[],active=[];
    const mailbox=patch.entries?.mailbox??patch.mailbox_cs_offset,csSegment=patch.codeSegment??patch.code_segment,dsSegment=patch.dataSegment??patch.data_segment;
    const residentName=new TextEncoder().encode(patch.signature.startsWith('TH02')?'MIKOConfig\0':'HUMAConfig\0'),residentOffset=patch.signature.startsWith('TH02')?0x27fc:0xba86;
    for(let at=bytes.indexOf(sig[0]);at>=0;at=bytes.indexOf(sig[0],at+1)){
      if(at+36>bytes.length||!sig.every((v,i)=>bytes[at+i]===v))continue;
      const ds=view.getUint16(at+20,true),cs=ds-dsSegment+csSegment,base=at-mailbox-cs*16,data=base+ds*16;
      if(cs<=0||base<0||base+0x100000>bytes.length||data<base||data+residentOffset+4>base+0xa0000)continue;
      const ptr=base+view.getUint16(data+residentOffset,true)+view.getUint16(data+residentOffset+2,true)*16;
      if(ptr>=base&&ptr+residentName.length<base+0xa0000&&residentName.every((v,i)=>bytes[ptr+i]===v)){matches.push(base);if(bytes[at+22]===1)active.push(base);}
    }
    const candidates=active.length===1?active:matches;if(candidates.length!==1)throw Error('Replay 游戏内存定位失败');bytes=bytes.subarray(candidates[0],candidates[0]+0x100000);
  }
  bytes=bytes.slice();let hash=2166136261;
  // Snapshot first, then yield between chunks so the result menu can paint
  // while the exact final-state checksum is being calculated.
  for(let start=0;start<bytes.length;start+=1024*1024){
    await new Promise(resolve=>setTimeout(resolve,0));
    const end=Math.min(start+1024*1024,bytes.length);
    for(let i=start;i<end;i++)hash=Math.imul(hash^bytes[i],16777619);
  }
  const memorySha256=await sha256(bytes);
  return {memoryHash:(hash>>>0).toString(16).padStart(8,'0'),memorySha256,memoryBytes:bytes.length,...(scope?{memoryScope:scope}:{})};
}

export function createExternalReplayHost({mode,clocks,events=[],endBoundary,endReason='sleep',endPhase='after-input',timeline,boundaryMode='sleep',onChunk=()=>{},onEnd=()=>{},onError=()=>{},
  onRecordingError=onError,applyInput=()=>{},inspect,observe,maxClockBytes=512*1024*1024,clock,onBoundary=()=>{},shouldHold,shouldFastForward=()=>false}={}){
  if(!['record','playback'].includes(mode)||!['sleep','clock'].includes(boundaryMode))throw Error('无效的单人 Replay 主机配置');
  let module=null,boundary=0,count=0,eventCursor=0,queued=[],buffer=new Uint8Array(CLOCK_BYTES*CLOCK_CHUNK),used=0;
  let stopped=false,ending=false,held=false,wake=null,timer=null,lastWake=0,recording=mode==='record',finished=false,extraClockReads=0,detached=false;
  let runStart=null,runStage=0,runReason='manual',pauseStart=null,skipCursor=0,fastYield=realNow(),sealedHold=false,manualFast=false,waitForInactive=false;
  const skipRanges=[];
  const source=clocks?new DataView(clocks.buffer,clocks.byteOffset,clocks.byteLength):null;
  const started=realNow();
  function releaseSeal(){if(sealedHold){sealedHold=false;held=false;if(wake)timer=setTimeout(resume,0);}}
  function disable(error){if(!recording)return;recording=false;ending=false;used=0;releaseSeal();try{onRecordingError?.(error);}catch{}}
  function emit(value){try{onChunk(value);}catch(error){disable(error);}}
  function flush(){if(!used)return;const bytes=buffer.slice(0,used);used=0;emit({clocks:bytes});}
  function recordCommands(commands,at){if(recording&&commands.length)emit({events:[{boundary:at,commands}]});}
  function apply(commands){for(const command of commands)applyInput(command);}
  function consumeClockEvents(){
    while(events[eventCursor]?.boundary===count){const row=events[eventCursor++];apply(row.commands);}
    if(events[eventCursor]?.boundary<count)throw Error('Replay 输入边界失步');
  }
  function closePause(){if(pauseStart!==null){if(boundary>pauseStart)skipRanges.push({from:pauseStart,to:boundary});pauseStart=null;}}
  function finishFailed(error){
    if(mode==='record'){
      used=0;recording=false;ending=false;releaseSeal();
      try{onRecordingError?.(error);}catch{}
      return;
    }
    stopped=true;clearTimeout(timer);
    try{onError(error);}catch{}
  }
  function finish(reason=boundaryMode,phase='after-input'){
    if(finished||stopped||mode==='record'&&!recording)return;finished=true;ending=false;
    if(mode==='playback')stopped=true;
    flush();if(mode==='record'&&!recording)return;
    recording=false;
    if(mode==='playback'&&(count*CLOCK_BYTES!==source.byteLength||eventCursor!==events.length||extraClockReads))throw Error('Replay 结束时仍有未消费的记录');
    closePause();
    const result={boundary:boundaryMode==='clock'?count:boundary,clockCount:count,endReason:reason,endPhase:mode==='playback'?endPhase:phase,elapsedMs:realNow()-started,stopped,extraClockReads,
      ...(mode==='record'&&runStart!==null?{timeline:{startBoundary:runStart,skipRanges,reason:runReason,stage:runStage}}:{})};
    let snapshot;
    try{snapshot=inspect?.();}catch(error){finishFailed(error);return;}
    Promise.resolve(snapshot).then(extra=>{releaseSeal();return onEnd({...result,...extra});}).catch(finishFailed);
  }
  function time(kind){
    if(detached)return kind===1?realPerformance.now():RealDate.now();
    if(stopped){extraClockReads++;return mode==='playback'&&source?.byteLength>=CLOCK_BYTES?source.getFloat64(source.byteLength-CLOCK_BYTES+1,true):0;}
    if(mode==='playback'&&boundaryMode==='clock')consumeClockEvents();
    let value;
    if(mode==='record'){
      value=kind===1?(clock?clock.now():realPerformance.now()):(clock?clock.date():RealDate.now());
      if(recording){
        if((count+1)*CLOCK_BYTES>maxClockBytes)disable(Error('Replay 时钟超过容量，录像停止；游戏继续'));
        else{const view=new DataView(buffer.buffer);view.setUint8(used,kind);view.setFloat64(used+1,value,true);used+=CLOCK_BYTES;if(used===buffer.length)flush();}
      }
    }else{
      const offset=count*CLOCK_BYTES;
      if(offset+CLOCK_BYTES>source.byteLength){extraClockReads++;value=source.getFloat64(source.byteLength-CLOCK_BYTES+1,true);}
      else{
        if(source.getUint8(offset)!==kind)throw Error(`Replay 时钟失步：读取 ${count}`);
        value=source.getFloat64(offset+1,true);
        if(!Number.isFinite(value)||value<0)throw Error('Replay 时钟值无效');
      }
    }
    if(kind===2&&epoch===undefined)epoch=value;
    if(mode==='playback'||recording)count++;
    if(mode==='playback'&&boundaryMode==='clock'&&count===endBoundary){consumeClockEvents();finish();}
    return value;
  }
  let epoch;
  function HostDate(...args){if(!new.target)return new RealDate(time(2)).toString();return Reflect.construct(RealDate,args.length?args:[time(2)],new.target);}
  Object.setPrototypeOf(HostDate,RealDate);HostDate.prototype=RealDate.prototype;HostDate.now=()=>time(2);
  const hostPerformance=new Proxy(realPerformance,{get(target,name){if(name==='now')return ()=>time(1);const value=Reflect.get(target,name,target);return typeof value==='function'?value.bind(target):value;}});
  function isFast(){
    if(held||stopped)return false;
    if(mode==='record')return shouldFastForward();
    if(manualFast)return true;
    const ranges=timeline?.skipRanges||[];
    while(ranges[skipCursor]&&boundary>=ranges[skipCursor].to)skipCursor++;
    return !!(ranges[skipCursor]&&boundary>=ranges[skipCursor].from&&boundary<ranges[skipCursor].to);
  }
  function observeRun(){
    if(!recording||!observe)return false;
    const state=observe();if(waitForInactive){if(!(state?.currentActive??state?.active))waitForInactive=false;return false;}if(!state)return false;
    if((state.currentActive??state.active)&&runStart===null){runStart=boundary;if(boundary>1)skipRanges.push({from:1,to:boundary});}
    if(runStart===null)return false;
    runStage=state.stage??runStage;
    if(state.paused&&pauseStart===null)pauseStart=boundary;
    if(!state.paused)closePause();
    if(skipRanges.length>1024)throw Error('Replay 暂停次数超过容量');
    if(!state.finish)return false;
    runReason=state.finish;sealedHold=true;held=true;finish('sleep','before-input');return true;
  }
  function resume(){
    if(stopped||!wake)return;clearTimeout(timer);
    try{
      // A viewer pause must not consume input while the adapter is paused.
      // Ending a paused recording captures the current boundary without
      // delivering commands that the DOS instance has not consumed yet.
      if(held){if(ending)finish(boundaryMode,'before-input');return;}
      if(detached){const next=wake;wake=null;next();return;}
      onBoundary();
      // Seal before input delivery: a queued continue/confirmation belongs to
      // the live game after this run, never to its replay.
      if(mode==='record'){
        try{if(observeRun())return;}catch(error){disable(error);}
      }else if(endPhase==='before-input'&&endReason!=='exit'&&boundary===endBoundary){finish();return;}
      // Park only after observing native entry pause, before delivering input.
      if(mode==='record'&&shouldHold?.()){held=true;return;}
      if(mode==='record'){
        const batch=queued;queued=[];apply(batch);recordCommands(batch,boundary);
      }else{
        if(events[eventCursor]?.boundary<boundary)throw Error('Replay 输入边界失步');
        while(events[eventCursor]?.boundary===boundary)apply(events[eventCursor++].commands);
      }
      if(ending||mode==='playback'&&endReason!=='exit'&&boundary===endBoundary){finish();if(mode==='playback')return;}
      const next=wake;wake=null;
      if(mode==='playback'&&boundary>endBoundary)throw Error('Replay 超出结束边界');
      next();
    }catch(error){stopped=true;onError(error);}
  }
  function initialize(){
    module.alive=true;module.sleep_count=module.nonskippable_sleep_count=module.sleep_time=0;
    lastWake=time(2);module.last_wakeup=lastWake;module.sync_sleep=()=>{};
    module.destroyAsyncify=()=>{
      if(!detached&&!finished){
        try{
          if(mode==='record'||endReason==='exit')finish('exit');
          else onError(Error('Replay 在结束边界前退出'));
        }catch(error){onError(error);}
      }
      stopped=true;clearTimeout(timer);wake=null;module.alive=false;
    };
    module.uncaughtAsyncify=onError;return 1;
  }
  return {
    Date:HostDate,performance:hostPerformance,
    keyTime:()=>time(2)-epoch,
    attach(target,imports){
      module=target;
      for(const namespace of Object.values(imports)){
        if(namespace.initMessageSyncSleep)namespace.initMessageSyncSleep=initialize;
        if(namespace.initTimeoutSyncSleep)namespace.initTimeoutSyncSleep=initialize;
        if(namespace.__asyncjs__syncSleep)namespace.__asyncjs__syncSleep=new WebAssembly.Suspending(async(ms,nonSkippable)=>{
          const now=time(2);if(!nonSkippable&&now-lastWake<24)return;
          module.sleep_count++;module.localFrameCount=(module.localFrameCount||0)+1;if(module.nativeBurstStarted!==undefined)module.observeLocalFrame?.(realNow()-module.nativeBurstStarted);if(nonSkippable)module.nonskippable_sleep_count++;
          await new Promise(resolve=>{
            boundary++;wake=resolve;
            // Consume the unchanged tape during skips. Yield regularly so
            // browser pause/stop controls remain responsive.
            if(isFast()&&realNow()-fastYield<8)queueMicrotask(resume);
            else{fastYield=realNow();timer=setTimeout(resume,isFast()?0:nonSkippable?Math.max(0,ms)/(clock?.speed||1):0);}
          });
          lastWake=now;module.last_wakeup=now;module.nativeBurstStarted=realNow();
        });
      }
    },
    queue(command){
      if(stopped||mode==='playback')return;
      const copy=structuredClone(command);
      if(boundaryMode==='clock'){try{apply([copy]);recordCommands([copy],count);}catch(error){disable(error);}}
      else queued.push(copy);
    },
    finish(){
      if(mode!=='record'||stopped||!recording)return;
      ending=true;
      if(boundaryMode==='clock')finish();
      else if(wake)resume();
    },
    rearm(){if(mode!=='record'||stopped||!finished)throw Error('Replay 尚未结束');finished=false;recording=true;ending=false;runStart=null;runStage=0;runReason='manual';pauseStart=null;skipRanges.length=0;waitForInactive=true;releaseSeal();},
    disable(){recording=false;ending=false;used=0;releaseSeal();},
    // Let the ordinary exit request finish even if playback held its last
    // JSPI continuation. No tape or proof work survives disposal.
    dispose(){detached=true;recording=false;ending=false;used=0;queued=[];held=false;stopped=false;if(wake)timer=setTimeout(resume,0);},
    pause(value){if(!value&&sealedHold)return;held=!!value;if(!held&&wake)timer=setTimeout(resume,0);},
    fastForward(value){manualFast=!!value;},
    get boundary(){return boundaryMode==='clock'?count:boundary;},
    get status(){return {mode,boundaryMode,boundary,count,stopped,held,recording,finished,extraClockReads,fastForward:isFast()};},
  };
}
