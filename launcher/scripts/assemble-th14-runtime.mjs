import { createHash } from 'node:crypto';
import { mkdir, readFile, writeFile } from 'node:fs/promises';
import { dirname, resolve } from 'node:path';
import { runtimeFileNames } from '../lib/runtime-release.mjs';
import { assertRuntimeDataShell } from '../lib/runtime-data-provider.mjs';
import { assertRuntimeProtocolSources } from '../lib/runtime-protocol-shell.mjs';

const args = Object.create(null);
for (const argument of process.argv.slice(2)) {
  const match = /^--(template|core-build|output)=(.+)$/.exec(argument);
  if (!match || args[match[1]]) throw Error('Use --template=DIR --core-build=DIR --output=NEW_DIRECTORY');
  args[match[1]] = resolve(match[2]);
}
const template = args.template, core = args['core-build'], output = args.output;
if (!template || !core || !output || new Set([template, core, output]).size !== 3)
  throw Error('Supply distinct template, core build and output directories');

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

const replacements = new Map();
for (const [name, expected] of Object.entries(build.runtimeFiles)) {
  if (!names.includes(name) || !/^(?:th14-sdl\.(?:mjs|wasm)|Th14[A-Za-z0-9]+\.mjs)$/.test(name))
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
manifest.execution.sha256 = build.sha256;
manifest.execution.loaderSha256 = build.loaderSha256;
replacements.set('manifest.json', Buffer.from(JSON.stringify(manifest, null, 2) + '\n'));

const originalShell = (await readTemplate('shell.mjs')).toString('utf8');
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
assertRuntimeProtocolSources([shell, (await readTemplate('eagler-host.mjs')).toString('utf8'),
  (await readTemplate('runtime-epoch.mjs')).toString('utf8')], 'th14', 'normal');
replacements.set('shell.mjs', Buffer.from(shell));
replacements.set('th14.html', Buffer.from(html));

for (const name of names) await readTemplate(name);
await mkdir(output);
const files = {};
for (const name of names) {
  const original = await readTemplate(name);
  const bytes = replacements.get(name) ?? original;
  const target = resolve(output, name);
  await mkdir(dirname(target), { recursive: true });
  await writeFile(target, bytes, { flag: 'wx' });
  files[name] = identity(bytes);
}
await writeFile(resolve(output, 'runtime-files.json'), JSON.stringify({
  schema: templateManifest.schema, game: 'th14', files,
}, null, 2) + '\n', { flag: 'wx' });
console.log(JSON.stringify({ game: 'th14', version, files: names.length, wasm: files['th14-sdl.wasm'] }));
