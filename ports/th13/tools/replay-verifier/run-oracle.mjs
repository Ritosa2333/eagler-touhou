// th13 replay-verifier：候选侧（WASI harness draws 导出）驱动。
// 形态承自 ports/th15/tools/replay-verifier（corpus + golden + 对跑两驱动），
// 裁决器为外层仓库 oracle/draw_compare.py——本脚本只负责「按 corpus 案例导出
// 移植侧清单 + 校验身份」，比对/绿灯判定交给 run-gate.mjs。
//
// 用法（容器内 /work/eagler-touhou/ports/th13）：
//   node tools/replay-verifier/run-oracle.mjs [--case <id>] [--output <dir>]
// 输出：<output>/<id>.candidate.jsonl（+ stdout 摘要行）。
// 依赖：web/artifacts/wasi/th13-harness.wasm（portable/build-harness.mjs 产物）。
import {spawnSync} from 'node:child_process';
import {readFileSync, mkdirSync, openSync, readSync, closeSync,
        createReadStream} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';

// 大文件流式 sha256（候选清单 GB 级，readFileSync 会爆内存/字符串上限）。
const sha256File = path => new Promise((done, fail) => {
  const hash = createHash('sha256');
  createReadStream(path).on('data', c => hash.update(c)).on('end',
      () => done(hash.digest('hex'))).on('error', fail);
});

const here = fileURLToPath(new URL('./', import.meta.url));
const project = resolve(here, '../..');
const corpus = JSON.parse(readFileSync(resolve(here, 'corpus.json')));
const option = name => {
  const i = process.argv.indexOf(name);
  return i < 0 ? null : process.argv[i + 1];
};
const wanted = option('--case');
const lane = option('--lane') || 'all';
const ids = wanted ? [wanted]
    : lane === 'all' ? Object.values(corpus.suites).flat()
                     : corpus.suites[lane];
if (!ids) throw Error('Unknown suite: ' + lane);
const output = resolve(option('--output') || resolve(project, 'artifacts/replay-verifier'));
mkdirSync(output, {recursive: true});

// 容器契约（AGENTS.md 红线 2）：一切构建/测试经容器。非容器环境下直接调用
// node + harness 需在容器内执行（本脚本被容器内 node 运行时天然满足）。
const runHarness = args => {
  const run = spawnSync(process.execPath, [
    '--experimental-wasi-unstable-preview1',
    resolve(project, 'tools/run-harness.mjs'), ...args,
  ], {cwd: project, encoding: 'utf8'});
  if (run.status !== 0) throw Error('run-harness failed:\n' + run.stdout + run.stderr);
  return run.stdout.trim().split('\n').pop();
};

for (const id of ids) {
  const fixture = corpus.cases.find(c => c.id === id);
  if (!fixture) throw Error('Unknown case ' + id);
  const target = resolve(output, id + '.candidate.jsonl');
  // run-harness 的 WASI preopen '/' = 外层工作区根。
  // 输出路径换算：宿主绝对路径 → 挂载内路径（去外层根前缀）。
  const outerRoot = resolve(project, '../../..');
  const mounted = target.startsWith(outerRoot)
      ? target.slice(outerRoot.length) : null;
  if (!mounted) throw Error('Candidate output must live under the workspace: ' + target);
  let summary;
  if (fixture.kind === 'noinput') {
    summary = runHarness([fixture.harnessMode, '/artifacts/reference/raw', mounted,
      String(fixture.frames), 'st01.ecl']);
  } else {
    summary = runHarness([fixture.harnessMode, '/games/replays/th13_udSr01.rpy',
      '/artifacts/reference/raw', mounted]);
  }
  // 身份自证：候选清单的首行 frame=0 + 帧数摘要回显（防拿错文件当基线——
  // 8 轮假 -1 前科的对策：比对前先自证身份）。回放窗候选 >1GB：首行只读
  // 头部块、sha256 走流式——整读 toString 撞 Node 512MB 字符串上限
  //（ERR_STRING_TOO_LONG，2026-10-23 f61 修复轮实测）。
  const fd = openSync(target, 'r');
  const head = Buffer.alloc(1 << 16);
  const got = readSync(fd, head, 0, head.length, 0);
  closeSync(fd);
  const first = head.toString('utf8', 0, got).split('\n', 1)[0];
  const frame0 = JSON.parse(first).frame;
  if (frame0 !== 0) throw Error('Candidate manifest does not start at frame 0: ' + id);
  const sha = await sha256File(target);
  console.log(JSON.stringify({id, target, framesFirst: frame0, sha256: sha,
                              summary}));
}
