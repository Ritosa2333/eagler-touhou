// Short original synth cues. This layer plays Launcher feedback, separate from game audio.
const cues = {
  seat: [784, 1175],
  ready: [659, 988, 1319],
  nudge: [880, 698, 880],
};

export type RoomActionSound = keyof typeof cues;

export function createRoomActionAudio(window: Window & typeof globalThis) {
  let context: AudioContext | undefined;

  return {
    unlock() {
      const Audio = window.AudioContext ?? (window as typeof window & {
        webkitAudioContext?: typeof AudioContext;
      }).webkitAudioContext;
      if (!Audio) return;
      try {
        context ??= new Audio();
        if (context.state !== 'running') void context.resume().catch(() => {});
      } catch {}
    },
    play(sound: RoomActionSound, volume: number) {
      if (context?.state !== 'running' || !Number.isFinite(volume) || volume <= 0) return;
      const start = context.currentTime;
      const level = 0.085 * Math.min(1, volume);
      for (const [index, frequency] of cues[sound].entries()) {
        const at = start + index * 0.046;
        const oscillator = context.createOscillator();
        const gain = context.createGain();
        oscillator.type = 'triangle';
        oscillator.frequency.value = frequency;
        gain.gain.setValueAtTime(0, at);
        gain.gain.linearRampToValueAtTime(level, at + 0.008);
        gain.gain.exponentialRampToValueAtTime(level * 0.01, at + 0.067);
        oscillator.connect(gain);
        gain.connect(context.destination);
        oscillator.onended = () => { oscillator.disconnect(); gain.disconnect(); };
        oscillator.start(at);
        oscillator.stop(at + 0.07);
      }
    },
    dispose() {
      if (context && context.state !== 'closed') void context.close().catch(() => {});
      context = undefined;
    },
  };
}
