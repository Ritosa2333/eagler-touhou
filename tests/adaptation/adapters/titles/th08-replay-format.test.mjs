import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {decodeTh08ReplayFile,inspectTh08ReplayInputFrames,inspectTh08ReplayMetadata,inspectTh08OriginalReplay,parseTh08MotionTrailer,TH08_REPLAY_SEMANTICS} from './th08-replay-format.mjs';

import {replayFile} from '../../fixtures/th08-replay-file.mjs';

function fnv1a(bytes){let hash=2166136261;for(const byte of bytes)hash=Math.imul(hash^byte,16777619)>>>0;return hash;}

function withThMotion(replay,{version=2,game=8}={}){
  const counts=Buffer.alloc(36),payloadParts=[counts],movements=[{tick:0,x:1.5,y:-2.25}],touch=[
    {tick:1,id:7101,x:.42,y:.64,action:0},{tick:2,id:7101,x:.48,y:.61,action:1},{tick:3,id:7101,x:.52,y:.58,action:2},
  ];
  counts.writeUInt32LE(movements.length,4*4);
  const movementBytes=Buffer.alloc(movements.length*12);movements.forEach((entry,index)=>{
    const offset=index*12;movementBytes.writeUInt32LE(entry.tick,offset);movementBytes.writeFloatLE(entry.x,offset+4);movementBytes.writeFloatLE(entry.y,offset+8);
  });payloadParts.push(movementBytes);
  if(version===2){
    const touchCounts=Buffer.alloc(36);touchCounts.writeUInt32LE(touch.length,4*4);payloadParts.push(touchCounts);
    const touchBytes=Buffer.alloc(touch.length*20);touch.forEach((entry,index)=>{
      const offset=index*20;touchBytes.writeUInt32LE(entry.tick,offset);touchBytes.writeInt32LE(entry.id,offset+4);
      touchBytes.writeFloatLE(entry.x,offset+8);touchBytes.writeFloatLE(entry.y,offset+12);touchBytes.writeUInt32LE(entry.action,offset+16);
    });payloadParts.push(touchBytes);
  }
  const payload=Buffer.concat(payloadParts),footer=Buffer.alloc(24),payloadBytes=payload.length;
  footer.write('THMOTION',0,'ascii');footer.writeUInt32LE(game,8);footer.writeUInt32LE(version,12);
  footer.writeUInt32LE(payloadBytes,16);footer.writeUInt32LE(fnv1a(payload),20);
  return Buffer.concat([replay,payload,footer]);
}
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
test('original export inspection checks identity and exact original file extent',()=>{
  const bytes=replayFile(),proof=inspectTh08OriginalReplay(bytes,{expectedSha256:sha(bytes)});
  assert.equal(proof.originalBytes,bytes.length);assert.equal(proof.extensionBytes,0);
  assert.throws(()=>inspectTh08OriginalReplay(bytes,{expectedSha256:'0'.repeat(64)}),/SHA-256/);
  const extended=withThMotion(bytes);
  assert.throws(()=>inspectTh08OriginalReplay(extended,{expectedSha256:sha(extended)}),/appended extension/);
  const stray=Buffer.concat([bytes,Buffer.from([0])]);
  assert.throws(()=>inspectTh08OriginalReplay(stray,{expectedSha256:sha(stray)}),/USER header/);
  const user=Buffer.alloc(16);user.write('USER');user.writeUInt32LE(16,4);user.write('abc',12);
  const original=Buffer.concat([bytes,user]);
  assert.equal(inspectTh08OriginalReplay(original,{expectedSha256:sha(original)}).userBytes,16);
  for(const [offset,value] of [[4,18],[4,15],[8,1]]){
    const bad=Buffer.from(original);bad.writeUInt32LE(value,bytes.length+offset);
    assert.throws(()=>inspectTh08OriginalReplay(bad,{expectedSha256:sha(bad)}),/USER/);
  }
  const extendedUser=withThMotion(original);
  assert.throws(()=>inspectTh08OriginalReplay(extendedUser,{expectedSha256:sha(extendedUser)}),/appended extension/);
});

test('TH08 title parser reads literal v6 frame words at declared stage stride and identity',()=>{
  const bytes=replayFile(),expectedSha256=sha(bytes),decoded=decodeTh08ReplayFile(bytes);
  assert.equal(decoded.debugRecords,false);
  const inspected=inspectTh08ReplayInputFrames(bytes,{stage:0,expectedSha256,firstFrame:0,frameCount:5});
  assert.deepEqual(inspected.frames.map(frame=>frame.buttons),[0,1,128,0,4]);
  assert.deepEqual(inspected.frames.map(frame=>frame.cursor),[0,2,4,6,8]);
  assert.throws(()=>inspectTh08ReplayInputFrames(bytes,{stage:0,expectedSha256:'f'.repeat(64),firstFrame:0,frameCount:2}),/SHA-256/);
  const debug=replayFile({debugRecords:true,inputFrames:[0,1,128,0,4,16]});
  assert.deepEqual(inspectTh08ReplayInputFrames(debug,{stage:0,expectedSha256:sha(debug),firstFrame:0,frameCount:6})
    .frames.map(frame=>frame.buttons),[0,1,128,0,4,16]);
  assert.equal(TH08_REPLAY_SEMANTICS.playback.rawIndexOffset,-1);
  assert.equal(TH08_REPLAY_SEMANTICS.nativeTrace.schema,2);
  assert.equal(TH08_REPLAY_SEMANTICS.nativeTrace.wordCount,36);
  assert.equal(TH08_REPLAY_SEMANTICS.nativeTrace.sample.consumerSequence,27);
  assert.equal(TH08_REPLAY_SEMANTICS.nativeTrace.sample.pointXFloat32Bits,33);
  assert.deepEqual(TH08_REPLAY_SEMANTICS.pointerExtension.appliedPoint,
    {source:'MotionTrack::playback_points.begin()',selection:'single active pointer in signed-id map order',
      coordinates:'Float32 normalized [0,1]',tolerance:1e-6,
      traceWords:{count:24,id:32,xFloat32Bits:33,yFloat32Bits:34,released:35}});
});

test('TH08 Replay metadata parser reads saved processing drop and practice identity from original bytes',()=>{
  const bytes=replayFile({lag:100}),expectedSha256=sha(bytes);
  const inspected=inspectTh08ReplayMetadata(bytes,{expectedSha256});
  assert.equal(inspected.sha256,expectedSha256);assert.equal(inspected.metadata.lag,100);
  assert.equal(inspected.metadata.practice,false);assert.match(inspected.source,/ReplayMetadata::lag/);
  const practiceBytes=replayFile({lag:100,practice:true});
  assert.equal(inspectTh08ReplayMetadata(practiceBytes,{expectedSha256:sha(practiceBytes)}).metadata.practice,true);
  assert.throws(()=>inspectTh08ReplayMetadata(bytes,{expectedSha256:'f'.repeat(64)}),/saved-byte SHA-256/);
  const nonFinite=replayFile({lag:Number.NaN});
  assert.throws(()=>inspectTh08ReplayMetadata(nonFinite,{expectedSha256:sha(nonFinite)}),/not finite/);
});

test('THMOTION v2 parser independently validates pointer trajectory and rejects changed bytes',()=>{
  const bytes=withThMotion(replayFile()),expectedSha256=sha(bytes);
  const parsed=parseTh08MotionTrailer(bytes,{expectedSha256});
  assert.equal(parsed.version,2);assert.equal(parsed.totals.movement,1);assert.equal(parsed.totals.touch,3);
  assert.deepEqual(parsed.touch[4].entries.map(entry=>entry.action),[0,1,2]);
  assert.deepEqual(parsed.touch[4].entries.map(entry=>entry.id),[7101,7101,7101]);
  assert.throws(()=>parseTh08MotionTrailer(bytes,{expectedSha256:'b'.repeat(64)}),/SHA-256/);
  const corrupt=Buffer.from(bytes);corrupt[corrupt.length-24-1]^=0x20;
  assert.throws(()=>parseTh08MotionTrailer(corrupt,{expectedSha256:sha(corrupt)}),/checksum/);
  for(const options of [{version:9},{game:7}]){
    const changed=withThMotion(replayFile(),options);
    assert.throws(()=>parseTh08MotionTrailer(changed,{expectedSha256:sha(changed)}),options.version? /version/ : /game id/);
  }
  assert.throws(()=>parseTh08MotionTrailer(replayFile(),{expectedSha256:sha(replayFile())}),/footer/);
});
