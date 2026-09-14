import assert from "node:assert/strict";
import {
  HOST_MANIFEST_FILE,
  HOST_MANIFEST_SCHEMA,
  hostOriginMigrationAvailable,
  validateHostManifest,
} from "../lib/contracts/host-manifest.mjs";

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

const hosted = validateHostManifest({
  schema: HOST_MANIFEST_SCHEMA,
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
assert.equal(HOST_MANIFEST_FILE, "host-manifest.json");
assert.equal(hostOriginMigrationAvailable(hosted, "https:"), false);

const migrationHost = validateHostManifest({
  ...hosted,
  shared: { ...hosted.shared, originMigration: { mode: "http-to-https" } },
});
assert.equal(hostOriginMigrationAvailable(migrationHost, "https:"), true);
assert.equal(hostOriginMigrationAvailable(migrationHost, "http:"), false);
for (const originMigration of [true, "http-to-https", {}, { mode: "https-only" }]) {
  assert.throws(() => validateHostManifest({
    ...hosted,
    shared: { ...hosted.shared, originMigration },
  }), /originMigration/);
}

assert.throws(() => validateHostManifest({ ...hosted, games: { th99: game } }), /games/);
assert.throws(() => validateHostManifest({ ...hosted, shared: { ...hosted.shared, resourceMode: "import-partial" } }), /resource mode/);
assert.throws(() => validateHostManifest({ ...hosted, shared: { ...hosted.shared, netplayRelay: "https://example.invalid/" } }), /netplayRelay/);
assert.doesNotThrow(() => validateHostManifest({
  ...hosted,
  games: { th06: { ...game, features: undefined } },
}));
assert.doesNotThrow(() => validateHostManifest({ ...hosted, games: { th06: gameWithLanguages } }));
for (const broken of [
  { ...gameWithLanguages, languageOptions: gameWithLanguages.languageOptions.map(item => item.id === "lang_en"
    ? { ...item, pack: { ...languagePack, url: "games/th06/language/missing.zip" } } : item) },
  { ...gameWithLanguages, languageOptions: [{ id: "ja", pack: null }] },
  { ...gameWithLanguages, languages: [...gameWithLanguages.languages, gameWithLanguages.languages[0]] },
  { ...gameWithLanguages, languageOptions: [{ id: "ja", pack: languagePack }, gameWithLanguages.languageOptions[1]] },
]) {
  assert.throws(() => validateHostManifest({ ...hosted, games: { th06: broken } }), /games/);
}

const external = validateHostManifest({
  ...hosted,
  shared: { resourceMode: "external" },
  games: { th06: { ...game, package: { revision: "a".repeat(16), descriptor: "th06.package.json" } } },
});
assert.equal(external.shared.resourceMode, "external");
assert.throws(() => validateHostManifest({
  ...hosted,
  shared: { ...hosted.shared, resourceMode: "external" },
  games: { th06: { ...game, package: { revision: "a".repeat(16), descriptor: "th06.package.json" } } },
}), /shared font URLs/);
assert.throws(() => validateHostManifest({
  ...hosted,
  shared: { resourceMode: "external" },
}), /games/);
for (const features of [
  { thprac: "yes" },
  { focusHitbox: 1 },
  { replayManagement: true },
  { languages: true },
]) {
  assert.throws(() => validateHostManifest({
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
  { midi: { files: [] }, ogg: { version: "legacy", files: [], sizes: [] } },
  { midi: { files: [] }, ogg: { version: `sha256-${"c".repeat(64)}`, files: [], sizes: [], sha256: [] } },
  { midi: { files: [] }, ogg: { version: `sha256-${"c".repeat(64)}`, files: ["a.ogg"], sizes: [1] } },
  { midi: { files: [] }, ogg: { version: `sha256-${"c".repeat(64)}`, files: ["a.mid"], sizes: [1], sha256: ["c".repeat(64)] } },
  { midi: { files: [] }, ogg: { version: `sha256-${"c".repeat(64)}`, files: ["a.ogg", "a.ogg"], sizes: [1, 1] } },
  { midi: { files: [] }, ogg: { version: `sha256-${"c".repeat(64)}`, files: ["a.ogg"], sizes: [1.5] } },
]) {
  assert.throws(() => validateHostManifest({
    ...hosted,
    games: { th06: { ...game, music } },
  }), /games/);
}

assert.throws(() => validateHostManifest({
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
assert.doesNotThrow(() => validateHostManifest({
  schema: HOST_MANIFEST_SCHEMA,
  protocol: "eagler-touhou/1",
  profile: "web-validation-import",
  shared: { resourceMode: "import" },
  games: { th06: importedGame },
}));

console.log(JSON.stringify({ hostManifest: "PASS", subsetProducts: true }));

for (const testBuild of [false, true]) assert.equal(validateHostManifest({...hosted, shared:{...hosted.shared,testBuild}}).shared.testBuild,testBuild);
for (const testBuild of ["true", "false", 0, 1, null]) assert.throws(()=>validateHostManifest({...hosted,shared:{...hosted.shared,testBuild}}),/testBuild/);
