import {mkdir, readFile, readdir, writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {parseContentBundle} from './content-bundle.mjs';
import {SPLIT_SCHEMA} from './split-content.mjs';

const arg = key => process.argv.find(value => value.startsWith('--' + key + '='))?.slice(key.length + 3);
if (!arg('prepared') || !arg('output')) throw Error('Use --prepared=DIR --output=DIR');
const source = resolve(arg('prepared')), output = resolve(arg('output'));
const hash = bytes => createHash('sha256').update(bytes).digest('hex');
const data = await readFile(resolve(source, 'th075.data'));
const bundle = parseContentBundle(data.buffer.slice(data.byteOffset, data.byteOffset + data.byteLength));
const files = [];
await mkdir(output, {recursive: true});
async function put(path, bytes) {
  const sha256 = hash(bytes);
  await writeFile(resolve(output, sha256 + '.bin'), bytes);
  files.push({path, bytes: bytes.length, sha256});
}
for (const path of bundle.paths.sort()) {
  if (/^\/resources\/(?:base|patched)\/data\/(?:character\/[a-z]+\/stage\d+|system\/staffroll)\.dat\.z$/i.test(path)) continue;
  await put(path, await bundle.read(path));
}
const music = (await readdir(resolve(source, 'music'))).filter(name => /^[a-z0-9_]+\.ogg$/.test(name)).sort();
for (const name of music) {
  if (name === 'sys99_ed.ogg') continue;
  await put('/music/' + name, await readFile(resolve(source, 'music', name)));
}
const manifest = Buffer.from(JSON.stringify({schema: SPLIT_SCHEMA, files}) + '\n');
const sha256 = hash(manifest), name = sha256 + '.json';
await writeFile(resolve(output, name), manifest);
console.log(JSON.stringify({manifest: name, sha256, files: files.length}));
