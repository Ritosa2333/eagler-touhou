import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {test} from 'node:test';
import {PRODUCT_GAMES} from '../../../common/contracts/product-catalog.mjs';
import {DATA_LAYOUT_SPEC, DATA_MAGIC, DATA_SCHEMA, ManagedContent} from '../web/managed-data.mjs';

const hash = bytes => createHash('sha256').update(bytes).digest('hex');
const payload = Buffer.from('fixture');
function pack(path) {
  const index = Buffer.from(JSON.stringify({schema:DATA_SCHEMA, game:'th02',
    files:[{path, offset:0, bytes:payload.length, sha256:hash(payload)}]}));
  const header = Buffer.alloc(12);
  header.write(DATA_MAGIC);
  header.writeUInt32LE(index.length, 8);
  return Buffer.concat([header, index, payload]);
}

test('TH02 Launcher and native loader agree on the DATA identity', async () => {
  assert.equal(PRODUCT_GAMES.th02.dataLayout, 'sha256-'+hash(JSON.stringify(DATA_LAYOUT_SPEC)));
  const path = 'native/content/full-files.json';
  const content = await ManagedContent.open(pack(path));
  assert.equal(await (await content.fetch(path)).text(), payload.toString());
});

test('TH02 rejects DATA with legacy disk entries', async () => {
  await assert.rejects(ManagedContent.open(pack('previous-generation.hdi.gz')), /条目无效/);
});
