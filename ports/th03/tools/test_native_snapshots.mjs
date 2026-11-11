import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import vm from 'node:vm';
import {createNativeSnapshots} from '../web/netplay/native-snapshots.js';

const nativeMemory=new WebAssembly.Memory({initial:328,maximum:328});
const heap=new Uint8Array(nativeMemory.buffer),disk=new Uint8Array(8192);
const node={mode:0,contents:disk},stream={node,position:0};
const fs={root:node,streams:[stream],nameTable:[],isDir:()=>false};
const memfs={stream_ops:{write(stream,buffer,offset,length,position){
  stream.node.contents.set(buffer.subarray(offset,offset+length),position);return length;
}}};
let tick=0,bridge=0,openFrame=null,replaying=false;
const host={capture:()=>({tick}),restore:saved=>{tick=saved.tick;}};
const globals={__rollback_g0:new WebAssembly.Global({value:'i32',mutable:true},0),
  __rollback_g1:new WebAssembly.Global({value:'i32',mutable:true},0),Ld:nativeMemory};
const module={HEAPU8:heap};
const snapshots=createNativeSnapshots({module,fs,memfs,tty:{ttys:{}},host,exports:()=>globals,
  readBridge:()=>({bridge}),writeBridge:saved=>{bridge=saved.bridge;}});
Object.assign(module,{
  netCapture:frame=>snapshots.capture(frame),netRestore:frame=>snapshots.restore(frame),
  netConfirm:prefix=>snapshots.confirm(prefix),netInfo:()=>({tick}),
  netBeginFrame(frame,replay){assert.equal(openFrame,null);openFrame=frame;replaying=replay;},
  netEndFrame(){snapshots.seal();openFrame=null;replaying=false;},
});
const context=vm.createContext({emulator:{module,run(){},pause(){},step(){tick++;}},
  nativePause:{update:()=>({phase:1}),result(){},capture:()=>({}),restore(){}},
  nativeMusic:{read:()=>[],capture:()=>({}),restore(){}},
  confirmedMusic:{stage(){},rewind(){},confirm(){}},
  presentationDue:()=>false,presentFrame(){},performance,
  controls:{applyFrame(){heap[0]++;bridge++;globals.__rollback_g0.value++;
    memfs.stream_ops.write(stream,new Uint8Array([heap[0]]),0,1,0);
  },capture:()=>({}),restore(){}},
  syncSnapshots:new Map(),hiddenSeats:new Set(),pauseSeat:null,pauseSelection:0,
  visibilityPaused:false,syncReplaying:false,syncFrame:0,nativeState:null,
  $:()=>({hidden:false}),window:{},refreshPauseMenu(){},localizePause(){}});
const source=readFileSync(new URL('../web/app.js',import.meta.url),'utf8');
vm.runInContext(source.slice(source.indexOf('function stepSynchronized('),source.indexOf('function syncHash(')),context);
const inputs=[{actions:[],commands:[],touch:0},{actions:[],commands:[],touch:0}];

// Exercise the actual application callbacks, including paused and replay steps.
context.captureSynchronized(0);
assert.throws(()=>snapshots.capture(1),/上一回滚帧尚未结束/,'Missing end-frame reproduces the original failure');
context.stepSynchronized(inputs,0);
context.pauseSeat=0;
context.captureSynchronized(1);context.stepSynchronized(inputs,1,true);
assert.equal(openFrame,null);assert.equal(replaying,false);assert.equal(tick,1);
context.restoreSynchronized(0);
assert.equal(heap[0],0);assert.equal(disk[0],0);assert.equal(bridge,0);
assert.equal(globals.__rollback_g0.value,0);assert.equal(tick,0);
assert.equal(context.syncSnapshots.size,0,'Restored checkpoint must be captured again');
context.captureSynchronized(0);context.stepSynchronized(inputs,0);
context.confirmSynchronized(1);

// Fill the entire prediction window, restore an odd frame and replay it.
for(let frame=1;frame<=12;frame++){
  context.captureSynchronized(frame);context.stepSynchronized(inputs,frame);
}
assert.equal(snapshots.info().snapshots,12);
context.restoreSynchronized(3);
assert.equal(heap[0],3);assert.equal(disk[0],3);assert.equal(tick,3);
for(let frame=3;frame<=12;frame++){
  context.captureSynchronized(frame);context.stepSynchronized(inputs,frame,true);
}
assert.equal(heap[0],13);assert.equal(disk[0],13);
context.confirmSynchronized(13);
for(let frame=13;frame<100;frame++){
  context.captureSynchronized(frame);context.stepSynchronized(inputs,frame);
  context.confirmSynchronized(frame+1);
}
assert.equal(snapshots.info().snapshots,0);assert.equal(context.syncSnapshots.size,0);

context.captureSynchronized(100);
context.controls.applyFrame=()=>{throw Error('test failure');};
assert.throws(()=>context.stepSynchronized(inputs,100,true),/test failure/);
assert.equal(openFrame,null);assert.equal(replaying,false);assert.equal(context.syncReplaying,false);
context.captureSynchronized(101);
module.netEndFrame();context.confirmSynchronized(102);
assert.equal(snapshots.info().snapshotFormat,'zero-pages');
assert(snapshots.info().snapshotBytes<13*heap.length/8,'Sparse fixtures should use a bounded, smaller pool');

// Compare both formats against independent, full byte-for-byte checkpoints.
// Include single bits at both page edges, 128-byte SIMD boundaries, old data
// becoming zero, the last byte of memory, and buffer growth/reuse after rewind.
for(const sparse of [false,true]){
  const native=new WebAssembly.Memory({initial:328,maximum:328}),h=new Uint8Array(native.buffer);
  const g={__rollback_g0:new WebAssembly.Global({value:'i32',mutable:true},0),
    __rollback_g1:new WebAssembly.Global({value:'i32',mutable:true},0)};
  if(sparse)g.Ld=native;
  const n={mode:0,contents:new Uint8Array(0)};
  const manager=createNativeSnapshots({module:{HEAPU8:h},exports:()=>g,
    fs:{root:n,streams:[],nameTable:[],isDir:()=>false},memfs:{stream_ops:{write(){}}},
    tty:{ttys:{}},host:{capture:()=>({}),restore(){}},readBridge:()=>({}),writeBridge(){}});
  const reference=[];
  let random=0x194703;
  for(let frame=0;frame<13;frame++){
    for(let j=0;j<80;j++){
      random=(Math.imul(random,1664525)+1013904223)>>>0;
      const page=random%5248;
      for(const offset of [0,15,16,127,128,4095])h[page*4096+offset]=(random>>>(offset%24))&255;
    }
    h.fill(frame%2?0:255,8*4096,12*4096);h[h.length-1]=frame+1;
    reference.push(h.slice());manager.capture(frame);manager.seal();
  }
  for(const frame of [11,7,3,0]){
    h.fill(0x81);manager.restore(frame);
    assert.deepEqual(h,reference[frame],`Every heap byte must restore (${sparse?'sparse':'full'}, frame ${frame})`);
  }
  manager.capture(0);manager.seal();h.fill(0xff);
  manager.capture(1);manager.seal();h.fill(0);
  manager.restore(1);assert(h.every(byte=>byte===0xff),'Dense frame must grow a reused sparse buffer');
  manager.restore(0);assert.deepEqual(h,reference[0]);
  assert.equal(manager.info().snapshotFormat,sparse?'zero-pages':'full');
}
console.log('PASS: native snapshot lifecycle, paused frames, odd-frame rollback, disk undo, checkpoint recapture, window reuse and error cleanup');
console.log('PASS: sparse/full snapshots restore every byte across all page boundaries, zero transitions, dense growth and repeated rewinds');
