// run-child 单测：经生产 helper（与 ui.test.mjs 同路径）验证
// 超大输出不再触发 ENOBUFS/SIGTERM、失败/信号/超时/启动失败如实透传，
// 以及超时组杀不留孤儿孙进程（正负对照）。
import test from 'node:test';
import assert from 'node:assert/strict';
import {statSync, rmSync, readFileSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {runChild, describeFailure, TAIL_BYTES} from './run-child.mjs';

const KIB = 1024;
const emit = (outKib, errKib) => [
  'node', ['-e',
    `process.stdout.write('o'.repeat(${outKib}*${KIB}));process.stderr.write('e'.repeat(${errKib}*${KIB}));`],
];

test('超过旧默认 1MiB maxBuffer 的大输出不再被 SIGTERM/ENOBUFS 截杀', async () => {
  const [cmd, args] = emit(1400, 50);
  const run = await runChild({command: cmd, args});
  assert.equal(run.error, null);            // 旧行为：Error ENOBUFS
  assert.equal(run.signal, null);           // 旧行为：SIGTERM
  assert.equal(run.status, 0);
  assert.equal(run.stdoutBytes, 1400 * KIB);
  assert.equal(run.stderrBytes, 50 * KIB);
  assert.ok(run.stdoutTail.startsWith('oooo')); // fd 直连，stdout/stderr 不串
  assert.ok(run.stderrTail.startsWith('eeee'));
  assert.ok(run.stdoutTail.length <= TAIL_BYTES + 2); // 主进程只留尾部，内存有界
});

test('stdout 与 stderr 同时超 1MiB 也完整落盘（合计 >2MiB）', async () => {
  const [cmd, args] = emit(1100, 1100);
  const run = await runChild({command: cmd, args});
  assert.equal(run.status, 0);
  assert.equal(run.error, null);
  assert.equal(run.stdoutBytes, 1100 * KIB);
  assert.equal(run.stderrBytes, 1100 * KIB);
});

test('非零退出：status 如实透传，尾部输出可诊断', async () => {
  const run = await runChild({command: 'node', args: ['-e',
    "process.stderr.write('boom\\n');process.exit(3);"]});
  assert.equal(run.status, 3);
  assert.equal(run.signal, null);
  assert.equal(run.error, null);
  assert.ok(run.stderrTail.includes('boom'));
  assert.equal(describeFailure('x', run), 'x 应以 0 退出，实际 3');
});

test('信号终止：signal 如实透传，status 为 null', async () => {
  const run = await runChild({command: 'bash', args: ['-c', 'kill -TERM $$']});
  assert.equal(run.signal, 'SIGTERM');
  assert.equal(run.status, null);
  assert.equal(run.spawnFailed, false);
  assert.match(describeFailure('x', run), /SIGTERM/);
});

test('超时：ETIMEDOUT 语义清晰', async () => {
  const run = await runChild({command: 'sleep', args: ['5'], timeout: 400});
  assert.equal(run.timedOut, true);
  assert.equal(run.status, null);
  assert.match(describeFailure('x', run), /预算/);
});

test('启动失败（ENOENT）：spawnFailed 如实透传', async () => {
  const run = await runChild({command: 'definitely-not-a-real-cmd-xyz'});
  assert.equal(run.spawnFailed, true);
  assert.equal(run.status, null);
  assert.equal(run.error?.code, 'ENOENT');
  assert.match(describeFailure('x', run), /ENOENT/);
});

test('超时组杀不留孤儿孙进程（正对照：孙进程随组被杀）', async () => {
  const pidFile = join(tmpdir(), `run-child-grandkid-${process.pid}-${Date.now()}`);
  const run = await runChild({command: 'bash', timeout: 400, args: ['-c',
    `sleep 30 & echo $! > ${pidFile}; wait`]});
  assert.equal(run.timedOut, true);
  const grandkid = Number(readFileSync(pidFile, 'utf8'));
  // 本容器 PID 1 不及时收尸：僵尸态（Z）即已死亡，不能只看 kill(pid,0) 是否报成功
  let gone = false;
  for (let i = 0; i < 40 && !gone; i++) {
    try {
      const stat = readFileSync(`/proc/${grandkid}/stat`, 'utf8');
      const state = stat.slice(stat.lastIndexOf(')') + 2).charAt(0);
      if (state === 'Z') gone = true;
    } catch {
      gone = true; // ESRCH：条目已消失
    }
    if (!gone) await new Promise(r => setTimeout(r, 50));
  }
  assert.equal(gone, true, `孙进程 ${grandkid} 应已随进程组被杀（或为已回收僵尸）`);
  rmSync(pidFile, {force: true});
});

test('无超时：同构脚本的孙进程正常完成，不受组杀波及（负对照）', async () => {
  const pidFile = join(tmpdir(), `run-child-grandkid-ok-${process.pid}-${Date.now()}`);
  const run = await runChild({command: 'bash', timeout: 30_000, args: ['-c',
    `sleep 1 & echo $! > ${pidFile}; wait; echo done`]});
  assert.equal(run.status, 0);
  assert.equal(run.timedOut, false);
  assert.ok(run.stdoutTail.includes('done')); // 孙进程跑完、脚本正常收尾
  rmSync(pidFile, {force: true});
});

test('dir 选项：输出持久化到指定目录供事后取证', async () => {
  const dir = join(tmpdir(), `run-child-test-${process.pid}-${Date.now()}`);
  try {
    const [cmd, args] = emit(10, 5);
    const run = await runChild({command: cmd, args, dir});
    assert.equal(run.status, 0);
    assert.equal(run.stdoutPath, join(dir, 'stdout.log'));
    assert.equal(run.stderrPath, join(dir, 'stderr.log'));
    assert.equal(statSync(run.stdoutPath).size, 10 * KIB);
    assert.equal(statSync(run.stderrPath).size, 5 * KIB);
  } finally {
    rmSync(dir, {recursive: true, force: true});
  }
});
