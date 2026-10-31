import {DEFAULT_VORBIS_MODULE_URL, inspectVorbis, openVorbisDecoder} from './vorbis-decoder.mjs';

const BLOCK_FRAMES = 8192;
const QUEUE_SECONDS = 2;
const REFILL_MILLISECONDS = 80;
const START_LEAD_SECONDS = .04;
const FADE_FLOOR = Math.pow(10, -2.5);

// The same track/pause/fade API as MusicPlayer. Only compressed OGG data is
// cached per file. One active decoder and at most ~2 s of PCM are retained.
export class StreamingMusicPlayer {
  constructor({context, readBytes, volume=.35, onError=null, decoderModuleURL=DEFAULT_VORBIS_MODULE_URL}) {
    this.context = context;
    this.readBytes = readBytes;
    this.volume = volume;
    this.onError = onError;
    this.decoderModuleURL = decoderModuleURL;
    // Values are encoded bytes and stream metadata, never decoded AudioBuffers.
    this.buffers = new Map();
    this.pending = new Map();
    this.source = null;
    this.gain = null;
    this.track = null;
    this.offset = 0;
    this.startedAt = 0;
    this.paused = false;
    this.fadeState = null;
    this.fadeWaiting = false;
    this.request = 0;
    this.error = null;
    this.underruns = 0;
    this.decoder = null;
    this.sources = new Set();
    this.timer = null;
    this.metadata = null;
    this.isCurrent = () => true;
    this.startFrame = 0;
    this.cursorFrame = 0;
    this.scheduledFrames = 0;
    this.loopStartFrame = 0;
    this.loopEndFrame = 0;
  }

  load(track) {
    const file = track.file;
    if (this.buffers.has(file)) return Promise.resolve(this.buffers.get(file));
    if (this.pending.has(file)) return this.pending.get(file);
    const promise = Promise.resolve().then(() => this.readBytes(file)).then(async bytes => {
      let encoded;
      if (bytes instanceof ArrayBuffer) encoded = new Uint8Array(bytes.slice(0));
      else if (ArrayBuffer.isView(bytes)) encoded = new Uint8Array(bytes.buffer, bytes.byteOffset, bytes.byteLength).slice();
      else throw Error('OGG 资源没有提供有效字节：' + file);
      // Header/setup and last-page metadata only, with the codec's bounded
      // startup frame. No decodeAudioData or whole-file PCM decode is used.
      const metadata = await inspectVorbis(encoded, {moduleURL:this.decoderModuleURL});
      const record = {...metadata, encoded};
      this.buffers.set(file, record);
      return record;
    }).catch(error => {
      throw new Error('OGG 音乐加载失败：' + file + '：' + error.message, {cause:error});
    }).finally(() => {
      if (this.pending.get(file) === promise) this.pending.delete(file);
    });
    this.pending.set(file, promise);
    return promise;
  }

  preload(tracks) { return Promise.all(tracks.map(track => this.load(track))); }

  stop() {
    ++this.request;
    this.track = null;
    this.offset = 0;
    this.paused = false;
    this.fadeState = null;
    this.fadeWaiting = false;
    this.detach();
  }

  detach() {
    if (this.timer !== null) clearTimeout(this.timer);
    this.timer = null;
    this.source = null;
    for (const source of this.sources) {
      source.onended = null;
      try { source.stop(); } catch {}
      source.disconnect();
    }
    this.sources.clear();
    this.gain?.disconnect();
    this.gain = null;
    this.decoder?.close();
    this.decoder = null;
    this.metadata = null;
  }

  setVolume(value) {
    value = Math.max(0, Math.min(1, value));
    if (value === this.volume) return;
    this.volume = value;
    this.fadeState = null;
    this.fadeWaiting = false;
    if (this.gain) {
      this.gain.gain.cancelScheduledValues(this.context.currentTime);
      this.gain.gain.value = value;
    }
  }

  fade(seconds=1) {
    const now = this.context.currentTime, from = this.fadeLevel(now);
    this.fadeState = {from, start:now, duration:Math.max(0, seconds)};
    this.fadeWaiting = false;
    this.scheduleFade();
  }

  fadeLevel(now=this.context.currentTime) {
    const fade = this.fadeState;
    if (!fade) return this.volume;
    if (fade.duration <= 0) return 0;
    const elapsed = this.paused || this.fadeWaiting ? 0 : Math.max(0, now - fade.start);
    const t = Math.min(1, elapsed / Math.max(fade.duration, Number.EPSILON));
    return !fade.from || t >= 1 ? 0 : fade.from * Math.pow(FADE_FLOOR / fade.from, t);
  }

  scheduleFade() {
    if (!this.gain || !this.fadeState) return;
    const now = this.context.currentTime, from = this.fadeLevel(now);
    const end = this.fadeState.start + this.fadeState.duration;
    const gain = this.gain.gain;
    gain.cancelScheduledValues(now);
    gain.setValueAtTime(from, now);
    if (from > 0 && end > now) {
      if (this.fadeState.start > now) gain.setValueAtTime(from, this.fadeState.start);
      gain.exponentialRampToValueAtTime(FADE_FLOOR, end);
      gain.setValueAtTime(0, end);
    }
  }

  offsetAt(seconds) {
    const track = this.track;
    if (!track) return seconds;
    const rate = this.metadata?.sampleRate ?? track.sampleRate;
    if (rate > 0) {
      const start = this.metadata ? this.loopStartFrame : Math.round((track.loopStart ?? 0) * rate);
      const end = this.metadata ? this.loopEndFrame : Math.round((track.loopEnd ?? 0) * rate);
      const frame = Math.round(seconds * rate), length = end - start;
      return (length > 0 && frame >= end ? start + (frame - start) % length : frame) / rate;
    }
    const length = (track.loopEnd ?? 0) - (track.loopStart ?? 0);
    return length > 0 && seconds >= track.loopEnd ? track.loopStart + (seconds - track.loopStart) % length : seconds;
  }

  pause() {
    if (!this.track || this.paused) return;
    const now = this.context.currentTime;
    if (this.source && this.metadata) {
      const elapsed = Math.max(0, Math.floor((now - this.startedAt) * this.metadata.sampleRate));
      this.offset = this.frameAt(this.startFrame + elapsed) / this.metadata.sampleRate;
    }
    if (this.fadeState) this.fadeState = {from:this.fadeLevel(now), start:now,
      duration:Math.max(0, this.fadeState.duration - (this.fadeWaiting ? 0 : Math.max(0, now - this.fadeState.start)))};
    this.fadeWaiting = false;
    this.paused = true;
    ++this.request;
    this.detach();
  }

  async resume(isCurrent=() => true) {
    if (!this.paused || !this.track) return false;
    const track = this.track, offsetSeconds = this.offset, fade = this.fadeState;
    const played = await this.play(track, isCurrent, {offsetSeconds, resumeFade:fade});
    return played;
  }

  async play(track, isCurrent=() => true, {offsetSeconds=0, resumeFade=null}={}) {
    this.stop();
    this.error = null;
    this.track = track;
    this.offset = this.offsetAt(Number.isFinite(offsetSeconds) ? Math.max(0, offsetSeconds) : 0);
    if (resumeFade) {
      this.fadeState = {...resumeFade, start:this.context.currentTime};
      this.fadeWaiting = true;
    }
    const request = this.request;
    if (!isCurrent()) return false;
    let decoder = null;
    try {
      const metadata = await this.load(track);
      if (request !== this.request || !isCurrent()) return false;
      decoder = await openVorbisDecoder(metadata.encoded, {moduleURL:this.decoderModuleURL});
      if (request !== this.request || !isCurrent()) { decoder.close(); return false; }
      this.configureTrack(track, metadata);
      this.decoder = decoder;
      this.isCurrent = isCurrent;
      this.startFrame = this.frameAt(Math.round(this.offset * metadata.sampleRate));
      this.offset = this.startFrame / metadata.sampleRate;
      this.cursorFrame = this.startFrame;
      decoder.seek(this.cursorFrame);
      this.scheduledFrames = 0;
      const first = this.readBlock();
      const gain = this.context.createGain();
      gain.gain.value = this.fadeLevel();
      this.gain = gain;
      gain.connect(this.context.destination);
      this.startedAt = this.context.currentTime + START_LEAD_SECONDS;
      if (this.fadeWaiting && this.fadeState) this.fadeState = {...this.fadeState, start:this.startedAt};
      this.fadeWaiting = false;
      this.enqueue(first, request);
      this.scheduleFade();
      this.pump(request);
      return true;
    } catch (error) {
      decoder?.close();
      if (request !== this.request || !isCurrent()) return false;
      this.error = new Error('OGG 流式播放失败：' + track.file + '：' + error.message, {cause:error});
      this.detach();
      throw this.error;
    }
  }

  configureTrack(track, metadata) {
    const rate = metadata.sampleRate;
    if (track.sampleRate != null && track.sampleRate !== rate) throw Error('OGG 采样率与音乐索引不一致');
    if (track.channels != null && track.channels !== metadata.channels) throw Error('OGG 声道数与音乐索引不一致');
    const start = Math.round((track.loopStart ?? 0) * rate);
    const end = Math.round((track.loopEnd ?? metadata.duration) * rate);
    if (!Number.isSafeInteger(start) || !Number.isSafeInteger(end) || start < 0 || end <= start || end > metadata.length) {
      throw Error('OGG 循环样本范围无效');
    }
    this.metadata = metadata;
    this.loopStartFrame = start;
    this.loopEndFrame = end;
  }

  frameAt(frame) {
    return frame >= this.loopEndFrame ? this.loopStartFrame +
      (frame - this.loopStartFrame) % (this.loopEndFrame - this.loopStartFrame) : frame;
  }

  readBlock() {
    const {channels, sampleRate} = this.metadata;
    const buffer = this.context.createBuffer(channels, BLOCK_FRAMES, sampleRate);
    let written = 0;
    while (written < BLOCK_FRAMES) {
      if (this.cursorFrame === this.loopEndFrame) {
        this.cursorFrame = this.loopStartFrame;
        this.decoder.seek(this.cursorFrame);
      }
      const requested = Math.min(BLOCK_FRAMES - written, this.loopEndFrame - this.cursorFrame);
      const decoded = this.decoder.read(requested);
      if (decoded.frames !== requested) throw Error('OGG 在循环终点之前意外结束');
      for (let channel = 0; channel < channels; ++channel) buffer.copyToChannel(decoded.channels[channel], channel, written);
      written += decoded.frames;
      this.cursorFrame += decoded.frames;
    }
    return buffer;
  }

  enqueue(buffer, request) {
    const source = this.context.createBufferSource();
    source.buffer = buffer;
    source.connect(this.gain);
    const wasEmpty = this.sources.size === 0;
    this.sources.add(source);
    if (wasEmpty || !this.source) this.source = source;
    source.onended = () => {
      source.onended = null;
      this.sources.delete(source);
      source.disconnect();
      if (request !== this.request || !this.decoder) return;
      // Keep source truthy through a late refill: callers use it as the
      // playback-state indicator. detach() clears it on pause/stop/error.
      if (this.source === source) this.source = this.sources.values().next().value ?? source;
      this.refill(request);
    };
    source.start(this.startedAt + this.scheduledFrames / this.metadata.sampleRate);
    this.scheduledFrames += buffer.length;
  }

  pump(request) {
    if (request !== this.request || !this.decoder || this.paused) return;
    if (!this.isCurrent()) { this.stop(); return; }
    if (this.context.state === 'closed') throw Error('音乐使用的音频环境已关闭');
    if (this.timer !== null) clearTimeout(this.timer);
    this.timer = null;
    const rate = this.metadata.sampleRate;
    const now = this.context.currentTime;
    const queuedUntil = this.startedAt + this.scheduledFrames / rate;
    if (now > queuedUntil) {
      // A stalled main thread cannot schedule in the past. Preserve the game
      // music clock by seeking to the current sample, never restarting it.
      this.scheduledFrames = Math.ceil((now + START_LEAD_SECONDS - this.startedAt) * rate);
      this.cursorFrame = this.frameAt(this.startFrame + this.scheduledFrames);
      this.decoder.seek(this.cursorFrame);
      ++this.underruns;
      if (this.underruns === 1) console.warn('OGG 流式音乐队列耗尽，已按音频时钟恢复');
    }
    const maxQueuedBuffers = Math.ceil(QUEUE_SECONDS * rate / BLOCK_FRAMES) + 1;
    while (this.sources.size < maxQueuedBuffers &&
      this.startedAt + this.scheduledFrames / rate < now + QUEUE_SECONDS) {
      const buffer = this.readBlock();
      this.enqueue(buffer, request);
    }
    this.timer = setTimeout(() => this.refill(request), REFILL_MILLISECONDS);
  }

  refill(request) {
    try { this.pump(request); }
    catch (error) {
      if (request !== this.request) return;
      const file = this.track?.file ?? '';
      this.error = new Error('OGG 流式播放中断：' + file + '：' + error.message, {cause:error});
      ++this.request;
      this.detach();
      if (this.onError) this.onError(this.error);
      else {
        const failure = this.error;
        setTimeout(() => { throw failure; }, 0);
      }
    }
  }
}

export {StreamingMusicPlayer as MusicPlayer};
