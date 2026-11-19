import {createHash} from 'node:crypto';
import {mkdir, writeFile} from 'node:fs/promises';
import {dirname, resolve} from 'node:path';
import {gzipSync, gunzipSync} from 'node:zlib';

const DISKS = Object.freeze({
  th02: ['th02-local-cn.hdi.gz', 'th02-local-jp.hdi.gz', 'th02-solo-cn.hdi.gz', 'th02-solo-jp.hdi.gz'],
  th03: ['disks/3-cn.hdi.gz', 'disks/3-jp.hdi.gz'],
  th04: ['th04-coop-jp.hdi.gz', 'th04-coop.hdi.gz', 'th04-solo-jp.hdi.gz', 'th04-solo.hdi.gz'],
});

// Publish each disk with its own identity while the existing DATA consumer is
// retained. Paths are a closed list, never taken unchecked from a manifest.
export async function exportPC98Disks({game, output, entries, payload}) {
  const expected = DISKS[game];
  if (!expected || entries.length !== payload.length) throw Error('Invalid PC-98 disk export');
  const files = [];
  const blocks = new Map();
  for (const path of expected) {
    const matches = entries.map((entry, index) => ({entry, index})).filter(item => item.entry.path === path);
    if (matches.length !== 1) throw Error(`Missing or duplicate PC-98 disk: ${path}`);
    const {entry, index} = matches[0], bytes = payload[index];
    const sha256 = createHash('sha256').update(bytes).digest('hex');
    if (bytes.length !== entry.bytes || sha256 !== entry.sha256) throw Error(`PC-98 disk identity mismatch: ${path}`);
    const target = resolve(output, 'disks', path);
    await mkdir(dirname(target), {recursive: true});
    await writeFile(target, bytes);
    const raw = gunzipSync(bytes), parts = [];
    for (let offset = 0; offset < raw.length; offset += 256 * 1024) {
      const part = raw.subarray(offset, Math.min(offset + 256 * 1024, raw.length));
      const hash = createHash('sha256').update(part).digest('hex');
      if (!blocks.has(hash)) {
        const packed = gzipSync(part, {level: 9});
        const packedHash = createHash('sha256').update(packed).digest('hex');
        const name = `disk-blocks/${hash}.gz`;
        await mkdir(resolve(output, 'disk-blocks'), {recursive: true});
        await writeFile(resolve(output, name), packed);
        blocks.set(hash, {file: name, bytes: part.length, sha256: hash, packedBytes: packed.length, packedSha256: packedHash});
      }
      parts.push(hash);
    }
    files.push({path, file: `disks/${path}`, bytes: bytes.length, sha256,
      imageBytes: raw.length, imageSha256: createHash('sha256').update(raw).digest('hex'), blocks: parts});
  }
  await writeFile(resolve(output, 'disks.json'), JSON.stringify({schema: 'eagler-touhou/pc98-disks/2', game, files, blocks: Object.fromEntries(blocks)}, null, 2) + '\n');
  return files;
}
