import {readFileSync, writeFileSync, mkdirSync, existsSync, readdirSync} from 'node:fs';
import {resolve, dirname, relative, sep} from 'node:path';
import {createHash} from 'node:crypto';

const root = resolve(import.meta.dirname, '..');
const game = 'th13';
const out = process.env.EAGLER_RUNTIME_OUTPUT ? resolve(process.env.EAGLER_RUNTIME_OUTPUT) : resolve(root, 'build-eagler');
const buildRoot = resolve(root, 'web/artifacts', process.env.TH13_OUTPUT || 'sdl3');
const fonts = process.env.EAGLER_FONT_ROOT;
const hash = b => createHash('sha256').update(b).digest('hex');

const manifestPath = resolve(buildRoot, 'build.json');
if (!existsSync(manifestPath)) throw Error('Build manifest missing at ' + manifestPath + '; compile web runtime first.');
const build = JSON.parse(readFileSync(manifestPath, 'utf8'));
if (!Number.isFinite(Date.parse(build.builtAt))) throw Error('Rebuild Runtime with build timestamp');

for (const [name, expected] of Object.entries(build.sourceFiles || {})) {
  const filePath = resolve(root, name);
  if (!existsSync(filePath) || hash(readFileSync(filePath)) !== expected) {
    throw Error('Rebuild modified source: ' + name);
  }
}

const entry = game + '.html';
const fontNames = fonts && existsSync(resolve(fonts, 'cp932.bin')) ? ['cp932.bin'] : [];
const runtimeNames = [
  'startup-branding.mjs',
  'shell.mjs',
  'managed.css',
  'keyboard.mjs',
  'directory-keyboard.mjs',
  'runtime-assets.mjs',
];
const names = [
  entry,
  'manifest.json',
  ...runtimeNames,
  game + '-sdl.mjs',
  game + '-sdl.wasm',
  'resources.json',
  ...fontNames.map(n => 'fonts/' + n),
];
const allowed = new Set([...names, 'runtime-files.json']);

function walk(dir) {
  return existsSync(dir)
    ? readdirSync(dir, {withFileTypes: true}).flatMap(e => (e.isDirectory() ? walk(resolve(dir, e.name)) : [resolve(dir, e.name)]))
    : [];
}

for (const path of walk(out)) {
  if (!allowed.has(relative(out, path).split(sep).join('/'))) {
    throw Error('Unexpected file in output; select a clean output directory: ' + path);
  }
}

const write = (name, bytes) => {
  const path = resolve(out, name);
  mkdirSync(dirname(path), {recursive: true});
  writeFileSync(path, bytes);
};
const copy = (from, name) => write(name, readFileSync(from));

const shellRoot = resolve(root, 'web/sdl-runtime');
const managedHtmlPath = resolve(shellRoot, 'managed.html');
const rawHtml = existsSync(managedHtmlPath) ? readFileSync(managedHtmlPath, 'utf8') : readFileSync(resolve(shellRoot, 'th13.html'), 'utf8');
const html = rawHtml.includes('eagler-data-provider')
  ? rawHtml
  : rawHtml.replace('<head>', '<head><meta name="eagler-data-provider" content="retail-memory">');
write(entry, html);

for (const name of runtimeNames) copy(resolve(shellRoot, name), name);

for (const ext of ['mjs', 'wasm']) {
  const bytes = readFileSync(resolve(buildRoot, game + '-sdl.' + ext));
  if (build.sha256 && ext === 'wasm' && hash(bytes) !== build.sha256) throw Error('Build identity mismatch for wasm');
  write(game + '-sdl.' + ext, bytes);
}

const resources = fontNames.map(name => {
  const bytes = readFileSync(resolve(fonts, name));
  write('fonts/' + name, bytes);
  return {path: '/fonts/' + name, url: './fonts/' + name, bytes: bytes.length};
});
write('resources.json', JSON.stringify({schema: 'eagler-sdl-resources/1', game, resources}, null, 2) + '\n');

const features = {thprac: false, languages: false, focusHitbox: false};
write(
  'manifest.json',
  JSON.stringify(
    {
      game,
      protocol: 'eagler-touhou/1',
      adapter: 'sdl3-eagler',
      profile: 'integration-in-progress',
      completeAdapter: false,
      builtAt: build.builtAt,
      version: build.version || '0.1.0-sdl3-skeleton',
      features,
      music: ['ogg', 'none'],
      touchReplay: false,
      execution: {
        kind: build.kind,
        sha256: build.sha256,
        loaderSha256: build.loaderSha256,
        architecture: 'eagler-touhou/1',
      },
    },
    null,
    2
  ) + '\n'
);

const files = Object.fromEntries(
  names.map(name => {
    const bytes = readFileSync(resolve(out, name));
    return [name, {bytes: bytes.length, sha256: hash(bytes)}];
  })
);
write('runtime-files.json', JSON.stringify({schema: 'eagler-touhou/runtime-directory/1', game, files}, null, 2) + '\n');
console.log(JSON.stringify({game, out, files: names.length, wasm: build.sha256}, null, 2));
