import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';

// ReplayFile.cpp / ResourceCodec.cpp: operate on a private packed-buffer copy.
// The existing oracle decoder is injected so there is no second LZSS implementation.
export function inspectReplay(bytes, decodeLzss) {
  assert(bytes.length >= 0x24 && bytes.toString('ascii', 0, 4) === 't10r', 'Not a TH10 replay');
  assert.equal(bytes.readUInt16LE(4), 5, 'Unsupported replay format');
  assert.equal(bytes.readUInt32LE(0x10), 0x100, 'Unsupported replay version');
  const packedSize = bytes.readUInt32LE(0x1c), unpackedSize = bytes.readUInt32LE(0x20);
  assert(packedSize > 0 && packedSize <= bytes.length - 0x24, 'Truncated packed stream');
  assert(unpackedSize >= 0x64 && unpackedSize <= 64 * 1024 * 1024, 'Invalid unpacked size');
  const packed = Buffer.from(bytes.subarray(0x24, 0x24 + packedSize));
  function transform(key, step, block) {
    const source = Buffer.from(packed), remainder = packed.length % block;
    let remaining = packed.length - (packed.length & 1) - (remainder < block / 4 ? remainder : 0), offset = 0;
    while (remaining > 0) {
      block = Math.min(block, remaining);
      let cursor = offset;
      for (let lane = 0; lane < 2; lane++) {
        for (let index = block - 1 - lane; index >= 0; index -= 2) {
          packed[offset + index] = source[cursor++] ^ key;
          key = (key + step) & 255;
        }
      }
      offset += block;
      remaining -= block;
    }
  }
  transform(0xaa, 0xe1, 0x400);
  transform(0x3d, 0x7a, 0x80);
  const data = Buffer.alloc(unpackedSize);
  assert.equal(decodeLzss(packed, data, new Uint8Array(8192)), unpackedSize, 'Incomplete decompression');
  const count = data.readInt32LE(0x4c);
  assert(count >= 1 && count <= 7, 'Unsupported stage-count encoding');
  const stages = [];
  let offset = 0x64;
  for (let index = 0; index < count; index++) {
    assert(offset + 0x1c4 <= data.length, 'Truncated stage header');
    const stage = data.readInt16LE(offset), frames = data.readInt32LE(offset + 4);
    const streamBytes = data.readInt32LE(offset + 8);
    assert(stage >= 1 && stage <= 7 && !stages.some(row => row.stage === stage), 'Invalid stage identity');
    assert(frames > 0 && streamBytes >= frames * 6, 'Invalid input stream size');
    stages.push({stage, frames, score: data.readInt32LE(offset + 12) * 10});
    offset += 0x1c4 + streamBytes;
    assert(offset <= data.length, 'Truncated stage input');
  }
  assert.equal(offset, data.length, 'Unaccounted replay payload');
  return {
    sha256: createHash('sha256').update(bytes).digest('hex'),
    character: data.readInt32LE(0x50), shot: data.readInt32LE(0x54),
    difficulty: data.readInt32LE(0x58), lastStage: data.readInt32LE(0x5c),
    score: data.readInt32LE(0x10) * 10, stages,
  };
}

// These are file/route/score checkpoints, not a candidate-vs-original tick gate.
export function validatePlayback(expected, result) {
  assert.equal(result.replay?.sha256, expected.sha256, 'Replay identity mismatch');
  assert.equal(expected.lastStage, 8, 'This gate requires a clear replay');
  assert.equal(result.final?.replayMode, 1, 'Replay did not return to the title mode');
  assert.deepEqual(result.transitions?.map(row => row.stage), expected.stages.map(row => row.stage), 'Recorded route was not completed');
  for (const [index, actual] of result.transitions.entries()) {
    assert.equal(actual.replayMode, 2, 'Stage observed outside gameplay');
    assert.equal(actual.score, expected.stages[index].score, `Stage ${actual.stage} entry score differs`);
    for (const field of ['character', 'shot', 'difficulty'])
      assert.equal(actual[field], expected[field], `Stage ${actual.stage} ${field} differs`);
  }
  assert.equal(result.final.stage, expected.stages.at(-1).stage, 'Wrong final stage');
  assert.equal(result.final.score, expected.score, 'Final recorded score differs');
  for (const field of ['character', 'shot', 'difficulty'])
    assert.equal(result.final[field], expected[field], `Final ${field} differs`);
  return {passed: true, coverage: 'original-playback-route-and-score-checkpoints', deterministicEquivalence: false};
}
