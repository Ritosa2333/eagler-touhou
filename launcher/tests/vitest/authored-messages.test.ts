import { access, cp, mkdir, mkdtemp, rm, symlink, writeFile } from 'node:fs/promises';
import { tmpdir } from 'node:os';
import { resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { build } from 'esbuild';
import { expect, test } from 'vitest';
import { authoredSourcesPlugin } from '../../lib/authored-source-resolution.mjs';

const launcher = fileURLToPath(new URL('../..', import.meta.url));

async function fixture() {
  const project = await mkdtemp(resolve(tmpdir(), 'eagler-authored-messages-'));
  await mkdir(resolve(project, 'scripts'));
  await mkdir(resolve(project, 'src/launcher'), { recursive: true });
  await mkdir(resolve(project, 'messages'));
  await cp(resolve(launcher, 'project.inlang'), resolve(project, 'project.inlang'), { recursive: true });
  await cp(resolve(launcher, 'scripts/compile-i18n.mjs'), resolve(project, 'scripts/compile-i18n.mjs'));
  await symlink(
    resolve(launcher, 'node_modules'),
    resolve(project, 'node_modules'),
    process.platform === 'win32' ? 'junction' : 'dir',
  );
  for (const [locale, text] of [
    ['zh-CN', '你好'],
    ['en', 'Hello'],
    ['ja', 'こんにちは'],
  ]) {
    await writeFile(resolve(project, `messages/${locale}.json`), JSON.stringify({ greeting: text }));
  }
  await writeFile(resolve(project, 'src/launcher/probe.mts'), 'export { greeting } from "../paraglide/messages.js";');
  return project;
}

async function bundle(project: string) {
  const result = await build({
    absWorkingDir: project,
    entryPoints: ['src/launcher/probe.mts'],
    bundle: true,
    write: false,
    platform: 'node',
    format: 'esm',
    logLevel: 'silent',
    plugins: [authoredSourcesPlugin(project)],
  });
  return import(`data:text/javascript;base64,${Buffer.from(result.outputFiles[0].contents).toString('base64')}`);
}

test('independent source bundles prepare real Paraglide messages from a cold checkout', async () => {
  const project = await fixture();
  try {
    const modules = await Promise.all([bundle(project), bundle(project)]);
    for (const messages of modules) {
      expect(messages.greeting({}, { locale: 'ja' })).toBe('こんにちは');
      expect(messages.greeting({}, { locale: 'en' })).toBe('Hello');
    }
    await rm(resolve(project, 'src/paraglide'), { recursive: true });
    expect((await bundle(project)).greeting({}, { locale: 'zh-CN' })).toBe('你好');
  } finally {
    await rm(project, { recursive: true, force: true });
  }
});

test('message compiler errors fail the source build and release ownership for a retry', async () => {
  const project = await fixture();
  try {
    await writeFile(resolve(project, 'messages/ja.json'), '{broken');
    await expect(bundle(project)).rejects.toThrow();
    await expect(access(resolve(project, '.cache/authored-messages.lock'))).rejects.toThrow();
    await writeFile(resolve(project, 'messages/ja.json'), JSON.stringify({ greeting: '修復済み' }));
    expect((await bundle(project)).greeting({}, { locale: 'ja' })).toBe('修復済み');
  } finally {
    await rm(project, { recursive: true, force: true });
  }
});
