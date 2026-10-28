// UI 基座单测包装：编译并运行 tests/ui/run.sh（g++ 原生套件），
// 汇入 node --test 统一入口；失败时透传完整输出（经 fd 落盘，无 1MiB 管道上限）。
import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve, dirname} from 'node:path';
import {fileURLToPath} from 'node:url';
import {runChild, describeFailure} from '../tools/run-child.mjs';

const port = resolve(dirname(fileURLToPath(import.meta.url)), '..');

// 子进程预算（秒）：仅约束 run.sh 进程本身（冷编译全量 g++ 实测约 385s，留余量取
// 900s），可用 TH13_UI_TEST_BUDGET_S 调整。这只是防挂死的进程预算，不参与断言语义，
// 与本次根因修复（ENOBUFS）无关。node:test 的 {timeout} 对同步阻塞体无效，故不用。
const budgetMs = Number(process.env.TH13_UI_TEST_BUDGET_S ?? 900) * 1000;

test('UI 基座单测（输入/调度/ANM 句柄/HUD/标题/暂停/对局，g++ 套件）', async () => {
  const run = await runChild({
    command: 'bash',
    args: [resolve(port, 'tests/ui/run.sh')],
    timeout: budgetMs,
    dir: '/tmp/th13-ui-test-output',
  });
  const evidence = [
    `status=${JSON.stringify(run.status)} signal=${JSON.stringify(run.signal)}`,
    `error=${JSON.stringify(run.error?.code ?? run.error?.message ?? null)}`,
    `stdout=${run.stdoutBytes}B stderr=${run.stderrBytes}B`,
    `full stdout: ${run.stdoutPath}  full stderr: ${run.stderrPath}`,
    `-- stdout 尾部 --\n${run.stdoutTail}`,
    `-- stderr 尾部 --\n${run.stderrTail}`,
  ].join('\n');
  if (run.status !== 0) {
    console.log(evidence);
  }
  // 逐条透传：启动失败/超时/信号/非零退出各自成断言，消息携带尾部证据。
  assert.equal(run.spawnFailed, false, describeFailure('tests/ui/run.sh', run));
  assert.equal(run.timedOut, false, describeFailure('tests/ui/run.sh', run));
  assert.equal(run.signal, null, describeFailure('tests/ui/run.sh', run));
  assert.equal(run.status, 0, describeFailure('tests/ui/run.sh', run));
  // ok 计数基于全量 stdout 落盘文件重扫（编译告警可能淹没尾部 32KiB 里的 ok 行）。
  const full = readFileSync(run.stdoutPath, 'utf8');
  const fullCount = full.split('\n').filter(l => l.startsWith('ok ')).length;
  assert.ok(fullCount >= 20, `应至少通过 20 条用例，实际 ${fullCount}`);
});
