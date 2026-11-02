import {test, afterAll as after} from 'vitest';
import assert from 'node:assert/strict';
import {existsSync} from 'node:fs';
import {mkdir, mkdtemp, readFile, readdir, rm, writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {spawn} from 'node:child_process';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {acquireBuildLock} from '../../lib/build-lock.mjs';
import {ensureI18nBuild} from '../../lib/i18n-build.mjs';

const root = fileURLToPath(new URL('../../', import.meta.url));
await mkdir(resolve(root, '.cache'), {recursive: true});
const work = await mkdtemp(resolve(root, '.cache/i18n-tests-'));
after(() => rm(work, {recursive: true, force: true}));
let serial = 0;
async function fixture() {
  const project = resolve(work, 'project-' + ++serial);
  for (const directory of ['messages', 'project.inlang', 'node_modules/@inlang/paraglide-js', 'node_modules/@inlang/plugin-message-format']) await mkdir(resolve(project, directory), {recursive: true});
  for (const path of ['project.inlang/settings.json', 'node_modules/@inlang/paraglide-js/package.json', 'node_modules/@inlang/plugin-message-format/package.json']) await writeFile(resolve(project, path), '{}');
  await writeFile(resolve(project, 'messages/en.json'), 'first');
  return project;
}
async function compile({project, outdir}) {
  const text = await readFile(resolve(project, '../messages/en.json'), 'utf8');
  await writeFile(resolve(outdir, 'messages.js'), 'export const text = ' + JSON.stringify(text) + ';');
  await writeFile(resolve(outdir, 'messages.d.ts'), 'export declare const text: string;');
  await writeFile(resolve(outdir, 'runtime.js'), 'export const baseLocale = "zh-CN";');
}
const cache = project => resolve(project, '.cache/i18n');
async function current(project) {return JSON.parse(await readFile(resolve(cache(project), 'current.json'), 'utf8'));}
async function message(project) {return import(pathToFileURL(resolve(project, 'src/paraglide/messages.js')).href + '?v=' + ++serial);}

test('a retired lock release cannot remove a later owner; contention is bounded', async () => {
  const path = resolve(work, 'owner.lock'), before = process.listenerCount('exit');
  const first = await acquireBuildLock(path); first();
  const second = await acquireBuildLock(path); first(); assert.equal(existsSync(path), true);
  await assert.rejects(acquireBuildLock(path, {timeout: 5, interval: 1}), /Timed out waiting/);
  second(); second(); assert.equal(existsSync(path), false); assert.equal(process.listenerCount('exit'), before);
});
test('ordinary error exit releases only its own acquired build lock', async () => {
  const path = resolve(work, 'exit.lock');
  const script = `import {acquireBuildLock} from ${JSON.stringify(new URL('../../lib/build-lock.mjs', import.meta.url).href)};await acquireBuildLock(${JSON.stringify(path)});process.exit(17);`;
  const code = await new Promise((yes, no) => {
    const child = spawn(process.execPath, ['--input-type=module', '-e', script], {stdio: 'pipe'});
    let error = ''; child.stderr.on('data', bytes => {error += bytes;}); child.once('error', no);
    child.once('exit', code => error ? no(new Error(error)) : yes(code));
  });
  assert.equal(code, 17); assert.equal(existsSync(path), false);
});
test('concurrent translation consumers share compilation, reuse verified output and repair entries', async () => {
  const project = await fixture(); let calls = 0;
  const compiler = async input => {calls++; await compile(input);};
  const first = ensureI18nBuild({project, compiler}), second = ensureI18nBuild({project, compiler});
  assert.equal(first, second); assert.equal((await first).built, true); assert.equal(calls, 1);
  assert.equal((await ensureI18nBuild({project, compiler})).built, false); assert.equal(calls, 1);
  await rm(resolve(project, 'src/paraglide/messages.js'));
  assert.equal((await ensureI18nBuild({project, compiler})).built, false);
  assert.equal((await message(project)).text, 'first'); assert.equal(calls, 1);
});
test('input changes during compilation retry before publishing a generation', async () => {
  const project = await fixture(); let calls = 0;
  const result = await ensureI18nBuild({project, compiler: async input => {
    await compile(input);
    if (++calls === 1) await writeFile(resolve(project, 'messages/en.json'), 'second');
  }});
  assert.equal(result.built, true); assert.equal(calls, 2); assert.equal((await message(project)).text, 'second');
  assert.equal((await readdir(cache(project))).some(name => name.startsWith('staging-')), false);
});
test('a partial failed compile preserves the current entry and can be retried', async () => {
  const project = await fixture(); await ensureI18nBuild({project, compiler: compile});
  const previous = await readFile(resolve(project, 'src/paraglide/messages.js'), 'utf8'), generation = await current(project);
  await writeFile(resolve(project, 'messages/en.json'), 'next');
  await assert.rejects(ensureI18nBuild({project, compiler: async ({outdir}) => {
    await writeFile(resolve(outdir, 'messages.js'), 'partial'); throw new Error('compiler failed');
  }}), /compiler failed/);
  assert.equal(await readFile(resolve(project, 'src/paraglide/messages.js'), 'utf8'), previous);
  assert.deepEqual(await current(project), generation); assert.equal(existsSync(resolve(cache(project), 'build.lock')), false);
  assert.equal((await readdir(cache(project))).some(name => name.startsWith('staging-')), false);
  await ensureI18nBuild({project, compiler: compile}); assert.equal((await message(project)).text, 'next');
});
test('corrupt generated output is rebuilt into a new immutable generation', async () => {
  const project = await fixture(); await ensureI18nBuild({project, compiler: compile});
  const previous = await current(project);
  await writeFile(resolve(cache(project), previous.generation, 'messages.js'), 'broken');
  assert.equal((await ensureI18nBuild({project, compiler: compile})).built, true);
  assert.notEqual((await current(project)).generation, previous.generation); assert.equal((await message(project)).text, 'first');
});
test('prebuilt packages do not import the compiler or create source output', async () => {
  const project = resolve(work, 'prebuilt'); await mkdir(project);
  assert.deepEqual(await ensureI18nBuild({project, compiler: () => {throw new Error('unexpected');}}), {built: false, mode: 'prebuilt'});
  assert.equal(existsSync(resolve(project, '.cache')), false);
});
