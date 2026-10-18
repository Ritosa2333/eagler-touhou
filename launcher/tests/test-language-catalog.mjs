import assert from "node:assert/strict";
import {
  buildLanguageCatalog,
  resolvePreferredGameLanguage,
  resolveLanguagePackSource,
  selectLanguageEntry,
  thpracLocaleForLanguage,
} from "../.cache/build/browser/assets/launcher/language-catalog.mjs";
import { languagePriority } from "../lib/contracts/product-catalog.mjs";

const translate = key => ({
  "gameLanguage.ja": "Japanese",
  "gameLanguage.zhHans": "Simplified Chinese",
  "gameLanguage.zhHant": "Traditional Chinese",
  "gameLanguage.en": "English",
  "gameLanguage.ru": "Russian",
})[key] || key;

const canonical = buildLanguageCatalog({
  languageOptions: [
    { id: "lang_en", title: "ignored", pack: { url: "en.zip" } },
    { id: "ja", title: "ignored", pack: null },
  ],
  legacyLanguages: [{ id: "lang_ru" }],
  translate,
  priority: languagePriority,
});
assert.deepEqual(canonical.map(entry => entry.id), ["ja", "lang_en"]);
assert.equal(canonical[0].title, "Japanese");
assert.equal(canonical[1].title, "English");

const legacy = buildLanguageCatalog({
  legacyLanguages: [{ id: "lang_zh-hans", title: "legacy", pack: { url: "zh.zip" } }],
  translate,
  priority: languagePriority,
});
assert.deepEqual(legacy.map(entry => entry.id), ["ja", "lang_zh-hans"]);
assert.equal(legacy[1].title, "Simplified Chinese");

const generation = {
  descriptor: {
    components: {
      language: {
        entries: [
          { id: "lang_en", title: "Package English", file: "lang-en" },
          { id: "lang_zh-hant", title: "Package Chinese", file: "lang-zh-hant" },
          { id: "lang_ru", title: "Missing Object", file: "missing" },
        ],
      },
    },
    files: {
      "lang-en": { bytes: 123 },
      "lang-zh-hant": { bytes: 456 },
      missing: { bytes: 789 },
    },
  },
  files: {
    "lang-en": { objectId: "obj-en" },
    "lang-zh-hant": { objectId: "obj-zh" },
  },
};
const merged = buildLanguageCatalog({
  languageOptions: canonical,
  generation,
  translate,
  priority: languagePriority,
});
assert.deepEqual(merged.map(entry => entry.id), ["ja", "lang_zh-hant", "lang_en"]);
assert.equal(merged.find(entry => entry.id === "lang_en").packageObjectId, "obj-en");
assert.equal(merged.find(entry => entry.id === "lang_en").packageBytes, 123);
assert.equal(merged.find(entry => entry.id === "lang_zh-hant").packageObjectId, "obj-zh");
assert.equal(merged.some(entry => entry.id === "lang_ru"), false);

assert.equal(selectLanguageEntry(merged, "lang_en").id, "lang_en");
assert.equal(selectLanguageEntry(merged, "missing").id, "ja");
assert.deepEqual(resolveLanguagePackSource(merged.find(entry => entry.id === "lang_en"), "https://host.example/app/"), {
  language: "lang_en",
  packageObjectId: "obj-en",
  packageFile: "lang-en",
  bytes: 123,
  packageLocal: true,
});
const remote = resolveLanguagePackSource({
  id: "lang_en",
  pack: { url: "packs/en.zip", sha256: "a".repeat(64), bytes: 42, files: 3 },
}, "https://host.example/app/");
assert.equal(remote.url, `https://host.example/app/packs/en.zip?v=${"a".repeat(64)}`);
assert.equal(remote.language, "lang_en");
assert.equal(remote.sha256, "a".repeat(64));
assert.throws(() => resolveLanguagePackSource({ id: "lang_en", pack: { url: "bad.zip" } }, "https://host.example/"), /语言包清单无效/);
assert.equal(thpracLocaleForLanguage("ja"), "ja-JP");
assert.equal(thpracLocaleForLanguage("lang_zh-hans"), "zh-CN");
assert.equal(thpracLocaleForLanguage("lang_en"), "en-US");

console.log(JSON.stringify({
  languageCatalog: "PASS",
  canonical: "languageOptions",
  legacyReadFallback: "languages",
  packageOverlay: true,
}));

const translatedCatalog = ["ja", "lang_zh-hans", "lang_zh-hant", "lang_en"].map(id => ({ id }));
assert.equal(resolvePreferredGameLanguage(translatedCatalog, null, "zh-CN"), "lang_zh-hans");
assert.equal(resolvePreferredGameLanguage(translatedCatalog, null, "en-US"), "lang_en");
assert.equal(resolvePreferredGameLanguage(translatedCatalog, "ja", "zh-CN"), "ja", "explicit original language must be preserved");
assert.equal(resolvePreferredGameLanguage(translatedCatalog, "lang_zh-hans", "en"), "lang_zh-hans");
assert.equal(resolvePreferredGameLanguage([{ id: "ja" }], null, "en"), "ja", "unpublished translations cannot be selected");
assert.equal(resolvePreferredGameLanguage([{ id: "ja" }], null, "zh-CN"), "ja");
assert.equal(resolvePreferredGameLanguage(translatedCatalog, null, "ja"), "ja");
assert.equal(resolvePreferredGameLanguage([{ id: "ja" }, { id: "lang_zh-hant" }], null, "zh-CN"), "lang_zh-hant");
assert.equal(resolvePreferredGameLanguage(translatedCatalog, "unavailable", "en"), "lang_en");
