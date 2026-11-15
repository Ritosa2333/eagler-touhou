import assert from 'node:assert/strict';
import { createHash } from 'node:crypto';
import { readFileSync } from 'node:fs';
import { dirname, resolve } from 'node:path';
import { spawnSync } from 'node:child_process';
import { fileURLToPath } from 'node:url';
import test from 'node:test';
import { decodeReplay, encodeReplay } from '../site/runtime/replay.mjs';

const project = resolve(dirname(fileURLToPath(import.meta.url)), '..');
const maxCompressed = 8 * 1024 * 1024;
const maxUncompressed = 32 * 1024 * 1024;
const maxFile = 16 * 1024 * 1024;
const maxFrames = 1000000;
const view = data => new DataView(data.buffer, data.byteOffset, data.byteLength);
const base64 = data => Buffer.from(data).toString('base64');
const sha256 = data => createHash('sha256').update(data).digest('hex');
const formatError = error => error instanceof Error
  && error.name === 'ReplayFormatError' && error.code === 'TH128_REPLAY_FORMAT';

function join(...parts) {
  const result = new Uint8Array(parts.reduce((total, part) => total + part.length, 0));
  let position = 0;
  for (const part of parts) {
    result.set(part, position);
    position += part.length;
  }
  return result;
}

function bits(data) {
  const padded = data.padEnd(Math.ceil(data.length / 8) * 8, '0');
  return Uint8Array.from(padded.match(/.{8}/g) ?? [], byte => Number.parseInt(byte, 2));
}

function literalBits(data) {
  return Array.from(data, value => `1${value.toString(2).padStart(8, '0')}`).join('');
}

function referenceEncrypt(data, block, initial, increment) {
  const result = data.slice();
  const remainder = data.length % block;
  const count = Math.max(0, data.length - (remainder < block / 4 ? remainder : 0) - data.length % 2);
  let cursor = 0;
  let mask = initial;
  for (let start = 0; start < count; start += block) {
    const size = Math.min(block, count - start);
    const order = [];
    for (let index = size - 1; index >= 0; index -= 2) order.push(index);
    for (let index = size - 2; index >= 0; index -= 2) order.push(index);
    for (const index of order) {
      result[cursor++] = data[start + index] ^ mask;
      mask = (mask + increment) % 256;
    }
  }
  return result;
}

function payloadFixture() {
  const metadata = Uint8Array.from({ length: 112 }, (_, index) => (index * 37 + 9) % 256);
  metadata.set([84, 101, 115, 116, 101, 114, 0, 129, 255, 65, 73, 76]);
  view(metadata).setBigUint64(12, 1281268800n, true);
  view(metadata).setUint32(20, 1234, true);
  view(metadata).setFloat32(84, 1.25, true);
  for (const [offset, value] of [[88, 1], [92, 5], [96, 0x12345678], [100, 2], [104, 15], [108, 0x89abcdef]]) {
    view(metadata).setUint32(offset, value, true);
  }
  const stage = Uint8Array.from({ length: 144 }, (_, index) => (index * 13 + 7) % 256);
  view(stage).setUint16(0, 15, true);
  view(stage).setUint16(2, 0x1234, true);
  for (const [offset, value] of [[4, 2], [8, 14], [12, 10], [16, 1], [20, 1000000],
    [32, 1], [36, 0xcafebabe], [40, 99], [128, 5000], [132, 10000]]) {
    view(stage).setUint32(offset, value, true);
  }
  view(stage).setInt32(24, -128, true);
  view(stage).setInt32(28, 256, true);
  view(stage).setFloat32(136, 12.5, true);
  const inputs = new Uint8Array(12);
  [65535, 65535, 65535, 513, 512, 0].forEach((value, index) => view(inputs).setUint16(index * 2, value, true));
  return join(metadata, stage, inputs, Uint8Array.of(60, 59), new TextEncoder().encode('unknown-payload'));
}

function userBlock(data, kind) {
  const header = Uint8Array.of(85, 83, 69, 82, 0, 0, 0, 0, 0, 0, 0, 0);
  view(header).setUint32(4, data.length + 12, true);
  view(header).setUint32(8, kind, true);
  return join(header, data);
}

function nativeFixture({ payload = payloadFixture(), compressed, user = true,
  gap = Uint8Array.of(9, 8, 7), tail = Uint8Array.of(222, 173, 190, 239) } = {}) {
  compressed ??= bits(`${literalBits(payload)}00`);
  const encrypted = referenceEncrypt(referenceEncrypt(compressed, 0x80, 0x7d, 0x36), 0x800, 0x5e, 0xe7);
  const header = new Uint8Array(36);
  header.set([49, 50, 56, 114]);
  for (const [offset, value] of [[4, 2], [8, 0xdeadbeef], [12, user ? 36 + encrypted.length + gap.length : 0],
    [16, 0x100], [20, 0x12345678], [24, 0x89abcdef], [28, encrypted.length], [32, payload.length]]) {
    view(header).setUint32(offset, value, true);
  }
  return join(header, encrypted, ...(user ? [gap, userBlock(Uint8Array.of(130, 160, 0), 0),
    userBlock(Uint8Array.of(255, 0), 1)] : []), tail);
}

function summary(decoded) {
  const metadata = decoded.metadata;
  return {
    format: decoded.format, timestamp: metadata.timestamp.toString(), name: metadata.name,
    route: metadata.route, rank: metadata.rank, scoreRaw: metadata.scoreRaw,
    payload: base64(decoded.rawPayload), gap: base64(decoded.dataGapRaw), tail: base64(decoded.fileTailRaw),
    userHeaders: decoded.userBlocks.map(block => base64(block.headerRaw)),
    userData: decoded.userBlocks.map(block => base64(block.data)),
    stages: decoded.stages.map(stage => ({ stage: stage.stage, seed: stage.seed, frames: stage.numFrames,
      fpsBytes: stage.fpsRaw.length, first: stage.inputs.length ? stage.inputs.at(0) : null,
      last: stage.inputs.length ? stage.inputs.at(-1) : null })),
  };
}

function editDecoded(decoded) {
  view(decoded.metadata.raw).setUint32(20, decoded.metadata.scoreRaw + 17, true);
  decoded.stages[0].inputRaw = join(decoded.stages[0].inputRaw, Uint8Array.of(0, 128, 1, 0, 2, 0));
  decoded.stages[0].fpsRaw = join(decoded.stages[0].fpsRaw, Uint8Array.of(60));
  decoded.stages[0].rawHeader[44] ^= 165;
  decoded.userBlocks[0].data = join(decoded.userBlocks[0].data, Uint8Array.of(0, 255));
}

function pythonReference(binary, mode = 'unchanged') {
  const script = `import base64,json,struct,sys
sys.path.insert(0,'tools')
from th128_assets.replay import decode_replay,encode_replay
request=json.load(sys.stdin)
decoded=decode_replay(base64.b64decode(request['binary']))
if request['mode']=='edited':
    raw=bytearray(decoded['metadata']['raw'])
    struct.pack_into('<I',raw,20,decoded['metadata']['scoreRaw']+17)
    decoded['metadata']['raw']=bytes(raw)
    stage=decoded['stages'][0]
    stage['inputRaw']+=struct.pack('<3H',0x8000,1,2)
    stage['fpsRaw']+=bytes([60])
    header=bytearray(stage['rawHeader'])
    header[44]^=165
    stage['rawHeader']=bytes(header)
    decoded['userBlocks'][0]['data']+=bytes([0,255])
if request['mode']=='recompress':
    decoded['encodedPayload']=b''
encoded=encode_replay(decoded)
decoded=decode_replay(encoded)
def b64(data): return base64.b64encode(data).decode('ascii')
metadata=decoded['metadata']
result={'format':decoded['format'],'timestamp':str(metadata['timestamp']),'name':metadata['name'],
    'route':metadata['route'],'rank':metadata['rank'],'scoreRaw':metadata['scoreRaw'],
    'payload':b64(decoded['rawPayload']),'gap':b64(decoded['dataGapRaw']),'tail':b64(decoded['fileTailRaw']),
    'userHeaders':[b64(block['headerRaw']) for block in decoded['userBlocks']],
    'userData':[b64(block['data']) for block in decoded['userBlocks']],
    'stages':[{'stage':stage['stage'],'seed':stage['seed'],'frames':stage['numFrames'],
      'fpsBytes':len(stage['fpsRaw']),'first':list(stage['inputs'][0]) if stage['numFrames'] else None,
      'last':list(stage['inputs'][-1]) if stage['numFrames'] else None} for stage in decoded['stages']]}
print(json.dumps({'summary':result,'encoded':b64(encoded)}))`;
  const result = spawnSync(process.env.TH128_PYTHON || 'python', ['-B', '-c', script], {
    cwd: project, input: JSON.stringify({ binary: base64(binary), mode }), encoding: 'utf8',
    env: { ...process.env, PYTHONDONTWRITEBYTECODE: '1' }, maxBuffer: 8 * 1024 * 1024,
  });
  assert.ifError(result.error);
  assert.equal(result.status, 0, result.stderr);
  return JSON.parse(result.stdout);
}

test('synthetic native metadata, unknown words, USER tags and lazy tuples survive', () => {
  const original = nativeFixture();
  const decoded = decodeReplay(original);
  assert.equal(decoded.format, 'th128-replay/1');
  assert.equal(decoded.metadata.name, 'Tester');
  assert.equal(decoded.metadata.timestamp, 1281268800n);
  assert.equal(decoded.metadata.score, 12340);
  assert.equal(decoded.metadata.routeName, 'C2');
  assert.equal(decoded.metadata.rankName, 'Hard');
  assert.deepEqual(decoded.rawPayload, payloadFixture());
  const stage = decoded.stages[0];
  assert.equal(stage.stageName, 'C2-3');
  assert.equal(stage.seed, 0x1234);
  assert.deepEqual(stage.positionRaw, [-128, 256]);
  assert.deepEqual(stage.position, [-1, 2]);
  assert.equal(stage.maxPiv, 10000);
  assert.equal(stage.motivation, 50);
  assert.equal(stage.perfectFreeze, 100);
  assert.equal(stage.inputs.length, 2);
  assert.equal(Array.isArray(stage.inputs), false);
  assert.deepEqual(stage.inputs.at(0), [65535, 65535, 65535]);
  assert.deepEqual(stage.inputs.at(-1), [513, 512, 0]);
  assert.deepEqual([...stage.inputs], [[65535, 65535, 65535], [513, 512, 0]]);
  assert.equal(decoded.userBlocks[0].text, '\u3042\0');
  assert.deepEqual(decoded.userBlocks.map(block => block.kind), [0, 1]);
  assert.deepEqual(encodeReplay(decoded), original);
});

test('native file offsets respect Uint8Array byteOffset and Buffer pooling', () => {
  const original = nativeFixture();
  const padded = new Uint8Array(original.length + 29).fill(170);
  padded.set(original, 13);
  const decoded = decodeReplay(padded.subarray(13, 13 + original.length));
  padded.fill(0);
  assert.deepEqual(encodeReplay(decoded), original);
  assert.deepEqual(encodeReplay(decodeReplay(Buffer.from(original))), original);
});

test('a uint64 timestamp is bigint, including values above Number precision', () => {
  const payload = payloadFixture();
  view(payload).setBigUint64(12, 0xffffffffffffffffn, true);
  const original = nativeFixture({ payload });
  const decoded = decodeReplay(original);
  assert.equal(decoded.metadata.timestamp, 18446744073709551615n);
  assert.deepEqual(encodeReplay(decoded), original);
  assert.deepEqual(summary(decoded), pythonReference(original).summary);
});

test('frame indices reject coercion, nonintegers, unsafe numbers and out-of-range positions', () => {
  const inputs = decodeReplay(nativeFixture()).stages[0].inputs;
  for (const index of ['0', 0n, false, null, undefined, NaN, Infinity, 0.5,
    Number.MAX_SAFE_INTEGER + 1, 2, -3, {}, { valueOf: () => 0 }]) {
    assert.throws(() => inputs.at(index), formatError);
  }
  assert.throws(() => { inputs.length = 400; }, TypeError);
});

test('strict typed-array and encode-option validation does not coerce malformed values', () => {
  for (const data of [null, [], {}, new ArrayBuffer(36), new Uint16Array(36), '128r',
    { length: 36, 0: 49 }, Object.create(Uint8Array.prototype)]) {
    assert.throws(() => decodeReplay(data), formatError);
  }
  const decoded = decodeReplay(nativeFixture());
  for (const options of [null, false, 1, 'true', [], { recompress: 1 }, { recompress: 'false' },
    { recompress: null }, { recompress: {} }]) {
    assert.throws(() => encodeReplay(decoded, options), formatError);
  }
  for (const value of [null, false, [], '1', 1]) assert.throws(() => encodeReplay(value), formatError);
});

test('detached, shared and resizable byte storage fails with an explicit codec error', () => {
  const transferred = nativeFixture();
  structuredClone(transferred.buffer, { transfer: [transferred.buffer] });
  assert.throws(() => decodeReplay(transferred), formatError);
  if (typeof SharedArrayBuffer !== 'undefined') {
    assert.throws(() => decodeReplay(new Uint8Array(new SharedArrayBuffer(36))), formatError);
  }
  const resizable = new ArrayBuffer(36, { maxByteLength: 72 });
  assert.throws(() => decodeReplay(new Uint8Array(resizable)), formatError);
  const decoded = decodeReplay(nativeFixture());
  structuredClone(decoded.stages[0].inputRaw.buffer, { transfer: [decoded.stages[0].inputRaw.buffer] });
  assert.throws(() => decoded.stages[0].inputs.at(0), formatError);
  assert.throws(() => encodeReplay(decoded), formatError);
});

test('raw edits, regenerated sizes and forced encoding match Python byte for byte', () => {
  const original = nativeFixture();
  for (const mode of ['unchanged', 'recompress', 'edited']) {
    const decoded = decodeReplay(original);
    if (mode === 'edited') editDecoded(decoded);
    const encoded = encodeReplay(decoded, { recompress: mode === 'recompress' });
    const reference = pythonReference(original, mode);
    assert.equal(base64(encoded), reference.encoded);
    assert.deepEqual(summary(decodeReplay(encoded)), reference.summary);
  }
});

test('cache verification repairs corrupt ciphertext rather than trusting it', () => {
  const decoded = decodeReplay(nativeFixture());
  decoded.encodedPayload = Uint8Array.of(1, 2, 3);
  assert.deepEqual(decodeReplay(encodeReplay(decoded)).rawPayload, payloadFixture());
});

test('all raw unknown tails, absent USER, unknown tags and NaN payloads remain exact', () => {
  for (const user of [true, false]) {
    const payload = payloadFixture();
    for (const [offset, value] of [[84, 0x7fc00001], [92, 0xffffffff], [100, 0xffffffff],
      [112 + 136, 0x7fc00002]]) view(payload).setUint32(offset, value, true);
    const original = nativeFixture({ payload, user });
    const decoded = decodeReplay(original);
    assert.equal(decoded.metadata.routeName, null);
    assert.equal(decoded.metadata.rankName, null);
    assert.equal(Number.isNaN(decoded.metadata.slowdown), true);
    assert.deepEqual(encodeReplay(decoded), original);
    assert.deepEqual(decodeReplay(encodeReplay(decoded, { recompress: true })).rawPayload, payload);
    if (user) {
      view(decoded.userBlocks[1].headerRaw).setUint32(8, 0xdeadbeef, true);
      assert.equal(decodeReplay(encodeReplay(decoded)).userBlocks[1].kind, 0xdeadbeef);
    }
  }
});

test('derived scalar and aggregate rawPayload views are not silently coerced into edits', () => {
  const original = nativeFixture();
  const decoded = decodeReplay(original);
  decoded.metadata.score = '9000';
  decoded.metadata.timestamp = 1;
  decoded.stages[0].numFrames = '3';
  decoded.rawPayload = new Uint8Array();
  assert.deepEqual(encodeReplay(decoded), original);
});

test('header magic, version, offsets, declarations and hard caps are validated', () => {
  const original = nativeFixture();
  for (const [offset, value] of [[0, 0], [4, 3], [12, 35], [12, original.length],
    [12, 0xffffffff], [28, 0], [28, original.length], [28, maxCompressed + 1],
    [32, 111], [32, maxUncompressed + 1]]) {
    const damaged = original.slice();
    view(damaged).setUint32(offset, value, true);
    assert.throws(() => decodeReplay(damaged), formatError);
  }
  assert.throws(() => decodeReplay(new Uint8Array(maxFile + 1)), formatError);
});

test('every truncated synthetic prefix fails instead of returning partial records', () => {
  const original = nativeFixture({ tail: new Uint8Array() });
  for (let length = 0; length < original.length; length += 1) {
    assert.throws(() => decodeReplay(original.subarray(0, length)), formatError);
  }
});

test('stage counts, partial input bodies, frame limits and aggregate budgets fail explicitly', () => {
  for (const [offset, value] of [[88, 0], [88, 4], [112 + 4, maxFrames + 1],
    [112 + 8, 11], [112 + 8, 0xffffffff]]) {
    const payload = payloadFixture();
    view(payload).setUint32(offset, value, true);
    assert.throws(() => decodeReplay(nativeFixture({ payload })), formatError);
  }
  const single = payloadFixture();
  const payload = join(single.slice(0, 270), single.slice(112, 270));
  view(payload).setUint32(88, 2, true);
  view(payload).setUint32(270 + 4, maxFrames, true);
  assert.throws(() => decodeReplay(nativeFixture({ payload })), /frame budget/);
  const decoded = decodeReplay(nativeFixture());
  const manyInputs = new Uint8Array(600000 * 6);
  decoded.stages = [{ ...decoded.stages[0], inputRaw: manyInputs },
    { ...decoded.stages[0], inputRaw: manyInputs }];
  assert.throws(() => encodeReplay(decoded), /frame budget/);
});

test('three-stage parsing preserves boundaries and unknown final bytes', () => {
  const single = payloadFixture();
  const payload = join(single.slice(0, 112), single.slice(112, 270), single.slice(112, 270),
    single.slice(112, 270), Uint8Array.of(1, 2, 3));
  view(payload).setUint32(88, 3, true);
  const original = nativeFixture({ payload });
  const decoded = decodeReplay(original);
  assert.deepEqual(decoded.stages.map(stage => stage.inputs.length), [2, 2, 2]);
  assert.deepEqual(decoded.payloadTailRaw, Uint8Array.of(1, 2, 3));
  assert.deepEqual(encodeReplay(decoded), original);
});

test('USER uint32 sizes never absorb the independent tag or escape the file', () => {
  const original = nativeFixture();
  const start = view(original).getUint32(12, true);
  for (const [offset, value] of [[start, 0], [start + 4, 0], [start + 4, 11],
    [start + 4, original.length], [start + 4, 4 * 1024 * 1024 + 1], [start + 4, 0xffffffff]]) {
    const damaged = original.slice();
    view(damaged).setUint32(offset, value, true);
    assert.throws(() => decodeReplay(damaged), formatError);
  }
});

test('partial LZSS tokens, uninitialized offsets and expansion overruns abort', () => {
  const payload = payloadFixture();
  const whole = literalBits(payload);
  const malformed = [bits('0' + '0000000000001' + '0000' + whole), bits('00' + whole),
    bits(whole.slice(0, -16)), bits(whole + '1'), bits(whole + '0'.repeat(40))];
  for (const compressed of malformed) {
    assert.throws(() => decodeReplay(nativeFixture({ payload, compressed })), formatError);
  }
  const prefix = payload.slice(0, -1);
  const compressed = bits(literalBits(prefix) + '0' + '0000000000001' + '1111' + '00');
  assert.throws(() => decodeReplay(nativeFixture({ payload, compressed })), /expansion/);
});

test('overlapping references and initialized ring wrap decode native LZSS vectors', () => {
  const prefix = join(payloadFixture(), new Uint8Array(8193 - payloadFixture().length));
  const compressed = bits(literalBits(prefix) + '0' + '0000000000001' + '0000' + '00');
  const payload = join(prefix, new Uint8Array(3));
  const decoded = decodeReplay(nativeFixture({ payload, compressed }));
  assert.deepEqual(decoded.rawPayload, payload);
  const crossing = bits(literalBits(prefix) + '0' + '1111111111111' + '0000' + '00');
  assert.deepEqual(decodeReplay(nativeFixture({ payload, compressed: crossing })).rawPayload, payload);
});

test('encoder rejects invalid raw byte components and oversized budgets before allocation', () => {
  for (const [key, value] of [['rawHeader', new Uint8Array(1)], ['inputRaw', Uint8Array.of(1)],
    ['fpsRaw', []], ['fpsRaw', new Uint8Array(maxUncompressed + 1)]]) {
    const decoded = decodeReplay(nativeFixture());
    decoded.stages[0][key] = value;
    assert.throws(() => encodeReplay(decoded), formatError);
  }
  const decoded = decodeReplay(nativeFixture());
  decoded.payloadTailRaw = new Uint8Array(Math.floor(maxCompressed * 8 / 9));
  assert.throws(() => encodeReplay(decoded, { recompress: true }), /compressed size/);
});

test('all three actual demos match pinned metadata, roundtrips and Python edited bytes', async context => {
  let catalog;
  try {
    catalog = JSON.parse(readFileSync(resolve(project, 'local/catalog.json'), 'utf8'));
  } catch (error) {
    if (error.code === 'ENOENT') return context.skip('private prepared catalog is not present');
    throw error;
  }
  const raw = resolve(project, 'local', dirname(catalog.variants.ja.manifest), 'raw');
  const expected = [
    ['33024e1b08c2a0f27c0b84f4a49d609e4b931d15c0359f88406cd5b257973c9e', 0, 1, 1, 6686, 40595],
    ['26adf86f10326d3672bfabf4a034f1a2a1fa606416cd3f29529c7550c3a3bcc6', 2, 1, 6, 6722, 40813],
    ['53d688e575961f793fba91621045d4dd520719b05fe0feadfb4905c4822866b3', 4, 2, 11, 6094, 37024],
  ];
  for (const [index, [digest, route, rank, stage, frames, size]] of expected.entries()) {
    await context.test(`demo${index}: decode, preserve, recompress, edit and cross-check`, () => {
      const original = new Uint8Array(readFileSync(resolve(raw, `demo${index}.rpy`)));
      assert.equal(sha256(original), digest);
      const decoded = decodeReplay(original);
      assert.equal(decoded.metadata.name, 'ZUN     ');
      assert.equal(decoded.metadata.route, route);
      assert.equal(decoded.metadata.rank, rank);
      assert.equal(decoded.stages[0].stage, stage);
      assert.equal(decoded.stages[0].inputs.length, frames);
      assert.equal(decoded.rawPayload.length, size);
      for (const mode of ['unchanged', 'recompress', 'edited']) {
        const candidate = decodeReplay(original);
        if (mode === 'edited') editDecoded(candidate);
        const encoded = encodeReplay(candidate, { recompress: mode === 'recompress' });
        const reference = pythonReference(original, mode);
        assert.equal(base64(encoded), reference.encoded);
        assert.deepEqual(summary(decodeReplay(encoded)), reference.summary);
        if (mode === 'unchanged') assert.deepEqual(encoded, original);
      }
    });
  }
});
