import {startReplayStage,replayInputEnded} from './replay-session.mjs';

function endReason(core){
  const hud=new Float32Array(core.HEAPU8.buffer,core._th12_hud(),core._th12_hud_size());
  return hud[13]?(hud[13]===2?'game-over':'stage-ended'):null;
}

// Cursor is the next unconsumed recorded input, not the core's logical clock.
// Stage resources must already be loaded, as for ordinary replay startup.
// onTick is synchronous and observes the state after that recorded input.
// With restart:false, the caller guarantees core already matches fromCursor.
export function createReplaySeeker(core,{batchTicks=240,
  yieldTask=()=>new Promise(resolve=>setTimeout(resolve,0)),onReset,onTick}={}){
  if(!Number.isSafeInteger(batchTicks)||batchTicks<1)throw RangeError('回放跳转分批大小无效');
  if(typeof yieldTask!=='function'||[onReset,onTick].some(callback=>callback!==undefined&&typeof callback!=='function'))
    throw TypeError('回放跳转回调无效');
  let serial=0;
  return {
    cancel(){serial++;},
    async seek(replay,stage,target,{onProgress,fromCursor=0,restart=true}={}){
      if(!stage||!Number.isSafeInteger(stage.frames)||stage.frames<1||
        !Number.isFinite(target)||!Number.isFinite(fromCursor)||typeof restart!=='boolean'||
        typeof replay?.input!=='function')throw RangeError('回放跳转位置无效');
      if(onProgress!==undefined&&typeof onProgress!=='function')throw TypeError('回放跳转进度回调无效');
      target=Math.max(0,Math.min(stage.frames,Math.floor(target)));
      fromCursor=Math.max(0,Math.min(stage.frames,Math.floor(fromCursor)));
      if(!restart&&target<fromCursor)throw RangeError('向前播放不能倒退回放位置');
      const request=++serial;
      let cursor=restart?0:fromCursor,previous=cursor>0?(replay.input(stage.number,cursor-1)?.held??0):0;
      const result=reason=>({cursor,target,previous,reason});
      const progress=()=>onProgress?.({cursor,target});
      if(restart){startReplayStage(core,replay,stage);onReset?.(core);}
      progress();
      for(;;){
        if(request!==serial)return result('cancelled');
        const ended=endReason(core);
        if(ended)return result(ended);
        if(cursor>=stage.frames)return result('replay-input-ended');
        if(cursor>=target)return result('target-reached');
        const limit=Math.min(target,cursor+batchTicks);
        while(cursor<limit){
          if(request!==serial)return result('cancelled');
          const input=replay.input(stage.number,cursor);
          if(replayInputEnded(input))return result('replay-input-ended');
          core._th12_tick(input.held,input.pressed);
          previous=input.held;cursor++;
          onTick?.(core,cursor,input);
          if(request!==serial)return result('cancelled');
          const ended=endReason(core);
          if(ended){progress();return result(ended);}
        }
        progress();
        if(cursor<target)await yieldTask();
      }
    }
  };
}
