// One native score request may serve concurrent autosave / Host sync callers.
// Replay bytes and files are persisted only after the native request completes.
export const isScoreSaveCancelled=error=>error?.code==='EAGLER_SCORE_SAVE_CANCELLED';
export function createScoreSave({request,poll,cancel,error,flushReplay,sync,
  now=()=>performance.now(),schedule=(fn,ms)=>setTimeout(fn,ms),unschedule=id=>clearTimeout(id),
  timeoutMs=15000,pollMs=50}){
  let pending=null;
  function failure(message){return Error(error?.()||message);}
  function cancelled(message){const why=Error(message);why.code='EAGLER_SCORE_SAVE_CANCELLED';return why;}
  function finish(holder,why){
    if(pending!==holder)return;
    if(holder.timer!==null)unschedule(holder.timer);pending=null;
    if(why)holder.reject(why);else holder.resolve();
  }
  function abandon(holder,why){
    if(holder.token&&holder.phase!=='sync'){
      try{cancel(holder.token);}catch(failure){if(!isScoreSaveCancelled(why))why=failure;}
    }
    finish(holder,why);
  }
  function persist(holder){
    holder.phase='sync';
    Promise.resolve().then(()=>{if(pending!==holder)return;flushReplay();return sync();})
      .then(()=>finish(holder),why=>finish(holder,why));
  }
  function check(holder){
    if(pending!==holder)return;holder.timer=null;
    try{
      const status=poll(holder.token);
      if(status===1){persist(holder);return;}
      if(status===-2)throw cancelled('Score save request belongs to a previous game');
      if(status!==0)throw failure('Native score save failed');
      if(now()>=holder.deadline)throw Error('Score save confirmation timed out');
      holder.timer=schedule(()=>check(holder),pollMs);
    }catch(why){abandon(holder,why);}
  }
  return {
    save(){
      if(pending)return pending.promise;
      const holder={token:0,timer:null,phase:'request',deadline:now()+timeoutMs};
      holder.promise=new Promise((resolve,reject)=>{holder.resolve=resolve;holder.reject=reject;});pending=holder;
      try{
        flushReplay();const token=request();
        // null means no active game, or an ordinary immediate save completed.
        if(token===null)persist(holder);
        else{if(!Number.isInteger(token)||token<=0||token>0xffffffff)throw failure('Native score save request failed');
          holder.token=token;check(holder);}
      }catch(why){abandon(holder,why);}
      return holder.promise;
    },
    cancel(reason='Score save was cancelled by the game lifecycle'){
      const holder=pending;if(!holder)return;
      abandon(holder,cancelled(reason));
    },
    // Hiding/closing a document cannot wait for more gameplay. Flush the
    // already-confirmed Replay and existing files without claiming a pending
    // score request completed or writing a predicted RecordStore.
    flushConfirmed(){try{flushReplay();return Promise.resolve(sync());}catch(why){return Promise.reject(why);}},
  };
}
