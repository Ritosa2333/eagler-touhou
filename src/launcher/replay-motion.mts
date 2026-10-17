/** Browser file-format adapter for the Runtime's THMOTION v2 trailer.
 * No native game code or decompression is needed to preserve its export name. */
export function motionReplayName(path: string, bytes: Uint8Array, game: number): string {
  if (!path.endsWith('.rpy')) return path;
  const signature = [84, 72, 77, 79, 84, 73, 79, 78], end = bytes.length - 24;
  if (end < 0 || signature.some((byte, index) => bytes[end + index] !== byte)) return path;
  const invalid = () => {throw new Error('触摸录像数据损坏或不属于这款游戏');};
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength), length = view.getUint32(end + 16, true), start = end - length;
  if (view.getUint32(end + 8, true) !== game || view.getUint32(end + 12, true) !== 2 || length < 80 || length > 80 + 600000 * 12 || start < 0) return invalid();
  let hash = 2166136261;
  for (let offset = start; offset < end; offset++) hash = Math.imul(hash ^ bytes[offset]!, 16777619) >>> 0;
  if (hash !== view.getUint32(end + 20, true)) return invalid();
  let offset = start + 80, total = 0;
  for (let stage = 0; stage < 20; stage++) {
    const count = view.getUint32(start + stage * 4, true);
    if (count > 600000 - total || count > (end - offset) / 12) return invalid();
    total += count;
    let previous = -1;
    for (let index = 0; index < count; index++, offset += 12) {
      const tick = view.getUint32(offset, true), x = view.getFloat32(offset + 4, true), y = view.getFloat32(offset + 8, true);
      if (tick <= previous || !Number.isFinite(x) || !Number.isFinite(y) || Math.abs(x) > 10000000 || Math.abs(y) > 10000000) return invalid();
      previous = tick;
    }
  }
  if (offset !== end) return invalid();
  return path + 'x';
}
