import test, {before, after} from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {fileURLToPath} from 'node:url';
import {build} from 'esbuild';
import {launchBrowser} from '../../../tools/browser/launch.mjs';

const source = await readFile(new URL('../../../common/include/eagler/audio/WebSfx.hpp', import.meta.url), 'utf8');
const begin = '// BEGIN_WEB_SFX_JS', end = '// END_WEB_SFX_JS';
assert.equal(source.split(begin).length, 2);
assert.equal(source.split(end).length, 2);
const body = source.split(begin)[1].split(end)[0];
let browser;
before(async () => {
  browser = await launchBrowser({args: ['--enable-unsafe-swiftshader', '--autoplay-policy=no-user-gesture-required']});
});
after(async () => {await browser?.close();});

test('audible room music mutes game BGM, preserves SFX, and restores the current BGM level', {timeout: 30000}, async () => {
  const launcher = new URL('../../', import.meta.url);
  const {outputFiles: [bundle]} = await build({
    stdin: {
      contents: `export {createRuntimeAudioMix} from './app/services/runtime-audio-mix.ts';
        export {createRoomMusic} from './app/services/room-music.ts';`,
      resolveDir: fileURLToPath(launcher),
    },
    bundle: true,
    format: 'iife',
    globalName: 'AudioTest',
    write: false,
  });
  const page = await browser.newPage();
  try {
    await page.addScriptTag({content: bundle.text});
    const result = await page.evaluate(async body => {
      const rate = 48000, frames = 2048;
      const ctx = new OfflineAudioContext(2, frames, rate);
      const music = ctx.createBuffer(2, frames, rate);
      const left = music.getChannelData(0);
      // The ramp exposes a restarted BGM cursor after restoring the gain.
      for (let i = 0; i < frames; i++) left[i] = .125 + i / (4 * frames);
      const track = ctx.createBufferSource();
      track.buffer = music;
      track.connect(ctx.destination);
      track.start();

      const effects = new Float32Array(frames * 2);
      for (let i = 0; i < frames; i++) effects[2 * i + 1] = .25;
      const Module = {SDL3: {audioContext: ctx, audio_playback: {scriptProcessorNode: track}}};
      const command = new Function('Module', 'HEAPU8', 'op', 'id', 'pcm', 'frames', 'channels', 'rate', 'format', 'value', body);
      const call = op => command(Module, new Uint8Array(effects.buffer), op, 1, 0, frames, 2, rate, 0, 0);
      const uploaded = call(0), started = call(2);

      const mix = AudioTest.createRuntimeAudioMix();
      let configured = .6, audible = false;
      const apply = () => mix.set(audible ? 0 : configured, .8);
      mix.attach({Module}, 'th10');
      apply();
      // Play a generated WAV through a real media element, with no provider or game files.
      const samples = new Int16Array(5 * 8000).map((_, i) => i % 32 < 16 ? 4000 : -4000);
      const header = new DataView(new ArrayBuffer(44));
      const tag = (offset, value) => new Uint8Array(header.buffer).set(new TextEncoder().encode(value), offset);
      tag(0, 'RIFF'); tag(8, 'WAVEfmt '); tag(36, 'data');
      header.setUint32(4, 36 + samples.byteLength, true);
      header.setUint32(16, 16, true);
      header.setUint16(20, 1, true);
      header.setUint16(22, 1, true);
      header.setUint32(24, 8000, true);
      header.setUint32(28, 16000, true);
      header.setUint16(32, 2, true);
      header.setUint16(34, 16, true);
      header.setUint32(40, samples.byteLength, true);
      const url = URL.createObjectURL(new Blob([header.buffer, samples], {type: 'audio/wav'}));
      const song = {platform: 'netease', id: 'test-tone', name: 'Test tone', artist: ''};
      let notify;
      const room = AudioTest.createRoomMusic({
        provider: {stream: async () => [url], track: async () => song},
        createAudio: () => new Audio(), storage: null, send: () => false,
        onAudibleChange(value) {audible = value; apply(); notify?.(value);},
      });
      function changePlayback(expected, action) {
        return new Promise((resolve, reject) => {
          const timeout = setTimeout(() => reject(Error(`Room music did not become ${expected ? 'audible' : 'quiet'}`)), 5000);
          notify = value => {
            if (value !== expected) return;
            clearTimeout(timeout);
            notify = undefined;
            resolve();
          };
          action();
        });
      }
      try {
        room.update({product: 'th10', room: {code: 'test', seats: []}, seat: 0});
        const playAt = ctx.suspend(512 / rate), adjustAt = ctx.suspend(1024 / rate), pauseAt = ctx.suspend(1536 / rate);
        const rendering = ctx.startRendering();
        await playAt;
        await changePlayback(true, () => room.choose(song));
        await ctx.resume();
        await adjustAt;
        configured = .24;
        apply();
        await ctx.resume();
        await pauseAt;
        await changePlayback(false, () => room.togglePlayback());
        await ctx.resume();
        const rendered = await rendering;
        const musicPcm = rendered.getChannelData(0), sfxPcm = rendered.getChannelData(1);
        const windows = [[0, 512, .6], [512, 1024, 0], [1024, 1536, 0], [1536, frames, .24]];
        return {uploaded, started, windows: windows.map(([start, end, gain]) => {
          let musicError = 0, sfxError = 0;
          for (let i = start; i < end; i++) {
            musicError = Math.max(musicError, Math.abs(musicPcm[i] - left[i] * gain));
            sfxError = Math.max(sfxError, Math.abs(sfxPcm[i] - .2));
          }
          return {start, end, musicError, sfxError};
        })};
      } finally {
        room.dispose();
        URL.revokeObjectURL(url);
      }
    }, body);
    assert.equal(result.uploaded, 1);
    assert.equal(result.started, 1);
    for (const {start, end, musicError, sfxError} of result.windows) {
      assert.ok(musicError < 1e-6, `BGM PCM ${start}..${end} differs by ${musicError}`);
      assert.ok(sfxError < 1e-6, `SFX PCM ${start}..${end} differs by ${sfxError}`);
    }
  } finally {await page.close();}
});
