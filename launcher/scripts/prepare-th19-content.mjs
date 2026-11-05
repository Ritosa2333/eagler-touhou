import {copyFile, mkdir, readFile, stat, writeFile} from 'node:fs/promises';
import {createHash} from 'node:crypto';
import {dirname, resolve} from 'node:path';
import {PRODUCT_CONTENT} from '../lib/content-definition.mjs';

const options = new Map();
for (const argument of process.argv.slice(2)) {
  const match = /^--(original|output)=(.+)$/.exec(argument);
  if (!match || options.has(match[1])) throw new Error(`Invalid argument: ${argument}`);
  options.set(match[1], resolve(match[2]));
}
if (!options.has('original') || !options.has('output')) {
  throw new Error('Usage: node scripts/prepare-th19-content.mjs --original=DIR --output=DIR');
}

const original = options.get('original');
const output = options.get('output');
const names = ['th19.dat', ...PRODUCT_CONTENT.th19.music.ogg.files.map(name => `music/${name}`)];
// Verify the complete input set before creating the prepared content directory.
for (const name of names) {
  const source = resolve(original, name);
  const info = await stat(source);
  if (!info.isFile() || info.size === 0) throw new Error(`Missing TH19 resource: ${name}`);
}
const files = {};
for (const name of names) {
  const source = resolve(original, name);
  const bytes = await readFile(source);
  if (name.endsWith('.ogg') && bytes.subarray(0, 4).toString() !== 'OggS') {
    throw new Error(`Invalid OGG resource: ${name}`);
  }
  const outputName = name === 'th19.dat' ? 'th19.data' : name;
  const target = resolve(output, outputName);
  await mkdir(dirname(target), {recursive: true});
  if (source !== target) await copyFile(source, target);
  files[outputName] = {bytes: bytes.length, sha256: createHash('sha256').update(bytes).digest('hex')};
}
await writeFile(resolve(output, 'provenance.json'), JSON.stringify({
  schema: 'eagler-touhou/prepared-content/1', game: 'th19', files,
}, null, 2) + '\n');
console.log(JSON.stringify({game: 'th19', output, files: names.length}));
