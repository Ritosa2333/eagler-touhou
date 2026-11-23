import {createMultiplayerSessionOwner} from '../../th128/site/runtime/multiplayer-session.mjs';
import {createConfirmedHistory} from './confirmed-history.mjs';
import {completeConfirmedRound,createMultiplayerRound} from './round.mjs';
import {createMultiplayerStartGate} from './startup.mjs';

// The host supplies a dedicated epoch engine and a seat-bound transport.
// Incoming messages are delivered in order; the host calls flush while waiting
// for asynchronous epoch preparation (including while no gameplay advances).
export const MATCH_RESTART_SCHEMA='th128-restart/2';

export async function createPlayerMatch({rules,localPlayer,allowedMask,openEngine,send,onConfirmed=()=>{},signal,
  restartClock=()=>performance.now(),restartTimeoutMs=10000,maxFrames=216000}){
  if(typeof openEngine!=='function'||typeof send!=='function'||typeof onConfirmed!=='function')
    throw new TypeError('A multiplayer engine, transport and confirmation consumer are required');
  if(!Number.isSafeInteger(maxFrames)||maxFrames<1||maxFrames>216000)throw new RangeError('Invalid multiplayer frame limit');
  const gate=await createMultiplayerStartGate({rules,localPlayer,allowedMask});
  const owner=new AbortController();
  const ownSignal=signal?AbortSignal.any([signal,owner.signal]):owner.signal;
  let session,current,candidate,archive=null,closed=false;
  function check(){if(closed)throw new Error('Multiplayer match is closed');ownSignal.throwIfAborted();}
  async function prepare(rules,signal){
    signal.throwIfAborted();
    const history=await createConfirmedHistory({rules,allowedMask});
    signal.throwIfAborted();
    const engine=await openEngine(rules,signal);
    try{
      signal.throwIfAborted();
      if(!engine||['checkpoint','restore','step','close'].some(name=>typeof engine[name]!=='function'))
        throw new TypeError('Invalid multiplayer epoch engine');
      return {rules,engine,history,round:null,detach:null};
    }catch(error){engine?.close?.();throw error;}
  }
  function dispose(value){
    if(!value)return;
    value.detach?.();value.detach=null;
    try{value.round?.close();}finally{value.engine.close();}
  }
  async function prepareEpoch({rules,signal}){
    if(candidate)throw new Error('An epoch engine is already being prepared');
    const engineOwner=await prepare(rules,AbortSignal.any([signal,ownSignal]));
    const abandon=()=>{if(candidate===engineOwner){candidate=null;dispose(engineOwner);}};
    candidate=engineOwner;
    signal.addEventListener('abort',abandon,{once:true});
    engineOwner.detach=()=>signal.removeEventListener('abort',abandon);
  }
  function bind(value){
    value.round=createMultiplayerRound({session,localPlayer,engine:value.engine,sendInput:send,maxFrames,
      onConfirmed(row,effects){
        value.history.append(row);
        const observed=onConfirmed(value.rules,row,effects);
        if(observed&&typeof observed.then==='function')return observed; // Round rejects asynchronous side effects.
        if(effects?.bothGameOver===true){
          retain(value,'completed');return completeConfirmedRound;
        }
      }});
    return value;
  }
  function retain(value,reason){
    if(!value.history.status().end)value.history.seal(reason);
    archive=value.history;
  }
  try{
    session=await createMultiplayerSessionOwner({rules,localPlayer,allowedMask,prepareEpoch,restartTail:{
      capture:()=>current.round.freezeForRestart(),complete:frameCount=>current.round.finishRestartAt(frameCount),
      resume:()=>current.round.resumeAfterRestart(),now:restartClock,timeoutMs:restartTimeoutMs,
    }});
    current=await prepare(rules,ownSignal);bind(current);
  }catch(error){dispose(current);session?.close();owner.abort();throw error;}

  function promote(){
    if(session.rules.epoch===current.rules.epoch)return;
    if(!candidate||candidate.rules!==session.rules)throw new Error('Committed epoch has no prepared multiplayer engine');
    const old=current;
    if(old.history.status().nextFrame!==session.status().lastRestartFrameCount)throw new Error('Retired history differs from the agreed restart tail');
    current=bind(candidate);candidate=null;current.detach?.();current.detach=null;
    retain(old,'restart');dispose(old);
  }
  function flush(){
    check();promote();current.round.flush();session.progressRestart();
    for(const message of session.drainMessages())send(message);
    return status();
  }
  function status(){
    return Object.freeze({...session.status(),startup:gate.status(),round:current.round.status(),history:current.history.status()});
  }
  function announceStart(){check();if(!gate.status().canStart)send(gate.offer());}
  async function receive(message,sourcePlayer){
    check();
    if(sourcePlayer!==1-localPlayer)throw new Error('Match packet must come from the bound remote seat');
    if(message?.schema==='th128-mp-ready/1'){
      const ack=await gate.receive(message,sourcePlayer);check();
      if(ack)send(ack);
      return status();
    }
    if(!gate.status().canStart)throw new Error('Both players must pass the ready handshake');
    promote();
    if(message?.schema===MATCH_RESTART_SCHEMA){
      // The peer may already have queued READY when this side aborts the same
      // attempt. The validated retired proposal has no effect on the old World.
      current.round.flush();session.receiveRestart(message,sourcePlayer,{ignoreAbortedAttempt:true});
    }else current.round.receiveInput(message,sourcePlayer);
    return flush();
  }
  function tick(held,options){
    check();
    if(!gate.status().canStart)return {advanced:false,reason:'waiting-ready'};
    flush();return current.round.tick(held,options);
  }
  function requestRestart(){
    check();if(!gate.status().canStart)throw new Error('Both players must pass the ready handshake');
    flush();const {epoch,attempt}=session.status();
    const vote=session.requestRestart(`restart_${epoch}_${attempt}`);flush();return vote;
  }
  function cancelRestart(){check();const message=session.cancelRestart();flush();return message;}
  function spectatorSource(){
    check();promote();const history=current.history;
    return Object.freeze({header:history.header,status:history.status,read:history.read,chunk:history.chunk});
  }
  function close(){
    if(closed)return;closed=true;
    try{session.close();}finally{
      owner.abort();
      const unused=candidate;candidate=null;
      try{dispose(unused);}finally{dispose(current);}
    }
  }
  return Object.freeze({announceStart,receive,tick,status,flush,requestRestart,cancelRestart,spectatorSource,close,
    waitRestart:async()=>{check();await session.waitRestart();return flush();},
    replayReceipt:()=>{check();promote();return archive?{epoch:archive.header.rules.epoch,end:archive.status().end}:null;},
    previousReplay:()=>{check();promote();return archive?.replayBytes()??null;}});
}
