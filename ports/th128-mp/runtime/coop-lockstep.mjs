import {coopFingerprint,TH128_COOP_FRAME_LIMIT as MAX_FRAMES} from './coop-rules.mjs';
const LOOKAHEAD=60,OLD_WINDOW=60;
const bounded=(n,a,b)=>Number.isSafeInteger(n)&&n>=a&&n<=b;
const supported=n=>bounded(n,0,0xffff)&&!(n&~763);
// Author peer/Relay supplies the transport sender. The seat in an envelope is never trusted alone.
// A checkpoint covers ALL participants plus the shared native heap, not one heap per seat.
export async function createCoopLockstep({rules,localSeat,engine,send,onConfirmed=()=>{}}){
 const key=await coopFingerprint(rules),{players:n,rollbackWindow:window,inputDelay:delay,epoch}=rules;
 if(!bounded(localSeat,0,n-1)||typeof send!=='function'||typeof onConfirmed!=='function'||['step','checkpoint','restore','release','close'].some(k=>typeof engine?.[k]!=='function'))throw Error('Missing co-op lockstep ownership');
 const wire=Object.freeze({schema:'th128-coop-peer/1',key,epoch,seat:localSeat}),readySeats=new Set([localSeat]);
 const received=new Map(),pending=new Map(),settled=new Map(),zero=Object.freeze(Array(n).fill(0));
 // All peers agree that delay bootstrap frames contain neutral input; no network party owns fictitious samples.
 for(let f=0;f<delay;f++)received.set(f,[...zero]);
 let next=0,committed=0,replayed=0,last=zero,closed=false,poison=false,busy=false;
 const guard=()=>{if(closed||poison||busy)throw Error('Co-op session is closed, faulted or reentered');};
 function announce(){guard();send({...wire,kind:'ready'});}
 function controls(frame,previous){const known=received.get(frame);return Array.from({length:n},(_,s)=>known?.[s]??previous[s]);}
 function full(frame){const data=received.get(frame);return !!data&&data.every(x=>x!==null);}
 function put(frame,s,keys){if(frame<committed){if(settled.get(frame)?.[s]!==keys)throw Error('Conflicting or retired co-op input');return;}
  if(!received.has(frame))received.set(frame,Array(n).fill(null));const k=received.get(frame);if(k[s]!==null&&k[s]!==keys)throw Error('A seat changed its submitted input');k[s]=keys;
 }
 function receive(msg,sender){guard();if(!bounded(sender,0,n-1)||sender===localSeat||msg?.schema!==wire.schema||msg.key!==key||msg.epoch!==epoch||msg.seat!==sender)throw Error('Wrong co-op identity or sender seat');
  if(msg.kind==='ready'){readySeats.add(sender);return;}
  if(msg.kind!=='input'||!readySeats.has(sender)||!bounded(msg.frame,Math.max(delay,committed-OLD_WINDOW),Math.min(MAX_FRAMES-1,next+LOOKAHEAD+delay))||!supported(msg.keys))throw Error('Unexpected co-op input');put(msg.frame,sender,msg.keys);
 }
 function sample(keys){guard();if(!supported(keys))throw Error('Invalid sampled co-op keys');if(readySeats.size!==n)return false;
  const f=next+delay;if(f>=MAX_FRAMES)return false;if(received.get(f)?.[localSeat]!=null)return false;
  const packet={...wire,kind:'input',frame:f,keys};put(f,localSeat,keys);try{send(packet);}catch(e){poison=true;throw e;}return true;
 }
 function row(frame,keys,prev){return Object.freeze({frame,inputs:Object.freeze(keys.map((held,player)=>Object.freeze({player,held,pressed:held&~prev[player],released:prev[player]&~held,predicted:false})))});}
 function simulate(f,prev,speculative){const keys=controls(f,prev),r=row(f,keys,prev),token=speculative?engine.checkpoint():null;
  let effects;try{effects=engine.step(r);}catch(e){if(token!==null)engine.release(token);throw e;}pending.set(f,{keys,r,token,effects});return keys;
 }
 function materialize(){let prev=last,first=-1;for(let f=committed;f<next;f++){const actual=controls(f,prev),old=pending.get(f);if(actual.some((v,i)=>v!==old.keys[i])){first=f;break;}prev=old.keys;}
  if(first!==-1){const t=pending.get(first).token;if(t===null)throw Error('Co-op correction crossed confirmed history');engine.restore(t);
   for(let f=first;f<next;f++){const old=pending.get(f);if(old.token!==null)engine.release(old.token);pending.delete(f);}
   let speculative=false;for(let f=committed;f<first;f++)if(!full(f))speculative=true;
   for(let f=first;f<next;f++){speculative||=!full(f);prev=simulate(f,prev,speculative);replayed++;}
  }
  while(committed<next&&full(committed)){const {r,keys,token,effects}=pending.get(committed);if(token!==null)engine.release(token);
    last=keys;settled.set(committed,keys);pending.delete(committed);received.delete(committed);settled.delete(committed-OLD_WINDOW);committed++;onConfirmed(r,effects);}
 }
 function run(cb){guard();busy=true;try{return cb();}catch(e){poison=true;throw e;}finally{busy=false;}}
 const flush=()=>run(materialize);
 function advance({allowPrediction=true}={}){return run(()=>{if(readySeats.size!==n||next>=MAX_FRAMES)return false;materialize();
  if(received.get(next)?.[localSeat]==null)return false;const speculative=next!==committed||!full(next);
  if(speculative&&(!allowPrediction||next-committed>=window||engine.requiresConfirmedFrame?.()))return false;
  simulate(next,next===committed?last:pending.get(next-1).keys,speculative);next++;materialize();return true;});}
 function close(){if(closed)return;if(busy)throw Error('Cannot close co-op in a frame callback');closed=true;for(const p of pending.values())if(p.token!==null)engine.release(p.token);pending.clear();received.clear();settled.clear();engine.close();}
 return Object.freeze({announce,receive,sample,advance,flush,close,needsSample:()=>next+delay<MAX_FRAMES&&received.get(next+delay)?.[localSeat]==null,status:()=>Object.freeze({ready:!closed&&!poison&&readySeats.size===n,next,committed,replayed,closed,faulted:poison})});
}
