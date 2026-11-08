import { readFile, writeFile } from 'node:fs/promises';
import { pathToFileURL } from 'node:url';
import { decodeJunowen, encodeJunowen, JUNOWEN_HEADER_BYTES, webKeysToNative } from '../../ports/th19/tools/replay/junowen-format.mjs';
import { decodeTh19Replay, parseTh19StreamPacket, TH19_FRAME_BYTES } from '../../ports/th19/web/sdl-runtime/th19-recording.mjs';

const WEB_UNSUPPORTED_KEY_BITS = 0x300;
const JUNOWEN_MATCHUP_OFFSET = 9;
const HUMAN_VS_CPU = 1;
const CPU_VS_CPU = 2;

function readWebReplay(webBytes) {
  const web = decodeTh19Replay(webBytes);
  if (web.meta.kind !== 'versus') throw Error('Expected a versus web recording');
  return web;
}

function readWebFrame(payload, frame) {
  const offset = frame * TH19_FRAME_BYTES;
  const { left, right } = parseTh19StreamPacket(payload.subarray(offset, offset + TH19_FRAME_BYTES));
  return [left, right];
}

// Compare recorded keys at the same frame index and seat (0 = P1, 1 = P2).
export function alignReplay(webBytes, nativeBytes, { maxMismatches = 64 } = {}) {
  if (!Number.isSafeInteger(maxMismatches) || maxMismatches < 0)
    throw new RangeError('maxMismatches must be a non-negative safe integer');
  const web = readWebReplay(webBytes);
  // The upstream CPU-vs-CPU recorder writes one seat and its reader expects two.
  // Reject before decoding because an even number of one-seat frames fits both layouts.
  if (nativeBytes instanceof Uint8Array && nativeBytes.length >= JUNOWEN_HEADER_BYTES
      && nativeBytes[JUNOWEN_MATCHUP_OFFSET] === CPU_VS_CPU)
    throw Error('Ambiguous Ju.N.Owen CPU-vs-CPU layout: recorder writes one seat, reader expects two');
  const native = decodeJunowen(nativeBytes);
  // Human-vs-CPU stores P1 only. Human-vs-human and youkai-vs-youkai store both seats.
  const comparedSeats = native.header.matchup === HUMAN_VS_CPU ? [0] : [0, 1];
  const webFrames = web.meta.frames;
  const nativeFrames = native.inputs.length;
  const mismatches = [];
  let mismatchCount = 0;
  let hasDirectMovement = false;
  let hasUnsupportedKeys = false;
  for (let frame = 0; frame < webFrames; frame++) {
    for (const [seat, input] of readWebFrame(web.payload, frame).entries()) {
      const webKeys = input.keys;
      const unsupportedWebKeyBits = webKeys & WEB_UNSUPPORTED_KEY_BITS;
      hasDirectMovement ||= Boolean(input.direct);
      hasUnsupportedKeys ||= unsupportedWebKeyBits !== 0;
      if (frame >= nativeFrames || seat >= comparedSeats.length) continue;

      const expectedNative = unsupportedWebKeyBits ? null : webKeysToNative(webKeys);
      const nativeKeys = native.inputs[frame][seat];
      if (expectedNative === nativeKeys) continue;
      mismatchCount++;
      if (mismatches.length < maxMismatches) {
        const mismatch = { frame, seat, webKeys, expectedNative, nativeKeys };
        if (unsupportedWebKeyBits) mismatch.unsupportedWebKeyBits = unsupportedWebKeyBits;
        mismatches.push(mismatch);
      }
    }
  }
  return {
    aligned: webFrames === nativeFrames && mismatchCount === 0,
    webFrames,
    nativeFrames,
    comparedSeats,
    compared: Math.min(webFrames, nativeFrames) * comparedSeats.length,
    mismatches,
    truncatedMismatchCount: mismatchCount - mismatches.length,
    blockers: [
      ...(hasDirectMovement ? ['web-direct-touch-movement'] : []),
      ...(hasUnsupportedKeys ? ['web-unsupported-key-bits'] : []),
      'rng-state-and-frame-clock-are-not-recorded',
    ],
    nativeHeader: native.header,
  };
}

// Encode the web key stream in Ju.N.Owen's 17-byte header and input format.
// Native playback also depends on RNG state and frame-clock behavior.
export function convertWebToJunowen(webBytes, { randSeed2 } = {}) {
  const web = readWebReplay(webBytes), c = web.meta.config;
  const rows = Array.from({ length: web.meta.frames }, (_, frame) =>
    readWebFrame(web.payload, frame).map((input, seat) => {
      if (input.direct) throw Error(`Frame ${frame}, player ${seat + 1}: touch movement cannot be represented in Ju.N.Owen`);
      if (input.keys & WEB_UNSUPPORTED_KEY_BITS) throw Error(`Frame ${frame}, player ${seat + 1}: unsupported web input bits`);
      return webKeysToNative(input.keys);
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
      throw Error('Usage: node align-replay.mjs --convert --web FILE.rpy --out NEW_FILE.rpy [--rand-seed2 UINT32]');
    const seed2 = argv.length === 7 ? Number(argv[6]) : undefined;
    if (seed2 !== undefined && (!Number.isInteger(seed2) || seed2 < 0 || seed2 > 0xffffffff)) throw Error('Invalid --rand-seed2');
    const converted = convertWebToJunowen(new Uint8Array(await readFile(argv[2])), { randSeed2: seed2 });
    await writeFile(argv[4], converted.bytes, { flag: 'wx' });
    process.stdout.write(JSON.stringify({ output: argv[4], frames: converted.frames, bytes: converted.bytes.length, header: converted.header, seed2Fallback: converted.seed2Fallback }, null, 2) + '\n');
    return;
  }
  if ((argv.length !== 4 && argv.length !== 6) || argv[0] !== '--web' || argv[2] !== '--junowen' || (argv.length === 6 && argv[4] !== '--report'))
    throw Error('Usage: node align-replay.mjs --web FILE.rpy --junowen FILE.rpy [--report NEW_FILE.json]');
  const reportPath = argv[4] === '--report' ? argv[5] : null;
  const report = alignReplay(new Uint8Array(await readFile(argv[1])), new Uint8Array(await readFile(argv[3])));
  const json = JSON.stringify(report, null, 2) + '\n';
  if (reportPath) await writeFile(reportPath, json, { flag: 'wx' }); else process.stdout.write(json);
  if (!report.aligned) process.exitCode = 2;
}

if (process.argv[1] && import.meta.url === pathToFileURL(process.argv[1]).href) {
  main(process.argv.slice(2)).catch(error => { console.error(error.message); process.exitCode = 1; });
}
