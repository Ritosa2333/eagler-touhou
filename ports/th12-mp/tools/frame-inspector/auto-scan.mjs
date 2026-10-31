// The native gate compares every tick; browser polling never samples over gaps.
export function createAutoScan({readNative,command,request,onUpdate=()=>{},delay=ms=>new Promise(r=>setTimeout(r,ms)),now=()=>Date.now(),newId=()=>crypto.randomUUID()}){
  let active=false,cancelled=false;
  return {
    running:()=>active,
    async cancel(){if(!active)return;cancelled=true;await command('pause');},
    async start(){
      if(active)throw Error('自动检查已在运行');active=true;cancelled=false;
      let startFrame,lastFrame;
      try{
        let native=await readNative();
        if(native.status!=='paused'||!native.replayActive)throw Error('请先在原版同一份录像中停帧，再开始自动检查');
        startFrame=lastFrame=native.replayCursor;
        await request('seek',{frame:startFrame});
        for(;;){
          if(cancelled){await request('seek',{frame:lastFrame});return {status:'cancelled',startFrame,lastFrame};}
          const batch=await request('scan-batch',{count:240});
          if(cancelled){native=await readNative();lastFrame=native.replayCursor;continue;}
          if(!batch.rows.length)return {status:'ended',startFrame,lastFrame};
          const id=newId(),target=batch.rows.at(-1).replayCursor;
          await command('scan-batch',{batchId:id,...batch});
          let changedAt=now(),observedCursor=lastFrame;
          for(;;){
            native=await readNative();
            if(native.status==='error'||native.status==='closed')throw Error(native.error??'原版已关闭，检查未完成');
            if(native.replayCursor!==observedCursor){changedAt=now();observedCursor=native.replayCursor;}
            const result=native.scan;
            if(cancelled){
              if(native.status==='paused'){lastFrame=native.replayCursor;await request('seek',{frame:lastFrame});return {status:'cancelled',startFrame,lastFrame};}
            }else if(result?.batchId===id){
              lastFrame=result.checkedFrame??lastFrame;
              onUpdate({status:'running',startFrame,lastFrame});
              if(result.status==='error')throw Error(result.error);
              if(result.status==='cancelled'){
                lastFrame=native.replayCursor;await request('seek',{frame:lastFrame});
                return {status:'cancelled',startFrame,lastFrame};
              }
              if(result.status==='diverged'){
                await request('seek',{frame:result.firstDifference.frame});
                return {...result,startFrame,lastFrame:result.firstDifference.frame};
              }
              if(result.status==='batch-complete'&&lastFrame===target)break;
            }else if(result?.status==='error')throw Error(result.error);
            if(now()-changedAt>15000)throw Error('原版15秒没有推进录像帧：请解除游戏自身的Esc暂停后重试');
            await delay(120);
          }
          if(batch.ended)return {status:'ended',startFrame,lastFrame};
        }
      }catch(error){
        // Leave the original at a safe completed-frame boundary on failure.
        await command('pause').catch(()=>{});
        throw error;
      }finally{active=false;}
    }
  };
}
