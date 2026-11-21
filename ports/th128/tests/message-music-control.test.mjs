import assert from 'node:assert/strict';
import test from 'node:test';
import {StageMusic} from '../site/runtime/sound.mjs';

function fakeParam() {
  const param = {value: 1, events: [],
    cancelScheduledValues(time) { this.events.push(['cancel', time]); },
    setValueAtTime(value, time) { this.value = value; this.events.push(['set', value, time]); },
    exponentialRampToValueAtTime(value, time) {
      this.value = value;
      this.events.push(['exponential', value, time]);
    }};
  return param;
}

function fakeContext() {
  const gains = [], sources = [];
  const context = {
    currentTime: 10,
    createGain() {
      const node = {gain: fakeParam(), connect(target) { this.target = target; },
        disconnect() { this.disconnected = true; }};
      gains.push(node);
      return node;
    },
    createBufferSource() {
      const source = {connect(target) { this.target = target; },
        disconnect() { this.disconnected = true; },
        start() { this.started = true; },
        stop(time) { this.stopTime = time; this.stopped = true; }};
      sources.push(source);
      return source;
    },
  };
  return {context, gains, sources};
}

function loadedMusic(context, master, name = 'a') {
  const music = new StageMusic(context, master);
  const track = {name, loopStart: 1, loopEnd: 10, sampleRate: 44100, channels: 2};
  const buffer = {duration: 12};
  music.track = track;
  music.buffer = buffer;
  music.preloaded.set(name, {track, buffer});
  return music;
}

test('original BGM fade-out uses a separate music gain, audio clock and lifecycle controls', () => {
  const {context, gains, sources} = fakeContext();
  const master = {gain: {value: 0.25}};
  const music = loadedMusic(context, master);
  const output = gains[0];
  assert.equal(output.target, master);
  music.start();
  const first = sources[0];
  assert.equal(first.target, output);
  assert.equal(first.started, true);
  assert.equal(master.gain.value, 0.25);

  const fade = music.fadeOutFrames(480);
  assert.equal(fade.seconds, 8);
  assert.equal(fade.start, 10);
  assert.equal(fade.end, 18);
  assert.equal(fade.immediate, false);
  assert.ok(Math.abs(fade.target - Math.pow(10, -2.5)) < 1e-12);
  assert.deepEqual(output.gain.events.at(-1), ['exponential', fade.target, 18]);
  assert.equal(first.stopTime, 18);
  assert.equal(master.gain.value, 0.25);
  assert.equal(music.fade.source, first);

  first.onended();
  assert.equal(music.source, null);
  assert.equal(music.fade, null);
  assert.equal(music.snapshot().playing, false);

  context.currentTime = 25;
  const next = loadedMusic(context, master, 'b');
  const nextOutput = gains.at(-1);
  next.start();
  const current = sources.at(-1);
  const secondFade = next.fadeOut(1);
  assert.equal(secondFade.start, 25);
  assert.equal(secondFade.end, 26);
  assert.equal(current.stopTime, 26);
  assert.equal(nextOutput.target, master);
  assert.equal(master.gain.value, 0.25);

  next.preloaded.set('c', {track: {name: 'c', loopStart: 0, loopEnd: 1, sampleRate: 44100, channels: 2},
    buffer: {duration: 2}});
  next.switchTo('c');
  assert.equal(next.fade, null);
  assert.equal(nextOutput.gain.value, 1);
  assert.equal(current.disconnected, true);
  assert.equal(sources.at(-1).target, nextOutput);
  assert.equal(next.snapshot().track, 'c');

  assert.throws(() => next.fadeOut(NaN));
  assert.throws(() => next.fadeOut(-1));
  const playing = next.source;
  const immediate = next.fadeOut(0);
  assert.equal(immediate.immediate, true);
  assert.equal(playing.stopped, true);
  assert.equal(nextOutput.gain.value, 1);

  next.dispose();
  assert.equal(nextOutput.disconnected, true);
  next.dispose();
  assert.throws(() => next.fadeOut(1));
});
