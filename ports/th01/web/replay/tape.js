// Browser-owned input history. Never stores speculative rows in the published prefix.
import {validNativeCommand} from './native-command-schema.js';
export const VERSION='pc98-confirmed-tape/1',MAX_FRAMES=216000,MAX_RAW=48*1024*1024;
export const MAX_POINT_BYTES=16*1024*1024;
export const MAX_POINT_TOTAL=128*1024*1024;
export function toBase64(bytes){let s='';for(let n=0;n<bytes.length;n+=16384)s+=String.fromCharCode(...bytes.subarray(n,n+16384));return btoa(s);}
export function pointBytes(point){return Uint8Array.from(atob(point.data),c=>c.charCodeAt(0));}
export function stable(value){
  if(value===undefined)return 'null';
  if(value===null||typeof value!=='object')return JSON.stringify(value);
  if(Array.isArray(value))return '['+value.map(stable).join(',')+']';
  return '{'+Object.keys(value).filter(k=>value[k]!==undefined).sort().map(k=>JSON.stringify(k)+':'+stable(value[k])).join(',')+'}';
}
import {sha256} from './sha256.js';
export const digest=bytes=>sha256(bytes instanceof ArrayBuffer?new Uint8Array(bytes):bytes);
export function randomId(){return Array.from(crypto.getRandomValues(new Uint8Array(16)),b=>b.toString(16).padStart(2,'0')).join('');}
export async function fingerprint(paths){
  const entries=await Promise.all(paths.map(async path=>{const r=await fetch(new URL(path,import.meta.url),{cache:'no-cache'});if(!r.ok)throw Error('录像资源读取失败：'+path);return [path,await digest(await r.arrayBuffer())];}));
  return digest(new TextEncoder().encode(stable(entries)));
}
export function validateHeader(h){
  if(h?.version!==VERSION||!['01','02','03','04'].includes(h.game)||!(h.adapter==='confirmed-worker/1'||h.game==='01'&&h.adapter==='native-local/1')||
    !Number.isInteger(h.players)||h.players<1||h.players>3||!Number.isInteger(h.hostSlot)||h.hostSlot<0||h.hostSlot>=h.players||
    !Number.isFinite(h.tickMs)||h.tickMs<10||h.tickMs>30||!h.settings||typeof h.settings!=='object'||
    !/^[a-f0-9]{64}$/.test(h.identity)||h.opening===undefined)throw Error('录像作品、版本或身份无效');
}
export function validateRow(row,h,frame){
  if(h.adapter==='native-local/1'){
    if(!row||!Array.isArray(row.inputs)||row.inputs.length!==1||!validNativeCommand(row.inputs[0])||row.offlineMask!==0||typeof row.proof!=='string'||row.proof.length>8192||((frame+1)%120===0)!==(row.hash!==null))throw Error('原生命令录像输入无效');return;
  }
  if(!row||!Array.isArray(row.inputs)||row.inputs.length!==h.players||
    row.inputs.some(n=>h.game==='03'?!validBattleInput(n):!Number.isSafeInteger(n)||n<0||n>=2**45)||
    !Number.isInteger(row.offlineMask)||row.offlineMask<0||row.offlineMask>=2**h.players||
    (h.game==='03'?row.offlineMask!==0:row.inputs.some((n,i)=>(row.offlineMask&(1<<i))&&n!==0))||
    typeof row.proof!=='string'||row.proof.length>8192||
    ((frame+1)%120===0)!==(row.hash!==null))throw Error('录像输入或校验字段无效');
}
function validBattleInput(v){
  return v&&Array.isArray(v.actions)&&v.actions.length<=16&&v.actions.every(a=>['up','down','left','right','shot','rapid','attack','focus'].includes(a))&&
    Number.isSafeInteger(v.touch)&&v.touch>=0&&v.touch<2**45&&typeof v.focusEnabled==='boolean'&&typeof v.points==='boolean'&&
    Array.isArray(v.commands)&&v.commands.length<=16&&v.commands.every(c=>['pause','resume','round','match','up','down','confirm','escape','hidden','visible'].includes(c));
}
export class ConfirmedTape{
  constructor(header){validateHeader(header);this.header=structuredClone(header);this.rows=[];this.pending=new Map();this.end=null;this.bytes=0;}
  stage(frame,row){
    if(this.end)return;
    if(!Number.isInteger(frame)||frame<this.rows.length||frame>this.rows.length+120)throw Error('录像预测窗口越界');
    validateRow(row,this.header,frame);this.pending.set(frame,structuredClone(row));
  }
  confirm(prefix,dirty=false){
    if(this.end||dirty)return false;
    if(!Number.isInteger(prefix)||prefix<this.rows.length||prefix>MAX_FRAMES)throw Error('录像确认范围越界');
    const batch=[];let bytes=0;
    for(let f=this.rows.length;f<prefix;f++){const row=this.pending.get(f);if(!row)throw Error('录像确认历史缺帧');batch.push(row);bytes+=stable(row).length*3;}
    if(this.bytes+bytes>MAX_RAW)throw Error('录像输入历史达到容量上限');
    for(let f=this.rows.length;f<prefix;f++)this.pending.delete(f);
    this.bytes+=bytes;this.rows.push(...batch);return true;
  }
  rewind(frame){if(frame<this.rows.length)throw Error('不能修改已发布录像');for(const f of this.pending.keys())if(f>=frame)this.pending.delete(f);}
  finish(reason='片段结束'){if(!this.end)this.end={frame:this.rows.length,reason:String(reason).slice(0,250)};this.pending.clear();}
  snapshot(){return {header:structuredClone(this.header),rows:structuredClone(this.rows),end:{frame:this.rows.length,reason:this.end?.reason||'途中导出的确认片段'}};}
}
export async function encodeTape(tape){
  validateTape(tape);const raw=new TextEncoder().encode(JSON.stringify(tape));if(raw.length>MAX_RAW)throw Error('录像超过容量');
  const payload=new Uint8Array(await new Response(new Blob([raw]).stream().pipeThrough(new CompressionStream('gzip'))).arrayBuffer());
  const header=new TextEncoder().encode('P98TAPE1'+await digest(payload));return new Blob([header,payload],{type:'application/octet-stream'});
}
export function validateTape(tape){
  if(tape?.header?.game!=='01'||tape?.header?.adapter!=='native-local/1'||tape?.header?.players!==1)throw Error('多人 Replay 文件已停用；多人仅支持实时旁观');
  validateHeader(tape?.header);if(!Array.isArray(tape.rows)||tape.rows.length>MAX_FRAMES||tape.end?.frame!==tape.rows.length||typeof tape.end.reason!=='string')throw Error('录像长度或结尾无效');
  tape.rows.forEach((r,f)=>validateRow(r,tape.header,f));
  if(tape.points!==undefined){
    if(!Array.isArray(tape.points)||tape.points.length>128)throw Error('录像检查点索引无效');let previous=-1,total=0;
    for(const p of tape.points){if(!Number.isInteger(p.frame)||p.frame<0||p.frame>tape.rows.length||p.frame<=previous||previous===-1&&p.frame!==0||!/^[a-f0-9]{64}$/.test(p.sha)||typeof p.data!=='string'||p.data.length>MAX_POINT_BYTES*4/3+4||p.data.length%4||/[^A-Za-z0-9+/=]/.test(p.data)||/=/.test(p.data.slice(0,-2)))throw Error('录像检查点范围无效');previous=p.frame;total+=p.data.length;}
    if(total>MAX_POINT_BYTES*4/3+4)throw Error('录像检查点达到容量上限');
  }
  return tape;
}
export async function decodeTape(file){
  if(file.size>MAX_RAW||file.size<73)throw Error('录像文件容量无效');const bytes=new Uint8Array(await file.arrayBuffer()),head=new TextDecoder().decode(bytes.subarray(0,72)),payload=bytes.subarray(72);
  if(head.slice(0,8)!=='P98TAPE1'||head.slice(8)!==await digest(payload))throw Error('录像文件损坏');
  const reader=new Blob([payload]).stream().pipeThrough(new DecompressionStream('gzip')).getReader(),parts=[];let size=0;
  for(;;){const {done,value}=await reader.read();if(done)break;size+=value.length;if(size>MAX_RAW){await reader.cancel();throw Error('录像解压超过容量');}parts.push(value);}
  return validateTape(JSON.parse(await new Blob(parts).text()));
}
