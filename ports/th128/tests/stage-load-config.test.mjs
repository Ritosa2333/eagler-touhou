import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {createHash} from 'node:crypto';
import {fileURLToPath} from 'node:url';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const exeSha256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4';
const tablePath = path.join(root, 'local/research/stage-table.json');
const tableBytes = fs.readFileSync(tablePath);
const table = JSON.parse(tableBytes.toString('utf8'));
const tableSha256 = createHash('sha256').update(tableBytes).digest('hex');
const catalog = JSON.parse(fs.readFileSync(path.join(root, 'local/catalog.json'), 'utf8'));

function stageCatalog(variant) {
  const manifest = path.resolve(root, 'local', catalog.variants[variant].manifest);
  const file = path.join(path.dirname(manifest), 'animations/stage-catalog.json');
  return JSON.parse(fs.readFileSync(file, 'utf8'));
}

function originalRecord(stage) {
  return {
    id: stage.id,
    background: stage.background,
    ecl: stage.ecl,
    message: stage.message,
    logo: stage.logo,
    music: [...stage.music],
  };
}

test('published stage catalog matches the fixed stage table for both variants', () => {
  const expected = table.stages.slice().sort((left, right) => left.id - right.id).map(originalRecord);
  assert.equal(table.schema, 'th128-original-stage-table/1');
  assert.equal(table.exeSha256, exeSha256);
  assert.equal(expected.length, 16);
  assert.deepEqual(expected.map(stage => stage.id), Array.from({length: 16}, (_, index) => index + 1));
  for (const variant of ['ja', 'zh']) {
    const published = stageCatalog(variant);
    assert.equal(published.schema, 'th128-stage-catalog/1');
    assert.equal(published.sourceExeSha256, exeSha256);
    assert.equal(published.sourceTableSha256, tableSha256);
    assert.deepEqual(published.stages, expected);
    assert.equal(published.stages[1].background, 'stage_b2.std');
    assert.deepEqual(published.stages[1].music, ['th128_01.wav', 'th128_02.wav']);
    assert.equal(published.stages[3].background, 'stage_c2.std');
    assert.deepEqual(published.stages[3].music, ['th128_01.wav', 'th128_02.wav']);
  }
  assert.deepEqual(stageCatalog('ja'), stageCatalog('zh'));
});
