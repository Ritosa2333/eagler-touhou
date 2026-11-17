import {
  encodeTh19Replay,
  decodeTh19Replay,
  parseTh19StreamPacket,
  TH19_FRAME_BYTES,
  TH19_RECORDING_LIMITS,
  validTh19RecordingConfig,
} from "./th19-recording.mjs";

// Immutable confirmed input, independently consumed by a file player or observer.
// Native references: 0x550130/0x430340 input update; 0x550750 sequential replay;
// 0x550dd0 restores initial state/RNG before consuming the first input.
export class ConfirmedTimeline {
  constructor(config) {
    if (!validTh19RecordingConfig(config)) throw Error("录像开局配置无效");
    this.config = structuredClone(config);
    this.rows = [];
    this.played = false;
    this.terminal = false;
    this.ended = false;
  }
  get length() {
    return this.rows.length;
  }
  append(bytes) {
    const row = parseTh19StreamPacket(bytes);
    if (
      this.ended ||
      this.terminal ||
      row.kind !== "frame" ||
      row.frame !== this.length
    )
      throw Error("录像确认帧不连续");
    if (this.length >= TH19_RECORDING_LIMITS.frames)
      throw Error("录像帧数超限");
    this.rows.push(bytes.slice());
    this.played ||= row.played;
    this.terminal = row.result === 2;
    return row;
  }
  end(frame = this.length) {
    if (frame !== this.length) throw Error("录像结束帧不一致");
    this.ended = true;
  }
  row(frame) {
    if (!Number.isSafeInteger(frame) || frame < 0 || frame >= this.length)
      throw Error("录像位置无效");
    return parseTh19StreamPacket(this.rows[frame]);
  }
  payload() {
    const out = new Uint8Array(this.length * TH19_FRAME_BYTES);
    this.rows.forEach((r, i) => out.set(r, i * TH19_FRAME_BYTES));
    return out;
  }
}

// No device sampling and no speculative frames enter this runner. Rewinding
// always resets the match and seed, then consumes the same verified input prefix.
export class VersusReplayRunner {
  constructor(core, timeline) {
    this.core = core;
    this.timeline = timeline;
    this.frame = 0;
    this.failed = false;
  }
  reset() {
    const c = this.timeline.config,
      core = this.core;
    core._th19_loop_stop?.();
    core._th19_network_end();
    if (
      !core._th19_network_set_rules(...c.rules) ||
      !core._th19_network_begin(c.seed, c.difficulty, ...c.characters)
    )
      throw Error("无法恢复录像开局");
    this.frame = 0;
    this.failed = false;
  }
  step(present = false, { silent = false } = {}) {
    if (this.failed) throw Error("录像重演已停止");
    const r = this.timeline.row(this.frame),
      a = r.left,
      b = r.right,
      core = this.core;
    if (silent) core._th19_rollback_effect_begin?.();
    try {
      const result = core._th19_network_step(
        this.frame,
        a.keys,
        b.keys,
        a.x,
        a.y,
        b.x,
        b.y,
        a.direct,
        b.direct,
        +present,
      );
      if (
        result !== r.result ||
        (r.hash !== null && core._th19_network_hash() >>> 0 !== r.hash)
      )
        throw Error(`录像状态校验失败（第 ${this.frame + 1} 帧）`);
      ++this.frame;
      return r;
    } catch (error) {
      this.failed = true;
      throw error;
    } finally {
      if (silent) core._th19_rollback_effect_end?.();
    }
  }
  seek(target) {
    if (
      !Number.isSafeInteger(target) ||
      target < 0 ||
      target > this.timeline.length
    )
      throw Error("录像位置无效");
    if (target < this.frame || this.failed) this.reset();
    return target;
  }
}

export function createVersusRecorder({
  build,
  names = [],
  write,
  notice = () => {},
}) {
  let current = null;
  function save(session, partial) {
    if (!session || !session.tape.played || !session.tape.length)
      return Promise.resolve();
    const frames = session.tape.length,
      payload = session.tape.payload();
    session.pending = session.pending
      .catch(() => {})
      .then(async () => {
        if (session.savedFrames === frames && session.savedPartial === partial)
          return;
        const bytes = encodeTh19Replay(
          {
            kind: "versus",
            build,
            config: session.tape.config,
            seed: session.tape.config.seed,
            frames,
            startedAt: session.startedAt,
            partial,
            names: session.names,
          },
          payload,
        );
        const path = await write(bytes, session.path);
        session.path = path;
        session.savedFrames = frames;
        session.savedPartial = partial;
      });
    return session.pending;
  }
  return {
    begin(config) {
      if (config.build !== build) throw Error("录像构建不一致");
      if (current && !current.tape.ended) throw Error("上一场录像尚未结束");
      current = {
        tape: new ConfirmedTimeline(config),
        startedAt: Date.now(),
        names: [0, 1].map((i) => String(names[i] || `P${i + 1}`).slice(0, 64)),
        pending: Promise.resolve(),
        path: null,
        savedFrames: 0,
        savedPartial: null,
        final: null,
      };
    },
    frame(bytes) {
      if (current) current.tape.append(bytes);
    },
    snapshot(partial = true) {
      return save(current, current?.tape.ended ? current.partial : partial);
    },
    end({ partial = false } = {}) {
      const s = current;
      if (!s) return Promise.resolve();
      if (s.final) return s.final;
      s.tape.end();
      s.partial = partial;
      s.final = save(s, partial)
        .then(() => {
          if (s.savedFrames) notice("对局录像已保存");
        })
        .catch((error) => {
          s.final = null;
          throw error;
        });
      return s.final;
    },
  };
}

function describeMetadata(m) {
  return {
    name: m.names?.join(" vs ") || "TH19",
    timestamp: Math.floor(m.startedAt / 1000),
    date: new Date(m.startedAt).toLocaleString(),
    character: m.config?.characters[0] ?? m.character ?? 0,
    difficulty: m.config?.difficulty ?? m.difficulty ?? 1,
    mode: m.kind === "versus" ? "Versus" : "Single",
    stages: [
      {
        id: 0,
        label: m.kind === "versus" ? "对战录像" : "旧版单机录像",
        score: null,
        frames: m.frames,
      },
    ],
  };
}
export function describeReplay(bytes) {
  return describeMetadata(decodeTh19Replay(bytes).meta);
}

export function createReplayPlayer({
  getCore,
  restartNormal,
  build,
  onState = () => {},
  onError = () => {},
  onFrame = () => {},
}) {
  let runner = null,
    description = null,
    path = "",
    paused = false,
    rate = 1,
    target = null,
    raf = 0,
    last = null,
    credit = 0,
    closed = false,
    lastReport = 0;
  const state = () => ({
    kind: "replay",
    path,
    frame: runner?.frame || 0,
    total: runner?.timeline.length || 0,
    start: 0,
    paused,
    seeking: target !== null,
    target,
    rate,
    ended: !!runner && runner.frame >= runner.timeline.length,
    canRewind: true,
    fileRelocation: 1,
    roundStarts: [0],
    stage: 0,
    difficulty: description?.difficulty ?? 1,
    left: runner?.timeline.config.characters[0] ?? 0,
    right: runner?.timeline.config.characters[1] ?? 0,
    description,
  });
  const report = () => onState(state());
  const schedule = () => {
    if (!raf && !closed && runner) raf = requestAnimationFrame(draw);
  };
  function draw(time) {
    raf = 0;
    if (closed || !runner) return;
    try {
      const delta =
        last === null ? 0 : Math.min(0.25, Math.max(0, (time - last) / 1000));
      last = time;
      if (!document.hidden && (!paused || target !== null)) {
        credit = Math.min(240, credit + delta * 60 * rate);
        const end = target ?? runner.timeline.length,
          count = target !== null || rate === 0 ? 4096 : Math.floor(credit),
          start = performance.now();
        let done = 0;
        while (
          done < count &&
          runner.frame < end &&
          (done === 0 || performance.now() - start < 8)
        ) {
          runner.step(
            done === 0 || done === count - 1 || runner.frame + 1 === end,
            { silent: target !== null || rate === 0 },
          );
          ++done;
          onFrame();
        }
        credit = Math.max(0, credit - done);
        if (target !== null && runner.frame >= target) {
          target = null;
          credit = 0;
        }
      }
      if (time - lastReport >= 100) {
        lastReport = time;
        report();
      }
      schedule();
    } catch (error) {
      paused = true;
      target = null;
      report();
      onError(error);
    }
  }
  return {
    get active() {
      return !!runner && !closed;
    },
    state,
    relocate(source, next) {
      if (path !== source) throw Error("录像路径已变化");
      path = next;
      report();
      return state();
    },
    async open(nextPath, bytes) {
      const recording = decodeTh19Replay(bytes),
        m = recording.meta;
      if (m.build !== build)
        throw Error("录像来自不同游戏构建，请使用录制时的版本");
      let next;
      if (m.kind === "versus") {
        const tape = new ConfirmedTimeline(m.config);
        for (let i = 0; i < m.frames; i++)
          tape.append(
            recording.payload.subarray(
              i * TH19_FRAME_BYTES,
              (i + 1) * TH19_FRAME_BYTES,
            ),
          );
        tape.end();
        next = new VersusReplayRunner(getCore(), tape);
      } else {
        // Read existing single-player files without enabling single-player recording.
        next = {
          frame: 0,
          timeline: {
            length: m.frames,
            config: { characters: [m.character ?? 0, 0] },
          },
          async reset() {
            await restartNormal(m);
            const core = getCore(),
              buffer = core._th19_replay_buffer(recording.payload.length);
            if (!buffer) throw Error("录像输入缓冲区不足");
            core.HEAPU8.set(recording.payload, buffer);
            if (!core._th19_replay_load(recording.payload.length))
              throw Error("旧版录像输入无效");
            this.frame = 0;
          },
          step(present) {
            if (!getCore()._th19_replay_step(+present))
              throw Error("旧版录像提前结束");
            ++this.frame;
          },
          async seek(value) {
            if (!Number.isSafeInteger(value) || value < 0 || value > m.frames)
              throw Error("录像位置无效");
            if (value < this.frame) await this.reset();
            return value;
          },
        };
      }
      await next.reset();
      runner = next;
      description = describeMetadata(m);
      path = nextPath;
      paused = false;
      target = null;
      closed = false;
      last = null;
      credit = 0;
      schedule();
      report();
      return state();
    },
    async control(action, value) {
      if (!runner || closed) throw Error("请先选择录像");
      if (action === "pause") paused = true;
      else if (action === "play") {
        if (runner.frame >= runner.timeline.length) await runner.reset();
        paused = false;
      } else if (action === "rate") {
        if (![0, 0.5, 1, 2, 4, 8].includes(value)) throw Error("播放速度无效");
        rate = value;
      } else if (action === "seek") {
        target = await runner.seek(value);
        if (target === runner.frame) target = null;
      } else throw Error("不支持的播放操作");
      last = null;
      credit = 0;
      schedule();
      report();
      return state();
    },
    close() {
      closed = true;
      cancelAnimationFrame(raf);
      raf = 0;
      runner = null;
    },
  };
}
