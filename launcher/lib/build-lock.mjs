import {closeSync, fstatSync, openSync} from 'node:fs';
import {parse, resolve} from 'node:path';
import {setTimeout as delay} from 'node:timers/promises';
import native from 'fs-native-extensions';

const owned = new Set();
function release(owner) {
  if (!owned.delete(owner)) return;
  try {native.unlock(owner.fd);} finally {closeSync(owner.fd);}
  if (!owned.size) process.removeListener('exit', cleanup);
}
function cleanup() {for (const owner of [...owned]) {try {release(owner);} catch {}}}

/** A permanent guard file uses an OS lock, released even when its owner is killed.
 * Never unlink the guard: waiters must continue locking the same file. */
export async function acquireBuildLock(path, {timeout = 120000, interval = 25} = {}) {
  const lock = resolve(path);
  if (lock === parse(lock).root || !lock.endsWith('.lock')) throw new Error('Build lock must name a .lock file');
  let fd;
  try {fd = openSync(lock, 'a+');}
  catch (error) {
    if (error?.code === 'EISDIR' || error?.code === 'EPERM')
      throw new Error(`Cannot open build lock: ${lock}. If this is a legacy lock directory, stop its build processes before moving it aside.`, {cause:error});
    throw error;
  }
  if (!fstatSync(fd).isFile()) {
    closeSync(fd);
    throw new Error(`Build lock is a legacy lock directory: ${lock}. Stop its build processes before moving it aside.`);
  }
  const started = Date.now();
  try {
    while (!native.tryLock(fd)) {
      if (Date.now() - started >= timeout) throw new Error(`Timed out waiting for build lock: ${lock}`);
      await delay(interval);
    }
  } catch (error) {closeSync(fd); throw error;}
  if (!owned.size) process.once('exit', cleanup);
  const owner = {fd};
  owned.add(owner);
  return () => release(owner);
}
