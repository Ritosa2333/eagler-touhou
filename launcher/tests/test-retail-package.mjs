import assert from 'node:assert/strict';
import {test} from 'vitest';
import {zipSync} from 'fflate';
import {expandRetailInputs} from '../app/services/retail-input-archive.ts';
import {sha256Hex} from '../package/package-integrity.mjs';
import {createRetailPackage, retailArchiveLimits} from '../src/launcher/retail-package.mjs';

// A small, repacked TH07 DAT whose BGM table refers to replacement PCM.
// Use literal LZSS tokens so the fixture is independent of the decoder.
function literals(data) {
  const bits = Array.from(data, value => `1${value.toString(2).padStart(8, '0')}`).join('');
  return Uint8Array.from(bits.match(/.{1,8}/g), value => parseInt(value.padEnd(8, '0'), 2));
}
function th07(marker = 'th07_0100b.ver') {
  const fmt = new Uint8Array(52), index = new DataView(fmt.buffer);
  fmt.set(new TextEncoder().encode('th07_01.wav'));
  for (const [at, value] of [[16, 16], [24, 4], [28, 8], [32, 0x20001], [36, 44100]]) index.setUint32(at, value, true);
  const packed = literals(fmt), version = literals(Uint8Array.of(1));
  const entries = [['thbgm.fmt', 16, fmt.length], [marker, 16 + packed.length, 1]].flatMap(([name, start, size]) => {
    const label = new TextEncoder().encode(`${name}\0`), fields = new Uint8Array(12), view = new DataView(fields.buffer);
    view.setUint32(0, start, true); view.setUint32(4, size, true);
    return [...label, ...fields];
  });
  const header = new Uint8Array(16), fields = new DataView(header.buffer);
  header.set(new TextEncoder().encode('PBG4'));
  fields.setUint32(4, 2, true);
  fields.setUint32(8, 16 + packed.length + version.length, true);
  fields.setUint32(12, entries.length, true);
  return new File([header, packed, version, literals(entries)], 'th07.dat');
}

test('original DAT and separate BGM installable payload preserves DATA and converts music', async () => {
  const game = 'th07';
  const bytes = 23829135;
  const layout = `sha256-${await sha256Hex(new TextEncoder().encode(JSON.stringify([['/th07.dat', 0, bytes]])))}`;
  const expected = {bytes, layout};
  const pcm = Uint8Array.of(20, 30, 40, 50, 60, 70, 80, 90);
  const header = Uint8Array.of(90, 87, 65, 86, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  const bgm = new File([header, pcm], 'thbgm.dat');
  const archive = th07();
  const font = new File(['OTTO test'], 'unifont.otf');
  const {descriptor, payloads} = await createRetailPackage({game, expected, inputs: [archive, bgm], fontTargets: ['/unifont.otf'],
    loadFont(target) {assert.equal(target, '/unifont.otf'); return font;},
    async encodeOgg(input, options) {
      assert.equal(options.pcm, true);
      assert.deepEqual(new Uint8Array(await input.arrayBuffer()), pcm);
      return new Blob(['OggS synthetic']);
    }});
  assert.equal(descriptor.files['game-data'].bytes, bytes);
  const replacement = Object.keys(descriptor.files).find(id => descriptor.files[id].target === '/th07.dat');
  assert.deepEqual(await payloads.get(replacement).arrayBuffer(), await archive.arrayBuffer());
  assert.equal(descriptor.files[descriptor.components.ogg.files[0]].target, '/bgm-ogg/th07_01.ogg');
  assert.equal(descriptor.components.language, undefined);
  assert.ok(descriptor.base.files.some(id => descriptor.files[id].target === '/unifont.otf'));
  await assert.rejects(createRetailPackage({game, expected, inputs: [archive, new File([header], 'thbgm.dat')]}), /Invalid thbgm\.fmt track/);
  await assert.rejects(createRetailPackage({game, expected, inputs: [archive, new File(['invalid'], 'thbgm.dat')]}), /Invalid thbgm\.dat header/);
});

test('three-digit retail version markers are valid for the selected game', async () => {
  const game = 'th07';
  assert.ok((await createRetailPackage({game, inputs: [th07('th07_100a.ver')]})).descriptor.files['game-data']);
  await assert.rejects(createRetailPackage({game, inputs: [th07('th19_100a.ver')]}), /game version marker does not match TH07/);
});

test('a Deflate ZIP is detected by its header and extracts only selected game resources', async () => {
  const archive = th07();
  const data = new Uint8Array(await archive.arrayBuffer());
  const bgm = new Uint8Array(256).fill(4);
  const zip = new File([zipSync({
    '東方妖々夢/': new Uint8Array(),
    '東方妖々夢/th07.dat': data,
    '東方妖々夢/thbgm.dat': bgm,
    '東方妖々夢/ignored.exe': Uint8Array.of(5),
    'other/th08.dat': Uint8Array.of(6),
  }, {level: 6})], 'misnamed.dat');
  const files = await expandRetailInputs([zip], retailArchiveLimits('th07'), '');
  assert.deepEqual(files.map(file => file.archivePath), ['東方妖々夢/th07.dat', '東方妖々夢/thbgm.dat']);
  assert.deepEqual(new Uint8Array(await files[0].arrayBuffer()), data);
  assert.deepEqual(new Uint8Array(await files[1].arrayBuffer()), bgm);
});
