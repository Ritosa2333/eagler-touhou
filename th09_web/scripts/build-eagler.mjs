// Materialize the resource-free directory Runtime consumed by eagler-touhou.
// Retail DAT, original font and OGG remain Host/Package resources.
import { createHash } from 'node:crypto';
import { copyFileSync, existsSync, mkdirSync, readFileSync, writeFileSync } from 'node:fs';
import { resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

const root = fileURLToPath(new URL('../', import.meta.url));
const compiled = resolve(root, process.env.TH09_OUTPUT || 'artifacts/sdl-release');
const output = resolve(root, 'build-eagler');
const native = resolve(root, 'assets/sdl-native');
const sha256 = bytes => createHash('sha256').update(bytes).digest('hex');
const inputs = [
  ['th09.html', resolve(root, 'sdl-runtime/managed.html')],
  ['shell.mjs', resolve(root, 'sdl-runtime/managed.mjs')],
  ['managed.css', resolve(root, 'sdl-runtime/managed.css')],
  ...['keyboard.mjs', 'netplay.mjs', 'motion-replay.mjs'].map(name => [name, resolve(root, 'sdl-runtime', name)]),
  ['th09.mjs', resolve(compiled, 'th09.mjs')],
  ['th09.wasm', resolve(compiled, 'th09.wasm')],
  ['fonts/cp932.bin', resolve(native, 'cp932.bin')],
  ['fonts/blend.bin', resolve(native, 'blend.bin')],
];
for (const [, source] of inputs) if (!existsSync(source)) throw Error(`Missing TH09 Runtime input: ${source}`);
const build = JSON.parse(readFileSync(resolve(compiled, 'build.json'), 'utf8'));
if (build.kind !== 'th09-native-web-release-candidate' || build.sha256 !== sha256(readFileSync(resolve(compiled, 'th09.wasm')))) {
  throw Error('TH09 release WASM does not match its build attestation');
}
mkdirSync(output, { recursive: true });
for (const [name, source] of inputs) {
  mkdirSync(resolve(output, name, '..'), { recursive: true });
  copyFileSync(source, resolve(output, name));
}
writeFileSync(resolve(output, 'manifest.json'), JSON.stringify({
  game: 'th09', protocol: 'eagler-touhou/1', features: { thprac: false, languages: true, focusHitbox: false },
}, null, 2) + '\n');
// version.json is a generation-local Runtime resource: netplay.mjs fetches it
// relative to the Runtime document so the immutable generation stays closed.
writeFileSync(resolve(output, 'version.json'), JSON.stringify({ game: 'th09', build: build.sha256.slice(0, 24) }) + '\n');
const files = Object.fromEntries([...inputs.map(([name]) => name), 'manifest.json', 'version.json'].map(name => {
  const bytes = readFileSync(resolve(output, name));
  return [name, { bytes: bytes.length, sha256: sha256(bytes) }];
}));
writeFileSync(resolve(output, 'runtime-files.json'), JSON.stringify({ schema: 'eagler-touhou/runtime-directory/1', files }, null, 2) + '\n');
console.log(JSON.stringify({ output, files: Object.keys(files), wasm: files['th09.wasm'].sha256 }));
