import { closeSync, fstatSync, openSync } from 'node:fs';
import { parse, resolve } from 'node:path';
import { setTimeout as delay } from 'node:timers/promises';
import { fileURLToPath } from 'node:url';
import native from 'fs-native-extensions';

function assertLockPath(path) {
  if (path === parse(path).root || !path.endsWith('.lock')) {
    throw new Error('Build lock must name a .lock file');
  }
}

/** The Node-only OS lock primitive shared by direct Node and Bun host processes. */
export async function acquireNativeBuildLock(path, { timeout = 120_000, interval = 25, cancelled } = {}) {
  const lock = resolve(path);
  assertLockPath(lock);
  let fd;
  try {
    fd = openSync(lock, 'a+');
  } catch (error) {
    if (error?.code === 'EISDIR' || error?.code === 'EPERM') {
      throw new Error(
        `Cannot open build lock: ${lock}. If this is a legacy lock directory, stop its build processes before moving it aside.`,
        { cause: error },
      );
    }
    throw error;
  }
  if (!fstatSync(fd).isFile()) {
    closeSync(fd);
    throw new Error(`Build lock is a legacy lock directory: ${lock}. Stop its build processes before moving it aside.`);
  }

  const started = Date.now();
  try {
    while (true) {
      if (cancelled?.()) {
        const error = new Error(`Parent closed while waiting for build lock: ${lock}`);
        error.code = 'ECANCELED';
        throw error;
      }
      if (native.tryLock(fd)) break;
      if (Date.now() - started >= timeout) throw new Error(`Timed out waiting for build lock: ${lock}`);
      await delay(interval);
    }
  } catch (error) {
    closeSync(fd);
    throw error;
  }

  let released = false;
  return () => {
    if (released) return;
    released = true;
    try {
      native.unlock(fd);
    } finally {
      closeSync(fd);
    }
  };
}

function writeError(error) {
  process.stdout.write(
    `ERROR\t${JSON.stringify({ code: error?.code ?? null, message: error?.message ?? String(error) })}\n`,
  );
}

async function runNodeLockHost() {
  const [path, timeoutText, intervalText] = process.argv.slice(2);
  const timeout = Number(timeoutText);
  const interval = Number(intervalText);
  let parentClosed = false;
  let releaseLock;
  let helperFailure;
  let readySent = false;
  const parentEnded = new Promise((resolvePromise) => {
    process.stdin.once('end', () => {
      parentClosed = true;
      resolvePromise();
    });
  });

  process.stdin.resume();

  try {
    if (!path || !Number.isFinite(timeout) || !Number.isFinite(interval) || interval < 0) {
      throw new Error('Node build-lock helper received invalid arguments');
    }
    process.stdout.write('STARTED\n');
    releaseLock = await acquireNativeBuildLock(path, { timeout, interval, cancelled: () => parentClosed });
    if (parentClosed) {
      releaseLock();
      releaseLock = undefined;
      return;
    }
    readySent = true;
    process.stdout.write('READY\n');
    await parentEnded;
    if (releaseLock) {
      releaseLock();
      releaseLock = undefined;
    }
    if (helperFailure) throw helperFailure;
  } catch (error) {
    let failure = error;
    if (releaseLock) {
      try {
        releaseLock();
      } catch (releaseError) {
        failure = releaseError;
      }
      releaseLock = undefined;
    }
    if (parentClosed && failure?.code === 'ECANCELED') return;
    if (readySent) process.stderr.write(`Build lock host failed: ${failure?.message ?? failure}\n`);
    else writeError(failure);
    process.exitCode = 1;
  }
}

if (process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url)) {
  await runNodeLockHost();
}
