// Build a user-importable TH09 Package ZIP from privately prepared local data.
// Neither the original DAT nor derived music belongs in this Git repository.
import { createHash } from 'node:crypto';
import { mkdir, readFile, writeFile } from 'node:fs/promises';
import { resolve } from 'node:path';
import { strToU8, zipSync } from 'fflate';
import { PRODUCT_CONTENT } from '../lib/content-definition.mjs';
import { HOST_PROTOCOL, PRODUCT_GAMES } from '../lib/contracts/product-catalog.mjs';
import { canonicalPackagePayload, validatePackageDescriptor } from '../package/package-descriptor.mjs';
import { parsePackageZip } from '../package/package-zip.mjs';

const args = Object.fromEntries(process.argv.slice(2).map(value => {
  const at = value.indexOf('=');
  if (!value.startsWith('--') || at < 3) throw Error(`Invalid argument: ${value}`);
  return [value.slice(2, at), value.slice(at + 1)];
}));
if (!args.content || !args.font || !args.output) {
  throw Error('Usage: node scripts/package-th09-local.mjs --content=DIR --font=FILE --output=FILE.zip');
}

const sha256 = bytes => createHash('sha256').update(bytes).digest('hex');
const files = {}, entries = {}, baseFiles = ['game-data', 'shared-msgothic'], oggFiles = [];
async function add(id, path, source, target) {
  const bytes = await readFile(path);
  if (!bytes.length) throw Error(`Empty resource: ${path}`);
  const hash = sha256(bytes);
  files[id] = { source, target, revision: hash.slice(0, 16), bytes: bytes.length, sha256: hash };
  entries[source] = new Uint8Array(bytes.buffer, bytes.byteOffset, bytes.byteLength);
}
const root = resolve(args.content);
await add('game-data', resolve(root, 'th09.data'), 'games/th09/th09.data', PRODUCT_GAMES.th09.package.dataTarget);
await add('shared-msgothic', resolve(args.font), 'shared/msgothic.ttc', '/msgothic.ttc');
for (const name of PRODUCT_CONTENT.th09.music.ogg.files) {
  const id = `ogg:${name}`;
  await add(id, resolve(root, 'music', name), `games/th09/music/ogg/${name}`, `/music/${name}`);
  oggFiles.push(id);
}
const descriptor = {
  schema: 'eagler-touhou/package/1', game: 'th09', revision: 'pending',
  runtimeRequirement: { protocol: HOST_PROTOCOL, target: 'th09', dataFile: 'game-data', dataLayout: PRODUCT_CONTENT.th09.dataLayout },
  files, base: { files: baseFiles }, components: { ogg: { type: 'ogg', files: oggFiles } },
};
descriptor.revision = sha256(canonicalPackagePayload(descriptor)).slice(0, 16);
validatePackageDescriptor(descriptor);
entries['package.json'] = strToU8(`${JSON.stringify(descriptor, null, 2)}\n`);
const output = resolve(args.output);
const zip = zipSync(entries, { level: 0 });
const parsed = await parsePackageZip(new Blob([zip]));
if (parsed.descriptor.revision !== descriptor.revision || parsed.files.size !== Object.keys(files).length) {
  throw Error('Generated ZIP failed package import validation');
}
await mkdir(resolve(output, '..'), { recursive: true });
await writeFile(output, zip);
console.log(JSON.stringify({ output, bytes: zip.length, sha256: sha256(zip), revision: descriptor.revision, files: Object.keys(files).length }));
