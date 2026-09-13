import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {runtimeFileNames} from '../lib/runtime-release.mjs';
import {PRODUCT_GAMES} from '../lib/contracts/product-catalog.mjs';
import {runtimeAppShellPaths} from '../lib/app-shell-policy.mjs';
const directory = resolve(process.argv[2] ?? '../th10-eagler/build-eagler');
const manifest = JSON.parse(await readFile(resolve(directory, 'runtime-files.json'), 'utf8'));
const names = runtimeFileNames('th10', manifest.files);
assert.ok(names.length > 10);
for (const name of names) {
  const bytes = await readFile(resolve(directory, name));
  assert.equal(bytes.length, manifest.files[name].bytes);
  assert.equal(createHash('sha256').update(bytes).digest('hex'), manifest.files[name].sha256);
}
for (const path of ['../outside.js', 'runtime/../../outside.js', 'data/th10.exe', 'native/any.dll', '/runtime/a.js', 'runtime/any.data']) {
  assert.throws(() => runtimeFileNames('th10', {...manifest.files, [path]: {bytes: 1, sha256: '0'.repeat(64)}}));
}
const incomplete = {...manifest.files}; delete incomplete['runtime/native-worker.mjs'];
assert.throws(() => runtimeFileNames('th10', incomplete));
assert.throws(() => runtimeFileNames('th06', {'th06.html': {}, 'th06.js': {}, 'th06.wasm': {}, 'runtime/x.js': {}}));
assert.deepEqual(runtimeFileNames('th06', {'th06.html': {}, 'th06.js': {}, 'th06.wasm': {}}), ['th06.html', 'th06.js', 'th06.wasm']);
assert.deepEqual(PRODUCT_GAMES.th10.requiredShared, []);
const files = runtimeAppShellPaths({games: {th10: {runtime: 'runtime/th10/th10.html?hosted=1&v=test'}}});
for (const path of PRODUCT_GAMES.th10.runtimeAssets) assert.ok(files.includes(`runtime/th10/${path}`));
assert.ok(!files.includes('runtime/th10/th10.js'));
console.log(JSON.stringify({th10DirectoryRuntime: 'PASS', verifiedFiles: names.length, executableContentRejected: true, wasiRuntimeContract: true}));
