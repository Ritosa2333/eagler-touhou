import assert from "node:assert/strict";
import { strToU8, zipSync } from "fflate";
import {
  GAME_DATA_PACK_SCHEMA,
  OFFLINE_GAME_PACK_SCHEMA,
  parseStoredGameDataPack,
} from "../legacy/legacy-game-pack.mjs";
import {
  importedGameDataMetadataKey,
  importedOggMetadataKey,
  localGameDataCacheUrl,
  localOggCacheUrl,
  migrateLegacyStoredImport,
} from "../legacy/legacy-import-storage.mjs";
import { adaptLegacyGamePackToPackage, adaptLegacyStoredImportToPackage } from "../legacy/legacy-package-adapter.mjs";

const data = strToU8("legacy-data");
const dataHash = "11".repeat(32);
const layout = `sha256-${"22".repeat(32)}`;
const version = `sha256-${dataHash}`;

const basicManifest = {
  schema: GAME_DATA_PACK_SCHEMA,
  game: "th07",
  version,
  data: {
    path: "th07.data",
    layout,
    bytes: data.length,
    sha256: dataHash,
  },
};

const basicZip = zipSync({
  "manifest.json": strToU8(JSON.stringify(basicManifest)),
  "th07.data": data,
}, { level: 0 });
const basic = await parseStoredGameDataPack(new Blob([basicZip]));
assert.equal(basic.manifest.schema, GAME_DATA_PACK_SCHEMA);
assert.equal(basic.manifest.game, "th07");
assert.equal(basic.data.method, 0);
assert.equal(new TextDecoder().decode(await basic.data.blob.arrayBuffer()), "legacy-data");

const ogg = strToU8("legacy-ogg");
const oggHash = "99".repeat(32);
const oggManifest = {
  ...basicManifest,
  music: {
    mode: "ogg",
    version: `sha256-${oggHash}`,
    files: [{ path: "th07_01.ogg", bytes: ogg.length, sha256: oggHash }],
  },
};
const oggPack = await parseStoredGameDataPack(new Blob([zipSync({
  "manifest.json": strToU8(JSON.stringify(oggManifest)),
  "th07.data": data,
  "th07_01.ogg": ogg,
}, { level: 0 })]));
const adaptedOgg = adaptLegacyGamePackToPackage(oggPack, { protocol: "eagler-touhou/1" });
assert.equal(adaptedOgg.descriptor.files["ogg:th07_01.ogg"].target, "/bgm-ogg/th07_01.ogg");

const runtimeVersion = "0123456789abcdef";
const runtimeHtml = strToU8("<html></html>");
const runtimeJs = strToU8("console.log('legacy')");
const runtimeWasm = new Uint8Array([0, 97, 115, 109]);
const msgothic = strToU8("ttc");
const unifont = strToU8("otf");
const language = strToU8("language");
const hashes = ["33", "44", "55", "66", "77", "88"].map(byte => byte.repeat(32));
const offlineManifest = {
  ...basicManifest,
  schema: OFFLINE_GAME_PACK_SCHEMA,
  offline: {
    runtime: {
      version: runtimeVersion,
      files: [
        { role: "html", path: "offline/runtime/th07.html", bytes: runtimeHtml.length, sha256: hashes[0] },
        { role: "js", path: "offline/runtime/th07.js", bytes: runtimeJs.length, sha256: hashes[1] },
        { role: "wasm", path: "offline/runtime/th07.wasm", bytes: runtimeWasm.length, sha256: hashes[2] },
      ],
    },
    shared: [
      { target: "/msgothic.ttc", path: "offline/shared/msgothic.ttc", bytes: msgothic.length, sha256: hashes[3] },
      { target: "/unifont.otf", path: "offline/shared/unifont.otf", bytes: unifont.length, sha256: hashes[4] },
    ],
    languages: [
      {
        id: "lang_zh-hans",
        title: "中文（简体）",
        path: "offline/languages/lang_zh-hans.zip",
        bytes: language.length,
        sha256: hashes[5],
        runtimeVersion,
      },
    ],
  },
};
const offlineZip = zipSync({
  "manifest.json": strToU8(JSON.stringify(offlineManifest)),
  "th07.data": data,
  "offline/runtime/th07.html": runtimeHtml,
  "offline/runtime/th07.js": runtimeJs,
  "offline/runtime/th07.wasm": runtimeWasm,
  "offline/shared/msgothic.ttc": msgothic,
  "offline/shared/unifont.otf": unifont,
  "offline/languages/lang_zh-hans.zip": language,
}, { level: 0 });
const offline = await parseStoredGameDataPack(new Blob([offlineZip]));
assert.equal(offline.manifest.schema, OFFLINE_GAME_PACK_SCHEMA);
assert.equal(offline.offline.runtime.version, runtimeVersion);
assert.equal(offline.offline.runtime.files.length, 3);
assert.deepEqual(offline.offline.shared.map(file => file.target), ["/msgothic.ttc", "/unifont.otf"]);
assert.equal(offline.offline.languages[0].id, "lang_zh-hans");

const memory = new Map();
const storage = {
  getItem: key => memory.has(key) ? memory.get(key) : null,
  setItem: (key, value) => memory.set(key, String(value)),
  removeItem: key => memory.delete(key),
};
const origin = "https://example.test";
const storedDataMeta = {
  source: "local-import",
  game: "th07",
  version,
  layout,
  sha256: dataHash,
  bytes: data.length,
  legacyAssets: {
    runtimeVersion,
    shared: [
      { target: "/msgothic.ttc", key: "/.eagler-local/offline/th07/0123456789abcdef/shared/msgothic.ttc", bytes: msgothic.length, sha256: hashes[3] },
      { target: "/unifont.otf", key: "/.eagler-local/offline/th07/0123456789abcdef/shared/unifont.otf", bytes: unifont.length, sha256: hashes[4] },
    ],
    languages: [
      { id: "lang_zh-hans", title: "中文（简体）", key: "/.eagler-local/offline/th07/0123456789abcdef/languages/lang_zh-hans.zip", bytes: language.length, sha256: hashes[5] },
    ],
  },
};
const oggVersion = `sha256-${"99".repeat(32)}`;
const storedOggMeta = { source: "local-import", game: "th07", version: oggVersion, files: ["th07_01.ogg"] };
storage.setItem(importedGameDataMetadataKey("th07"), JSON.stringify(storedDataMeta));
storage.setItem(importedOggMetadataKey("th07"), JSON.stringify(storedOggMeta));
const cached = new Map([
  [localGameDataCacheUrl(origin, "th07", version), new Blob([data])],
  [localOggCacheUrl(origin, "th07", oggVersion, "th07_01.ogg"), new Blob([strToU8("ogg")])],
  [storedDataMeta.legacyAssets.shared[0].key, new Blob([msgothic])],
  [storedDataMeta.legacyAssets.shared[1].key, new Blob([unifont])],
  [storedDataMeta.legacyAssets.languages[0].key, new Blob([language])],
]);
const cacheStorage = {
  match: async key => cached.has(key) ? new Response(cached.get(key)) : undefined,
  open: async () => ({ delete: async key => cached.delete(key) }),
  delete: async () => true,
};
let installedMigration = null;
const migration = await migrateLegacyStoredImport("th07", {
  protocol: "eagler-touhou/1",
  origin,
  storage,
  indexedDBFactory: null,
  cacheStorage,
  install: async parsed => {
    installedMigration = parsed;
    return { generation: { descriptor: parsed.descriptor } };
  },
});
assert.equal(migration.status, "migrated");
assert.equal(installedMigration.descriptor.schema, "eagler-touhou/package/1");
assert.equal(installedMigration.descriptor.runtimeRequirement.dataLayout, layout);
assert.deepEqual(installedMigration.descriptor.components.ogg.files, ["ogg:th07_01.ogg"]);
assert.equal(installedMigration.descriptor.components.language.entries[0].id, "lang_zh-hans");
assert.equal(storage.getItem(importedGameDataMetadataKey("th07")), null);
assert.equal(storage.getItem(importedOggMetadataKey("th07")), null);
assert.equal(cached.size, 0);

storage.setItem(importedGameDataMetadataKey("th07"), JSON.stringify({ ...storedDataMeta, legacyAssets: null }));
cached.set(localGameDataCacheUrl(origin, "th07", version), new Blob([data]));
let repeatedInstall = false;
const alreadyCurrent = await migrateLegacyStoredImport("th07", {
  protocol: "eagler-touhou/1",
  origin,
  storage,
  indexedDBFactory: null,
  cacheStorage,
  currentRevision: `legacy-${dataHash.slice(0, 16)}-noogg-data`,
  install: async () => { repeatedInstall = true; },
});
assert.equal(alreadyCurrent.status, "already-current");
assert.equal(repeatedInstall, false);
assert.equal(storage.getItem(importedGameDataMetadataKey("th07")), null);

const adapted = adaptLegacyGamePackToPackage(offline, { protocol: "eagler-touhou/1" });
assert.equal(adapted.descriptor.schema, "eagler-touhou/package/1");
assert.equal(adapted.descriptor.game, "th07");
assert.deepEqual(adapted.descriptor.base.files, ["game-data", "shared-msgothic", "shared-unifont"]);
assert.equal(adapted.descriptor.components.language.entries[0].id, "lang_zh-hans");
assert.ok(adapted.files.has("game-data"));
assert.ok(adapted.files.has("shared-msgothic"));
assert.ok(adapted.files.has("language:lang_zh-hans"));
assert.ok(![...adapted.files.values()].some(file => /\.(?:html|m?js|wasm)$/i.test(file.declaration.source)),
  "legacy executable Runtime files must not cross into Package Store");

const th10MissingSharedPack = {
  manifest: {
    ...basicManifest,
    game: "th10",
    data: { ...basicManifest.data, path: "th10.data" },
  },
  data: { blob: new Blob([data]) },
  music: [],
  offline: { runtime: { version: runtimeVersion }, shared: [], languages: [] },
};
assert.throws(
  () => adaptLegacyGamePackToPackage(th10MissingSharedPack, { protocol: "eagler-touhou/1" }),
  /missing required shared resources: \/msgothic\.ttc, \/unifont\.otf/,
);

assert.throws(
  () => adaptLegacyStoredImportToPackage({
    gameData: {
      game: "th10",
      version,
      layout,
      sha256: dataHash,
      bytes: data.length,
      legacyAssets: null,
    },
    dataKey: "legacy-th10-data",
    assets: new Map([["legacy-th10-data", new Blob([data])]]),
    ogg: null,
  }, { protocol: "eagler-touhou/1", origin }),
  /missing required shared resources: \/msgothic\.ttc, \/unifont\.otf/,
);

const compressed = zipSync({
  "manifest.json": strToU8(JSON.stringify(basicManifest)),
  "th07.data": data,
}, { level: 9 });
await assert.rejects(parseStoredGameDataPack(new Blob([compressed])), /ZIP entry must use STORE/);

console.log(JSON.stringify({
  legacyGamePack: "PASS",
  readCompatibility: [GAME_DATA_PACK_SCHEMA, OFFLINE_GAME_PACK_SCHEMA],
  producer: "retired",
  newImports: "package-store",
  storedMigration: "one-way",
}));
