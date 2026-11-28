import assert from 'node:assert/strict';
import crypto from 'node:crypto';
import fs from 'node:fs';
import path from 'node:path';
import test from 'node:test';
import {fileURLToPath} from 'node:url';
import {decodeType6Profile, loadType6ProfileOnce, TYPE6_BYTES} from '../artifacts/dsh-type6-loading/type6-loader.mjs';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const profile=JSON.parse(fs.readFileSync(path.join(root,'artifacts/dsh-type6-profile/type6-profile.json'),'utf8'));
const expected={
  sourceExeSha256:'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4',
  sourceAddress:'0x4b2b90',
  bytes:288,
  sha256:'e26b6619e74337ad7e27614e55b095221b8555a8d8152f520e4c74d53c3a93f0',
  script:41,
  secondaryScript:0,
  hitboxSize:4,
  auxiliarySize:16,
  appearance:3,
  selector:6,
  freezePolicy:0,
  opaque114:30,
  identitySpriteRemap:false,
  colorRemaps:[
    [96,323,336,337],[97,324,337,339],[98,324,338,341],[99,325,339,343],
    [100,325,340,345],[101,326,341,347],[102,326,342,349],[103,327,343,351],
    [104,327,344,353],[105,328,345,355],[106,328,346,357],[107,328,347,359],
    [108,329,348,361],[109,329,349,363],[110,329,350,365],[111,330,351,367],
  ],
  cancellationScripts:[337,339,341,343,345,347,349,351,353,355,357,359,361,363,365,367],
  callback:'per-color four-sprite remap table; no identity 0x40ccb0 callback',
};
const digest=async bytes=>crypto.createHash('sha256').update(bytes).digest('hex');

function nativeMock(result=1){
  const added=[],freed=[],allocations=[];
  let nextPointer=0x1000;
  const module={
    _th128_world_add_bullet_profile(type,pointer,length){added.push([type,pointer,length]);return result;},
    _free(pointer){freed.push(pointer);},
    _th128_world_error(){return 'native rejected actual type6 fixture';},
    UTF8ToString(value){return value;},
  };
  const memory=bytes=>{const pointer=nextPointer;nextPointer+=0x100;allocations.push([pointer,bytes]);return pointer;};
  return {module,memory,added,freed,allocations};
}

test('actual original type6 JSON decodes/adds exactly once with its provenance',async()=>{
  assert.equal(profile.sourceAddress,'0x4b2b90');
  assert.equal(profile.hex.length,576);
  const native=nativeMock(),status={loaded:false};
  assert.equal(await loadType6ProfileOnce({profile,expected,module:native.module,memory:native.memory,digest,status}),true);
  assert.deepEqual(native.added,[[6,0x1000,TYPE6_BYTES]]);
  assert.deepEqual(native.freed,[0x1000]);
  assert.equal(status.loaded,true);
  assert.equal(await loadType6ProfileOnce({profile,expected,module:native.module,memory:native.memory,digest,status}),false);
  assert.equal(native.added.length,1);
});

test('actual schema rejects corrupt checksum, fields, remaps and invented properties',async()=>{
  const variants=[
    {type:7},
    {schema:'other'},
    {sourceExeSha256:'wrong'},
    {sourceAddress:'0x004b2b90'},
    {bytes:287},
    {sha256:'0'.repeat(64)},
    {script:42},
    {secondaryScript:1},
    {hitboxSize:4.001},
    {auxiliarySize:15},
    {appearance:2},
    {selector:8},
    {freezePolicy:1},
    {opaque114:31},
    {identitySpriteRemap:true},
    {supportedTypes:[6,7]},
    {colorRemaps:profile.colorRemaps.map((row,index)=>index===0?[96,323,336,338]:row)},
    {cancellationScripts:[...profile.cancellationScripts.slice(0,15),368]},
    {callback:'wrong callback'},
    {perColor:[]},
    {callbackExtra:()=>{}},
  ];
  for(const patch of variants){
    const native=nativeMock(),status={loaded:false};
    await assert.rejects(
      loadType6ProfileOnce({profile:{...profile,...patch},expected,module:native.module,memory:native.memory,digest,status}),
      /type6|Type6|Unknown|Invalid/i,
    );
    assert.deepEqual(native.added,[]);
    assert.deepEqual(native.allocations,[]);
    assert.equal(status.loaded,false);
  }

  const native=nativeMock(),status={loaded:false};
  const otherHex=Buffer.alloc(TYPE6_BYTES,7).toString('hex');
  await assert.rejects(
    loadType6ProfileOnce({profile:{...profile,hex:otherHex},expected,module:native.module,memory:native.memory,digest,status}),
    /checksum/i,
  );
  assert.deepEqual(native.added,[]);
  assert.equal(status.loaded,false);
});

test('native rejection frees the temporary allocation and keeps loaded false',async()=>{
  const native=nativeMock(0),status={loaded:false};
  await assert.rejects(
    loadType6ProfileOnce({profile,expected,module:native.module,memory:native.memory,digest,status}),
    /native rejected actual type6 fixture/,
  );
  assert.deepEqual(native.added,[[6,0x1000,TYPE6_BYTES]]);
  assert.deepEqual(native.freed,[0x1000]);
  assert.equal(status.loaded,false);
});

test('actual decoder returns exactly 288 bytes with the published SHA',async()=>{
  const bytes=decodeType6Profile(profile,expected);
  assert.equal(bytes.length,TYPE6_BYTES);
  assert.equal(await digest(bytes),expected.sha256);
});

test('production controller and both active variants use the exact Type6 evidence',()=>{
  const source=fs.readFileSync(path.join(root,'site/play.mjs'),'utf8');
  for(const needle of [
    'function supportsBulletType6()',
    'capabilities.supportedBulletTypes.includes(6)',
    'animations/bullet-type6-profile.json',
    "const type6SourceAddress='0x4b2b90'",
    'e26b6619e74337ad7e27614e55b095221b8555a8d8152f520e4c74d53c3a93f0',
    '_th128_world_add_bullet_profile(6,pointer,type6Bytes.length)',
    'profile.colorRemaps.some',
    'profile.cancellationScripts.some',
  ]){
    assert.ok(source.includes(needle),`missing production Type6 contract: ${needle}`);
  }
  const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json'),'utf8'));
  for(const variant of ['ja','zh']){
    const manifest=catalog.variants?.[variant]?.manifest;
    assert.equal(typeof manifest,'string');
    const assetPath=path.join(root,'local',path.dirname(manifest),'animations','bullet-type6-profile.json');
    const asset=fs.readFileSync(assetPath);
    assert.equal(Buffer.compare(asset,fs.readFileSync(path.join(root,'artifacts/dsh-type6-profile/type6-profile.json'))),0);
    const parsed=JSON.parse(asset);
    const bytes=Uint8Array.from(parsed.hex.match(/../g),value=>parseInt(value,16));
    const sha=crypto.createHash('sha256').update(bytes).digest('hex');
    assert.equal(sha,expected.sha256);
    assert.equal(parsed.sourceAddress,expected.sourceAddress);
  }
});
