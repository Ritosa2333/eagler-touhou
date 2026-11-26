// 绘制调用流结构对拍：harness `game-draw` 录下的每帧 Clear / 绘制操作，与原版 D3D 调用流（game-oracle.py --draw，
// 经 tools/oracle/draw-compare.py --structure-golden 折叠成每帧 sha1）逐帧比较。
//
// "结构" = 目标 / 视口 / 图元类型与数量 / 顶点格式 / 纹理 / 渲染状态子集，不含顶点坐标与矩阵——那部分由
// draw-compare.py 带 --eps 对拍（原版 x87 与移植版 SSE 单精度在个别顶点上差 1 ulp）；本测试守的是
// "画了哪些东西、按什么顺序、用什么状态"，例如 HUD 文字、得分飘字、Boss 血条这类整条绘制的有无。
// 行口径必须与 draw-compare.py 的 structure_lines 逐字一致。
import {test} from 'node:test';
import assert from 'node:assert/strict';
import {execFileSync, spawn} from 'node:child_process';
import {existsSync, readFileSync, mkdtempSync, rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join, resolve} from 'node:path';
import {structureDigest} from '../tools/draw-structure.mjs';

const here = resolve(import.meta.dirname);
const monorepo = resolve(here, '../../..');
const harness = process.env.TH16_HARNESS ?? resolve(here, 'build/harness');
const dat = process.env.TH16_DAT ?? resolve(monorepo, 'games/th16/original/th16.dat');
const needDat = {skip: !existsSync(dat) && 'original th16.dat not present'};

// 独立场景各自使用临时目录。按容器内存限制并发，外层可按测试名分片。
const oracleJobs = Number(process.env.TH16_ORACLE_JOBS ?? 2);
assert.ok(Number.isSafeInteger(oracleJobs) && oracleJobs > 0, 'TH16_ORACLE_JOBS must be a positive integer');
function runAsync(file, args, options = {}) {
  // 这些场景只比较 D3D 流，不读取状态快照；保留逐帧绘制，省去整局状态 JSON。
  if (args[0]?.endsWith('/game-oracle.py') && args.includes('--draw') && !args.includes('--every')) {
    args = [...args, '--every', String(0x7fffffff)];
  }
  const rest = {...options};
  delete rest.encoding;
  delete rest.maxBuffer;
  const stdio = rest.stdio ?? ['ignore', 'pipe', 'pipe'];
  return new Promise((resolve, reject) => {
    const child = spawn(file, args, {...rest, stdio});
    let stdout = '', stderr = '';
    if (child.stdout) child.stdout.on('data', data => { stdout += data; });
    if (child.stderr) child.stderr.on('data', data => { stderr += data; });
    child.on('error', reject);
    child.on('close', (code, signal) => {
      if (code === 0) resolve(stdout);
      else reject(new Error(`${file} exited with ${signal ?? code}\n${stderr}\n${stdout}`));
    });
  });
}

async function mapConcurrent(items, worker, limit = 8) {
  let next = 0;
  async function run() {
    while (next < items.length) {
      const index = next++;
      await worker(items[index], index);
    }
  }
  await Promise.all(Array.from({length: Math.min(limit, oracleJobs, items.length)}, run));
}

// 跑 harness game-draw，返回 {frame: ops[]}（只保留 golden 里有的帧）
function recordPort(golden, lastFrame, fromFrame) {
  const dir = mkdtempSync(join(tmpdir(), 'th16-draw-'));
  const out = join(dir, 'port.jsonl');
  const c = golden.config;
  try {
    execFileSync(harness, ['game-draw', dat, String(c.stage), String(c.shot), String(c.season), String(lastFrame + 1),
      c.input, '', '', String(c.difficulty), String(c.replay_slot ?? -1), String(c.lives), String(c.refill ?? 0),
      String(c.mode ?? 0), out, String(fromFrame)],
    // 绘制流写进 out；stdout 上的逐帧 game-run JSON 不需要（长序列会超过 maxBuffer），直接丢弃
    {encoding: 'utf8', stdio: ['ignore', 'ignore', 'pipe'], maxBuffer: 1 << 24});
    const frames = {};
    for (const line of readFileSync(out, 'utf8').split('\n')) {
      if (!line) continue;
      const rec = JSON.parse(line);
      if (golden.frames[String(rec.frame)] !== undefined) frames[rec.frame] = rec.ops;
    }
    return frames;
  } finally {
    rmSync(dir, {recursive: true, force: true});
  }
}

function checkGolden(name) {
  const golden = JSON.parse(readFileSync(resolve(here, 'golden', `${name}.json`), 'utf8'));
  const wanted = Object.keys(golden.frames).map(Number).sort((a, b) => a - b);
  const port = recordPort(golden, wanted.at(-1), wanted[0]);
  const textureSlots = golden.texture_slots ? new Set(golden.texture_slots) : null;
  const bad = [];
  for (const frame of wanted) {
    assert.ok(port[frame], `port recorded frame ${frame}`);
    if (structureDigest(port[frame], textureSlots) !== golden.frames[String(frame)]) bad.push(frame);
  }
  assert.deepEqual(bad, [], `${name}: draw structure differs from the original on frames ${bad.slice(0, 10).join(', ')}${bad.length > 10 ? ' …' : ''}`);
  return wanted.length;
}

test('draw structure: stage 1 Reimu / Spring Easy, frames 800–820 and 1000–1500 match the original call stream', needDat, () => {
  assert.equal(checkGolden('draw-st1'), 522);
});

test('draw structure: stage 1 Normal Reimu spring idle, frames 2000–2100 match the original call stream (geometry eps 1e-4, 101 frames)', needDat, () => {
  assert.equal(checkGolden('draw-st1-normal-2000'), 101);
});

test('draw structure: stage 1 Cirno / Autumn Normal, first 8 frames after load (HUD frame, logo, loading text camera switch)', needDat, () => {
  assert.equal(checkGolden('draw-st1-boot'), 8);
});

test('draw structure: stage 1 Reimu / Autumn Normal season release, frames 1030–1110 incl. the "+10" release bonus text (0x40de30)', needDat, () => {
  assert.equal(checkGolden('draw-st1-release'), 81);
});

test('draw structure: stage 1 continue menu Credit line (0x43edc0), frames 1050–1110', needDat, () => {
  assert.equal(checkGolden('draw-st1-menu-credit'), 61);
});

test('draw structure: stage 1 continue → replay slots → name entry (0x43e9b0 / 0x43eb50 / 0x43e730), frames 1120–1180', needDat, () => {
  assert.equal(checkGolden('draw-st1-menu-replay'), 61);
});

test('draw structure: stage 3 Normal spell 26 curve lasers (0x438750 strips via 0x4681f0) + boss distortion grid (0x418c70) + HP ring / effect strips (0x4632f0), frames 230–330', needDat, () => {
  assert.equal(checkGolden('draw-st3-curve-laser'), 101);
});

test('draw structure: stage 1 Normal STD ins 17 stage distortion (0x40c4a0 kind 1 via draw hook 4) + enemy main VMs on layer 8 (0x41b580 anm_layer_offset = 1), frames 4200–4400', needDat, () => {
  assert.equal(checkGolden('draw-st1-distortion'), 201);
});

test('draw structure: stage 1 Easy boss dialogue (draw hook 5 = 0x42b5f0 text follow) + boss life ring markers (0x42865b), frames 6300–6560', needDat, () => {
  assert.equal(checkGolden('draw-st1-dialogue'), 261);
});

test('draw structure: stage 1 Normal Cirno bomb burst particles (draw hook 3 = 0x406860 / 0x469890 line strips), frames 880–1300', needDat, () => {
  assert.equal(checkGolden('draw-st1-cirno-bomb'), 421);
});

test('draw structure: stage 1 Easy first spellcard start / name text / bonus countdown / death during spell, frames 7840–8200', needDat, () => {
  assert.equal(checkGolden('draw-st1-spell'), 361);
});

test('draw structure: stage 1 Easy player death (state 4 → 2 → 0 → 1) and respawn, frames 940–1150', needDat, () => {
  assert.equal(checkGolden('draw-st1-death'), 211);
});

test('draw structure: stage 1 → 2 transition (kind 5 full-screen fade 0x45c860, clear color reset + Loading VM 0x42d200, inherited fog camera 0x4097c0, kind 3 field fade-in 0x45c990), frames 12100–12300', needDat, () => {
  assert.equal(checkGolden('draw-st1-stage-transition'), 201);
});

test('draw structure: stage 4 Normal st04 background (mode 8 texture-matrix cache 0x467b0d) + mode 4 projected leaf particles (0x466390, camera right = facing_norm × up), frames 2–300', needDat, () => {
  assert.equal(checkGolden('draw-st4-leaves'), 299);
});

test('draw structure: stage 2 Easy Reimu spring idle, frames 10–419 match the original call stream (geometry eps 1e-4, 410 frames)', needDat, () => {
  assert.equal(checkGolden('draw-st2'), 410);
});

test('draw structure: stage 2 Easy Reimu spring boss dialogue (0x429ff0 / draw hook 5), frames 7570–7660', needDat, () => {
  assert.equal(checkGolden('draw-st2-dialogue'), 91);
});

test('draw structure: stage 6 Easy Reimu spring idle, frames 10–419 match the original call stream (geometry eps 1e-4, 410 frames)', needDat, () => {
  assert.equal(checkGolden('draw-st6'), 410);
});

test('draw structure: stage 6 Easy Reimu spring idle, frames 2000–2100 match the original call stream (geometry eps 1e-4, 101 frames)', needDat, () => {
  assert.equal(checkGolden('draw-st6-easy-2000'), 101);
});

test('draw structure: stage 3 Easy Reimu spring idle, frames 10–419 match the original call stream (geometry eps 1e-4, 410 frames)', needDat, () => {
  assert.equal(checkGolden('draw-st3'), 410);
});

test('draw structure: stage 3 Easy Reimu spring dialogue (portrait + text box, draw hook 5 = 0x42b5f0), frames 7690–7780 match the original call stream (geometry eps 1e-4, 91 frames)', needDat, () => {
  assert.equal(checkGolden('draw-st3-dialogue'), 91);
});

test('draw structure: stage 3 Normal Reimu spring idle, frames 4000–4100 match the original call stream (geometry eps 1e-4, 101 frames)', needDat, () => {
  assert.equal(checkGolden('draw-st3-normal-4000'), 101);
});

test('draw structure: stage 4 Easy Reimu spring idle, frames 10–419 match the original call stream (geometry eps 1e-4, 410 frames)', needDat, () => {
  assert.equal(checkGolden('draw-st4'), 410);
});

test('draw structure: stage 5 Easy Reimu spring idle, frames 10–419 match the original call stream (geometry eps 1e-4, 410 frames)', needDat, () => {
  assert.equal(checkGolden('draw-st5'), 410);
});

test('draw structure: Extra Reimu doyou idle, frames 10–419 match the original call stream (geometry eps 1e-4, 410 frames)', needDat, () => {
  assert.equal(checkGolden('draw-st7'), 410);
});

// 第 1–6 面 Easy 灵梦，子季节 1/2/3。输入与 stage-matrix 相同。窗口取第 590–599 帧，
// 由 game-oracle.py --draw 与 harness game-draw 逐顶点对拍。
test('stage 1–6 Easy Reimu summer/autumn/winter: frames 590–599 match original vertices', needDat, async () => {
  const exe = process.env.TH16_EXE ?? resolve(monorepo, 'games/th16/original/th16.exe');
  const datDir = resolve(monorepo, 'artifacts/th16/dat');
  const py = process.env.TH16_PYTHON ?? '/tmp/th16-venv/bin/python';
  const oracle = resolve(here, '../tools/oracle/game-oracle.py');
  const compare = resolve(here, '../tools/oracle/draw-compare.py');
  assert.ok(existsSync(exe) && existsSync(py) && existsSync(datDir), 'th16.exe / oracle python / dat dir');
  const input = '0:0x11,1500:0x13,1505:0x11,3000:0x13,3005:0x11';
  await mapConcurrent([1, 2, 3, 4, 5, 6], async stage => {
    for (const season of [1, 2, 3]) {
      const dir = mkdtempSync(join(tmpdir(), 'th16-season-draw-'));
      try {
        const state = join(dir, 'state.json');
        const oracleDraw = join(dir, 'oracle.draw.jsonl');
        const portDraw = join(dir, 'port.draw.jsonl');
        await runAsync(py, [oracle, exe, datDir, '600', state,
                          '--stage', String(stage), '--shot', '0', '--season', String(season), '--difficulty', '0',
                          '--lives', '8', '--refill-lives', '--input', input, '--enemies',
                          '--draw', '--draw-out', oracleDraw, '--draw-from', '590'],
                     {stdio: ['ignore', 'ignore', 'pipe']});
        await runAsync(harness, ['game-draw', dat, String(stage), '0', String(season), '600', input, '', '',
                               '0', '-1', '8', '1', '0', portDraw, '590'],
                     {stdio: ['ignore', 'ignore', 'pipe']});
        const compared = await runAsync('python3', [compare, oracleDraw, portDraw, '--frames', '590-599', '--eps', '1e-4'],
                                      {encoding: 'utf8'});
        process.stdout.write(compared);
        assert.match(compared, /frames=10 mismatches=0/, `stage ${stage} season ${season}`);
      } finally {
        rmSync(dir, {recursive: true, force: true});
      }
    }
  }, 8);
});

test('stage 1–6 Cirno/Aya/Marisa summer/autumn/winter: frames 590–599 match original vertices', needDat, async () => {
  const exe = process.env.TH16_EXE ?? resolve(monorepo, 'games/th16/original/th16.exe');
  const datDir = resolve(monorepo, 'artifacts/th16/dat');
  const py = process.env.TH16_PYTHON ?? '/tmp/th16-venv/bin/python';
  const oracle = resolve(here, '../tools/oracle/game-oracle.py');
  const compare = resolve(here, '../tools/oracle/draw-compare.py');
  assert.ok(existsSync(exe) && existsSync(py) && existsSync(datDir), 'th16.exe / oracle python / dat dir');
  const input = '0:0x11,1500:0x13,1505:0x11,3000:0x13,3005:0x11';
  await mapConcurrent([1, 2, 3, 4, 5, 6], async stage => {
  for (const shot of [1, 2, 3]) {
    for (const season of [1, 2, 3]) {
      const dir = mkdtempSync(join(tmpdir(), 'th16-shot-draw-'));
      try {
        const state = join(dir, 'state.json');
        const oracleDraw = join(dir, 'oracle.draw.jsonl');
        const portDraw = join(dir, 'port.draw.jsonl');
        await runAsync(py, [oracle, exe, datDir, '600', state,
                          '--stage', String(stage), '--shot', String(shot), '--season', String(season), '--difficulty', '0',
                          '--lives', '8', '--refill-lives', '--input', input, '--enemies',
                          '--draw', '--draw-out', oracleDraw, '--draw-from', '590'],
                     {stdio: ['ignore', 'ignore', 'pipe']});
        await runAsync(harness, ['game-draw', dat, String(stage), String(shot), String(season), '600', input, '', '',
                               '0', '-1', '8', '1', '0', portDraw, '590'],
                     {stdio: ['ignore', 'ignore', 'pipe']});
        const compared = await runAsync('python3', [compare, oracleDraw, portDraw, '--frames', '590-599', '--eps', '1e-4'],
                                      {encoding: 'utf8'});
        process.stdout.write(compared);
        assert.match(compared, /frames=10 mismatches=0/, `stage ${stage} shot ${shot} season ${season}`);
      } finally {
        rmSync(dir, {recursive: true, force: true});
      }
    }
  }
  }, 8);
});

test('stage 6 Easy Reimu spring idle: frames 25000–25009 match original vertices', needDat, () => {
  const exe = process.env.TH16_EXE ?? resolve(monorepo, 'games/th16/original/th16.exe');
  const datDir = resolve(monorepo, 'artifacts/th16/dat');
  const py = process.env.TH16_PYTHON ?? '/tmp/th16-venv/bin/python';
  const oracle = resolve(here, '../tools/oracle/game-oracle.py');
  const compare = resolve(here, '../tools/oracle/draw-compare.py');
  assert.ok(existsSync(exe) && existsSync(py) && existsSync(datDir), 'th16.exe / oracle python / dat dir');
  const dir = mkdtempSync(join(tmpdir(), 'th16-easy25000-draw-'));
  try {
    const state = join(dir, 'state.json');
    const oracleDraw = join(dir, 'oracle.draw.jsonl');
    const portDraw = join(dir, 'port.draw.jsonl');
    execFileSync(py, [oracle, exe, datDir, '25010', state,
                      '--stage', '6', '--shot', '0', '--season', '0', '--difficulty', '0',
                      '--lives', '8', '--refill-lives', '--input', '0:0', '--enemies',
                      '--draw', '--draw-out', oracleDraw, '--draw-from', '25000'],
                 {stdio: ['ignore', 'ignore', 'pipe']});
    execFileSync(harness, ['game-draw', dat, '6', '0', '0', '25010', '0:0', '', '',
                           '0', '-1', '8', '1', '0', portDraw, '25000'],
                 {stdio: ['ignore', 'ignore', 'pipe']});
    const compared = execFileSync('python3', [compare, oracleDraw, portDraw, '--frames', '25000-25009', '--eps', '1e-4'],
                                  {encoding: 'utf8'});
    process.stdout.write(compared);
    assert.match(compared, /frames=10 mismatches=0/, 'easy25000 stage 6');
  } finally {
    rmSync(dir, {recursive: true, force: true});
  }
});

test('stage 6 Easy Reimu spring idle: frames 24000–24009 match original vertices', needDat, () => {
  const exe = process.env.TH16_EXE ?? resolve(monorepo, 'games/th16/original/th16.exe');
  const datDir = resolve(monorepo, 'artifacts/th16/dat');
  const py = process.env.TH16_PYTHON ?? '/tmp/th16-venv/bin/python';
  const oracle = resolve(here, '../tools/oracle/game-oracle.py');
  const compare = resolve(here, '../tools/oracle/draw-compare.py');
  assert.ok(existsSync(exe) && existsSync(py) && existsSync(datDir), 'th16.exe / oracle python / dat dir');
  const dir = mkdtempSync(join(tmpdir(), 'th16-easy24000-draw-'));
  try {
    const state = join(dir, 'state.json');
    const oracleDraw = join(dir, 'oracle.draw.jsonl');
    const portDraw = join(dir, 'port.draw.jsonl');
    execFileSync(py, [oracle, exe, datDir, '24010', state,
                      '--stage', '6', '--shot', '0', '--season', '0', '--difficulty', '0',
                      '--lives', '8', '--refill-lives', '--input', '0:0', '--enemies',
                      '--draw', '--draw-out', oracleDraw, '--draw-from', '24000'],
                 {stdio: ['ignore', 'ignore', 'pipe']});
    execFileSync(harness, ['game-draw', dat, '6', '0', '0', '24010', '0:0', '', '',
                           '0', '-1', '8', '1', '0', portDraw, '24000'],
                 {stdio: ['ignore', 'ignore', 'pipe']});
    const compared = execFileSync('python3', [compare, oracleDraw, portDraw, '--frames', '24000-24009', '--eps', '1e-4'],
                                  {encoding: 'utf8'});
    process.stdout.write(compared);
    assert.match(compared, /frames=10 mismatches=0/, 'easy24000 stage 6');
  } finally {
    rmSync(dir, {recursive: true, force: true});
  }
});

test('stage 1–6 Easy Reimu spring idle: frames 22000–22009 match original vertices', needDat, async () => {
  const exe = process.env.TH16_EXE ?? resolve(monorepo, 'games/th16/original/th16.exe');
  const datDir = resolve(monorepo, 'artifacts/th16/dat');
  const py = process.env.TH16_PYTHON ?? '/tmp/th16-venv/bin/python';
  const oracle = resolve(here, '../tools/oracle/game-oracle.py');
  const compare = resolve(here, '../tools/oracle/draw-compare.py');
  assert.ok(existsSync(exe) && existsSync(py) && existsSync(datDir), 'th16.exe / oracle python / dat dir');
  await mapConcurrent([1, 2, 3, 4, 5, 6], async stage => {
    const dir = mkdtempSync(join(tmpdir(), 'th16-easy22000-draw-'));
    try {
      const state = join(dir, 'state.json');
      const oracleDraw = join(dir, 'oracle.draw.jsonl');
      const portDraw = join(dir, 'port.draw.jsonl');
      await runAsync(py, [oracle, exe, datDir, '22010', state,
                        '--stage', String(stage), '--shot', '0', '--season', '0', '--difficulty', '0',
                        '--lives', '8', '--refill-lives', '--input', '0:0', '--enemies',
                        '--draw', '--draw-out', oracleDraw, '--draw-from', '22000'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      await runAsync(harness, ['game-draw', dat, String(stage), '0', '0', '22010', '0:0', '', '',
                             '0', '-1', '8', '1', '0', portDraw, '22000'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      const compared = await runAsync('python3', [compare, oracleDraw, portDraw, '--frames', '22000-22009', '--eps', '1e-4'],
                                    {encoding: 'utf8'});
      process.stdout.write(compared);
      assert.match(compared, /frames=10 mismatches=0/, `easy22000 stage ${stage}`);
    } finally {
      rmSync(dir, {recursive: true, force: true});
    }
  }, 8);
});

// 第 4 面第 18700 帧前后 Boss 登场：敌机析构 0x41ba10 注销 Boss 槽之后，槽内不再残留已消失的 id（否则 0x417580 落到
// 链表最后一只，Gui 给同一只 Boss 画第二套血环：front.anm 244 / 245 / 246 三条 38 节点条带各画两遍）。
test('stage 1–6 Easy Reimu spring idle: frames 20000–20009 match original vertices', needDat, async () => {
  const exe = process.env.TH16_EXE ?? resolve(monorepo, 'games/th16/original/th16.exe');
  const datDir = resolve(monorepo, 'artifacts/th16/dat');
  const py = process.env.TH16_PYTHON ?? '/tmp/th16-venv/bin/python';
  const oracle = resolve(here, '../tools/oracle/game-oracle.py');
  const compare = resolve(here, '../tools/oracle/draw-compare.py');
  assert.ok(existsSync(exe) && existsSync(py) && existsSync(datDir), 'th16.exe / oracle python / dat dir');
  await mapConcurrent([1, 2, 3, 4, 5, 6], async stage => {
    const dir = mkdtempSync(join(tmpdir(), 'th16-easy20000-draw-'));
    try {
      const state = join(dir, 'state.json');
      const oracleDraw = join(dir, 'oracle.draw.jsonl');
      const portDraw = join(dir, 'port.draw.jsonl');
      await runAsync(py, [oracle, exe, datDir, '20010', state,
                        '--stage', String(stage), '--shot', '0', '--season', '0', '--difficulty', '0',
                        '--lives', '8', '--refill-lives', '--input', '0:0', '--enemies',
                        '--draw', '--draw-out', oracleDraw, '--draw-from', '20000'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      await runAsync(harness, ['game-draw', dat, String(stage), '0', '0', '20010', '0:0', '', '',
                             '0', '-1', '8', '1', '0', portDraw, '20000'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      const compared = await runAsync('python3', [compare, oracleDraw, portDraw, '--frames', '20000-20009', '--eps', '1e-4'],
                                    {encoding: 'utf8'});
      process.stdout.write(compared);
      assert.match(compared, /frames=10 mismatches=0/, `easy20000 stage ${stage}`);
    } finally {
      rmSync(dir, {recursive: true, force: true});
    }
  }, 8);
});

test('stage 1–6 Easy Reimu spring idle: frames 18000–18009 match original vertices', needDat, async () => {
  const exe = process.env.TH16_EXE ?? resolve(monorepo, 'games/th16/original/th16.exe');
  const datDir = resolve(monorepo, 'artifacts/th16/dat');
  const py = process.env.TH16_PYTHON ?? '/tmp/th16-venv/bin/python';
  const oracle = resolve(here, '../tools/oracle/game-oracle.py');
  const compare = resolve(here, '../tools/oracle/draw-compare.py');
  assert.ok(existsSync(exe) && existsSync(py) && existsSync(datDir), 'th16.exe / oracle python / dat dir');
  await mapConcurrent([1, 2, 3, 4, 5, 6], async stage => {
    const dir = mkdtempSync(join(tmpdir(), 'th16-easy18000-draw-'));
    try {
      const state = join(dir, 'state.json');
      const oracleDraw = join(dir, 'oracle.draw.jsonl');
      const portDraw = join(dir, 'port.draw.jsonl');
      await runAsync(py, [oracle, exe, datDir, '18010', state,
                        '--stage', String(stage), '--shot', '0', '--season', '0', '--difficulty', '0',
                        '--lives', '8', '--refill-lives', '--input', '0:0', '--enemies',
                        '--draw', '--draw-out', oracleDraw, '--draw-from', '18000'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      await runAsync(harness, ['game-draw', dat, String(stage), '0', '0', '18010', '0:0', '', '',
                             '0', '-1', '8', '1', '0', portDraw, '18000'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      const compared = await runAsync('python3', [compare, oracleDraw, portDraw, '--frames', '18000-18009', '--eps', '1e-4'],
                                    {encoding: 'utf8'});
      process.stdout.write(compared);
      assert.match(compared, /frames=10 mismatches=0/, `easy18000 stage ${stage}`);
    } finally {
      rmSync(dir, {recursive: true, force: true});
    }
  }, 8);
});

test('stage 1–6 Easy Reimu spring idle: frames 16000–16009 match original vertices', needDat, async () => {
  const exe = process.env.TH16_EXE ?? resolve(monorepo, 'games/th16/original/th16.exe');
  const datDir = resolve(monorepo, 'artifacts/th16/dat');
  const py = process.env.TH16_PYTHON ?? '/tmp/th16-venv/bin/python';
  const oracle = resolve(here, '../tools/oracle/game-oracle.py');
  const compare = resolve(here, '../tools/oracle/draw-compare.py');
  assert.ok(existsSync(exe) && existsSync(py) && existsSync(datDir), 'th16.exe / oracle python / dat dir');
  await mapConcurrent([1, 2, 3, 4, 5, 6], async stage => {
    const dir = mkdtempSync(join(tmpdir(), 'th16-easy16000-draw-'));
    try {
      const state = join(dir, 'state.json');
      const oracleDraw = join(dir, 'oracle.draw.jsonl');
      const portDraw = join(dir, 'port.draw.jsonl');
      await runAsync(py, [oracle, exe, datDir, '16010', state,
                        '--stage', String(stage), '--shot', '0', '--season', '0', '--difficulty', '0',
                        '--lives', '8', '--refill-lives', '--input', '0:0', '--enemies',
                        '--draw', '--draw-out', oracleDraw, '--draw-from', '16000'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      await runAsync(harness, ['game-draw', dat, String(stage), '0', '0', '16010', '0:0', '', '',
                             '0', '-1', '8', '1', '0', portDraw, '16000'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      const compared = await runAsync('python3', [compare, oracleDraw, portDraw, '--frames', '16000-16009', '--eps', '1e-4'],
                                    {encoding: 'utf8'});
      process.stdout.write(compared);
      assert.match(compared, /frames=10 mismatches=0/, `easy16000 stage ${stage}`);
    } finally {
      rmSync(dir, {recursive: true, force: true});
    }
  }, 8);
});

test('stage 1–6 Easy Reimu spring idle: frames 14000–14009 match original vertices', needDat, async () => {
  const exe = process.env.TH16_EXE ?? resolve(monorepo, 'games/th16/original/th16.exe');
  const datDir = resolve(monorepo, 'artifacts/th16/dat');
  const py = process.env.TH16_PYTHON ?? '/tmp/th16-venv/bin/python';
  const oracle = resolve(here, '../tools/oracle/game-oracle.py');
  const compare = resolve(here, '../tools/oracle/draw-compare.py');
  assert.ok(existsSync(exe) && existsSync(py) && existsSync(datDir), 'th16.exe / oracle python / dat dir');
  await mapConcurrent([1, 2, 3, 4, 5, 6], async stage => {
    const dir = mkdtempSync(join(tmpdir(), 'th16-easy14000-draw-'));
    try {
      const state = join(dir, 'state.json');
      const oracleDraw = join(dir, 'oracle.draw.jsonl');
      const portDraw = join(dir, 'port.draw.jsonl');
      await runAsync(py, [oracle, exe, datDir, '14010', state,
                        '--stage', String(stage), '--shot', '0', '--season', '0', '--difficulty', '0',
                        '--lives', '8', '--refill-lives', '--input', '0:0', '--enemies',
                        '--draw', '--draw-out', oracleDraw, '--draw-from', '14000'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      await runAsync(harness, ['game-draw', dat, String(stage), '0', '0', '14010', '0:0', '', '',
                             '0', '-1', '8', '1', '0', portDraw, '14000'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      const compared = await runAsync('python3', [compare, oracleDraw, portDraw, '--frames', '14000-14009', '--eps', '1e-4'],
                                    {encoding: 'utf8'});
      process.stdout.write(compared);
      assert.match(compared, /frames=10 mismatches=0/, `easy14000 stage ${stage}`);
    } finally {
      rmSync(dir, {recursive: true, force: true});
    }
  }, 8);
});

test('stage 1–6 Easy Reimu spring idle: frames 12000–12009 match original vertices', needDat, async () => {
  const exe = process.env.TH16_EXE ?? resolve(monorepo, 'games/th16/original/th16.exe');
  const datDir = resolve(monorepo, 'artifacts/th16/dat');
  const py = process.env.TH16_PYTHON ?? '/tmp/th16-venv/bin/python';
  const oracle = resolve(here, '../tools/oracle/game-oracle.py');
  const compare = resolve(here, '../tools/oracle/draw-compare.py');
  assert.ok(existsSync(exe) && existsSync(py) && existsSync(datDir), 'th16.exe / oracle python / dat dir');
  await mapConcurrent([1, 2, 3, 4, 5, 6], async stage => {
    const dir = mkdtempSync(join(tmpdir(), 'th16-easy12000-draw-'));
    try {
      const state = join(dir, 'state.json');
      const oracleDraw = join(dir, 'oracle.draw.jsonl');
      const portDraw = join(dir, 'port.draw.jsonl');
      await runAsync(py, [oracle, exe, datDir, '12010', state,
                        '--stage', String(stage), '--shot', '0', '--season', '0', '--difficulty', '0',
                        '--lives', '8', '--refill-lives', '--input', '0:0', '--enemies',
                        '--draw', '--draw-out', oracleDraw, '--draw-from', '12000'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      await runAsync(harness, ['game-draw', dat, String(stage), '0', '0', '12010', '0:0', '', '',
                             '0', '-1', '8', '1', '0', portDraw, '12000'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      const compared = await runAsync('python3', [compare, oracleDraw, portDraw, '--frames', '12000-12009', '--eps', '1e-4'],
                                    {encoding: 'utf8'});
      process.stdout.write(compared);
      assert.match(compared, /frames=10 mismatches=0/, `easy12000 stage ${stage}`);
    } finally {
      rmSync(dir, {recursive: true, force: true});
    }
  }, 8);
});

test('stage 1–6 Easy Reimu spring idle: frames 10000–10009 match original vertices', needDat, async () => {
  const exe = process.env.TH16_EXE ?? resolve(monorepo, 'games/th16/original/th16.exe');
  const datDir = resolve(monorepo, 'artifacts/th16/dat');
  const py = process.env.TH16_PYTHON ?? '/tmp/th16-venv/bin/python';
  const oracle = resolve(here, '../tools/oracle/game-oracle.py');
  const compare = resolve(here, '../tools/oracle/draw-compare.py');
  assert.ok(existsSync(exe) && existsSync(py) && existsSync(datDir), 'th16.exe / oracle python / dat dir');
  await mapConcurrent([1, 2, 3, 4, 5, 6], async stage => {
    const dir = mkdtempSync(join(tmpdir(), 'th16-easy10000-draw-'));
    try {
      const state = join(dir, 'state.json');
      const oracleDraw = join(dir, 'oracle.draw.jsonl');
      const portDraw = join(dir, 'port.draw.jsonl');
      await runAsync(py, [oracle, exe, datDir, '10010', state,
                        '--stage', String(stage), '--shot', '0', '--season', '0', '--difficulty', '0',
                        '--lives', '8', '--refill-lives', '--input', '0:0', '--enemies',
                        '--draw', '--draw-out', oracleDraw, '--draw-from', '10000'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      await runAsync(harness, ['game-draw', dat, String(stage), '0', '0', '10010', '0:0', '', '',
                             '0', '-1', '8', '1', '0', portDraw, '10000'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      const compared = await runAsync('python3', [compare, oracleDraw, portDraw, '--frames', '10000-10009', '--eps', '1e-4'],
                                    {encoding: 'utf8'});
      process.stdout.write(compared);
      assert.match(compared, /frames=10 mismatches=0/, `easy10000 stage ${stage}`);
    } finally {
      rmSync(dir, {recursive: true, force: true});
    }
  }, 8);
});

test('idle frames 10000–10009: stage 1 Easy, stage 3 Easy, stage 6 Easy, Extra doyou match original vertices', needDat, async () => {
  const exe = process.env.TH16_EXE ?? resolve(monorepo, 'games/th16/original/th16.exe');
  const datDir = resolve(monorepo, 'artifacts/th16/dat');
  const py = process.env.TH16_PYTHON ?? '/tmp/th16-venv/bin/python';
  const oracle = resolve(here, '../tools/oracle/game-oracle.py');
  const compare = resolve(here, '../tools/oracle/draw-compare.py');
  assert.ok(existsSync(exe) && existsSync(py) && existsSync(datDir), 'th16.exe / oracle python / dat dir');
  const cases = [
    {stage: 1, season: 0, difficulty: 0},
    {stage: 3, season: 0, difficulty: 0},
    {stage: 6, season: 0, difficulty: 0},
    {stage: 7, season: 4, difficulty: 4},
  ];
  await mapConcurrent(cases, async c => {
    const dir = mkdtempSync(join(tmpdir(), 'th16-idle10000-draw-'));
    try {
      const state = join(dir, 'state.json');
      const oracleDraw = join(dir, 'oracle.draw.jsonl');
      const portDraw = join(dir, 'port.draw.jsonl');
      await runAsync(py, [oracle, exe, datDir, '10010', state,
                        '--stage', String(c.stage), '--shot', '0', '--season', String(c.season),
                        '--difficulty', String(c.difficulty),
                        '--lives', '8', '--refill-lives', '--input', '0:0', '--enemies',
                        '--draw', '--draw-out', oracleDraw, '--draw-from', '10000'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      await runAsync(harness, ['game-draw', dat, String(c.stage), '0', String(c.season), '10010', '0:0', '', '',
                             String(c.difficulty), '-1', '8', '1', '0', portDraw, '10000'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      const compared = await runAsync('python3', [compare, oracleDraw, portDraw, '--frames', '10000-10009', '--eps', '1e-4'],
                                    {encoding: 'utf8'});
      process.stdout.write(compared);
      assert.match(compared, /frames=10 mismatches=0/,
                   `idle10000 stage ${c.stage} season ${c.season} difficulty ${c.difficulty}`);
    } finally {
      rmSync(dir, {recursive: true, force: true});
    }
  }, 8);
});

test('stage 1–6 Easy Reimu spring idle: frames 8000–8009 match original vertices', needDat, async () => {
  const exe = process.env.TH16_EXE ?? resolve(monorepo, 'games/th16/original/th16.exe');
  const datDir = resolve(monorepo, 'artifacts/th16/dat');
  const py = process.env.TH16_PYTHON ?? '/tmp/th16-venv/bin/python';
  const oracle = resolve(here, '../tools/oracle/game-oracle.py');
  const compare = resolve(here, '../tools/oracle/draw-compare.py');
  assert.ok(existsSync(exe) && existsSync(py) && existsSync(datDir), 'th16.exe / oracle python / dat dir');
  await mapConcurrent([1, 2, 3, 4, 5, 6], async stage => {
    const dir = mkdtempSync(join(tmpdir(), 'th16-easy8000-draw-'));
    try {
      const state = join(dir, 'state.json');
      const oracleDraw = join(dir, 'oracle.draw.jsonl');
      const portDraw = join(dir, 'port.draw.jsonl');
      await runAsync(py, [oracle, exe, datDir, '8010', state,
                        '--stage', String(stage), '--shot', '0', '--season', '0', '--difficulty', '0',
                        '--lives', '8', '--refill-lives', '--input', '0:0', '--enemies',
                        '--draw', '--draw-out', oracleDraw, '--draw-from', '8000'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      await runAsync(harness, ['game-draw', dat, String(stage), '0', '0', '8010', '0:0', '', '',
                             '0', '-1', '8', '1', '0', portDraw, '8000'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      const compared = await runAsync('python3', [compare, oracleDraw, portDraw, '--frames', '8000-8009', '--eps', '1e-4'],
                                    {encoding: 'utf8'});
      process.stdout.write(compared);
      assert.match(compared, /frames=10 mismatches=0/, `easy8000 stage ${stage}`);
    } finally {
      rmSync(dir, {recursive: true, force: true});
    }
  }, 8);
});

test('idle frames 8000–8009: stage 1 Easy, stage 3 Normal, stage 6 Easy, Extra doyou match original vertices', needDat, async () => {
  const exe = process.env.TH16_EXE ?? resolve(monorepo, 'games/th16/original/th16.exe');
  const datDir = resolve(monorepo, 'artifacts/th16/dat');
  const py = process.env.TH16_PYTHON ?? '/tmp/th16-venv/bin/python';
  const oracle = resolve(here, '../tools/oracle/game-oracle.py');
  const compare = resolve(here, '../tools/oracle/draw-compare.py');
  assert.ok(existsSync(exe) && existsSync(py) && existsSync(datDir), 'th16.exe / oracle python / dat dir');
  const cases = [
    {stage: 1, season: 0, difficulty: 0},
    {stage: 3, season: 0, difficulty: 1},
    {stage: 6, season: 0, difficulty: 0},
    {stage: 7, season: 4, difficulty: 4},
  ];
  await mapConcurrent(cases, async c => {
    const dir = mkdtempSync(join(tmpdir(), 'th16-idle8000-draw-'));
    try {
      const state = join(dir, 'state.json');
      const oracleDraw = join(dir, 'oracle.draw.jsonl');
      const portDraw = join(dir, 'port.draw.jsonl');
      await runAsync(py, [oracle, exe, datDir, '8010', state,
                        '--stage', String(c.stage), '--shot', '0', '--season', String(c.season),
                        '--difficulty', String(c.difficulty),
                        '--lives', '8', '--refill-lives', '--input', '0:0', '--enemies',
                        '--draw', '--draw-out', oracleDraw, '--draw-from', '8000'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      await runAsync(harness, ['game-draw', dat, String(c.stage), '0', String(c.season), '8010', '0:0', '', '',
                             String(c.difficulty), '-1', '8', '1', '0', portDraw, '8000'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      const compared = await runAsync('python3', [compare, oracleDraw, portDraw, '--frames', '8000-8009', '--eps', '1e-4'],
                                    {encoding: 'utf8'});
      process.stdout.write(compared);
      assert.match(compared, /frames=10 mismatches=0/,
                   `idle8000 stage ${c.stage} season ${c.season} difficulty ${c.difficulty}`);
    } finally {
      rmSync(dir, {recursive: true, force: true});
    }
  }, 8);
});

test('Extra Reimu spring/summer/autumn/winter/doyou idle: frames 6000–6009 match original vertices', needDat, async () => {
  const exe = process.env.TH16_EXE ?? resolve(monorepo, 'games/th16/original/th16.exe');
  const datDir = resolve(monorepo, 'artifacts/th16/dat');
  const py = process.env.TH16_PYTHON ?? '/tmp/th16-venv/bin/python';
  const oracle = resolve(here, '../tools/oracle/game-oracle.py');
  const compare = resolve(here, '../tools/oracle/draw-compare.py');
  assert.ok(existsSync(exe) && existsSync(py) && existsSync(datDir), 'th16.exe / oracle python / dat dir');
  await mapConcurrent([0, 1, 2, 3, 4], async season => {
    const dir = mkdtempSync(join(tmpdir(), 'th16-extra6000-draw-'));
    try {
      const state = join(dir, 'state.json');
      const oracleDraw = join(dir, 'oracle.draw.jsonl');
      const portDraw = join(dir, 'port.draw.jsonl');
      await runAsync(py, [oracle, exe, datDir, '6010', state,
                        '--stage', '7', '--shot', '0', '--season', String(season), '--difficulty', '4',
                        '--lives', '8', '--refill-lives', '--input', '0:0', '--enemies',
                        '--draw', '--draw-out', oracleDraw, '--draw-from', '6000'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      await runAsync(harness, ['game-draw', dat, '7', '0', String(season), '6010', '0:0', '', '',
                             '4', '-1', '8', '1', '0', portDraw, '6000'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      const compared = await runAsync('python3', [compare, oracleDraw, portDraw, '--frames', '6000-6009', '--eps', '1e-4'],
                                    {encoding: 'utf8'});
      process.stdout.write(compared);
      assert.match(compared, /frames=10 mismatches=0/, `extra6000 season ${season}`);
    } finally {
      rmSync(dir, {recursive: true, force: true});
    }
  }, 8);
});

test('stage 1–6 Easy Reimu spring idle: frames 6000–6009 match original vertices', needDat, async () => {
  const exe = process.env.TH16_EXE ?? resolve(monorepo, 'games/th16/original/th16.exe');
  const datDir = resolve(monorepo, 'artifacts/th16/dat');
  const py = process.env.TH16_PYTHON ?? '/tmp/th16-venv/bin/python';
  const oracle = resolve(here, '../tools/oracle/game-oracle.py');
  const compare = resolve(here, '../tools/oracle/draw-compare.py');
  assert.ok(existsSync(exe) && existsSync(py) && existsSync(datDir), 'th16.exe / oracle python / dat dir');
  await mapConcurrent([1, 2, 3, 4, 5, 6], async stage => {
    const dir = mkdtempSync(join(tmpdir(), 'th16-idle6000-draw-'));
    try {
      const state = join(dir, 'state.json');
      const oracleDraw = join(dir, 'oracle.draw.jsonl');
      const portDraw = join(dir, 'port.draw.jsonl');
      await runAsync(py, [oracle, exe, datDir, '6010', state,
                        '--stage', String(stage), '--shot', '0', '--season', '0', '--difficulty', '0',
                        '--lives', '8', '--refill-lives', '--input', '0:0', '--enemies',
                        '--draw', '--draw-out', oracleDraw, '--draw-from', '6000'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      await runAsync(harness, ['game-draw', dat, String(stage), '0', '0', '6010', '0:0', '', '',
                             '0', '-1', '8', '1', '0', portDraw, '6000'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      const compared = await runAsync('python3', [compare, oracleDraw, portDraw, '--frames', '6000-6009', '--eps', '1e-4'],
                                    {encoding: 'utf8'});
      process.stdout.write(compared);
      assert.match(compared, /frames=10 mismatches=0/, `idle6000 stage ${stage}`);
    } finally {
      rmSync(dir, {recursive: true, force: true});
    }
  }, 8);
});

test('stage 1, 3, 6 and Extra: frames 6000–6009 and Extra 4000–4009 match original vertices', needDat, async () => {
  const exe = process.env.TH16_EXE ?? resolve(monorepo, 'games/th16/original/th16.exe');
  const datDir = resolve(monorepo, 'artifacts/th16/dat');
  const py = process.env.TH16_PYTHON ?? '/tmp/th16-venv/bin/python';
  const oracle = resolve(here, '../tools/oracle/game-oracle.py');
  const compare = resolve(here, '../tools/oracle/draw-compare.py');
  assert.ok(existsSync(exe) && existsSync(py) && existsSync(datDir), 'th16.exe / oracle python / dat dir');
  const cases = [
    {stage: 1, shot: 0, season: 0, difficulty: 0, frames: 6010, from: 6000, window: '6000-6009'},
    {stage: 3, shot: 0, season: 0, difficulty: 1, frames: 6010, from: 6000, window: '6000-6009'},
    {stage: 6, shot: 0, season: 3, difficulty: 3, frames: 6010, from: 6000, window: '6000-6009'},
    {stage: 7, shot: 0, season: 0, difficulty: 4, frames: 4010, from: 4000, window: '4000-4009'},
  ];
  await mapConcurrent(cases, async c => {
    const dir = mkdtempSync(join(tmpdir(), 'th16-long-draw-'));
    try {
      const state = join(dir, 'state.json');
      const oracleDraw = join(dir, 'oracle.draw.jsonl');
      const portDraw = join(dir, 'port.draw.jsonl');
      await runAsync(py, [oracle, exe, datDir, String(c.frames), state,
                        '--stage', String(c.stage), '--shot', String(c.shot), '--season', String(c.season),
                        '--difficulty', String(c.difficulty),
                        '--lives', '8', '--refill-lives', '--input', '0:0', '--enemies',
                        '--draw', '--draw-out', oracleDraw, '--draw-from', String(c.from)],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      await runAsync(harness, ['game-draw', dat, String(c.stage), String(c.shot), String(c.season), String(c.frames),
                             '0:0', '', '', String(c.difficulty), '-1', '8', '1', '0', portDraw, String(c.from)],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      const compared = await runAsync('python3', [compare, oracleDraw, portDraw, '--frames', c.window, '--eps', '1e-4'],
                                    {encoding: 'utf8'});
      process.stdout.write(compared);
      assert.match(compared, /frames=10 mismatches=0/,
                   `long stage ${c.stage} shot ${c.shot} season ${c.season} difficulty ${c.difficulty} ${c.window}`);
    } finally {
      rmSync(dir, {recursive: true, force: true});
    }
  }, 8);
});

test('stage 1–6 idle and firing: frames 4000–4009 match original vertices', needDat, async () => {
  const exe = process.env.TH16_EXE ?? resolve(monorepo, 'games/th16/original/th16.exe');
  const datDir = resolve(monorepo, 'artifacts/th16/dat');
  const py = process.env.TH16_PYTHON ?? '/tmp/th16-venv/bin/python';
  const oracle = resolve(here, '../tools/oracle/game-oracle.py');
  const compare = resolve(here, '../tools/oracle/draw-compare.py');
  assert.ok(existsSync(exe) && existsSync(py) && existsSync(datDir), 'th16.exe / oracle python / dat dir');
  const firing = '0:0x11,1500:0x13,1505:0x11,3000:0x13,3005:0x11';
  // 每个关卡、机体、季节、难度各进一次，空闲与射击交替。全组合 768 组一次跑不完。
  const cases = [];
  for (const stage of [1, 2, 3, 4, 5, 6]) {
    for (let i = 0; i < 4; ++i) {
      cases.push({stage, shot: i, season: i, difficulty: i, input: i % 2 === 0 ? '0:0' : firing});
      cases.push({stage, shot: i, season: (i + 1) % 4, difficulty: (i + 2) % 4, input: i % 2 === 0 ? firing : '0:0'});
    }
  }
  await mapConcurrent(cases, async c => {
    const dir = mkdtempSync(join(tmpdir(), 'th16-f4000-draw-'));
    try {
      const state = join(dir, 'state.json');
      const oracleDraw = join(dir, 'oracle.draw.jsonl');
      const portDraw = join(dir, 'port.draw.jsonl');
      await runAsync(py, [oracle, exe, datDir, '4010', state,
                        '--stage', String(c.stage), '--shot', String(c.shot), '--season', String(c.season),
                        '--difficulty', String(c.difficulty),
                        '--lives', '8', '--refill-lives', '--input', c.input, '--enemies',
                        '--draw', '--draw-out', oracleDraw, '--draw-from', '4000'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      await runAsync(harness, ['game-draw', dat, String(c.stage), String(c.shot), String(c.season), '4010', c.input, '', '',
                             String(c.difficulty), '-1', '8', '1', '0', portDraw, '4000'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      const compared = await runAsync('python3', [compare, oracleDraw, portDraw, '--frames', '4000-4009', '--eps', '1e-4'],
                                    {encoding: 'utf8'});
      process.stdout.write(compared);
      assert.match(compared, /frames=10 mismatches=0/,
                   `f4000 stage ${c.stage} shot ${c.shot} season ${c.season} difficulty ${c.difficulty}`);
    } finally {
      rmSync(dir, {recursive: true, force: true});
    }
  }, 16);
});

test('stage 1–6 and Extra idle: frames 2000–2009 match original vertices', needDat, async () => {
  const exe = process.env.TH16_EXE ?? resolve(monorepo, 'games/th16/original/th16.exe');
  const datDir = resolve(monorepo, 'artifacts/th16/dat');
  const py = process.env.TH16_PYTHON ?? '/tmp/th16-venv/bin/python';
  const oracle = resolve(here, '../tools/oracle/game-oracle.py');
  const compare = resolve(here, '../tools/oracle/draw-compare.py');
  assert.ok(existsSync(exe) && existsSync(py) && existsSync(datDir), 'th16.exe / oracle python / dat dir');
  const cases = [];
  for (const stage of [1, 2, 3, 4, 5, 6]) {
    for (const shot of [0, 1, 2, 3]) {
      for (const season of [0, 1, 2, 3]) {
        for (const difficulty of [0, 1, 2, 3]) {
          cases.push({stage, shot, season, difficulty});
        }
      }
    }
  }
  for (const shot of [0, 1, 2, 3]) {
    for (const season of [0, 1, 2, 3, 4]) {
      cases.push({stage: 7, shot, season, difficulty: 4});
    }
  }
  await mapConcurrent(cases, async c => {
    const dir = mkdtempSync(join(tmpdir(), 'th16-idle2000-draw-'));
    try {
      const state = join(dir, 'state.json');
      const oracleDraw = join(dir, 'oracle.draw.jsonl');
      const portDraw = join(dir, 'port.draw.jsonl');
      await runAsync(py, [oracle, exe, datDir, '2010', state,
                        '--stage', String(c.stage), '--shot', String(c.shot), '--season', String(c.season),
                        '--difficulty', String(c.difficulty),
                        '--lives', '8', '--refill-lives', '--input', '0:0', '--enemies',
                        '--draw', '--draw-out', oracleDraw, '--draw-from', '2000'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      await runAsync(harness, ['game-draw', dat, String(c.stage), String(c.shot), String(c.season), '2010', '0:0', '', '',
                             String(c.difficulty), '-1', '8', '1', '0', portDraw, '2000'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      const compared = await runAsync('python3', [compare, oracleDraw, portDraw, '--frames', '2000-2009', '--eps', '1e-4'],
                                    {encoding: 'utf8'});
      process.stdout.write(compared);
      assert.match(compared, /frames=10 mismatches=0/,
                   `idle2000 stage ${c.stage} shot ${c.shot} season ${c.season} difficulty ${c.difficulty}`);
    } finally {
      rmSync(dir, {recursive: true, force: true});
    }
  }, 32);
});

test('stage 1–6 four shots spring four difficulties: frames 590–599 match original vertices', needDat, async () => {
  const exe = process.env.TH16_EXE ?? resolve(monorepo, 'games/th16/original/th16.exe');
  const datDir = resolve(monorepo, 'artifacts/th16/dat');
  const py = process.env.TH16_PYTHON ?? '/tmp/th16-venv/bin/python';
  const oracle = resolve(here, '../tools/oracle/game-oracle.py');
  const compare = resolve(here, '../tools/oracle/draw-compare.py');
  assert.ok(existsSync(exe) && existsSync(py) && existsSync(datDir), 'th16.exe / oracle python / dat dir');
  const input = '0:0x11,1500:0x13,1505:0x11,3000:0x13,3005:0x11';
  await mapConcurrent([1, 2, 3, 4, 5, 6], async stage => {
    for (const shot of [0, 1, 2, 3]) {
      for (const difficulty of [0, 1, 2, 3]) {
        const dir = mkdtempSync(join(tmpdir(), 'th16-spring-draw-'));
        try {
          const state = join(dir, 'state.json');
          const oracleDraw = join(dir, 'oracle.draw.jsonl');
          const portDraw = join(dir, 'port.draw.jsonl');
          await runAsync(py, [oracle, exe, datDir, '600', state,
                            '--stage', String(stage), '--shot', String(shot), '--season', '0', '--difficulty', String(difficulty),
                            '--lives', '8', '--refill-lives', '--input', input, '--enemies',
                            '--draw', '--draw-out', oracleDraw, '--draw-from', '590'],
                       {stdio: ['ignore', 'ignore', 'pipe']});
          await runAsync(harness, ['game-draw', dat, String(stage), String(shot), '0', '600', input, '', '',
                                 String(difficulty), '-1', '8', '1', '0', portDraw, '590'],
                       {stdio: ['ignore', 'ignore', 'pipe']});
          const compared = await runAsync('python3', [compare, oracleDraw, portDraw, '--frames', '590-599', '--eps', '1e-4'],
                                        {encoding: 'utf8'});
          process.stdout.write(compared);
          assert.match(compared, /frames=10 mismatches=0/, `spring stage ${stage} shot ${shot} difficulty ${difficulty}`);
        } finally {
          rmSync(dir, {recursive: true, force: true});
        }
      }
    }
  }, 8);
});

test('stage 1–6 Lunatic four shots summer/autumn/winter: frames 590–599 match original vertices', needDat, async () => {
  const exe = process.env.TH16_EXE ?? resolve(monorepo, 'games/th16/original/th16.exe');
  const datDir = resolve(monorepo, 'artifacts/th16/dat');
  const py = process.env.TH16_PYTHON ?? '/tmp/th16-venv/bin/python';
  const oracle = resolve(here, '../tools/oracle/game-oracle.py');
  const compare = resolve(here, '../tools/oracle/draw-compare.py');
  assert.ok(existsSync(exe) && existsSync(py) && existsSync(datDir), 'th16.exe / oracle python / dat dir');
  const input = '0:0x11,1500:0x13,1505:0x11,3000:0x13,3005:0x11';
  await mapConcurrent([1, 2, 3, 4, 5, 6], async stage => {
    for (const shot of [0, 1, 2, 3]) {
      for (const season of [1, 2, 3]) {
        const dir = mkdtempSync(join(tmpdir(), 'th16-lunatic-draw-'));
        try {
          const state = join(dir, 'state.json');
          const oracleDraw = join(dir, 'oracle.draw.jsonl');
          const portDraw = join(dir, 'port.draw.jsonl');
          await runAsync(py, [oracle, exe, datDir, '600', state,
                            '--stage', String(stage), '--shot', String(shot), '--season', String(season), '--difficulty', '3',
                            '--lives', '8', '--refill-lives', '--input', input, '--enemies',
                            '--draw', '--draw-out', oracleDraw, '--draw-from', '590'],
                       {stdio: ['ignore', 'ignore', 'pipe']});
          await runAsync(harness, ['game-draw', dat, String(stage), String(shot), String(season), '600', input, '', '',
                                 '3', '-1', '8', '1', '0', portDraw, '590'],
                       {stdio: ['ignore', 'ignore', 'pipe']});
          const compared = await runAsync('python3', [compare, oracleDraw, portDraw, '--frames', '590-599', '--eps', '1e-4'],
                                        {encoding: 'utf8'});
          process.stdout.write(compared);
          assert.match(compared, /frames=10 mismatches=0/, `lunatic stage ${stage} shot ${shot} season ${season}`);
        } finally {
          rmSync(dir, {recursive: true, force: true});
        }
      }
    }
  }, 8);
});

test('stage 1–6 Hard four shots summer/autumn/winter: frames 590–599 match original vertices', needDat, async () => {
  const exe = process.env.TH16_EXE ?? resolve(monorepo, 'games/th16/original/th16.exe');
  const datDir = resolve(monorepo, 'artifacts/th16/dat');
  const py = process.env.TH16_PYTHON ?? '/tmp/th16-venv/bin/python';
  const oracle = resolve(here, '../tools/oracle/game-oracle.py');
  const compare = resolve(here, '../tools/oracle/draw-compare.py');
  assert.ok(existsSync(exe) && existsSync(py) && existsSync(datDir), 'th16.exe / oracle python / dat dir');
  const input = '0:0x11,1500:0x13,1505:0x11,3000:0x13,3005:0x11';
  await mapConcurrent([1, 2, 3, 4, 5, 6], async stage => {
    for (const shot of [0, 1, 2, 3]) {
      for (const season of [1, 2, 3]) {
        const dir = mkdtempSync(join(tmpdir(), 'th16-hard-draw-'));
        try {
          const state = join(dir, 'state.json');
          const oracleDraw = join(dir, 'oracle.draw.jsonl');
          const portDraw = join(dir, 'port.draw.jsonl');
          await runAsync(py, [oracle, exe, datDir, '600', state,
                            '--stage', String(stage), '--shot', String(shot), '--season', String(season), '--difficulty', '2',
                            '--lives', '8', '--refill-lives', '--input', input, '--enemies',
                            '--draw', '--draw-out', oracleDraw, '--draw-from', '590'],
                       {stdio: ['ignore', 'ignore', 'pipe']});
          await runAsync(harness, ['game-draw', dat, String(stage), String(shot), String(season), '600', input, '', '',
                                 '2', '-1', '8', '1', '0', portDraw, '590'],
                       {stdio: ['ignore', 'ignore', 'pipe']});
          const compared = await runAsync('python3', [compare, oracleDraw, portDraw, '--frames', '590-599', '--eps', '1e-4'],
                                        {encoding: 'utf8'});
          process.stdout.write(compared);
          assert.match(compared, /frames=10 mismatches=0/, `hard stage ${stage} shot ${shot} season ${season}`);
        } finally {
          rmSync(dir, {recursive: true, force: true});
        }
      }
    }
  }, 8);
});

test('stage 1–6 Normal four shots summer/autumn/winter: frames 590–599 match original vertices', needDat, async () => {
  const exe = process.env.TH16_EXE ?? resolve(monorepo, 'games/th16/original/th16.exe');
  const datDir = resolve(monorepo, 'artifacts/th16/dat');
  const py = process.env.TH16_PYTHON ?? '/tmp/th16-venv/bin/python';
  const oracle = resolve(here, '../tools/oracle/game-oracle.py');
  const compare = resolve(here, '../tools/oracle/draw-compare.py');
  assert.ok(existsSync(exe) && existsSync(py) && existsSync(datDir), 'th16.exe / oracle python / dat dir');
  const input = '0:0x11,1500:0x13,1505:0x11,3000:0x13,3005:0x11';
  await mapConcurrent([1, 2, 3, 4, 5, 6], async stage => {
  for (const shot of [0, 1, 2, 3]) {
    for (const season of [1, 2, 3]) {
      const dir = mkdtempSync(join(tmpdir(), 'th16-normal-draw-'));
      try {
        const state = join(dir, 'state.json');
        const oracleDraw = join(dir, 'oracle.draw.jsonl');
        const portDraw = join(dir, 'port.draw.jsonl');
        await runAsync(py, [oracle, exe, datDir, '600', state,
                          '--stage', String(stage), '--shot', String(shot), '--season', String(season), '--difficulty', '1',
                          '--lives', '8', '--refill-lives', '--input', input, '--enemies',
                          '--draw', '--draw-out', oracleDraw, '--draw-from', '590'],
                     {stdio: ['ignore', 'ignore', 'pipe']});
        await runAsync(harness, ['game-draw', dat, String(stage), String(shot), String(season), '600', input, '', '',
                               '1', '-1', '8', '1', '0', portDraw, '590'],
                     {stdio: ['ignore', 'ignore', 'pipe']});
        const compared = await runAsync('python3', [compare, oracleDraw, portDraw, '--frames', '590-599', '--eps', '1e-4'],
                                      {encoding: 'utf8'});
        process.stdout.write(compared);
        assert.match(compared, /frames=10 mismatches=0/, `normal stage ${stage} shot ${shot} season ${season}`);
      } finally {
        rmSync(dir, {recursive: true, force: true});
      }
    }
  }
  }, 8);
});

test('Extra four shots spring/summer/autumn/winter: frames 590–599 match original vertices', needDat, async () => {
  const exe = process.env.TH16_EXE ?? resolve(monorepo, 'games/th16/original/th16.exe');
  const datDir = resolve(monorepo, 'artifacts/th16/dat');
  const py = process.env.TH16_PYTHON ?? '/tmp/th16-venv/bin/python';
  const oracle = resolve(here, '../tools/oracle/game-oracle.py');
  const compare = resolve(here, '../tools/oracle/draw-compare.py');
  assert.ok(existsSync(exe) && existsSync(py) && existsSync(datDir), 'th16.exe / oracle python / dat dir');
  const input = '0:0x11,1500:0x13,1505:0x11,3000:0x13,3005:0x11';
  await mapConcurrent([0, 1, 2, 3], async shot => {
  for (const season of [0, 1, 2, 3]) {
    const dir = mkdtempSync(join(tmpdir(), 'th16-extra-draw-'));
    try {
      const state = join(dir, 'state.json');
      const oracleDraw = join(dir, 'oracle.draw.jsonl');
      const portDraw = join(dir, 'port.draw.jsonl');
      await runAsync(py, [oracle, exe, datDir, '600', state,
                        '--stage', '7', '--shot', String(shot), '--season', String(season), '--difficulty', '4',
                        '--lives', '8', '--refill-lives', '--input', input, '--enemies',
                        '--draw', '--draw-out', oracleDraw, '--draw-from', '590'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      await runAsync(harness, ['game-draw', dat, '7', String(shot), String(season), '600', input, '', '',
                             '4', '-1', '8', '1', '0', portDraw, '590'],
                   {stdio: ['ignore', 'ignore', 'pipe']});
      const compared = await runAsync('python3', [compare, oracleDraw, portDraw, '--frames', '590-599', '--eps', '1e-4'],
                                    {encoding: 'utf8'});
      process.stdout.write(compared);
      assert.match(compared, /frames=10 mismatches=0/, `extra shot ${shot} season ${season}`);
    } finally {
      rmSync(dir, {recursive: true, force: true});
    }
  }
  }, 8);
});

test('draw structure: stage 5 Normal spell 46 line lasers (0x433720 beam / tip / origin VMs) + boss distortion grid, frames 400–500', needDat, () => {
  assert.equal(checkGolden('draw-st5-line-laser'), 101);
});

test('draw structure: stage 5 Lunatic Reimu winter midstage line lasers (0x433720 via ECL ins_702, bullet.anm slot 7), frames 3373–3473', needDat, () => {
  assert.equal(checkGolden('draw-st5-mid-laser'), 101);
});

test('draw structure: stage 6 Easy Aya winter BossCard1 line lasers (0x433720 via 0x413860 Ex 0x8000000, bullet.anm slot 7), frames 24645–24740', needDat, () => {
  assert.equal(checkGolden('draw-st6-boss-laser'), 96);
});

test('draw structure: stage 3 Normal Reimu spring BossCard2 curve lasers (0x438750 strip, bullet.anm slot 7), frames 22899–22999', needDat, () => {
  assert.equal(checkGolden('draw-st3-mid-curve'), 101);
});

test('draw structure: Extra spell 115 infinite lasers (0x4357a0 beam / origin VMs), Extra difficulty HUD (0x42cb60), frames 210–330', needDat, () => {
  assert.equal(checkGolden('draw-st7-infinite-laser'), 121);
});

test('draw structure: stage 5 Normal spell 50 curve lasers (0x438750 strips, bullet.anm slot 7, mode 2), frames 395–495', needDat, () => {
  assert.equal(checkGolden('draw-st5-curve-laser'), 101);
});

test('draw structure: Extra spell 117 infinite lasers (0x4357a0, bullet.anm slot 7, mode 2), frames 808–900', needDat, () => {
  assert.equal(checkGolden('draw-st7-card9-laser'), 93);
});

function checkBootGolden(name) {
  const golden = JSON.parse(readFileSync(resolve(here, 'golden', `${name}.json`), 'utf8'));
  const wanted = Object.keys(golden.frames).map(Number).sort((a, b) => a - b);
  const dir = mkdtempSync(join(tmpdir(), 'th16-boot-draw-'));
  const out = join(dir, 'port.jsonl');
  try {
    execFileSync(harness, ['scene-boot', dat, String(wanted.at(-1)), `draw=${out}`, `draw-from=${wanted[0]}`],
      {encoding: 'utf8', stdio: ['ignore', 'ignore', 'pipe']});
    const frames = {};
    for (const line of readFileSync(out, 'utf8').split('\n')) {
      if (!line) continue;
      const rec = JSON.parse(line);
      if (golden.frames[String(rec.frame)] !== undefined) frames[rec.frame] = rec.ops;
    }
    const bad = [];
    for (const frame of wanted) {
      assert.ok(frames[frame], `port recorded boot frame ${frame}`);
      if (structureDigest(frames[frame], null) !== golden.frames[String(frame)]) bad.push(frame);
    }
    assert.deepEqual(bad, []);
    return wanted.length;
  } finally {
    rmSync(dir, {recursive: true, force: true});
  }
}

test('draw structure: boot logo frames 170–180 match the original (0x43b300 ascii.anm 0x11 @ (960,784))', needDat, () => {
  assert.equal(checkBootGolden('draw-boot-170'), 11);
});
