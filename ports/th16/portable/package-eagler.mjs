// th16 启动器运行时目录打包：web/artifacts/sdl3 的 wasm/loader + web/sdl-runtime 的壳 → build-eagler/
//（eagler-touhou/runtime-directory/1；文件清单与 common/contracts/product-catalog.mjs 的 runtimeAssets 一致）。
// 用法：node portable/package-eagler.mjs（EAGLER_RUNTIME_OUTPUT 可改输出目录）
import {readFileSync, writeFileSync, mkdirSync, existsSync, readdirSync} from 'node:fs';
import {resolve, dirname, relative, sep} from 'node:path';
import {createHash} from 'node:crypto';

const root = resolve(import.meta.dirname, '..');
const game = 'th16';
const out = process.env.EAGLER_RUNTIME_OUTPUT ? resolve(process.env.EAGLER_RUNTIME_OUTPUT) : resolve(root, 'build-eagler');
const buildRoot = resolve(root, 'web/artifacts/sdl3');
const hash = bytes => createHash('sha256').update(bytes).digest('hex');
const build = JSON.parse(readFileSync(resolve(buildRoot, 'build.json'), 'utf8'));
if (!Number.isFinite(Date.parse(build.builtAt))) throw Error('Rebuild Runtime with build timestamp');
for (const [name, expected] of Object.entries(build.sourceFiles)) {
  if (hash(readFileSync(resolve(root, name))) !== expected) throw Error('Rebuild modified source: ' + name);
}
const entry = game + '.html';
// 字体（msgothic.ttc / unifont.otf）由启动器 requiredShared 供给；CP932 表由 shell.mjs 在浏览器内生成——不打包字体
const runtimeNames = ['shell.mjs', 'directory-keyboard.mjs', 'eagler-host.mjs'];
const names = [entry, 'manifest.json', ...runtimeNames, game + '-sdl.mjs', game + '-sdl.wasm', 'resources.json'];
const allowed = new Set([...names, 'runtime-files.json']);
const walk = dir => existsSync(dir) ? readdirSync(dir, {withFileTypes: true}).flatMap(e => e.isDirectory() ? walk(resolve(dir, e.name)) : [resolve(dir, e.name)]) : [];
for (const path of walk(out)) {
  if (!allowed.has(relative(out, path).split(sep).join('/'))) throw Error('Unexpected file in output; select a clean output directory: ' + path);
}
const write = (name, bytes) => { const path = resolve(out, name); mkdirSync(dirname(path), {recursive: true}); writeFileSync(path, bytes); };
const shellRoot = resolve(root, 'web/sdl-runtime');
write(entry, readFileSync(resolve(shellRoot, entry), 'utf8').replace('<head>', '<head><meta name="eagler-data-provider" content="retail-memory">'));
for (const name of runtimeNames) write(name, readFileSync(resolve(shellRoot, name)));
for (const ext of ['mjs', 'wasm']) {
  const bytes = readFileSync(resolve(buildRoot, game + '-sdl.' + ext));
  if (hash(bytes) !== (ext === 'wasm' ? build.sha256 : build.loaderSha256)) throw Error('Build identity mismatch');
  write(game + '-sdl.' + ext, bytes);
}
write('resources.json', JSON.stringify({schema: 'eagler-sdl-resources/1', game, resources: []}, null, 2) + '\n');
const features = {thprac: build.features?.thprac === true, languages: build.features?.languages === true, focusHitbox: build.features?.focusHitbox === true};
write('manifest.json', JSON.stringify({game, protocol: 'eagler-touhou/1', adapter: 'sdl3-eagler', profile: 'production',
  builtAt: build.builtAt, version: build.version, features, music: ['ogg-stream', 'ogg-full', 'none'], touchReplay: false,
  execution: {kind: build.kind, sha256: build.sha256, loaderSha256: build.loaderSha256, architecture: build.architecture}}, null, 2) + '\n');
const files = Object.fromEntries(names.map(name => { const bytes = readFileSync(resolve(out, name)); return [name, {bytes: bytes.length, sha256: hash(bytes)}]; }));
write('runtime-files.json', JSON.stringify({schema: 'eagler-touhou/runtime-directory/1', game, files}, null, 2) + '\n');
console.log(JSON.stringify({game, out, files: names.length, wasm: build.sha256}, null, 2));
