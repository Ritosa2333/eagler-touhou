import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import createCore from '../site/runtime/core.mjs';
const native=JSON.parse(fs.readFileSync(new URL('./fixtures/camera-pulse-original.json',import.meta.url)));
const state=core=>{const at=core._th12_state(),length=core._th12_state_size();return JSON.parse(new TextDecoder().decode(core.HEAPU8.subarray(at,at+length)));};
const word=n=>{const b=Buffer.alloc(4);b.writeInt32LE(n);return b;};
function instruction(op,args){const body=Buffer.concat(args),header=Buffer.alloc(16);header.writeUInt16LE(op,4);header.writeUInt16LE(16+body.length,6);header[10]=63;header[11]=args.length;return Buffer.concat([header,body]);}
test('original ECL417(30,12,0) schedules profile1 from startup before Player16',async()=>{
  assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
  const core=await createCore(),raw=Buffer.alloc(64);
  raw.write('SCPT');raw.writeUInt16LE(1,4);raw.writeUInt16LE(1,16);raw.writeUInt32LE(48,36);raw.write('main',40);raw.write('ECLH',48);
  const program=Buffer.concat([raw,instruction(417,[word(30),word(12),word(0)]),instruction(83,[word(100000)])]),at=core._malloc(program.length);
  try{core.HEAPU8.set(program,at);assert.equal(core._th12_load_ecl(at,program.length),1);}finally{core._free(at);}
  core._th12_start(0,0,1,native.seed);
  const row=native.records.find(r=>r.name==='retail-ecl417-fade');
  for(const frame of row.frames){
    core._th12_tick(0,0);const actual=state(core);
    assert.deepEqual([actual.rng.seed,actual.rng.calls],frame.rng,'input'+frame.tick+' RNG');
    assert.deepEqual(actual.background.camera.screenShake.map(Math.fround),frame.offsets,'input'+frame.tick+' shake');
  }
  assert.equal(new Uint32Array(core.HEAPU8.buffer,core._th12_unsupported(),1024)[417],0);
});
