import {test} from 'vitest';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {execFileSync} from 'node:child_process';
import {mkdtemp, mkdir, readFile, writeFile, rm, stat} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve, dirname} from 'node:path';
import {assertFrontendRuntime} from '../../scripts/ui-rewrite/check-runtime.mjs';
import {resolveBunExecutable, assertSupportedBun, ensureBunDependencies, installedBunEnvironmentMatches, isSupportedBunVersion} from '../../host/lib/bun-environment.mjs';

test('frontend CLI and isolated worker use the selected Bun runtime', () => {
  assert.equal(assertFrontendRuntime(), process.env.EAGLER_FRONTEND_BUN_VERSION);
  for (const version of ['1.4.4', '1.4.5', '1.5.0', '2.0.0']) assert.equal(isSupportedBunVersion(version), true);
  for (const version of ['1.4.3', '1.3.99', '0.9.0', undefined, 'unknown']) assert.throws(() => assertSupportedBun(version ?? 'unknown'), /Bun >= 1\.4\.4/);
  assert.throws(() => assertFrontendRuntime('0.0.0'), /differs from CLI/);
});

test('Bun first installation, warm reuse and input/install drift use a verified receipt', async () => {
  const project = await mkdtemp(resolve(tmpdir(), 'eagler Bun first install '));
  try {
    await mkdir(resolve(project, 'vendor', 'fixture'), {recursive: true});
    await writeFile(resolve(project, 'vendor/fixture/package.json'), JSON.stringify({
      name: 'fixture', version: '1.0.0', scripts: {postinstall: 'node -e "require(\'fs\').writeFileSync(\'executed\',\'bad\')"'},
    }));
    const manifest = JSON.stringify({name: 'bun-environment-fixture', private: true, dependencies: {fixture: 'file:vendor/fixture'}});
    await writeFile(resolve(project, 'package.json'), manifest);
    execFileSync(resolveBunExecutable(), ['install', '--lockfile-only', '--ignore-scripts'], {cwd: project, windowsHide: true, stdio: 'pipe'});
    await ensureBunDependencies(project, ['fixture']);
    const lock = await readFile(resolve(project, 'bun.lock'));
    const identity = createHash('sha256').update(lock).digest('hex');
    assert.equal(await installedBunEnvironmentMatches(project, ['fixture'], identity), true);
    const receiptPath = resolve(project, 'node_modules/.eagler-host-lock.json');
    const first = await stat(receiptPath);
    await ensureBunDependencies(project, ['fixture']);
    assert.equal((await stat(receiptPath)).mtimeMs, first.mtimeMs, 'warm environment is reused');
    await assert.rejects(readFile(resolve(project, 'node_modules/fixture/executed')), /ENOENT/);
    await writeFile(resolve(project, 'package.json'), manifest + '\n');
    assert.equal(await installedBunEnvironmentMatches(project, ['fixture'], identity), false, 'manifest bytes changed');
    await writeFile(resolve(project, 'package.json'), manifest);
    await writeFile(resolve(project, 'bun.lock'), Buffer.concat([lock, Buffer.from('\n')]));
    assert.equal(await installedBunEnvironmentMatches(project, ['fixture'], identity), false, 'lock changed outside required dependency');
    await writeFile(resolve(project, 'bun.lock'), lock);
    const installedPath = resolve(project, 'node_modules/fixture/package.json');
    const installed = await readFile(installedPath, 'utf8');
    await writeFile(installedPath, JSON.stringify({name: 'fixture', version: '9.0.0'}));
    assert.equal(await installedBunEnvironmentMatches(project, ['fixture'], identity), false, 'installed version changed');
    await writeFile(installedPath, installed);
    const receipt = await readFile(receiptPath, 'utf8');
    await writeFile(receiptPath, receipt.replace(process.versions.bun, '0.0.0'));
    assert.equal(await installedBunEnvironmentMatches(project, ['fixture'], identity), false, 'runtime changed');
    await writeFile(receiptPath, receipt);
    await rm(installedPath);
    assert.equal(await installedBunEnvironmentMatches(project, ['fixture'], identity), false, 'installed dependency missing');
  } finally {
    assert.equal(dirname(project), resolve(tmpdir()));
    await rm(project, {recursive: true, force: true});
  }
});
