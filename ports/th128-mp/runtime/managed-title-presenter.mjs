import {createMatchClock} from './match-clock.mjs';

// The private title VM owns the opening transition and then the animated Main ANMs.
// The ordinary player's input controller and idle-demo clock are separate owners;
// neither belongs to the room. Never present a menu action or another page here.
function mainReady(title){
  const {page,ready,transitioning,mainAction,error}=title.status();
  if(error||page!==0||mainAction)throw new Error(error||`Room title left native Main (page ${page}, action ${mainAction})`);
  return ready===1&&transitioning===0;
}

/** present() acknowledges one actual ready draw. Ownership of subsequent frames
 * and any in-flight bank load lasts until stop(), replacement, abort or close().
 * A non-room backdrop may request {once:true} to retire at that first draw. */
export function createManagedTitlePresenter({title,draw,visible,requestFrame,cancelFrame,prepare,onError=error=>{throw error;}}){
  let current=null,closed=false;
  function present(signal,{once=false}={}){
    if(closed)return Promise.reject(new DOMException('标题已关闭','AbortError'));
    if(signal?.aborted)return Promise.reject(signal.reason);
    const scope=new AbortController(),live=signal?AbortSignal.any([signal,scope.signal]):scope.signal;
    return new Promise((resolve,reject)=>{
      let raf=null,acknowledged=false,ready=false;
      const clock=createMatchClock({stepFrame(){
        live.throwIfAborted();title.step();
        live.throwIfAborted();ready=mainReady(title);return {advanced:true};
      }});
      const owner={stop:()=>scope.abort(),resetClock:clock.reset};
      live.addEventListener('abort',()=>{
        if(current===owner)current=null;
        if(raf!==null)cancelFrame(raf);
        if(!acknowledged)reject(live.reason);
      },{once:true});
      function fail(error){
        if(live.aborted)return;
        scope.abort(error);
        if(acknowledged)onError(error); // the first-draw promise has already settled
      }
      function schedule(){if(!live.aborted)raf=requestFrame(frame);}
      function frame(now){
        if(live.aborted)return; // cancellation can race a callback already dequeued by the browser
        raf=null;
        try{
          const shown=visible(),{advanced}=clock.advance(now,shown);
          if(shown&&!live.aborted){
            if(!advanced)ready=mainReady(title); // includes the first paint on return to a retained title
            if(live.aborted)return;
            if(draw()===true&&ready&&!acknowledged&&!live.aborted){
              acknowledged=true;resolve(true);if(once)owner.stop();
            }
          }
          schedule();
        }catch(error){fail(error);}
      }
      const previous=current;current=owner;previous?.stop();
      if(live.aborted)return;
      try{
        if(prepare)Promise.resolve(prepare(live)).then(schedule).catch(fail);
        else schedule();
      }catch(error){fail(error);}
    });
  }
  function stop(){current?.stop();} // retain the native VM; the next owner redraws its current frame
  function close(){closed=true;stop();}
  return Object.freeze({present,stop,resetClock:()=>current?.resetClock(),close});
}
