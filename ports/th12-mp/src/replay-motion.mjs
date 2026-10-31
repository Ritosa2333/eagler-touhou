// Explicit, versioned in-file extension only for movement unrepresentable by
// the retail three-word key stream. Ordinary recordings have no extension.
import {readPracticeMetadata} from './practice/bridge.mjs';
import {validateDamageMetadata} from './practice/damage-sources.mjs';
const encoder=new TextEncoder(),decoder=new TextDecoder('utf-8',{fatal:true}),MAGIC='TH12WEB1';
const bytesOf=value=>value instanceof Uint8Array?value:new Uint8Array(value);
function crc32(bytes){let crc=0xffffffff;for(const byte of bytes){crc^=byte;for(let bit=0;bit<8;bit++)crc=(crc>>>1)^((crc&1)?0xedb88320:0);}return (crc^0xffffffff)>>>0;}
export function appendMotionReplay(value,stages,unlimited=false,{practice=null,damage=null}={}){
  practice=readPracticeMetadata(practice);damage=validateDamageMetadata(damage);
  if(!stages.some(stage=>stage.motion?.length)&&!practice&&!damage)return value;
  const payload=encoder.encode(JSON.stringify({schema:'th12-web-motion/1',coordinateScale:128,unlimited:!!unlimited,practice,damage,stages:stages.map(stage=>({number:stage.number,deltas:stage.motion??[]}))}));
  if(payload.length>16*1024*1024)throw Error('录像扩展过大');
  const input=bytesOf(value),out=new Uint8Array(input.length+16+payload.length),v=new DataView(out.buffer);
  out.set(input);out.set(encoder.encode(MAGIC),input.length);v.setUint32(input.length+8,payload.length,true);v.setUint32(input.length+12,crc32(payload),true);out.set(payload,input.length+16);return out;
}
export function readMotionReplay(source,stages){
  const bytes=bytesOf(source),v=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);let at=36+v.getUint32(28,true);
  while(at+12<=bytes.length&&v.getUint32(at,true)===0x52455355){const length=v.getUint32(at+4,true);if(length<12||length>bytes.length-at)throw Error('录像说明块无效');at+=length;}
  if(at===bytes.length||at+8>bytes.length||!Array.from(MAGIC,(letter,index)=>bytes[at+index]===letter.charCodeAt(0)).every(Boolean))return null;
  if(at+16>bytes.length)throw Error('录像扩展头不完整');
  const size=v.getUint32(at+8,true);if(size>16*1024*1024||at+16+size!==bytes.length)throw Error('录像扩展长度无效');
  const payload=bytes.subarray(at+16);if(crc32(payload)!==v.getUint32(at+12,true))throw Error('录像扩展校验失败');
  const data=JSON.parse(decoder.decode(payload));
  if(data.schema!=='th12-web-motion/1'||data.coordinateScale!==128||typeof data.unlimited!=='boolean'||!Array.isArray(data.stages)||data.stages.length!==stages.length)throw Error('录像移动扩展格式无效');
  const indexed=new Map();
  for(const record of data.stages){
    const stage=stages.find(stage=>stage.number===record.number);if(!stage||indexed.has(stage.number)||!Array.isArray(record.deltas)||record.deltas.length>stage.frames)throw Error('录像移动扩展关卡无效');
    const deltas=new Map();let previous=-1;
    for(const row of record.deltas){
      if(!Array.isArray(row)||row.length!==3||!row.every(Number.isInteger)||row[0]<=previous||row[0]>=stage.frames||Math.abs(row[1])>0x10000||Math.abs(row[2])>0x10000)throw Error('录像移动扩展帧无效');
      previous=row[0];deltas.set(row[0],row.slice(1));
    }
    indexed.set(stage.number,deltas);
  }
  return {schema:data.schema,unlimited:data.unlimited,practice:readPracticeMetadata(data.practice),damage:validateDamageMetadata(data.damage),motion:(stage,frame)=>indexed.get(stage)?.get(frame)??null};
}
