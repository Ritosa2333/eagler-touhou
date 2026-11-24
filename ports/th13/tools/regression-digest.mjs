// 全回放回归 digest 门禁：生成 / 校验 golden（replay 与 draws 两种模式）。
//
// 背景：移植侧没有原版真值可逐帧对拍，重构的「行为不变」只能以位精确的
// 状态/事件/绘制摘要证明。harness（tests/harness/harness.cpp）在
// TH13_DIGEST_LOG 下逐采样帧写：
//   stage <s> frame <f> state <16hex> events <16hex> [draws <16hex>] stepped <0|1>
// 末尾一行 total。本工具跑完整录像（TH13_REPLAY_ALL=1 + TH13_REPLAY_FULL=1），
// 解析日志并把每关/每采样行冻结成 golden。
//
// 用法（容器内，工作目录 ports/th13）：
//   node tools/regression-digest.mjs                 重建 golden/digests.json
//   node tools/regression-digest.mjs --check         与 golden 比对（首差退出 1）
//   node tools/regression-digest.mjs --draws         重建 golden/digests-draws.json
//   node tools/regression-digest.mjs --draws --check 校验绘制子集
//   node tools/regression-digest.mjs --replay <path> [--replay ...] [--stride N]
//                                    [--draws-frames N] [--keep]
//
// 构建身份（wasm sha256/build.json）只作 provenance 记录；--check 绝不要求
// 构建哈希一致——门禁的全部意义就是比较不同构建。
//
// 稳定性注意：wasm harness 在部分录像的深 ANM 路径上原生栈余量不足，
// 同一二进制会随机 SIGSEGV（V8 栈帧/ASLR 相关，原生 ASan 无错）。每条
// 录像重试 --attempts 次并轮换 V8 编译模式；仍失败的条目在 golden 里记为
// stable:false，只保留停止点证据，不参与摘要比较（--check 遇差异还会
// 整卷复核 --confirmations 次）。稳定条目一旦变为不稳定即判失败。
//
// 默认语料 = games/replays/*.rpy（6 个玩家录像）+ artifacts/reference/raw/
// demo0..3.rpy。draws 模式只跑第 0 关的前 --draws-frames 帧（默认 600）：
// 生产绘制路径在重绘制关卡会超过 wasm 1MB 线性栈，整卷 draws 不是门禁范围。
import {createHash} from 'node:crypto';
import {existsSync, mkdirSync, readFileSync, readdirSync, rmSync, statSync, writeFileSync} from 'node:fs';
import {basename, dirname, resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {guestPath} from './harness-paths.mjs';

const port = resolve(dirname(fileURLToPath(import.meta.url)), '..');
const workspace = resolve(port, '../../..');
const rawGuest = '/artifacts/reference/raw';
const tempGuest = '/artifacts/tmp-regression-digest';
const wasmPath = resolve(port, 'web/artifacts/wasi/th13-harness.wasm');
const buildManifest = resolve(port, 'web/artifacts/wasi/build.json');
const goldenDir = resolve(port, 'tools/replay-verifier/golden');

export const SCHEMA = 'th13-regression-digest/v1';
export const ROW_HEADER = 'th13-regression-digest/v1';

const hex16 = value => typeof value === 'string' && /^[0-9a-f]{16}$/.test(value);

function number(token, where) {
  if (!/^\d+$/.test(token || '')) throw Error(`${where}: expected an integer, got "${token}"`);
  return Number(token);
}

// 解析 digest 日志。行序保留（stage 内 frame 必须非降），供首差定位。
export function parseDigestLog(text) {
  const rows = [];
  let header = null;
  let total = null;
  const lastFrame = new Map();
  for (const rawLine of String(text).split('\n')) {
    const line = rawLine.trim();
    if (!line) continue;
    if (line === ROW_HEADER) {
      if (header) throw Error('duplicate digest header');
      header = line;
      continue;
    }
    const token = line.split(/\s+/);
    if (token[0] === 'stage') {
      if (token.length < 8 || token[2] !== 'frame' || token[4] !== 'state' || token[6] !== 'events')
        throw Error(`malformed digest row: ${line.slice(0, 64)}`);
      const row = {
        stage: number(token[1], 'stage'),
        frame: number(token[3], 'frame'),
        state: token[5],
        events: token[7],
        draws: null,
        stepped: null,
      };
      if (!hex16(row.state) || !hex16(row.events))
        throw Error(`malformed digest hex: ${line.slice(0, 64)}`);
      let at = 8;
      if (token[at] === 'draws') {
        row.draws = token[at + 1];
        if (!hex16(row.draws)) throw Error(`malformed draws digest: ${line.slice(0, 64)}`);
        at += 2;
      }
      if (token[at] === 'stepped') {
        if (token[at + 1] !== '0' && token[at + 1] !== '1')
          throw Error(`malformed stepped flag: ${line.slice(0, 64)}`);
        row.stepped = token[at + 1] === '1';
        at += 2;
      }
      if (at !== token.length) throw Error(`trailing digest tokens: ${line.slice(0, 64)}`);
      const previous = lastFrame.get(row.stage);
      if (previous !== undefined && row.frame < previous)
        throw Error(`frame order regressed: ${line.slice(0, 64)}`);
      lastFrame.set(row.stage, row.frame);
      rows.push(row);
      continue;
    }
    if (token[0] === 'total') {
      if (total) throw Error('duplicate digest total line');
      if (token.length < 9 || token[1] !== 'stages' || token[3] !== 'frames' ||
          token[5] !== 'state' || token[7] !== 'events')
        throw Error(`malformed digest total: ${line.slice(0, 64)}`);
      total = {
        stages: number(token[2], 'total stages'),
        frames: number(token[4], 'total frames'),
        state: token[6],
        events: token[8],
        draws: null,
      };
      if (!hex16(total.state) || !hex16(total.events))
        throw Error(`malformed total hex: ${line.slice(0, 64)}`);
      let at = 9;
      if (token[at] === 'draws') {
        total.draws = token[at + 1];
        if (!hex16(total.draws)) throw Error(`malformed total draws: ${line.slice(0, 64)}`);
        at += 2;
      }
      if (at !== token.length) throw Error(`trailing total tokens: ${line.slice(0, 64)}`);
      continue;
    }
    throw Error(`unrecognized digest line: ${line.slice(0, 64)}`);
  }
  if (!header) throw Error('missing th13-regression-digest/v1 header');
  return {header, rows, total};
}

function describeRow(row) {
  if (!row) return null;
  return `${row.state}/${row.events}/${row.draws ?? '-'}/${row.stepped === null ? '-' : row.stepped ? 1 : 0}`;
}

// 首个差异（golden vs current）。返回 null 或
// {replay, stage, frame, stream, expected, actual}。
export function firstDivergence(golden, current) {
  const at = (replay, stage, frame, stream, expected, actual) =>
    ({replay, stage, frame, stream, expected, actual});
  if (!golden || typeof golden !== 'object')
    return at('*', '-', '-', 'golden', 'object', typeof golden);
  for (const key of ['schema', 'mode'])
    if ((golden[key] ?? null) !== (current[key] ?? null))
      return at('*', '-', '-', key, golden[key] ?? null, current[key] ?? null);
  if (golden.stride !== current.stride)
    return at('*', '-', '-', 'stride', golden.stride, current.stride);
  if (golden.mode === 'draws' && golden.drawsFrames !== current.drawsFrames)
    return at('*', '-', '-', 'draws-frames', golden.drawsFrames, current.drawsFrames);
  const expected = Array.isArray(golden.corpus) ? golden.corpus : [];
  const actual = Array.isArray(current.corpus) ? current.corpus : [];
  const count = Math.max(expected.length, actual.length);
  for (let index = 0; index < count; index++) {
    const left = expected[index];
    const right = actual[index];
    if (!left) return at(right.file, '-', '-', 'replay-extra', null, right.file);
    if (!right) return at(left.file, '-', '-', 'replay-missing', left.file, null);
    if (left.file !== right.file)
      return at(left.file, '-', '-', 'replay', left.file, right.file);
    if (left.sha256 !== right.sha256)
      return at(left.file, '-', '-', 'replay-sha256', left.sha256, right.sha256);
    // 不稳定条目：harness 在该录像上会因 wasm 栈余量随机 SIGSEGV，金标只
    // 记录失败证据，不参与摘要比较（仍逐条跑、逐条报告）。稳定条目一旦
    // 变为不稳定 = 门禁失败。
    const leftStable = left.stable !== false;
    const rightStable = right.stable !== false;
    if (!leftStable) continue;
    if (!rightStable)
      return at(left.file, '-', '-', 'stability', true, false);
    for (const key of ['stages', 'frames', 'completed'])
      if (left[key] !== right[key])
        return at(left.file, '-', '-', key, left[key], right[key]);
    if (JSON.stringify(left.stop ?? null) !== JSON.stringify(right.stop ?? null))
      return at(left.file, '-', '-', 'stop', JSON.stringify(left.stop ?? null),
                JSON.stringify(right.stop ?? null));
    const rowsLeft = Array.isArray(left.rows) ? left.rows : [];
    const rowsRight = Array.isArray(right.rows) ? right.rows : [];
    const rows = Math.max(rowsLeft.length, rowsRight.length);
    for (let row = 0; row < rows; row++) {
      const a = rowsLeft[row];
      const b = rowsRight[row];
      if (!a) return at(right.file, b.stage, b.frame, 'rows-extra', null, describeRow(b));
      if (!b) return at(left.file, a.stage, a.frame, 'rows-missing', describeRow(a), null);
      for (const key of ['stage', 'frame', 'state', 'events', 'draws', 'stepped'])
        if ((a[key] ?? null) !== (b[key] ?? null))
          return at(left.file, a.stage, a.frame, key, a[key] ?? null, b[key] ?? null);
    }
  }
  return null;
}

// golden 文档（键序固定，便于逐字节 diff；build 只作 provenance）。
export function makeDocument({mode = 'replay', stride = 60, drawsFrames = null,
                              replays = [], build = null} = {}) {
  const corpus = replays.map(entry => ({
    file: entry.file,
    source: entry.source,
    sha256: entry.sha256,
    stages: entry.stages,
    frames: entry.frames,
    completed: entry.completed,
    stable: entry.completed,
    stop: entry.completed ? null : (entry.stop ?? null),
    rows: entry.rows,
  }));
  return {
    schema: SCHEMA,
    mode,
    stride,
    ...(mode === 'draws' ? {drawsFrames} : {}),
    corpus,
    build,
  };
}

export function parseArguments(argv) {
  const options = {check: false, draws: false, stride: 60, drawsFrames: 600,
                   attempts: 100, confirmations: 2, replays: [], keep: false, help: false};
  for (let index = 0; index < argv.length; index++) {
    const arg = argv[index];
    const value = () => {
      const next = argv[++index];
      if (next === undefined) throw Error(`${arg}: missing value`);
      return next;
    };
    switch (arg) {
      case '--check': options.check = true; break;
      case '--draws': options.draws = true; break;
      case '--keep': options.keep = true; break;
      case '--help': case '-h': options.help = true; break;
      case '--stride': options.stride = Number.parseInt(value(), 10); break;
      case '--draws-frames': options.drawsFrames = Number.parseInt(value(), 10); break;
      case '--attempts': options.attempts = Number.parseInt(value(), 10); break;
      case '--confirmations': options.confirmations = Number.parseInt(value(), 10); break;
      case '--replay': options.replays.push(value()); break;
      default: throw Error(`unknown argument: ${arg}`);
    }
  }
  if (!Number.isInteger(options.stride) || options.stride < 1)
    throw Error('--stride must be a positive integer');
  if (!Number.isInteger(options.drawsFrames) || options.drawsFrames < 1)
    throw Error('--draws-frames must be a positive integer');
  if (!Number.isInteger(options.attempts) || options.attempts < 1)
    throw Error('--attempts must be a positive integer');
  if (!Number.isInteger(options.confirmations) || options.confirmations < 0)
    throw Error('--confirmations must be a non-negative integer');
  return options;
}

function walkFiles(directory, out = []) {
  for (const entry of readdirSync(directory, {withFileTypes: true})) {
    const path = resolve(directory, entry.name);
    if (entry.isDirectory()) walkFiles(path, out);
    else out.push(path);
  }
  return out;
}

// harness 产物必须存在且不旧于源码；不静默重建（构建耗时且属于显式动作）。
export function staleHarnessInputs() {
  if (!existsSync(wasmPath)) return {missing: true, wasm: wasmPath, newer: []};
  const built = statSync(wasmPath).mtimeMs;
  const inputs = [
    ...walkFiles(resolve(port, 'web/cpp')).filter(path => /\.(?:cpp|hpp|h|inc)$/.test(path)),
    ...walkFiles(resolve(port, 'tests/harness')).filter(path => /\.(?:cpp|hpp|h|inc)$/.test(path)),
    resolve(port, 'portable/build-harness.mjs'),
  ];
  const newer = inputs.filter(path => existsSync(path) && statSync(path).mtimeMs > built + 1);
  return {missing: false, wasm: wasmPath, newer};
}

function readBuild() {
  if (!existsSync(buildManifest)) return null;
  try {
    const manifest = JSON.parse(readFileSync(buildManifest, 'utf8'));
    return {
      wasmSha256: manifest?.binary?.sha256 ?? null,
      builtAt: manifest?.builtAt ?? null,
      toolchain: manifest?.toolchain?.version ?? null,
    };
  } catch {
    return null;
  }
}

function defaultCorpus() {
  const corpus = [];
  const replaysDir = resolve(workspace, 'games/replays');
  if (existsSync(replaysDir)) {
    for (const name of readdirSync(replaysDir).filter(name => name.endsWith('.rpy')).sort())
      corpus.push({file: name, source: 'replays', host: resolve(replaysDir, name)});
  } else {
    console.warn(`warn: missing replay directory ${replaysDir}`);
  }
  for (let index = 0; index < 4; index++) {
    const name = `demo${index}.rpy`;
    const host = resolve(workspace, 'artifacts/reference/raw', name);
    if (existsSync(host)) corpus.push({file: name, source: 'reference', host});
    else console.warn(`warn: missing demo replay ${host}`);
  }
  return corpus;
}

function explicitCorpus(paths) {
  return paths.map(value => {
    const guest = guestPath(workspace, value);
    return {file: basename(value), source: 'extra', host: resolve(workspace, '.' + guest)};
  });
}

function sha256File(path) {
  return createHash('sha256').update(readFileSync(path)).digest('hex');
}

function sanitize(value) {
  return value.replace(/[^A-Za-z0-9_.-]+/g, '_');
}

// 跑一条录像的一次尝试并解析日志。失败也返回已落盘的采样行（不静默丢弃）。
function runReplayOnce(entry, options, mode, index, attempt) {
  const slug = `${String(index).padStart(2, '0')}-${sanitize(entry.file.replace(/\.rpy$/i, ''))}-${mode}`;
  const logGuest = `${tempGuest}/${slug}.log`;
  const logHost = resolve(workspace, '.' + logGuest);
  const auxGuest = `${tempGuest}/${slug}.${mode === 'draws' ? 'draws' : 'events'}.jsonl`;
  const auxHost = resolve(workspace, '.' + auxGuest);
  mkdirSync(dirname(logHost), {recursive: true});
  for (const path of [logHost, auxHost]) rmSync(path, {force: true});

  const env = {
    ...process.env,
    TH13_DIGEST_LOG: logGuest,
    TH13_DIGEST_STRIDE: String(options.stride),
  };
  // 诊断开关会改变模拟/输出（freeze 门、ANM 负控、探针、状态流），门禁
  // 环境必须干净；TH13_HARNESS_WASM 也清掉，保证测的是标准产物。
  for (const key of ['TH13_FREEZE_AT', 'TH13_NO_ECL_SLOT_ANM', 'TH13_DAMAGE_PROBE',
                     'TH13_RNG_TRACE', 'TH13_SPIRIT_PROBE', 'TH13_STATE_LOG',
                     'TH13_LIFE_PROBE', 'TH13_SHOT_PROBE', 'TH13_HARNESS_WASM'])
    delete env[key];
  const harnessArgs = mode === 'draws'
    ? ['draws-replay', entry.guest, rawGuest, auxGuest, String(options.drawsFrames), '0']
    : ['replay', entry.guest, rawGuest, auxGuest, '0', '0'];
  if (mode === 'replay') {
    env.TH13_REPLAY_ALL = '1';
    env.TH13_REPLAY_FULL = '1';
  }
  const started = Date.now();
  // V8 的分层/JIT 方式会改变原生栈帧，同一 wasm 偶发 SIGSEGV。按固定周期
  // 轮换三种编译模式，让重试覆盖不同的栈行为（顺序固定 = 可复现）。
  const modes = [[], ['--liftoff-only'], ['--no-liftoff']];
  const flags = modes[(attempt - 1) % modes.length];
  const run = spawnSync(process.execPath,
    [...flags, '--experimental-wasi-unstable-preview1',
     resolve(port, 'tools/run-harness.mjs'), ...harnessArgs],
    {cwd: workspace, env, encoding: 'utf8', maxBuffer: 64 * 1024 * 1024, timeout: 900000});
  const seconds = (Date.now() - started) / 1000;

  let parsed = null;
  let parseError = null;
  if (existsSync(logHost)) {
    try {
      parsed = parseDigestLog(readFileSync(logHost, 'utf8'));
    } catch (error) {
      parseError = error.message;
    }
  } else {
    parseError = `digest log was not written: ${logHost}`;
  }
  const rows = parsed ? parsed.rows : [];
  const hasDrawsRows = rows.some(row => row.draws !== null);
  if (mode === 'draws' && run.status === 0 && parsed?.total != null && !parseError &&
      !hasDrawsRows)
    parseError = 'digest log has no draws rows';
  const completed = run.status === 0 && parsed != null && parsed.total != null && !parseError;
  const stages = parsed?.total?.stages ?? new Set(rows.map(row => row.stage)).size;
  let frames = parsed?.total?.frames ?? 0;
  let stop = null;
  if (!completed) {
    // 没有 total 行：把每关最后一个采样行当作停止点（金标只承诺已记录到的帧）。
    const perStage = new Map();
    for (const row of rows) perStage.set(row.stage, row);
    for (const row of perStage.values()) frames += row.frame + 1;
    const last = rows[rows.length - 1] ?? null;
    if (last) stop = {stage: last.stage, frame: last.frame};
  }
  const entryResult = {
    file: entry.file,
    source: entry.source,
    sha256: sha256File(entry.host),
    stages,
    frames,
    completed,
    stop,
    rows: rows.map(row => mode === 'draws'
      ? {stage: row.stage, frame: row.frame, state: row.state, events: row.events,
         draws: row.draws, stepped: row.stepped}
      : {stage: row.stage, frame: row.frame, state: row.state, events: row.events,
         draws: null, stepped: row.stepped}),
    seconds,
    exitCode: run.status,
    parseError,
    logHost,
    auxHost,
  };
  if (!completed) {
    const reason = run.error ? `spawn-error(${run.error.code ?? run.error.message})`
                             : run.signal ? `signal(${run.signal})`
                                          : `exit(${run.status})`;
    entryResult.diagnostic = reason;
    entryResult.detail = (run.stderr || run.stdout || '').trim().split('\n').slice(-3);
  }
  // 成功即清临时文件；失败时保留 digest 日志便于定位（--keep 时一律保留）。
  if (completed && !options.keep)
    for (const path of [logHost, auxHost]) if (existsSync(path)) rmSync(path, {force: true});
  return entryResult;
}

// V8 的原生/线性栈在本移植的深 ANM 路径上余量很小：同一 wasm 偶发
// SIGSEGV（受 JIT 模式与栈布局影响，ASLR/环境长度也会改变余量）。
// 门禁对每条录像重试 --attempts 次（默认 20，三种编译模式轮换）；只有
// 始终失败才记为 incomplete。确定性崩溃（如 Extra 关首帧）重试也不会
// 通过，金标如实记录停止点；--check 遇到差异还会整卷复核 --confirmations 次。
function runReplay(entry, options, mode, index) {
  let result = null;
  for (let attempt = 1; attempt <= options.attempts; attempt++) {
    result = runReplayOnce(entry, options, mode, index, attempt);
    result.attempts = attempt;
    if (result.completed) return result;
  }
  return result;
}

function buildDocument(corpus, options, mode) {
  const entries = [];
  for (let index = 0; index < corpus.length; index++) {
    const entry = {...corpus[index]};
    entry.guest = guestPath(workspace, entry.host);
    const result = runReplay(entry, options, mode, index);
    const status = result.completed ? `ok(a${result.attempts})` : `FAILED(${result.diagnostic})`;
    console.log(`[${index + 1}/${corpus.length}] ${entry.file}: stages=${result.stages} ` +
                `frames=${result.frames} rows=${result.rows.length} ${status} ` +
                `(${result.seconds.toFixed(1)}s)`);
    if (!result.completed) {
      const detail = result.parseError ?? (result.detail || []).join(' | ');
      console.log(`  stopped at stage=${result.stop?.stage ?? '-'} ` +
                  `frame=${result.stop?.frame ?? '-'} ${detail || ''}`);
      if (result.logHost && existsSync(result.logHost))
        console.log(`  digest log kept: ${result.logHost}`);
    }
    entries.push(result);
  }
  return makeDocument({
    mode,
    stride: options.stride,
    drawsFrames: mode === 'draws' ? options.drawsFrames : null,
    replays: entries,
    build: readBuild(),
  });
}

function printDivergence(diff) {
  const expected = typeof diff.expected === 'string' ? diff.expected : JSON.stringify(diff.expected);
  const actual = typeof diff.actual === 'string' ? diff.actual : JSON.stringify(diff.actual);
  console.log(`DIVERGENCE replay=${diff.replay} stage=${diff.stage} frame=${diff.frame} ` +
              `stream=${diff.stream} golden=${expected} current=${actual}`);
}

function usage() {
  return [
    'usage: node tools/regression-digest.mjs [--check] [--draws] [--replay <path> ...]',
    '                                      [--stride N] [--draws-frames N] [--attempts N]',
    '                                      [--confirmations N] [--keep]',
  ].join('\n');
}

function main(argv) {
  let options;
  try {
    options = parseArguments(argv);
  } catch (error) {
    console.error(error.message);
    console.error(usage());
    return 2;
  }
  if (options.help) {
    console.log(usage());
    return 0;
  }
  const fresh = staleHarnessInputs();
  if (fresh.missing) {
    console.error(`harness missing: ${fresh.wasm}`);
    console.error('build it first: node portable/build-harness.mjs');
    return 2;
  }
  if (fresh.newer.length) {
    console.error('harness is stale (source newer than wasm):');
    for (const path of fresh.newer.slice(0, 5)) console.error(`  ${path}`);
    console.error('rebuild it first: node portable/build-harness.mjs');
    return 2;
  }

  const mode = options.draws ? 'draws' : 'replay';
  const corpus = options.replays.length ? explicitCorpus(options.replays) : defaultCorpus();
  if (!corpus.length) {
    console.error('no replays in corpus');
    return 2;
  }
  const names = new Set();
  for (const entry of corpus) {
    if (names.has(entry.file)) {
      console.error(`duplicate replay name in corpus: ${entry.file}`);
      return 2;
    }
    names.add(entry.file);
  }
  const goldenPath = resolve(goldenDir,
    mode === 'draws' ? 'digests-draws.json' : 'digests.json');

  if (options.check) {
    if (!existsSync(goldenPath)) {
      console.error(`golden missing: ${goldenPath}`);
      return 2;
    }
    const golden = JSON.parse(readFileSync(goldenPath, 'utf8'));
    for (let round = 0; round <= options.confirmations; round++) {
      const current = buildDocument(corpus, options, mode);
      const diff = firstDivergence(golden, current);
      if (!diff) {
        console.log(`digest-ok ${mode} stride=${options.stride} ` +
                    `replays=${current.corpus.length} ` +
                    `build=${current.build?.wasmSha256 ?? '(none)'}`);
        return 0;
      }
      printDivergence(diff);
      if (round < options.confirmations) {
        console.log(`divergence not confirmed yet; rerunning corpus ` +
                    `(${round + 1}/${options.confirmations})`);
        continue;
      }
      const goldenBuild = golden.build?.wasmSha256 ?? '(none)';
      const currentBuild = current.build?.wasmSha256 ?? '(none)';
      console.log(`build provenance (not compared): golden=${goldenBuild} ` +
                  `current=${currentBuild}`);
      return 1;
    }
    return 1;
  }

  const document = buildDocument(corpus, options, mode);
  mkdirSync(goldenDir, {recursive: true});
  writeFileSync(goldenPath, JSON.stringify(document, null, 2) + '\n');
  const incomplete = document.corpus.filter(entry => !entry.completed).length;
  const unstable = document.corpus.filter(entry => entry.stable === false)
    .map(entry => entry.file);
  console.log(`digest-written ${goldenPath} replays=${document.corpus.length} ` +
              `incomplete=${incomplete} unstable=${unstable.join(',') || '(none)'} ` +
              `build=${document.build?.wasmSha256 ?? '(none)'}`);
  return 0;
}

if (process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url))
  process.exit(main(process.argv.slice(2)));
