import {test} from 'vitest';
import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";
import { resolve } from "node:path";
import { PRODUCT_GAMES } from "../lib/contracts/product-catalog.mjs";
import {
  assertHostedSiteMatchesOperatorProfile,
  validateOperatorProfile,
} from "../tools/maintainer/operator-delivery.mjs";

test("operator delivery", async () => {
const profile = validateOperatorProfile(JSON.parse(await readFile(
  resolve("tools/maintainer/operator-delivery/profile.json"), "utf8"
)));
const preview = {...profile, games: Object.fromEntries(Object.keys(PRODUCT_GAMES)
  .filter(game => Object.hasOwn(profile.games, game) || game === 'th13')
  .map(game => [game, profile.games[game] ?? {languages: ['ja'], music: ['ogg'], thprac: false, runtimeVariants: ['normal']}]))};
assert.equal(validateOperatorProfile(preview), preview);
const missingRequired = structuredClone(preview);
delete missingRequired.games.th06;
assert.throws(() => validateOperatorProfile(missingRequired), /every registered game/);
const manifest = {
  schema: "eagler-touhou/site-manifest/1",
  shared: {
    resourceMode: "hosted",
    gameDataFallback: { url: "https://example.invalid/games" },
    originMigration: { mode: "http-to-https" },
  },
  games: Object.fromEntries(Object.entries(profile.games).map(([game, expected]) => [game, {
    features: { thprac: expected.thprac },
    music: Object.fromEntries(expected.music.map(mode => [mode, { files: [] }])),
    runtime: `runtime/${game}/${game}.html`,
    ...(expected.runtimeVariants.includes("multiplayer")
      ? { multiplayerRuntime: `runtime/${game}/multiplayer/${game}.html` }
      : {}),
    languageOptions: expected.languages.map(id => ({ id, pack: id === "ja" ? null : { url: `${id}.zip` } })),
    package: { descriptor: `${game}.package.json` },
  }])),
};

assert.equal(assertHostedSiteMatchesOperatorProfile(manifest, profile), profile);

const missingLanguage = structuredClone(manifest);
missingLanguage.games.th06.languageOptions = missingLanguage.games.th06.languageOptions.filter(entry => entry.id !== "lang_en");
assert.throws(
  () => assertHostedSiteMatchesOperatorProfile(missingLanguage, profile),
  /th06: Operator language profile mismatch/,
);

const configuredRelay = structuredClone(manifest);
configuredRelay.shared.netplayRelay = "wss://example.invalid/eagler-netplay/";
assert.throws(
  () => assertHostedSiteMatchesOperatorProfile(configuredRelay, profile),
  /requires Netplay Relay to be absent/,
);

console.log("Internal Operator delivery profile: PASS");
});
