import { readFile, writeFile } from 'node:fs/promises';
import { pathToFileURL } from 'node:url';
import { decodeJunowen, encodeJunowen, webKeysToNative } from '../../ports/th19/tools/replay/junowen-format.mjs';
import { decodeTh19Replay, parseTh19StreamPacket, TH19_FRAME_BYTES } from '../../ports/th19/web/sdl-runtime/th19-recording.mjs';

function readWebRows(webBytes) {
  const web = decodeTh19Replay(webBytes);
  if (web.meta.kind !== 'versus') throw Error('Only versus web recordings can be converted');
  const rows = [];
  for (let at = 0; at < web.payload.length; at += TH19_FRAME_BYTES) {
    const row = parseTh19StreamPacket(web.payload.subarray(at, at + TH19_FRAME_BYTES));
    rows.push([row.left, row.right]);
  }
  return { ...web, rows };
}

// Compare a web .rpy input stream with a Ju.N.Owen input stream.
export function alignReplay(webBytes, nativeBytes, { maxMismatches = 64 } = {}) {
  const web = readWebRows(webBytes);
  const native = decodeJunowen(nativeBytes);
  const seats = native.header.matchup === 1 ? 1 : 2;
  const webRows = web.rows;
  const frames = Math.min(webRows.length, native.inputs.length);
  const mismatches = [];
  let compared = 0;
  for (let frame = 0; frame < frames; frame++) {
    for (let seat = 0; seat < seats; seat++) {
      const expected = webKeysToNative(webRows[frame][seat].keys);
      const actual = native.inputs[frame][seat];
      compared++;
      if (expected !== actual && mismatches.length < maxMismatches)
        mismatches.push({ frame, seat, webKeys: webRows[frame][seat].keys, expectedNative: expected, nativeKeys: actual });
    }
  }
  return {
    aligned: webRows.length === native.inputs.length && mismatches.length === 0,
    webFrames: webRows.length,
    nativeFrames: native.inputs.length,
    compared,
    mismatches,
    truncatedMismatchCount: Math.max(0, mismatches.length - maxMismatches),
    blockers: [
      ...(webRows.some(row => row.some(input => input.direct)) ? ['web-direct-touch-movement'] : []),
      ...(webRows.some(row => row.some(input => input.keys & 768)) ? ['web-unsupported-key-bits'] : []),
      'rng-state-and-frame-clock-are-not-recorded',
    ],
    nativeHeader: native.header,
  };
}

// Convert the web input stream to the 17-byte file layout consumed by
// progre/junowen's th19replayplayer. The web game has one shared seed; when a
// native second seed is not embedded, the deterministic fallback is seed1.
export function convertWebToJunowen(webBytes, { randSeed2 } = {}) {
  const web = readWebRows(webBytes), c = web.meta.config;
  if (web.rows.length !== web.meta.frames) throw Error('Web recording frame count is inconsistent');
  const rows = web.rows.map((row, frame) => row.map((input, seat) => {
    if (input.direct) throw Error(`Frame ${frame}, player ${seat + 1}: touch movement cannot be represented in Ju.N.Owen`);
    try { return webKeysToNative(input.keys); }
    catch { throw Error(`Frame ${frame}, player ${seat + 1}: unsupported web input bits`); }
  }));
  const rules = c.rules;
  const header = {
    randSeed1: web.meta.seed,
    randSeed2: Number.isInteger(randSeed2) ? randSeed2 >>> 0 : (web.meta.native?.randSeed2 ?? web.meta.seed),
    difficulty: c.difficulty,
    matchup: 0,
    common: rules[0] | (rules[1] << 4) | (rules[2] << 6),
    p1Settings: rules[3] | (rules[5] << 3),
    p2Settings: rules[4] | (rules[6] << 3),
    p1Character: c.characters[0], p1Card: rules[7],
    p2Character: c.characters[1], p2Card: rules[8],
  };
  return { bytes: encodeJunowen({ header, inputs: rows }), header, frames: rows.length, seed2Fallback: !Number.isInteger(randSeed2) && web.meta.native?.randSeed2 === undefined };
}

async function main(argv) {
  if (argv[0] === '--convert') {
    if (![5, 7].includes(argv.length) || argv[1] !== '--web' || argv[3] !== '--out' || (argv.length === 7 && argv[5] !== '--rand-seed2'))
      throw Error('Usage: node align-replay.mjs --convert --web FILE.rpy --out FILE.rpy [--rand-seed2 UINT32]');
    const seed2 = argv.length === 7 ? Number(argv[6]) : undefined;
    if (seed2 !== undefined && (!Number.isInteger(seed2) || seed2 < 0 || seed2 > 0xffffffff)) throw Error('Invalid --rand-seed2');
    const converted = convertWebToJunowen(new Uint8Array(await readFile(argv[2])), { randSeed2: seed2 });
    await writeFile(argv[4], converted.bytes);
    process.stdout.write(JSON.stringify({ output: argv[4], frames: converted.frames, bytes: converted.bytes.length, header: converted.header, seed2Fallback: converted.seed2Fallback }, null, 2) + '\n');
    return;
  }
  if ((argv.length !== 4 && argv.length !== 6) || argv[0] !== '--web' || argv[2] !== '--junowen' || (argv.length === 6 && argv[4] !== '--report'))
    throw Error('Usage: node align-replay.mjs --web FILE.rpy --junowen FILE.rpy [--report FILE.json]');
  const reportPath = argv[4] === '--report' ? argv[5] : null;
  const report = alignReplay(new Uint8Array(await readFile(argv[1])), new Uint8Array(await readFile(argv[3])));
  const json = JSON.stringify(report, null, 2) + '\n';
  if (reportPath) await writeFile(reportPath, json); else process.stdout.write(json);
  if (!report.aligned) process.exitCode = 2;
}

if (process.argv[1] && import.meta.url === pathToFileURL(process.argv[1]).href) {
  main(process.argv.slice(2)).catch(error => { console.error(error.message); process.exitCode = 1; });
}
