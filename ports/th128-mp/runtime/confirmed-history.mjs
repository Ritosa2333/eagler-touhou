import {createMultiplayerRules,multiplayerRulesFingerprint,multiplayerRulesWire} from '../../th128/site/runtime/multiplayer-rules.mjs';

const legacySchemas={header:'th128-mp-history/1',chunk:'th128-mp-confirmed/1',end:'th128-mp-end/1',replay:'th128-mp-replay/1'};
const coopSchemas={header:'th128-coop-history/1',chunk:'th128-coop-confirmed/1',end:'th128-coop-end/1',replay:'th128-coop-replay/1'};
import {makeCoopRules,coopFingerprint,coopWire,TH128_COOP_FRAME_LIMIT} from './coop-rules.mjs';
const isCoop=r=>r?.schema==='th128-coop/1';
export const historySchemas=rules=>isCoop(rules)?coopSchemas:legacySchemas;
export const historyRules=wire=>isCoop(wire)?makeCoopRules(wire):createMultiplayerRules(wire);
const fingerprint=r=>isCoop(r)?coopFingerprint(r):multiplayerRulesFingerprint(r);
export const historyWire=r=>isCoop(r)?coopWire(r):multiplayerRulesWire(r);
const parallelFrameLimit=216000,maxReplayBytes=8*1024*1024,maxChunk=120,nativeMask=763;
const reasons=new Set(['completed','restart','left','disconnect']);
const integer=(v,min,max)=>Number.isSafeInteger(v)&&v>=min&&v<=max;
const elapsedInput=(held,prev,player)=>Object.freeze({player,held,pressed:held&~prev,released:prev&~held,predicted:false});
function fields(value,names){
  if(!value||![Object.prototype,null].includes(Object.getPrototypeOf(value)))throw new TypeError('Invalid multiplayer history data');
  const descriptors=Object.getOwnPropertyDescriptors(value);
  if(Reflect.ownKeys(descriptors).length!==names.length||names.some(name=>!descriptors[name]||!Object.hasOwn(descriptors[name],'value')))
    throw new Error('Unexpected multiplayer history fields');
  return Object.fromEntries(names.map(name=>[name,descriptors[name].value]));
}
function checkMask(mask){
  if(!integer(mask,1,nativeMask)||(mask&~nativeMask))throw new RangeError('Invalid history native mask');
}

function makeTrack(header,rules){
  const {fingerprint,allowedMask}=header,{matchId,epoch}=rules;
  const scope={matchId,epoch,fingerprint},schemas=historySchemas(rules),n=rules.players,seats=Array.from({length:n},(_,i)=>i);
  const maxFrames=isCoop(rules)?TH128_COOP_FRAME_LIMIT:parallelFrameLimit;
  const data=new Uint16Array(maxFrames*n);
  let length=0,finished=null;
  const status=()=>Object.freeze({nextFrame:length,end:finished?.reason??null});
  const matches=m=>m.matchId===matchId&&m.epoch===epoch&&m.fingerprint===fingerprint;
  function checkHeld(held,frame){
    if(!integer(held,0,nativeMask)||(held&~allowedMask)||(frame<rules.inputDelay&&held!==0))throw new RangeError('History contains an invalid held input mask');
  }
  function read(frame){
    if(!integer(frame,0,length-1))return null;
    return Object.freeze({frame,inputs:Object.freeze(seats.map(player=>elapsedInput(data[frame*n+player],frame?data[(frame-1)*n+player]:0,player)))});
  }
  function append(row){
    if(finished)throw new Error('History is sealed');
    if(row?.frame!==length||length>=maxFrames)throw new Error('Expected the next contiguous confirmed frame');
    if(!Array.isArray(row.inputs)||row.inputs.length!==n)throw new Error('A confirmed frame requires both seats');
    const pair=seats.map(player=>{const input=row.inputs[player];
      if(input?.player!==player||input.predicted!==false)throw new Error('History accepts only confirmed inputs');
      checkHeld(input.held,length);
      const expected=elapsedInput(input.held,length?data[(length-1)*n+player]:0,player);
      if(input.pressed!==expected.pressed||input.released!==expected.released)throw new Error('History input edge differs from preceding held state');
      return input.held;
    });
    data.set(pair,length*n);length++;
  }
  function chunk(fromFrame,count=maxChunk){
    if(!integer(fromFrame,0,length)||!integer(count,1,maxChunk))throw new RangeError('Invalid history chunk range');
    if(fromFrame===length)return null;
    const frames=[];
    for(let frame=fromFrame;frame<Math.min(fromFrame+count,length);frame++)frames.push(Object.freeze(seats.map(i=>data[frame*n+i])));
    return Object.freeze({schema:schemas.chunk,...scope,fromFrame,frames:Object.freeze(frames)});
  }
  function accept(chunk){
    if(finished)throw new Error('Timeline has ended');
    const m=fields(chunk,['schema','matchId','epoch','fingerprint','fromFrame','frames']);
    if(m.schema!==schemas.chunk||!matches(m))throw new Error('History chunk identity mismatch');
    if(m.fromFrame!==length||!Array.isArray(m.frames)||!integer(m.frames.length,1,maxChunk)||length+m.frames.length>maxFrames)
      throw new Error('Expected a contiguous history chunk');
    const copy=new Uint16Array(m.frames.length*n);
    for(let i=0;i<m.frames.length;i++){
      const frame=m.frames[i];
      if(!Array.isArray(frame)||frame.length!==n)throw new Error('History chunk must contain both seats');
      for(let seat=0;seat<n;seat++){const input=frame[seat];checkHeld(input,length+i);copy[i*n+seat]=input;}
    }
    data.set(copy,n*length);length+=m.frames.length;
  }
  function finish(tail){
    const m=fields(tail,['schema','matchId','epoch','fingerprint','frameCount','reason']);
    if(m.schema!==schemas.end||!matches(m)||m.frameCount!==length||!reasons.has(m.reason))throw new Error('History tail does not match the accepted frames');
    if(finished&&finished.reason!==m.reason)throw new Error('History tail changed after sealing');
    return finished??=Object.freeze(m);
  }
  function seal(reason){return finish({schema:schemas.end,...scope,frameCount:length,reason});}
  function replayBytes(){
    if(!finished)throw new Error('Replay requires a sealed history');
    return new TextEncoder().encode(JSON.stringify({schema:schemas.replay,header,tail:finished,held:Array.from(data.subarray(0,n*length))}));
  }
  function loadReplay(held,tail){
    if(!Array.isArray(held)||held.length%n||held.length>maxFrames*n)throw new Error('Invalid replay input length');
    for(let i=0;i<held.length;i++)checkHeld(held[i],Math.floor(i/n));
    length=held.length/n;data.set(held);finish(tail);
  }
  return {status,read,append,chunk,accept,finish,seal,replayBytes,loadReplay};
}

export async function createConfirmedHistory({rules,allowedMask}){
  checkMask(allowedMask);
  const header=Object.freeze({schema:historySchemas(rules).header,rules:historyWire(rules),fingerprint:await fingerprint(rules),allowedMask});
  const {status,read,append,chunk,seal,replayBytes}=makeTrack(header,rules);
  return Object.freeze({header,status,read,append,chunk,seal,replayBytes});
}
async function validateHeader(value,build){
  const input=fields(value,['schema','rules','fingerprint','allowedMask']);
  if(input.schema!==historySchemas(input.rules).header)throw new Error('Unknown multiplayer history schema');
  const rules=historyRules(input.rules);
  if(rules.coreSHA256!==build?.coreSHA256||rules.resourceSHA256!==build?.resourceSHA256)throw new Error('Multiplayer history build or resource mismatch');
  if(input.fingerprint!==await fingerprint(rules))throw new Error('Multiplayer history rule fingerprint mismatch');
  checkMask(input.allowedMask);
  return [Object.freeze({...input,rules:historyWire(rules)}),rules];
}
export async function openConfirmedTimeline(header,build){
  const [validated,rules]=await validateHeader(header,build);
  const {accept,finish,status,read,replayBytes}=makeTrack(validated,rules);
  return Object.freeze({header:validated,accept,finish,status,read,replayBytes});
}
export async function readMultiplayerReplay(bytes,build){
  if(!(bytes instanceof Uint8Array)||!integer(bytes.byteLength,1,maxReplayBytes))throw new Error('Invalid multiplayer replay size');
  const parsed=fields(JSON.parse(new TextDecoder('utf-8',{fatal:true}).decode(bytes)),['schema','header','tail','held']);
  if(parsed.schema!==historySchemas(parsed.header?.rules).replay)throw new Error('Unknown multiplayer replay schema');
  const [header,rules]=await validateHeader(parsed.header,build);
  const {loadReplay,status,read}=makeTrack(header,rules);
  loadReplay(parsed.held,parsed.tail);
  return Object.freeze({header,status,read});
}
