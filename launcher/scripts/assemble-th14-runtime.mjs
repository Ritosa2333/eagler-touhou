import { createHash } from 'node:crypto';
import { mkdir, readFile, writeFile } from 'node:fs/promises';
import { gzipSync } from 'node:zlib';
import { dirname, resolve } from 'node:path';
import { runtimeFileNames } from '../lib/runtime-release.mjs';
import { assertRuntimeDataShell } from '../lib/runtime-data-provider.mjs';
import { assertRuntimeProtocolSources } from '../lib/runtime-protocol-shell.mjs';

const args = Object.create(null);
for (const argument of process.argv.slice(2)) {
  const match = /^--(template|core-build|output|max-file-bytes)=(.+)$/.exec(argument);
  if (!match || args[match[1]]) throw Error('Use --template=DIR --core-build=DIR --output=NEW_DIRECTORY [--max-file-bytes=N]');
  args[match[1]] = match[1] === 'max-file-bytes' ? match[2] : resolve(match[2]);
}
const template = args.template, core = args['core-build'], output = args.output;
if (!template || !core || !output || new Set([template, core, output]).size !== 3)
  throw Error('Supply distinct template, core build and output directories');
const maxBytes = args['max-file-bytes'] == null ? Infinity : Number(args['max-file-bytes']);
if (args['max-file-bytes'] != null && (!Number.isSafeInteger(maxBytes) || maxBytes < 1)) throw Error('Invalid file size limit');

const identity = bytes => ({ bytes: bytes.length, sha256: createHash('sha256').update(bytes).digest('hex') });
async function checked(directory, name, expected) {
  const bytes = await readFile(resolve(directory, name));
  const actual = identity(bytes);
  if (actual.bytes !== expected?.bytes || actual.sha256 !== expected?.sha256)
    throw Error('Stale TH14 runtime file: ' + name);
  return bytes;
}
const templateManifest = JSON.parse(await readFile(resolve(template, 'runtime-files.json'), 'utf8'));
if (templateManifest.schema !== 'eagler-touhou/runtime-directory/1' || templateManifest.game !== 'th14')
  throw Error('Missing TH14 runtime template manifest');
const names = runtimeFileNames('th14', templateManifest.files);
const build = JSON.parse(await readFile(resolve(core, 'build.json'), 'utf8'));
if (!build.flags?.includes('-DPORT_TITLE=14') || !build.runtimeFiles ||
    build.sha256 !== build.runtimeFiles['th14-sdl.wasm']?.sha256 ||
    build.loaderSha256 !== build.runtimeFiles['th14-sdl.mjs']?.sha256)
  throw Error('Expected a TH14 GLES core build with verified runtime files');

const practiceFiles = ['Th14PracticeUi.mjs', 'Th14PracticeCatalog.mjs'];
const replacements = new Map();
for (const [name, expected] of Object.entries(build.runtimeFiles)) {
  if ((!names.includes(name) && !practiceFiles.includes(name)) || !/^(?:th14-sdl\.(?:mjs|wasm)|Th14[A-Za-z0-9]+\.mjs|eagler-host\.mjs|shell\.mjs)$/.test(name))
    throw Error('Unexpected TH14 core output: ' + name);
  replacements.set(name, await checked(core, name, expected));
}
if (!replacements.has('th14-sdl.mjs') || !replacements.has('th14-sdl.wasm'))
  throw Error('TH14 core loader and Wasm must be replaced together');

const readTemplate = name => checked(template, name, templateManifest.files[name]);
const manifest = JSON.parse(await readTemplate('manifest.json'));
if (manifest.game !== 'th14' || manifest.protocol !== 'eagler-touhou/1' || manifest.execution?.backend !== 'gles')
  throw Error('Expected a TH14 GLES template');
const version = '1.00b-thp-' + build.sha256.slice(0, 12);
manifest.version = version;
const thprac = build.flags.includes('-DTH14_ENABLE_THPRAC=1');
if (thprac && !['shell.mjs', ...practiceFiles].every(name => replacements.has(name)))
  throw Error('Missing TH14 practice host shell or UI modules');
manifest.features = {...manifest.features, thprac};
manifest.execution.sha256 = build.sha256;
manifest.execution.loaderSha256 = build.loaderSha256;
replacements.set('manifest.json', Buffer.from(JSON.stringify(manifest, null, 2) + '\n'));

// Current core builds carry their source-derived host shell. Legacy core
// receipts can still use the verified template for older releases.
const originalShell = (replacements.get('shell.mjs') ?? await readTemplate('shell.mjs')).toString('utf8');
const buildInfo = /\/\*TH14_BUILD_INFO\*\/\{[^\n]+?\}(?=;)/g;
if ([...originalShell.matchAll(buildInfo)].length !== 1) throw Error('Missing TH14 shell build identity');
const shell = originalShell.replace(buildInfo, '/*TH14_BUILD_INFO*/' + JSON.stringify({
  version, completeGame: manifest.completeGame === true, nativeEquivalent: manifest.nativeEquivalent === true, sha256: build.sha256,
}));
const originalHtml = (await readTemplate('th14.html')).toString('utf8');
const canvas = /<canvas\b[^>]*\bid="canvas"[^>]*>/g;
if ([...originalHtml.matchAll(canvas)].length !== 1) throw Error('Missing TH14 canvas');
const html = originalHtml.replace(canvas, tag =>
  tag.replace(/\s(?:width|height)="\d+"/g, '').replace(/>$/, ' width="1280" height="960">'));
assertRuntimeDataShell(html, 'th14', 'normal');
const host = replacements.get('eagler-host.mjs') ?? await readTemplate('eagler-host.mjs');
assertRuntimeProtocolSources([shell, host.toString('utf8'),
  (await readTemplate('runtime-epoch.mjs')).toString('utf8')], 'th14', 'normal');
replacements.set('shell.mjs', Buffer.from(shell));
replacements.set('th14.html', Buffer.from(html));

const changedFonts = new Map();
const resources = JSON.parse(await readTemplate('resources.json'));
for (const resource of resources.resources) {
  const name = resource.url?.slice(2);
  if (!/^fonts\/font[0-7]\.bin$/.test(name) || !names.includes(name) || templateManifest.files[name].bytes <= maxBytes) continue;
  if (resource.url !== './' + name || resource.path !== '/' + name) throw Error('Invalid TH14 font descriptor: ' + name);
  const bytes = await readTemplate(name);
  const compressed = gzipSync(bytes, { level: 6 });
  if (compressed.length > maxBytes) throw Error('TH14 compressed font exceeds file size limit: ' + name);
  changedFonts.set(name, name + '.gz');
  replacements.set(name + '.gz', compressed);
  Object.assign(resource, identity(bytes), {url: './' + name + '.gz', encoding: 'gzip'});
}
if (changedFonts.size) replacements.set('resources.json', Buffer.from(JSON.stringify(resources, null, 2) + '\n'));

for (const name of names) await readTemplate(name);
const outputNames = [...new Set([...names.map(name => changedFonts.get(name) ?? name), ...replacements.keys()])];
for (const name of outputNames) {
  const size = replacements.get(name)?.length ?? templateManifest.files[name]?.bytes;
  if (!(size <= maxBytes)) throw Error('TH14 runtime exceeds file size limit: ' + name);
}
await mkdir(output);
const files = {};
for (const name of outputNames) {
  const bytes = replacements.get(name) ?? await readTemplate(name);
  const target = resolve(output, name);
  await mkdir(dirname(target), { recursive: true });
  await writeFile(target, bytes, { flag: 'wx' });
  files[name] = identity(bytes);
}
runtimeFileNames('th14', files);
await writeFile(resolve(output, 'runtime-files.json'), JSON.stringify({
  schema: templateManifest.schema, game: 'th14', files,
}, null, 2) + '\n', { flag: 'wx' });
console.log(JSON.stringify({ game: 'th14', version, files: outputNames.length, wasm: files['th14-sdl.wasm'] }));
