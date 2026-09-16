import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {runtimeFileNames, runtimeStem} from '../lib/runtime-release.mjs';
import {PRODUCT_GAMES} from '../lib/contracts/product-catalog.mjs';
import {runtimeAppShellPaths} from '../lib/app-shell-policy.mjs';
import {workspacePath} from '../lib/workspace-layout.mjs';
import {TH10_MUSIC_LAYOUT, TH10_MUSIC_NAMES} from '../lib/th10-content-layout.mjs';
const verified = {};
const directoryGames = Object.entries(PRODUCT_GAMES)
  .filter(([, product]) => product.runtimeFileLayout === 'directory')
  .map(([game]) => game)
  .sort();
for (const game of directoryGames) {
  const directory = resolve(game === 'th10' && process.argv[2] ? process.argv[2] : workspacePath(game,'build-eagler'));
  const manifest = JSON.parse(await readFile(resolve(directory, 'runtime-files.json'), 'utf8'));
  const names = runtimeFileNames(game, manifest.files);
  assert.equal(names.length, PRODUCT_GAMES[game].runtimeAssets.length);
  for (const name of names) {
    const bytes = await readFile(resolve(directory, name));
    assert.equal(bytes.length, manifest.files[name].bytes);
    assert.equal(createHash('sha256').update(bytes).digest('hex'), manifest.files[name].sha256);
  }
  for (const path of ['../outside.js', 'runtime/../../outside.js', `data/${game}.exe`, 'native/any.dll', '/runtime/a.js', 'runtime/any.data']) {
    assert.throws(() => runtimeFileNames(game, {...manifest.files, [path]: {bytes: 1, sha256: '0'.repeat(64)}}));
  }
  assert.doesNotThrow(() => runtimeFileNames(game, {
    ...manifest.files,
    'adapter-worker.wasm': {bytes: 1, sha256: '0'.repeat(64)},
    'adapter-metadata.json': {bytes: 1, sha256: '0'.repeat(64)},
    'fonts/runtime-extra.woff2': {bytes: 1, sha256: '0'.repeat(64)},
  }));
  const requiredWasm = PRODUCT_GAMES[game].runtimeAssets.find(name => name.endsWith('.wasm'));
  assert.ok(requiredWasm, `${game}: directory Runtime must declare its required Wasm asset`);
  const incomplete = {...manifest.files}; delete incomplete[requiredWasm];
  assert.throws(() => runtimeFileNames(game, incomplete));
  const stem = runtimeStem(game);
  const files = runtimeAppShellPaths({games: {[game]: {runtime: `runtime/${game}/${stem}.html?hosted=1&v=test`}}});
  for (const path of PRODUCT_GAMES[game].runtimeAssets) assert.ok(files.includes(`runtime/${game}/${path}`));
  assert.ok(!files.includes(`runtime/${game}/${stem}.js`));
  verified[game] = names.length;
}
assert.deepEqual(Object.keys(verified).sort(), directoryGames,
  'directory Runtime verification must cover every registered directory adapter');
assert.throws(() => runtimeFileNames('th06', {'th06.html': {}, 'th06.js': {}, 'th06.wasm': {}, 'runtime/x.js': {}}));
assert.deepEqual(runtimeFileNames('th06', {'th06.html': {}, 'th06.js': {}, 'th06.wasm': {}}), ['th06.html', 'th06.js', 'th06.wasm']);
for (const game of directoryGames) {
  assert.ok(Array.isArray(PRODUCT_GAMES[game].requiredShared), `${game}: directory Runtime must declare shared-resource requirements`);
}
const portableTh10Layout = JSON.parse(await readFile(
  workspacePath('th10','th10_web','assets','sdl-native','music-layout.json'),
  'utf8',
));
assert.deepEqual(portableTh10Layout, TH10_MUSIC_LAYOUT.map(row => ({...row})),
  'Launcher-owned TH10 content layout must match the canonical portable Runtime');
assert.deepEqual(TH10_MUSIC_NAMES, ['02','00','01','03','04','05','06','07','08','09','10','11','12','15','16','13','14','17']);
console.log(JSON.stringify({directoryRuntime: 'PASS', verifiedFiles: verified, executableContentRejected: true, sdl3GlesRuntimeContract: true}));
