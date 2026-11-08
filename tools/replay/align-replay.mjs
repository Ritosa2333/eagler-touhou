import { readFile, writeFile } from 'node:fs/promises';
import { pathToFileURL } from 'node:url';
import { decodeJunowen, webKeysToNative } from '../../ports/th19/tools/replay/junowen-format.mjs';
import { decodeTh19Replay, parseTh19StreamPacket, TH19_FRAME_BYTES } from '../../ports/th19/web/sdl-runtime/th19-recording.mjs';

// Compare a web .rpy input stream with a Ju.N.Owen input stream. This aligns
// only the recorded input timeline; it does not claim native engine parity.
export function alignReplay(webBytes, nativeBytes, { maxMismatches = 64 } = {}) {
  const web = decodeTh19Replay(webBytes);
  if (web.meta.kind !== 'versus') throw Error('Only versus web recordings can be aligned');
  const native = decodeJunowen(nativeBytes);
  const seats = native.header.matchup === 1 ? 1 : 2;
  const webRows = [];
  for (let at = 0; at < web.payload.length; at += TH19_FRAME_BYTES) {
    const row = parseTh19StreamPacket(web.payload.subarray(at, at + TH19_FRAME_BYTES));
    webRows.push([row.left, row.right]);
  }
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

async function main(argv) {
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
