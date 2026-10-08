import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import createCore from '../site/runtime/core.mjs';

const fixture=JSON.parse(fs.readFileSync(new URL('fixtures/bullet-radius-birth-native.json',import.meta.url)));
const word=value=>{const bytes=Buffer.alloc(4);bytes.writeInt32LE(value);return bytes;};
const float=value=>{const bytes=Buffer.alloc(4);bytes.writeFloatLE(value);return bytes;};
function instruction(op,args=[]){
  const payload=Buffer.concat(args),bytes=Buffer.alloc(16+payload.length);
  bytes.writeUInt16LE(op,4);bytes.writeUInt16LE(bytes.length,6);bytes[10]=63;bytes[11]=args.length;
  payload.copy(bytes,16);return bytes;
}
function program(row){
  const code=Buffer.concat([Buffer.from('ECLH'),Buffer.alloc(12),
    instruction(500,[word(0)]),instruction(525,[word(0),...row.origin.map(float)]),
    instruction(524,[word(0),float(row.radius)]),instruction(504,[word(0),float(row.rawAngle),float(0)]),
    instruction(505,[word(0),float(0),float(0)]),instruction(506,[word(0),word(1),word(1)]),
    instruction(507,[word(0),word(1)]),instruction(501,[word(0)]),instruction(83,[word(100000)])]);
  const bytes=Buffer.alloc(48+code.length);bytes.write('SCPT');bytes.writeUInt16LE(1,4);
  bytes.writeUInt16LE(1,16);bytes.writeUInt32LE(48,36);bytes.write('main\0',40);code.copy(bytes,48);
  return bytes;
}
const snapshot=core=>{
  const pointer=core._th12_state(),size=core._th12_state_size();
  return JSON.parse(new TextDecoder().decode(core.HEAPU8.subarray(pointer,pointer+size)));
};

test('production emitter radius uses native normalized heading and stored polar offsets',async()=>{
  assert.equal(fixture.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
  assert.equal(fixture.wholeGameOracle,false);assert.equal(fixture.rows.length,63);
  const core=await createCore({wasmBinary:fs.readFileSync(new URL('../site/runtime/core.wasm',import.meta.url))});
  for(const row of fixture.rows){
    const bytes=program(row),pointer=core._malloc(bytes.length);
    try{core.HEAPU8.set(bytes,pointer);assert.equal(core._th12_load_ecl(pointer,bytes.length),1);}finally{core._free(pointer);}
    core._th12_start(0,0,1,1234);core._th12_oracle_fixture(7);
    const bullet=snapshot(core).bullets.find(b=>!b.friendly);
    assert.ok(bullet,`radius${row.radius} angle${row.rawAngle}`);
    assert.equal(Math.fround(bullet.motion[0]),row.storedAngle);
    for(let axis=0;axis<2;axis++)assert.equal(Math.fround(bullet.position[axis]),row.position[axis],
      `origin${row.origin} radius${row.radius} angle${row.rawAngle} axis${axis}`);
  }
});
