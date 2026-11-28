'use strict';
// Node-only tests for the OGG-tolerant StageMusic candidate. No audio device, no DOM,
// no real network: fetch is stubbed here and decodeAudioData is an explicit context seam.
import test, { after, beforeEach } from 'node:test';
import assert from 'node:assert/strict';
import { StageMusic } from '../../site/runtime/sound.mjs';

const SHA = 'a'.repeat(64);
const bytes = (tag) => new TextEncoder().encode(tag).buffer;
const resolveUrl = (base, file) => `${base}/${file}`;

const realFetch = globalThis.fetch;
const fetchState = { log: [], response: { bytes: bytes('OGG-FILE'), ok: true }, beforeResolve: null };
globalThis.fetch = async (url, { signal } = {}) => {
  fetchState.log.push(url);
  if (fetchState.beforeResolve) fetchState.beforeResolve(url, signal);
  const payload = fetchState.response.bytes;
  return { ok: fetchState.response.ok, async arrayBuffer() { return payload; } };
};
after(() => { globalThis.fetch = realFetch; });
beforeEach(() => {
  fetchState.log.length = 0;
  fetchState.response = { bytes: bytes('OGG-FILE'), ok: true };
  fetchState.beforeResolve = null;
});

function fakeContext({ duration = 300, decodeError = null } = {}) {
  const decoded = [];
  const sources = [];
  const gainParam = {
    value: 1,
    cancelScheduledValues() {},
    setValueAtTime(value) { this.value = value; },
    exponentialRampToValueAtTime() {},
  };
  return {
    currentTime: 0,
    state: 'running',
    decoded,
    sources,
    createGain() { return { gain: gainParam, connect() {}, disconnect() {} }; },
    createBufferSource() {
      const source = {
        buffer: null, loop: false, loopStart: 0, loopEnd: 0, onended: null,
        connect() {}, disconnect() {}, start() { this.started = true; }, stop() { this.stopped = true; },
      };
      sources.push(source);
      return source;
    },
    async decodeAudioData(arrayBuffer) {
      decoded.push(new Uint8Array(arrayBuffer));
      if (decodeError) throw decodeError;
      return { duration, sampleRate: 44100, numberOfChannels: 2 };
    },
  };
}

function track(name, mode, overrides = {}) {
  const base = {
    name,
    file: mode === 'ogg' ? `music/ogg/${name.replace(/\.wav$/, '.ogg')}` : `music/${name}`,
    loopStart: 1.5,
    loopEnd: 42.25,
    sampleRate: 44100,
    channels: 2,
  };
  if (mode === 'ogg') Object.assign(base, { codec: 'vorbis', bytes: 1234, sha256: SHA });
  return { ...base, ...overrides };
}

function owner(options) {
  const context = fakeContext(options);
  const gain = { connect() {}, disconnect() {} };
  return { context, music: new StageMusic(context, gain) };
}

const ok = () => ({ signal: new AbortController().signal });

test('legacy WAV mapping still loads and keeps the logical name', async () => {
  const { music, context } = owner();
  await music.load([track('th128_00.wav', 'wav')], 'https://host/assets', resolveUrl, { ...ok(), tracks: ['th128_00.wav'] });
  assert.deepEqual(fetchState.log, ['https://host/assets/music/th128_00.wav']);
  assert.equal(music.snapshot().track, 'th128_00.wav');
  assert.equal(music.track.file, 'music/th128_00.wav');
  assert.equal(context.decoded.length, 1);
  music.start();
  assert.equal(context.sources[0].loop, true);
  assert.equal(context.sources[0].loopStart, 1.5);
  assert.equal(context.sources[0].loopEnd, 42.25);
});

test('OGG file maps from the logical name and keeps the loop contract', async () => {
  const { music, context } = owner();
  await music.load([track('th128_00.wav', 'ogg')], 'https://host/assets', resolveUrl, { ...ok(), tracks: ['th128_00.wav'] });
  assert.deepEqual(fetchState.log, ['https://host/assets/music/ogg/th128_00.ogg']);
  assert.equal(music.snapshot().track, 'th128_00.wav');
  assert.equal(music.track.file, 'music/ogg/th128_00.ogg');
  assert.equal(music.track.loopStart, 1.5);
  assert.equal(music.track.loopEnd, 42.25);
  music.start();
  assert.equal(context.sources[0].loopEnd, 42.25);
});

test('decode seam receives the actual fetched OGG bytes', async () => {
  const payload = bytes('OggS-real-bytes');
  fetchState.response = { bytes: payload, ok: true };
  const { music, context } = owner();
  await music.load([track('th128_01.wav', 'ogg')], 'https://host/assets', resolveUrl, { ...ok(), tracks: ['th128_01.wav'] });
  assert.deepEqual([...context.decoded[0]], [...new Uint8Array(payload)]);
  assert.deepEqual(fetchState.log, ['https://host/assets/music/ogg/th128_01.ogg']);
});

test('wrong extension is rejected', async () => {
  const { music } = owner();
  await assert.rejects(
    music.load([track('th128_00.wav', 'wav', { file: 'music/th128_00.mp3' })], 'b', resolveUrl, { ...ok(), tracks: ['th128_00.wav'] }),
    /Invalid original music loop metadata/,
  );
});

test('OGG basename must match the logical track name', async () => {
  const { music } = owner();
  await assert.rejects(
    music.load([track('th128_00.wav', 'wav', { file: 'music/ogg/th128_01.ogg', codec: 'vorbis', bytes: 10, sha256: SHA })], 'b', resolveUrl, { ...ok(), tracks: ['th128_00.wav'] }),
    /Invalid original music loop metadata/,
  );
});

test('cross-source and escaping paths are rejected', async () => {
  const cases = [
    'music/ogg/../th128_00.ogg',
    '/music/ogg/th128_00.ogg',
    'https://evil.example/music/ogg/th128_00.ogg',
    'music/ogg/sub/th128_00.ogg',
    'music/th128_00.ogg',
  ];
  for (const file of cases) {
    const { music } = owner();
    await assert.rejects(
      music.load([track('th128_00.wav', 'wav', { file, codec: 'vorbis', bytes: 10, sha256: SHA })], 'b', resolveUrl, { ...ok(), tracks: ['th128_00.wav'] }),
      /Invalid original music loop metadata/,
      file,
    );
  }
});

test('OGG receipt without bytes/sha256/codec is rejected', async () => {
  const missing = [
    { sha256: undefined },
    { bytes: undefined },
    { codec: 'mp3' },
    { sha256: 'not-a-sha' },
    { bytes: 0 },
  ];
  for (const overrides of missing) {
    const { music } = owner();
    await assert.rejects(
      music.load([track('th128_00.wav', 'ogg', overrides)], 'b', resolveUrl, { ...ok(), tracks: ['th128_00.wav'] }),
      /Invalid original OGG music file receipt/,
      JSON.stringify(overrides),
    );
  }
});

test('loop metadata contract is unchanged', async () => {
  const bad = [
    { loopStart: -1 },
    { loopEnd: 1.5 },
    { loopEnd: 601 },
    { sampleRate: 48000 },
    { channels: 1 },
  ];
  for (const overrides of bad) {
    const { music } = owner();
    await assert.rejects(
      music.load([track('th128_00.wav', 'ogg', overrides)], 'b', resolveUrl, { ...ok(), tracks: ['th128_00.wav'] }),
      /Invalid original music loop metadata/,
      JSON.stringify(overrides),
    );
  }
});

test('loopEnd beyond decoded duration is rejected', async () => {
  const { music } = owner({ duration: 10 });
  await assert.rejects(
    music.load([track('th128_00.wav', 'ogg')], 'b', resolveUrl, { ...ok(), tracks: ['th128_00.wav'] }),
    /Original music loop exceeds decoded PCM/,
  );
});

test('pre-aborted signal rejects before any fetch', async () => {
  const { music } = owner();
  const controller = new AbortController();
  controller.abort();
  await assert.rejects(
    music.load([track('th128_00.wav', 'ogg')], 'b', resolveUrl, { signal: controller.signal, tracks: ['th128_00.wav'] }),
    (error) => error.name === 'AbortError',
  );
  assert.deepEqual(fetchState.log, []);
});

test('abort during decode rejects with AbortError and leaves the owner unloaded', async () => {
  const { music } = owner();
  const controller = new AbortController();
  fetchState.beforeResolve = () => controller.abort();
  await assert.rejects(
    music.load([track('th128_00.wav', 'ogg')], 'b', resolveUrl, { signal: controller.signal, tracks: ['th128_00.wav'] }),
    (error) => error.name === 'AbortError',
  );
  assert.equal(music.snapshot().playing, false);
  assert.equal(music.track, null);
});

test('owner lifecycle: start/switchTo/dispose keep their contracts', async () => {
  const { music, context } = owner();
  const tracks = [track('th128_00.wav', 'ogg'), track('th128_02.wav', 'ogg')];
  await music.load(tracks, 'b', resolveUrl, { ...ok(), tracks: ['th128_00.wav', 'th128_02.wav'] });
  music.start();
  assert.equal(context.sources.filter((source) => source.started).length, 1);
  music.switchTo('th128_02.wav');
  assert.equal(music.snapshot().track, 'th128_02.wav');
  assert.equal(context.sources.at(-1).loopStart, 1.5);
  assert.equal(context.sources[0].stopped, true);
  music.dispose();
  assert.equal(music.snapshot().playing, false);
  assert.throws(() => music.start(), /Stage music is disposed/);
  assert.throws(() => music.switchTo('th128_00.wav'), /Stage music is disposed/);
  await assert.rejects(
    music.load(tracks, 'b', resolveUrl, { ...ok(), tracks: ['th128_00.wav'] }),
    (error) => error.name === 'AbortError',
  );
});

test('preload set validation is unchanged', async () => {
  const { music } = owner();
  await assert.rejects(
    music.load([], 'b', resolveUrl, { ...ok(), tracks: [] }),
    /Invalid original track preload set/,
  );
  await assert.rejects(
    music.load([], 'b', resolveUrl, { ...ok(), tracks: ['th128_00.wav', 'th128_00.wav'] }),
    /Invalid original track preload set/,
  );
});

test('duplicate or missing metadata rows are rejected', async () => {
  const { music } = owner();
  await assert.rejects(
    music.load([track('th128_00.wav', 'ogg'), track('th128_00.wav', 'ogg')], 'b', resolveUrl, { ...ok(), tracks: ['th128_00.wav'] }),
    /Original stage music metadata is unavailable/,
  );
  const { music: music2 } = owner();
  await assert.rejects(
    music2.load([track('th128_01.wav', 'ogg')], 'b', resolveUrl, { ...ok(), tracks: ['th128_00.wav'] }),
    /Original stage music metadata is unavailable/,
  );
});
