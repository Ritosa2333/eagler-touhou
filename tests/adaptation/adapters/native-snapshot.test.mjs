import test from 'node:test';
import assert from 'node:assert/strict';
import {readNativeSnapshotRaw} from './native-snapshot.mjs';

function fixture(t){
  const runtime={global:'__nativeSnapshotFixture',core:'core',module:'Module',app:'app'},memory=new WebAssembly.Memory({initial:1}),calls=[];
  const owner={app:0,Module:{},core:{memory,snapshot(app){calls.push(app);return 128;}}};
  globalThis.__nativeSnapshotFixture=owner;t.after(()=>delete globalThis.__nativeSnapshotFixture);
  const request={runtime,snapshot:{export:'snapshot',wordCount:3,wordType:'f64',app:true}};
  const read=Function(`return (${readNativeSnapshotRaw.toString()})`)();return{owner,request,read,memory,calls};
}
test('shared snapshot reader preserves typed source words and owns the returned copy',t=>{
  const f=fixture(t);for(const [wordType,Type,values]of [['f64',Float64Array,[1,.125,-2]],['u32',Uint32Array,[1,0xffffffff,42]],['i32',Int32Array,[1,-2,42]]]){
    f.request.snapshot.wordType=wordType;new Type(f.memory.buffer,128,3).set(values);const raw=f.read(f.request);assert.deepEqual(raw,{available:true,words:values});
    new Type(f.memory.buffer,128,3).fill(0);assert.deepEqual(raw.words,values);
  }assert.deepEqual(f.calls,[0,0,0]);
});
test('memory is resolved after the native export grows its owned memory',t=>{
  const f=fixture(t);f.owner.core.snapshot=()=>{f.memory.grow(1);new Float64Array(f.memory.buffer,70000,3).set([1,2,3]);return 70000;};
  assert.deepEqual(f.read(f.request),{available:true,words:[1,2,3]});
});
for(const [name,mutate]of Object.entries({
  'oversized copy':f=>f.request.snapshot.wordCount=4097,
  'nonintegral count':f=>f.request.snapshot.wordCount=.5,
  'undeclared word type':f=>f.request.snapshot.wordType='bytes',
  'nested export path':f=>f.request.snapshot.export='core.snapshot',
  'missing app handle':f=>delete f.owner.app,
  'negative app handle':f=>f.owner.app=-1,
  'null pointer':f=>f.owner.core.snapshot=()=>0,
  'misaligned pointer':f=>f.owner.core.snapshot=()=>129,
  'out of range pointer':f=>f.owner.core.snapshot=()=>65528,
  'different module memory':f=>f.owner.Module.HEAPU8=new Uint8Array(new ArrayBuffer(65536)),
}))test(`shared snapshot reader rejects ${name}`,t=>{const f=fixture(t);mutate(f);assert.equal(f.read(f.request).available,false);});
