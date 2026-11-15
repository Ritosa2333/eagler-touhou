import {test} from 'vitest';
import assert from 'node:assert/strict';
import {mkdtemp, mkdir, writeFile, rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {buildAppShell} from '../lib/app-shell-build.mjs';
import {hostArtworkFiles} from '../lib/host-artwork.mjs';
import {PRODUCT_GAMES} from '../lib/contracts/product-catalog.mjs';

test('explicit frontend graph also caches present Host covers with current byte identities', async () => {
  const directory = await mkdtemp(resolve(tmpdir(), 'eagler-offline-covers-'));
  try {
    await mkdir(resolve(directory, 'assets/pwa'), {recursive: true});
    await writeFile(resolve(directory, 'index.html'), '<html>offline fixture</html>');
    const build = () => buildAppShell({quiet: true, globDirectory: directory,
      shellFiles: ['index.html'], workerContractSource: '// synthetic contract fixture'});
    const bare = await build();
    assert.deepEqual(bare.contract.entries.slice().sort(), ['./', 'index.html']);
    const artwork = hostArtworkFiles(Object.keys(PRODUCT_GAMES)).map(name => `assets/${name}`);
    for (const path of artwork) await writeFile(resolve(directory, path), `synthetic artwork: ${path}`);
    for (const path of ['assets/unowned.webp', 'assets/th09.data']) await writeFile(resolve(directory, path), 'excluded');
    const cached = await build();
    assert.equal(cached.warnings.length, 0);
    for (const path of artwork) {
      assert.equal(cached.manifestEntries.filter(entry => entry.url === path).length, 1, path);
      assert.equal(cached.manifestEntries.find(entry => entry.url === path).revision,
        createHash('sha256').update(`synthetic artwork: ${path}`).digest('hex'));
    }
    assert.equal(cached.contract.entries.includes('assets/unowned.webp'), false);
    assert.equal(cached.contract.entries.includes('assets/th09.data'), false);
    await writeFile(resolve(directory, 'assets/th09-card.webp'), 'replacement cover');
    const updated = await build();
    assert.notEqual(updated.buildId, cached.buildId);
    assert.equal(updated.manifestEntries.find(entry => entry.url === 'assets/th09-card.webp').revision,
      createHash('sha256').update('replacement cover').digest('hex'));
    await writeFile(resolve(directory, 'assets/th09-card.webp'), '');
    await assert.rejects(build(), /Invalid App Shell artwork/);
  } finally { await rm(directory, {recursive: true, force: true}); }
});
