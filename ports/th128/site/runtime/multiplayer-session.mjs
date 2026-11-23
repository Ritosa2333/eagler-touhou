import {createMultiplayerRules,multiplayerRulesFingerprint} from './multiplayer-rules.mjs';
import {createMultiplayerInputLedger} from './multiplayer-input-ledger.mjs';
const wireFields=['schema','matchId','epoch','mode','route','seed','coreSHA256','resourceSHA256','inputDelay','rollbackWindow'];
const messageFields=['schema','kind','matchId','fromEpoch','toEpoch','attempt','requestId','fingerprint','allowedMask','player'];
const token=value=>typeof value==='string'&&/^[a-zA-Z0-9_-]{1,64}$/.test(value);
const integer=(v,min,max)=>Number.isSafeInteger(v)&&v>=min&&v<=max;
function messageCopy(input,agreeTail){
 if(!input||Array.isArray(input)||![Object.prototype,null].includes(Object.getPrototypeOf(input)))throw Error('Restart requires a plain data object');
 const descriptors=Object.getOwnPropertyDescriptors(input),keys=Reflect.ownKeys(descriptors),out={};
 const fields=agreeTail?[...messageFields,'tailFrames']:messageFields;
 if(keys.length!==fields.length||keys.some(key=>typeof key!=='string'||!fields.includes(key)))throw Error('Unknown or missing restart message field');
 for(const key of fields){if(!descriptors[key]||!Object.hasOwn(descriptors[key],'value'))throw Error('Restart needs data property fields');out[key]=descriptors[key].value;}
 return out;
}
async function nextTarget(rules){
 if(rules.epoch===2147483647)return null;
 const wire=Object.fromEntries(wireFields.map(key=>[key,rules[key]]));wire.epoch++;
 const next=createMultiplayerRules(wire);return Object.freeze({rules:next,fingerprint:await multiplayerRulesFingerprint(next)});
}
export async function createMultiplayerSessionOwner({rules,allowedMask,localPlayer,prepareEpoch=async()=>{},restartTail}){
 if(typeof prepareEpoch!=='function')throw Error('Epoch preparation consumer required');
 if(restartTail&&(['capture','complete','resume','now'].some(key=>typeof restartTail[key]!=='function')||
   !integer(restartTail.timeoutMs,1,30000)))throw Error('Invalid restart tail consumer');
 const schema=restartTail?'th128-restart/2':'th128-restart/1';
 let ledger=await createMultiplayerInputLedger({rules,allowedMask,localPlayer}),target,fingerprint;
 try{fingerprint=await multiplayerRulesFingerprint(rules);target=await nextTarget(rules);}catch(error){ledger.close();throw error;}
 let pending=null,outbox=[],closed=false,attempt=1,restartCommits=0,lastCommit=null,lastAbort=null,lastError=null;
 const live=()=>{if(closed)throw Error('Multiplayer session is closed');};
 const active=()=>{live();if(pending?.votes.every(Boolean))throw Error('Restart preparation is in progress');};
 function status(){return Object.freeze({closed,epoch:rules.epoch,fingerprint,attempt,restartCommits,lastError,
  phase:closed?'closed':!pending?'running':pending.ready[localPlayer]?'ready':pending.votes.every(Boolean)?'preparing':'voting',
  requestId:pending?.requestId??null,votes:Object.freeze(pending?[...pending.votes]:[false,false]),ready:Object.freeze(pending?[...pending.ready]:[false,false]),outbox:outbox.length,
  ...(restartTail?{restartFrameCount:pending?.cutoff??null,lastRestartFrameCount:lastCommit?.cutoff??null}:{})});}
 function room(){if(outbox.length>5)throw Error('Restart outbox must be drained before another request');}
 function enqueue(message){if(outbox.length>=8)throw Error('Restart outbox is full');outbox.push(message);}
 function packet(p,kind){return Object.freeze({schema,kind,matchId:rules.matchId,fromEpoch:rules.epoch,toEpoch:target.rules.epoch,attempt:p.attempt,requestId:p.requestId,fingerprint:target.fingerprint,allowedMask,player:localPlayer,
  ...(restartTail?{tailFrames:kind==='tail'?p.tails[localPlayer]:kind==='ready'?p.cutoff:null}:{})});}
 function slot(requestId){
  if(!target)throw Error('Restart epoch exhausted');
  if(!token(requestId)||!integer(attempt,1,2147483647))throw Error('Invalid restart request or attempt');
  if(pending){if(pending.requestId!==requestId||pending.attempt!==attempt)throw Error('Conflicting restart request');return pending;}
  room();pending={requestId,attempt,votes:[false,false],ready:[false,false],controller:new AbortController(),candidate:null,nextTarget:null,localVote:null,
   tails:[null,null],cutoff:null,frozen:false,deadline:null,work:null};
  return pending;
 }
 function commit(p){
  if(pending!==p||closed||!p.candidate||!p.votes.every(Boolean)||!p.ready.every(Boolean))return;
  const old=ledger;lastCommit={fromEpoch:rules.epoch,toEpoch:target.rules.epoch,attempt:p.attempt,requestId:p.requestId,fingerprint:target.fingerprint,cutoff:p.cutoff,tails:p.tails};
  ledger=p.candidate;p.candidate=null;rules=target.rules;fingerprint=target.fingerprint;target=p.nextTarget;pending=null;attempt=1;restartCommits++;lastError=null;old.close();
 }
 function discard(p,message,notify){
  if(pending!==p||closed)return;
  const abort=notify?packet(p,'abort'):message;
  p.controller.abort();p.candidate?.close();p.candidate=null;pending=null;lastAbort={...abort};attempt++;
  if(p.frozen)restartTail.resume();
  if(notify)enqueue(abort);
 }
 function abortPreparation(p,error){if(!closed&&pending===p){lastError=error.message??String(error);discard(p,null,true);}}
 function begin(p){
  if(pending!==p||closed||!p.votes.every(Boolean))return;
  if(restartTail&&!p.ready[localPlayer]){
   try{
    if(!p.frozen){
     p.frozen=true;p.deadline=restartTail.now()+restartTail.timeoutMs;
     const tail=restartTail.capture();if(!integer(tail,0,2147483647))throw Error('Invalid local restart tail');
     p.tails[localPlayer]=tail;enqueue(packet(p,'tail'));
    }
    if(restartTail.now()>=p.deadline)throw Error('Restart preparation deadline expired');
    if(p.preparing||p.tails.includes(null))return;
    // A published prefix cannot be removed. Catch up to the larger exclusive
    // frontier using only already sampled, mutually confirmed input rows.
    p.cutoff=Math.max(...p.tails);
    if(!restartTail.complete(p.cutoff))return;
   }catch(error){abortPreparation(p,error);return;}
  }
  if(p.preparing)return;
  p.preparing=true;
  p.work=(async()=>{
   let candidate=null;
   try{
    await prepareEpoch(Object.freeze({rules:target.rules,requestId:p.requestId,fromEpoch:rules.epoch,toEpoch:target.rules.epoch,signal:p.controller.signal}));
    if(closed||pending!==p||p.controller.signal.aborted)return;
    candidate=await createMultiplayerInputLedger({rules:target.rules,allowedMask,localPlayer});
    const following=await nextTarget(target.rules);
    if(closed||pending!==p||p.controller.signal.aborted){candidate.close();return;}
    if(restartTail&&restartTail.now()>=p.deadline)throw Error('Restart preparation deadline expired');
    p.candidate=candidate;candidate=null;p.nextTarget=following;p.ready[localPlayer]=true;enqueue(packet(p,'ready'));commit(p);
   }catch(error){
    candidate?.close();
    abortPreparation(p,error);
   }
  })();
 }
 function requestRestart(requestId=pending?.requestId??globalThis.crypto.randomUUID()){
  live();const p=slot(requestId);if(p.localVote)return p.localVote;
  const vote=packet(p,'vote');p.votes[localPlayer]=true;p.localVote=vote;enqueue(vote);begin(p);return vote;
 }
 function sameHistory(m,h){return h&&m.fromEpoch===h.fromEpoch&&m.toEpoch===h.toEpoch&&m.attempt===h.attempt&&m.requestId===h.requestId&&m.fingerprint===h.fingerprint;}
 function receiveRestart(input,sourcePlayer,{ignoreAbortedAttempt=false}={}){
  live();const m=messageCopy(input,!!restartTail);
  if(!integer(sourcePlayer,0,1)||m.player!==sourcePlayer)throw Error('Restart source player mismatch');
  const kinds=restartTail?['vote','tail','ready','abort']:['vote','ready','abort'];
  if(m.schema!==schema||!kinds.includes(m.kind)||m.matchId!==rules.matchId||m.allowedMask!==allowedMask||!token(m.requestId)||!integer(m.attempt,1,2147483647))throw Error('Invalid restart message or capability mask');
  if(restartTail&&((m.kind==='tail'||m.kind==='ready')?!integer(m.tailFrames,0,2147483647):m.tailFrames!==null))throw Error('Invalid restart tail frames');
  if(m.kind!=='abort'&&sameHistory(m,lastCommit)){
   if(restartTail&&(m.kind==='tail'&&m.tailFrames!==lastCommit.tails[sourcePlayer]||m.kind==='ready'&&m.tailFrames!==lastCommit.cutoff))throw Error('Changed committed restart tail');
   return Object.freeze({duplicate:true,epoch:rules.epoch,committed:true});
  }
  if((m.kind==='abort'||ignoreAbortedAttempt)&&sameHistory(m,lastAbort))return Object.freeze({duplicate:true,epoch:rules.epoch,committed:false});
  if(m.fromEpoch!==rules.epoch||!target||m.toEpoch!==target.rules.epoch||m.fingerprint!==target.fingerprint)throw Error('Restart epoch or fingerprint mismatch');
  if(m.attempt!==attempt)throw Error('Restart attempt is stale');
  if(restartTail&&(m.kind==='tail'||m.kind==='ready')&&!pending?.votes.every(Boolean))throw Error('Restart tail requires both player votes');
  const p=slot(m.requestId);
  if(m.kind==='abort'){discard(p,m,false);return Object.freeze({duplicate:false,epoch:rules.epoch,committed:false});}
  if(m.kind==='tail'){
   const previous=p.tails[sourcePlayer];
   if(previous!==null&&previous!==m.tailFrames)throw Error('Changed restart tail declaration');
   p.tails[sourcePlayer]=m.tailFrames;begin(p);return Object.freeze({duplicate:previous!==null,epoch:rules.epoch,committed:rules.epoch===m.toEpoch});
  }
  if(restartTail&&m.kind==='ready'&&(p.cutoff===null||p.cutoff!==m.tailFrames))throw Error('READY differs from the agreed restart tail');
  const values=m.kind==='vote'?p.votes:p.ready,duplicate=values[sourcePlayer];values[sourcePlayer]=true;begin(p);commit(p);
  return Object.freeze({duplicate,epoch:rules.epoch,committed:pending!==p});
 }
 function cancelRestart(){live();if(!pending)return null;const p=pending;if(p.ready[localPlayer])throw Error('Local READY cannot be revoked');const abort=packet(p,'abort');discard(p,null,true);return abort;}
 function drainMessages(){live();const messages=Object.freeze(outbox);outbox=[];return messages;}
 async function waitRestart(){
  const task=pending?.work,signal=pending?.controller.signal;
  if(task&&signal&&!signal.aborted){
   let stop;const cancelled=new Promise(resolve=>{stop=resolve;});signal.addEventListener('abort',stop,{once:true});
   try{await Promise.race([task,cancelled]);}finally{signal.removeEventListener('abort',stop);}
  }
  return status();
 }
 function consumeRestartFrame(){
  live();const s=ledger.status();
  if(!restartTail||!pending?.votes.every(Boolean)||pending.cutoff===null||s.nextFrame>=pending.cutoff||s.nextFrame>s.confirmedThrough)
   throw Error('Restart may only consume the agreed confirmed prefix');
  return ledger.consume();
 }
 function close(){if(closed)return;closed=true;pending?.controller.abort();pending?.candidate?.close();pending=null;ledger.close();outbox=[];}
 return Object.freeze({get rules(){return rules;},status,requestRestart,receiveRestart,cancelRestart,drainMessages,waitRestart,close,
  progressRestart:()=>{live();if(pending)begin(pending);},consumeRestartFrame,
  inputStatus:()=>{live();return ledger.status();},
  receiveInput:(input,sourcePlayer)=>{live();return ledger.receive(input,sourcePlayer);},
  queueLocal:held=>{active();return ledger.queueLocal(held);},consume:()=>{active();return ledger.consume();},
  prepareCorrection:()=>{live();return ledger.prepareCorrection();},acceptCorrection:plan=>{live();return ledger.acceptCorrection(plan);},
  exportConfirmed:()=>{live();return ledger.exportConfirmed();}
 });
}
