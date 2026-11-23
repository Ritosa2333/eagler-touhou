import {openConfirmedTimeline,historyRules,historySchemas,historyWire} from './confirmed-history.mjs';

export const TH128_SPECTATOR_MARKER=0x48;
const checkpointSchema=1,maxFrames=108000,headerBytes=32,stateLimit=4096;
const encoder=new TextEncoder(),decoder=new TextDecoder('utf-8',{fatal:true});
const abi=r=>r.schema==='th128-coop/1'?2:1,realm=r=>abi(r)===2?'th128-coop-spectator-checkpoint/1':'th128-mp-spectator-checkpoint/1';
// This ABI has a single stage checkpoint (serial 1). Terminal serials 2–5
// distinguish why this confirmed history ended without altering the shared
// Relay End layout. A multi-stage ABI must assign fresh serial semantics.
const endReasons=['completed','restart','left','disconnect'];
const idPattern=/^[a-zA-Z0-9_-]{1,64}$/,sessionPattern=/^[a-f0-9]{16}$/;
const within=(n,min,max)=>Number.isSafeInteger(n)&&n>=min&&n<=max;
const view=bytes=>new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);
function sessionId(header){
  const session=header.fingerprint.slice(0,16);
  if(!sessionPattern.test(session)||/^0+$/.test(session))throw new Error('Invalid TH128 spectator session identity');
  return session;
}
function endPacket(kind,scope,frame,serial,state=new Uint8Array()){
  const packet=new Uint8Array(headerBytes+(kind===1?8+state.length:0)),wire=view(packet);
  packet.set([0x45,TH128_SPECTATOR_MARKER,0x53,0x56,2,kind,scope.players,0]);
  wire.setBigUint64(8,BigInt('0x'+scope.session),true);wire.setUint32(16,scope.abi,true);
  wire.setUint32(20,frame,true);wire.setUint32(24,scope.stage,true);wire.setUint32(28,serial,true);
  if(kind===1){wire.setUint32(32,checkpointSchema,true);wire.setUint32(36,state.length,true);packet.set(state,40);}
  return packet;
}
function upload(packet){const envelope=new Uint8Array(1+packet.length);envelope[0]=0xe8;envelope.set(packet,1);return envelope;}

export async function createTh128SpectatorEncoder(inputHeader,build){
  const {header}=await openConfirmedTimeline(inputHeader,build);
  const rules=historyRules(header.rules),session=sessionId(header),scope={session,stage:rules.stageId,players:rules.players,abi:abi(rules)};
  const checkpoint=encoder.encode(JSON.stringify({schema:realm(rules),header}));
  if(checkpoint.length>stateLimit)throw new Error('TH128 spectator checkpoint exceeds Relay limit');
  let nextFrame=0,previous=Array(rules.players).fill(0),opened=false,ended=false;
  function open(){if(opened)throw new Error('Spectator checkpoint was already opened');opened=true;return upload(endPacket(1,scope,0,1,checkpoint));}
  function frame(row){
    if(!opened||ended)throw new Error('Spectator upload is not open');
    if(row?.frame!==nextFrame||nextFrame>=maxFrames)throw new Error('Expected a contiguous spectator frame');
    if(!Array.isArray(row.inputs)||row.inputs.length!==rules.players)throw new Error('Spectator frame needs both seats');
    const pair=Array.from({length:rules.players},(_,seat)=>{
      const input=row.inputs[seat];
      if(!input||input.player!==seat||input.predicted!==false)throw new Error('Spectator upload needs confirmed inputs');
      const {held}=input;
      if(!within(held,0,65535)||(held&~header.allowedMask)||(nextFrame<rules.inputDelay&&held!==0)||
        input.pressed!==(held&~previous[seat])||input.released!==(previous[seat]&~held))throw new Error('Invalid confirmed spectator input');
      return held;
    });
    const packet=new Uint8Array(24+12*rules.players),wire=view(packet);
    packet.set([0x45,TH128_SPECTATOR_MARKER,0x4e,0x50,4,3,rules.players,0]);
    wire.setBigUint64(8,BigInt('0x'+session),true);wire.setUint32(16,nextFrame,true);wire.setUint32(20,scope.abi,true);
    pair.forEach((held,i)=>wire.setUint16(24+12*i,held,true));
    previous=pair;nextFrame++;return upload(packet);
  }
  function finish(reason){
    const index=endReasons.indexOf(reason);
    if(!opened||ended||index<0)throw new Error('Cannot end the TH128 spectator upload');
    ended=true;return upload(endPacket(3,scope,nextFrame,index+2));
  }
  return Object.freeze({announce:()=>({type:'spectator-session',session,seed:rules.seed,generation:rules.epoch-1}),open,frame,finish});
}

export function createTh128SpectatorDecoder({build,matchId,expectedRules}){
  if(typeof matchId!=='string'||!idPattern.test(matchId))throw new Error('Invalid spectator match identity');
  let announcement=null,timeline=null,rules=null,serial=0,bootstrapped=false;
  function status(){
    const {nextFrame=0,end=null}=timeline?.status()??{};
    return Object.freeze({epoch:rules?.epoch??null,nextFrame,end,bootstrapped});
  }
  function announce(value){
    if(!value||Object.keys(value).length!==4||value.type!=='spectator-session'||typeof value.session!=='string'||
        !sessionPattern.test(value.session)||/^0+$/.test(value.session)||!within(value.seed,0,65535)||
        !within(value.generation,0,0x7ffffffe))throw new Error('Invalid spectator session announcement');
    if(announcement){
      if(value.generation===announcement.generation&&value.session===announcement.session&&value.seed===announcement.seed)return;
      if(value.generation!==announcement.generation+1||value.session===announcement.session)throw new Error('Unexpected spectator generation');
    }
    announcement=Object.freeze({...value});
  }
  function scope(){return {matchId,epoch:rules.epoch,fingerprint:timeline.header.fingerprint};}
  async function accept(bytes){
    if(!announcement)throw new Error('Spectator session announcement is missing');
    if(!(bytes instanceof Uint8Array)||bytes.length<headerBytes||bytes.length>40+stateLimit||bytes[0]!==0x45||
      bytes[1]!==TH128_SPECTATOR_MARKER||![2,3].includes(bytes[6])||bytes[7]!==0)throw new Error('Invalid TH128 spectator envelope');
    const data=view(bytes),session=data.getBigUint64(8,true).toString(16).padStart(16,'0');
    if(session!==announcement.session)throw new Error('Spectator session identity mismatch');
    if(bytes[2]===0x53&&bytes[3]===0x56&&bytes[4]===2){
      const frame=data.getUint32(20,true),stage=data.getUint32(24,true),sequence=data.getUint32(28,true);
      if(![1,2].includes(data.getUint32(16,true))||frame>maxFrames)throw new Error('Spectator stage ABI or frame mismatch');
      if(bytes[5]===1){
        if(rules?.epoch===announcement.generation+1||bytes.length<=40||frame!==0||sequence!==1||
          data.getUint32(32,true)!==checkpointSchema||data.getUint32(36,true)!==bytes.length-40)throw new Error('Invalid TH128 spectator checkpoint');
        const state=JSON.parse(decoder.decode(bytes.subarray(40)));
        if(!state||state.schema!==realm(state.header?.rules??{})||Object.keys(state).length!==2)throw new Error('Unsupported TH128 spectator initializer');
        const current=announcement,track=await openConfirmedTimeline(state.header,build),selection=historyRules(track.header.rules);
        if(expectedRules){const expected=historyWire(expectedRules),remote=historyWire(selection);
          if(JSON.stringify({...expected,epoch:remote.epoch})!==JSON.stringify(remote))throw new Error('Spectator venue rules differ');}
        if(bytes[6]!==selection.players||data.getUint32(16,true)!==abi(selection)||current!==announcement||selection.matchId!==matchId||selection.epoch!==current.generation+1||selection.seed!==current.seed||
          selection.stageId!==stage||sessionId(track.header)!==session)throw new Error('Spectator checkpoint identity mismatch');
        rules=selection;timeline=track;serial=sequence;bootstrapped=false;return 'checkpoint';
      }
      if(!timeline||bytes[6]!==rules.players||data.getUint32(16,true)!==abi(rules)||rules.epoch!==announcement.generation+1||stage!==rules.stageId||frame!==timeline.status().nextFrame||bytes.length!==headerBytes)
        throw new Error('Spectator stage identity or frame mismatch');
      if(bytes[5]===4&&sequence===serial){bootstrapped=true;return 'bootstrap';}
      if(bytes[5]===3&&serial===1&&within(sequence,2,5)){
        timeline.finish({schema:historySchemas(rules).end,...scope(),frameCount:frame,reason:endReasons[sequence-2]});
        serial=sequence;return 'end';
      }
      throw new Error('Unexpected TH128 spectator stage event');
    }
    if(!timeline||rules.epoch!==announcement.generation+1||bytes[6]!==rules.players||bytes.length!==24+12*rules.players||bytes[2]!==0x4e||bytes[3]!==0x50||
      bytes[4]!==4||bytes[5]!==3||data.getUint32(20,true)!==abi(rules))throw new Error('Invalid TH128 spectator input identity');
    const frame=data.getUint32(16,true);
    if(frame!==timeline.status().nextFrame||frame>=maxFrames)throw new Error('Expected a contiguous spectator input');
        const held=Array.from({length:rules.players},(_,i)=>{const at=24+12*i;if(bytes.subarray(at+2,at+12).some(Boolean))throw new Error('Unexpected TH128 spectator movement payload');return data.getUint16(at,true);});
    timeline.accept({schema:historySchemas(rules).chunk,...scope(),fromFrame:frame,frames:[held]});
return 'frame';
  }
  function retain(){
    if(!timeline)return null;
    // This view keeps a paused viewer's already received tail readable across
    // a newer checkpoint, without giving it any of the stream's write methods.
    const {header,read,status,replayBytes}=timeline;
    return Object.freeze({header,read,status,replayBytes});
  }
  return Object.freeze({announce,accept,status,retain,read:frame=>timeline?.read(frame)??null,header:()=>timeline?.header??null});
}
