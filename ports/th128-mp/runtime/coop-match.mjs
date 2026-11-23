import {createCoopPause,PAUSE_WIRE} from './coop-pause.mjs';
import {coopWire,makeCoopRules,coopFingerprint} from './coop-rules.mjs';
import {createConfirmedHistory} from './confirmed-history.mjs';
import {createCoopLockstep} from './coop-lockstep.mjs';
const barrier='th128-coop-barrier/2',limit=216000;
// R is a shared restart request; peers acknowledge the same last frame, without asking
// each human to vote or manufacturing remote inputs for a machine which is behind.
export async function createCoopMatch({rules,localPlayer,allowedMask,openEngine,send,signal,onConfirmed=()=>{}}){
 const n=rules.players,owner=new AbortController(),live=signal?AbortSignal.any([signal,owner.signal]):owner.signal;
 let current,restart=null,closed=false,error=null,previous=null;
 function check(){live.throwIfAborted();if(closed)throw Error('Closed co-op room');if(error)throw error;}
 async function open(r){const key=await coopFingerprint(r),tape=await createConfirmedHistory({rules:r,allowedMask});live.throwIfAborted();const engine=await openEngine(r,live);
  try{live.throwIfAborted();const result={reason:null};const round=await createCoopLockstep({rules:r,localSeat:localPlayer,engine,send,onConfirmed(row,effects){
    if(tape.status().end)return;tape.append(row);onConfirmed(r,row,effects);if(effects?.teamOver||effects?.cleared){result.reason=effects.cleared?'cleared':'team-over';tape.seal('completed');previous=tape;}
  }});const pause=createCoopPause({players:n,seat:localPlayer,epoch:r.epoch,key,send,position:()=>round.status().next,confirmed:()=>tape.status().nextFrame});return {r,key,tape,round,pause,result};}catch(e){engine.close();throw e;}
 }
 current=await open(rules);
 function packet(phase,frame,action){return {schema:barrier,phase,action,seat:localPlayer,epoch:current.r.epoch,key:current.key,frame};}
 function freeze(action='restart'){if(restart)return;const fence=current.tape.status().end?current.tape.status().nextFrame:current.round.status().next;
  restart={actions:new Map([[localPlayer,action]]),cause:null,frontiers:new Map([[localPlayer,fence]]),prepared:new Map(),target:null,opening:false,next:null,early:new Map(),since:performance.now()};send(packet('fence',fence,action));}
 function progress(){const b=restart;if(!b)return;
  if(performance.now()-b.since>30000)throw Error('Cooperative restart did not reach a common frame');
  if(b.frontiers.size===n&&b.target===null){b.cause=[...b.actions.values()].includes('left')?'left':'restart';b.target=Math.max(...b.frontiers.values());if(b.target-Math.min(...b.frontiers.values())>60)throw Error('Cooperative restart peers are too far apart');}
  if(b.target===null)return;
  const frame=current.tape.status().nextFrame;if(b.target<frame)throw Error('Confirmed history passed the restart fence');
  function prepared(){b.prepared.set(localPlayer,{frame:b.target,action:b.cause});send(packet('prepared',b.target,b.cause));}
  if(frame===b.target&&!b.opening){b.opening=true;if(b.cause==='left')prepared();else{
   const nr=makeCoopRules({...coopWire(current.r),epoch:current.r.epoch+1});
   void open(nr).then(next=>{if(closed||b!==restart){next.round.close();return;}b.next=next;prepared();}).catch(e=>{if(!closed)error=e;});}}
  if((b.cause!=='left'&&!b.next)||b.prepared.size!==n)return;
  if([...b.prepared.values()].some(v=>v.frame!==b.target||v.action!==b.cause))throw Error('Cooperative result/restart acknowledgements differ');
  if(b.cause==='left'){if(!current.tape.status().end)current.tape.seal('left');previous=current.tape;current.result.reason='left';restart=null;return;}
  if(!current.tape.status().end)current.tape.seal('restart');previous=current.tape;const old=current;current=b.next;restart=null;
  old.round.close();current.round.announce();for(const [seat,msg] of b.early)current.round.receive(msg,seat);
 }
 function flush(){check();current.round.flush();if(!restart)current.pause.progress();progress();return status();}
 function requestRestart(){check();if(current.tape.status().end&&current.result.reason!=='team-over')return false;if(!current.round.status().ready)throw Error('Wait for all co-op seats');freeze();progress();return true;}
 function requestFinish(){check();if(restart||current.tape.status().end||!current.pause.paused())return false;freeze('left');progress();return true;}
 function receive(msg,from){check();if(!Number.isInteger(from)||from<0||from>=n||from===localPlayer||msg?.seat!==from)throw Error('Wrong room packet sender');
  if(msg.schema===PAUSE_WIRE){if(!current.round.status().ready)throw Error('Pause before cooperative ready');if(!restart)current.pause.receive(msg,from);return flush();}
  if(msg.schema!==barrier){if(restart&&msg.epoch===current.r.epoch+1&&msg.kind==='ready'&&msg.schema==='th128-coop-peer/1'){
      const existing=restart.early.get(from);if(existing&&existing.key!==msg.key)throw Error('New epoch ready changed');restart.early.set(from,msg);return status();}
    current.round.receive(msg,from);return flush();}
  if(!current.round.status().ready||msg.epoch!==current.r.epoch||msg.key!==current.key||!['fence','prepared'].includes(msg.phase)||!Number.isInteger(msg.frame)||msg.frame<0||msg.frame>limit)throw Error('Invalid co-op restart barrier');
  if(!['restart','left'].includes(msg.action))throw Error('Invalid cooperative barrier action');
  if(current.tape.status().end&&(current.result.reason!=='team-over'||msg.action!=='restart'))return status();
  if(!restart&&msg.phase!=='fence')throw Error('Unexpected cooperative ready');freeze(msg.action);const b=restart;
  const entries=msg.phase==='fence'?b.frontiers:b.prepared;if(msg.phase==='prepared'&&!b.frontiers.has(from))throw Error('Sender has not fenced its history');
  if(msg.phase==='fence'){if(entries.has(from)&&(entries.get(from)!==msg.frame||b.actions.get(from)!==msg.action))throw Error('A seat changed its cooperative frontier');b.actions.set(from,msg.action);entries.set(from,msg.frame);}
  else{const old=entries.get(from);if(old&&(old.frame!==msg.frame||old.action!==msg.action))throw Error('Changed cooperative acknowledgement');entries.set(from,{frame:msg.frame,action:msg.action});}return flush();
 }
 function tick(held,{allowPrediction=true}={}){flush();const s=current.round.status();if(!s.ready)return {advanced:false,reason:'waiting-ready'};
  if(current.tape.status().end)return {advanced:false,reason:'round-completed'};
  if(!restart&&!current.pause.mayStep())return {advanced:false,reason:'shared-pause'};
  if(restart&&(restart.target===null||s.next>=restart.target))return {advanced:false,reason:'restart-preparing'};
  if(current.round.needsSample())current.round.sample(typeof held==='function'?held():held);
  const advanced=current.round.advance({allowPrediction:restart||current.pause.catchingUp()?false:allowPrediction});progress();return {advanced,reason:advanced?null:'waiting-input'};
 }
 function status(){const s=current.round.status();return {epoch:current.r.epoch,result:current.result.reason,phase:current.result.reason?'ended':restart?'voting':current.pause.paused()?'paused':'running',startup:{canStart:s.ready},history:current.tape.status(),round:{replayedFrames:s.replayed}};}
 function close(){if(closed)return;closed=true;owner.abort();restart?.next?.round.close();current.round.close();restart=null;}
 return Object.freeze({announceStart:()=>current.round.announce(),receive,tick,flush,status,requestRestart,requestFinish,togglePause(){check();if(!restart&&current.round.status().ready&&!current.tape.status().end)current.pause.toggle();},close,spectatorSource:()=>current.tape,
   replayReceipt:()=>previous?{epoch:previous.header.rules.epoch}:null,previousReplay:()=>previous?.replayBytes()??null});
}
