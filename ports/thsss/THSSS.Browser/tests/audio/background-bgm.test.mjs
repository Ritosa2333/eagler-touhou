import assert from "node:assert/strict";
import test from "node:test";

import { createAudio } from "../../wwwroot/audio.js";

function stubAudioContext() {
  const nodes = [];
  class FakeBufferSource {
    constructor() {
      this.buffer = null;
      this.loop = false;
      this.loopStart = 0;
      this.loopEnd = 0;
      this.started = false;
      this.stopped = false;
      nodes.push(this);
    }
    connect() { return this; }
    disconnect() {}
    start() { this.started = true; }
    stop() { this.stopped = true; }
  }
  class FakeGain {
    constructor() { this.gain = { value: 1 }; }
    connect() { return this; }
    disconnect() {}
  }
  class FakeFilter {
    constructor() {
      this.type = "highpass";
      this.frequency = { value: 10 };
      this.Q = { value: 0.5 };
    }
    connect() { return this; }
    disconnect() {}
  }
  class FakeContext {
    constructor() {
      this.state = "running";
      this.currentTime = 0;
      this.sampleRate = 44100;
      this.destination = {};
    }
    createBufferSource() { return new FakeBufferSource(); }
    createGain() { return new FakeGain(); }
    createBiquadFilter() { return new FakeFilter(); }
    resume() { this.state = "running"; return Promise.resolve(); }
    decodeAudioData(data) {
      return Promise.resolve({
        duration: 2,
        numberOfChannels: 2,
        sampleRate: 44100,
        length: 88200,
        _bytes: data,
      });
    }
  }
  globalThis.window = globalThis;
  globalThis.AudioContext = FakeContext;
  globalThis.document = globalThis.document || { hidden: false, addEventListener() {}, baseURI: "http://local/" };
  globalThis.location = globalThis.location || { search: "" };
  return nodes;
}

test("hiding the page pauses BGM and restores the same voice on show", async () => {
  const nodes = stubAudioContext();
  const files = new Map();
  const ogg = Uint8Array.from([0x4f, 0x67, 0x67, 0x53, 1, 2, 3, 4]);
  files.set("/BGM/OP.ogg", ogg);
  globalThis.__thsssFiles = files;

  const audio = createAudio();
  const bufferId = audio.prepare("./BGM/OP.ogg");
  await new Promise((resolve) => setTimeout(resolve, 0));
  const voiceId = audio.createVoice();
  audio.submit(voiceId, bufferId, 0, 0, 1, 0, 0);
  audio.start(voiceId);
  assert.equal(nodes.filter((node) => node.started && !node.stopped).length, 1);

  audio.setPageHidden(true);
  assert.equal(globalThis.__thsssAudio.bgmPaused, true);
  assert.equal(nodes.every((node) => !node.started || node.stopped), true);

  audio.setPageHidden(false);
  assert.equal(globalThis.__thsssAudio.bgmPaused, false);
  assert.ok(nodes.some((node) => node.started && !node.stopped));
});

test("page-hidden start keeps music wanted without starting a node", async () => {
  stubAudioContext();
  const files = new Map();
  files.set("/BGM/OP.ogg", Uint8Array.from([0x4f, 0x67, 0x67, 0x53, 9, 8, 7, 6]));
  globalThis.__thsssFiles = files;
  const audio = createAudio();
  const bufferId = audio.prepare("./BGM/OP.ogg");
  await new Promise((resolve) => setTimeout(resolve, 0));
  audio.setPageHidden(true);
  const voiceId = audio.createVoice();
  audio.submit(voiceId, bufferId, 0, 0, 1, 0, 0);
  audio.start(voiceId);
  assert.equal(audio.samplesPlayed(voiceId), 0);
  audio.setPageHidden(false);
  assert.ok(audio.samplesPlayed(voiceId) >= 0);
});
