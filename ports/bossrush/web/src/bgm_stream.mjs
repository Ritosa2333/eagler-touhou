// RGL Web Runtime -- OGG 流式 BGM 播放器（#11 二期）
//
// 为什么不直接 decodeAudioData：它必须等**整首**解完才能拿到 AudioBuffer。
// 实测 170 秒的曲子 520ms、195 秒 798ms，这段时间里游戏已经在跑帧，于是
// 「进关卡过一会才播」和「已经播过一段」成了同一个 bug（见 bugs.md #11）。
//
// 这里改成「按块解 + 按块排」：
//   1. C 侧 stb_vorbis（web/src/rgl_ogg.c）只解「接下来要播的那 8192 帧」；
//   2. 解出来直接灌进 8192 帧的 AudioBuffer，接到 AudioContext 时间轴上；
//   3. 队列维持约 2 秒，用 source.onended + 80ms 定时器补货。
// 首块即出声，PCM 只保留队列里那 2 秒，编码字节留在 JS 侧给循环回卷复用。
//
// 参考：eagler-touhou `ports/th12/src/audio/{streaming-music,vorbis-decoder}.mjs`
//（同一套范式：不解整曲、小块排程、按音频时钟算位置）。这里只保留我们
// 引擎用得到的部分（无 fade —— 淡入淡出由 audio.mjs 的 gain 节点负责）。

const BLOCK_FRAMES = 8192;
const QUEUE_SECONDS = 2;
const REFILL_MILLISECONDS = 80;
/** 首个 source 稍微往后排一点，避免 start() 时时间戳已经过去。 */
const START_LEAD_SECONDS = 0.02;

/**
 * 一份已打开的 Vorbis 字节流。close() 之后所有方法都会抛错。
 * Module 需导出 rgl_ogg_* 与 _malloc/_free、HEAPU8/HEAPF32。
 */
export function openVorbis(Module, bytes) {
  const data = bytes instanceof Uint8Array ? bytes : new Uint8Array(bytes);
  if (!data.length) throw new Error('OGG 字节为空');
  const input = Module._malloc(data.length);
  if (!input) throw new Error('OGG 解码器内存不足');
  Module.HEAPU8.set(data, input);
  let handle = 0;
  let closed = false;
  const failure = (what) => new Error('OGG ' + what + '失败（err=' + Module._rgl_ogg_error() + '）');
  const close = () => {
    if (closed) return;
    closed = true;
    if (handle) Module._rgl_ogg_close(handle);
    if (input) Module._free(input);
    handle = 0;
  };
  try {
    handle = Module._rgl_ogg_open(input, data.length);
    if (!handle) throw failure('打开');
    const channels = Module._rgl_ogg_channels(handle);
    const sampleRate = Module._rgl_ogg_rate(handle);
    const length = Module._rgl_ogg_length(handle) >>> 0;
    const capacity = Module._rgl_ogg_max_frames();
    if (!channels || !sampleRate || !length) throw failure('读取头信息');
    const assertOpen = () => { if (closed) throw new Error('OGG 解码器已关闭'); };
    return {
      channels, sampleRate, length, capacity,
      duration: length / sampleRate,
      seek(frame) {
        assertOpen();
        if (!Number.isInteger(frame) || frame < 0 || frame >= length) {
          throw new Error('OGG 采样位置无效：' + frame);
        }
        if (!Module._rgl_ogg_seek(handle, frame)) throw failure('定位');
      },
      /** 返回 planar 的 Float32Array 视图；只在下次 read/seek 前有效。 */
      read(frames) {
        assertOpen();
        if (!Number.isInteger(frames) || frames < 1 || frames > capacity) {
          throw new Error('OGG 解码块大小无效：' + frames);
        }
        const count = Module._rgl_ogg_read(handle, frames);
        if (count < 0) throw failure('解码');
        const ptr = Module._rgl_ogg_pcm(handle);
        const base = (ptr >>> 2) + 0;
        const heap = Module.HEAPF32;
        const view = [];
        for (let ch = 0; ch < channels; ch++) {
          const off = base + ch * capacity;
          view.push(heap.subarray(off, off + count));
        }
        return { frames: count, channels: view };
      },
      close,
    };
  } catch (e) {
    close();
    throw e;
  }
}

/**
 * 单轨流式播放器。一个实例只管一首正在播的 BGM（暂停时保留位置、释放解码器）。
 *
 * @param {object} o
 * @param {object} o.Module       wasm 模块（需 rgl_ogg_* / _malloc / _free）
 * @param {object} o.ctx          AudioContext
 * @param {object} o.out          目标 gain 节点（audio.mjs 的 bgmGain）
 * @param {function} o.onEnd      循环关闭时播到结尾的回调
 * @param {function} o.onError    运行期出错的回调
 */
export class OggStreamPlayer {
  constructor({ Module, ctx, out, name = '', onEnd = null, onError = null }) {
    this.Module = Module;
    this.ctx = ctx;
    this.out = out;
    /** 曲名：onEnd/onError 回调要靠它把 streams 表里的自己摘掉。 */
    this.name = name;
    this.onEnd = onEnd;
    this.onError = onError;
    this.volume = 1;
    this.rate = 1;
    this.loop = true;
    this.loopStartFrame = 0;
    this.loopEndFrame = 0;
    this.bytes = null;
    this.decoder = null;
    this.gain = null;
    this.sources = new Set();
    this.timer = null;
    this.startedAt = 0;
    this.scheduledFrames = 0;
    this.startFrame = 0;
    this.cursorFrame = 0;
    this.underruns = 0;
    /** 解码器采样率（打开后才知道；暂停时用它把帧号换算回秒）。 */
    this.sampleRate = 44100;
    this.stats = { blocks: 0, underruns: 0, firstBlockMs: 0 };
    this.version = 0;
    this.finished = false;
  }

  get playing() { return !!this.decoder; }

  /** 当前播放位置（秒），暂停状态下就是停表位置。 */
  get positionSeconds() {
    if (!this.decoder) return this.startFrame / this.sampleRate;
    const played = this.playedFrames();
    return this.frameAt(this.startFrame + played) / this.decoder.sampleRate;
  }

  playedFrames() {
    if (!this.decoder) return 0;
    const elapsed = Math.max(0, this.ctx.currentTime - this.startedAt);
    const frames = Math.round(elapsed * this.rate * this.decoder.sampleRate);
    return Math.min(frames, this.scheduledFrames);
  }

  frameAt(frame) {
    const start = this.loopStartFrame;
    const end = this.loopEndFrame;
    if (!this.loop || end <= start || frame < end) return frame;
    return start + (frame - start) % (end - start);
  }

  /**
   * 起播。offsetSeconds 是曲内位置（秒）。
   * @returns {Promise<boolean>} 是否真的开始排程
   */
  async play(bytes, { volume = 1, rate = 1, offsetSeconds = 0, loop = true,
    loopStart = 0, loopEnd = 0 } = {}) {
    this.stop();
    const version = ++this.version;
    this.bytes = bytes;
    this.volume = Math.max(0, Math.min(1, volume));
    this.rate = rate > 0 ? rate : 1;
    this.loop = !!loop;
    const t0 = now();
    const decoder = openVorbis(this.Module, bytes);
    if (version !== this.version) { decoder.close(); return false; }
    this.decoder = decoder;
    this.sampleRate = decoder.sampleRate;
    this.loopStartFrame = Math.max(0, Math.round((loopStart > 0 ? loopStart : 0) * decoder.sampleRate));
    const endDefault = decoder.duration;
    this.loopEndFrame = Math.round((loopEnd > 0 ? loopEnd : endDefault) * decoder.sampleRate);
    if (this.loopEndFrame <= this.loopStartFrame || this.loopEndFrame > decoder.length) {
      this.loopStartFrame = 0;
      this.loopEndFrame = decoder.length;
    }
    this.startFrame = this.frameAt(Math.max(0, Math.round((offsetSeconds > 0 ? offsetSeconds : 0) * decoder.sampleRate)));
    this.cursorFrame = this.startFrame;
    decoder.seek(this.cursorFrame);
    this.scheduledFrames = 0;
    this.finished = false;
    const gain = this.ctx.createGain();
    gain.gain.value = this.volume;
    gain.connect(this.out);
    this.gain = gain;
    this.startedAt = this.ctx.currentTime + START_LEAD_SECONDS;
    this.stats.firstBlockMs = 0;
    try {
      this.pump(version);
    } catch (e) {
      this.fail(e);
      throw e;
    }
    this.stats.firstBlockMs = Math.round(now() - t0);
    return this.sources.size > 0;
  }

  /** 解一块 8192 帧（跨循环点自动回卷）。 */
  readBlock() {
    const d = this.decoder;
    const buffer = this.ctx.createBuffer(d.channels, BLOCK_FRAMES, d.sampleRate);
    let written = 0;
    let guard = 0;
    while (written < BLOCK_FRAMES) {
      if (this.cursorFrame >= this.loopEndFrame) {
        if (!this.loop) {
          // 不循环：剩下多少写多少，播完就结束。
          if (!written) return null;
          break;
        }
        this.cursorFrame = this.loopStartFrame;
        d.seek(this.cursorFrame);
      }
      const want = Math.min(BLOCK_FRAMES - written, this.loopEndFrame - this.cursorFrame);
      const got = d.read(want);
      if (got.frames !== want) {
        if (!got.frames) throw new Error('OGG 在循环终点前意外结束');
        written += got.frames;
        this.cursorFrame += got.frames;
        break;
      }
      for (let ch = 0; ch < d.channels; ch++) buffer.copyToChannel(got.channels[ch], ch, written);
      written += got.frames;
      this.cursorFrame += got.frames;
      guard++;
      if (guard > BLOCK_FRAMES) throw new Error('OGG 解码陷入死循环');
    }
    this.stats.blocks++;
    return buffer;
  }

  enqueue(buffer, version) {
    const d = this.decoder;
    const source = this.ctx.createBufferSource();
    source.buffer = buffer;
    try { source.playbackRate.value = this.rate; } catch { }
    source.connect(this.gain);
    this.sources.add(source);
    source.onended = () => {
      source.onended = null;
      this.sources.delete(source);
      try { source.disconnect(); } catch { }
      if (version !== this.version || !this.decoder) return;
      try { this.pump(version); } catch (e) { this.fail(e); }
    };
    const when = this.startedAt + this.scheduledFrames / (this.rate * d.sampleRate);
    source.start(Math.max(when, this.ctx.currentTime));
    this.scheduledFrames += buffer.length;
  }

  pump(version) {
    if (version !== this.version || !this.decoder) return;
    if (this.timer !== null) { clearTimeout(this.timer); this.timer = null; }
    const d = this.decoder;
    const nowTime = this.ctx.currentTime;
    const queuedUntil = this.startedAt + this.scheduledFrames / (this.rate * d.sampleRate);
    if (this.sources.size && nowTime > queuedUntil) {
      /* 主线程被阻塞超过队列长度：不能在过去排程，只能按音频时钟往前跳，
         跳过的声音找不回来（与参考实现同样语义），但要保证不静音卡死。 */
      this.scheduledFrames = Math.ceil((nowTime + START_LEAD_SECONDS - this.startedAt) * this.rate * d.sampleRate);
      this.cursorFrame = this.frameAt(this.startFrame + this.scheduledFrames);
      d.seek(this.cursorFrame);
      this.underruns++;
      this.stats.underruns = this.underruns;
      if (this.underruns === 1) console.warn('OGG 流式 BGM 队列耗尽，已按音频时钟恢复');
    }
    const maxBuffers = Math.ceil(QUEUE_SECONDS * this.rate * d.sampleRate / BLOCK_FRAMES) + 1;
    while (this.sources.size < maxBuffers &&
      this.startedAt + this.scheduledFrames / (this.rate * d.sampleRate) < nowTime + QUEUE_SECONDS) {
      const buffer = this.readBlock();
      if (!buffer) { this.finish(); return; }
      this.enqueue(buffer, version);
    }
    if (this.sources.size) {
      this.timer = setTimeout(() => {
        try { this.pump(version); } catch (e) { this.fail(e); }
      }, REFILL_MILLISECONDS);
    } else {
      this.finish();
    }
  }

  finish() {
    if (this.finished) return;
    this.finished = true;
    const cb = this.onEnd;
    const name = this.name;
    this.detach();
    this.decoder = null;
    this.startFrame = 0;
    this.scheduledFrames = 0;
    if (cb) { try { cb(name); } catch { } }
  }

  fail(e) {
    ++this.version;
    const cb = this.onError;
    this.detach();
    this.decoder = null;
    this.error = e;
    if (cb) { try { cb(e); } catch { } }
  }

  detach() {
    if (this.timer !== null) { clearTimeout(this.timer); this.timer = null; }
    for (const source of this.sources) {
      source.onended = null;
      try { source.stop(); } catch { }
      try { source.disconnect(); } catch { }
    }
    this.sources.clear();
    if (this.gain) {
      try { this.gain.disconnect(); } catch { }
      this.gain = null;
    }
    if (this.decoder) {
      this.decoder.close();
      this.decoder = null;
    }
    this.bytes = null;
  }

  /** 记下当前位置并停掉排程（WebAudio 的 BufferSource 不能真暂停）。 */
  pause() {
    const offset = this.positionSeconds;
    ++this.version;
    this.detach();
    this.startFrame = Math.round(offset * this.sampleRate);
    this.scheduledFrames = 0;
    return offset;
  }

  stop() {
    ++this.version;
    this.detach();
    this.startFrame = 0;
    this.scheduledFrames = 0;
    this.finished = false;
  }

  setVolume(v) {
    this.volume = Math.max(0, Math.min(1, v));
    if (this.gain) {
      try { this.gain.gain.setTargetAtTime(this.volume, this.ctx.currentTime, 0.02); }
      catch { this.gain.gain.value = this.volume; }
    }
  }

  setRate(r) {
    if (!(r > 0) || r === this.rate) return;
    /* 改速度要重新排程：已排的 source 各带一份 playbackRate，改一半会错拍。
       位置按改之前的时钟算准，再用新 rate 重排。 */
    const pos = this.positionSeconds;
    const bytes = this.bytes;
    const state = {
      volume: this.volume, loop: this.loop,
      loopStart: this.loopStartFrame / this.sampleRate,
      loopEnd: this.loopEndFrame / this.sampleRate,
    };
    if (!bytes || !this.decoder) { this.rate = r; return; }
    this.play(bytes, { ...state, rate: r, offsetSeconds: pos }).catch((e) => this.fail(e));
  }

  setLoop(loop) {
    this.loop = !!loop;
  }

  setLoopRange(startSeconds, endSeconds) {
    const d = this.decoder;
    const start = Math.max(0, Math.round((startSeconds || 0) * this.sampleRate));
    if (!d) {
      this.loopStartFrame = start;
      return;
    }
    const end = Math.round((endSeconds || d.duration) * d.sampleRate);
    if (end <= start || end > d.length) return;
    this.loopStartFrame = start;
    this.loopEndFrame = end;
    this.cursorFrame = this.frameAt(this.cursorFrame);
    d.seek(this.cursorFrame);
  }
}

function now() {
  return typeof performance !== 'undefined' && performance.now ? performance.now() : Date.now();
}
