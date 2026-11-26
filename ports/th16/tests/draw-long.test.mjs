// 整段绘制对拍：原版 game-oracle.py --draw --draw-digest 把整段跑的每一帧绘制流折成摘要，与移植侧逐帧比较；
// 通过的帧按量化网格折成段，段表存 tests/golden/draw-long.json（段 = [起帧, 止帧, 网格 a / b, 段摘要]）。
// 本测试只跑移植侧 harness game-draw（digest: 输出，harness 内直接折摘要），逐段核对：结构、顶点（1e-4 相对格）与
// 绘制次序在整段的每一帧都与原版一致。窗口级的精确定位仍由 draw.test.mjs 负责。
// 生成段表：tools/oracle/draw-compare.py <oracle.digest.jsonl> <port.digest.jsonl> --compare-digests --golden-segments
import {test, describe} from 'node:test';
import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {existsSync, readFileSync} from 'node:fs';
import {createInterface} from 'node:readline';
import {resolve} from 'node:path';
import {digestStream, segmentDigest} from '../tools/draw-structure.mjs';

const here = resolve(import.meta.dirname);
const monorepo = resolve(here, '../../..');
const harness = process.env.TH16_HARNESS ?? resolve(here, 'build/harness');
const dat = process.env.TH16_DAT ?? resolve(monorepo, 'games/th16/original/th16.dat');
const needDat = {skip: !existsSync(dat) && 'original th16.dat not present'};
const golden = JSON.parse(readFileSync(resolve(here, 'golden/draw-long.json'), 'utf8'));

// 跑移植侧整段；绘制流写进继承来的描述符 3：outSpec 为 "digest:fd:3"（每帧一行摘要）或 "fd:3"（每帧一行原始操作 JSON）。
// from：只输出该帧起的绘制流（默认 1）。退出监听在启动时就挂上：回放很快，等读完管道再挂会错过 close 事件
function runPort(c, outSpec) {
  const input = c.input.startsWith('@') ? '@' + resolve(monorepo, c.input.slice(1)) : c.input;
  const child = spawn(harness, ['game-draw', dat, String(c.stage), String(c.shot), String(c.season), String(c.frames), input, '', '',
    String(c.difficulty), String(c.replay_slot ?? -1), String(c.lives ?? 8), String(c.refill ?? 1), String(c.mode ?? 0), outSpec, String(c.from ?? 1)],
    {stdio: ['ignore', 'ignore', 'inherit', 'pipe']});
  return {stream: child.stdio[3], exited: new Promise(done => child.on('close', done))};
}

async function collect(stream, fold) {
  const rows = new Map();
  for await (const row of fold(stream)) rows.set(row.frame, row);
  return rows;
}

const jsonLines = stream => (async function* () {
  for await (const line of createInterface({input: stream, crlfDelay: Infinity})) if (line) yield JSON.parse(line);
})();

async function digests(c) {
  const {stream, exited} = runPort(c, 'digest:fd:3');
  const rows = await collect(stream, jsonLines);
  assert.equal(await exited, 0, 'harness game-draw exit code');
  return rows;
}

// 三处实现（harness 内 C++、tools/draw-structure.mjs、tools/oracle/draw-compare.py）的口径互校：
// 同一段跑，harness 直接折出的摘要必须与"原始 JSON → draw-structure.mjs"逐帧相同。
// 第二段取第 1 面过关进第 2 面后的 80 帧：这里有 alphaArg2 ≠ textureTransform 的屏幕条带，结构行跳过 textureTransform 时
// 容易错位到相邻字段
const selfCheckScenarios = [
  {name: 'stage 1 opening with firing', stage: 1, shot: 0, season: 0, difficulty: 0, frames: 2400, from: 1,
    input: '0:0x11,300:0x13,305:0x11,1500:0x13,1505:0x11'},
  {name: 'stage 2 opening after the stage 1 clear', stage: 1, shot: 0, season: 0, difficulty: 0, frames: 22520, from: 22440, input: '0:0'},
];
for (const c of selfCheckScenarios) {
  test(`harness digest lines agree with draw-structure.mjs on the raw stream: ${c.name}`, needDat, async () => {
    const native = runPort(c, 'digest:fd:3');
    const raw = runPort(c, 'fd:3');
    const [nativeRows, rawRows] = await Promise.all([collect(native.stream, jsonLines), collect(raw.stream, digestStream)]);
    assert.deepEqual(await Promise.all([native.exited, raw.exited]), [0, 0]);
    assert.ok(rawRows.size > 50, `raw rows ${rawRows.size}`);
    assert.deepEqual([...nativeRows], [...rawRows]);
  });
}

const drawJobs = Number(process.env.TH16_DRAW_JOBS ?? 2);
assert.ok(Number.isSafeInteger(drawJobs) && drawJobs > 0, 'TH16_DRAW_JOBS must be a positive integer');
// 只断言已按当前原版重采并核实的参考；未标记的参考保留上一轮摘要，待重采后纳入回归。
const verifiedCases = golden.cases.filter(c => c.verified === true);
assert.equal(verifiedCases.length, 8,
  `verified long-draw references: ${verifiedCases.map(c => c.name).join(', ')}`);
describe('long draw regressions', {concurrency: drawJobs}, () => {
  for (const c of golden.cases) {
    const options = c.verified === true
      ? needDat
      : {skip: '参考未按当前原版重采，重采后再纳入断言'};
    test(`long draw stream matches original: ${c.name}`, options, async () => {
      // 随测试启动子进程，避免模块装载时同时运行全部长程回放；skip / 名称筛选也生效。
      const rows = await digests(c);
      const bad = [];
      for (const [from, to, grid, want] of c.segments) {
        const frames = [];
        for (let frame = from; frame <= to; ++frame) frames.push(frame);
        if (frames.some(frame => !rows.has(frame))) {
          bad.push(`frames ${from}-${to}: port stopped before the segment ends (last frame ${Math.max(...rows.keys())})`);
          continue;
        }
        if (segmentDigest(rows, frames, grid) !== want) bad.push(`frames ${from}-${to} (grid ${grid})`);
      }
      assert.deepEqual(bad.slice(0, 8), [],
        `${c.name}: ${bad.length} of ${c.segments.length} segments differ from the original: ${bad.slice(0, 8).join('; ')}`);
    });
  }
});
