import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {mkdtemp, readFile, rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {test} from 'node:test';
import {gzipSync} from 'node:zlib';
import {exportPC98Disks} from './pc98-disks.mjs';
import {restoreDiskBlocks} from '../../launcher/lib/pc98-disk-blocks.mjs';

test('exports verified disks independently and rejects damaged or missing input', async () => {
  const output = await mkdtemp(join(tmpdir(), 'pc98-disks-'));
  try {
    const paths = ['disks/3-cn.hdi.gz', 'disks/3-jp.hdi.gz'];
    const payload = paths.map(path => gzipSync(Buffer.from(path)));
    const entries = paths.map((path, index) => ({path, bytes: payload[index].length,
      sha256: createHash('sha256').update(payload[index]).digest('hex')}));
    const files = await exportPC98Disks({game: 'th03', output, entries, payload});
    assert.equal(files.length, 2);
    assert.deepEqual(await readFile(join(output, files[0].file)), payload[0]);
    const manifest = JSON.parse(await readFile(join(output, 'disks.json'), 'utf8'));
    assert.deepEqual(manifest.files, files);
    const restored = await restoreDiskBlocks(manifest, paths[0], path => readFile(join(output, path)));
    assert.equal(Buffer.from(restored).toString(), paths[0]);
    await assert.rejects(restoreDiskBlocks(manifest, paths[0], () => Buffer.from('damaged')), /corrupt/);
    await assert.rejects(exportPC98Disks({game: 'th03', output, entries: entries.slice(0, 1), payload: payload.slice(0, 1)}), /Missing/);
    await assert.rejects(exportPC98Disks({game: 'th03', output, entries, payload: [Buffer.alloc(payload[0].length), payload[1]]}), /identity mismatch/);
  } finally {
    await rm(output, {recursive: true, force: true});
  }
});
