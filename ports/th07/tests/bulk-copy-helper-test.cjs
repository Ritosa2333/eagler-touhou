const assert = require('node:assert/strict');
const fs = require('node:fs');
const {performance} = require('node:perf_hooks');
const file = process.argv[2];
if (!file) throw new Error('usage: node bulk-copy-helper-test.cjs <compiled.wasm> [--benchmark]');
const bytes = fs.readFileSync(file);
const module_ = new WebAssembly.Module(bytes);
const memory = new WebAssembly.Memory({initial:512, maximum:1024});
const {copy,batch} = new WebAssembly.Instance(module_, {env:{memory}}).exports;
const SRC = 1024, DST = 8*1024*1024, PLAN = 24*1024*1024;
let u8 = new Uint8Array(memory.buffer), u32 = new Uint32Array(memory.buffer);
for (let i=0;i<4104192;i++) u8[SRC+i]=(i*37 + (i>>>7))&255;
const expected = u8.slice(SRC,SRC+4104192);
// Gather the actual Bullet stride and normal VM/tail pattern, then scatter.
let n=0,total=0;
for(let i=0;i<1024;i++)for(const [offset,length] of [[0,700],[3500,508]]) {
  const index=PLAN/4+n*3;
  u32[index]=i*4008+offset;u32[index+1]=total;u32[index+2]=length;
  total+=length;n++;
}
batch(DST,SRC,PLAN,n);
let pos=0;
for(let i=0;i<1024;i++)for(const [offset,length] of [[0,700],[3500,508]]){
  assert.deepEqual(u8.slice(DST+pos,DST+pos+length),expected.slice(i*4008+offset,i*4008+offset+length));pos+=length;
}
// Overlap semantics and traps are provided by memory.copy, not manual loops.
for(const [dst,src,len] of [[SRC+1,SRC,3000],[SRC,SRC+9,4000],[DST,DST,500],[SRC,SRC,0]]){
  const oracle=u8.slice();oracle.copyWithin(dst,src,src+len);copy(dst,src,len);assert.deepEqual(u8,oracle);
}
assert.throws(()=>copy(u8.length-2,SRC,8),WebAssembly.RuntimeError);
u32[PLAN/4]=0xfffffff0;u32[PLAN/4+1]=0;u32[PLAN/4+2]=4;
assert.throws(()=>batch(DST,1024,PLAN,1),WebAssembly.RuntimeError,'base+offset overflow must trap');
// One imported Memory object follows growth; do not retain detached views.
memory.grow(1);u8=new Uint8Array(memory.buffer);u32=new Uint32Array(memory.buffer);
const fresh=u8.length-128;u8[SRC]=219;copy(fresh,SRC,1);assert.equal(u8[fresh],219);
console.log('PASS memory.copy helper: exact gather, overlap, bounds, overflow and imported-memory growth');
if(process.argv.includes('--benchmark')) {
  // Representative merged live-journal ranges: source stride4008, destination
  // contiguous. Warm up both before recording; this is not a phone benchmark.
  for(let i=0;i<1024;i++){
    const index=PLAN/4+i*3;u32[index]=i*4008;u32[index+1]=i*1208;u32[index+2]=1208;
  }
  const native=()=>batch(DST,SRC,PLAN,1024);
  const typed=()=>{for(let i=0,j=PLAN/4;i<1024;i++,j+=3){const from=SRC+u32[j];u8.copyWithin(DST+u32[j+1],from,from+u32[j+2]);}};
  for(let i=0;i<100;i++){native();typed();}
  const results=[];
  for(let round=0;round<6;round++)for(const [name,fn] of (round%2?[['typed',typed],['memory.copy',native]]:[['memory.copy',native],['typed',typed]])){
    const started=performance.now();for(let i=0;i<200;i++)fn();results.push({round,name,ms:performance.now()-started});
  }
  console.log(JSON.stringify({scope:'microbenchmark, not end-to-end speed',results}));
}
