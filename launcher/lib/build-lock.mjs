import { spawn } from 'node:child_process';
import { existsSync } from 'node:fs';
import { parse, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { nodeHostExecutable } from './node-runtime.mjs';

const helperName = 'build-lock-host.mjs';
const startupTimeout = 15_000;

function helperPath() {
  let moduleCandidate;
  try {
    moduleCandidate = fileURLToPath(new URL(`./${helperName}`, import.meta.url));
  } catch {}
  if (moduleCandidate && existsSync(moduleCandidate)) return moduleCandidate;
  const bundledCandidates = [
    resolve(process.cwd(), 'lib', helperName),
    resolve(process.cwd(), 'launcher', 'lib', helperName),
  ];
  const found = bundledCandidates.find((path) => existsSync(path));
  if (found) return found;
  throw new Error(`Bun build-lock helper ${helperName} is missing from the current project bundle.`);
}

function acquireWithNodeHost(lock, { timeout, interval }) {
  const executable = nodeHostExecutable();
  const child = spawn(executable, [helperPath(), lock, String(timeout), String(interval)], {
    cwd: process.cwd(),
    env: process.env,
    stdio: ['pipe', 'pipe', 'inherit'],
    windowsHide: true,
  });

  return new Promise((resolvePromise, reject) => {
    let output = '';
    let acquired = false;
    let released = false;
    let releasePromise;
    let helperFailure;
    let startupTimer;
    let settled = false;
    let finishExit;
    const exited = new Promise((resolveExit) => {
      finishExit = resolveExit;
    });

    function fail(error, stopChild = true) {
      if (settled) return;
      settled = true;
      clearTimeout(startupTimer);
      if (stopChild) child.kill();
      reject(error);
    }

    function reportHelperFailure(code, signal) {
      helperFailure = new Error(`Node build-lock helper exited before release (${signal ?? code ?? 'unknown'})`);
      console.error(helperFailure.message);
      process.exit(1);
    }

    function receiveLine(line) {
      if (!line) return;
      if (line === 'STARTED') {
        clearTimeout(startupTimer);
        return;
      }
      if (line === 'READY') {
        if (settled) {
          child.stdin.end();
          return;
        }
        acquired = true;
        settled = true;
        clearTimeout(startupTimer);
        child.unref();
        child.stdin.unref?.();
        // The host only writes protocol data during acquisition. Close stdout
        // after READY so Bun does not keep its event loop alive while the lock is idle.
        child.stdout.destroy();
        resolvePromise(async () => {
          if (releasePromise) return releasePromise;
          if (helperFailure) return Promise.reject(helperFailure);
          released = true;
          child.ref();
          child.stdin.ref?.();
          releasePromise = (async () => {
            child.stdin.end();
            const result = await exited;
            if (helperFailure) throw helperFailure;
            if (result.code !== 0) {
              throw new Error(
                `Node build-lock helper failed while releasing (${result.signal ?? result.code ?? 'unknown'})`,
              );
            }
          })();
          return releasePromise;
        });
        return;
      }
      if (line.startsWith('ERROR\t')) {
        let detail;
        try {
          detail = JSON.parse(line.slice('ERROR\t'.length));
        } catch {}
        const error = new Error(detail?.message ?? `Node build-lock helper failed: ${line}`);
        if (acquired) {
          helperFailure = error;
          process.exitCode = 1;
        } else fail(error);
        return;
      }
      const error = new Error(`Node build-lock helper sent an invalid response: ${line}`);
      if (acquired) {
        helperFailure = error;
        process.exitCode = 1;
      } else fail(error);
    }

    child.stdout.setEncoding('utf8');
    child.stdout.on('data', (chunk) => {
      output += chunk;
      const lines = output.split(/\r?\n/);
      output = lines.pop() ?? '';
      for (const line of lines) receiveLine(line);
    });
    child.stdin.on('error', (error) => {
      if (released) return;
      helperFailure = new Error(`Node build-lock helper input closed unexpectedly: ${error.message}`);
      process.exitCode = 1;
    });
    child.once('error', (error) => fail(new Error(`Cannot start Node build-lock helper: ${error.message}`), false));
    child.once('exit', (code, signal) => {
      clearTimeout(startupTimer);
      finishExit({ code, signal });
      if (acquired && !released) reportHelperFailure(code, signal);
      else if (!acquired && !settled) {
        const line = output.split(/\r?\n/).find((value) => value.startsWith('ERROR\t'));
        let detail;
        try {
          detail = line ? JSON.parse(line.slice('ERROR\t'.length)) : null;
        } catch {}
        fail(
          new Error(
            detail?.message ?? `Node build-lock helper exited before acquiring (${signal ?? code ?? 'unknown'})`,
          ),
          false,
        );
      }
    });

    startupTimer = setTimeout(() => {
      fail(new Error(`Node build-lock helper did not start within ${startupTimeout}ms`));
    }, startupTimeout);
  });
}

/**
 * Serialize Launcher builds with a permanent OS-locked .lock file. Bun delegates
 * native lock ownership to Node because Bun's POSIX N-API layer cannot translate
 * the expected errno returned for a contended lock.
 */
export async function acquireBuildLock(path, { timeout = 120_000, interval = 25 } = {}) {
  const lock = resolve(path);
  if (lock === parse(lock).root || !lock.endsWith('.lock')) {
    throw new Error('Build lock must name a .lock file');
  }
  if (process.versions.bun) return acquireWithNodeHost(lock, { timeout, interval });
  const { acquireNativeBuildLock } = await import('./build-lock-host.mjs');
  const releaseNative = await acquireNativeBuildLock(lock, { timeout, interval });
  return async () => releaseNative();
}
