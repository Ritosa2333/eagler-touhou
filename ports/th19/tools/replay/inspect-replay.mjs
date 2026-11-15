import { decodeJunowen, encodeJunowen } from './junowen-format.mjs';
import { decodeTh19Replay, parseTh19StreamPacket, TH19_FRAME_BYTES } from '../../web/sdl-runtime/th19-recording.mjs';

// Inspect contents, never infer the format from .rpy/.rep alone.
export function inspectReplay(bytes, { format = 'auto' } = {}) {
  if (!(bytes instanceof Uint8Array)) throw Error('Expected replay bytes');
  if (!['auto', 'web', 'junowen'].includes(format)) throw Error('Unknown format');
  const webMagic = [69, 84, 49, 57, 82, 80, 1, 0];
  const isWeb = webMagic.every((v, i) => i === 6 || bytes[i] === v);
  if (format === 'web' || (format === 'auto' && isWeb)) {
    const { meta, payload } = decodeTh19Replay(bytes);
    const counts = { directMovement: 0, unsupportedKeys: 0, pause: 0 };
    if (meta.kind === 'versus') {
      for (let offset = 0; offset < payload.length; offset += TH19_FRAME_BYTES) {
        const row = parseTh19StreamPacket(payload.subarray(offset, offset + TH19_FRAME_BYTES));
        for (const input of [row.left, row.right]) {
          if (input.direct) counts.directMovement++;
          if (input.keys & 768) counts.unsupportedKeys++;
          if (input.keys & 1024) counts.pause++;
        }
      }
    }
    return {
      format: 'web', frames: meta.frames, kind: meta.kind,
      difficulty: meta.config?.difficulty ?? meta.difficulty,
      counts,
      nativePlaybackVerified: false,
      conversionBlockers: [
        'native-rng-state-and-shadow-not-recorded',
        'native-frame-clock-not-recorded',
        'engine-random-call-order-not-aligned',
        ...(meta.kind !== 'versus' ? ['legacy-single-player-recording'] : []),
        ...(counts.directMovement ? ['direct-movement-has-no-native-key-equivalent'] : []),
        ...(counts.unsupportedKeys ? ['unsupported-web-key-bits'] : []),
      ],
    };
  }
  if (format === 'auto') {
    // Ju.N.Owen has no magic: a plausible 17-byte header is not identification.
    throw Error('Unidentified container; specify --format junowen for a Ju.N.Owen file');
  }
  const replay = decodeJunowen(bytes), h = replay.header;
  const packed = encodeJunowen(replay);
  const unknownInputRows = replay.inputs.filter(row => row.some(keys => keys & ~511)).length;
  return {
    format: 'junowen', header: h, frames: replay.inputs.length,
    settings: {
      timeLimitIndex: h.common & 7, roundIndex: (h.common >>> 4) & 3,
      cardMode: h.common >>> 6,
      lifeIndices: [h.p1Settings & 7, h.p2Settings & 7],
      barrierModes: [(h.p1Settings >>> 3) & 3, (h.p2Settings >>> 3) & 3],
    },
    byteExactRoundtrip: packed.every((v, i) => v === bytes[i]),
    nativePlaybackVerified: false,
    rngStateFields: 2,
    roundBoundaryRecords: false,
    warnings: [
      ...(unknownInputRows ? [`${unknownInputRows} rows contain flags without a web mapping`] : []),
      ...(h.matchup === 2 ? ['Upstream CPU-vs-CPU recorder writes one seat, but reader expects two; source provenance is required'] : []),
      ...(((h.common >>> 4) & 3) !== 0 ? ['Multiple-round settings: this format has no round boundaries; the upstream recorder may overwrite prior-round inputs when the native frame resets'] : []),
    ],
  };
}

// This command only reads files and emits an audit; it never starts a player.
if (process.argv[1] && import.meta.url === (await import('node:url')).pathToFileURL(process.argv[1]).href) {
  try {
    const args = process.argv.slice(2);
    if (![1, 3].includes(args.length) || (args.length === 3 && args[1] !== '--format')) {
      throw Error('Usage: node inspect-replay.mjs FILE [--format web|junowen]');
    }
    const { readFile, stat } = await import('node:fs/promises');
    if ((await stat(args[0])).size > 32 * 1024 * 1024) throw Error('Replay exceeds 32 MiB');
    console.log(JSON.stringify(inspectReplay(new Uint8Array(await readFile(args[0])), { format: args[2] ?? 'auto' }), null, 2));
  } catch (error) {
    console.error(error.message);
    process.exitCode = 1;
  }
}
