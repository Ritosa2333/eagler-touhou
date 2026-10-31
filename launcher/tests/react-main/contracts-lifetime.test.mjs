import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {existsSync} from 'node:fs';
import {mkdir, mkdtemp, rename, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {test} from 'vitest';
import {ensureContractsBuild} from '../../lib/contracts-build.mjs';

const project = fileURLToPath(new URL('../..', import.meta.url));
test('failed frontend loading releases contracts build ownership before the CLI exits', async () => {
  await mkdir(resolve(project, '.cache/tests'), {recursive: true});
  const temporary = await mkdtemp(resolve(project, '.cache/tests/contracts-lifetime-'));
  const built = await ensureContractsBuild();
  const lock = resolve(built.root, '../.build.lock');
  try {
    const failed = spawnSync(process.execPath, ['scripts/package-server.mjs'], {cwd: project, encoding: 'utf8', timeout: 10_000,
      env: {...process.env, EAGLER_FRONTEND: 'react', EAGLER_REACT_BUILD_DIRECTORY: resolve(temporary, 'missing')}});
    assert.equal(failed.status, 1);
    assert.match(failed.stderr, /React frontend artifact is missing/);
    assert.equal(existsSync(lock), false, 'a rejected CLI must not block the next contracts consumer');
  } finally {
    // The child is stopped, and files run serially. Preserve a failed lease as
    // part of this fixture until cleanup so a failing regression cannot stall others.
    if (existsSync(lock)) await rename(lock, resolve(temporary, 'failed-build-lock'));
    await rm(temporary, {recursive: true, force: true});
  }
});
