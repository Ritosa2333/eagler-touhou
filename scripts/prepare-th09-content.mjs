// Package TH09's retail archive and the independently prepared 19 OGG tracks.
// Run th09/th09_web/scripts/prepare-assets.py with the original game first, or
// place the resulting music directory under the original game's assets-ogg/.
import { copyFile, mkdir, open, stat } from 'node:fs/promises';
import { resolve } from 'node:path';
import { PRODUCT_CONTENT } from '../lib/content-definition.mjs';

const args = Object.fromEntries(process.argv.slice(2).map(argument => {
  const separator = argument.indexOf('=');
  if (!argument.startsWith('--') || separator < 3) throw Error(`Invalid argument: ${argument}`);
  return [argument.slice(2, separator), argument.slice(separator + 1)];
}));
if (!args.original || !args.output) throw Error('Usage: prepare-th09-content --original=DIR --output=DIR');
const original = resolve(args.original), output = resolve(args.output);
const archive = resolve(original, 'th09.dat');
const archiveInfo = await stat(archive);
if (!archiveInfo.isFile() || archiveInfo.size < 1_000_000) throw Error('TH09 retail archive is missing or too small');
const handle = await open(archive, 'r');
let header;
try { header = Buffer.alloc(4); await handle.read(header, 0, 4, 0); }
finally { await handle.close(); }
if (header.toString('ascii') !== 'PBGZ') throw Error('TH09 retail archive is not PBGZ');
const musicRoots = [
  resolve(original, 'assets-ogg/music'),
  resolve(original, 'music'),
  resolve(import.meta.dirname, '../../th09/th09_web/assets/sdl-native/music'),
];
const names = PRODUCT_CONTENT.th09.music.ogg.files;
let musicRoot = null;
for (const candidate of musicRoots) {
  try {
    await Promise.all(names.map(name => stat(resolve(candidate, name)).then(info => {
      if (!info.isFile() || info.size < 1024) throw Error(name);
    })));
    musicRoot = candidate;
    break;
  } catch { /* try next prepared music source */ }
}
if (!musicRoot) throw Error('TH09 OGG set is missing; prepare all 19 tracks before Host assembly');
await mkdir(resolve(output, 'music'), { recursive: true });
await copyFile(archive, resolve(output, 'th09.data'));
for (const name of names) await copyFile(resolve(musicRoot, name), resolve(output, 'music', name));
console.log(JSON.stringify({ output, musicRoot, tracks: names.length, dataBytes: archiveInfo.size }));
