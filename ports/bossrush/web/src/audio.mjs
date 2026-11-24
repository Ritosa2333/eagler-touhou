// RGL Web runtime -- WebAudio 执行层（1c-3）
//
// C 侧产命令 CSV（op,name,a,b,c），这里每帧取一次并执行。
// 资源是 game/ 下的音频（SE 与 BGM），按需 fetch + decodeAudioData，不预载整包。
// 2026-11-19 起 game/ BGM 就地压成 96kbps mono ogg（扩展名 .ogg 不变）；
// 2026-11-22 起 **SE 一律保持原生 PCM WAV**（清酒要求，音效果不再压缩），
// 所以同一个资源目录里 .wav 是真 RIFF、.ogg 是 vorbis，两种都要能解。
// decodeAudioData 按内容嗅探、不看扩展名，故路径不变、逻辑不变。
// 旧的 audioMap 重定向层保留兼容（传了就优先），默认不再使用。
//
// 浏览器自动播放策略：AudioContext 必须在用户手势后才能 resume。
// 解锁前的命令直接丢弃（不排队），否则首次点击会把启动时攒的几百个
// 音效一次性倾泻出来。shell.mjs 在启动快进后也会 flush 一次。

/* #11 二期：BGM 改流式分片解码（见 web/src/bgm_stream.mjs）。
   decodeAudioData 必须整首解完才能 start，170 秒的曲子要 520ms，于是
   「进关卡过一会才播」+「已经播过一段」同源。这里对 OGG 走
   「解 8192 帧 → 排一块 → 2 秒队列」，首块即出声；非 OGG（改过的
   .wav BGM）与 wasm 没带解码器时自动退回原来的 decodeAudioData 路径。 */
import { OggStreamPlayer } from './bgm_stream.mjs';

const OP = {
  PLAY_SE: 1, STOP_SE: 2, PAUSE_SE: 3, RESUME_SE: 4,
  PLAY_BGM: 5, STOP_BGM: 6, PAUSE_BGM: 7, RESUME_BGM: 8,
  SE_VOLUME: 9, BGM_VOLUME: 10, SE_SPEED: 11, BGM_SPEED: 12,
  BGM_LOOP: 13, LOOP_RANGE: 14, SE_VOLUME_ONE: 15, BGM_VOLUME_ONE: 16,
};
export function createAudioLayer(Module, opts = {}) {
  const { assetBase = './game/', enabled = true, force = false } = opts;
  const ccall = Module.ccall;
  /* 阶段 3：压缩音频映射表（可选）。game/ 源文件保持原样，
     web/build/audio/ 是低码率副本；命中映射就走压缩副本，未命中（如刚
     LFS 拉下来还没重压的曲子）回退原路径，行为与之前完全一致。 */
  const audioMap = opts.audioMap || null;
  /* SE 并发语义 —— 1c-26 起对齐原生，**每个音效资源只有一个播放器**。
     原生 `ResourceSoundEffectImpl`（v0.21.103，与 master 一致）里
     `m_player` 是单个 `IAudioPlayer`，`Play(vol,pan)` 只是把「Reset」命令
     记在 m_last_command 上，等这一帧的 FlushCommand 才 reset+setVolume+
     setBalance+start。也就是说：
       · 同一个音效在一帧里被 PlaySound 多少次，最终**只响一次**；
       · 每次都是从 0 重新开始（不会叠成回声）；
       · 音量取同帧所有命令里的**最大值**。
     以前这里是「同名最多 8 层 + 全局 64 层，超了抢占最旧」，于是
     「敌人被击破的 explode/enep00 连响十几遍」在 Web 端变成十几层同时混音，
     听起来就是「过响而且很糊」（bug5）。改成单声道池即可，同时把
     WebAudio 节点数量从 64 降到「每个资源 1 个」，对触屏端也是净收益。 */
  const SE_MAX_PER_NAME = opts.seMaxPerName || 1;
  const SE_MAX_TOTAL = opts.seMaxTotal || 48;
  /* BGM 解码后是 Float32 PCM：一首 184 秒的立体声 ogg 约 65MB。
     47 首全留 = 上 GB，触屏端会直接崩。所以设预算，超出时按 LRU 逐出
     **没在播** 的 BGM（正在播/暂停中的绝不逐出，否则会当场静音）。 */
  const BUF_BUDGET = (opts.bufferBudgetMB || 128) * 1048576;

  /** name → { path, kind, loopStart, loopEnd } */
  const res = new Map();
  /** name → AudioBuffer（只放整首解码的东西：SE，以及退回旧路径的 BGM） */
  const buffers = new Map();
  /** name → Uint8Array：OGG 编码字节，流式 BGM 复用（循环回卷不解码第二遍） */
  const oggBytes = new Map();
  let oggBytesTotal = 0;
  /* 编码字节比 PCM 小两个数量级（一首 3 分钟的曲子约 1~3 MB，解成 PCM 是
     60 MB），所以单独给一个宽松预算：47 首全留也就 100 MB 上下，但真正的
     保护是「只留用过的」，超出仍按 LRU 逐出。 */
  const OGG_BYTE_BUDGET = (opts.oggBudgetMB || 96) * 1048576;
  /** name → OggStreamPlayer：正在播 / 暂停中的流式 BGM */
  const streams = new Map();
  /** name → 解码后字节数，用于预算统计 */
  const bufBytes = new Map();
  let totalBytes = 0;
  /** LRU：name → 最后使用序号 */
  const lastUse = new Map();
  let useSeq = 0;
  /** name → Promise<AudioBuffer|null> */
  const pending = new Map();
  /** 正在播放的 SE：name → Set<{ src, gain, panner, startedAt, offset }> */
  const seVoices = new Map();
  /** 全部存活 SE voice（按启动顺序），用于全局并发上限抢占最旧的 */
  const liveVoices = [];
  /** 正在播放的 BGM：name → { src, gain, startedAt, offset, rate, loop, loopStart, loopEnd } */
  const bgm = new Map();
  /** BGM 代次：异步 decode 回来时若代次已变（被 stop/重播），这次 start 必须丢弃 */
  const bgmGen = new Map();
  /** 每个资源的循环区间（C 侧 LoadMusic 会给） */
  const loopRange = new Map();
  /** 暂停中的资源记录，供 resume 恢复 */
  const paused = new Map();

  let ctx = null;
  let seGain = null, bgmGain = null;
  let seVol = 1, bgmVol = 1;
  let unlocked = false;
  let onUnlock = null;
  let resVersion = -1;
  let running = enabled;
  let didResync = false;

  const stats = {
    cmds: 0, playSE: 0, playBGM: 0, decodeOK: 0, decodeFail: 0,
    fetchFail: 0, droppedLocked: 0, droppedRunning: 0, activeSE: 0, activeBGM: 0,
    seDropped: 0, sePreempt: 0, bufEvicted: 0, resyncs: 0, volSync: 0,
    bgmLog: [], bgmDrops: [],
    bgmFetch: 0, bgmStream: 0, bgmStreamFail: 0, oggEvicted: 0, activeStreams: 0,
    errors: [],
  };

  /* force：调试开关（?audio=force）。无头浏览器没有可信用户手势，
     AudioContext 恒为 suspended，命令会被正常丢弃；用这个开关强制
     走完 fetch / decode / start 全链路来验证代码本身没有异常。
     只影响自动化验证，正常游玩仍需真实手势解锁。 */
  const ready = () => unlocked || force;

  function ensureContext() {
    if (ctx) return ctx;
    const AC = window.AudioContext || window.webkitAudioContext;
    if (!AC) return null;
    try {
      /* 建节点之前先把 C 侧权威音量拉过来：launch.lua 在最早期就调了
         SetSEVolume(setting.sevolume/100)，那条命令会随启动期的快进被
         flush 掉，JS 侧于是永远停在默认 1.0 —— 玩家必须进一次「设置」
         把音量重新下发才正常（bug9）。只靠 didResync 那条路径不够：
         它要求 ctx 已经真的 running，而首次 unlock 恰好就是建 ctx 的时刻。 */
      syncVolumesFromHost();
      ctx = new AC({ latencyHint: 'interactive' });
      seGain = ctx.createGain();
      bgmGain = ctx.createGain();
      seGain.gain.value = seVol;
      bgmGain.gain.value = bgmVol;
      seGain.connect(ctx.destination);
      bgmGain.connect(ctx.destination);
    } catch (e) {
      stats.errors.push('AudioContext: ' + (e && e.message ? e.message : String(e)));
      ctx = null;
    }
    return ctx;
  }

  /** 用户手势解锁；解锁前到达的命令不缓存。 */
  /* 启动快进（fastForward 走 rgl_eval）会 flush 掉所有播放命令，而 C 侧
     状态机仍认为 BGM 在播。解锁后把「当前 playing 的 BGM」补播一次，
     否则 ?stage= 调试入口 / 快进补帧之后永远听不到音乐。 */
  function resyncPlaying() {
    const csv = ccall('rgl_host_audio_resync', 'string', [], []);
    if (!csv) return;
    for (const line of csv.split('\n')) {
      if (!line || line[0] === '#') continue;
      const [name, vol, speed, ls, le] = line.split(',');
      if (!name || bgm.has(name)) continue;
      loopRange.set(name, { start: +ls || 0, end: +le || 0 });
      playBGM(name, +vol || 1, 0, null, +speed || 1);
      stats.resyncs++;
    }
  }

  /* 平滑改音量：直接写 gain.value 在淡入淡出时每帧一个台阶，会有明显
     的"拉链噪声"（zzzip）。setTargetAtTime 是浏览器端的指数逼近，
     时间常数 20ms 足够快，又听不出台阶。 */
  function setGain(param, v) {
    if (!ctx) { param.value = v; return; }
    try { param.setTargetAtTime(v, ctx.currentTime, 0.02); }
    catch { param.value = v; }
  }

  /* LRU 逐出：只在预算超了时动，且跳过正在播 / 暂停中 / 正在解码的。
     keep 是「刚解码完、马上要 start 的那一首」——decode 完成到 start 之间
     它还不在 bgm 表里，不显式保护的话会被自己触发的逐出清掉：buf 对象
     还活着所以照样出声，但缓存里已经没了，下一帧再播就得重新 fetch+decode
     （预算小于单曲时表现为无限重解码）。 */
  function trimBuffers(keep) {
    if (totalBytes <= BUF_BUDGET) return;
    const victims = [];
    for (const [name, bytes] of bufBytes) {
      const rec = res.get(name);
      if (!rec || rec.kind !== 4) continue;        // SE 都很小，留着
      if (name === keep) continue;
      if (bgm.has(name) || pending.has(name)) continue;
      if (paused.has(name) || paused.has('se:' + name)) continue;
      victims.push([name, bytes, lastUse.get(name) || 0]);
    }
    victims.sort((a, b) => a[2] - b[2]);
    for (const [name, bytes] of victims) {
      if (totalBytes <= BUF_BUDGET) break;
      buffers.delete(name);
      bufBytes.delete(name);
      lastUse.delete(name);
      totalBytes -= bytes;
      stats.bufEvicted++;
    }
  }

  function unlock() {
    if (unlocked) return;
    const c = ensureContext();
    if (!c) return;
    const done = () => {
      unlocked = true;
      /* ctx 刚 running 时再对一次账：从建 ctx 到真正 running 之间，游戏可能
         又改过音量（设置菜单、存档恢复）。 */
      syncVolumesFromHost();
      if (onUnlock) onUnlock();
      // 重放统一放到 consume() 的「首次可播放」分支，避免 force 模式下
      // 从未触发 unlock() 而漏掉（无头浏览器没有可信手势）。
    };
    if (c.state === 'running' || force) done();   // force：无头环境 ctx 恒 suspended
    else {
      c.resume().then(done).catch(() => { /* 等下一次手势再试 */ });
    }
  }

  // 常见手势，一次即可；不用 once 是因为首次可能仍在 suspended
  for (const ev of ['pointerdown', 'keydown', 'touchstart']) {
    window.addEventListener(ev, unlock, { passive: true });
  }

  const urlOf = (p) => {
    const rel = String(p).replace(/\\/g, '/').replace(/^\/+/, '');
    const key = rel.toLowerCase();
    if (audioMap && audioMap[key]) return audioMap[key];
    return assetBase + rel.split('/').map(encodeURIComponent).join('/');
  };
  /* 导入包优先；不传时 urlOf 的 HTTP 行为不变。 */
  const assets = opts.assets || { fetchAsset: (p) => fetch(urlOf(p)) };

  /** 资源表：C 侧 LoadSound/LoadMusic 会递增 version，变化才重拉。 */
  function syncResources(force = false) {
    const v = ccall('rgl_host_audio_res_version', 'number', [], []);
    if (!force && v === resVersion) return;
    resVersion = v;
    const csv = ccall('rgl_host_audio_res', 'string', [], []);
    for (const line of csv.split('\n')) {
      if (!line || line[0] === '#') continue;
      const [name, kind, path, ls, le] = line.split(',');
      if (!name || name === 'name') continue;
      const rec = { path: path || '', kind: +kind || 0,
        loopStart: +ls || 0, loopEnd: +le || 0 };
      res.set(name, rec);
      if (rec.kind === 4 && rec.loopEnd > 0) {
        loopRange.set(name, { start: rec.loopStart, end: rec.loopEnd });
      }
    }
  }

  function loadBuffer(name) {
    if (buffers.has(name)) {
      lastUse.set(name, ++useSeq);
      return Promise.resolve(buffers.get(name));
    }
    if (pending.has(name)) return pending.get(name);
    const rec = res.get(name);
    if (!rec || !rec.path) {
      stats.fetchFail++;
      return Promise.resolve(null);
    }
    // 命中压缩映射时直接 fetch 映射后的 URL（assets 自带 fetchAsset 会
    // 忽略 urlOf，所以这里必须显式判断）；否则走调用方的 fetchAsset。
    const key = String(rec.path).replace(/\\/g, '/').replace(/^\/+/, '').toLowerCase();
    const mapped = audioMap && audioMap[key];
    const p = (mapped ? fetch(mapped) : assets.fetchAsset(rec.path))
      .then((r) => { if (!r.ok) throw new Error('HTTP ' + r.status); return r.arrayBuffer(); })
      .then((ab) => ensureContext().decodeAudioData(ab))
      .then((buf) => {
        buffers.set(name, buf);
        const bytes = buf.length * buf.numberOfChannels * 4;
        bufBytes.set(name, bytes);
        totalBytes += bytes;
        lastUse.set(name, ++useSeq);
        pending.delete(name);
        stats.decodeOK++;
        if (rec.kind === 4) trimBuffers(name);   // 只有 BGM 值得逐出
        return buf;
      })
      .catch((e) => {
        pending.delete(name);
        stats.decodeFail++;
        if (stats.errors.length < 12) stats.errors.push(name + ': ' + (e && e.message ? e.message : String(e)));
        return null;
      });
    pending.set(name, p);
    return p;
  }

  /* ---------------- BGM 流式分片解码（#11 二期） ---------------- */

  /* 老 wasm 没带 rgl_ogg_* 时整体退回 decodeAudioData，行为与之前一致。 */
  const oggDecoderReady = !!(Module && Module._rgl_ogg_open && Module._malloc);
  /** name → Promise<Uint8Array|null> */
  const pendingBytes = new Map();

  function isOggBytes(bytes) {
    return !!bytes && bytes.length > 4 &&
      bytes[0] === 0x4f && bytes[1] === 0x67 && bytes[2] === 0x67 && bytes[3] === 0x53;   // 'OggS'
  }

  /* 只取编码字节、不解码。流式路径靠它反复 seek 循环点，退回 decodeAudioData
     时也要一份完整字节（注意 decodeAudioData 会 detach 传进去的 ArrayBuffer，
     所以必须 slice 出副本，不能把缓存直接交出去）。 */
  function loadBytes(name) {
    if (oggBytes.has(name)) { lastUse.set(name, ++useSeq); return Promise.resolve(oggBytes.get(name)); }
    if (pendingBytes.has(name)) return pendingBytes.get(name);
    const rec = res.get(name);
    if (!rec || !rec.path) { stats.fetchFail++; return Promise.resolve(null); }
    const key = String(rec.path).replace(/\\/g, '/').replace(/^\/+/, '').toLowerCase();
    const mapped = audioMap && audioMap[key];
    const p = (mapped ? fetch(mapped) : assets.fetchAsset(rec.path))
      .then((r) => { if (!r.ok) throw new Error('HTTP ' + r.status); return r.arrayBuffer(); })
      .then((ab) => {
        const bytes = new Uint8Array(ab);
        oggBytes.set(name, bytes);
        oggBytesTotal += bytes.length;
        lastUse.set(name, ++useSeq);
        pendingBytes.delete(name);
        trimOggBytes(name);
        stats.bgmFetch++;
        return bytes;
      })
      .catch((e) => {
        pendingBytes.delete(name);
        stats.fetchFail++;
        if (stats.errors.length < 12) stats.errors.push('bgm ' + name + ': ' + (e && e.message ? e.message : String(e)));
        return null;
      });
    pendingBytes.set(name, p);
    return p;
  }

  /* encoded 字节的 LRU：正在播 / 暂停中 / 正在 fetch 的绝不逐出。 */
  function trimOggBytes(keep) {
    if (oggBytesTotal <= OGG_BYTE_BUDGET) return;
    const victims = [];
    for (const [name, bytes] of oggBytes) {
      if (name === keep) continue;
      if (streams.has(name) || pendingBytes.has(name)) continue;
      if (paused.has(name)) continue;
      victims.push([name, bytes.length, lastUse.get(name) || 0]);
    }
    victims.sort((a, b) => a[2] - b[2]);
    for (const [name, len] of victims) {
      if (oggBytesTotal <= OGG_BYTE_BUDGET) break;
      oggBytes.delete(name);
      oggBytesTotal -= len;
      stats.oggEvicted++;
    }
  }

  /* 退回路径：把已经取到的字节交给 decodeAudioData。必须送副本 ——
     decodeAudioData 会 detach 传进去的 ArrayBuffer，而字节缓存还要给
     循环回卷复用（同一首再播不该重新 fetch）。 */
  function decodeBytes(name, bytes) {
    if (buffers.has(name)) { lastUse.set(name, ++useSeq); return Promise.resolve(buffers.get(name)); }
    if (pending.has(name)) return pending.get(name);
    const c = ensureContext();
    if (!c || !bytes) return Promise.resolve(null);
    const copy = bytes.slice();
    const p = c.decodeAudioData(copy.buffer).then((buf) => {
      buffers.set(name, buf);
      const b = buf.length * buf.numberOfChannels * 4;
      bufBytes.set(name, b);
      totalBytes += b;
      lastUse.set(name, ++useSeq);
      pending.delete(name);
      stats.decodeOK++;
      return buf;
    }).catch((e) => {
      pending.delete(name);
      stats.decodeFail++;
      if (stats.errors.length < 12) stats.errors.push(name + ': ' + (e && e.message ? e.message : String(e)));
      return null;
    });
    pending.set(name, p);
    return p;
  }

  /* ---------------- SE ---------------- */

  /* 释放一个 voice：先标 dead，onended 回来时就不会重复摘链。
     顺带把它从 liveVoices 里惰性剔除（数组很小，逐次 filter 不划算）。 */
  function releaseVoice(name, v) {
    if (!v || v.dead) return;
    v.dead = true;
    try { v.src.stop(); } catch { }
    try { v.src.disconnect(); } catch { }
    const s = seVoices.get(name);
    if (s) { s.delete(v); if (!s.size) seVoices.delete(name); }
  }

  function pruneVoices() {
    if (!liveVoices.length) return;
    let j = 0;
    for (let i = 0; i < liveVoices.length; i++) {
      if (!liveVoices[i].v.dead) liveVoices[j++] = liveVoices[i];
    }
    liveVoices.length = j;
  }

  /* 1d-31 task#4：把 SE **全部解码**进内存（不是只下字节）。
     清酒的反馈：「部分 SE 滞后出现（boss 出现音效、击破音效）」。字节预取只
     解决了下载，`decodeAudioData` 仍然发生在**第一次 play** 那一刻 —— 那就是
     几十毫秒的迟到，而且是最容易察觉的「第一次响」。
     149 个 wav / 34.7 MB，解开后的 PCM 也就 ~35 MB，一次解完最划算；BGM
     （kind 4）是长流，不进这里（有自己的按需 + 逐出策略）。 */
  async function warm(opts) {
    const o = opts || {};
    const conc = Math.max(1, o.concurrency || 4);
    const names = [];
    for (const [n, rec] of res) {
      if (!rec.path || rec.kind === 4) continue;
      if (!buffers.has(n)) names.push(n);
    }
    let i = 0, done = 0, failed = 0;
    const t0 = (typeof performance !== 'undefined' ? performance.now() : Date.now());
    const worker = async () => {
      while (i < names.length) {
        const n = names[i++];
        const b = await loadBuffer(n);
        if (b) done++; else failed++;
        if (o.onProgress) { try { o.onProgress(done + failed, names.length); } catch { /* ignore */ } }
      }
    };
    const n = Math.min(conc, names.length);
    await Promise.all(Array.from({ length: n }, worker));
    const t1 = (typeof performance !== 'undefined' ? performance.now() : Date.now());
    return { total: names.length, done, failed, ms: Math.round(t1 - t0) };
  }

  function stopSE(name) {
    const set = seVoices.get(name);
    if (!set) return;
    for (const v of [...set]) releaseVoice(name, v);
    seVoices.delete(name);
  }

  /* 同一帧内同一个音效只允许起一次（原生 FlushCommand 每帧只放一次）。
     key = 音效名，value = { frame, voice, vol }；consume() 每帧不清理，
     靠 frame 号判断是不是本帧的。 */
  const seFrame = new Map();
  let frameNo = 0;

  function playSE(name, vol, pan, queuedBuf) {
    const rec = res.get(name);
    if (!rec || !rec.path) { stats.fetchFail++; return; }
    const start = (buf) => {
      if (!buf || !ctx || !ready() || !running) return;
      /* 本帧已经为这个音效起过一次：原生不会叠第二层，只把音量取大值。 */
      const prev = seFrame.get(name);
      if (prev && prev.frame === frameNo && prev.voice && !prev.voice.dead) {
        const v = Math.max(0, Math.min(1, Math.max(prev.vol, vol)));
        if (v !== prev.vol) { setGain(prev.voice.gain.gain, v); prev.vol = v; }
        stats.seDropped++;
        return;
      }
      /* 跨帧再来一次 = 原生 reset + start：把上一次的停掉，从头重放。
         这正是「敌人被击破音效」不该叠成一片糊糊的关键。 */
      let set = seVoices.get(name);
      if (set && set.size) {
        for (const old of [...set]) { releaseVoice(name, old); stats.sePreempt++; }
        set = seVoices.get(name);
      }
      if (!set) { set = new Set(); seVoices.set(name, set); }
      if (set.size >= SE_MAX_PER_NAME) {
        const oldest = set.values().next().value;
        releaseVoice(name, oldest);
        stats.sePreempt++;
        set = seVoices.get(name);
      }
      pruneVoices();
      if (liveVoices.length >= SE_MAX_TOTAL) {
        const old = liveVoices.shift();
        if (old) { releaseVoice(old.name, old.v); stats.sePreempt++; }
      }
      const v0 = Math.max(0, Math.min(1, vol));
      const src = ctx.createBufferSource();
      const g = ctx.createGain();
      src.buffer = buf;
      src.playbackRate.value = 1;
      g.gain.value = v0;
      let tail = g;
      if (ctx.createStereoPanner && pan) {
        const p = ctx.createStereoPanner();
        p.pan.value = Math.max(-1, Math.min(1, pan));
        g.connect(p);
        tail = p;
      }
      tail.connect(seGain);
      src.connect(g);
      const v = { src, gain: g, startedAt: ctx.currentTime, offset: 0, dead: false, name };
      src.onended = () => {
        try { src.disconnect(); } catch { }
        v.dead = true;
        const s = seVoices.get(name);
        if (s) { s.delete(v); if (!s.size) seVoices.delete(name); }
      };
      set.add(v);
      liveVoices.push({ name, v });
      src.start(0);
      seFrame.set(name, { frame: frameNo, voice: v, vol: v0 });
      stats.playSE++;
    };
    if (queuedBuf) start(queuedBuf);
    else loadBuffer(name).then(start);
  }

  /* ---------------- BGM ---------------- */

  function stopBGM(name) {
    bgmGen.set(name, (bgmGen.get(name) || 0) + 1);   // 让还在 decode 的那次 start 作废
    const st = streams.get(name);
    if (st) {
      st.stop();
      streams.delete(name);
      paused.delete(name);
      bgm.delete(name);
      stats.activeStreams = streams.size;
      return;
    }
    const v = bgm.get(name);
    if (!v) { paused.delete(name); return; }
    try { v.src.stop(); } catch { }
    try { v.src.disconnect(); } catch { }
    bgm.delete(name);
    paused.delete(name);
  }

  function playBGM(name, vol, position, queuedBuf, rate = 1) {
    const rec = res.get(name);
    if (!rec || !rec.path) { stats.fetchFail++; return; }
    stopBGM(name);
    const gen = bgmGen.get(name);
    /* 请求时刻的游戏帧。首播要 fetch（+ 旧路径的整首 decode）才出声，
       回来时游戏可能已经走了几十帧；不补偿的话整首 BGM 相对弹幕永远慢
       这一截，时非 / 地灵殿这种踩点卡就全错位。start 时把「已经过去的
       游戏时间」加进 offset。流式路径只剩 fetch + 首块解码，补偿量很小。 */
    const reqFrame = frameNo;
    const tReq = (typeof performance !== 'undefined' ? performance.now() : 0);
    /* 流式路径：读 8192 帧就能起播，不做整首解码。 */
    const streamStart = (bytes) => {
      if (!bytes || !ctx || !ready() || !running) {
        if (stats.bgmDrops.length < 12) {
          stats.bgmDrops.push(name + ':bytes=' + (!!bytes) + ',ctx=' + (!!ctx) +
            ',ready=' + ready() + ',running=' + running + ',stream');
        }
        return;
      }
      if (bgmGen.get(name) !== gen) return;
      const player = new OggStreamPlayer({
        Module, ctx, out: bgmGain, name,
        onEnd: (n) => {
          const cur = streams.get(n);
          if (cur === player) { streams.delete(n); bgm.delete(n); stats.activeStreams = streams.size; }
        },
        onError: (e) => {
          stats.bgmStreamFail++;
          if (stats.errors.length < 12) stats.errors.push('stream ' + name + ': ' + (e && e.message ? e.message : String(e)));
          const cur = streams.get(name);
          if (cur === player) { streams.delete(name); bgm.delete(name); stats.activeStreams = streams.size; }
        },
      });
      const range = loopRange.get(name);
      let offset = position > 0 ? position : 0;
      const lateFrames = frameNo - reqFrame;
      if (lateFrames > 0) offset += (lateFrames / 60);
      streams.set(name, player);
      /* bgm 表里放一条与旧路径同形的记录：BGM_VOLUME_ONE / SPEED / LOOP /
         playing 探针都靠它，不必为流式路径分叉。src 为 null 时按 stream 读。 */
      bgm.set(name, {
        stream: player, gain: player.gain, vol: Math.max(0, Math.min(1, vol)),
        rate, offset, loopStart: (range && range.start) || 0, loopEnd: (range && range.end) || 0,
      });
      player.play(bytes, {
        volume: vol, rate, offsetSeconds: offset, loop: true,
        loopStart: (range && range.start) || 0, loopEnd: (range && range.end) || 0,
      }).then((ok) => {
        if (!ok) return;
        stats.playBGM++;
        stats.bgmStream++;
        /* 用 player.gain 顶替原记录里的 gain（play 内部才知道节点）。 */
        const rec2 = bgm.get(name);
        if (rec2 && rec2.stream === player) rec2.gain = player.gain;
      }).catch((e) => {
        stats.bgmStreamFail++;
        if (stats.errors.length < 12) stats.errors.push('stream ' + name + ': ' + (e && e.message ? e.message : String(e)));
      });
      if (stats.bgmLog.length < 12) {
        stats.bgmLog.push({
          name, mode: 'stream',
          waitMs: Math.round((typeof performance !== 'undefined' ? performance.now() : 0) - tReq),
          lateFrames, offset: +offset.toFixed(2), queued: !!queuedBuf,
        });
      }
    };
    /* 旧路径：整首 decodeAudioData（非 OGG / 没有解码器 / 已解好的 queuedBuf）。 */
    const start = (buf) => {
      if (!buf || !ctx || !ready() || !running) {
        // 静默失败最难查：把丢弃原因记下来（#11 现场取证用）。
        if (stats.bgmDrops.length < 12) {
          stats.bgmDrops.push(name + ':buf=' + (!!buf) + ',ctx=' + (!!ctx) +
            ',ready=' + ready() + ',running=' + running);
        }
        return;
      }
      /* decode 是异步的：同名的第二次 PlayMusic 可能在第一次还没 start 时到达，
         旧的那次必须丢弃，否则同一首会叠两轨（听着像"回声"）。 */
      if (bgmGen.get(name) !== gen) return;
      const src = ctx.createBufferSource();
      const g = ctx.createGain();
      src.buffer = buf;
      try { src.playbackRate.value = rate; } catch { }
      g.gain.value = Math.max(0, Math.min(1, vol));
      const range = loopRange.get(name);
      if (range && range.end > 0 && range.end <= buf.duration) {
        src.loop = true;
        src.loopStart = range.start;
        src.loopEnd = range.end;
      } else {
        src.loop = true;
        src.loopStart = 0;
        src.loopEnd = buf.duration;
      }
      src.connect(g);
      g.connect(bgmGain);
      let offset = position > 0 ? position : 0;
      /* 迟到补偿：见上面 reqFrame 的注释。首播结束后游戏推进的帧数折成秒，
         直接跳过相应长度，让 BGM 重新对齐到「本应在的位置」。 */
      const lateFrames = frameNo - reqFrame;
      if (lateFrames > 0) offset += (lateFrames / 60) * rate;
      if (buf.duration > 0) offset = offset % buf.duration;
      if (offset < 0) offset = 0;
      const v = { src, gain: g, startedAt: ctx.currentTime, offset, rate,
        loop: true, loopStart: src.loopStart, loopEnd: src.loopEnd };
      bgm.set(name, v);
      src.start(0, offset);
      stats.playBGM++;
      // 首播延迟 = fetch + decode 的耗时；offset 就是「迟到补偿」跳过的那一截。
      if (stats.bgmLog.length < 12) {
        stats.bgmLog.push({
          name, mode: 'decode',
          waitMs: Math.round((typeof performance !== 'undefined' ? performance.now() : 0) - tReq),
          lateFrames,
          offset: +offset.toFixed(2),
          dur: +buf.duration.toFixed(1),
          queued: !!queuedBuf,
        });
      }
    };
    if (queuedBuf) { start(queuedBuf); return; }
    if (oggDecoderReady) {
      /* OGG 一律先取字节，闻一下 'OggS' 再决定流式还是退回整首解码。 */
      loadBytes(name).then((bytes) => {
        if (bgmGen.get(name) !== gen) return;
        if (isOggBytes(bytes)) { streamStart(bytes); return; }
        decodeBytes(name, bytes).then((buf) => {
          if (bgmGen.get(name) !== gen) return;
          start(buf);
        });
      });
      return;
    }
    loadBuffer(name).then(start);
  }

  /** WebAudio 的 BufferSource 不能真暂停，用「记录 offset → stop → 重开」模拟。 */
  function pauseBGM(name) {
    const st = streams.get(name);
    if (st) {
      const off = st.pause();
      paused.set(name, { off, vol: st.volume, rate: st.rate, stream: true });
      streams.delete(name);
      bgm.delete(name);
      stats.activeStreams = streams.size;
      return;
    }
    const v = bgm.get(name);
    if (!v || !ctx) return;
    const elapsed = (ctx.currentTime - v.startedAt) * v.rate;
    const off = v.offset + elapsed;
    paused.set(name, { off, vol: v.gain.gain.value, rate: v.rate });
    try { v.src.stop(); } catch { }
    try { v.src.disconnect(); } catch { }
    bgm.delete(name);
  }

  function resumeBGM(name) {
    const p = paused.get(name);
    if (!p || !ctx || !ready()) return;
    paused.delete(name);
    playBGM(name, p.vol, p.off, null, p.rate);
  }

  /* ---------------- 命令消费 ---------------- */

  function exec(op, name, a, b) {
    switch (op) {
      case OP.PLAY_SE: playSE(name, a, b); break;
      case OP.STOP_SE: stopSE(name); break;
      case OP.PAUSE_SE: {
        // SE 的暂停语义：游戏用它做「暂停菜单静音」，这里停掉活跃 voice，
        // resume 时按记录的偏移重开（与 BGM 一致）。
        const set = seVoices.get(name);
        if (set && ctx) {
          const items = [];
          for (const v of [...set]) {
            items.push({ off: v.offset + (ctx.currentTime - v.startedAt), vol: v.gain.gain.value });
            releaseVoice(name, v);
          }
          paused.set('se:' + name, items);
        }
        break;
      }
      case OP.RESUME_SE: {
        const items = paused.get('se:' + name);
        if (items && ctx && ready()) {
          paused.delete('se:' + name);
          for (const it of items) playSE(name, it.vol, 0);
        }
        break;
      }
      case OP.PLAY_BGM: playBGM(name, a, b); break;
      case OP.STOP_BGM: stopBGM(name); break;
      case OP.PAUSE_BGM: pauseBGM(name); break;
      case OP.RESUME_BGM: resumeBGM(name); break;
      case OP.SE_VOLUME:
        seVol = a; if (seGain) setGain(seGain.gain, seVol); break;
      case OP.BGM_VOLUME:
        bgmVol = a; if (bgmGain) setGain(bgmGain.gain, bgmVol); break;
      case OP.BGM_VOLUME_ONE: {
        const v = bgm.get(name);
        if (v && v.stream) v.stream.setVolume(a);
        else if (v && v.gain) setGain(v.gain.gain, a);
        break;
      }
      case OP.SE_VOLUME_ONE: {
        const set = seVoices.get(name);
        if (set) for (const v of set) setGain(v.gain.gain, a);
        break;
      }
      case OP.BGM_SPEED: {
        const v = bgm.get(name);
        if (v && v.stream) { v.stream.setRate(a); v.rate = a; }
        else if (v) { v.rate = a; try { v.src.playbackRate.value = a; } catch { } }
        break;
      }
      case OP.SE_SPEED: {
        const set = seVoices.get(name);
        if (set) for (const v of set) { try { v.src.playbackRate.value = a; } catch { } }
        break;
      }
      case OP.BGM_LOOP: {
        const v = bgm.get(name);
        if (v && v.stream) v.stream.setLoop(a);
        else if (v) v.src.loop = !!a;
        break;
      }
      case OP.LOOP_RANGE: {
        loopRange.set(name, { start: a, end: b });
        const v = bgm.get(name);
        if (v && v.stream) v.stream.setLoopRange(a, b);
        else if (v && v.src.buffer && b > 0 && b <= v.src.buffer.duration) {
          v.src.loopStart = a; v.src.loopEnd = b;
        }
        break;
      }
      default: break;
    }
  }

  /* 从 C 侧状态机把全局音量拉回来。启动期的 SetSEVolume/SetBGMVolume 命令
     会在 fastForward 的 flush 里被丢掉，JS 侧就会一直用默认 1.0——表现就是
     「进网页后音量跟设置里调的不一样，进一次设置才恢复」（bug9）。 */
  function syncVolumesFromHost() {
    let s = '';
    try { s = ccall('rgl_audio_state', 'string', [], []) || ''; } catch { return; }
    const mse = /seVol=([\d.]+)/.exec(s);
    const mbgm = /bgmVol=([\d.]+)/.exec(s);
    if (mse) {
      const v = +mse[1];
      if (Number.isFinite(v) && v !== seVol) {
        seVol = v;
        if (seGain) setGain(seGain.gain, v);
        stats.volSync++;
      }
    }
    if (mbgm) {
      const v = +mbgm[1];
      if (Number.isFinite(v) && v !== bgmVol) {
        bgmVol = v;
        if (bgmGain) setGain(bgmGain.gain, v);
        stats.volSync++;
      }
    }
  }

  /** 每帧调用一次：同步资源表 + 取命令 + 执行。 */
  function consume() {
    if (!running) { stats.droppedRunning++; return stats; }
    frameNo++;
    try { syncResources(); } catch (e) {
      if (stats.errors.length < 12) stats.errors.push('res: ' + e.message);
    }
    /* 首次「可以出声」时补播一次当前 BGM：启动快进把播放命令 flush 掉了，
       但 C 侧状态机仍认为在播。force 模式下没有手势也不会漏。
       同时把**音量**从 C 侧状态机拉回来（bug9）：启动链里
       `SetSEVolume(setting.sevolume/100)` 发的命令在解锁前到达，
       按「解锁前只保留状态类命令」的规则本该生效，但启动快进
       （rgl_host_audio_flush）会把整批命令直接丢掉，于是 JS 侧停在默认
       1.0，玩家必须进一次「设置」把音量重新下发才正常。这里不再依赖命令
       流，直接读 C 侧权威值。 */
    if (ready() && !didResync) {
      didResync = true;
      syncVolumesFromHost();
      try { resyncPlaying(); } catch (e) {
        if (stats.errors.length < 12) stats.errors.push('resync: ' + e.message);
      }
    }
    const csv = ccall('rgl_host_audio_take', 'string', [], []);
    if (!csv) return stats;
    if (!ready()) {
      // 解锁前丢弃，但音量/循环区间这类「状态」命令仍要执行，
      // 否则解锁后音量会停在默认值。
      for (const line of csv.split('\n')) {
        if (!line || line[0] === '#') continue;
        const f = line.split(',');
        const op = +f[0];
        if (op === OP.SE_VOLUME || op === OP.BGM_VOLUME || op === OP.LOOP_RANGE) {
          exec(op, f[1] || '', +f[2] || 0, +f[3] || 0);
        }
      }
      stats.droppedLocked += csv.split('\n').length - 1;
      return stats;
    }
    for (const line of csv.split('\n')) {
      if (!line || line[0] === '#') continue;
      const f = line.split(',');
      const op = +f[0];
      if (!op) continue;
      try {
        exec(op, f[1] || '', +f[2] || 0, +f[3] || 0);
        stats.cmds++;
      } catch (e) {
        if (stats.errors.length < 12) stats.errors.push('op' + op + ': ' + e.message);
      }
    }
    stats.activeSE = seVoices.size;
    stats.activeBGM = bgm.size;
    pruneVoices();
    stats.activeVoices = liveVoices.length;
    stats.bufMB = +((totalBytes / 1048576).toFixed(1));
    return stats;
  }

  return {
    consume,
    unlock,
    /* 快进（R.fastForward / ?stage= 跳转）会清掉攒下来的命令，但 C 侧状态机
       仍认为那几首 BGM 在播。以前只有「首次可播放」补播一次，之后任何一次
       flush 都会让 C/JS 永久失步——实测快进 600 帧后 playing=6 而 JS 一首没播。
       现在 flush 一并把 didResync 打回 false，下一帧 consume 会重新补播。 */
    flush() {
      ccall('rgl_host_audio_flush', null, [], []);
      didResync = false;
    },
    syncResources: () => syncResources(true),
    stopAll() {
      for (const n of [...seVoices.keys()]) stopSE(n);
      for (const n of [...bgm.keys()]) stopBGM(n);
      liveVoices.length = 0;
    },
    warm,
    get stats() { return stats; },
    /* 诊断：JS 侧当前生效的全局音量。与 C 侧 rgl_audio_state 的
       seVol/bgmVol 对照，是验证「开机音量是否等于设置值」（bug9）的唯一口径——
       AudioContext 的 gain.value 只有真机才读得准，无头环境读它没意义。 */
    get volumes() { return { se: seVol, bgm: bgmVol }; },
    get unlocked() { return unlocked; },
    get context() { return ctx; },
    /* 探针用：正在响的 BGM 明细（含真实循环区间与解码时长）。
       流式路径没有 AudioBuffer，dur 记 loopEnd 或不给。 */
    get playing() {
      const list = [];
      for (const [name, v] of bgm) {
        if (v.stream) {
          list.push({
            name, mode: 'stream', vol: +v.stream.volume.toFixed(3), rate: v.stream.rate,
            loopStart: +(v.stream.loopStartFrame / v.stream.sampleRate).toFixed(2),
            loopEnd: +(v.stream.loopEndFrame / v.stream.sampleRate).toFixed(2),
            dur: null, offset: +v.stream.positionSeconds.toFixed(2),
            blocks: v.stream.stats.blocks, underruns: v.stream.stats.underruns,
            firstBlockMs: v.stream.stats.firstBlockMs,
          });
          continue;
        }
        const b = v.src.buffer;
        list.push({
          name, vol: +v.gain.gain.value.toFixed(3), rate: v.rate,
          loopStart: +v.loopStart.toFixed(2), loopEnd: +v.loopEnd.toFixed(2),
          dur: b ? +b.duration.toFixed(2) : null, offset: +v.offset.toFixed(2),
        });
      }
      return list;
    },
    /* SE voice 计数：按名分组，total 是真实并发数（stats.activeSE 只是名字数） */
    get seVoices() {
      let total = 0; const per = {};
      for (const [name, set] of seVoices) { total += set.size; per[name] = set.size; }
      return { total, per };
    },
    get pausedList() { return [...paused.keys()]; },
    /* 诊断：C 侧音频状态机原文（bgmPlaying / 各曲 state 都在这）。真机复现 #11
       「卡里没音乐、esc 才有」时，把 host + bgmLog + bgmDrops + pausedList 一起贴出来：
       - host 里 bgmPlaying=0 而游戏认为在放 → C 状态机是 stopped/paused；
       - bgmDrops 非空 → 命令到了 JS 但被 start 的守卫丢掉；
       - 两者都正常且 bgmLog 有记录 → 是解码太慢（看 waitMs）。 */
    get host() { return ccall('rgl_audio_state', 'string', [], []); },
    get resources() { return res; },
    get buffers() { return buffers; },
    set onUnlockHook(fn) { onUnlock = fn; },
    set enabled(v) {
      running = !!v;
      if (!running) this.stopAll();
    },
    get enabled() { return running; },
  };
}
