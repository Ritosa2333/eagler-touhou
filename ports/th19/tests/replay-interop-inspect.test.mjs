import test from 'node:test';
import assert from 'node:assert/strict';
import { inspectReplay } from '../tools/replay/inspect-replay.mjs';
import { encodeJunowen } from '../tools/replay/junowen-format.mjs';
import { encodeTh19Replay, th19StreamFrame } from '../web/sdl-runtime/th19-recording.mjs';

test('native header has no auto-detect signature; explicit inspection preserves settings', () => {
  const header = {randSeed1:1,randSeed2:2,difficulty:3,matchup:0,common:0x90,p1Settings:12,p2Settings:12,p1Character:0,p2Character:18,p1Card:0,p2Card:0};
  const bytes = encodeJunowen({header,inputs:[[1,256],[0,0]]});
  assert.throws(() => inspectReplay(bytes), /specify --format/);
  const report = inspectReplay(bytes, {format:'junowen'});
  assert.deepEqual(report.settings, {timeLimitIndex:0,roundIndex:1,cardMode:2,lifeIndices:[4,4],barrierModes:[1,1]});
  assert.equal(report.byteExactRoundtrip, true);
  assert.equal(report.nativePlaybackVerified, false);
  assert.equal(report.rngStateFields, 2);
  assert.equal(report.roundBoundaryRecords, false);
  assert.match(report.warnings[0], /prior-round inputs/);
});

test('CPU recorder/reader layout disagreement and unmapped flags are reported', () => {
  const bytes = new Uint8Array(21);
  bytes[9]=2;bytes[17]=0;bytes[18]=2;
  assert.equal(inspectReplay(bytes,{format:'junowen'}).warnings.length, 2);
});

test('web v2 identifies exact barriers without substituting guessed seeds or dropping pointer input', () => {
  const build='a'.repeat(64);
  const config={rules:[0,1,1,4,4,1,1,0,0],protocol:1,build,seed:123,difficulty:3,characters:[0,18],delay:0,rollback:true};
  const left={keys:1024,direct:1,x:2,y:3}, right={keys:256,direct:0,x:0,y:0};
  const bytes=encodeTh19Replay({kind:'versus',config,build,seed:123,frames:1,startedAt:0},th19StreamFrame(0,left,right,{played:true}));
  const before=bytes.slice(), report=inspectReplay(bytes);
  assert.equal(report.format,'web');
  assert.deepEqual(report.counts,{directMovement:1,unsupportedKeys:1,pause:1});
  assert.equal(report.conversionBlockers.length,5);
  assert.equal(report.nativePlaybackVerified,false);
  assert.deepEqual(bytes,before);
  bytes[bytes.length-1]^=1;
  assert.throws(()=>inspectReplay(bytes),/校验/);
});
