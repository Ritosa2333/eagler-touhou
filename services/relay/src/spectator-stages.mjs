// Each run retains its E?SV version while published v1 and v2 clients coexist.
export const COOPERATIVE_SPECTATOR_MARKERS = Object.freeze({
  th06mp: 0x36, th07mp: 0x37, th08mp: 0x38, th10mp: 0x41, th11mp: 0x42, th12mp: 0x43, th15mp: 0x46,
});
const markers = new Set(Object.values(COOPERATIVE_SPECTATOR_MARKERS));
export const SPECTATOR_STAGE_LIMITS = Object.freeze({ stateBytes: 4 * 1024, historyBytes: 16 * 1024 * 1024, historyFrames: 108000 });

export function parseSpectatorStageEvent(payload, marker, count) {
  if (!markers.has(marker) || !(payload instanceof Uint8Array) || payload.length < 32 || payload.length > 40 + (payload[4] === 1 ? 60 * 1024 : SPECTATOR_STAGE_LIMITS.stateBytes) ||
      payload[0] !== 69 || payload[1] !== marker || payload[2] !== 83 || payload[3] !== 86 ||
      ![1, 2].includes(payload[4]) || payload[6] !== count || ![2, 3].includes(count) || payload[7]) return null;
  const v = new DataView(payload.buffer, payload.byteOffset, payload.byteLength);
  const event = {version: payload[4], kind: payload[5], session: v.getBigUint64(8, true), abi: v.getUint32(16, true),
    frame: v.getUint32(20, true), stage: v.getUint32(24, true), serial: v.getUint32(28, true)};
  if (!event.session || !event.abi || event.frame === 0xffffffff || event.stage > 65535 || !event.serial) return null;
  if (event.kind === 1) {
    if (payload.length < 41 || !v.getUint32(32, true) || v.getUint32(36, true) !== payload.length - 40) return null;
    event.schema = v.getUint32(32, true);
  } else if (event.kind === 2) {
    if (payload.length !== 40 || !v.getUint32(32, true) || payload[38] || payload[39] ||
        !(payload[36] === 1 && !payload[37] || payload[36] === 2 && payload[37])) return null;
    event.checkpoint = v.getUint32(32, true); event.node = payload[36]; event.bossSlots = payload[37];
  } else if (![3, 4].includes(event.kind) || payload.length !== 32) return null;
  return event;
}

export class SpectatorStageRelay {
  constructor(marker, count, limits = SPECTATOR_STAGE_LIMITS) {
    if (!markers.has(marker) || ![2, 3].includes(count)) throw Error('unsupported cooperative spectator identity');
    this.marker = marker; this.count = count; this.limits = limits;
    this.history = []; this.bytes = 0; this.frames = 0;
    this.session = 0n; this.version = 0; this.abi = this.next = this.stage = this.serial = this.checkpoint = 0;
    this.timing = null; this.ended = false;
  }
  accept(payload) {
    if (this.ended) throw Error('spectator stage stream already ended');
    if (payload[2] === 84 && payload[3] === 77) return this.acceptTiming(payload);
    const event = parseSpectatorStageEvent(payload, this.marker, this.count);
    if (event) {
      if (event.kind === 4) throw Error('spectator bootstrap is relay-owned');
      if (this.session && (event.version !== this.version || event.session !== this.session || event.abi !== this.abi ||
          event.frame !== this.next || event.serial <= this.serial ||
          event.kind !== 1 && event.stage !== this.stage)) throw Error('spectator stage identity or order mismatch');
      if (!this.session && event.kind !== 1) throw Error('spectator stage initializer missing');
      if (event.kind === 1) {
        if (this.session && event.frame <= this.checkpointFrame) throw Error('spectator stage did not advance');
        if (this.timing) {
          const timing = new DataView(this.timing.buffer, this.timing.byteOffset, this.timing.byteLength);
          if (timing.getBigUint64(12, true) !== event.session || timing.getUint32(20, true) !== event.abi)
            throw Error('spectator stage timing mismatch');
        }
        // Validate the complete replacement before retiring the old prefix.
        const replacement = this.timing ? [owned(this.timing), owned(payload)] : [owned(payload)];
        const bytes = replacement.reduce((sum, row) => sum + row.length, 0);
        if (bytes > this.limits.historyBytes) throw Error('spectator stage history limit');
        this.history = replacement; this.bytes = bytes; this.frames = 0;
        this.session = event.session; this.version = event.version; this.abi = event.abi; this.next = event.frame;
        this.stage = event.stage; this.serial = this.checkpoint = event.serial;
        this.checkpointFrame = event.frame;
        return 'checkpoint';
      }
      if (event.kind === 2 && event.checkpoint !== this.checkpoint) throw Error('spectator node dependency mismatch');
      this.append(payload); this.serial = event.serial;
      if (event.kind === 3) this.ended = true;
      return event.kind === 2 ? 'node' : 'end';
    }
    if (!this.session || payload.length !== 24 + this.count * 12 || payload[0] !== 69 ||
        payload[1] !== this.marker || payload[2] !== 78 || payload[3] !== 80 || payload[4] !== 4 ||
        payload[5] !== 3 || payload[6] !== this.count || payload[7]) throw Error('invalid spectator stage payload');
    const input = new DataView(payload.buffer, payload.byteOffset, payload.byteLength);
    if (input.getBigUint64(8, true) !== this.session || input.getUint32(20, true) !== this.abi ||
        input.getUint32(16, true) !== this.next || this.next === 0xffffffff) throw Error('spectator input identity or gap');
    for (let player = 0; player < this.count; ++player) {
      const at = 24 + player * 12;
      if (payload[at + 2] > 4 || payload[at + 3] > 7 ||
          !Number.isFinite(input.getFloat32(at + 4, true)) || !Number.isFinite(input.getFloat32(at + 8, true)))
        throw Error('invalid spectator movement');
    }
    if (this.frames >= this.limits.historyFrames) throw Error('spectator stage frame limit');
    this.append(payload); ++this.next; ++this.frames;
    return 'input';
  }
  acceptTiming(payload) {
    if (payload.length !== 40 || payload[0] !== 69 || payload[1] !== this.marker || payload[4] !== 1)
      throw Error('invalid spectator stage timing');
    const v = new DataView(payload.buffer, payload.byteOffset, payload.byteLength);
    if (this.session && (v.getBigUint64(12, true) !== this.session || v.getUint32(20, true) !== this.abi))
      throw Error('spectator stage timing identity');
    if (this.timing) {
      if (!this.timing.equals(Buffer.from(payload))) throw Error('spectator stage timing changed');
      return 'duplicate-timing';
    }
    if (this.session) throw Error('spectator stage timing arrived after initializer');
    this.timing = owned(payload); return 'timing';
  }
  append(payload) {
    if (payload.length > this.limits.historyBytes - this.bytes) throw Error('spectator stage byte limit');
    this.history.push(owned(payload)); this.bytes += payload.length;
  }
  bootstrap() {
    if (!this.session) throw Error('spectator initializer missing');
    const bytes = Buffer.alloc(32);
    bytes.set([69, this.marker, 83, 86, this.version, 4, this.count, 0]);
    bytes.writeBigUInt64LE(this.session, 8); bytes.writeUInt32LE(this.abi, 16);
    bytes.writeUInt32LE(this.next, 20); bytes.writeUInt32LE(this.stage, 24); bytes.writeUInt32LE(this.serial, 28);
    return bytes;
  }
}

function owned(payload) {
  const copy = Buffer.allocUnsafeSlow(payload.length); copy.set(payload); return copy;
}
