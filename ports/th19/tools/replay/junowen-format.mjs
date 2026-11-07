// Ju.N.Owen 534f264 replay writer layout; this codec does not assert engine parity.
export const JUNOWEN_HEADER_BYTES = 17;
const maxFrames = 216000;
const uint = (v,max) => Number.isInteger(v)&&v>=0&&v<=max;
function validate(h) {
 for(const k of ['randSeed1','randSeed2']) if(!uint(h[k],0xffffffff))throw Error(`Invalid ${k}`);
 if(!uint(h.difficulty,3)||!uint(h.matchup,3))throw Error('Invalid mode');
 for(const k of ['common','p1Settings','p2Settings','p1Card','p2Card'])if(!uint(h[k],255))throw Error(`Invalid ${k}`);
 for(const k of ['p1Character','p2Character'])if(!uint(h[k],18))throw Error(`Invalid ${k}`);
}
export function decodeJunowen(bytes) {
 if(!(bytes instanceof Uint8Array)||bytes.length<17)throw Error('Truncated 17-byte header');
 const v=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);
 const h={randSeed1:v.getUint32(0,true),randSeed2:v.getUint32(4,true)};
 ['difficulty','matchup','common','p1Settings','p2Settings','p1Character','p1Card','p2Character','p2Card'].forEach((k,i)=>h[k]=bytes[8+i]);validate(h);
 const stride=h.matchup===1?2:4,n=(bytes.length-17)/stride;
 if(!Number.isInteger(n)||n>maxFrames)throw Error('Invalid input payload length');
 return {header:h,inputs:Array.from({length:n},(_,i)=>Array.from({length:stride/2},(_,j)=>v.getUint16(17+i*stride+j*2,true)))};
}
export function encodeJunowen({header:h,inputs}) {
 validate(h);const seats=h.matchup===1?1:2;
 if(!Array.isArray(inputs)||inputs.length>maxFrames)throw Error('Invalid input count');
 const b=new Uint8Array(17+inputs.length*seats*2),v=new DataView(b.buffer);
 v.setUint32(0,h.randSeed1,true);v.setUint32(4,h.randSeed2,true);
 ['difficulty','matchup','common','p1Settings','p2Settings','p1Character','p1Card','p2Character','p2Card'].forEach((k,i)=>b[8+i]=h[k]);
 inputs.forEach((row,i)=>{if(!Array.isArray(row)||row.length!==seats||row.some(x=>!uint(x,65535)))throw Error('Invalid input row');row.forEach((x,j)=>v.setUint16(17+(i*seats+j)*2,x,true));});return b;
}
// Native keyboard bit8 is pause; the web transport uses bit10 instead.
export function webKeysToNative(keys) {
 if(!uint(keys,2047)||(keys&768))throw Error('Unsupported web input bits');
 return (keys&255)|((keys&1024)?256:0);
}
export function nativeKeysToWeb(keys) {
 if(!uint(keys,65535)||(keys&~511))throw Error('Unsupported native input bits');
 return (keys&255)|((keys&256)?1024:0);
}
// EXE0x402540: state at object+4, shadow at+0x14, result modulo object+0x10.
export function nativeRandomStep(state,range=0xffff00) {
 if(!uint(state,0xffffffff)||!uint(range,0xffffffff)||range===0)throw Error('Invalid RNG state/range');
 const next=Number(BigInt(state)*48271n%2147483647n);
 return {state:next,shadow:next,value:next%range};
}
