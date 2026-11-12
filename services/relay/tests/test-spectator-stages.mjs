import test from 'node:test';
import assert from 'node:assert/strict';
import {COOPERATIVE_SPECTATOR_MARKERS, SPECTATOR_STAGE_LIMITS, TH13_SPECTATOR_RUN_FRAMES, parseSpectatorStageEvent, SpectatorStageRelay} from '../src/spectator-stages.mjs';
import {isSpectatorFrameForRoom} from '../src/spectator-frame.mjs';

test('TH13 uses its own protocol for confirmed spectator input and stage headers',()=>{
  const marker=COOPERATIVE_SPECTATOR_MARKERS.th13mp;
  assert.equal(marker,0x4d);
  assert.notEqual(marker,COOPERATIVE_SPECTATOR_MARKERS.th12mp);
  for(const count of [2,3]){
    assert.equal(isSpectatorFrameForRoom('th13mp-1234',input(marker,count,0),count),true);
    assert.equal(isSpectatorFrameForRoom('th13mp-1234',input(COOPERATIVE_SPECTATOR_MARKERS.th12mp,count,0),count),false);
    const stream=new SpectatorStageRelay(marker,count);
    assert.equal(stream.accept(event(marker,count,{frame:0,stage:1,schema:1})),'checkpoint');
    stream.accept(input(marker,count,0));
    assert.equal(stream.next,1);
  }
});

function event(marker, count, {kind = 1, frame = 600, stage = 4, serial = 1, checkpoint = 1, node = 2, slots = 1, schema = 7} = {}) {
  const b = Buffer.alloc(kind === 1 ? 44 : kind === 2 ? 40 : 32);
  b.set([69, marker, 83, 86, 2, kind, count, 0]);
  b.writeBigUInt64LE(0x123456789abcdef0n, 8); b.writeUInt32LE(0x10203040, 16);
  b.writeUInt32LE(frame, 20); b.writeUInt32LE(stage, 24); b.writeUInt32LE(serial, 28);
  if (kind === 1) { b.writeUInt32LE(schema, 32); b.writeUInt32LE(4, 36); b.set([4, 10, 3, 2], 40); }
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
test('TH13 schema2 keeps the root and exact whole-run inputs for late join', () => {
  const marker = COOPERATIVE_SPECTATOR_MARKERS.th13mp;
  for (const count of [2, 3]) {
    const stream = new SpectatorStageRelay(marker, count);
    for (const root of [{frame: 1, stage: 1}, {frame: 0, stage: 2}]) {
      assert.throws(() => stream.accept(event(marker, count, {...root, schema: 2})), /root missing/);
      assert.equal(stream.history.length, 0);
    }
    const root = event(marker, count, {frame: 0, stage: 1, schema: 2});
    stream.accept(root);
    assert.equal(stream.frameLimit, TH13_SPECTATOR_RUN_FRAMES);
    assert.equal(stream.limits.historyBytes, 16 * 1024 * 1024);
    const tape = stream.history;
    let serial = 1, stage = 1;
    for (let frame = 0; frame < 136004; ++frame) {
      if (frame === 50000 || frame === 100000) {
        ++stage; ++serial;
        assert.equal(stream.accept(event(marker, count, {frame, stage, serial, schema: 2})), 'stage-bookmark');
      }
      stream.accept(input(marker, count, frame));
    }
    assert.equal(stream.history, tape);
    assert.deepEqual(stream.history[0], root);
    assert.equal(stream.frames, 136004);
    assert.equal(stream.next, 136004);
    assert.equal(stream.history.length, 136007);
    assert.equal(stream.bootstrap().readUInt32LE(20), 136004);
    assert.equal(stream.bootstrap().readUInt32LE(24), 3);
    assert.throws(() => stream.accept(event(marker, count, {frame: 136004, stage: 4, serial: 4, schema: 1})), /restore mode/);
    stream.accept(event(marker, count, {kind: 3, frame: 136004, stage: 3, serial: 4}));
    assert.equal(stream.ended, true);
  }
});
test('TH13 run retention respects explicit frame and byte limits without dropping the prefix', () => {
  const marker = COOPERATIVE_SPECTATOR_MARKERS.th13mp;
  for (const limits of [{historyBytes: 512, historyFrames: 2}, {historyBytes: 163, historyFrames: 1000}]) {
    const stream = new SpectatorStageRelay(marker, 3, limits);
    stream.accept(event(marker, 3, {frame: 0, stage: 7, schema: 2}));
    stream.accept(input(marker, 3, 0));
    if (limits.historyFrames === 2) stream.accept(input(marker, 3, 1));
    const history = stream.history, size = history.length, bytes = stream.bytes, next = stream.next;
    assert.throws(() => stream.accept(input(marker, 3, next)), /limit/);
    assert.equal(stream.history, history); assert.equal(history.length, size);
    assert.equal(stream.bytes, bytes); assert.equal(stream.next, next);
  }
});
test('TH17 continuous spectators keep earlier confirmed inputs through stage changes', () => {
  const marker = COOPERATIVE_SPECTATOR_MARKERS.th17mp;
  const stream = new SpectatorStageRelay(marker, 3, {historyBytes: 512, historyFrames: 2});
  stream.accept(event(marker, 3, {schema: 3, stage: 1}));
  const tape = stream.history;
  stream.accept(input(marker, 3, 600));
  const stageTwo = {stage: 2, frame: 601, serial: 2};
  assert.throws(() => stream.accept(event(marker, 3, stageTwo)), /restore mode/);
  assert.equal(stream.accept(event(marker, 3, {...stageTwo, schema: 3})), 'stage-bookmark');
  assert.equal(stream.history, tape, 'a downloading viewer continues on the same tape');
  stream.accept(input(marker, 3, 601));
  stream.accept(event(marker, 3, {kind: 2, stage: 2, frame: 602, serial: 3, checkpoint: 2}));
  assert.deepEqual(tape.map(row => row[5]), [1, 3, 1, 3, 2]);
  assert.equal(stream.bootstrap().readUInt32LE(20), 602);
  assert.equal(stream.bootstrap().readUInt32LE(24), 2);
  assert.throws(() => stream.accept(input(marker, 3, 602)), /frame limit/);
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
