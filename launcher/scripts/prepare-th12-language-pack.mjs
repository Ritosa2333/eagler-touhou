import {createHash} from 'node:crypto';
import {readFile, stat, writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {PRODUCT_CONTENT} from '../lib/content-definition.mjs';
import {workspacePath} from '../lib/workspace-layout.mjs';
import {run} from '../host/lib/process.mjs';

const args = new Map();
for (let index = 2; index < process.argv.length; index++) {
  const argument = process.argv[index];
  if (!argument.startsWith('--')) throw new Error(`unexpected argument: ${argument}`);
  const separator = argument.indexOf('=');
  const key = argument.slice(2, separator < 0 ? undefined : separator);
  const value = separator < 0 ? process.argv[++index] : argument.slice(separator + 1);
  if (!value || value.startsWith('--')) throw new Error(`missing value for --${key}`);
  args.set(key, value);
}

const project = fileURLToPath(new URL('..', import.meta.url));
const game = args.get('game') || 'th12';
const language = args.get('language') || 'lang_zh-hant';
const preparation = PRODUCT_CONTENT[game]?.hostPreparation?.languagePack;
if (game !== 'th12' || preparation?.kind !== 'retail-overlay-compiler' || !preparation.languages.includes(language)) {
  throw new Error(`unsupported retail overlay language: ${game}/${language}`);
}
if (!args.get('original') || !args.get('output')) {
  throw new Error('Supply --original=INSTALLATION and --output=CONTENT_DIRECTORY');
}
const original = resolve(args.get('original'));
const output = resolve(args.get('output'));

async function fileExists(path) {
  try { return (await stat(path)).isFile(); }
  catch (error) { if (error.code === 'ENOENT') return false; throw error; }
}

const optionalInputs = await Promise.all(preparation.optionalFiles.map(name => fileExists(resolve(original, name))));
if (preparation.optional && optionalInputs.every(present => !present)) {
  // Do not consume a stale generated catalog after the optional inputs disappear.
  console.log(JSON.stringify({game, output, skipped: true, languages: []}));
  process.exit(0);
}
if (!optionalInputs.every(Boolean)) {
  throw new Error(`${game}: incomplete translation installation; supply ${preparation.optionalFiles.join(' and ')}`);
}

async function buildSource(role) {
  const source = preparation.buildSources.find(entry => entry.role === role);
  if (!source) throw new Error(`${game}: missing declared ${role} build source`);
  const bundled = resolve(project, source.target);
  if (await fileExists(bundled)) return bundled;
  const canonical = workspacePath(source.repository, source.source);
  if (!await fileExists(canonical)) throw new Error(`${game}: language ${role} source is unavailable: ${canonical}`);
  return canonical;
}

const compiler = await buildSource('compiler');
const sourceManifest = await buildSource('source-manifest');
const python = args.get('python') || process.env.EAGLER_CONTENT_PYTHON || process.env.TH12_PYTHON || 'python';
const thdat = args.get('thdat') || process.env.EAGLER_CONTENT_THDAT || process.env.TH12_THDAT || process.env.THDAT;
const result = await run(python, [
  compiler, '--original', original, '--output', output, '--source-manifest', sourceManifest,
  ...(thdat ? ['--thdat', thdat] : []),
], {cwd: project, capture: true});
if (result.stdout) process.stderr.write(`${result.stdout}\n`);
if (result.stderr) process.stderr.write(`${result.stderr}\n`);

const catalog = JSON.parse(await readFile(resolve(output, 'language/catalog.json'), 'utf8'));
if (catalog.schema !== 'th12-localization-catalog/1' || catalog.game !== game ||
    catalog.defaultLanguage !== 'ja' || !Array.isArray(catalog.languages) || catalog.languages.length !== 1) {
  throw new Error(`${game}: invalid retail overlay catalog`);
}
const entry = catalog.languages[0];
if (entry.id !== language || entry.pack?.url !== `language/${language}.zip` || entry.pack.runtimeVersion !== 'independent') {
  throw new Error(`${game}: unexpected retail overlay language entry`);
}
const archive = await readFile(resolve(output, entry.pack.url));
const sha256 = createHash('sha256').update(archive).digest('hex');
const manifest = JSON.parse(await readFile(resolve(output, `language/${language}.manifest.json`), 'utf8'));
if (entry.pack.bytes !== archive.length || entry.pack.sha256 !== sha256 ||
    manifest.schema !== 'eagler-touhou/thcrap-static-pack/1' || manifest.game !== game ||
    manifest.language !== language || manifest.runtimeVersion !== 'independent' ||
    !Array.isArray(manifest.files) || entry.pack.files !== manifest.files.length) {
  throw new Error(`${game}: retail overlay output identity does not match its catalog`);
}
// The compiler's source catalog stays with its coverage report. The Host reads
// this standard catalog at the content root and publishes only its static ZIP.
await writeFile(resolve(output, 'catalog.json'), `${JSON.stringify({
  schema: 'eagler-touhou/thcrap-static-catalog/1', game,
  runtimeVersion: 'independent', defaultLanguage: 'ja', languages: [entry],
}, null, 2)}\n`);
console.log(JSON.stringify({game, output, skipped: false, languages: [language],
  files: manifest.files.length, bytes: archive.length, sha256, pack: entry.pack.url}));
