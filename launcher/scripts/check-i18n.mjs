import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readFile} from 'node:fs/promises';
import {parse, parseFragment, serialize} from 'parse5';

const read = async path => JSON.parse(await readFile(new URL('../' + path, import.meta.url), 'utf8'));
const manifest = await read('messages/keys.json'), locales = ['zh-CN', 'en', 'ja'];
const ids = Object.values(manifest).map(entry => entry.id);
assert.equal(new Set(ids).size, ids.length, 'Compiled message identifiers must be unique');
const catalogs = Object.fromEntries(await Promise.all(locales.map(async locale => [locale, await read(`messages/${locale}.json`)])));
for (const locale of locales) {
  assert.deepEqual(Object.keys(catalogs[locale]).filter(key => key !== '$schema').sort(), [...ids].sort(), `${locale}: catalog keys differ`);
  for (const [key, entry] of Object.entries(manifest)) {
    assert.equal(new Set(entry.params).size, entry.params.length, `${key}: duplicate parameter declarations`);
    const text = catalogs[locale][entry.id];
    assert.equal(typeof text, 'string', `${locale}/${key}: missing message`);
    assert.ok(text.trim() || !catalogs.en[entry.id].trim(), `${locale}/${key}: empty message`);
    assert.deepEqual([...new Set([...text.matchAll(/\{([A-Za-z0-9_]+)\}/g)].map(match => match[1]))].sort(), [...entry.params].sort(), `${locale}/${key}: parameters differ`);
  }
}
for (const {key, source} of await read('messages/errors.json')) {
  assert.ok(manifest[key], `Unknown error message: ${key}`);
  assert.equal(catalogs['zh-CN'][manifest[key].id], source, `Error mapping changed: ${key}`);
}
for (const [key, file] of [['content.firstUse', 'FIRST_USE_NOTICE'], ['content.multiplayer', 'MULTIPLAYER']]) {
  const source = (await readFile(new URL(`../public/content/${file}.html`, import.meta.url), 'utf8')).replace(/\r\n?/g, '\n').trim();
  assert.equal(catalogs['zh-CN'][manifest[key].id].replace(/\r\n?/g, '\n').trim(), source, `${key}: authored Chinese content differs; rebuild content and update its catalog`);
}
for (const kind of ['faq', 'about']) {
  const document = parse(await readFile(new URL(`../public/${kind}.html`, import.meta.url), 'utf8'));
  const find = node => node.attrs?.some(({name}) => name === 'data-localized-content') ? node : node.childNodes?.map(find).find(Boolean);
  const target = find(document);
  assert.ok(target, `${kind}: missing localized content target`);
  const authored = serialize(parseFragment(catalogs['zh-CN'][manifest[`content.${kind}`].id])).trim();
  assert.equal(serialize(target).trim(), authored, `${kind}: authored Chinese content differs from its catalog`);
}
// Migration evidence is an explicit gate. Normal wording changes use the normal
// catalog check; a future copy edit does not need to preserve historical text.
if (process.argv.includes('--migration')) {
  const baseline = await read('tests/fixtures/i18n-migration.sha256.json');
  for (const locale of ['zh-CN', 'en']) {
    const values = baseline.keys.map(key => {assert.ok(manifest[key], `Missing migrated key: ${key}`); return catalogs[locale][manifest[key].id];});
    assert.equal(createHash('sha256').update(JSON.stringify(values)).digest('hex'), baseline.sha256[locale], `${locale}: migration changed authored wording`);
  }
}
console.log(`PASS | ${locales.join('/')} | ${ids.length} messages | complete keys and parameters${process.argv.includes('--migration') ? ' | migration wording' : ''}`);
