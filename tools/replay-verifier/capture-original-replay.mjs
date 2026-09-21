import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {createWriteStream, mkdirSync, readFileSync, writeFileSync} from 'node:fs';
import {dirname, resolve} from 'node:path';
import {pathToFileURL} from 'node:url';

const option = name => {
  const index = process.argv.indexOf(name);
  return index < 0 ? undefined : process.argv[index + 1];
};
const replayPath = resolve(option('--replay') ?? '');
const oracleRoot = resolve(option('--oracle-root') ?? 'D:/workspace/东方测试.zip/TouhouDev/th08_web');
const output = resolve(option('--output') ?? 'artifacts/replay-verifier/original-replay/result.json');
const rowsPath = resolve(option('--rows') ?? output.replace(/\.json$/i, '.rows.jsonl'));
const startStage = Number(option('--start-stage') ?? 0);
const maxFrames = Number(option('--max-frames') ?? 180000);
if (!option('--replay')) throw Error('Missing --replay');
if (!Number.isSafeInteger(startStage) || startStage < 0 || startStage > 8) throw Error('Invalid --start-stage');
if (!Number.isSafeInteger(maxFrames) || maxFrames < 300 || maxFrames > 500000) throw Error('Invalid --max-frames');

const bytes = readFileSync(replayPath);
const replaySha256 = createHash('sha256').update(bytes).digest('hex');
const executableSha256 = createHash('sha256').update(readFileSync(resolve(oracleRoot, '../[th08] 东方永夜抄 (日文版)/th08.exe'))).digest('hex');
const resourceSha256 = createHash('sha256').update(readFileSync(resolve(oracleRoot, '../[th08] 东方永夜抄 (日文版)/th08.dat'))).digest('hex');
process.chdir(oracleRoot);
const {headlessSession} = await import(pathToFileURL(resolve(oracleRoot, 'scripts/native/headless-session.mjs')));
const {decodeLzss} = await import(pathToFileURL(resolve(oracleRoot, 'scripts/native/lzss.mjs')));

function inspectReplay(raw) {
  const plain = Buffer.from(raw), fileSize = plain.readUInt32LE(12);
  assert(fileSize >= 104 && fileSize <= plain.length, 'Invalid TH08 replay size');
  let key = plain[21];
  for (let i = 24; i < fileSize; i++) { plain[i] = (plain[i] - key) & 255; key = (key + 7) & 255; }
  const data = Buffer.alloc(104 + plain.readUInt32LE(28));
  plain.copy(data, 0, 0, 104);
  decodeLzss(plain.subarray(104, fileSize), data.subarray(104), new Uint8Array(8192));
  const stages = Array.from({length: 9}, (_, stage) => {
    const offset = data.readUInt32LE(32 + stage * 4);
    return offset ? {stage, score: data.readUInt32LE(offset)} : null;
  }).filter(value => value && value.stage >= startStage);
  assert(stages.length, 'Replay has no selected stage');
  return {stages};
}

mkdirSync(dirname(output), {recursive: true});
mkdirSync(dirname(rowsPath), {recursive: true});
const expected = inspectReplay(bytes);
const rows = createWriteStream(rowsPath, {encoding: 'utf8'});
const rowsClosed = new Promise((resolveClosed, reject) => {
  rows.once('finish', resolveClosed);
  rows.once('error', reject);
});
const session = await headlessSession({invulnerable: false});
const {m, host, ready, select, press, frames, until, state} = session;
host.files.set('replay/th8_04.rpy', bytes);
host.directories.add('replay');
host.d3d.onDraw = () => {};
let lastIdentity = null, tickCount = 0, playbackStarted = false;
const observedStages = [], transitions = [];
let lastStage = null, lastSupervisor = null;

function readRow() {
  const owner = m.u32(0x18b8a28), globals = m.u32(0x160f510), player = 0x17d5ef8;
  const flags = m.u32(0x164d0b4);
  if (m.u32(0x17ce8b4) !== 2 || (flags & 12) !== 12 || !owner) return null;
  const frame = m.u32(owner), stage = m.u32(0x164d2cc);
  if (!frame || stage > 8 || !globals) return null;
  return {
    frame, stage, gameFrames: m.u32(0x164d30c), controlFrames: m.u32(0x164d09c), flags,
    rng: m.u32(0x164d520) & 0xffff, rngCalls: m.u32(0x164d524),
    player: [m.f32(player + 0x2b4), m.f32(player + 0x2b8)], playerState: m.view(player, 1)[0],
    keys: m.u32(0x164d52c) & 0xffff, numbers: Array.from(m.bytes(globals, 228)),
  };
}

host.d3d.onPresent = () => {
  const current = state(), globals = m.u32(0x160f510);
  if (current.stage !== lastStage || current.supervisor !== lastSupervisor) {
    transitions.push({...current, score: globals ? m.u32(globals + 8) : null});
    lastStage = current.stage; lastSupervisor = current.supervisor;
  }
  if (!playbackStarted) return;
  const row = readRow();
  if (!row) return;
  const identity = `${row.stage}:${row.frame}`;
  if (identity === lastIdentity) return;
  lastIdentity = identity; tickCount++;
  if (!observedStages.includes(row.stage)) observedStages.push(row.stage);
  rows.write(`${JSON.stringify(row)}\n`);
};

let result;
try {
  await ready(0); await frames(60);
  await select(4, 9); await press('KeyZ'); await ready(7);
  await select(0, 15); await press('KeyZ'); await frames(90);
  await select(startStage, 9); await press('KeyZ'); await frames(90); await press('KeyZ');
  await until(value => value.supervisor === 2 && (value.flags & 8), 3000);
  playbackStarted = true;
  const playbackFrame = host.d3d.frames;
  await until(value => value.supervisor === 1 || value.supervisor === 6 || value.supervisor === 9, maxFrames);
  const globals = m.u32(0x160f510), finalScore = globals ? m.u32(globals + 8) : null;
  assert.deepEqual(observedStages, expected.stages.map(value => value.stage), 'Every recorded stage must play');
  assert.equal(finalScore, expected.stages.at(-1).score, 'Final recorded score');
  result = {
    schema: 'th08/original-replay-tick-capture/v1', complete: true, provider: 'th08-original-jit/present-observer',
    replay: {path: replayPath, bytes: bytes.length, sha256: replaySha256},
    identity: {executableSha256, resourceSha256}, rows: {path: rowsPath, ticks: tickCount},
    applicationFramesAfterPlaybackStart: host.d3d.frames - playbackFrame,
    expected, observedStages, transitions, final: state(), finalScore,
  };
} catch (error) {
  result = {
    schema: 'th08/original-replay-tick-capture/v1', complete: false, provider: 'th08-original-jit/present-observer',
    replay: {path: replayPath, bytes: bytes.length, sha256: replaySha256},
    identity: {executableSha256, resourceSha256}, rows: {path: rowsPath, ticks: tickCount},
    expected, observedStages, transitions, final: state(), error: error.stack ?? String(error),
  };
  process.exitCode = 1;
} finally {
  rows.end();
  await rowsClosed;
  session.close();
  writeFileSync(output, `${JSON.stringify(result, null, 2)}\n`, 'utf8');
  console.log(JSON.stringify({complete: result.complete, ticks: tickCount, stages: observedStages, output, rows: rowsPath}));
}
