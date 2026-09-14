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

console.log(JSON.stringify({ languagePublication: "PASS", surfaces: ["languages", "languageOptions", "descriptor"] }));
