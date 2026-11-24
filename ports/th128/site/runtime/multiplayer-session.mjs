import {createMultiplayerRules,multiplayerRulesFingerprint} from './multiplayer-rules.mjs';
import {createMultiplayerInputLedger} from './multiplayer-input-ledger.mjs';
const wireFields=['schema','matchId','epoch','mode','route','seed','coreSHA256','resourceSHA256','inputDelay','rollbackWindow'];
const messageFields=['schema','kind','matchId','fromEpoch','toEpoch','attempt','requestId','fingerprint','allowedMask','player'];
const token=value=>typeof value==='string'&&/^[a-zA-Z0-9_-]{1,64}$/.test(value);
const integer=(v,min,max)=>Number.isSafeInteger(v)&&v>=min&&v<=max;
function messageCopy(input){
 if(!input||Array.isArray(input)||![Object.prototype,null].includes(Object.getPrototypeOf(input)))throw Error('Restart requires a plain data object');
 const descriptors=Object.getOwnPropertyDescriptors(input),keys=Reflect.ownKeys(descriptors),out={};
 if(keys.length!==messageFields.length||keys.some(key=>typeof key!=='string'||!messageFields.includes(key)))throw Error('Unknown or missing restart message field');
 for(const key of messageFields){if(!descriptors[key]||!Object.hasOwn(descriptors[key],'value'))throw Error('Restart needs data property fields');out[key]=descriptors[key].value;}
 return out;
}
async function nextTarget(rules){
 if(rules.epoch===2147483647)return null;
 const wire=Object.fromEntries(wireFields.map(key=>[key,rules[key]]));wire.epoch++;
 const next=createMultiplayerRules(wire);return Object.freeze({rules:next,fingerprint:await multiplayerRulesFingerprint(next)});
}
export async function createMultiplayerSessionOwner({rules,allowedMask,localPlayer,prepareEpoch=async()=>{}}){
 if(typeof prepareEpoch!=='function')throw Error('Epoch preparation consumer required');
 let ledger=await createMultiplayerInputLedger({rules,allowedMask,localPlayer}),target,fingerprint;
 try{fingerprint=await multiplayerRulesFingerprint(rules);target=await nextTarget(rules);}catch(error){ledger.close();throw error;}
 let pending=null,work=null,outbox=[],closed=false,attempt=1,restartCommits=0,lastCommit=null,lastAbort=null,lastError=null;
 const live=()=>{if(closed)throw Error('Multiplayer session is closed');};
 const active=()=>{live();if(pending?.votes.every(Boolean))throw Error('Restart preparation is in progress');};
 function status(){return Object.freeze({closed,epoch:rules.epoch,fingerprint,attempt,restartCommits,lastError,
  phase:closed?'closed':!pending?'running':pending.ready[localPlayer]?'ready':pending.votes.every(Boolean)?'preparing':'voting',
  requestId:pending?.requestId??null,votes:Object.freeze(pending?[...pending.votes]:[false,false]),ready:Object.freeze(pending?[...pending.ready]:[false,false]),outbox:outbox.length});}
 function room(){if(outbox.length>5)throw Error('Restart outbox must be drained before another request');}
 function enqueue(message){if(outbox.length>=8)throw Error('Restart outbox is full');outbox.push(message);}
 function packet(p,kind){return Object.freeze({schema:'th128-restart/1',kind,matchId:rules.matchId,fromEpoch:rules.epoch,toEpoch:target.rules.epoch,attempt:p.attempt,requestId:p.requestId,fingerprint:target.fingerprint,allowedMask,player:localPlayer});}
 function slot(requestId){
  if(!target)throw Error('Restart epoch exhausted');
  if(!token(requestId)||!integer(attempt,1,2147483647))throw Error('Invalid restart request or attempt');
  if(pending){if(pending.requestId!==requestId||pending.attempt!==attempt)throw Error('Conflicting restart request');return pending;}
  room();pending={requestId,attempt,votes:[false,false],ready:[false,false],controller:new AbortController(),candidate:null,nextTarget:null,localVote:null};
  return pending;
 }
 function commit(p){
  if(pending!==p||closed||!p.candidate||!p.votes.every(Boolean)||!p.ready.every(Boolean))return;
  const old=ledger;lastCommit={fromEpoch:rules.epoch,toEpoch:target.rules.epoch,attempt:p.attempt,requestId:p.requestId,fingerprint:target.fingerprint};
  ledger=p.candidate;p.candidate=null;rules=target.rules;fingerprint=target.fingerprint;target=p.nextTarget;pending=null;attempt=1;restartCommits++;lastError=null;old.close();
 }
 function discard(p,message,notify){
  if(pending!==p||closed)return;
  const abort=notify?packet(p,'abort'):message;
  p.controller.abort();p.candidate?.close();p.candidate=null;pending=null;lastAbort={...abort};attempt++;
  if(notify)enqueue(abort);
 }
 function begin(p){
  if(pending!==p||closed||p.preparing||!p.votes.every(Boolean))return;
  p.preparing=true;
  const task=(async()=>{
   let candidate=null;
   try{
    await prepareEpoch(Object.freeze({rules:target.rules,requestId:p.requestId,fromEpoch:rules.epoch,toEpoch:target.rules.epoch,signal:p.controller.signal}));
    if(closed||pending!==p||p.controller.signal.aborted)return;
    candidate=await createMultiplayerInputLedger({rules:target.rules,allowedMask,localPlayer});
    const following=await nextTarget(target.rules);
    if(closed||pending!==p||p.controller.signal.aborted){candidate.close();return;}
    p.candidate=candidate;candidate=null;p.nextTarget=following;p.ready[localPlayer]=true;enqueue(packet(p,'ready'));commit(p);
   }catch(error){
    candidate?.close();
    if(!closed&&pending===p){lastError=error.message??String(error);discard(p,null,true);}
   }
  })();work=task;task.finally(()=>{if(work===task)work=null;});
 }
 function requestRestart(requestId=pending?.requestId??globalThis.crypto.randomUUID()){
  live();const p=slot(requestId);if(p.localVote)return p.localVote;
  const vote=packet(p,'vote');p.votes[localPlayer]=true;p.localVote=vote;enqueue(vote);begin(p);return vote;
 }
 function sameHistory(m,h){return h&&m.fromEpoch===h.fromEpoch&&m.toEpoch===h.toEpoch&&m.attempt===h.attempt&&m.requestId===h.requestId&&m.fingerprint===h.fingerprint;}
 function receiveRestart(input,sourcePlayer){
  live();const m=messageCopy(input);
  if(!integer(sourcePlayer,0,1)||m.player!==sourcePlayer)throw Error('Restart source player mismatch');
  if(m.schema!=='th128-restart/1'||!['vote','ready','abort'].includes(m.kind)||m.matchId!==rules.matchId||m.allowedMask!==allowedMask||!token(m.requestId)||!integer(m.attempt,1,2147483647))throw Error('Invalid restart message or capability mask');
  if(m.kind!=='abort'&&sameHistory(m,lastCommit))return Object.freeze({duplicate:true,epoch:rules.epoch,committed:true});
  if(m.kind==='abort'&&sameHistory(m,lastAbort))return Object.freeze({duplicate:true,epoch:rules.epoch,committed:false});
  if(m.fromEpoch!==rules.epoch||!target||m.toEpoch!==target.rules.epoch||m.fingerprint!==target.fingerprint)throw Error('Restart epoch or fingerprint mismatch');
  if(m.attempt!==attempt)throw Error('Restart attempt is stale');
  const p=slot(m.requestId);
  if(m.kind==='abort'){discard(p,m,false);return Object.freeze({duplicate:false,epoch:rules.epoch,committed:false});}
  const values=m.kind==='vote'?p.votes:p.ready,duplicate=values[sourcePlayer];values[sourcePlayer]=true;begin(p);commit(p);
  return Object.freeze({duplicate,epoch:rules.epoch,committed:pending!==p});
 }
 function cancelRestart(){live();if(!pending)return null;const p=pending;if(p.ready[localPlayer])throw Error('Local READY cannot be revoked');const abort=packet(p,'abort');discard(p,null,true);return abort;}
 function drainMessages(){live();const messages=Object.freeze(outbox);outbox=[];return messages;}
 async function waitRestart(){const task=work;if(task)await task;return status();}
 function close(){if(closed)return;closed=true;pending?.controller.abort();pending?.candidate?.close();pending=null;ledger.close();outbox=[];}
 return Object.freeze({get rules(){return rules;},status,requestRestart,receiveRestart,cancelRestart,drainMessages,waitRestart,close,
  receiveInput:(input,sourcePlayer)=>{live();return ledger.receive(input,sourcePlayer);},
  queueLocal:held=>{active();return ledger.queueLocal(held);},consume:()=>{active();return ledger.consume();},
  prepareCorrection:()=>{live();return ledger.prepareCorrection();},acceptCorrection:plan=>{live();return ledger.acceptCorrection(plan);},
  exportConfirmed:()=>{live();return ledger.exportConfirmed();}
 });
}
