// Module regression: a structurally valid old hosted site must not suppress
// preparation of newly enabled default language packs. No game data is used.
import assert from "node:assert/strict";
import { mkdir, mkdtemp, writeFile } from "node:fs/promises";
import { resolve } from "node:path";
import { PRODUCT_GAMES } from "../lib/contracts/product-catalog.mjs";
import { hostedLanguagesComplete } from "../host/lib/site-builder.mjs";

const games = Object.keys(PRODUCT_GAMES);
const manifest = { games: Object.fromEntries(games.map(game => [game, {
  languageOptions: ["ja", "lang_zh-hans", "lang_en"].map(id => ({
    id, pack: id === "ja" ? null : { url: `games/${game}/language/${id}.zip` },
  })),
}])) };
const save = async () => {};
await save();
assert.equal(hostedLanguagesComplete(manifest), true);
const full = manifest.games.th09.languageOptions;
manifest.games.th09.languageOptions = full.slice(0, 1);
await save();
assert.equal(hostedLanguagesComplete(manifest), false, "Japanese-only TH09 cache must rebuild");
manifest.games.th09.languageOptions = full.map(option => ({ ...option, pack: null }));
await save();
assert.equal(hostedLanguagesComplete(manifest), false, "labels without packs must rebuild");
manifest.games.th09.languageOptions = full;
await save();
assert.equal(hostedLanguagesComplete(manifest), true);
console.log(JSON.stringify({ hostedLanguageCache: "validated", th09MissingPacks: "rebuild" }));
