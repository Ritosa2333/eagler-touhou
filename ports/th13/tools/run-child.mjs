// 有界子进程捕获 + 明确失败语义（仅 ui.test.mjs 使用，勿扩成通用框架）。
// 修复背景：默认 spawnSync 管道 maxBuffer=1MiB，编译告警 >1MiB 时子进程被
// SIGTERM（ENOBUFS），status=null 且错误不透传。方案：stdout/stderr 直连 fd
// 落盘（无上限），主进程只保留 32KiB 尾部。超时=子进程预算：detached 进程组
// 组杀 SIGTERM→3s→SIGKILL，close 后终扫，不留 g++ 等孤儿孙进程。
import {spawn} from 'node:child_process';
import {openSync, closeSync, readSync, statSync, rmSync, mkdirSync, mkdtempSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join} from 'node:path';

export const TAIL_BYTES = 32 * 1024;
const KILL_GRACE_MS = 3000;

function readTail(path) {
  const size = statSync(path).size;
  const start = Math.max(0, size - TAIL_BYTES);
  const buf = Buffer.alloc(size - start);
  const fd = openSync(path, 'r');
  try {
    readSync(fd, buf, 0, buf.length, start);
  } finally {
    closeSync(fd);
  }
  let text = buf.toString('utf8');
  if (start > 0) text = text.slice(text.indexOf('\n') + 1); // 丢弃被截断的首行残段
  return text;
}

// opts: {command, args?, cwd?, timeout?, dir?}
// dir 给出时日志持久化到 dir/stdout.log、dir/stderr.log（事后取证）；否则临时目录用后即删。
// 返回 {status, signal, error, timedOut, spawnFailed, stdoutPath, stderrPath,
//       stdoutBytes, stderrBytes, stdoutTail, stderrTail}。
export async function runChild(opts) {
  const dir = opts.dir || mkdtempSync(join(tmpdir(), 'run-child-'));
  mkdirSync(dir, {recursive: true});
  const stdoutPath = join(dir, 'stdout.log');
  const stderrPath = join(dir, 'stderr.log');
  const out = openSync(stdoutPath, 'w');
  const err = openSync(stderrPath, 'w');
  let ended = null, spawnError = null, timedOut = false, pid = null;
  try {
    ({ended, pid} = await new Promise(resolve => {
      const child = spawn(opts.command, opts.args ?? [], {
        cwd: opts.cwd,
        stdio: ['ignore', out, err],
        detached: true, // 子进程自成进程组：组杀只波及本子进程树
      });
      const killGroup = sig => {
        try { process.kill(-child.pid, sig); } catch {} // ESRCH=组已空
      };
      let timer, grace, settled = false;
      const finish = code => {
        if (settled) return;
        settled = true;
        clearTimeout(timer);
        clearTimeout(grace);
        resolve({ended: code, pid: child.pid});
      };
      child.on('error', e => { spawnError = e; finish({code: null, signal: null}); });
      child.on('close', (code, signal) => finish({code, signal}));
      if (opts.timeout != null) {
        timer = setTimeout(() => {
          timedOut = true;
          killGroup('SIGTERM');
          grace = setTimeout(() => killGroup('SIGKILL'), KILL_GRACE_MS);
        }, opts.timeout);
      }
    }));
  } finally {
    closeSync(out);
    closeSync(err);
  }
  if (timedOut && pid != null) {
    try { process.kill(-pid, 'SIGKILL'); } catch {} // 终扫：组内孙进程可能比 bash 晚死
  }
  const status = ended?.code ?? null;
  const signal = ended?.signal ?? null;
  return {
    status,
    signal,
    error: spawnError,
    timedOut,
    spawnFailed: status == null && signal == null && spawnError != null && !timedOut,
    stdoutPath,
    stderrPath,
    stdoutBytes: statSync(stdoutPath).size,
    stderrBytes: statSync(stderrPath).size,
    stdoutTail: readTail(stdoutPath),
    stderrTail: readTail(stderrPath),
  };
}

// 把 launch 错误/超时/信号/非零退出翻译成一句可读断言消息。
export function describeFailure(label, run) {
  if (run.spawnFailed) return `${label} 无法启动：${run.error?.code ?? run.error?.message}`;
  if (run.timedOut) return `${label} 超过子进程预算被组杀（退出码 null）`;
  if (run.signal) return `${label} 被信号 ${run.signal} 终止（退出码 null）`;
  if (run.status !== 0) return `${label} 应以 0 退出，实际 ${run.status}`;
  return `${label} 正常退出`;
}
