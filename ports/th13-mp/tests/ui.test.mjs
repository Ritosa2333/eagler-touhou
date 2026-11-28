import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve, dirname} from 'node:path';
import {fileURLToPath} from 'node:url';
import {runChild, describeFailure} from '../tools/run-child.mjs';

const port = resolve(dirname(fileURLToPath(import.meta.url)), '..');

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
  assert.equal(run.spawnFailed, false, describeFailure('tests/ui/run.sh', run));
  assert.equal(run.timedOut, false, describeFailure('tests/ui/run.sh', run));
  assert.equal(run.signal, null, describeFailure('tests/ui/run.sh', run));
  assert.equal(run.status, 0, describeFailure('tests/ui/run.sh', run));
  const full = readFileSync(run.stdoutPath, 'utf8');
  const fullCount = full.split('\n').filter(l => l.startsWith('ok ')).length;
  assert.ok(fullCount >= 20, `应至少通过 20 条用例，实际 ${fullCount}`);
});
