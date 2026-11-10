import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {mkdtemp, readFile, rm, writeFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {gzipSync} from 'node:zlib';
import {test} from 'node:test';
import {exportPC98Disks} from '../../tools/build/pc98-disks.mjs';
import {publishPC98Blocks} from '../lib/pc98-static-publication.mjs';

test('publishes independently addressed blocks only when disks match DATA', async () => {
  const root = await mkdtemp(join(tmpdir(), 'pc98-publish-'));
  try {
    const paths = ['disks/3-cn.hdi.gz', 'disks/3-jp.hdi.gz', 'common.bin'];
    const payload = paths.map(path => gzipSync(Buffer.from(path)));
    let offset = 0;
    const entries = paths.map((path, index) => {
      const bytes = payload[index], item = {path, offset, bytes: bytes.length, sha256: createHash('sha256').update(bytes).digest('hex')};
      offset += bytes.length; return item;
    });
    const manifest = Buffer.from(JSON.stringify({game: 'th03', files: entries})), header = Buffer.alloc(12);
    header.write('EPC98PK1'); header.writeUInt32LE(manifest.length, 8);
    const data = join(root, 'th03.data'); await writeFile(data, Buffer.concat([header, manifest, ...payload]));
    await exportPC98Disks({game: 'th03', output: root, entries, payload});
    const files = {}, components = {}, staging = join(root, 'site');
    const descriptorFile = async (source, target) => ({source, target, bytes: (await readFile(join(staging, source))).length, revision: 'test'});
    assert.equal(await publishPC98Blocks({game: 'th03', source: root, data, staging, descriptorFile, packageFiles: files, components}), true);
    assert.equal(components['pc98-disk:disks:3-cn.hdi.gz'].files.length, 1);
    assert.ok(files['pc98-disk-manifest']);
    const split = await readFile(join(staging, 'games/th03/th03.data'));
    const splitIndex = JSON.parse(split.subarray(12, 12 + split.readUInt32LE(8)));
    assert.deepEqual(splitIndex.files.map(file => file.path), ['common.bin']);
    const diskManifest = JSON.parse(await readFile(join(root, 'disks.json'), 'utf8'));
    await writeFile(join(root, diskManifest.blocks[diskManifest.files[0].blocks[0]].file), 'damaged');
    await assert.rejects(publishPC98Blocks({game: 'th03', source: root, data, staging, descriptorFile, packageFiles: {}, components: {}}), /corrupt/);
  } finally { await rm(root, {recursive: true, force: true}); }
});
