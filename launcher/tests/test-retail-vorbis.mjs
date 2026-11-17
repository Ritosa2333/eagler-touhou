import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {test} from 'vitest';
import {createEncoder} from 'wasm-media-encoders/esnext';
import {encodeRetailVorbis} from '../app/services/retail-vorbis';

function chunk(name, data) {
  const header = Buffer.alloc(8);
  header.write(name);
  header.writeUInt32LE(data.length, 4);
  return Buffer.concat([header, data, Buffer.alloc(data.length & 1)]);
}

function wave(pcm, channels, sampleRate) {
  const format = Buffer.alloc(16);
  format.writeUInt16LE(1, 0);
  format.writeUInt16LE(channels, 2);
  format.writeUInt32LE(sampleRate, 4);
  format.writeUInt32LE(sampleRate * channels * 2, 8);
  format.writeUInt16LE(channels * 2, 12);
  format.writeUInt16LE(16, 14);
  return new Blob([chunk('RIFF', Buffer.concat([Buffer.from('WAVE'),
    chunk('JUNK', Buffer.from([17])), chunk('fmt ', format), chunk('data', pcm)]))]);
}

async function oggInfo(blob) {
  const bytes = Buffer.from(await blob.arrayBuffer());
  assert.equal(bytes.toString('ascii', 28, 35), '\x01vorbis');
  const channels = bytes[39];
  const sampleRate = bytes.readUInt32LE(40);
  for (let offset = 0; offset < bytes.length;) {
    assert.equal(bytes.toString('ascii', offset, offset + 4), 'OggS');
    const flags = bytes[offset + 5], segments = bytes[offset + 26], header = offset + 27;
    const next = header + segments + bytes.subarray(header, header + segments).reduce((a, b) => a + b, 0);
    if (flags & 4) {
      assert.equal(next, bytes.length);
      return {channels, sampleRate, samples: Number(bytes.readBigInt64LE(offset + 6))};
    }
    offset = next;
  }
  assert.fail('Missing Vorbis end of stream');
}

test('the Vorbis encoder preserves WAV format and sample count, then resets for a raw PCM track', async () => {
  const encoder = await createEncoder('audio/ogg', await readFile(new URL(import.meta.resolve('wasm-media-encoders/wasm/ogg'))));
  const wavSamples = 70003;
  const mono = Buffer.alloc(wavSamples * 2);
  for (let i = 0; i < wavSamples; i++) mono.writeInt16LE(Math.round(24000 * Math.sin(i / 7)), i * 2);
  const wav = await encodeRetailVorbis(wave(mono, 1, 32000), false, encoder);
  assert.deepEqual(await oggInfo(wav), {channels: 1, sampleRate: 32000, samples: wavSamples});
  const raw = await encodeRetailVorbis(new Blob([Buffer.alloc(4097 * 4)]), true, encoder);
  assert.deepEqual(await oggInfo(raw), {channels: 2, sampleRate: 44100, samples: 4097});
  await assert.rejects(encodeRetailVorbis(new Blob([Buffer.alloc(7)]), true, encoder), /Invalid PCM sample count/);
  await assert.rejects(encodeRetailVorbis(wave(mono, 1, 32000).slice(0, 110), false, encoder), /Truncated WAV file/);
});
