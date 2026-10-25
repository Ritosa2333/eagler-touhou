import test from 'node:test';
import assert from 'node:assert/strict';
import {readPracticeProgramRaw} from './practice.mjs';

function read(change=()=>{}){
  const memory=new ArrayBuffer(256),header=new Uint32Array(memory,16,4);header.set([1,1,64,72]);
  new Uint8Array(memory,64,72).fill(7);change(header);
  const request={runtime:{global:'__practiceProgramUnit',core:'core',module:'module',app:'app'},contract:{programSnapshot:{export:'snapshot'}}};
  let calls=0;globalThis.__practiceProgramUnit={app:0,core:{memory:{buffer:memory},snapshot:app=>{assert.equal(app,0);calls++;return 16;}}};
  try{const result=readPracticeProgramRaw(request);assert.equal(calls,1);new Uint8Array(memory).fill(0);return result;}
  finally{delete globalThis.__practiceProgramUnit;}
}
test('program observer makes an owned copy of bounded frozen native bytes using the declared ABI',()=>{
  const raw=read();assert.deepEqual(raw.header,[1,1,64,72]);assert.equal(raw.bytes.length,72);assert.ok(raw.bytes.every(byte=>byte===7));
});
for(const [name,mutate] of Object.entries({'unknown ABI':h=>h[0]=2,'no immutable program':h=>h[1]=0,
  'null program pointer':h=>h[2]=0,'program outside memory':h=>h[2]=240,'short program header':h=>h[3]=71,'unbounded copy':h=>h[3]=2097153}))
  test(`program observer rejects ${name}`,()=>assert.throws(()=>read(mutate)));
