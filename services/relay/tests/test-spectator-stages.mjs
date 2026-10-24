import test from 'node:test';
import assert from 'node:assert/strict';
import {COOPERATIVE_SPECTATOR_MARKERS, SPECTATOR_STAGE_LIMITS, parseSpectatorStageEvent, SpectatorStageRelay} from '../src/spectator-stages.mjs';
import {isSpectatorFrameForRoom} from '../src/spectator-frame.mjs';

function event(marker, count, {kind = 1, frame = 600, stage = 4, serial = 1, checkpoint = 1, node = 2, slots = 1} = {}) {
  const b = Buffer.alloc(kind === 1 ? 44 : kind === 2 ? 40 : 32);
  b.set([69, marker, 83, 86, 2, kind, count, 0]);
  b.writeBigUInt64LE(0x123456789abcdef0n, 8); b.writeUInt32LE(0x10203040, 16);
  b.writeUInt32LE(frame, 20); b.writeUInt32LE(stage, 24); b.writeUInt32LE(serial, 28);
  if (kind === 1) { b.writeUInt32LE(7, 32); b.writeUInt32LE(4, 36); b.set([4, 10, 3, 2], 40); }
  if (kind === 2) { b.writeUInt32LE(checkpoint, 32); b[36] = node; b[37] = slots; }
  return b;
}
function input(marker, count, frame) {
  const b = Buffer.alloc(24 + count * 12);
  b.set([69, marker, 78, 80, 4, 3, count, 0]);
  b.writeBigUInt64LE(0x123456789abcdef0n, 8); b.writeUInt32LE(frame, 16); b.writeUInt32LE(0x10203040, 20);
  return b;
}
for (const [product, marker] of Object.entries(COOPERATIVE_SPECTATOR_MARKERS)) for (const count of [2, 3]) {
  test(`${product}/${count}: published v1 clients retain their envelope and bootstrap version`, () => {
    const initial = event(marker, count), large = Buffer.alloc(40 + 60 * 1024);
    large.set(initial.subarray(0, 40)); large[4] = 1; large.writeUInt32LE(large.length - 40, 36);
    const stream = new SpectatorStageRelay(marker, count);
    assert.equal(stream.accept(large), 'checkpoint'); assert.equal(stream.bootstrap()[4], 1);
    stream.accept(input(marker, count, 600));
    const next = event(marker, count, {serial: 2, stage: 5, frame: 601});
    assert.throws(() => stream.accept(next), /identity/);
    assert.equal(stream.next, 601); assert.equal(stream.history.length, 2);
    next[4] = 1; assert.equal(stream.accept(next), 'checkpoint'); assert.equal(stream.bootstrap()[4], 1);
    const tooLarge = Buffer.concat([large, Buffer.from([0])]); tooLarge.writeUInt32LE(tooLarge.length - 40, 36);
    assert.equal(parseSpectatorStageEvent(tooLarge, marker, count), null);
    const current = new SpectatorStageRelay(marker, count); current.accept(initial);
    next[4] = 1; assert.throws(() => current.accept(next), /identity/); assert.equal(current.bootstrap()[4], 2);
  });
}
for (const [product, marker] of Object.entries(COOPERATIVE_SPECTATOR_MARKERS)) for (const count of [2, 3]) {
  test(`${product}/${count}: nonzero stage bootstrap, Boss dependency and exact next-stage replacement`, () => {
    const stream = new SpectatorStageRelay(marker, count);
    const initial = event(marker, count);
    assert.equal(stream.accept(initial), 'checkpoint'); initial[40] = 99;
    assert.equal(stream.history[0][40], 4, 'history owns its exact allocation');
    stream.accept(input(marker, count, 600));
    stream.accept(event(marker, count, {kind: 2, serial: 2, frame: 601}));
    const old = stream.history;
    assert.equal(old.length, 3, 'Boss retains the complete stage prefix');
    stream.accept(event(marker, count, {serial: 3, stage: 5, frame: 601}));
    assert.notEqual(stream.history, old); assert.equal(old.length, 3, 'in-flight old replay remains complete');
    assert.equal(stream.history.length, 1); assert.equal(stream.stage, 5);
    stream.accept(input(marker, count, 601));
    stream.accept(event(marker, count, {kind: 3, serial: 4, stage: 5, frame: 602}));
    assert.equal(stream.ended, true); assert.throws(() => stream.accept(input(marker, count, 602)), /ended/);
  });
}
test('reject malformed, cross-product, stale and missing-dependency events without changing history', () => {
  const marker = 55, initial = event(marker, 3), stream = new SpectatorStageRelay(marker, 3);
  for (let length = 0; length < initial.length; ++length) assert.equal(parseSpectatorStageEvent(initial.subarray(0, length), marker, 3), null);
  assert.equal(parseSpectatorStageEvent(initial, 54, 3), null);
  assert.equal(COOPERATIVE_SPECTATOR_MARKERS.th09mp, undefined);
  stream.accept(initial);
  for (const broken of [event(marker, 3), event(marker, 3, {frame: 602, serial: 2}),
    event(marker, 3, {kind: 2, frame: 600, serial: 2, checkpoint: 99})]) {
    assert.throws(() => stream.accept(broken)); assert.equal(stream.history.length, 1); assert.equal(stream.serial, 1);
  }
  const nan = input(marker, 3, 600); nan.writeFloatLE(NaN, 28);
  assert.throws(() => stream.accept(nan), /movement/); assert.equal(stream.next, 600);
});
test('history limits fail before evicting a still-needed input prefix', () => {
  const stream = new SpectatorStageRelay(55, 2, {historyBytes: 200, historyFrames: 1});
  stream.accept(event(55, 2)); stream.accept(input(55, 2, 600));
  assert.throws(() => stream.accept(input(55, 2, 601)), /frame limit/);
  assert.equal(stream.history.length, 2); assert.equal(stream.next, 601);
});
test('native Replay stage headers fit inside the bounded relay envelope',()=>{
  const header=event(70,3),large=Buffer.alloc(40+SPECTATOR_STAGE_LIMITS.stateBytes);
  large.set(header.subarray(0,40));large.writeUInt32LE(large.length-40,36);
  assert.ok(large.length+1<64*1024);
  const stream=new SpectatorStageRelay(70,3);assert.equal(stream.accept(large),'checkpoint');
  const oversized=Buffer.concat([large,Buffer.from([1])]);oversized.writeUInt32LE(oversized.length-40,36);
  assert.equal(parseSpectatorStageEvent(oversized,70,3),null);
});

test('TH15 measured pure-delay timing remains before a nonzero stage bootstrap',()=>{
  const stream=new SpectatorStageRelay(70,3),timing=Buffer.alloc(40);
  timing.set([69,70,84,77,1,1,2,0]);timing.writeUInt32LE(1,8);
  timing.writeBigUInt64LE(0x123456789abcdef0n,12);timing.writeUInt32LE(0x10203040,20);
  timing.writeUInt32LE(32000,24);timing.writeUInt32LE(120,36);
  assert.equal(stream.accept(timing),'timing');stream.accept(event(70,3));
  stream.accept(input(70,3,600));assert.deepEqual(stream.history.map(p=>[p[2],p[3]]),[[84,77],[83,86],[78,80]]);
  const hybrid=Buffer.from(timing);hybrid[5]=2;assert.equal(isSpectatorFrameForRoom('th15mp-7403',hybrid,3),false);
});
