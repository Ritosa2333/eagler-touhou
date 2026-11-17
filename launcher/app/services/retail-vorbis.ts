import type {WasmMediaEncoder} from 'wasm-media-encoders/esnext';

interface PcmSource {
  channels: 1 | 2;
  sampleRate: number;
  offset: number;
  length: number;
  bytesPerSample: number;
  readSample(view: DataView, offset: number): number;
}

const signed16 = (view: DataView, offset: number) => view.getInt16(offset, true) / 32768;

async function viewAt(blob: Blob, offset: number, length: number) {
  return new DataView(await blob.slice(offset, offset + length).arrayBuffer());
}

function pcmReader(encoding: number, bits: number) {
  if (encoding === 1) {
    if (bits === 8) return (view: DataView, offset: number) => (view.getUint8(offset) - 128) / 128;
    if (bits === 16) return signed16;
    if (bits === 24) return (view: DataView, offset: number) =>
      ((view.getInt8(offset + 2) << 16) | view.getUint16(offset, true)) / 8388608;
    if (bits === 32) return (view: DataView, offset: number) => view.getInt32(offset, true) / 2147483648;
  }
  if (encoding === 3 && bits === 32) return (view: DataView, offset: number) => {
    const value = view.getFloat32(offset, true);
    return Number.isFinite(value) ? Math.max(-1, Math.min(1, value)) : 0;
  };
  throw Error('Expected uncompressed PCM or float32 WAV audio');
}

async function readWave(blob: Blob): Promise<PcmSource> {
  const header = await viewAt(blob, 0, 12);
  if (header.byteLength !== 12 || header.getUint32(0) !== 0x52494646 || header.getUint32(8) !== 0x57415645)
    throw Error('Invalid RIFF/WAVE header');
  const end = header.getUint32(4, true) + 8;
  if (end > blob.size) throw Error('Truncated WAV file');

  let format: Omit<PcmSource, 'offset' | 'length'> | undefined;
  let samples: Pick<PcmSource, 'offset' | 'length'> | undefined;
  for (let pos = 12, chunks = 0; pos + 8 <= end && chunks < 4096; chunks++) {
    const chunk = await viewAt(blob, pos, 8), kind = chunk.getUint32(0), size = chunk.getUint32(4, true);
    const offset = pos + 8;
    if (offset + size > end) throw Error('Truncated WAV chunk');
    if (kind === 0x666d7420 && !format) { // fmt
      if (size < 16) throw Error('Invalid WAV format chunk');
      const fields = await viewAt(blob, offset, Math.min(40, size));
      let encoding = fields.getUint16(0, true);
      const channels = fields.getUint16(2, true), sampleRate = fields.getUint32(4, true);
      const bits = fields.getUint16(14, true), bytesPerSample = bits / 8;
      if (encoding === 0xfffe && size >= 40 && fields.getUint16(16, true) >= 22 &&
          fields.getUint16(26, true) === 0 && fields.getUint32(28) === 0x00001000 &&
          fields.getUint32(32) === 0x800000aa && fields.getUint32(36) === 0x00389b71) {
        encoding = fields.getUint16(24, true);
      }
      if ((channels !== 1 && channels !== 2) || !sampleRate || fields.getUint16(12, true) !== channels * bytesPerSample)
        throw Error('Invalid WAV channels, sample rate or block alignment');
      format = {channels, sampleRate, bytesPerSample, readSample: pcmReader(encoding, bits)};
    } else if (kind === 0x64617461 && !samples) { // data
      samples = {offset, length: size};
    }
    if (format && samples) return {...format, ...samples};
    pos = offset + size + (size & 1);
  }
  throw Error('WAV is missing its format or audio data');
}

export async function encodeRetailVorbis(input: Blob, rawPcm: boolean, encoder: WasmMediaEncoder<'audio/ogg'>) {
  const {channels, sampleRate, offset, length, bytesPerSample, readSample}: PcmSource = rawPcm
    ? {channels: 2, sampleRate: 44100, offset: 0, length: input.size, bytesPerSample: 2, readSample: signed16}
    : await readWave(input);
  const frameSize = channels * bytesPerSample;
  if (!length || length % frameSize) throw Error('Invalid PCM sample count');

  // Keep the original sample rate, frame count and FFmpeg's Vorbis q5 setting.
  encoder.configure({channels, sampleRate, vbrQuality: 5, oggSerialNo: 1});
  const framesPerBlock = 65536, blockSize = framesPerBlock * frameSize;
  const planes = Array.from({length: channels}, () => new Float32Array(framesPerBlock));
  const pages: ArrayBuffer[] = [];
  function append(bytes: Uint8Array) {
    // The next call reuses the encoder's output buffer.
    if (bytes.length) pages.push(bytes.slice().buffer);
  }
  for (let pos = 0; pos < length; pos += blockSize) {
    const block = await viewAt(input, offset + pos, Math.min(blockSize, length - pos));
    const frames = block.byteLength / frameSize;
    for (let channel = 0; channel < channels; channel++) {
      const plane = planes[channel];
      for (let frame = 0, at = channel * bytesPerSample; frame < frames; frame++, at += frameSize) {
        plane[frame] = readSample(block, at);
      }
    }
    append(encoder.encode(frames === framesPerBlock ? planes : planes.map(plane => plane.subarray(0, frames))));
  }
  append(encoder.finalize());
  if (!pages.length) throw Error('Local Vorbis encoder produced no audio');
  return new Blob(pages, {type: 'audio/ogg'});
}
