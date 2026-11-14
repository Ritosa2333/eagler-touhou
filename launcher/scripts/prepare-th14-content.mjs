import {copyFile, mkdir, readFile, rename, writeFile} from 'node:fs/promises';
import {createHash, randomUUID} from 'node:crypto';
import {resolve, join} from 'node:path';
import {spawnSync} from 'node:child_process';
import {TH14_MUSIC_LAYOUT} from '../lib/th14-content-layout.mjs';
import {pinOggSerial} from '../lib/ogg-production-baseline.mjs';

const args = Object.fromEntries(process.argv.slice(2).map(argument => {
  const match = /^--(original|output)=(.+)$/.exec(argument);
  if (!match) throw Error('Use --original=DIR --output=DIR');
  return [match[1], resolve(match[2])];
}));
if (!args.original || !args.output) throw Error('Use --original=DIR --output=DIR');
const digest = bytes => createHash('sha256').update(bytes).digest('hex');
const archivePath = join(args.original, 'th14.dat');
const musicPath = join(args.original, 'thbgm.dat');
const archive = await readFile(archivePath);
const music = await readFile(musicPath);
if (digest(archive) !== '57d84a5cbe82460931cca26f090a0fde7aef180889ce5bfe8315851fc97b0233') {
  throw Error('TH14 v1.00b Japanese th14.dat is required');
}
if (digest(music) !== '6fbee538ad86555fa06d1a4d6b25a5fc048cde087b9a35de3e8a158fdbde11b4') {
  throw Error('TH14 v1.00b thbgm.dat does not match the declared PCM layout');
}
await mkdir(join(args.output, 'music'), {recursive: true});
if (archivePath !== join(args.output, 'th14.data')) await copyFile(archivePath, join(args.output, 'th14.data'));
const tracks = [];
for (const row of TH14_MUSIC_LAYOUT) {
  const target = join(args.output, 'music', row.file);
  const end = row.offset + row.frames * 4;
  if (row.offset < 16 || row.offset % 4 || row.loopFrame < 0 ||
      row.loopFrame >= row.frames || end > music.length) throw Error('Invalid PCM layout for ' + row.file);
  const result = spawnSync('ffmpeg', [
    '-hide_banner', '-loglevel', 'error', '-nostdin',
    '-f', 's16le', '-ar', '44100', '-ac', '2', '-i', 'pipe:0',
    '-c:a', 'libvorbis', '-q:a', '5', '-f', 'ogg', 'pipe:1',
  ], {input: music.subarray(row.offset, end), windowsHide: true, maxBuffer: 16 * 1024 * 1024});
  if (result.error || result.status !== 0 || !result.stdout?.length)
    throw result.error || Error('Ogg preparation failed for ' + row.file + ': ' + result.stderr);
  // FFmpeg's default stream serial is random; pin it so re-encoding the
  // same input with the same encoder does not invalidate the package cache.
  const encoded = pinOggSerial(result.stdout, Number.parseInt(digest(row.file).slice(0, 8), 16));
  const temporary = target + '.' + randomUUID() + '.tmp';
  await writeFile(temporary, encoded, {flag: 'wx'});
  await rename(temporary, target);
  tracks.push({...row, bytes: encoded.length, sha256: digest(encoded)});
}
await writeFile(join(args.output, 'provenance.json'), JSON.stringify({
  game: 'th14', archiveSha256: digest(archive), musicSha256: digest(music),
  codec: 'vorbis', encoder: {codec: 'libvorbis', quality: 5}, sampleRate: 44100, channels: 2, tracks,
}, null, 2) + '\n');
