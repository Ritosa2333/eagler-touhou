// TH19's web recording is an input/seed replay, shared with its observer lane.
// Files use .rpy; the versioned header identifies this web input extension.
export const TH19_RECORDING_LIMITS = Object.freeze({
  frames: 216000,
  fileBytes: 32 * 1024 * 1024,
  headerBytes: 512 * 1024,
});
export const TH19_FRAME_BYTES = 44;
const streamMagic = [84, 49, 57, 83, 1],
  fileMagic = [69, 84, 49, 57, 82, 80, 1, 0];
const integer = (n, low, high) => Number.isInteger(n) && n >= low && n <= high;
export function validTh19RecordingConfig(c) {
  const v = c?.rules;
  return (
    !!c &&
    c.protocol === 1 &&
    /^[a-f0-9]{64}$/.test(c.build) &&
    integer(c.seed, 1, 0xffffffff) &&
    integer(c.difficulty, 0, 3) &&
    Array.isArray(c.characters) &&
    c.characters.length === 2 &&
    c.characters.every((n) => integer(n, 0, 18)) &&
    integer(c.delay, 0, 8) &&
    typeof c.rollback === "boolean" &&
    Array.isArray(v) &&
    v.length === 9 &&
    v.every((n, i) => integer(n, 0, [6, 2, 3, 4, 4, 3, 3, 38, 38][i])) &&
    v[0] === 0 &&
    v[3] === v[4] &&
    v[5] === 1 &&
    v[6] === 1 &&
    (v[2] !== 2 || (v[7] < 2 && v[8] < 2))
  );
}
export function validTh19RecordedInput(i) {
  return (
    !!i &&
    integer(i.keys, 0, 2047) &&
    integer(i.direct, 0, 1) &&
    Number.isFinite(i.x) &&
    Number.isFinite(i.y) &&
    Math.abs(i.x) <= 512 &&
    Math.abs(i.y) <= 512
  );
}
function packet(type, size, frame = 0) {
  const b = new Uint8Array(size);
  b.set(streamMagic);
  b[5] = type;
  b[6] = 2;
  new DataView(b.buffer).setUint32(8, frame, true);
  return b;
}
export function th19StreamBegin(config) {
  if (!validTh19RecordingConfig(config)) throw Error("兽王园录像配置无效");
  const json = new TextEncoder().encode(JSON.stringify(config));
  if (json.length > 2048) throw Error("兽王园录像配置过大");
  const b = packet(1, 12 + json.length);
  b.set(json, 12);
  return b;
}
export function th19StreamFrame(
  frame,
  left,
  right,
  { hash = null, result = 1, played = false } = {},
) {
  if (
    !integer(frame, 0, TH19_RECORDING_LIMITS.frames - 1) ||
    !validTh19RecordedInput(left) ||
    !validTh19RecordedInput(right) ||
    ![1, 2].includes(result) ||
    (hash !== null && !integer(hash, 0, 0xffffffff))
  )
    throw Error("兽王园录像输入无效");
  const b = packet(2, TH19_FRAME_BYTES, frame),
    v = new DataView(b.buffer);
  b[7] = +(hash !== null) | (played ? 2 : 0);
  for (const [n, i] of [left, right].entries()) {
    const p = 12 + n * 12;
    v.setUint16(p, i.keys, true);
    v.setUint16(p + 2, i.direct, true);
    v.setFloat32(p + 4, i.x, true);
    v.setFloat32(p + 8, i.y, true);
  }
  v.setUint32(36, hash ?? 0, true);
  v.setUint32(40, result, true);
  return b;
}
export function th19StreamEnd(frame, winner = -1) {
  if (
    !integer(frame, 1, TH19_RECORDING_LIMITS.frames) ||
    !integer(winner, -1, 1)
  )
    throw Error("录像结束帧无效");
  const b = packet(3, 16, frame);
  new DataView(b.buffer).setInt32(12, winner, true);
  return b;
}
export function parseTh19StreamPacket(b) {
  if (
    !(b instanceof Uint8Array) ||
    b.length < 12 ||
    b.length > 2060 ||
    streamMagic.some((n, i) => b[i] !== n) ||
    b[6] !== 2 ||
    b[7] > 3
  )
    throw Error("兽王园旁观数据格式错误");
  const v = new DataView(b.buffer, b.byteOffset, b.byteLength),
    frame = v.getUint32(8, true);
  if (frame > TH19_RECORDING_LIMITS.frames) throw Error("兽王园旁观帧超出范围");
  if (b[5] === 1) {
    if (frame || b[7]) throw Error("旁观初始化帧无效");
    const config = JSON.parse(
      new TextDecoder("utf-8", { fatal: true }).decode(b.subarray(12)),
    );
    if (!validTh19RecordingConfig(config)) throw Error("旁观对局配置无效");
    return { kind: "begin", config, frame };
  }
  if (
    b[5] === 2 &&
    b.length === TH19_FRAME_BYTES &&
    frame < TH19_RECORDING_LIMITS.frames
  ) {
    const inputs = [0, 1].map((n) => {
      const p = 12 + n * 12;
      return {
        keys: v.getUint16(p, true),
        direct: v.getUint16(p + 2, true),
        x: v.getFloat32(p + 4, true),
        y: v.getFloat32(p + 8, true),
      };
    });
    const result = v.getUint32(40, true);
    if (!inputs.every(validTh19RecordedInput) || ![1, 2].includes(result))
      throw Error("旁观输入帧无效");
    return {
      kind: "frame",
      frame,
      left: inputs[0],
      right: inputs[1],
      result,
      hash: b[7] & 1 ? v.getUint32(36, true) : null,
      played: !!(b[7] & 2),
    };
  }
  if (b[5] === 3 && b.length === 16 && frame > 0 && !b[7]) {
    const winner = v.getInt32(12, true);
    if (!integer(winner, -1, 1)) throw Error("旁观结束结果无效");
    return { kind: "end", frame, winner };
  }
  throw Error("不支持的兽王园旁观消息");
}
function crc32(bytes) {
  let crc = 0xffffffff;
  for (const byte of bytes) {
    crc ^= byte;
    for (let bit = 0; bit < 8; bit++)
      crc = (crc >>> 1) ^ (crc & 1 ? 0xedb88320 : 0);
  }
  return (crc ^ 0xffffffff) >>> 0;
}
function decodeLegacyReplay(b) {
  if (
    !(b instanceof Uint8Array) ||
    b.length < 22 ||
    b.length > TH19_RECORDING_LIMITS.fileBytes ||
    fileMagic.some((n, i) => b[i] !== n)
  )
    throw Error("不是有效的兽王园扩展录像");
  const v = new DataView(b.buffer, b.byteOffset, b.byteLength),
    h = v.getUint32(8, true),
    size = v.getUint32(12, true);
  if (
    !h ||
    h > TH19_RECORDING_LIMITS.headerBytes ||
    !size ||
    20 + h + size !== b.length ||
    v.getUint32(b.length - 4, true) !== crc32(b.subarray(0, b.length - 4))
  )
    throw Error("录像文件不完整或校验失败");
  const meta = JSON.parse(
      new TextDecoder("utf-8", { fatal: true }).decode(b.subarray(16, 16 + h)),
    ),
    payload = b.subarray(16 + h, b.length - 4);
  return validateLegacyPayload(meta, payload);
}
function validateLegacyPayload(meta, payload) {
  if (
    meta.schema !== "eagler-touhou/th19-replay/1" ||
    meta.game !== "th19" ||
    !["normal", "versus"].includes(meta.kind) ||
    !/^[a-f0-9]{64}$/.test(meta.build) ||
    !integer(meta.seed, 1, 0xffffffff) ||
    !integer(meta.frames, 1, TH19_RECORDING_LIMITS.frames) ||
    !Number.isSafeInteger(meta.startedAt) ||
    meta.startedAt < 0 ||
    meta.startedAt > 8.64e15
  )
    throw Error("录像信息无效");
  if (meta.partial !== undefined && typeof meta.partial !== "boolean")
    throw Error("录像完成标记无效");
  if (
    meta.names !== undefined &&
    (!Array.isArray(meta.names) ||
      meta.names.length !== 2 ||
      meta.names.some((n) => typeof n !== "string" || n.length > 64))
  )
    throw Error("录像玩家名称无效");
  if (
    (meta.character !== undefined && !integer(meta.character, 0, 18)) ||
    (meta.difficulty !== undefined && !integer(meta.difficulty, 0, 3))
  )
    throw Error("录像角色或难度无效");
  if (meta.kind === "versus") {
    if (
      !validTh19RecordingConfig(meta.config) ||
      meta.config.build !== meta.build ||
      meta.config.seed !== meta.seed ||
      payload.length !== meta.frames * TH19_FRAME_BYTES
    )
      throw Error("录像对局信息不一致");
    for (let frame = 0; frame < meta.frames; frame++) {
      const row = parseTh19StreamPacket(
        payload.subarray(
          frame * TH19_FRAME_BYTES,
          (frame + 1) * TH19_FRAME_BYTES,
        ),
      );
      if (
        row.kind !== "frame" ||
        row.frame !== frame ||
        (frame + 1 < meta.frames && row.result === 2)
      )
        throw Error("录像输入帧不连续");
    }
  } else {
    if (
      payload.length % 80 ||
      payload.length / 80 > meta.frames ||
      !Array.isArray(meta.files) ||
      meta.files.length > 32
    )
      throw Error("单机录像数据无效");
    let frames = 0,
      total = 0;
    const p = new DataView(
      payload.buffer,
      payload.byteOffset,
      payload.byteLength,
    );
    for (let at = 0; at < payload.length; at += 80) {
      const run = p.getUint32(at, true);
      if (
        !run ||
        run > TH19_RECORDING_LIMITS.frames ||
        p.getUint32(at + 76, true) > 1 ||
        !Number.isFinite(p.getFloat32(at + 68, true)) ||
        !Number.isFinite(p.getFloat32(at + 72, true)) ||
        Math.abs(p.getFloat32(at + 68, true)) > 512 ||
        Math.abs(p.getFloat32(at + 72, true)) > 512
      )
        throw Error("单机录像输入无效");
      frames += run;
    }
    if (frames !== meta.frames) throw Error("单机录像帧数不一致");
    const names = new Set();
    for (const file of meta.files) {
      if (
        !file ||
        !/^(?:scoreth19(?:bak)?\.dat|th19\.cfg|th19-(?:progress|notices)-[a-f0-9]{8}\.bin|th19-character-clear\.bin)$/.test(
          file.name,
        ) ||
        names.has(file.name) ||
        !Array.isArray(file.bytes) ||
        file.bytes.length > 65536 ||
        file.bytes.some((n) => !integer(n, 0, 255))
      )
        throw Error("录像初始存档无效");
      names.add(file.name);
      total += file.bytes.length;
    }
    if (total > 128 * 1024) throw Error("录像初始存档过大");
  }
  return { meta, payload };
}

// 0x430340: edge = previous XOR held, pressed = edge & held,
// released = edge & ~held. Bit positions here remain the web input ABI.
export function th19InputEdges(previous, held) {
  return {
    held,
    pressed: (previous ^ held) & held,
    released: (previous ^ held) & ~held,
  };
}
// Version 2 stores a six-byte held/pressed/released tuple for EACH player.
// The surrounding header, touch coordinates and verification hashes are web
// extensions, not the retail t18r container consumed by 0x550250.
export function encodeTh19Replay(meta, payload) {
  if (meta.kind !== "versus") throw Error("只保存联机录像");
  if (!(payload instanceof Uint8Array) || !payload.length)
    throw Error("录像为空");
  validateLegacyPayload(
    { ...meta, schema: "eagler-touhou/th19-replay/1", game: "th19" },
    payload,
  );
  const packed = new Uint8Array(payload.length),
    v = new DataView(packed.buffer),
    previous = [0, 0];
  let played = false;
  for (let frame = 0; frame < meta.frames; frame++) {
    const row = parseTh19StreamPacket(
        payload.subarray(frame * 44, (frame + 1) * 44),
      ),
      at = frame * 44;
    v.setUint32(at, frame, true);
    played ||= row.played;
    for (const [side, input] of [row.left, row.right].entries()) {
      const p = at + 4 + side * 16,
        e = th19InputEdges(previous[side], input.keys);
      previous[side] = e.held;
      v.setUint16(p, e.held, true);
      v.setUint16(p + 2, e.pressed, true);
      v.setUint16(p + 4, e.released, true);
      v.setUint16(p + 6, input.direct, true);
      v.setFloat32(p + 8, input.x, true);
      v.setFloat32(p + 12, input.y, true);
    }
    v.setUint32(at + 36, row.hash ?? 0, true);
    v.setUint8(at + 40, row.result);
    v.setUint8(at + 41, +(row.hash !== null) | (row.played ? 2 : 0));
  }
  if (!played) throw Error("尚未正式开始游戏，不保存录像");
  const header = new TextEncoder().encode(
    JSON.stringify({
      ...meta,
      schema: "eagler-touhou/th19-replay/2",
      game: "th19",
      inputLayout: "two-player-held-pressed-released",
    }),
  );
  const size = 20 + header.length + packed.length;
  if (
    header.length > TH19_RECORDING_LIMITS.headerBytes ||
    size > TH19_RECORDING_LIMITS.fileBytes
  )
    throw Error("录像文件过大");
  const out = new Uint8Array(size),
    d = new DataView(out.buffer);
  out.set(fileMagic);
  out[6] = 2;
  d.setUint32(8, header.length, true);
  d.setUint32(12, packed.length, true);
  out.set(header, 16);
  out.set(packed, 16 + header.length);
  d.setUint32(size - 4, crc32(out.subarray(0, size - 4)), true);
  return out;
}
export function decodeTh19Replay(b) {
  if (b instanceof Uint8Array && b[6] === 1) return decodeLegacyReplay(b);
  if (
    !(b instanceof Uint8Array) ||
    b.length < 22 ||
    b.length > TH19_RECORDING_LIMITS.fileBytes ||
    fileMagic.some((n, i) => b[i] !== (i === 6 ? 2 : n))
  )
    throw Error("不是有效的兽王园录像");
  const v = new DataView(b.buffer, b.byteOffset, b.byteLength),
    h = v.getUint32(8, true),
    size = v.getUint32(12, true);
  if (
    !h ||
    h > TH19_RECORDING_LIMITS.headerBytes ||
    !size ||
    size % 44 ||
    20 + h + size !== b.length ||
    v.getUint32(b.length - 4, true) !== crc32(b.subarray(0, b.length - 4))
  )
    throw Error("录像文件不完整或校验失败");
  const meta = JSON.parse(
    new TextDecoder("utf-8", { fatal: true }).decode(b.subarray(16, 16 + h)),
  );
  if (
    meta.schema !== "eagler-touhou/th19-replay/2" ||
    meta.kind !== "versus" ||
    meta.game !== "th19" ||
    meta.inputLayout !== "two-player-held-pressed-released" ||
    meta.frames !== size / 44
  )
    throw Error("录像信息无效");
  const payload = new Uint8Array(size),
    previous = [0, 0];
  let played = false;
  for (let frame = 0; frame < size / 44; frame++) {
    const at = 16 + h + frame * 44,
      inputs = [];
    if (
      v.getUint32(at, true) !== frame ||
      v.getUint16(at + 42, true) !== 0 ||
      v.getUint8(at + 41) > 3
    )
      throw Error("录像输入帧不连续");
    for (let side = 0; side < 2; side++) {
      const p = at + 4 + side * 16,
        held = v.getUint16(p, true),
        e = th19InputEdges(previous[side], held);
      previous[side] = held;
      if (
        v.getUint16(p + 2, true) !== e.pressed ||
        v.getUint16(p + 4, true) !== e.released
      )
        throw Error("录像按键边沿不一致");
      inputs.push({
        keys: held,
        direct: v.getUint16(p + 6, true),
        x: v.getFloat32(p + 8, true),
        y: v.getFloat32(p + 12, true),
      });
    }
    const flags = v.getUint8(at + 41);
    played ||= !!(flags & 2);
    payload.set(
      th19StreamFrame(frame, ...inputs, {
        result: v.getUint8(at + 40),
        hash: flags & 1 ? v.getUint32(at + 36, true) : null,
        played: !!(flags & 2),
      }),
      frame * 44,
    );
  }
  if (!played) throw Error("录像没有正式游戏帧");
  // Keep one metadata validator for both generations, including sandboxed legacy saves.
  validateLegacyPayload(
    { ...meta, schema: "eagler-touhou/th19-replay/1" },
    payload,
  );
  return { meta, payload };
}
