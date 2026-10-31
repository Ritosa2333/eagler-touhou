import {test} from 'vitest';
import assert from "node:assert/strict";
import {
  canUseExistingInstallationAfterRemoteFailure,
  desiredFilesForPublishedPackage,
  installedComponentIds,
} from "../package/package-launcher.mjs";

test("test-package-launcher", async () => {
const descriptor = {
  schema: "eagler-touhou/package/1",
  game: "th08",
  revision: "r2",
  runtime: { type: "html", entry: "html", playerProtocol: "eagler-touhou/player/1", bootstrap: ["html", "js", "wasm", "data"] },
  files: {
    html: { source: "r/th08.html", target: "/r/th08.html", revision: "a" },
    js: { source: "r/th08.js", target: "/r/th08.js", revision: "b" },
    wasm: { source: "r/th08.wasm", target: "/r/th08.wasm", revision: "c" },
    data: { source: "r/game.bin", target: "/game.bin", revision: "d" },
    o1: { source: "ogg/01.ogg", target: "/music/01.ogg", revision: "e" },
    o2: { source: "ogg/02.ogg", target: "/music/02.ogg", revision: "f" },
    zh: { source: "lang/zh.zip", target: "/lang/zh.zip", revision: "g" },
    en: { source: "lang/en.zip", target: "/lang/en.zip", revision: "h" },
  },
  base: { files: ["html", "js", "wasm", "data"] },
  components: {
    ogg: { type: "ogg", files: ["o1", "o2"] },
    language: { type: "language", entries: [
      { id: "lang_zh-hans", title: "中文（简体）", file: "zh" },
      { id: "lang_en", title: "English", file: "en" },
    ] },
  },
};
const current = {
  game: "th08",
  descriptor,
  files: { html: { objectId: "x" }, js: { objectId: "y" }, wasm: { objectId: "z" }, data: { objectId: "q" }, o1: { objectId: "ogg" }, zh: { objectId: "lang" } },
};
assert.deepEqual(installedComponentIds(current), ["ogg", "language"], "present optional resources are discoverable as installed components");
assert.deepEqual(desiredFilesForPublishedPackage(descriptor, { current }), ["html", "js", "wasm", "data", "o1", "zh"],
  "updates must preserve only installed progressive OGG files and installed language entries");
assert.deepEqual(desiredFilesForPublishedPackage(descriptor, { current, addFileIds: ["o2"] }), ["html", "js", "wasm", "data", "o2", "o1", "zh"],
  "a progressive OGG step adds one requested track without expanding to the complete component");
assert.deepEqual(desiredFilesForPublishedPackage(descriptor, { current: null }), ["html", "js", "wasm", "data"],
  "first install must not force optional components");
assert.deepEqual(desiredFilesForPublishedPackage(descriptor, { current: null, addComponents: ["ogg"] }), ["html", "js", "wasm", "data", "o1", "o2"],
  "explicitly requested optional components must be added to the generation");

const currentWithAllLanguages = {
  ...current,
  files: { ...current.files, en: { objectId: "english" } },
};
assert.deepEqual(desiredFilesForPublishedPackage(descriptor, {
  current: currentWithAllLanguages,
  selectedComponentEntries: { language: ["lang_zh-hans"] },
}), ["html", "js", "wasm", "data", "o1", "zh"],
  "an update after a full local import must retain only the language selected by the player");
assert.deepEqual(desiredFilesForPublishedPackage(descriptor, {
  current: currentWithAllLanguages,
  selectedComponentEntries: { language: [] },
}), ["html", "js", "wasm", "data", "o1"],
  "selecting the built-in Japanese resources must not retain translated language packs");
assert.throws(() => desiredFilesForPublishedPackage(descriptor, {
  current: currentWithAllLanguages,
  selectedComponentEntries: { language: ["lang_missing"] },
}), /unknown requested Package component entry/,
  "an unavailable selected entry must fail instead of silently widening the component download");

const descriptorWithoutO2 = structuredClone(descriptor);
descriptorWithoutO2.revision = "r3";
descriptorWithoutO2.components.ogg.files = ["o1"];
delete descriptorWithoutO2.files.o2;
const currentWithBothOgg = {
  ...current,
  files: { ...current.files, o2: { objectId: "ogg2" } },
};
assert.deepEqual(desiredFilesForPublishedPackage(descriptorWithoutO2, { current: currentWithBothOgg }), ["html", "js", "wasm", "data", "o1", "zh"],
  "files removed by the new Descriptor are intentionally dropped instead of being fetched or treated as update failures");

assert.equal(canUseExistingInstallationAfterRemoteFailure({
  hostManifestAvailable: true,
  installedGeneration: { id: "current" },
}), true, "a remote failure may fall back only when a current local generation exists");
for (const fallbackState of [
  { hostManifestAvailable: false, installedGeneration: { id: "current" } },
  { hostManifestAvailable: true, installedGeneration: null },
  { hostManifestAvailable: false, installedGeneration: null },
]) {
  assert.equal(canUseExistingInstallationAfterRemoteFailure(fallbackState), false,
    "a first-install or unavailable Site Manifest must not claim that an existing installation will be used");
}
console.log("Package Launcher policy contract: PASS (fetch cancellation is Browser-owned)");
});
