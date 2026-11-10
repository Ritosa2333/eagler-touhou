import assert from 'node:assert/strict';
import {test} from 'vitest';
import {zipSync} from 'fflate';
import {parsePackageZip} from '../package/package-zip.mjs';
import {buildStoredZip} from '../package/stored-zip.mjs';
import {sha256Hex} from '../package/package-integrity.mjs';
import {createRetailPackage} from '../src/launcher/retail-package.mjs';

// A small, repacked TH07 DAT whose BGM table refers to replacement PCM.
// Use literal LZSS tokens so the fixture is independent of the decoder.
function literals(data) {
  const bits = Array.from(data, value => `1${value.toString(2).padStart(8, '0')}`).join('');
  return Uint8Array.from(bits.match(/.{1,8}/g), value => parseInt(value.padEnd(8, '0'), 2));
}
function th07() {
  const fmt = new Uint8Array(52), index = new DataView(fmt.buffer);
  fmt.set(new TextEncoder().encode('th07_01.wav'));
  for (const [at, value] of [[16, 16], [24, 4], [28, 8], [32, 0x20001], [36, 44100]]) index.setUint32(at, value, true);
  const packed = literals(fmt), version = literals(Uint8Array.of(1));
  const entries = [['thbgm.fmt', 16, fmt.length], ['th07_0100b.ver', 16 + packed.length, 1]].flatMap(([name, start, size]) => {
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

test('original ZIP and separate BGM produce an importable package with patched DAT and compiled language files', async () => {
  const game = 'th07';
  const bytes = 23829135;
  const layout = `sha256-${await sha256Hex(new TextEncoder().encode(JSON.stringify([['/th07.dat', 0, bytes]])))}`;
  const expected = {bytes, layout};
  const pcm = Uint8Array.of(20, 30, 40, 50, 60, 70, 80, 90);
  const bgm = new File([new Uint8Array(16), pcm], 'thbgm.dat');
  const archive = th07();
  const inputZip = new File([zipSync({'game/': new Uint8Array(), 'game/th07.dat': new Uint8Array(await archive.arrayBuffer()),
    'game/thcrap/th07/localization/spells.etl': Uint8Array.of(4, 5)})], 'game.zip');
  const languagePath = '/thcrap/th07/localization/musiccmt.etl';
  const manifest = {schema: 'eagler-touhou/thcrap-static-pack/1', game, language: 'lang_zh-hans', runtimeVersion: 'independent', files: [{path: languagePath, bytes: 3}]};
  const patch = new File([await buildStoredZip([['manifest.json', new Blob([JSON.stringify(manifest)])],
    [languagePath.slice(1), new Blob(['abc'])]])], 'lang_zh-hans.zip');
  const font = new File(['OTTO test'], 'unifont.otf');
  const result = await createRetailPackage({game, expected, inputs: [inputZip, bgm, patch, font], fontTargets: ['/unifont.otf'],
    loadFont() {assert.fail('A user-selected font must not request a Host font');},
    async encodeOgg(input, options) {
      assert.equal(options.pcm, true);
      assert.deepEqual(new Uint8Array(await input.arrayBuffer()), pcm);
      return new Blob(['OggS synthetic']);
    }});
  const {descriptor, payloads} = result;
  assert.equal(descriptor.files['game-data'].bytes, bytes);
  const replacement = Object.keys(descriptor.files).find(id => descriptor.files[id].target === '/th07.dat');
  assert.deepEqual(await payloads.get(replacement).arrayBuffer(), await archive.arrayBuffer());
  assert.equal(descriptor.files[descriptor.components.ogg.files[0]].target, '/bgm-ogg/th07_01.ogg');
  assert.deepEqual(descriptor.components.language.entries.map(entry => entry.id), ['lang_zh-hans', 'lang_local']);
  assert.ok(descriptor.base.files.some(id => descriptor.files[id].target === '/unifont.otf'));

  const zip = await buildStoredZip([['package.json', new Blob([JSON.stringify(descriptor)])],
    ...[...payloads].map(([id, blob]) => [descriptor.files[id].source, blob])]);
  const reopened = await parsePackageZip(zip);
  assert.equal(reopened.descriptor.revision, descriptor.revision);
  assert.equal(reopened.files.size, payloads.size);
  assert.deepEqual(await reopened.files.get(replacement).blob.arrayBuffer(), await archive.arrayBuffer());
  await assert.rejects(createRetailPackage({game, expected, inputs: [archive, new File(['short'], 'thbgm.dat')]}), /Invalid thbgm\.fmt track/);
});
