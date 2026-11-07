import { expect, test, vi } from 'vitest';
import { createRuntimeAudioMix } from '../../app/services/runtime-audio-mix';

class Node {
  constructor(context) {
    this.context = context;
    this.outputs = new Set();
  }
  connect(node) { this.outputs.add(node); return node; }
  disconnect(node) {
    if (node === undefined) this.outputs.clear();
    else if (!this.outputs.delete(node)) throw new Error('unconnected');
  }
}
class Context {
  destination = new Node(this);
  state = 'running';
  createGain() { return Object.assign(new Node(this), { gain: { value: 1 } }); }
  createScriptProcessor() { return new Node(this); }
  createChannelMerger() { return new Node(this); }
  createBufferSource() { return new Node(this); }
}

test('a mixed Runtime stays audible until independent effect output is confirmed', () => {
  const context = new Context();
  const processor = context.createScriptProcessor();
  const merger = context.createChannelMerger();
  processor.connect(context.destination);
  merger.connect(context.destination);
  const runtime = { AudioContext: Context, Module: { SDL3: {
    audioContext: context, audio_playback: { scriptProcessorNode: processor },
  } } };
  const mix = createRuntimeAudioMix();
  mix.set(0, 0.7);
  mix.attach(runtime, 'th09');
  const [bgm] = processor.outputs;
  expect(bgm.gain.value).toBe(1);

  runtime.Module.eaglerSfxOutput = { context, voices: new Map([[0, { merge: merger }]]) };
  mix.refresh();
  const [sfx] = merger.outputs;
  expect(bgm.gain.value).toBe(0);
  expect(sfx.gain.value).toBe(0.7);
  const later = context.createChannelMerger();
  later.connect(context.destination);
  expect([...later.outputs]).toEqual([sfx]);

  processor.disconnect();
  processor.connect(context.destination);
  mix.set(0.35, 0.7);
  expect([...processor.outputs]).toEqual([bgm]);
  expect(bgm.gain.value).toBe(0.35);
  expect(sfx.gain.value).toBe(0.7);
  mix.detach();
  expect(runtime.AudioContext).toBe(Context);
});

test('th05 preserves the authored BGM gain and scales its independent sound output', () => {
  const action = vi.fn();
  const runtime = { AudioContext: Context, Module: { eaglerAudio: { action } } };
  const mix = createRuntimeAudioMix();
  mix.attach(runtime, 'th05');
  const context = new runtime.AudioContext();
  const effect = context.createBufferSource();
  effect.connect(context.destination);
  mix.set(0, 0.25);
  runtime.Module.eaglerAudio.action(5, 0.6);
  expect(action).toHaveBeenLastCalledWith(5, 0);
  mix.set(0.5, 0.25);
  expect(action).toHaveBeenLastCalledWith(5, 0.3);
  expect([...effect.outputs][0].gain.value).toBe(0.25);
  mix.detach();
  expect(runtime.Module.eaglerAudio.action).toBe(action);
});
