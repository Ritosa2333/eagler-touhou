import { execFileSync } from 'node:child_process';
import { createHash } from 'node:crypto';
import { mkdir, mkdtemp, readFile, rm, writeFile } from 'node:fs/promises';
import { tmpdir } from 'node:os';
import { resolve } from 'node:path';
import { expect, test } from 'vitest';
import { ensureBunDependencies, installedBunEnvironmentMatches } from '../../host/lib/bun-environment.mjs';

const hash = (bytes: string) => createHash('sha256').update(bytes).digest('hex');
test('Bun bootstrap locks a fresh local dependency, reuses its stamp and detects drift', async () => {
  const root = await mkdtemp(resolve(tmpdir(), 'touhou-bun-bootstrap-'));
  try {
    await mkdir(resolve(root, 'vendor/fixture'), { recursive: true });
    await writeFile(
      resolve(root, 'vendor/fixture/package.json'),
      JSON.stringify({ name: 'fixture', version: '1.2.3' }),
    );
    const manifest = { name: 'bun-bootstrap-fixture', private: true, dependencies: { fixture: 'file:vendor/fixture' } };
    await writeFile(resolve(root, 'package.json'), JSON.stringify(manifest));
    execFileSync(process.execPath, ['install', '--lockfile-only', '--ignore-scripts'], { cwd: root, stdio: 'pipe' });
    const identity = hash(await readFile(resolve(root, 'bun.lock'), 'utf8'));
    await ensureBunDependencies(root, ['fixture']);
    expect(await installedBunEnvironmentMatches(root, ['fixture'], identity)).toBe(true);
    const stamp = await readFile(resolve(root, 'node_modules/.eagler-host-lock.json'), 'utf8');
    await ensureBunDependencies(root, ['fixture']);
    expect(await readFile(resolve(root, 'node_modules/.eagler-host-lock.json'), 'utf8')).toBe(stamp);
    await rm(resolve(root, 'node_modules/fixture'), { recursive: true, force: true });
    expect(await installedBunEnvironmentMatches(root, ['fixture'], identity)).toBe(false);
    await mkdir(resolve(root, 'node_modules/fixture'));
    await writeFile(
      resolve(root, 'node_modules/fixture/package.json'),
      JSON.stringify({ name: 'fixture', version: '9.0.0' }),
    );
    expect(await installedBunEnvironmentMatches(root, ['fixture'], identity)).toBe(false);
    await writeFile(
      resolve(root, 'node_modules/fixture/package.json'),
      JSON.stringify({ name: 'fixture', version: '1.2.3' }),
    );
    expect(await installedBunEnvironmentMatches(root, ['fixture'], identity)).toBe(true);
    await writeFile(resolve(root, 'package.json'), JSON.stringify({ ...manifest, description: 'changed manifest' }));
    expect(await installedBunEnvironmentMatches(root, ['fixture'], identity)).toBe(false);
    await writeFile(resolve(root, 'package.json'), JSON.stringify(manifest));
    expect(await installedBunEnvironmentMatches(root, ['fixture'], identity)).toBe(true);
    await writeFile(resolve(root, 'bun.lock'), '{broken');
    expect(await installedBunEnvironmentMatches(root, ['fixture'], identity)).toBe(false);
  } finally {
    await rm(root, { recursive: true, force: true });
  }
});
