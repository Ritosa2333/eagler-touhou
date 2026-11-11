import {digest,stable} from './tape.js';
export const NATIVE_LAYOUT='th01-native-state/1';
// This exact ordinary core was audited with wasm-dis: no imports, one mutable
// i32 stack pointer, fixed 64-page memory and a fixed 3-entry function table.
// The element segment initializes that table; there are no table write/grow
// instructions. The isolated replica exports the existing global; code/data
// sections and the player's original module remain byte-for-byte unchanged.
export const AUDITED_CORE='5b424c9e372635ffe9711900c2610a5fb5fece5a1e91366171db545067d1bd17';
const HEAP=4194304,MAX_JSON=16384;
const leb=n=>{const bytes=[];do{let b=n&127;n>>>=7;bytes.push(b|(n?128:0));}while(n);return bytes;};
function uint(bytes,cursor){let value=0,shift=0;for(let n=0;n<5;n++){const b=bytes[cursor.at++];if(b===undefined)throw Error('原生模块截断');value|=(b&127)<<shift;if(!(b&128))return value>>>0;shift+=7;}throw Error('原生模块长度无效');}
export async function replicaBinary(bytes){
  if(await digest(bytes)!==AUDITED_CORE)throw Error('原生核心尚未通过完整状态布局审计');
  const parts=[bytes.subarray(0,8)],cursor={at:8};let exported=false;
  while(cursor.at<bytes.length){
    const begin=cursor.at,id=bytes[cursor.at++],size=uint(bytes,cursor),start=cursor.at,end=start+size;if(end>bytes.length)throw Error('原生模块区段越界');
    if(id===7){
      const count=uint(bytes,cursor),name=new TextEncoder().encode('__replay_stack'),extra=Uint8Array.from([...leb(name.length),...name,3,0]);
      const payload=Uint8Array.from([...leb(count+1),...bytes.subarray(cursor.at,end),...extra]);
      parts.push(Uint8Array.from([7,...leb(payload.length)]),payload);exported=true;
    }else parts.push(bytes.subarray(begin,end));cursor.at=end;
  }
  if(!exported)throw Error('原生模块缺少导出段');
  return new Uint8Array(await new Blob(parts).arrayBuffer());
}
export function captureNative(simulation,identity){
  const e=simulation.core;if(!(e.__replay_stack instanceof WebAssembly.Global)||e.memory.buffer.byteLength!==HEAP)throw Error('原生状态布局无效');
  return {host:{version:NATIVE_LAYOUT,identity,frame:simulation.frame,previous:simulation.previous.slice(),hostSlot:simulation.hostSlot,settings:simulation.settings,
    stack:e.__replay_stack.value,proof:JSON.stringify(simulation.checksum())},heap:new Uint8Array(e.memory.buffer).slice()};
}
export async function encodeNative(state,base){
  const meta=new TextEncoder().encode(JSON.stringify({...state.host,delta:!!base}));if(meta.length>MAX_JSON)throw Error('原生检查点元数据超过容量');
  const prefix=new Uint8Array(12);prefix.set(new TextEncoder().encode('T01STAT1'));new DataView(prefix.buffer).setUint32(8,meta.length,true);
  const heap=state.heap.slice();if(base){if(base.heap.length!==heap.length||base.host.identity!==state.host.identity)throw Error('原生基准不匹配');for(let i=0;i<heap.length;i++)heap[i]^=base.heap[i];}
  return new Uint8Array(await new Response(new Blob([prefix,meta,heap]).stream().pipeThrough(new CompressionStream('gzip'))).arrayBuffer());
}
export async function decodeNative(bytes,base){
  const reader=new Blob([bytes]).stream().pipeThrough(new DecompressionStream('gzip')).getReader(),parts=[];let size=0;
  for(;;){const r=await reader.read();if(r.done)break;if((size+=r.value.length)>HEAP+MAX_JSON+12){await reader.cancel();throw Error('原生检查点解压超过容量');}parts.push(r.value);}
  const raw=new Uint8Array(await new Blob(parts).arrayBuffer());
  if(raw.length<12||new TextDecoder().decode(raw.subarray(0,8))!=='T01STAT1')throw Error('原生检查点版本无效');
  const length=new DataView(raw.buffer).getUint32(8,true);if(length>MAX_JSON||raw.length!==12+length+HEAP)throw Error('原生检查点长度无效');
  const host=JSON.parse(new TextDecoder().decode(raw.subarray(12,12+length))),heap=raw.slice(12+length);
  if(host.version!==NATIVE_LAYOUT||typeof host.delta!=='boolean')throw Error('原生检查点状态版本无效');
  if(host.delta){if(!base||base.host.delta||base.host.frame!==0||base.host.identity!==host.identity)throw Error('原生差分缺少匹配的帧零基准');for(let i=0;i<heap.length;i++)heap[i]^=base.heap[i];}
  return {host,heap};
}
export function restoreNative(simulation,state,identity){
  const h=state.host,e=simulation.core;
  if(h.identity!==identity||h.version!==NATIVE_LAYOUT||h.hostSlot!==simulation.hostSlot||stable(h.settings)!==stable(simulation.settings)||
    !Number.isInteger(h.frame)||h.frame<0||h.frame>216000||!Array.isArray(h.previous)||
    (h.frame===0?h.previous.length!==0:h.previous.length!==simulation.settings.players)||h.previous.some(n=>!Number.isSafeInteger(n)||n<0||n>=2**44)||
    !Number.isInteger(h.stack)||h.stack<0||h.stack>HEAP||typeof h.proof!=='string'||state.heap.length!==HEAP)throw Error('原生检查点身份或宿主状态无效');
  new Uint8Array(e.memory.buffer).set(state.heap);e.__replay_stack.value=h.stack;simulation.frame=h.frame;simulation.previous=h.previous.slice();
  if(JSON.stringify(simulation.checksum())!==h.proof)throw Error('原生检查点恢复证明不一致');return {frame:h.frame,proof:h.proof};
}
