import assert from "node:assert/strict";
import { assertLanguagePublicationConsistency } from "../lib/language-publication-contract.mjs";

const pack = {
  url: "games/th06/language/lang_en.zip",
  bytes: 42,
  sha256: "a".repeat(64),
  runtimeVersion: "independent",
  files: 3,
};
const host = {
  languages: [{ id: "lang_en", title: "English", pack }],
  languageOptions: [{ id: "ja", pack: null }, { id: "lang_en", title: "English", pack: { ...pack } }],
};
const descriptor = {
  files: {
    "language:lang_en": {
      source: pack.url,
      target: "/__eagler/language/lang_en.zip",
      revision: pack.sha256.slice(0, 16),
      bytes: pack.bytes,
      sha256: pack.sha256,
    },
  },
  components: {
    language: { type: "language", entries: [{ id: "lang_en", title: "English", file: "language:lang_en" }] },
  },
};

assert.doesNotThrow(() => assertLanguagePublicationConsistency("th06", host, descriptor));
for (const filePatch of [
  { source: "games/th06/language/missing.zip" },
  { bytes: 43 },
  { sha256: "b".repeat(64) },
  { revision: "b".repeat(16) },
  { target: "/__eagler/language/other.zip" },
]) {
  const broken = structuredClone(descriptor);
  Object.assign(broken.files["language:lang_en"], filePatch);
  assert.throws(() => assertLanguagePublicationConsistency("th06", host, broken), /language identity mismatch/);
}
const omitted = structuredClone(descriptor);
delete omitted.components.language;
assert.throws(() => assertLanguagePublicationConsistency("th06", host, omitted), /entry set mismatch/);
assert.throws(() => assertLanguagePublicationConsistency("th06", { languages: [] }, descriptor), /undeclared/);

// Directory-layout games (th08) publish through the same Host/Descriptor
// language surfaces as the preload-layout games.
const th08Packs = ["lang_zh-hans", "lang_en"].map((id, index) => ({
  id,
  pack: {
    url: `games/th08/language/${id}.zip`,
    bytes: 100 + index,
    sha256: (index ? "c" : "d").repeat(64),
    runtimeVersion: "independent",
    files: 10 + index,
  },
}));
const th08Host = {
  languages: th08Packs.map(({ id, pack }) => ({ id, title: id, pack })),
  languageOptions: [{ id: "ja", pack: null }, ...th08Packs.map(({ id, pack }) => ({ id, title: id, pack: { ...pack } }))],
};
const th08Descriptor = {
  files: Object.fromEntries(th08Packs.map(({ id, pack }) => [`language:${id}`, {
    source: pack.url,
    target: `/__eagler/language/${id}.zip`,
    revision: pack.sha256.slice(0, 16),
    bytes: pack.bytes,
    sha256: pack.sha256,
  }])),
  components: {
    language: { type: "language", entries: th08Packs.map(({ id }) => ({ id, title: id, file: `language:${id}` })) },
  },
};
assert.doesNotThrow(() => assertLanguagePublicationConsistency("th08", th08Host, th08Descriptor));
const th08Broken = structuredClone(th08Descriptor);
delete th08Broken.files["language:lang_en"];
assert.throws(() => assertLanguagePublicationConsistency("th08", th08Host, th08Broken), /language identity mismatch/);
const th08Omitted = structuredClone(th08Descriptor);
th08Omitted.components.language.entries.pop();
assert.throws(() => assertLanguagePublicationConsistency("th08", th08Host, th08Omitted), /entry set mismatch/);

console.log(JSON.stringify({ languagePublication: "PASS", surfaces: ["languages", "languageOptions", "descriptor"], th08: "directory-layout" }));
