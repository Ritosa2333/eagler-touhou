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
  if (start > 0) text = text.slice(text.indexOf('\n') + 1);
  return text;
}

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
        detached: true,
      });
      const killGroup = sig => {
        try { process.kill(-child.pid, sig); } catch {}
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
    try { process.kill(-pid, 'SIGKILL'); } catch {}
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

export function describeFailure(label, run) {
  if (run.spawnFailed) return `${label} 无法启动：${run.error?.code ?? run.error?.message}`;
  if (run.timedOut) return `${label} 超过子进程预算被组杀（退出码 null）`;
  if (run.signal) return `${label} 被信号 ${run.signal} 终止（退出码 null）`;
  if (run.status !== 0) return `${label} 应以 0 退出，实际 ${run.status}`;
  return `${label} 正常退出`;
}
