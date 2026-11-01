import { spawnSync } from 'node:child_process';
import { mkdtemp, rm, writeFile } from 'node:fs/promises';
import { tmpdir } from 'node:os';
import { resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { describe, expect, test } from 'vitest';
import { assertSupportedBun, isSupportedBunVersion } from '../../host/lib/bun-environment.mjs';

describe('Bun execution contract', () => {
  test('runs the Vitest worker on Bun', () => {
    expect(process.versions.bun).toBeDefined();
    expect(isSupportedBunVersion(process.versions.bun)).toBe(true);
    assertSupportedBun();
  });
  test('rejects unsupported or malformed runtime versions', () => {
    for (const value of ['1.4.3', '1.3.99', '0.9.0', 'unknown']) {
      expect(isSupportedBunVersion(value)).toBe(false);
      expect(() => assertSupportedBun(value)).toThrow('Bun >= 1.4.4');
    }
    for (const value of ['1.4.4', '1.5.0', '2.0.0']) expect(isSupportedBunVersion(value)).toBe(true);
  });
});

test('Bun syntax validation parses without executing source and rejects malformed code', async () => {
  const root = await mkdtemp(resolve(tmpdir(), 'touhou-bun-syntax-'));
  const script = fileURLToPath(new URL('../../scripts/check-javascript-syntax.mjs', import.meta.url));
  try {
    const valid = resolve(root, 'syntax-valid.mjs');
    await writeFile(valid, 'throw new Error("NEVER_EXECUTED");');
    const parsed = spawnSync(process.execPath, [script, valid], { encoding: 'utf8' });
    expect(parsed.status).toBe(0);
    expect(parsed.stdout).toContain('JavaScript syntax: PASS');
    const invalid = resolve(root, 'syntax-invalid.mjs');
    await writeFile(invalid, 'const broken = ;');
    const rejected = spawnSync(process.execPath, [script, invalid], { encoding: 'utf8' });
    expect(rejected.status).not.toBe(0);
    expect(rejected.stderr).toContain('syntax-invalid.mjs:');
  } finally {
    await rm(root, { recursive: true, force: true });
  }
});
