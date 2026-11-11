// Shared presentation/input ceiling. Never used to discard simulation history.
export const FRAME_HZ=60;
export const FRAME_MS=1000/FRAME_HZ;
export function createFrameGate(){
  let epoch=null,last=-1;
  return now=>{
    if(!Number.isFinite(now))return false;
    if(epoch===null||now<epoch){epoch=now;last=-1;}
    const slot=Math.floor((now-epoch+.01)/FRAME_MS);
    if(slot<=last)return false;
    last=slot;return true;
  };
}
export function startFrameLoop(callback){
  const due=createFrameGate();let stopped=false,id;
  const run=now=>{if(stopped)return;if(due(now))callback(now);if(!stopped)id=requestAnimationFrame(run);};
  id=requestAnimationFrame(run);
  return ()=>{stopped=true;cancelAnimationFrame(id);};
}
