import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {runInNewContext} from 'node:vm';
import {CONTENT_SCHEMA,parseContentBundle} from '../../portable/content-bundle.mjs';
function fixture(change=()=>{}){
  const values=[Buffer.from('glyphs'),Buffer.alloc(16)];
  const files=values.map((bytes,index)=>({path:index?'/music/catalog.bin':'/fonts/original-glyphs.bin',offset:index?values[0].length:0,bytes:bytes.length,sha256:createHash('sha256').update(bytes).digest('hex')}));
  change(files);const json=Buffer.from(JSON.stringify({schema:CONTENT_SCHEMA,files})),head=Buffer.alloc(12);head.write('E75DATA1');head.writeUInt32LE(json.length,8);
  const result=Buffer.concat([head,json,...values]);return result.buffer.slice(result.byteOffset,result.byteOffset+result.byteLength);
}
test('DATA from another Window realm is valid and lazily verifies resource hashes',async()=>{
  const bytes=new Uint8Array(fixture()),foreign=runInNewContext('new ArrayBuffer('+bytes.length+')');
  new Uint8Array(foreign).set(bytes);assert.equal(foreign instanceof ArrayBuffer,false);
  const data=parseContentBundle(foreign);assert.equal(new TextDecoder().decode(await data.read('/fonts/original-glyphs.bin')),'glyphs');
  await assert.rejects(data.read('/fonts/missing.bin'),/not declared/);
});
test('reject unsafe, duplicate, overlapping and truncated resource indices',()=>{
  assert.throws(()=>parseContentBundle(fixture(files=>files[0].path='/resources/base/../secret.z')),/entry/);
  assert.throws(()=>parseContentBundle(fixture(files=>files[1].path=files[0].path)),/entry/);
  assert.throws(()=>parseContentBundle(fixture(files=>files[1].offset=0)),/entry/);
  assert.throws(()=>parseContentBundle(fixture().slice(0,-1)),/entry/);
});
test('corrupt resource bytes fail at the read boundary',async()=>{
  const buffer=fixture();new Uint8Array(buffer)[buffer.byteLength-1]=1;
  await assert.rejects(parseContentBundle(buffer).read('/music/catalog.bin'),/Corrupt/);
});
