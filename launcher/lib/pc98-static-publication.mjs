import {createHash} from 'node:crypto';
import {cp, mkdir, readFile, writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {gunzipSync} from 'node:zlib';
import {restoreDiskBlocks} from './pc98-disk-blocks.mjs';

const hash = bytes => createHash('sha256').update(bytes).digest('hex');
const diskNames = Object.freeze({
  th02: ['th02-local-cn.hdi.gz', 'th02-local-jp.hdi.gz', 'th02-solo-cn.hdi.gz', 'th02-solo-jp.hdi.gz'],
  th03: ['disks/3-cn.hdi.gz', 'disks/3-jp.hdi.gz'],
  th04: ['th04-coop-jp.hdi.gz', 'th04-coop.hdi.gz', 'th04-solo-jp.hdi.gz', 'th04-solo.hdi.gz'],
});

// Build Package components only from a verified, matching DATA generation.
export async function publishPC98Blocks({game, source, data, staging, descriptorFile, packageFiles, components}) {
  const expected = diskNames[game];
  if (!expected) return false;
  let manifest;
  try { manifest = JSON.parse(await readFile(resolve(source, 'disks.json'), 'utf8')); }
  catch (error) { if (error.code === 'ENOENT') return false; throw error; }
  if (manifest.game !== game || manifest.schema !== 'eagler-touhou/pc98-disks/2' || !Array.isArray(manifest.files) ||
      manifest.files.length !== expected.length || !manifest.blocks || typeof manifest.blocks !== 'object') throw Error(`${game}: invalid static disk manifest`);
  const dataBytes = await readFile(data), size = dataBytes.readUInt32LE(8), start = 12 + size;
  if (dataBytes.subarray(0, 8).toString('ascii') !== 'EPC98PK1' || start > dataBytes.length) throw Error(`${game}: invalid DATA`);
  const dataManifest = JSON.parse(dataBytes.subarray(12, start));
  if (dataManifest.game !== game || !Array.isArray(dataManifest.files)) throw Error(`${game}: invalid DATA identity`);
  const used = new Set();
  for (const path of expected) {
    const matches = manifest.files.filter(file => file.path === path);
    const entries = dataManifest.files.filter(file => file.path === path);
    if (matches.length !== 1 || entries.length !== 1) throw Error(`${game}: disk list mismatch`);
    const file = matches[0], entry = entries[0];
    if (!Number.isSafeInteger(entry.offset) || !Number.isSafeInteger(entry.bytes) || entry.offset < 0 || entry.bytes <= 0 || start + entry.offset + entry.bytes > dataBytes.length) throw Error(`${game}: invalid disk range`);
    const original = dataBytes.subarray(start + entry.offset, start + entry.offset + entry.bytes);
    if (file.bytes !== entry.bytes || file.sha256 !== entry.sha256 || hash(original) !== entry.sha256) throw Error(`${game}: disk identity mismatch`);
    const restored = await restoreDiskBlocks(manifest, path, name => readFile(resolve(source, name)));
    if (!Buffer.from(restored).equals(gunzipSync(original))) throw Error(`${game}: static disk mismatch`);
    const ids = [...new Set(file.blocks)].map(block => `pc98-block:${block}`);
    components[`pc98-disk:${path.replaceAll('/', ':')}`] = {type: 'pc98-disk', files: ids};
    for (const block of file.blocks) used.add(block);
  }
  const targetRoot = resolve(staging, 'games', game, 'pc98');
  await mkdir(resolve(targetRoot, 'disk-blocks'), {recursive: true});
  await cp(resolve(source, 'disks.json'), resolve(targetRoot, 'disks.json'));
  packageFiles['pc98-disk-manifest'] = await descriptorFile(`games/${game}/pc98/disks.json`, '/pc98/disks.json');
  components['pc98-manifest'] = {type: 'pc98-disk', files: ['pc98-disk-manifest']};
  for (const block of used) {
    const filename = `${block}.gz`;
    await cp(resolve(source, 'disk-blocks', filename), resolve(targetRoot, 'disk-blocks', filename));
    packageFiles[`pc98-block:${block}`] = await descriptorFile(`games/${game}/pc98/disk-blocks/${filename}`, `/pc98/disk-blocks/${filename}`);
  }
  // Keep the legacy source DATA intact; the published copy contains only the
  // common resources. Disk bytes are supplied through the components above.
  const payload = [], entries = []; let offset = 0;
  for (const entry of dataManifest.files) {
    if (expected.includes(entry.path)) continue;
    if (!Number.isSafeInteger(entry.offset) || !Number.isSafeInteger(entry.bytes) || entry.offset < 0 || entry.bytes <= 0 || start + entry.offset + entry.bytes > dataBytes.length) throw Error(`${game}: invalid DATA range`);
    const bytes = dataBytes.subarray(start + entry.offset, start + entry.offset + entry.bytes);
    if (hash(bytes) !== entry.sha256) throw Error(`${game}: DATA identity mismatch`);
    entries.push({...entry, offset}); payload.push(bytes); offset += bytes.length;
  }
  if (!entries.length) throw Error(`${game}: empty common DATA`);
  const index = Buffer.from(JSON.stringify({...dataManifest, files: entries})), header = Buffer.alloc(12);
  header.write('EPC98PK1'); header.writeUInt32LE(index.length, 8);
  await writeFile(resolve(staging, 'games', game, `${game}.data`), Buffer.concat([header, index, ...payload]));
  return true;
}
