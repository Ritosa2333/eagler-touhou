import assert from 'node:assert/strict';
import { createHash } from 'node:crypto';
import { readFile } from 'node:fs/promises';

const read = async (name) => JSON.parse(await readFile(new URL(`../${name}`, import.meta.url), 'utf8'));
const manifest = await read('messages/keys.json');
const locales = ['zh-CN', 'en', 'ja'];
const ids = Object.values(manifest).map((entry) => entry.id);
assert.equal(new Set(ids).size, ids.length, 'Compiled IDs must be unique');
const catalogs = Object.fromEntries(
  await Promise.all(locales.map(async (locale) => [locale, await read(`messages/${locale}.json`)])),
);
for (const locale of locales) {
  assert.deepEqual(
    Object.keys(catalogs[locale]).filter((key) => key !== '$schema'),
    ids,
    `${locale}: incomplete catalog`,
  );
  for (const [key, entry] of Object.entries(manifest)) {
    const message = catalogs[locale][entry.id];
    assert.equal(typeof message, 'string', `${locale}/${key}: missing message`);
    assert.ok(message.trim() || !catalogs.en[entry.id].trim(), `${locale}/${key}: empty message`);
    assert.deepEqual(
      [...new Set(message.match(/\{[A-Za-z0-9_]+\}/g) || [])].sort(),
      [...new Set(entry.params.map((name) => `{${name}}`))].sort(),
      `${locale}/${key}: changed inputs`,
    );
  }
}
const original = await read('tests/fixtures/i18n-migration.sha256.json');
for (const locale of ['zh-CN', 'en']) {
  const values = ids.slice(0, original.count).map((id) => catalogs[locale][id]);
  assert.equal(
    createHash('sha256').update(JSON.stringify(values)).digest('hex'),
    original.sha256[locale],
    `${locale}: original wording changed`,
  );
}

const errors = await read('messages/errors.json');
for (const { key, source } of errors) {
  assert.ok(manifest[key], `Unknown UI error message: ${key}`);
  assert.equal(catalogs['zh-CN'][manifest[key].id], source, `Changed protocol error mapping: ${key}`);
}

// The original Chinese documents are authored Markdown; prevent their translated
// catalog counterpart from silently drifting when those documents are rebuilt.
const normalizeHtml = (value) => value.replace(/\s+/g, ' ').replace(/>\s+</g, '><').trim();
for (const [kind, file] of [
  ['firstUse', 'public/content/FIRST_USE_NOTICE.html'],
  ['multiplayer', 'public/content/MULTIPLAYER.html'],
  ['faq', 'public/faq.html'],
]) {
  const html = await readFile(new URL(`../${file}`, import.meta.url), 'utf8');
  const content = kind === 'faq' ? html.match(/<article class="faq-content"[^>]*>([\s\S]*?)<\/article>/)?.[1] : html;
  assert.ok(content, `Missing authored ${kind} content`);
  assert.equal(
    normalizeHtml(content),
    normalizeHtml(catalogs['zh-CN'][manifest[`content.${kind}`].id]),
    `Update the ${kind} Paraglide content alongside its authored document`,
  );
}

console.log(
  JSON.stringify({
    locales,
    messages: ids.length,
    originalMessages: original.count,
    originalWording: 'PASS',
    placeholders: 'PASS',
  }),
);
