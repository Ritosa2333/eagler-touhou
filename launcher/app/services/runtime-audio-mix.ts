type AudioChannel = 'music' | 'effects';
type AudioLevels = Record<AudioChannel, number>;
type AudioFactory = 'createScriptProcessor' | 'createChannelMerger' | 'createBufferSource';
type AudioOutputs = Record<AudioChannel, GainNode> & { separated: boolean };

export interface RuntimeAudioWindow {
  AudioContext?: typeof AudioContext;
  webkitAudioContext?: typeof AudioContext;
  Module?: {
    SDL3?: { audioContext?: AudioContext; audio_playback?: { scriptProcessorNode?: AudioNode } };
    eaglerSfxOutput?: { context: AudioContext; voices: Map<unknown, { merge: AudioNode }> };
    eaglerAudio?: { action(command: number, value: number): void };
  };
}

const clampGain = (value: number) => Number.isFinite(value) ? Math.min(1, Math.max(0, value)) : 1;

/** A local output mix for the authenticated game document. Runtime track
 * timing, in-game fades and effect playback remain owned by the game. */
export function createRuntimeAudioMix() {
  let levels: AudioLevels = { music: 1, effects: 1 };
  let binding: ReturnType<typeof bind> | null = null;

  function bind(runtime: RuntimeAudioWindow, th05: boolean) {
    const outputs = new Map<AudioContext, AudioOutputs>();
    const routed = new WeakSet<AudioNode>();
    const cleanup: (() => void)[] = [];
    let active = true;
    let sfxPool: NonNullable<RuntimeAudioWindow['Module']>['eaglerSfxOutput'];
    let setTh05Music: ((volume: number) => void) | undefined;

    function route(node: AudioNode, output: GainNode, alreadyConnected = false) {
      if (routed.has(node)) return;
      routed.add(node);
      const { context, connect, disconnect } = node;
      // The Runtime may reconnect this same node after resuming Web Audio.
      node.connect = function (destination, ...args: number[]) {
        if (active && destination === context.destination) {
          Reflect.apply(connect, node, [output, ...args]);
          return destination;
        }
        return Reflect.apply(connect, node, [destination, ...args]);
      } as AudioNode['connect'];
      node.disconnect = function (...args: unknown[]) {
        if (active && args[0] === context.destination) args[0] = output;
        return Reflect.apply(disconnect, node, args);
      } as AudioNode['disconnect'];
      if (alreadyConnected) {
        try {
          Reflect.apply(disconnect, node, [context.destination]);
          Reflect.apply(connect, node, [output]);
        } catch { /* An output paused by its Runtime will reconnect later. */ }
      }
    }

    function contextOutputs(context: AudioContext) {
      const existing = outputs.get(context);
      if (existing) return existing;
      const gains = { music: context.createGain(), effects: context.createGain(), separated: false };
      gains.effects.gain.value = levels.effects;
      for (const channel of ['music', 'effects'] as const) {
        gains[channel].connect(context.destination);
      }
      outputs.set(context, gains);
      function wrap(factory: AudioFactory, channel: AudioChannel) {
        const original = context[factory];
        if (typeof original !== 'function') return;
        const descriptor = Object.getOwnPropertyDescriptor(context, factory);
        const create = (...args: unknown[]) => {
          const node = Reflect.apply(original, context, args) as AudioNode;
          route(node, gains[channel]);
          if (factory === 'createChannelMerger' && !gains.separated) queueMicrotask(refresh);
          return node;
        };
        Object.defineProperty(context, factory, { configurable: true, writable: true, value: create });
        cleanup.push(() => {
          if (context[factory] !== create) return;
          if (descriptor) Object.defineProperty(context, factory, descriptor);
          else Reflect.deleteProperty(context, factory);
        });
      }
      // SDL streams supply PCM music. WebSfx merges its own stereo voices
      // before output. th05 supplies its own BGM gain and connects SE directly.
      if (th05) wrap('createBufferSource', 'effects');
      else {
        wrap('createScriptProcessor', 'music');
        wrap('createChannelMerger', 'effects');
      }
      return gains;
    }

    for (const key of ['AudioContext', 'webkitAudioContext'] as const) {
      const Original = runtime[key];
      if (!Original) continue;
      const Wrapped = new Proxy(Original, {
        construct(target, args, newTarget) {
          const context = Reflect.construct(target, args, newTarget) as AudioContext;
          contextOutputs(context);
          return context;
        },
      });
      runtime[key] = Wrapped;
      cleanup.push(() => { if (runtime[key] === Wrapped) runtime[key] = Original; });
    }

    function refresh() {
      if (!active) return;
      if (th05) {
        const audio = runtime.Module?.eaglerAudio;
        if (!audio || setTh05Music) return;
        const action = audio.action;
        let gameVolume = 1;
        const wrapped = function (command: number, value: number) {
          if (command !== 5) return action.call(audio, command, value);
          gameVolume = clampGain(value);
          action.call(audio, 5, gameVolume * levels.music);
        };
        audio.action = wrapped;
        setTh05Music = (volume) => action.call(audio, 5, gameVolume * volume);
        cleanup.push(() => { if (audio.action === wrapped) audio.action = action; });
        if (levels.music !== 1) setTh05Music(levels.music);
        return;
      }
      const module = runtime.Module;
      const sdl = module?.SDL3;
      const context = sdl?.audioContext;
      if (!context || context.state === 'closed') return;
      const gains = contextOutputs(context);
      const music = sdl.audio_playback?.scriptProcessorNode;
      if (music) route(music, gains.music, true);
      const pool = module?.eaglerSfxOutput;
      // Older Runtime generations can mix music and effects into one stream.
      // Only change the SDL gain once the independent WebSfx output exists.
      if (!gains.separated && pool?.context === context && pool.voices.size > 0) {
        gains.separated = true;
        gains.music.gain.value = levels.music;
      }
      if (sfxPool !== pool) {
        sfxPool = pool;
        for (const voice of pool?.voices.values() ?? []) route(voice.merge, gains.effects, true);
      }
    }
    return {
      refresh,
      apply(previous: AudioLevels) {
        for (const gains of outputs.values()) {
          if (gains.separated && previous.music !== levels.music) gains.music.gain.value = levels.music;
          if (previous.effects !== levels.effects) gains.effects.gain.value = levels.effects;
        }
        if (previous.music !== levels.music) setTh05Music?.(levels.music);
      },
      dispose() {
        active = false;
        for (const undo of cleanup.reverse()) undo();
        for (const gains of outputs.values()) { gains.music.disconnect(); gains.effects.disconnect(); }
        outputs.clear();
      },
    };
  }

  return {
    attach(runtime: RuntimeAudioWindow, game: string) {
      binding?.dispose();
      binding = bind(runtime, game === 'th05');
      binding.refresh();
    },
    refresh() { binding?.refresh(); },
    set(music: number, effects: number) {
      const next = { music: clampGain(music), effects: clampGain(effects) };
      if (next.music === levels.music && next.effects === levels.effects) return;
      const previous = levels;
      levels = next;
      binding?.apply(previous);
    },
    detach() { binding?.dispose(); binding = null; },
  };
}
