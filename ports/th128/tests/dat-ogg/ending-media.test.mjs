'use strict';
// Node-only tests for the OGG-capable ending media candidate.
// No browser, no WebAudio device, no World, no native code: the decode seam and the
// IndexedDB receipts are explicit fakes.
import test from 'node:test';
import assert from 'node:assert/strict';
import { createHash } from 'node:crypto';
import { OrdinaryEndingStopConsumer, createEndingBrowserHost, endingMusicPCM, endingWavPCM, endingOggPCM } from '../../site/runtime/native-ending-browser-host-v4.mjs';

const HEX = (n) => n.toString(16).padStart(2, '0').repeat(32);
const SHA = (bytes) => createHash('sha256').update(Buffer.from(bytes)).digest('hex');

function wav(frames, { loopStartBytes = 0, extra = 0, pcmBytes = null } = {}) {
  const data = new Uint8Array(frames * 4 + extra);
  for (let i = 0; i < frames; i++) {
    new DataView(data.buffer).setInt16(i * 4, 1000 + i, true);
    new DataView(data.buffer).setInt16(i * 4 + 2, -1000 - i, true);
  }
  const buffer = new Uint8Array(44 + data.length);
  const view = new DataView(buffer.buffer);
  const text = (offset, value) => { for (let i = 0; i < 4; i++) buffer[offset + i] = value.charCodeAt(i); };
  text(0, 'RIFF'); view.setUint32(4, 36 + data.length, true); text(8, 'WAVE');
  text(12, 'fmt '); view.setUint32(16, 16, true); view.setUint16(20, 1, true); view.setUint16(22, 2, true);
  view.setUint32(24, 44100, true); view.setUint32(28, 176400, true); view.setUint16(32, 4, true); view.setUint16(34, 16, true);
  text(36, 'data'); view.setUint32(40, data.length, true); buffer.set(data, 44);
  return { buffer, data, metadata: { pcmBytes: pcmBytes ?? data.length, loopStartBytes } };
}

function decoded(frames, values = { left: 0.5, right: -0.5 }) {
  const left = new Float32Array(frames).fill(values.left);
  const right = new Float32Array(frames).fill(values.right);
  return { sampleRate: 44100, numberOfChannels: 2, length: frames, getChannelData: (channel) => (channel === 0 ? left : right) };
}

test('legacy WAV path keeps the strict RIFF/PCM contract', async () => {
  const { buffer, data, metadata } = wav(100, { loopStartBytes: 40 });
  const pcm = await endingMusicPCM(buffer, metadata);
  assert.equal(pcm.audioFormat, 'pcm-s16le');
  assert.equal(pcm.frames, 100);
  assert.equal(pcm.loopStart, 10);
  assert.equal(pcm.offset, 0);
  assert.deepEqual([...pcm.bytes], [...data]);
  assert.equal(pcm.view.getInt16(5 * 4, true), 1005);
  assert.equal(pcm.view.getInt16(5 * 4 + 2, true), -1005);
});

test('WAV metadata mismatches are still rejected', async () => {
  const { buffer } = wav(100, { loopStartBytes: 40 });
  await assert.rejects(endingMusicPCM(buffer, { pcmBytes: 396, loopStartBytes: 40 }), /Original ending PCM or loop metadata rejected/);
  await assert.rejects(endingMusicPCM(buffer, { pcmBytes: 400, loopStartBytes: 41 }), /Original ending PCM or loop metadata rejected/);
  await assert.rejects(endingMusicPCM(buffer, { pcmBytes: 400, loopStartBytes: 400 }), /Original ending PCM or loop metadata rejected/);
  const truncated = buffer.slice(0, 60);
  await assert.rejects(endingMusicPCM(truncated, { pcmBytes: 400, loopStartBytes: 0 }), /Truncated original ending WAV|Original ending WAV identity rejected/);
});

test('OGG branch decodes, converts to the logical int16 stream, and keeps loop/format', async () => {
  const ogg = new Uint8Array([0x4f,0x67,0x67,0x53,0,0,0,0,0,0,0,0]);
  const pcm = await endingOggPCM(ogg, { audioFormat: 'vorbis-ogg', decodedFrames: 500, loopStartSamples: 100, pcmBytes: 2000, loopStartBytes: 400 }, async (bytes) => {
    assert.deepEqual([...bytes], [...ogg]);
    return decoded(500);
  });
  assert.equal(pcm.audioFormat, 'vorbis-ogg');
  assert.equal(pcm.frames, 500);
  assert.equal(pcm.loopStart, 100);
  assert.equal(pcm.bytes.length, 2000);
  assert.equal(pcm.view.getInt16(0, true), 16384);
  assert.equal(pcm.view.getInt16(2, true), -16384);
});

test('OGG int16 conversion clamps at the PCM limits', async () => {
  const ogg = new Uint8Array([0x4f,0x67,0x67,0x53,0,0,0,0,0,0,0,0]);
  const pcm = await endingOggPCM(ogg, { audioFormat: 'vorbis-ogg', decodedFrames: 2, loopStartSamples: 0 }, async () => decoded(2, { left: 1, right: -1 }));
  assert.equal(pcm.view.getInt16(0, true), 32767);
  assert.equal(pcm.view.getInt16(2, true), -32768);
});

test('OGG rejects unverifiable frame counts, wrong rate/channels, and bad loops', async () => {
  const ogg = new Uint8Array([0x4f,0x67,0x67,0x53,0,0,0,0,0,0,0,0]);
  const decodeOk = async () => decoded(100);
  await assert.rejects(endingOggPCM(ogg, { audioFormat: 'vorbis-ogg', loopStartSamples: 0 }, decodeOk), /decoded frame count is unverifiable/);
  await assert.rejects(endingOggPCM(ogg, { audioFormat: 'vorbis-ogg', decodedFrames: 101, loopStartSamples: 0 }, decodeOk), /decoded frame count mismatch/);
  await assert.rejects(endingOggPCM(ogg, { audioFormat: 'vorbis-ogg', decodedFrames: 100, loopStartSamples: 0 }, async () => ({ ...decoded(100), sampleRate: 48000 })), /decoded audio metadata rejected/);
  await assert.rejects(endingOggPCM(ogg, { audioFormat: 'vorbis-ogg', decodedFrames: 100, loopStartSamples: 0 }, async () => ({ ...decoded(100), numberOfChannels: 1 })), /decoded audio metadata rejected/);
  await assert.rejects(endingOggPCM(ogg, { audioFormat: 'vorbis-ogg', decodedFrames: 100, loopStartSamples: -1 }, decodeOk), /OGG loop metadata rejected/);
  await assert.rejects(endingOggPCM(ogg, { audioFormat: 'vorbis-ogg', decodedFrames: 100, loopStartSamples: 100 }, decodeOk), /OGG loop metadata rejected/);
  await assert.rejects(endingOggPCM(ogg, { audioFormat: 'vorbis-ogg', decodedFrames: 100, loopStartBytes: 401 }, decodeOk), /OGG loop metadata rejected/);
});

test('OGG branch requires an explicit decode seam and a declared format', async () => {
  const ogg = new Uint8Array([0x4f,0x67,0x67,0x53,0,0,0,0,0,0,0,0]);
  await assert.rejects(endingOggPCM(ogg, { audioFormat: 'vorbis-ogg', decodedFrames: 10, loopStartSamples: 0 }, null), /decode seam is unavailable/);
  await assert.rejects(endingOggPCM(ogg, { audioFormat: 'pcm-s16le', decodedFrames: 10, loopStartSamples: 0 }, async () => decoded(10)), /OGG source format rejected/);
});

test('dispatch: RIFF goes to WAV, anything else goes to OGG', async () => {
  const { buffer, metadata } = wav(10, { loopStartBytes: 0 });
  const wavPCM = await endingMusicPCM(buffer, metadata, null);
  assert.equal(wavPCM.audioFormat, 'pcm-s16le');
  const oggPCM = await endingMusicPCM(new Uint8Array([0x4f,0x67,0x67,0x53,0,0,0,0,0,0,0,0]), { audioFormat: 'vorbis-ogg', decodedFrames: 3, loopStartSamples: 0 }, async () => decoded(3));
  assert.equal(oggPCM.audioFormat, 'vorbis-ogg');
});

function stopFixture(identity, { afterIdentity = null, onCommit = null } = {}) {
  const packet = new Uint8Array(128);
  const view = new DataView(packet.buffer);
  view.setUint32(0, 0x3152544f, true); view.setUint32(4, 1, true); view.setUint32(8, 128, true); view.setUint32(12, 1, true);
  view.setUint32(16, 1, true); view.setUint32(24, 5, true); view.setUint32(36, 4, true);
  view.setUint32(64, 2, true); view.setUint32(68, 2, true); view.setUint32(72, 5, true); view.setUint32(76, 180, true);
  view.setUint32(112, 16, true); view.setUint32(116, 1, true); view.setUint32(120, 0x394d4350, true);
  const calls = [];
  let reads = 0;
  const module = {
    HEAPU8: new Uint8Array(4096),
    _th128_world_ending_return_packet_size: () => 128,
    _th128_world_ending_return_packet_data: () => { module.HEAPU8.set(packet, 1024); return 1024; },
  };
  const readPCM = () => {
    const source = reads++ === 0 || !afterIdentity ? identity : afterIdentity;
    return {
      bytes: new Uint8Array(735 * 4), cursor: 12, fadeRemaining: 0, fadeDuration: 180,
      stopped: true, ticks: 5, closed: false, deviceStopIssued: true, presentAudio: true, ...source,
    };
  };
  const receipts = {
    async commit(key, metadata, payload) {
      calls.push({ key, metadata, bytes: payload.length });
      if (onCommit) onCommit();
      return {
        committed: true, fullReadback: true, durability: 'strict-indexeddb-transaction', domain: metadata.domain,
        identitySHA256: metadata.identitySHA256, bytes: payload.length, sha256: SHA(payload),
      };
    },
  };
  const consumer = new OrdinaryEndingStopConsumer({ module, receipts, readPCM, isCurrent: () => true });
  return { consumer, packet: { descriptor: packet, generation: 1, logicalTick: 5, index: 4 }, calls };
}

test('stop consumer accepts the OGG identity and records the encoded source', async () => {
  const asset = HEX(1), pcm = HEX(2);
  const { consumer, packet, calls } = stopFixture({ audioFormat: 'vorbis-ogg', sourceAssetSHA256: asset, sourcePCMInt16SHA256: pcm, sourceWAVSHA256: undefined });
  const receipt = await consumer.consume(packet);
  assert.equal(receipt.committed, true);
  const metadata = calls[0].metadata;
  assert.equal(metadata.audioFormat, 'vorbis-ogg');
  assert.equal(metadata.sourceAssetSHA256, asset);
  assert.equal(metadata.sourcePCMInt16SHA256, pcm);
  assert.equal(metadata.sourceWAVSHA256, undefined);
  assert.equal(metadata.encodedSourceSHA256, asset);
  assert.equal(metadata.pcmFrames, 735);
});

test('stop consumer keeps the legacy WAV identity branch', async () => {
  const asset = HEX(3);
  const { consumer, packet, calls } = stopFixture({ audioFormat: 'pcm-s16le', sourceAssetSHA256: asset, sourcePCMInt16SHA256: HEX(4), sourceWAVSHA256: asset });
  await consumer.consume(packet);
  const metadata = calls[0].metadata;
  assert.equal(metadata.audioFormat, 'pcm-s16le');
  assert.equal(metadata.sourceWAVSHA256, asset);
  assert.equal(metadata.encodedSourceSHA256, undefined);
});

test('stop consumer rejects mixed or malformed identities', async () => {
  const asset = HEX(1);
  const bad = [
    { audioFormat: 'vorbis-ogg', sourceAssetSHA256: asset, sourcePCMInt16SHA256: HEX(2), sourceWAVSHA256: asset },
    { audioFormat: 'pcm-s16le', sourceAssetSHA256: asset, sourcePCMInt16SHA256: HEX(2), sourceWAVSHA256: HEX(9) },
    { audioFormat: 'pcm-s16le', sourceAssetSHA256: 'not-a-sha', sourcePCMInt16SHA256: HEX(2), sourceWAVSHA256: 'not-a-sha' },
    { audioFormat: 'mp3', sourceAssetSHA256: asset, sourcePCMInt16SHA256: HEX(2), sourceWAVSHA256: undefined },
  ];
  for (const identity of bad) {
    const { consumer, packet } = stopFixture(identity);
    await assert.rejects(consumer.consume(packet), /Ordinary return requires actual source.stop and exact complete final silent PCM/, JSON.stringify(identity));
  }
});

test('stop consumer still requires complete silent PCM after the stop', async () => {
  const asset = HEX(1);
  const { consumer, packet } = stopFixture({ audioFormat: 'vorbis-ogg', sourceAssetSHA256: asset, sourcePCMInt16SHA256: HEX(2), sourceWAVSHA256: undefined });
  const original = consumer.readPCM;
  consumer.readPCM = () => ({ ...original(), bytes: new Uint8Array(735 * 4).fill(1) });
  await assert.rejects(consumer.consume(packet), /Ordinary return requires actual source.stop and exact complete final silent PCM/);
});

test('OGG rejects a non-OggS container before any decode', async () => {
  let decoded = 0;
  await assert.rejects(
    endingOggPCM(new Uint8Array([1, 2, 3]), { audioFormat: 'vorbis-ogg', decodedFrames: 10, loopStartSamples: 0 }, async () => { decoded++; return decodedBuffer(10); }),
    /Original ending OGG encoded source bytes rejected/,
  );
  assert.equal(decoded, 0);
});

test('OGG rejects short channel buffers and non-finite samples instead of writing zeros', async () => {
  const ogg = new Uint8Array([0x4f,0x67,0x67,0x53,0,0,0,0,0,0,0,0]);
  const short = (frames) => ({ sampleRate: 44100, numberOfChannels: 2, length: frames, getChannelData: (channel) => (channel === 0 ? new Float32Array(frames) : new Float32Array(frames - 1)) });
  await assert.rejects(endingOggPCM(ogg, { audioFormat: 'vorbis-ogg', decodedFrames: 8, loopStartSamples: 0 }, async () => short(8)), /Original ending OGG decoded PCM samples rejected/);
  const nan = (frames) => {
    const left = new Float32Array(frames).fill(0.25); left[3] = NaN;
    return { sampleRate: 44100, numberOfChannels: 2, length: frames, getChannelData: (channel) => (channel === 0 ? left : new Float32Array(frames).fill(0.25)) };
  };
  await assert.rejects(endingOggPCM(ogg, { audioFormat: 'vorbis-ogg', decodedFrames: 8, loopStartSamples: 0 }, async () => nan(8)), /Original ending OGG decoded PCM samples rejected/);
  const infinite = (frames) => {
    const right = new Float32Array(frames).fill(0.25); right[7] = Infinity;
    return { sampleRate: 44100, numberOfChannels: 2, length: frames, getChannelData: (channel) => (channel === 0 ? new Float32Array(frames).fill(0.25) : right) };
  };
  await assert.rejects(endingOggPCM(ogg, { audioFormat: 'vorbis-ogg', decodedFrames: 8, loopStartSamples: 0 }, async () => infinite(8)), /Original ending OGG decoded PCM samples rejected/);
});

test('OGG refuses frame counts above the owned 600 second bound', async () => {
  const frames = 44100 * 600 + 1;
  const huge = { sampleRate: 44100, numberOfChannels: 2, length: frames, getChannelData: () => ({ length: frames, every: () => true }) };
  await assert.rejects(
    endingOggPCM(new Uint8Array([0x4f,0x67,0x67,0x53,0,0,0,0,0,0,0,0]), { audioFormat: 'vorbis-ogg', decodedFrames: frames, loopStartSamples: 0 }, async () => huge),
    /Original ending OGG decoded frame count exceeds the owned bound/,
  );
});

test('stop consumer rejects an identity switch between validation and after-readback', async () => {
  const asset = HEX(1);
  const { consumer, packet } = stopFixture(
    { audioFormat: 'vorbis-ogg', sourceAssetSHA256: asset, sourcePCMInt16SHA256: HEX(2), sourceWAVSHA256: undefined },
    { afterIdentity: { audioFormat: 'pcm-s16le', sourceAssetSHA256: asset, sourcePCMInt16SHA256: HEX(2), sourceWAVSHA256: asset } },
  );
  await assert.rejects(consumer.consume(packet), /Ordinary ending stopped PCM complete durable receipt rejected/);
});

test('stop consumer keeps the legacy WAV readPCM shape without fabricating a PCM hash', async () => {
  const asset = HEX(3);
  const { consumer, packet, calls } = stopFixture({ sourceWAVSHA256: asset });
  const receipt = await consumer.consume(packet);
  assert.equal(receipt.committed, true);
  assert.equal(receipt.audioFormat, 'pcm-s16le');
  assert.equal(receipt.sourceAssetSHA256, asset);
  const metadata = calls[0].metadata;
  assert.equal(metadata.audioFormat, 'pcm-s16le');
  assert.equal(metadata.sourceWAVSHA256, asset);
  assert.equal(metadata.sourcePCMInt16SHA256, undefined);
  assert.equal(metadata.encodedSourceSHA256, undefined);
});

test('stop consumer rejects a legacy shape that also carries a fabricated PCM hash', async () => {
  const { consumer, packet } = stopFixture({ sourceWAVSHA256: HEX(3), sourcePCMInt16SHA256: HEX(4) });
  await assert.rejects(consumer.consume(packet), /Ordinary return requires actual source.stop and exact complete final silent PCM/);
});

test('host: a decode that completes after the owner is gone opens no receipt store and creates no gain', async () => {
  const state = { current: true };
  const counts = { opens: 0, closes: 0, gains: 0, connects: 0 };
  let release;
  const pending = new Promise((resolve) => { release = resolve; });
  installIndexedDB(state, counts);
  const deps = hostDependencies(state, counts);
  let signalEntered;const entered=new Promise(resolve=>{signalEntered=resolve;});
  const host = createEndingBrowserHost({ ...deps, decodeMusic: () => {signalEntered();return pending;} });
  await entered;
  state.current = false;
  release(decodedBuffer(256));
  await assert.rejects(host, /Original ending browser generation changed during music decode/);
  assert.equal(counts.opens, 0);
  assert.equal(counts.gains, 0);
  assert.equal(counts.connects, 0);
});

test('host: an owner that dies while the receipt store opens is rejected and the store is closed', async () => {
  const state = { current: true };
  const counts = { opens: 0, closes: 0, gains: 0, connects: 0 };
  installIndexedDB(state, counts, () => { state.current = false; });
  const deps = hostDependencies(state, counts);
  await assert.rejects(
    createEndingBrowserHost({ ...deps, decodeMusic: async () => decodedBuffer(256) }),
    /Original ending browser generation changed while opening the strict ending receipt store/,
  );
  assert.equal(counts.opens, 1);
  assert.equal(counts.closes, 1);
  assert.equal(counts.gains, 0);
  assert.equal(counts.connects, 0);
});

function decodedBuffer(frames) {
  const left = new Float32Array(frames).fill(0.5), right = new Float32Array(frames).fill(-0.5);
  return { sampleRate: 44100, numberOfChannels: 2, length: frames, getChannelData: (channel) => (channel === 0 ? left : right) };
}

function installIndexedDB(state, counts, onOpen = null) {
  globalThis.indexedDB = {
    open(name) {
      counts.opens++;
      const request = {
        result: {
          name,
          createObjectStore() { return {}; },
          close() { counts.closes++; },
          transaction() { return { durability: 'strict', objectStore() { return { add() {}, get() {} }; }, abort() {}, set oncomplete(_) {}, set onabort(_) {}, set onerror(_) {} }; },
        },
      };
      if (onOpen) onOpen();
      setImmediate(() => { request.onupgradeneeded?.(); request.onsuccess?.(); });
      return request;
    },
  };
}

function hostDependencies(state, counts) {
  const bytes = new Uint8Array([0x4f,0x67,0x67,0x53,0,0,0,0,0,0,0,0]);
  const crypto = createHash('sha256').update(Buffer.from(bytes)).digest('hex');
  const audio = {
    state: 'running',
    decodeAudioData: async () => { throw new Error('the injected seam must be used'); },
    createGain() {
      counts.gains++;
      return { gain: { value: 0, setValueAtTime() {} }, connect() { counts.connects++; }, disconnect() { counts.disconnects = (counts.disconnects ?? 0) + 1; } };
    },
  };
  return {
    module: {},
    renderer: { renderEndingPixels() {}, releaseEndingPixels() {}, renderOriginalTerminalFade() {} },
    resources: { music: { asset: { file: 'music/ogg/th128_09.ogg', bytes: bytes.length, sha256: crypto }, metadata: { audioFormat: 'vorbis-ogg', decodedFrames: 256, loopStartSamples: 0 } } },
    readAsset: async () => bytes,
    audio,
    gain: {},
    soundQueue: { consumeNativeRequest() { return null; }, flush() {}, assertIdentityHost() {} },
    legacyMusic: { stopSource() {} },
    isCurrent: () => state.current,
  };
}

test('host closes newly opened receipts and output when required native constructor APIs are absent',async()=>{
 const state={current:true},counts={opens:0,closes:0,gains:0,connects:0};
 installIndexedDB(state,counts);
 await assert.rejects(createEndingBrowserHost({...hostDependencies(state,counts),decodeMusic:async()=>decodedBuffer(256)}),/required API unavailable/);
 assert.equal(counts.opens,1);assert.equal(counts.closes,1);assert.equal(counts.gains,1);assert.equal(counts.disconnects,1);
});

test('default OGG decoding targets44100Hz even when the output device context is48000Hz',async()=>{
 const state={current:true},counts={opens:0,closes:0,gains:0,connects:0},deps=hostDependencies(state,counts);
 deps.audio.sampleRate=48000;
 const prior=globalThis.OfflineAudioContext,calls=[];
 globalThis.OfflineAudioContext=class{
  constructor(channels,length,rate){calls.push({channels,length,rate});}
  async decodeAudioData(bytes){assert.equal(bytes.byteLength,12);state.current=false;return decodedBuffer(256);}
 };
 try{
  await assert.rejects(createEndingBrowserHost(deps),/generation changed during music decode/);
  assert.deepEqual(calls,[{channels:2,length:1,rate:44100}]);assert.equal(counts.opens,0);assert.equal(counts.gains,0);
 }finally{if(prior===undefined)delete globalThis.OfflineAudioContext;else globalThis.OfflineAudioContext=prior;}
});
