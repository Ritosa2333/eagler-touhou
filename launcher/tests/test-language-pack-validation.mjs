import assert from "node:assert/strict";

import { validateStaticLanguagePackEntries } from "../.cache/build/browser/assets/launcher/language-pack-validation.mjs";

const encoder = new TextEncoder();
const filePath = "/thcrap/th06/localization/stages.etl";
const payload = encoder.encode("ETL1");
const manifest = {
  schema: "eagler-touhou/thcrap-static-pack/1",
  game: "th06",
  language: "lang_zh-hans",
  runtimeVersion: "0123456789abcdef",
  files: [{ path: filePath, bytes: payload.length }],
};
const entries = value => ({
  "manifest.json": encoder.encode(JSON.stringify(value)),
  "thcrap/th06/localization/stages.etl": payload,
});

const valid = validateStaticLanguagePackEntries(entries(manifest), { game: "th06", language: "lang_zh-hans" });
assert.equal(valid.manifest.runtimeVersion, manifest.runtimeVersion);
assert.deepEqual(valid.files.map(file => file.path), [filePath]);
assert.deepEqual(valid.files[0].bytes, payload);

assert.throws(() => validateStaticLanguagePackEntries(entries({ ...manifest, game: "th07" }), {
  game: "th06", language: "lang_zh-hans",
}), /清单不兼容/);
assert.throws(() => validateStaticLanguagePackEntries(entries({ ...manifest, language: "lang_en" }), {
  game: "th06", language: "lang_zh-hans",
}), /清单不兼容/);
assert.throws(() => validateStaticLanguagePackEntries(entries({
  ...manifest,
  files: [manifest.files[0], { ...manifest.files[0] }],
}), { game: "th06", language: "lang_zh-hans" }), /重复路径/);
assert.throws(() => validateStaticLanguagePackEntries({
  ...entries(manifest),
  "thcrap/th06/extra.bin": new Uint8Array([1]),
}, { game: "th06", language: "lang_zh-hans" }), /文件数量不一致/);
assert.throws(() => validateStaticLanguagePackEntries(entries({
  ...manifest,
  files: [{ path: "/thcrap/th07/escape.bin", bytes: 1 }],
}), { game: "th06", language: "lang_zh-hans" }), /文件清单无效/);
assert.throws(() => validateStaticLanguagePackEntries(entries({
  ...manifest,
  files: [{ path: filePath, bytes: payload.length + 1 }],
}), { game: "th06", language: "lang_zh-hans" }), /文件大小错误/);

const th08Path = "/thcrap/th08/localization/strings.etl";
const th08Manifest = {
  schema: "eagler-touhou/thcrap-static-pack/1",
  game: "th08",
  language: "lang_zh-hans",
  runtimeVersion: "independent",
  files: [{ path: th08Path, bytes: payload.length }],
};
const th08Valid = validateStaticLanguagePackEntries({
  "manifest.json": encoder.encode(JSON.stringify(th08Manifest)),
  "thcrap/th08/localization/strings.etl": payload,
}, { game: "th08", language: "lang_zh-hans" });
assert.equal(th08Valid.manifest.game, "th08");
assert.deepEqual(th08Valid.files.map(file => file.path), [th08Path]);
assert.throws(() => validateStaticLanguagePackEntries({
  "manifest.json": encoder.encode(JSON.stringify(th08Manifest)),
  "thcrap/th08/localization/strings.etl": payload,
}, { game: "th06", language: "lang_zh-hans" }), /清单不兼容/);

const th09Path = "/thcrap/th09/localization/strings.etl";
const th09Manifest = {
  ...th08Manifest,
  game: "th09",
  files: [{ path: th09Path, bytes: payload.length }],
};
const th09Valid = validateStaticLanguagePackEntries({
  "manifest.json": encoder.encode(JSON.stringify(th09Manifest)),
  "thcrap/th09/localization/strings.etl": payload,
}, { game: "th09", language: "lang_zh-hans" });
assert.equal(th09Valid.manifest.game, "th09");
assert.deepEqual(th09Valid.files.map(file => file.path), [th09Path]);
assert.throws(() => validateStaticLanguagePackEntries({
  "manifest.json": encoder.encode(JSON.stringify(th09Manifest)),
  "thcrap/th09/localization/strings.etl": payload,
}, { game: "th08", language: "lang_zh-hans" }), /清单不兼容/);

console.log(JSON.stringify({ languagePackValidation: "PASS", duplicatePaths: "rejected", fileSha256: "not-a-contract", th08: "accepted", th09: "accepted" }));
