// Optional audit of locally prepared TH09 packs; no retail bytes are committed.
import assert from 'node:assert/strict';
import { readFile } from 'node:fs/promises';
import { resolve } from 'node:path';
import { unzipSync } from 'fflate';

const root = resolve(process.argv[2] || 'prepared/th09-language');
for (const language of ['lang_zh-hans', 'lang_en']) {
  const archive = unzipSync(new Uint8Array(await readFile(resolve(root, 'language', `${language}.zip`))));
  const manifest = JSON.parse(new TextDecoder().decode(archive['manifest.json']));
  assert.equal(manifest.game, 'th09');
  assert.equal(manifest.language, language);
  assert.equal(manifest.files.length, Object.keys(archive).length - 1);
  const table = Buffer.from(archive['thcrap/th09/localization/strings.etl']);
  assert.equal(table.toString('ascii', 0, 4), 'EST1');
  let offset = 8;
  const translated = [];
  for (let index = 0; index < table.readUInt32LE(4); index++) {
    const idLength = table.readUInt16LE(offset);
    const textLength = table.readUInt16LE(offset + 2);
    const flags = table.readUInt16LE(offset + 4);
    offset += 8;
    const id = table.toString('utf8', offset, offset + idLength);
    offset += idLength;
    const text = table.toString('utf8', offset, offset + textLength);
    offset += textLength;
    if (flags & 1) translated.push({ id, text });
  }
  assert.equal(offset, table.length);
  assert.ok(archive['thcrap/th09/pl00.msg']);
  assert.ok(archive['thcrap/th09/data/title/sl_text.png']);
  console.log(JSON.stringify({ language, files: manifest.files.length, translatedStrings: translated.length,
    translatedIds: translated.map(entry => entry.id) }));
}
