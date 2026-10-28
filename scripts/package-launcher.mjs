// Assemble a local TH12 site with the upstream Launcher and Package Store.
// Retail content stays in ignored output directories; this does not run a game
// or publish anything to a remote server.
import {spawnSync} from 'node:child_process';
import fs from 'node:fs';
import path from 'node:path';

const root = path.resolve(import.meta.dirname, '..');
const args = new Map(process.argv.slice(2).map(argument => {
  const split = argument.indexOf('=');
  if (!argument.startsWith('--') || split < 3) throw Error('Use --name=value arguments');
  return [argument.slice(2, split), argument.slice(split + 1)];
}));
const allowed = new Set(['workspace', 'content', 'language-packs', 'output', 'site-url', 'runtime', 'multiplayer-runtime', 'font', 'vanilla-font', 'build-launcher']);
for (const name of args.keys()) if (!allowed.has(name)) throw Error('Unknown option: --' + name);
for (const name of ['workspace', 'content']) if (!args.get(name)) throw Error('Missing --' + name + '=PATH');
const workspace = path.resolve(args.get('workspace'));
const launcher = path.join(workspace, 'launcher');
const content = path.resolve(args.get('content'));
const runtime = path.resolve(args.get('runtime') || path.join(root, 'build-eagler'));
const multiplayerRuntime = path.resolve(args.get('multiplayer-runtime') || path.join(root, '../th12-mp/build-eagler-multiplayer'));
const output = path.resolve(args.get('output') || path.join(root, 'artifacts/launcher-site'));
const siteUrl = new URL(args.get('site-url') || 'http://127.0.0.1:8130/');
if (siteUrl.username || siteUrl.password || siteUrl.pathname !== '/' || siteUrl.search || siteUrl.hash ||
    !['http:', 'https:'].includes(siteUrl.protocol)) throw Error('Supply an origin with a root mount for --site-url');
const buildLauncher = args.get('build-launcher') ?? '1';
if (!['0', '1'].includes(buildLauncher)) throw Error('--build-launcher must be 0 or 1');
const inside = (parent, child) => {
  const relative = path.relative(parent, child);
  return relative !== '' && relative !== '..' && !relative.startsWith('..' + path.sep) && !path.isAbsolute(relative);
};
if (![path.join(root, 'artifacts'), path.join(launcher, 'artifacts'), path.join(workspace, 'artifacts')].some(parent => inside(parent, output))) {
  throw Error('Site output must be below an artifacts directory in this source or the Launcher workspace');
}
for (const source of [root, workspace, content, runtime]) {
  if (output === source || inside(output, source)) throw Error('Output would replace a source directory');
}
for (const file of ['package.json', 'scripts/package-server.mjs', 'scripts/serve-static.mjs']) {
  if (!fs.existsSync(path.join(launcher, file))) throw Error('Missing upstream Launcher: ' + file);
}
const portManifest = JSON.parse(fs.readFileSync(path.join(runtime, 'manifest.json'), 'utf8'));
if (portManifest.game !== 'th12' || portManifest.protocol !== 'eagler-touhou/1' || portManifest.features?.thprac !== true) {
  throw Error('Build the complete TH12 Runtime with npm run build:eagler first');
}
const work = path.join(root, 'artifacts/launcher-inputs');
fs.mkdirSync(work, {recursive: true});
let languageSource = path.resolve(args.get('language-packs') || content);
let languageCatalog;
if (fs.existsSync(path.join(languageSource, 'catalog.json'))) {
  languageCatalog = JSON.parse(fs.readFileSync(path.join(languageSource, 'catalog.json'), 'utf8'));
} else if (fs.existsSync(path.join(languageSource, 'language/catalog.json'))) {
  languageCatalog = JSON.parse(fs.readFileSync(path.join(languageSource, 'language/catalog.json'), 'utf8'));
}
if (args.has('language-packs') && !languageCatalog) throw Error('Language pack directory has no catalog');
if (languageCatalog && (languageCatalog.schema !== 'eagler-touhou/thcrap-static-catalog/1' || languageCatalog.game !== 'th12')) {
  throw Error('Unsupported Host language catalog');
}
const languages = ['ja', ...(languageCatalog?.languages ?? []).map(entry => entry.id)];
if (languages.some(language => language !== 'ja' && !/^lang_[a-z0-9-]+$/i.test(language)) || new Set(languages).size !== languages.length) {
  throw Error('Unsupported or duplicated TH12 language selection');
}
const features = path.join(work, 'features.json');
fs.writeFileSync(features, JSON.stringify({schema: 'eagler-touhou/server-features/1', resourceMode: 'hosted',
  games: {th12: {languages, thprac: true}}}, null, 2) + '\n');

// sourceIdentity reads only repositories supplied by this invocation. Scope
// ownership exceptions to this child process instead of changing global Git.
const environment = {...process.env};
const count = Number(environment.GIT_CONFIG_COUNT || 0);
if (!Number.isSafeInteger(count) || count < 0) throw Error('Invalid GIT_CONFIG_COUNT');
environment.GIT_CONFIG_COUNT = String(count + 1);
environment['GIT_CONFIG_KEY_' + count] = 'safe.directory';
environment['GIT_CONFIG_VALUE_' + count] = workspace.replaceAll('\\', '/');
function run(command, arguments_) {
  const result = spawnSync(command, arguments_, {cwd: launcher, env: environment, stdio: 'inherit', windowsHide: true});
  if (result.error) throw result.error;
  if (result.status !== 0) process.exit(result.status || 1);
}
if (buildLauncher === '1') run(process.execPath, [path.join(launcher, 'scripts/build-launcher.mjs'), '--force']);
run(process.execPath, [path.join(launcher, 'scripts/package-server.mjs'), '--games=th12', '--profile=web-validation-th12',
  '--test-build=1', '--output=' + output, '--site-url=' + siteUrl.href,
  '--th12-build=' + runtime, '--th12-multiplayer-build=' + multiplayerRuntime, '--th12-assets=' + content, '--th12-ogg=' + content,
  '--music=ogg', '--feature-config=' + features, '--artwork-dir=' + path.join(content, 'artwork'),
  '--font=' + path.resolve(args.get('font') || path.join(content, 'shared/unifont.otf')),
  '--vanilla-font=' + path.resolve(args.get('vanilla-font') || path.join(content, 'shared/msgothic.ttc')),
  ...(languageCatalog ? ['--th12-language-packs=' + languageSource] : []),
]);
console.log(JSON.stringify({site: output, url: siteUrl.href, languages, profile: 'web-validation-th12',
  serve: [process.execPath, path.join(launcher, 'scripts/serve-static.mjs'), output, siteUrl.port || (siteUrl.protocol === 'https:' ? '443' : '80')]}));
