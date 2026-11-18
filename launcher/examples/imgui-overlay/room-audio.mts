import type {RoomAudio} from '../../app/services/room-music';

type AudioCommand = {action: 'load' | 'clear' | 'seek' | 'volume' | 'play' | 'pause'; value?: string | number; generation: number};
type AudioState = {type: string; generation: number; duration?: number; position?: number};

export class NativeRoomAudio extends EventTarget implements RoomAudio {
  src = '';
  preload: HTMLMediaElement['preload'] = 'auto';
  paused = true;
  ended = false;
  muted = false;
  error: MediaError | null = null;
  duration = 0;
  readyState = 0;
  private generation = 0;
  private position = 0;
  private gain = .5;

  constructor(private readonly send: (command: AudioCommand) => void) { super(); }

  get volume() { return this.gain; }
  set volume(value: number) {
    this.gain = value;
    this.command('volume', value);
    this.dispatchEvent(new Event('volumechange'));
  }

  get currentTime() { return this.position; }
  set currentTime(value: number) {
    this.position = value;
    this.ended = false;
    this.command('seek', value);
  }

  private command(action: AudioCommand['action'], value?: AudioCommand['value']) {
    this.send({action, ...(value === undefined ? {} : {value}), generation:this.generation});
  }

  removeAttribute(name: string) { if (name === 'src') this.src = ''; }
  load() {
    this.generation++;
    this.paused = true;
    this.ended = false;
    this.error = null;
    this.duration = this.readyState = this.position = 0;
    this.command(this.src ? 'load' : 'clear', this.src || undefined);
    if (this.src) this.command('volume', this.gain);
    else this.dispatchEvent(new Event('emptied'));
  }

  async play() {
    if (!this.src || !this.readyState) throw Error('unavailable');
    this.paused = false;
    this.ended = false;
    this.command('play');
  }

  pause() {
    this.paused = true;
    this.command('pause');
    this.dispatchEvent(new Event('pause'));
  }

  receive(message: AudioState) {
    if (!this.src || message.generation !== this.generation) return;
    if (typeof message.position === 'number' && Number.isFinite(message.position)) this.position = message.position;
    switch (message.type) {
      case 'loadedmetadata':
        this.duration = message.duration ?? 0;
        this.readyState = 1;
        break;
      case 'playing':
        if (this.paused) return;
        break;
      case 'ended':
        this.ended = this.paused = true;
        break;
      case 'error':
        this.error = { code: 3, message: 'unavailable' } as MediaError;
        this.paused = true;
        break;
      case 'position': return;
      default: return;
    }
    this.dispatchEvent(new Event(message.type));
  }
}
