import {createHash} from 'node:crypto';
import {createWriteStream, mkdirSync, readFileSync, writeFileSync} from 'node:fs';
import {dirname, resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {inspectReplay, validatePlayback} from './playback-validation.mjs';

const option = name => {
  const index = process.argv.indexOf(name);
  return index < 0 ? undefined : process.argv[index + 1];
};
const replayPath = resolve(option('--replay') ?? '');
const oracleRoot = resolve(option('--oracle-root') ?? 'D:/workspace/东方测试.zip/TouhouDev/th10_web');
const output = resolve(option('--output') ?? 'artifacts/replay-verifier/original-replay/result.json');
const rowsPath = resolve(option('--rows') ?? output.replace(/\.json$/i, '.rows.jsonl'));
const maxFrames = Number(option('--max-frames') ?? 180000);
if (!option('--replay')) throw Error('Missing --replay');
if (!Number.isSafeInteger(maxFrames) || maxFrames < 300 || maxFrames > 500000)
  throw Error('Invalid frame bound');

const bytes = readFileSync(replayPath);
if (bytes.toString('ascii', 0, 4) !== 't10r') throw Error('Not a TH10 replay');
process.chdir(oracleRoot);
const {createSession} = await import(pathToFileURL(resolve(oracleRoot, 'scripts/native/session.mjs')));
const {decodeLzss} = await import(pathToFileURL(resolve(oracleRoot, 'runtime/lzss.mjs')));
const expected = inspectReplay(bytes, decodeLzss);
const session = await createSession({language: 'jp'});
const transitions = [];
let lastStage = null;
let lastIdentity = null, tickCount = 0;
mkdirSync(dirname(rowsPath), {recursive: true});
const rows = createWriteStream(rowsPath, {encoding: 'utf8'});
const rowsClosed = new Promise((resolveClosed, reject) => {
  rows.once('finish', resolveClosed);
  rows.once('error', reject);
});

function readRow() {
  const m = session.m, replay = m.u32(0x477838), player = m.u32(0x477834), gameSession = m.u32(0x477810);
  const short = (address, signed = false) => {
    const bytes = m.bytes(address, 2), view = new DataView(bytes.buffer, bytes.byteOffset, 2);
    return signed ? view.getInt16(0, true) : view.getUint16(0, true);
  };
  const screen = m.i32(0x491fb4), pending = m.i32(0x491fb8);
  if (!replay || m.i32(replay + 0x10) !== 1 || !gameSession || screen !== pending) return null;
  const stage = m.i32(replay + 0x1d0);
  if (stage < 0 || stage >= 8) return null;
  const frame = m.i32(replay + 0xa0 + stage * 0x24 + 0x14);
  if (frame <= 0) return null;
  return {
    frame, stage, stageFrames: m.u32(0x474c88), sectionFrames: m.u32(0x474c8c), flags: m.u32(0x474ca0),
    score: m.i32(0x474c44), power: short(0x474c48, true), itemValue: m.i32(0x474c4c), lives: m.i32(0x474c70), rank: m.i32(0x474c98),
    rng: m.u32(0x4918b0), rngCalls: m.u32(0x4918b4), keys: short(0x474e5c),
    player: player ? [m.f32(player + 0x3c0), m.f32(player + 0x3c4)] : [0, 0],
    playerState: player ? m.i32(player + 0x458) : 0, sessionFlags: m.u32(gameSession + 0x58),
  };
}

function snapshot() {
  const state = session.screen();
  return {
    ...state,
    replayMode: session.m.u32(0x491c00),
    replayName: session.host.text(0x477710),
    score: session.m.u32(0x474c44) * 10,
  };
}

function observe() {
  const row = snapshot();
  if (row.replayMode === 2 && row.stage !== lastStage) {
    transitions.push(row);
    lastStage = row.stage;
    console.log(JSON.stringify({event: 'stage', ...row}));
  }
  const tick = readRow();
  if (tick) {
    const identity = `${tick.stage}:${tick.frame}`;
    if (identity !== lastIdentity) {
      lastIdentity = identity; tickCount++;
      rows.write(`${JSON.stringify(tick)}\n`);
    }
  }
}

try {
  session.host.overlay.set('replay/th10_01.rpy', bytes);
  session.host.directories.add('replay');
  session.frames(450);
  session.key('KeyZ');
  session.key('ArrowDown');
  session.key('ArrowDown');
  session.key('KeyZ');
  session.key('KeyZ');
  session.key('KeyZ');
  session.frames(300, observe);
  if (session.m.u32(0x491c00) !== 2) throw Error(`Replay did not start: ${JSON.stringify(snapshot())}`);
  const playbackStart = session.host.d3d.frames;
  while (session.host.d3d.frames - playbackStart < maxFrames && session.m.u32(0x491c00) === 2) {
    await session.framesAsync(300, observe);
    if ((session.host.d3d.frames - playbackStart) % 3000 < 300)
      console.log(JSON.stringify({event: 'progress', ...snapshot()}));
  }
  const final = snapshot();
  const result = {
    schema: 'th10/original-replay-playback/v1',
    complete: false,
    provider: 'th10-original-jit',
    replay: {path: replayPath, bytes: bytes.length, sha256: createHash('sha256').update(bytes).digest('hex')},
    rows: {path: rowsPath, ticks: tickCount},
    // Startup already consumed playback frames; this is not a logical tick count.
    applicationFramesAfterStartup: session.host.d3d.frames - playbackStart,
    expected,
    transitions,
    final,
  };
  try {
    result.validation = validatePlayback(expected, result);
    result.complete = true;
  } catch (error) {
    result.validation = {passed: false, error: error.message};
  }
  mkdirSync(dirname(output), {recursive: true});
  writeFileSync(output, JSON.stringify(result, null, 2), 'utf8');
  if (!result.complete) throw Error(`Playback validation failed: ${result.validation.error}; evidence: ${output}`);
  console.log(JSON.stringify({event: 'complete', validation: result.validation, final}));
} finally {
  rows.end();
  await rowsClosed;
  session.close();
}
