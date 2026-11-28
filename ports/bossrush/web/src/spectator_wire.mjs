/**
 * BRO 观战流（阶段 A：实时观战）—— 帧编码/解码
 *
 * 设计依据（eagler-touhou 仓库）：
 *  - `services/relay/src/spectator-frame.mjs` 的通用协作分支认「E?NP」帧：
 *    `payload.length === 24 + playerCount * 12`、`[0]===0x45('E')`、`[1]===作品协议字节`、
 *    `[2..3]==='NP'`、`[4]===4`、`[5]===3`、`[6]===playerCount`、`[7]===0`。
 *  - `services/relay/src/spectator-stages.mjs` 的 `COOPERATIVE_SPECTATOR_MARKERS` 存放
 *    各作品的协议字节（th05mp 0x35 … th17mp 0x47），BRO 用 0x52。
 *  - `handleSpectatorConnection`（netplay-relay.mjs:2823）会先按序回放
 *    `run.spectatorHistory` 再转 live，所以观战端只要按到达顺序解帧即可。
 *
 * 为什么观战端不需要「开局状态包」：launcher 会给同一局的每个 runtime 下发同一份
 * `netplay*` 配置（seed / stageQueue / loadouts / systems），观战端用同样配置
 * 起同一场确定性对局，只缺「每帧输入」——正好由本流的 INPUT 帧逐步供给。
 *
 * 帧布局（小端）：
 *   INPUT：24 字节头 + playerCount × 12 字节
 *     [0..7]  0x45, 0x52, 'N', 'P', 4, 3, playerCount, 0
 *     [8..11] frame        u32  逻辑帧号
 *     [12..15] epoch       u32  对局代号（host 自增，换关重开时区分）
 *     [16..19] keysMask    u32  该帧有输入的玩家位图（bit i = 玩家 i）
 *     [20..23] reserved    0
 *     每个玩家记录：[0] keys  [1] flags  [2..3] 0  [4..7] x f32  [8..11] y f32
 *       flags: bit0 在场、bit1 死亡、bit2 该玩家在本端是本地玩家（仅调试展示）
 *   BEGIN：24 字节头 + 8 字节
 *     [0..7] 0x45, 0x52, 'N', 'B', 4, 1, playerCount, 0
 *     [8..11] epoch  u32；[12..15] seed u32；[16..19] playerCount；[20..23] 0
 *     [24..27] stageCount u32；[28..31] reserved
 */

export const SPEC_MAGIC_0 = 0x45; // 'E'
export const SPEC_MARKER = 0x52; // 'R'：bromp 的协议字节
export const SPEC_KIND_INPUT = 3;
export const SPEC_KIND_BEGIN = 1;
export const SPEC_INFO_LEN = 24;
export const SPEC_REC_LEN = 12;

export const SPEC_FLAG_PRESENT = 1;
export const SPEC_FLAG_DEAD = 2;
export const SPEC_FLAG_LOCAL = 4;

function checkCount(playerCount) {
  return Number.isInteger(playerCount) && playerCount >= 1 && playerCount <= 4;
}

/** 打包一帧输入。rows[i] = {keys, flags, x, y}（缺省按 0 处理）。 */
export function packSpecInput(frame, epoch, playerCount, rows) {
  if (!checkCount(playerCount)) throw new Error('invalid playerCount');
  const out = new Uint8Array(SPEC_INFO_LEN + playerCount * SPEC_REC_LEN);
  const v = new DataView(out.buffer);
  out[0] = SPEC_MAGIC_0;
  out[1] = SPEC_MARKER;
  out[2] = 0x4e; // 'N'
  out[3] = 0x50; // 'P'
  out[4] = 4;
  out[5] = SPEC_KIND_INPUT;
  out[6] = playerCount;
  out[7] = 0;
  v.setUint32(8, frame >>> 0, true);
  v.setUint32(12, epoch >>> 0, true);
  let mask = 0;
  for (let i = 0; i < playerCount; i++) if (rows && rows[i]) mask |= 1 << i;
  v.setUint32(16, mask >>> 0, true);
  v.setUint32(20, 0, true);
  for (let i = 0; i < playerCount; i++) {
    const r = (rows && rows[i]) || null;
    const at = SPEC_INFO_LEN + i * SPEC_REC_LEN;
    out[at] = r ? (r.keys | 0) & 0xff : 0;
    out[at + 1] = r ? (r.flags | 0) & 0xff : 0;
    v.setFloat32(at + 4, r && Number.isFinite(r.x) ? r.x : 0, true);
    v.setFloat32(at + 8, r && Number.isFinite(r.y) ? r.y : 0, true);
  }
  return out;
}

/** 打包对局开始帧（仅供 relay 开闸 / 观战端识别代号）。 */
export function packSpecBegin(epoch, seed, playerCount, stageCount) {
  if (!checkCount(playerCount)) throw new Error('invalid playerCount');
  const out = new Uint8Array(SPEC_INFO_LEN + 8);
  const v = new DataView(out.buffer);
  out[0] = SPEC_MAGIC_0;
  out[1] = SPEC_MARKER;
  out[2] = 0x4e;
  out[3] = 0x42; // 'B'
  out[4] = 4;
  out[5] = SPEC_KIND_BEGIN;
  out[6] = playerCount;
  out[7] = 0;
  v.setUint32(8, epoch >>> 0, true);
  v.setUint32(12, seed >>> 0, true);
  v.setUint32(16, playerCount, true);
  v.setUint32(20, 0, true);
  v.setUint32(24, stageCount >>> 0, true);
  v.setUint32(28, 0, true);
  return out;
}

/** 解一帧观战流；不是本流格式或长度非法时返回 null（relay/观战端都要能容忍别的流量）。 */
export function unpackSpec(bytes) {
  if (!bytes || bytes.length < SPEC_INFO_LEN) return null;
  if (bytes[0] !== SPEC_MAGIC_0 || bytes[1] !== SPEC_MARKER || bytes[2] !== 0x4e) return null;
  const v = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  const playerCount = bytes[6];
  if (!checkCount(playerCount) || bytes[4] !== 4 || bytes[7] !== 0) return null;
  const kind = bytes[5];
  if (kind === SPEC_KIND_BEGIN) {
    if (bytes[3] !== 0x42 || bytes.length !== SPEC_INFO_LEN + 8) return null;
    return {
      kind,
      epoch: v.getUint32(8, true),
      seed: v.getUint32(12, true),
      playerCount,
      stageCount: v.getUint32(24, true),
    };
  }
  if (kind !== SPEC_KIND_INPUT || bytes[3] !== 0x50) return null;
  if (bytes.length !== SPEC_INFO_LEN + playerCount * SPEC_REC_LEN) return null;
  const rows = new Array(playerCount);
  for (let i = 0; i < playerCount; i++) {
    const at = SPEC_INFO_LEN + i * SPEC_REC_LEN;
    rows[i] = {
      keys: bytes[at],
      flags: bytes[at + 1],
      x: v.getFloat32(at + 4, true),
      y: v.getFloat32(at + 8, true),
    };
  }
  return {
    kind,
    frame: v.getUint32(8, true),
    epoch: v.getUint32(12, true),
    keysMask: v.getUint32(16, true),
    playerCount,
    rows,
  };
}

/**
 * 观战流的「按帧缓冲」：relay 先回放历史再转 live，可能一次收到很多帧，
 * 观战端按自己的逻辑帧节奏逐帧消费；重复帧直接忽略，跳帧则记录下来。
 */
export class SpectatorFrameBuffer {
  constructor() {
    this.pending = new Map();
    this.epoch = null;
    this.received = 0;
    this.duplicated = 0;
    this.lastFrame = -1;
    this.gaps = 0;
  }

  /** 收一帧（字节）。返回解出的对象或 null。 */
  push(bytes) {
    const msg = unpackSpec(bytes);
    if (!msg || msg.kind !== SPEC_KIND_INPUT) return msg || null;
    if (this.epoch === null) this.epoch = msg.epoch;
    else if (msg.epoch !== this.epoch) {
      this.epoch = msg.epoch;
      this.pending.clear();
      this.lastFrame = -1;
    }
    if (msg.frame <= this.lastFrame || this.pending.has(msg.frame)) {
      this.duplicated++;
      return msg;
    }
    this.received++;
    this.pending.set(msg.frame, msg);
    return msg;
  }

  /** 取下一帧；还没有就返回 null（观战端此时等待，不要推进逻辑）。 */
  next() {
    const frame = this.lastFrame + 1;
    const msg = this.pending.get(frame);
    if (!msg) {
      if (this.pending.size > 0) this.gaps++;
      return null;
    }
    this.pending.delete(frame);
    this.lastFrame = frame;
    return msg;
  }

  get buffered() {
    return this.pending.size;
  }
}
