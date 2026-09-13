/** L3/module. Preconditions: static product policy only. Mutations: none.
 * Proves: stable product registry invariants, product-id mapping and language ordering. */
import assert from "node:assert/strict";
import {
  PRODUCT_GAMES,
  PRODUCT_IDS,
  gameIdForProduct,
  isMultiplayerProductId,
  languagePriority,
  multiplayerConfigForProduct,
  multiplayerProductIdForGame,
  productFeatureAvailable,
  productEnabledForBuild,
} from "../lib/contracts/product-catalog.mjs";

assert.deepEqual(
  ["lang_ru", "lang_en", "ja", "lang_zh-hant", "lang_zh-hans", "lang_de"]
    .sort((a, b) => languagePriority(a) - languagePriority(b) || a.localeCompare(b, "en")),
  ["ja", "lang_zh-hans", "lang_zh-hant", "lang_en", "lang_de", "lang_ru"],
);

assert.deepEqual(PRODUCT_IDS, ["th06", "th07", "th08", "th10", "th06mp", "th07mp"]);
assert.equal(isMultiplayerProductId("th06mp"), true);
assert.equal(isMultiplayerProductId("th07mp"), true);
assert.equal(isMultiplayerProductId("th06"), false);
assert.equal(isMultiplayerProductId("th08mp"), false);
assert.equal(gameIdForProduct("th06mp"), "th06");
assert.equal(gameIdForProduct("th07"), "th07");
assert.equal(multiplayerProductIdForGame("th06"), "th06mp");
assert.equal(multiplayerProductIdForGame("th07"), "th07mp");
assert.equal(multiplayerProductIdForGame("th08"), null);
assert.equal(multiplayerConfigForProduct("th06mp"), PRODUCT_GAMES.th06.multiplayer);
assert.equal(multiplayerConfigForProduct("th07"), PRODUCT_GAMES.th07.multiplayer);
assert.equal(multiplayerConfigForProduct("th08"), null);
assert.equal(productFeatureAvailable("th06", "thprac"), true);
assert.equal(productFeatureAvailable("th06", "thprac", { thprac: false }), false);
assert.equal(productFeatureAvailable("th08", "thprac", { thprac: true }), false);
assert.equal(productFeatureAvailable("th06", "focusHitbox", { focusHitbox: false }), false);
assert.equal(productFeatureAvailable("th07", "focusHitbox", { focusHitbox: true }), false);
assert.equal(productFeatureAvailable("th06", "replayManagement", { replayManagement: false }), true);
assert.equal(productFeatureAvailable("th08", "replayManagement", { replayManagement: true }), false);
assert.equal(productFeatureAvailable("th06", "languages", { languages: false }), true);
assert.equal(PRODUCT_GAMES.th06.multiplayerRuntime, "./runtime/th06/multiplayer/th06.html");
assert.deepEqual(PRODUCT_GAMES.th06.multiplayer, {
  difficultyMax: 4,
  characterMax: 1,
  loadoutCount: 4,
  peerTransportGlobal: "__th06PeerTransport",
});
assert.equal(PRODUCT_GAMES.th07.multiplayerRuntime, "./runtime/th07/multiplayer/th07.html");
assert.deepEqual(PRODUCT_GAMES.th07.multiplayer, {
  difficultyMax: 5,
  characterMax: 2,
  loadoutCount: 6,
  peerTransportGlobal: "__th07PeerTransport",
});

const roots = new Set();
for (const [game, product] of Object.entries(PRODUCT_GAMES)) {
  assert.equal(roots.has(product.storage.saveRoot), false, `${game}: unique save owner`);
  roots.add(product.storage.saveRoot);
  assert.ok(product.storage.saveRoot.startsWith("/"));
  assert.ok(!product.storage.scoreFile.includes("/"));
  assert.equal(typeof product.features.replayManagement, "boolean");
  assert.equal(typeof product.features.languages, "boolean");
  assert.equal(typeof product.features.focusHitbox, "boolean");
  assert.equal(typeof product.package.dataFileId, "string");
  assert.equal(product.package.dataTarget, `/${game}.data`);
  for (const [mode, directory] of Object.entries(product.package.musicSourceDirectories || {})) {
    assert.ok(["wav", "ogg"].includes(mode));
    assert.equal(typeof directory, "string");
    assert.ok(directory === "." || /^[A-Za-z0-9_.-]+$/.test(directory));
  }
  if (product.features.replayManagement) assert.match(product.replay?.prefix || "", /^th\d$/);
  else assert.equal(product.replay, undefined);
  if (product.multiplayerRuntime) {
    assert.ok(product.multiplayer);
    assert.ok(Number.isInteger(product.multiplayer.difficultyMax));
    assert.ok(Number.isInteger(product.multiplayer.characterMax));
    assert.ok(Number.isInteger(product.multiplayer.loadoutCount));
    assert.match(product.multiplayer.peerTransportGlobal, /^__[A-Za-z0-9]+$/);
  } else {
    assert.equal(product.multiplayer, undefined);
  }
}
console.log("Product catalog policy: PASS");

for (const id of ["th06", "th07", "th08", "th10", "th06mp", "th07mp"]) {
  assert.equal(productEnabledForBuild(id), true);
  assert.equal(productEnabledForBuild(id, false), true);
  assert.equal(productEnabledForBuild(id, true), true);
}
assert.equal(productEnabledForBuild("th99", true), false);
