import {createMatchClock} from './match-clock.mjs';

const restorable=['paused','playing','ended'],seekBatch=4;

// The archive owns both seats. The visible seat is only a render choice; no live
// inputs, prediction, netplay connection or changes to the tape are involved.
export async function openTh128ReplayPlayback({tape,openWorld,render,signal}){
  const {nextFrame:total,end}=tape.status();
  if(!Number.isSafeInteger(total)||total<0||!end||typeof openWorld!=='function'||typeof render!=='function')
    throw new TypeError('A sealed multiplayer Replay and World owner are required');
  signal?.throwIfAborted();
  const seatCount=tape.header?.rules?.schema==='th128-coop/1'?tape.header.rules.players:2;
  if(![2,3].includes(seatCount))throw new Error('Invalid Replay seat count');
  const lifetime=new AbortController();
  let world=null,phase='opening',frame=0,seat=0,target=null;
  const status=()=>Object.freeze({phase,seat,frame,total,end,...(seatCount===3?{seatCount}:{}),...(target===null?{}:{target})});
  function step(){
    const row=tape.read(frame);
    if(row?.frame!==frame)throw new Error('Replay timeline stopped before the declared end');
    world.engine.step(row);frame++;
  }
  const clock=createMatchClock({stepFrame(){
    if(frame===total)return {advanced:false,reason:'replay-ended'};
    step();
    return {advanced:true};
  }});

  function close(){
    if(phase==='closed')return;
    phase='closed';target=null;clock.reset();signal?.removeEventListener('abort',close);
    const old=world;world=null;lifetime.abort(signal?.reason);old?.engine.close();
  }
  async function replaceWorld(seekTo=null){
    const candidate=await openWorld(tape.header.rules,lifetime.signal);
    if(phase==='closed'){candidate.engine.close();return false;}
    try{if(seekTo===null||seekTo===0)render(candidate,seat,0);}catch(error){candidate.engine.close();throw error;}
    if(phase==='closed'){candidate.engine.close();return false;}
    const previous=world;
    world=candidate;frame=0;
    target=seekTo>0?seekTo:null;
    phase=target!==null?'seeking':total===0?'ended':'paused';
    previous?.engine.close();
    return true;
  }
  function play(){
    if(phase!=='paused')return false;
    phase='playing';clock.reset();return true;
  }
  function pause(){
    if(phase!=='playing')return false;
    phase='paused';clock.reset();return true;
  }
  function selectSeat(next){
    if(!Number.isInteger(next)||next<0||next>=seatCount)throw new RangeError('Unknown Replay seat');
    if(next===seat||!restorable.includes(phase))return false;
    render(world,next,frame);seat=next;return true;
  }
  async function restart(){
    if(!restorable.includes(phase))return false;
    const before=phase;
    phase='opening';clock.reset();
    try{return await replaceWorld();}
    catch(error){if(phase==='closed')return false;phase=before;throw error;}
  }
  async function seek(next){
    if(!Number.isSafeInteger(next)||next<0||next>total)throw new RangeError('Replay seek is outside its bounds');
    if(!restorable.includes(phase))return false;
    clock.reset();
    if(next===frame){phase=frame===total?'ended':'paused';return true;}
    if(next>frame){target=next;phase='seeking';return true;}
    const before=phase;phase='opening';
    try{return await replaceWorld(next);}
    catch(error){if(phase==='closed')return false;phase=before;throw error;}
  }
  function advance(time,visible=true){
    try{
      if(!Number.isFinite(time))throw new Error('Invalid multiplayer presentation timestamp');
      if(phase==='seeking'&&visible){
        const old=frame,through=Math.min(target,frame+seekBatch);
        while(frame<through)step();
        if(frame===target){target=null;phase=frame===total?'ended':'paused';render(world,seat,frame);}
        return {advanced:frame-old,stalled:false,reason:null};
      }
      const state=clock.advance(time,phase==='playing'&&visible);
      if(state.advanced)render(world,seat,frame);
      if(phase==='playing'&&frame===total){phase='ended';clock.reset();}
      return state;
    }catch(error){close();throw error;}
  }

  signal?.addEventListener('abort',close,{once:true});
  try{
    if(!await replaceWorld())lifetime.signal.throwIfAborted();
  }catch(error){close();throw error;}
  return Object.freeze({status,play,pause,restart,seek,selectSeat,advance,close});
}
