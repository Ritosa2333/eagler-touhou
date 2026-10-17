import {mkdirSync, writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';

const option = name => {
  const index = process.argv.indexOf(name);
  return index < 0 ? undefined : process.argv[index + 1];
};
const oracleRoot = resolve(option('--oracle-root') ?? 'D:/workspace/东方测试.zip/TouhouDev/th10_web');
const output = resolve(option('--output') ?? 'artifacts/replay-verifier/original-demo/suite.json');
const maxFrames = Number(option('--max-frames') ?? 100000);
if (!Number.isSafeInteger(maxFrames) || maxFrames < 10000 || maxFrames > 500000)
  throw Error('Invalid frame bound');

process.chdir(oracleRoot);
const {createSession} = await import(pathToFileURL(resolve(oracleRoot, 'scripts/native/session.mjs')));
const session = await createSession({language: 'jp'});
const demos = [];
let active = null;

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
  return {
    frame, stage, stageFrames: m.u32(0x474c88), sectionFrames: m.u32(0x474c8c), flags: m.u32(0x474ca0),
    score: m.i32(0x474c44), power: short(0x474c48, true), itemValue: m.i32(0x474c4c), lives: m.i32(0x474c70), rank: m.i32(0x474c98),
    rng: m.u32(0x4918b0), rngCalls: m.u32(0x4918b4), keys: short(0x474e5c),
    player: player ? [m.f32(player + 0x3c0), m.f32(player + 0x3c4)] : [0, 0],
    playerState: player ? m.i32(player + 0x458) : 0, sessionFlags: m.u32(gameSession + 0x58),
  };
}

function observe() {
  const row = readRow();
  if (row) {
    if (!active) active = {last: null, rows: []};
    const identity = `${row.stage}:${row.frame}`;
    if (identity !== active.last) { active.last = identity; active.rows.push(row); }
  } else if (active?.rows.length) {
    demos.push({rotationIndex: demos.length, reason: 'returned-from-demo', rows: active.rows});
    console.log(JSON.stringify({rotationIndex: demos.length - 1, ticks: active.rows.length, last: active.rows.at(-1)}));
    active = null;
  }
}

try {
  while (session.host.d3d.frames < maxFrames && demos.length < 4)
    await session.framesAsync(Math.min(300, maxFrames - session.host.d3d.frames), observe);
  const result = {
    schema: 'th10/original-title-demo-capture/v1',
    complete: demos.length === 4,
    provider: 'th10-original-jit/present-observer',
    applicationFrames: session.host.d3d.frames,
    demos,
  };
  mkdirSync(resolve(output, '..'), {recursive: true});
  writeFileSync(output, JSON.stringify(result), 'utf8');
  if (!result.complete) throw Error(`Original TH10 Demo capture incomplete; evidence: ${output}`);
} finally {
  session.close();
}
