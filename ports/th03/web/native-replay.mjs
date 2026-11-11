import {MAX_INPUT} from './touch-input.js';
const encoder=new TextEncoder(),decoder=new TextDecoder('utf-8',{fatal:true});
const hex=bytes=>Array.from(bytes,v=>v.toString(16).padStart(2,'0')).join('');
export async function encodeReplay(rows,focus,native,extra={}){
 const payload={...extra,schema:'eagler-th03-native/1',focus,rows,native:Array.from(native)};
 const body=encoder.encode(JSON.stringify(payload));const sha=hex(new Uint8Array(await crypto.subtle.digest('SHA-256',body)));
 return encoder.encode(JSON.stringify({sha,body:Array.from(body)}));
}
export async function decodeReplay(bytes){
 if(!(bytes instanceof Uint8Array)||bytes.length>32*1024*1024||!bytes.length)throw Error('录像大小无效');
 let envelope;try{envelope=JSON.parse(decoder.decode(bytes));}catch{throw Error('录像格式无效');}
 if(!envelope||!/^[0-9a-f]{64}$/.test(envelope.sha)||!Array.isArray(envelope.body)||envelope.body.length>24*1024*1024||envelope.body.some(n=>!Number.isInteger(n)||n<0||n>255))throw Error('录像格式无效');
 const body=Uint8Array.from(envelope.body),sha=hex(new Uint8Array(await crypto.subtle.digest('SHA-256',body)));if(sha!==envelope.sha)throw Error('录像校验失败');
 let replay;try{replay=JSON.parse(decoder.decode(body));}catch{throw Error('录像格式无效');}
 const solo=replay?.mode==='solo';
 const validRow=row=>Array.isArray(row)&&(solo?row.length===5&&row.every(Number.isSafeInteger)&&row[0]>=0&&row[0]<=63&&row[1]>=0&&row[1]<=0xffff&&row[2]>=0&&row[2]<=1&&row[3]>=0&&row[3]<=MAX_INPUT&&(row[4]===0||row[4]===1):row.length===2&&row.every(n=>Number.isSafeInteger(n)&&n>=0&&n<=MAX_INPUT));
 const files=replay?.files;
 if(replay?.schema!=='eagler-th03-native/1'||replay.mode!==undefined&&!solo||typeof replay.focus!=='boolean'||!Array.isArray(replay.rows)||!replay.rows.length||replay.rows.length>216000||replay.rows.some(row=>!validRow(row))||solo&&(!files||typeof files!=='object'||Array.isArray(files)||Object.keys(files).some(k=>!['YUME.CFG','YUME.NEM'].includes(k))||Object.values(files).some(a=>!Array.isArray(a)||a.length>1048576||a.some(n=>!Number.isInteger(n)||n<0||n>255)))||!Array.isArray(replay.native)||!replay.native.length||replay.native.length>16*1024*1024||replay.native.some(n=>!Number.isInteger(n)||n<0||n>255))throw Error('录像数据无效');
 return replay;
}
