import {createHash} from 'node:crypto';
import {existsSync} from 'node:fs';
import {mkdir, mkdtemp, readFile, readdir, rename, rm, writeFile} from 'node:fs/promises';
import {basename, isAbsolute, relative, resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {acquireBuildLock} from '../lib/build-lock.mjs';
import {writeFileAtomic} from '../lib/atomic-file.mjs';

const defaultProject = fileURLToPath(new URL('..', import.meta.url));
const receiptName = '.build.json';
const digest = bytes => createHash('sha256').update(bytes).digest('hex');
import {i18nCompilerOptions} from '../lib/i18n-build.mjs';

export async function i18nBuildInputs(project = defaultProject) {
  project = resolve(project);
  const messages = await readdir(resolve(project, 'messages'));
  return [resolve(project, 'project.inlang/settings.json'),
    ...messages.filter(name => name.endsWith('.json')).sort().map(name => resolve(project, 'messages', name)),
    resolve(project, 'node_modules/@inlang/paraglide-js/package.json'),
    resolve(project, 'node_modules/@inlang/plugin-message-format/package.json'),
    fileURLToPath(import.meta.url), fileURLToPath(new URL('../lib/i18n-build.mjs', import.meta.url)), fileURLToPath(new URL('../lib/build-lock.mjs', import.meta.url)),
    fileURLToPath(new URL('../lib/build-lock-host.mjs', import.meta.url)), fileURLToPath(new URL('../lib/node-runtime.mjs', import.meta.url)),
    fileURLToPath(new URL('../lib/atomic-file.mjs', import.meta.url))];
}
async function identity(project) {
  const files = await i18nBuildInputs(project);
  return digest(JSON.stringify(await Promise.all(files.map(async path => [relative(project, path).replaceAll('\\', '/'), digest(await readFile(path))]))));
}
function confined(root, path) {
  const suffix = relative(root, path);
  if (!suffix || isAbsolute(suffix) || suffix === '..' || suffix.startsWith('../') || suffix.startsWith('..\\')) throw new Error('Generated translation path escaped its directory');
  return path;
}
async function discard(cache, path) {await rm(confined(cache, path), {recursive: true, force: true});}
async function outputHashes(directory, prefix = '') {
  const entries = {};
  for (const file of await readdir(directory, {withFileTypes: true})) {
    const name = prefix + file.name, path = resolve(directory, file.name);
    if (file.isFile() && !prefix && file.name === receiptName) continue;
    if (file.isDirectory()) Object.assign(entries, await outputHashes(path, name + '/'));
    else if (file.isFile()) entries[name] = digest(await readFile(path));
    else throw new Error(`Unexpected generated translation file: ${name}`);
  }
  return Object.fromEntries(Object.entries(entries).sort(([a], [b]) => a.localeCompare(b)));
}
async function valid(directory, hash) {
  try {
    const receipt = JSON.parse(await readFile(resolve(directory, receiptName), 'utf8'));
    if (receipt.schema !== 'eagler-touhou/i18n-build/1' || receipt.inputHash !== hash) return false;
    return JSON.stringify(await outputHashes(directory)) === JSON.stringify(receipt.outputs);
  } catch (error) {
    if (error?.code === 'ENOENT' || error instanceof SyntaxError) return false;
    throw error;
  }
}
async function entrypoints(generation, output) {
  let previous;
  try {previous = JSON.parse(await readFile(resolve(output, receiptName), 'utf8'));} catch (error) {if (error?.code !== 'ENOENT') throw error;}
  await mkdir(output, {recursive: true});
  // The watched source directory stays in place on Windows. Each entry points
  // into one complete immutable generation, including all relative imports.
  for (const file of (await readdir(generation)).filter(name => /\.(?:js|ts)$/.test(name))) {
    const target = relative(output, resolve(generation, file)).replaceAll('\\', '/');
    const text = `export * from ${JSON.stringify(target.startsWith('.') ? target : './' + target)};\n`;
    const path = resolve(output, file);
    if (existsSync(path) && await readFile(path, 'utf8') === text) continue;
    await writeFileAtomic(path, text);
  }
  // Retire the initial direct compiler output after its entries are replaced.
  if (previous?.schema === 'eagler-touhou/i18n-build/1') {
    for (const file of Object.keys(previous.outputs)) if (file.startsWith('messages/')) await rm(confined(output, resolve(output, file)), {force: true});
  }
  await writeFileAtomic(resolve(output, receiptName), JSON.stringify({schema: 'eagler-touhou/i18n-entry/1', generation}) + '\n');
}
async function prepare(project, force, compiler) {
  const settings = resolve(project, 'project.inlang/settings.json');
  if (!existsSync(settings)) return {built: false, mode: 'prebuilt'};
  const cache = resolve(project, '.cache/i18n'), output = resolve(project, 'src/paraglide');
  await mkdir(cache, {recursive: true});
  const release = await acquireBuildLock(resolve(cache, 'build.lock'));
  try {
    for (let attempt = 0; attempt < 3; attempt++) {
      const hash = await identity(project);
      if (!force) {
        try {
          const current = JSON.parse(await readFile(resolve(cache, 'current.json'), 'utf8'));
          const generation = confined(cache, resolve(cache, current.generation));
          if (await valid(generation, hash)) {await entrypoints(generation, output); return {built: false, inputHash: hash};}
        } catch (error) {if (error?.code !== 'ENOENT' && !(error instanceof SyntaxError)) throw error;}
      }
      const staging = await mkdtemp(resolve(cache, 'staging-'));
      try {
        const compile = compiler ?? (await import('@inlang/paraglide-js')).compile;
        await compile({...i18nCompilerOptions, project: resolve(project, 'project.inlang'), outdir: staging});
        if (await identity(project) !== hash) continue;
        const outputs = await outputHashes(staging);
        for (const file of ['messages.js', 'messages.d.ts', 'runtime.js']) if (!outputs[file]) throw new Error(`Missing compiled translation: ${file}`);
        await writeFile(resolve(staging, receiptName), JSON.stringify({schema: 'eagler-touhou/i18n-build/1', inputHash: hash, outputs}) + '\n');
        const generation = resolve(cache, basename(staging).replace('staging-', 'generation-'));
        await rename(staging, generation);
        await entrypoints(generation, output);
        await writeFileAtomic(resolve(cache, 'current.json'), JSON.stringify({generation: basename(generation)}) + '\n');
        return {built: true, inputHash: hash};
      } finally {await discard(cache, staging);}
    }
    throw new Error('Translation inputs changed repeatedly during compilation');
  } finally {await release();}
}
/** CLI, type checks, services and Vite share the same generation and lock. */
export function ensureI18nBuild({project = defaultProject, force = false, compiler} = {}) {
  project = resolve(project);
  return prepare(project, force, compiler);
}
