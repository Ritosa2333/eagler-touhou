// The existing FNV-1a checksum, over the same bytes in the same order.
// Keep scanning inside WASM to avoid periodic JS work on the input thread.
// This helper imports native memory read-only and has no mutable state.
const HASH_WASM=new Uint8Array([0,97,115,109,1,0,0,0,1,7,1,96,2,127,127,1,127,2,15,1,3,101,110,118,6,109,101,109,111,114,121,2,0,0,3,2,1,0,7,8,1,4,104,97,115,104,0,0,10,208,1,1,205,1,1,1,127,65,197,187,242,136,120,33,2,2,64,3,64,32,1,32,0,107,65,8,73,13,1,32,2,32,0,45,0,0,115,65,147,131,128,8,108,33,2,32,2,32,0,45,0,1,115,65,147,131,128,8,108,33,2,32,2,32,0,45,0,2,115,65,147,131,128,8,108,33,2,32,2,32,0,45,0,3,115,65,147,131,128,8,108,33,2,32,2,32,0,45,0,4,115,65,147,131,128,8,108,33,2,32,2,32,0,45,0,5,115,65,147,131,128,8,108,33,2,32,2,32,0,45,0,6,115,65,147,131,128,8,108,33,2,32,2,32,0,45,0,7,115,65,147,131,128,8,108,33,2,32,0,65,8,106,33,0,12,0,11,11,2,64,3,64,32,0,32,1,79,13,1,32,2,32,0,45,0,0,115,65,147,131,128,8,108,33,2,32,0,65,1,106,33,0,12,0,11,11,32,2,11]);
const helpers=new WeakMap();let compiled;
export function hashNativeMemory(memory,start,end){
  if(!(memory instanceof WebAssembly.Memory)||!Number.isSafeInteger(start)||!Number.isSafeInteger(end)
    ||start<0||end<start||end>memory.buffer.byteLength)throw Error('无效原生校验范围');
  let hash=helpers.get(memory);
  if(hash===undefined){
    try{
      compiled??=WebAssembly.validate(HASH_WASM)?new WebAssembly.Module(HASH_WASM):false;
      hash=compiled?new WebAssembly.Instance(compiled,{env:{memory}}).exports.hash:null;
    }catch{hash=null;}
    helpers.set(memory,hash);
  }
  if(hash)return hash(start,end)>>>0;
  const heap=new Uint8Array(memory.buffer);let result=2166136261;
  for(let at=start;at<end;at++)result=Math.imul(result^heap[at],16777619);
  return result>>>0;
}
