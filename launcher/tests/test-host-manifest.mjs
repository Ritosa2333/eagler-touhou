import assert from "node:assert/strict";
import {
  SITE_MANIFEST_FILE,
  SITE_MANIFEST_SCHEMA,
  siteOriginMigrationAvailable,
  validateSiteManifest,
} from "../lib/contracts/site-manifest.mjs";

const game = {
  runtime: "runtime/th06/th06.html?hosted=1&v=1",
  multiplayerRuntime: "runtime/th06/multiplayer/th06.html?hosted=1&v=2",
  gameData: {
    path: "th06.data",
    bytes: 1,
    sha256: "a".repeat(64),
    version: `sha256-${"a".repeat(64)}`,
    layout: `sha256-${"b".repeat(64)}`,
  },
  music: { midi: { files: [] } },
  features: { thprac: true, focusHitbox: true },
};

const languagePack = {
  url: "games/th06/language/lang_en.zip",
  bytes: 42,
  sha256: "c".repeat(64),
  runtimeVersion: "independent",
  files: 3,
};
const gameWithLanguages = {
  ...game,
  languages: [{ id: "lang_en", title: "English", pack: languagePack }],
  languageOptions: [
    { id: "ja", title: "Japanese", pack: null },
    { id: "lang_en", title: "English", pack: { ...languagePack } },
  ],
};

const hosted = validateSiteManifest({
  schema: SITE_MANIFEST_SCHEMA,
  protocol: "eagler-touhou/1",
  profile: "web-validation-test",
  shared: {
    resourceMode: "hosted",
    vanillaFont: "../shared/msgothic.ttc?v=x",
    unicodeFont: "../shared/unifont.otf?v=y",
    netplayRelay: "wss://relay.example.invalid/",
  },
  games: { th06: game },
});
assert.equal(hosted.shared.resourceMode, "hosted");
assert.equal(SITE_MANIFEST_FILE, "site-manifest.json");
assert.equal(siteOriginMigrationAvailable(hosted, "https:"), false);

const migrationHost = validateSiteManifest({
  ...hosted,
  shared: { ...hosted.shared, originMigration: { mode: "http-to-https" } },
});
assert.equal(siteOriginMigrationAvailable(migrationHost, "https:"), true);
assert.equal(siteOriginMigrationAvailable(migrationHost, "http:"), false);
for (const originMigration of [true, "http-to-https", {}, { mode: "https-only" }]) {
  assert.throws(() => validateSiteManifest({
    ...hosted,
    shared: { ...hosted.shared, originMigration },
  }), /originMigration/);
}

assert.throws(() => validateSiteManifest({ ...hosted, games: { th99: game } }), /games/);
assert.throws(() => validateSiteManifest({ ...hosted, shared: { ...hosted.shared, resourceMode: "import-partial" } }), /resource mode/);
assert.throws(() => validateSiteManifest({ ...hosted, shared: { ...hosted.shared, netplayRelay: "https://example.invalid/" } }), /netplayRelay/);
assert.doesNotThrow(() => validateSiteManifest({
  ...hosted,
  games: { th06: { ...game, features: undefined } },
}));
assert.doesNotThrow(() => validateSiteManifest({
  ...hosted,
  games: { th06: { ...game, music: { midi: { files: [], supported: false } } } },
}));
assert.doesNotThrow(() => validateSiteManifest({ ...hosted, games: { th06: gameWithLanguages } }));
for (const broken of [
  { ...gameWithLanguages, languageOptions: gameWithLanguages.languageOptions.map(item => item.id === "lang_en"
    ? { ...item, pack: { ...languagePack, url: "games/th06/language/missing.zip" } } : item) },
  { ...gameWithLanguages, languageOptions: [{ id: "ja", pack: null }] },
  { ...gameWithLanguages, languages: [...gameWithLanguages.languages, gameWithLanguages.languages[0]] },
  { ...gameWithLanguages, languageOptions: [{ id: "ja", pack: languagePack }, gameWithLanguages.languageOptions[1]] },
]) {
  assert.throws(() => validateSiteManifest({ ...hosted, games: { th06: broken } }), /games/);
}

const external = validateSiteManifest({
  ...hosted,
  shared: { resourceMode: "external" },
  games: { th06: { ...game, package: { revision: "a".repeat(16), descriptor: "th06.package.json" } } },
});
assert.equal(external.shared.resourceMode, "external");
assert.throws(() => validateSiteManifest({
  ...hosted,
  shared: { ...hosted.shared, resourceMode: "external" },
  games: { th06: { ...game, package: { revision: "a".repeat(16), descriptor: "th06.package.json" } } },
}), /shared font URLs/);
assert.throws(() => validateSiteManifest({
  ...hosted,
  shared: { resourceMode: "external" },
}), /games/);
for (const features of [
  { thprac: "yes" },
  { focusHitbox: 1 },
  { replayManagement: true },
  { languages: true },
]) {
  assert.throws(() => validateSiteManifest({
    ...hosted,
    games: { th06: { ...game, features } },
  }), /games/);
}

for (const music of [
  { midi: { files: [1] } },
  { midi: { files: ["bad/path.mid"] } },
  { midi: { files: ["."] } },
  { midi: { files: ["track.ogg"] } },
  { midi: { files: ["x?y.mid"] } },
  { midi: { files: ["a.mid"], sizes: [0] } },
  { midi: { files: [], supported: "no" } },
  { midi: { files: [] }, ogg: { version: "legacy", files: [], sizes: [] } },
  { midi: { files: [] }, ogg: { version: `sha256-${"c".repeat(64)}`, files: [], sizes: [], sha256: [] } },
  { midi: { files: [] }, ogg: { version: `sha256-${"c".repeat(64)}`, files: ["a.ogg"], sizes: [1] } },
  { midi: { files: [] }, ogg: { version: `sha256-${"c".repeat(64)}`, files: ["a.mid"], sizes: [1], sha256: ["c".repeat(64)] } },
  { midi: { files: [] }, ogg: { version: `sha256-${"c".repeat(64)}`, files: ["a.ogg", "a.ogg"], sizes: [1, 1] } },
  { midi: { files: [] }, ogg: { version: `sha256-${"c".repeat(64)}`, files: ["a.ogg"], sizes: [1.5] } },
]) {
  assert.throws(() => validateSiteManifest({
    ...hosted,
    games: { th06: { ...game, music } },
  }), /games/);
}

assert.throws(() => validateSiteManifest({
  ...hosted,
  games: { th06: { ...game, gameData: { ...game.gameData, bytes: Number.MAX_SAFE_INTEGER + 1 } } },
}), /games/);

const importedGame = {
  ...game,
  offlineCompatibility: {
    schema: "eagler-touhou/offline-game-pack/1",
    runtimeCompatibility: {
      protocol: "eagler-touhou/1",
      dataLayout: game.gameData.layout,
      versionSource: "offline-pack",
    },
    requiredShared: ["/msgothic.ttc", "/unifont.otf"],
    languages: { source: "offline-pack", baseline: ["ja"] },
  },
};
assert.doesNotThrow(() => validateSiteManifest({
  schema: SITE_MANIFEST_SCHEMA,
  protocol: "eagler-touhou/1",
  profile: "web-validation-import",
  shared: { resourceMode: "import" },
  games: { th06: importedGame },
}));

console.log(JSON.stringify({ hostManifest: "PASS", subsetProducts: true }));

for (const testBuild of [false, true]) assert.equal(validateSiteManifest({...hosted, shared:{...hosted.shared,testBuild}}).shared.testBuild,testBuild);
for (const testBuild of ["true", "false", 0, 1, null]) assert.throws(()=>validateSiteManifest({...hosted,shared:{...hosted.shared,testBuild}}),/testBuild/);
