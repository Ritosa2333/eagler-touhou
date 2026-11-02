import {mkdir} from 'node:fs/promises';
import {rmSync} from 'node:fs';
import {parse, resolve} from 'node:path';
import {setTimeout as delay} from 'node:timers/promises';

const owned = new Map();
function remove(owner) {
  const lock = owner.path;
  if (owned.get(lock) !== owner) return;
  // Only directories created by this process can reach recursive removal.
  if (lock !== resolve(lock) || lock === parse(lock).root || !lock.endsWith('.lock')) throw new Error('Invalid build lock path');
  rmSync(lock, {recursive: true, force: true});
  owned.delete(lock);
  if (!owned.size) process.removeListener('exit', cleanup);
}
function cleanup() {for (const owner of [...owned.values()]) {try {remove(owner);} catch {}}}

/** A directory lock survives async work and is released on ordinary process exit. */
export async function acquireBuildLock(path, {timeout = 120000, interval = 25} = {}) {
  const lock = resolve(path);
  if (lock === parse(lock).root || !lock.endsWith('.lock')) throw new Error('Build lock must name a .lock directory');
  const started = Date.now();
  for (;;) {
    try {await mkdir(lock); break;}
    catch (error) {
      if (error?.code !== 'EEXIST') throw error;
      if (Date.now() - started >= timeout) throw new Error(`Timed out waiting for build lock: ${lock}`);
      await delay(interval);
    }
  }
  if (!owned.size) process.once('exit', cleanup);
  const owner = {path: lock};
  owned.set(lock, owner);
  return () => remove(owner);
}
