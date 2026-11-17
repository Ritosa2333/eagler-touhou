import {multiplayerRulesFingerprint} from './multiplayer-rules.mjs';
const nativeInputBits=763,maxFrame=2147483647,maxFutureFrames=60;
const packetFields=['matchId','epoch','player','frame','held'];
const integer=(value,min,max)=>Number.isSafeInteger(value)&&value>=min&&value<=max;
function copyPacket(input){
 if(!input||Array.isArray(input)||![Object.prototype,null].includes(Object.getPrototypeOf(input)))throw Error('Input packet requires a plain data object');
 const rows=Object.getOwnPropertyDescriptors(input),keys=Reflect.ownKeys(rows),copy={};
 if(keys.length!==packetFields.length||keys.some(key=>typeof key!=='string'||!packetFields.includes(key)))throw Error('Unknown or missing input packet field');
 for(const key of packetFields){if(!rows[key]||!Object.hasOwn(rows[key],'value'))throw Error('Input packet needs data property fields');copy[key]=rows[key].value;}
 return copy;
}
const freezeFrame=(frame,inputs)=>Object.freeze({frame,inputs:Object.freeze(inputs.map(row=>Object.freeze(row)))});
export async function createMultiplayerInputLedger({rules,allowedMask,localPlayer}){
 if(!integer(localPlayer,0,1))throw Error('Invalid local player');
 if(!integer(allowedMask,0,nativeInputBits)||(allowedMask&~nativeInputBits))throw Error('Unsupported native capability mask');
 const fingerprint=await multiplayerRulesFingerprint(rules);
 const raw=[new Map(),new Map()],used=new Map(),prefix=[rules.inputDelay-1,rules.inputDelay-1];
 for(let frame=0;frame<rules.inputDelay;frame++)for(const table of raw)table.set(frame,0);
 let floorFrame=0,nextFrame=0,confirmedThrough=rules.inputDelay-1,dirtyFrame=null,revision=0,closed=false;
 let lastHeld=[0,0],beforeHeld=[0,0],plans=new WeakMap();
 const live=()=>{if(closed)throw Error('Input ledger is closed');};
 function status(){return Object.freeze({closed,nextFrame,floorFrame,confirmedThrough,dirtyFrame,storedPackets:raw[0].size+raw[1].size,usedFrames:used.size});}
 function receipt(duplicate){return Object.freeze({duplicate,confirmedThrough,dirtyFrame});}
 function receive(input,sourcePlayer){
  live();const p=copyPacket(input);
  if(!integer(sourcePlayer,0,1)||p.player!==sourcePlayer)throw Error('Input source player mismatch');
  if(p.matchId!==rules.matchId||p.epoch!==rules.epoch)throw Error('Input match or epoch mismatch');
  if(!integer(p.frame,0,maxFrame)||!integer(p.held,0,65535)||(p.held&~allowedMask))throw Error('Input frame or capability mask invalid');
  if(p.frame<floorFrame)throw Error('Input is older than retained history');
  if(p.frame>nextFrame+maxFutureFrames)throw Error('Input is beyond the future window');
  const table=raw[p.player];
  if(table.has(p.frame)){if(table.get(p.frame)!==p.held)throw Error('Conflicting duplicate input');return receipt(true);}
  table.set(p.frame,p.held);revision++;
  while(table.has(prefix[p.player]+1))prefix[p.player]++;
  confirmedThrough=Math.min(...prefix);
  const previous=used.get(p.frame);
  if(previous&&previous.inputs[p.player].held!==p.held)dirtyFrame=dirtyFrame===null?p.frame:Math.min(dirtyFrame,p.frame);
  return receipt(false);
 }
 function rowAt(frame,previous){
  return freezeFrame(frame,raw.map((table,player)=>{
   const known=table.has(frame),held=known?table.get(frame):previous[player];
   return{player,held,pressed:held&~previous[player],released:previous[player]&~held,predicted:!known};
  }));
 }
 function prune(){
  const floor=Math.max(0,Math.min(confirmedThrough+1,nextFrame)-rules.rollbackWindow);
  if(floor<=floorFrame)return;
  beforeHeld=used.get(floor-1).inputs.map(row=>row.held);
  for(const table of [...raw,used])for(const frame of table.keys())if(frame<floor)table.delete(frame);
  floorFrame=floor;
 }
 function consume(){
  live();if(dirtyFrame!==null)throw Error('Apply input correction before consuming more frames');
  if(nextFrame>maxFrame||nextFrame>confirmedThrough+rules.rollbackWindow)throw Error('Input prediction window exhausted; stall until confirmation');
  const row=rowAt(nextFrame,lastHeld);used.set(nextFrame,row);nextFrame++;lastHeld=row.inputs.map(input=>input.held);prune();return row;
 }
 function queueLocal(held){
  live();const packet=Object.freeze({matchId:rules.matchId,epoch:rules.epoch,player:localPlayer,frame:nextFrame+rules.inputDelay,held});
  return Object.freeze({packet,receipt:receive(packet,localPlayer)});
 }
 function prepareCorrection(){
  live();if(dirtyFrame===null)return null;
  let previous=dirtyFrame===floorFrame?beforeHeld:used.get(dirtyFrame-1).inputs.map(input=>input.held);
  const frames=[];
  for(let frame=dirtyFrame;frame<nextFrame;frame++){const row=rowAt(frame,previous);frames.push(row);previous=row.inputs.map(input=>input.held);}
  const plan=Object.freeze({fromFrame:dirtyFrame,toFrame:nextFrame-1,frames:Object.freeze(frames)});
  plans.set(plan,{revision});return plan;
 }
 function acceptCorrection(plan){
  live();const owned=plans.get(plan);if(!owned)throw Error('Correction receipt belongs to another owner');
  if(owned.revision!==revision||dirtyFrame===null||plan.fromFrame!==dirtyFrame||plan.toFrame!==nextFrame-1)throw Error('Correction receipt is stale');
  for(const row of plan.frames)used.set(row.frame,row);
  lastHeld=plan.frames.at(-1).inputs.map(input=>input.held);dirtyFrame=null;revision++;plans=new WeakMap();prune();
 }
 function exportConfirmed(){
  live();if(dirtyFrame!==null)throw Error('Pending correction prevents confirmed export');
  const through=Math.min(confirmedThrough,nextFrame-1),frames=[];
  for(let frame=floorFrame;frame<=through;frame++){
   const row=used.get(frame);frames.push(freezeFrame(frame,row.inputs.map(input=>({...input,predicted:false}))));
  }
  return Object.freeze({schema:'th128-multiplayer-input-chunk/1',fingerprint,matchId:rules.matchId,epoch:rules.epoch,allowedMask,
   fromFrame:floorFrame,toFrame:through,beforeHeld:Object.freeze([...beforeHeld]),frames:Object.freeze(frames),completeReplay:false,durable:false});
 }
 function close(){if(closed)return;closed=true;for(const table of [...raw,used])table.clear();plans=new WeakMap();dirtyFrame=null;}
 return Object.freeze({status,receive,consume,queueLocal,prepareCorrection,acceptCorrection,exportConfirmed,close});
}
