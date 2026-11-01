import { createHash } from 'node:crypto';
import { existsSync } from 'node:fs';
import { readFile, writeFile } from 'node:fs/promises';
import { resolve } from 'node:path';
import { run } from './process.mjs';

const STAMP_FILE = '.eagler-host-lock.json';
const digest = (value) => createHash('sha256').update(value).digest('hex');
export function isSupportedBunVersion(version = process.versions.bun) {
  const parts = /^(\d+)\.(\d+)\.(\d+)(?:-|$)/.exec(String(version));
  if (!parts) return false;
  const [major, minor, patch] = parts.slice(1).map(Number);
  return major > 1 || (major === 1 && (minor > 4 || (minor === 4 && patch >= 4)));
}
export function assertSupportedBun(version = process.versions.bun) {
  if (!isSupportedBunVersion(version))
    throw new Error(`Bun >= 1.4.4 is required; current version is ${version ?? 'unavailable'}`);
}
export function usesBunLock(project) {
  return Boolean(process.versions.bun && existsSync(resolve(project, 'bun.lock')));
}
async function inputs(project) {
  assertSupportedBun();
  const lockBytes = await readFile(resolve(project, 'bun.lock'));
  const manifest = await readFile(resolve(project, 'package.json'));
  return {
    identity: digest(lockBytes),
    manifestSha256: digest(manifest),
    lock: globalThis.Bun.JSONC.parse(lockBytes.toString()),
  };
}
async function versionsMatch(project, required, lock) {
  for (const name of required) {
    try {
      const specifier = lock.packages?.[name]?.[0];
      let expected = specifier?.startsWith(name + '@') ? specifier.slice(name.length + 1) : null;
      if (expected?.startsWith('file:')) {
        expected = JSON.parse(await readFile(resolve(project, expected.slice(5), 'package.json'), 'utf8')).version;
      }
      const actual = JSON.parse(await readFile(resolve(project, 'node_modules', name, 'package.json'), 'utf8')).version;
      if (!expected || expected !== actual) return false;
    } catch {
      return false;
    }
  }
  return true;
}
export async function installedBunEnvironmentMatches(project, required, identity) {
  try {
    const stamp = JSON.parse(await readFile(resolve(project, 'node_modules', STAMP_FILE), 'utf8'));
    const current = await inputs(project);
    return (
      stamp.schema === 'eagler-touhou/bun-environment/1' &&
      stamp.runtimeVersion === process.versions.bun &&
      stamp.lockSha256 === identity &&
      current.identity === identity &&
      stamp.manifestSha256 === current.manifestSha256 &&
      JSON.stringify(stamp.required) === JSON.stringify(required) &&
      (await versionsMatch(project, required, current.lock))
    );
  } catch {
    return false;
  }
}
export async function ensureBunDependencies(project, required) {
  const current = await inputs(project);
  if (await installedBunEnvironmentMatches(project, required, current.identity)) return;
  console.log(`[Build] Refreshing locked Bun dependencies (${required.join(', ')})`);
  await run(process.execPath, ['install', '--frozen-lockfile', '--ignore-scripts'], { cwd: project });
  if (!(await versionsMatch(project, required, current.lock)))
    throw new Error('Bun install completed with missing or mismatched locked dependencies');
  await writeFile(
    resolve(project, 'node_modules', STAMP_FILE),
    JSON.stringify(
      {
        schema: 'eagler-touhou/bun-environment/1',
        runtimeVersion: process.versions.bun,
        lockSha256: current.identity,
        manifestSha256: current.manifestSha256,
        required,
      },
      null,
      2,
    ) + '\n',
  );
}
