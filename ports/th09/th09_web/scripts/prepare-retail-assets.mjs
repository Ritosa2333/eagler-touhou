// Derive the private TH09 web assets from a matching retail 1.50a installation.
// No original bytes are checked in; this script only writes ignored build inputs.
import { createHash } from 'node:crypto';
import { spawnSync } from 'node:child_process';
import { mkdirSync, readFileSync, writeFileSync, copyFileSync, existsSync, statSync } from 'node:fs';
import { resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

const root = fileURLToPath(new URL('../../', import.meta.url));
const game = resolve(root, 'th09_web');
const original = process.argv[2];
if (!original) throw Error('Usage: node scripts/prepare-retail-assets.mjs ORIGINAL_GAME_DIRECTORY');
const source = resolve(original);
const target = JSON.parse(readFileSync(resolve(game, 'target.json')));
const manifest = JSON.parse(readFileSync(resolve(game, 'reference/archive-manifest.json')));
const digest = bytes => createHash('sha256').update(bytes).digest('hex');
const executable = readFileSync(resolve(source, 'th09.exe'));
const archive = readFileSync(resolve(source, 'th09.dat'));
if (digest(executable) !== target.sha256 || digest(archive) !== manifest.sourceSha256)
  throw Error('Original game does not match the recorded Japanese TH09 1.50a source');
if (archive.toString('ascii', 0, 4) !== 'PBGZ') throw Error('Invalid PBGZ header');

function crypt(input, key, step, blockSize, limit) {
  const output = Buffer.from(input);
  const untouched = (input.length % blockSize < blockSize / 4 ? input.length % blockSize : 0) + (input.length & 1);
  const remaining = input.length - untouched;
  let cursor = 0;
  while (cursor < remaining && limit > 0) {
    const block = Math.min(blockSize, remaining - cursor);
    let linear = 0;
    for (let parity = 1; parity <= 2; ++parity)
      for (let pos = block - parity; pos >= 0; pos -= 2) {
        output[cursor + pos] = input[cursor + linear] ^ key;
        key = (key + step) & 255;
        ++linear;
      }
    cursor += block;
    limit -= block;
  }
  return output;
}
function decode(input, capacity) {
  const output = Buffer.alloc(capacity), dictionary = Buffer.alloc(8192);
  let cursor = 0, head = 1, written = 0, mask = 128, byte = 0;
  function bit() {
    if (mask === 128) byte = cursor < input.length ? input[cursor++] : 0;
    const value = +(!!(byte & mask));
    mask >>= 1;
    if (!mask) mask = 128;
    return value;
  }
  function bits(count) { let value = 0; while (count--) value = (value << 1) | bit(); return value; }
  function put(value) {
    if (written >= capacity) throw Error('PBGZ decoded data exceeds expected size');
    output[written++] = value;
    dictionary[head] = value;
    head = (head + 1) & 8191;
  }
  for (;;) {
    if (bit()) put(bits(8));
    else {
      const offset = bits(13);
      if (!offset) return output.subarray(0, written);
      const length = bits(4) + 3;
      for (let n = 0; n < length; ++n) put(dictionary[(offset + n) & 8191]);
    }
  }
}
const header = crypt(archive.subarray(4, 16), 0x1b, 0x37, 12, 0x400);
const count = header.readUInt32LE(0) - 123456;
const offset = header.readUInt32LE(4) - 345678;
const unpacked = header.readUInt32LE(8) - 567891;
if (count !== manifest.entries || offset < 16 || offset >= archive.length || unpacked > 64 * 1024 * 1024)
  throw Error('Unexpected PBGZ index');
const table = decode(crypt(archive.subarray(offset), 0x3e, 0x9b, 0x80, 0x400), unpacked);
let cursor = 0, formatEntry;
for (let i = 0; i < count; ++i) {
  const end = table.indexOf(0, cursor);
  if (end < 0 || end + 13 > table.length) throw Error('Truncated PBGZ index');
  const name = table.toString('ascii', cursor, end);
  cursor = end + 1;
  const start = table.readUInt32LE(cursor), size = table.readUInt32LE(cursor + 4);
  cursor += 12;
  if (formatEntry && name.toLowerCase() !== 'thbgm.fmt' && formatEntry.compressed == null)
    formatEntry.compressed = start - formatEntry.start;
  if (name.toLowerCase() === 'thbgm.fmt') formatEntry = { start, size };
}
if (!formatEntry) throw Error('thbgm.fmt not found in PBGZ');
if (formatEntry.compressed == null) formatEntry.compressed = offset - formatEntry.start;
let format = decode(archive.subarray(formatEntry.start, formatEntry.start + formatEntry.compressed), formatEntry.size);
if (format.toString('ascii', 0, 3) === 'edz') {
  const tags = [0x4d, 0x54, 0x41, 0x4a, 0x45, 0x57, 0x2d, 0x2a];
  const params = [[0x1b, 0x37, 0x40, 0x2800], [0x51, 0xe9, 0x40, 0x3000], [0xc1, 0x51, 0x400, 0x400], [0x03, 0x19, 0x400, 0x400], [0xab, 0xcd, 0x200, 0x1000], [0x12, 0x34, 0x400, 0x400], [0x35, 0x97, 0x80, 0x2800], [0x99, 0x37, 0x400, 0x1000]];
  const kind = tags.indexOf(format[3]);
  if (kind < 0) throw Error('Unknown TH09 resource encryption');
  format = crypt(format.subarray(4), ...params[kind]);
}
const expected = manifest.resources.find(file => file.name === 'thbgm.fmt');
if (format.length !== expected.bytes || digest(format) !== expected.sha256)
  throw Error('TH09 music layout failed reference checksum');
const assetDir = resolve(game, 'reference/assets');
mkdirSync(assetDir, { recursive: true });
writeFileSync(resolve(assetDir, 'thbgm.fmt'), format);

// TH10's first codepage is the same 65,536-entry CP932-to-Unicode table.
const shared = resolve(root, '../th10/th10_web/assets/sdl-native');
const native = resolve(game, 'assets/sdl-native');
mkdirSync(native, { recursive: true });
const codepages = readFileSync(resolve(shared, 'codepages.bin'));
if (codepages.length !== 262144) throw Error('Unexpected shared codepage table');
writeFileSync(resolve(native, 'cp932.bin'), codepages.subarray(0, 131072));
copyFileSync(resolve(shared, 'blend.bin'), resolve(native, 'blend.bin'));
copyFileSync(resolve(shared, 'msgothic.ttc'), resolve(native, 'msgothic.ttc'));

const bgm = readFileSync(resolve(source, 'thbgm.dat'));
if (bgm.toString('ascii', 0, 4) !== 'ZWAV') throw Error('Invalid TH09 BGM container');
const musicDir = resolve(native, 'music');
mkdirSync(musicDir, { recursive: true });
const report = [];
for (let at = 0; at + 52 <= format.length && format[at]; at += 52) {
  const name = format.toString('ascii', at, at + 16).split('\0')[0];
  const stem = name.replace(/\.wav$/i, '');
  const start = format.readUInt32LE(at + 16), intro = format.readUInt32LE(at + 24), length = format.readUInt32LE(at + 28);
  if (!/^th(?:07|08|09)_[a-z0-9_]+\.wav$/.test(name) || start + length > bgm.length || length % 4 || intro >= length || intro % 4)
    throw Error(`Invalid PCM layout for ${name}`);
  const pcm = bgm.subarray(start, start + length);
  const reference = JSON.parse(readFileSync(resolve(native, 'music-verification.json'))).find(item => item.name === name);
  if (!reference || reference.frames !== length / 4 || reference.loopFrame !== intro / 4 || reference.pcmSha256 !== digest(pcm))
    throw Error(`PCM mismatch for ${name}`);
  const filename = resolve(musicDir, `${stem}.ogg`);
  if (!existsSync(filename) || statSync(filename).size < 1024) {
    const encoded = spawnSync('ffmpeg', ['-hide_banner', '-loglevel', 'error', '-y', '-f', 's16le', '-ar', '44100', '-ac', '2', '-i', 'pipe:0', '-c:a', 'libvorbis', '-q:a', '4', filename], { input: pcm, maxBuffer: 2 * 1024 * 1024 });
    if (encoded.error || encoded.status !== 0) throw Error(`ffmpeg failed for ${name}: ${encoded.error || encoded.stderr}`);
  }
  const inspected = spawnSync('ffprobe', ['-v', 'error', '-select_streams', 'a:0', '-show_entries', 'stream=sample_rate,duration_ts', '-of', 'json', filename], { encoding: 'utf8' });
  if (inspected.error || inspected.status !== 0) throw Error(`ffprobe failed for ${name}: ${inspected.error || inspected.stderr}`);
  const stream = JSON.parse(inspected.stdout).streams?.[0];
  if (Number(stream?.sample_rate) !== 44100 || Number(stream?.duration_ts) !== length / 4)
    throw Error(`Encoded OGG has wrong PCM frame count for ${name}`);
  const ogg = readFileSync(filename);
  report.push({ name, file: `${stem}.ogg`, frames: length / 4, loopFrame: intro / 4, pcmSha256: digest(pcm), encodedBytes: ogg.length, encodedSha256: digest(ogg), codec: 'vorbis', lossless: false });
  console.log(`${report.length}/19 ${name}`);
}
if (report.length !== 19) throw Error(`Expected 19 tracks, found ${report.length}`);
writeFileSync(resolve(native, 'music-local-verification.json'), JSON.stringify(report, null, 2) + '\n');
console.log(JSON.stringify({ original: source, tracks: report.length, format: resolve(assetDir, 'thbgm.fmt') }));
