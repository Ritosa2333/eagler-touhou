import { PACKAGE_DESCRIPTOR_SCHEMA, validatePackageDescriptor } from "../package/package-descriptor.mjs";
import { PRODUCT_GAMES } from "../product-catalog.mjs";
import { localOggCacheUrl } from "./legacy-import-storage.mjs";

function normalizedMount(value) {
  if (typeof value !== "string" || !value.startsWith("/")) return null;
  return value === "/" ? "" : value.replace(/\/$/, "");
}

// Adapts already-persisted historical import state into the same Package
// shape. This is the retirement bridge for pre-Package-Store browser data;
// callers should delete the historical storage only after Package commit.
export function adaptLegacyStoredImportToPackage(state, { protocol, origin } = {}) {
  if (!state?.gameData || !(state.assets instanceof Map) || typeof protocol !== "string" || !protocol ||
      typeof origin !== "string" || !origin) {
    throw new Error("invalid legacy stored import adaptation request");
  }
  const { gameData, ogg } = state;
  const game = gameData.game;
  const files = {};
  const parsedFiles = new Map();
  const baseFiles = [];
  const components = {};
  const declare = (fileId, { source, target, key, bytes, revision, sha256 = null }) => {
    const blob = state.assets.get(key);
    if (!(blob instanceof Blob) || (bytes && blob.size !== bytes)) throw new Error(`${fileId}: legacy stored payload is missing or damaged`);
    const declaration = { revision, source, target, bytes: blob.size, ...(sha256 ? { sha256: sha256.toLowerCase() } : {}) };
    files[fileId] = declaration;
    parsedFiles.set(fileId, { fileId, declaration, blob, bytes: blob.size });
    return fileId;
  };

  baseFiles.push(declare("game-data", {
    source: PRODUCT_GAMES[game].package.dataTarget.slice(1),
    target: PRODUCT_GAMES[game].package.dataTarget,
    key: state.dataKey,
    bytes: gameData.bytes,
    revision: revisionFromSha256(gameData.sha256),
    sha256: gameData.sha256,
  }));
  for (const item of gameData.legacyAssets?.shared || []) {
    const fileId = item.target === "/msgothic.ttc" ? "shared-msgothic" : item.target === "/unifont.otf" ? "shared-unifont" : `legacy-shared:${baseFiles.length}`;
    baseFiles.push(declare(fileId, {
      source: `legacy/shared/${item.target.slice(1)}`,
      target: item.target,
      key: item.key,
      bytes: item.bytes,
      revision: revisionFromSha256(item.sha256),
      sha256: item.sha256,
    }));
  }

  if (ogg?.files?.length) {
    const mount = normalizedMount(PRODUCT_GAMES[game]?.package?.musicMounts?.ogg);
    if (mount == null) throw new Error("legacy stored OGG requires a declared music mount");
    const identity = ogg.version.replace(/^sha256-/i, "").slice(0, 16).toLowerCase();
    const oggFiles = [];
    for (let index = 0; index < ogg.files.length; index++) {
      const name = ogg.files[index];
      const fileId = `ogg:${name}`;
      oggFiles.push(declare(fileId, {
        source: name,
        target: `${mount}/${name}`,
        key: localOggCacheUrl(origin, game, ogg.version, name),
        revision: `legacy-${identity}-${index}`,
      }));
    }
    components.ogg = { type: "ogg", files: oggFiles };
  }

  const languageEntries = [];
  for (const item of gameData.legacyAssets?.languages || []) {
    const fileId = `language:${item.id}`;
    declare(fileId, {
      source: `legacy/languages/${item.id}.zip`,
      target: `/__eagler/language/${item.id}.zip`,
      key: item.key,
      bytes: item.bytes,
      revision: revisionFromSha256(item.sha256),
      sha256: item.sha256,
    });
    languageEntries.push({ id: item.id, title: item.title, file: fileId });
  }
  if (languageEntries.length) components.language = { type: "language", entries: languageEntries };

  const dataIdentity = gameData.sha256.toLowerCase().slice(0, 16);
  const musicIdentity = ogg?.version?.replace(/^sha256-/i, "").slice(0, 8).toLowerCase() || "noogg";
  const offlineIdentity = gameData.legacyAssets?.runtimeVersion?.slice(0, 8).toLowerCase() || "data";
  const descriptor = {
    schema: PACKAGE_DESCRIPTOR_SCHEMA,
    game,
    revision: `legacy-${dataIdentity}-${musicIdentity}-${offlineIdentity}`,
    runtimeRequirement: {
      protocol,
      target: game,
      dataFile: "game-data",
      dataLayout: gameData.layout,
    },
    files,
    base: { files: baseFiles },
    components,
  };
  validatePackageDescriptor(descriptor);
  return { descriptor, files: parsedFiles, legacy: true, storedMigration: true };
}

function revisionFromSha256(sha256) {
  if (typeof sha256 !== "string" || !/^[a-f0-9]{64}$/i.test(sha256)) throw new Error("legacy package file SHA-256 is invalid");
  return sha256.toLowerCase().slice(0, 16);
}

// Adapts a parsed historical game-data/offline-game ZIP into the same parsed
// Package shape consumed by installParsedPackageZip(). Historical executable
// Runtime files are intentionally excluded: the current App-managed Runtime is
// always authoritative.
export function adaptLegacyGamePackToPackage(pack, { protocol } = {}) {
  if (!pack?.manifest || !pack?.data?.blob || typeof protocol !== "string" || !protocol) {
    throw new Error("invalid legacy game pack adaptation request");
  }
  const manifest = pack.manifest;
  const files = {};
  const parsedFiles = new Map();
  const baseFiles = [];
  const components = {};

  const declare = (fileId, { source, target, blob, bytes, sha256 }) => {
    if (!(blob instanceof Blob)) throw new Error(`${fileId}: legacy package payload is missing`);
    const declaration = {
      revision: revisionFromSha256(sha256),
      source,
      target,
      bytes,
      sha256: sha256.toLowerCase(),
    };
    files[fileId] = declaration;
    parsedFiles.set(fileId, { fileId, declaration, blob, bytes: blob.size });
    return fileId;
  };

  baseFiles.push(declare("game-data", {
    source: manifest.data.path,
    target: `/${manifest.data.path}`,
    blob: pack.data.blob,
    bytes: manifest.data.bytes,
    sha256: manifest.data.sha256,
  }));

  for (const item of pack.offline?.shared || []) {
    const fileId = item.target === "/msgothic.ttc" ? "shared-msgothic"
      : item.target === "/unifont.otf" ? "shared-unifont"
      : `legacy-shared:${baseFiles.length}`;
    baseFiles.push(declare(fileId, {
      source: item.path,
      target: item.target,
      blob: item.blob,
      bytes: item.bytes,
      sha256: item.sha256,
    }));
  }

  if (pack.music?.length) {
    const mount = normalizedMount(PRODUCT_GAMES[manifest.game]?.package?.musicMounts?.ogg);
    if (mount == null) throw new Error("legacy OGG package requires a declared music mount");
    const oggFiles = [];
    for (const item of pack.music) {
      const fileId = `ogg:${item.name}`;
      oggFiles.push(declare(fileId, {
        source: item.name,
        target: `${mount}/${item.name}`,
        blob: item.blob,
        bytes: item.uncompressedSize,
        sha256: item.sha256,
      }));
    }
    components.ogg = { type: "ogg", files: oggFiles };
  }

  const languageEntries = [];
  for (const item of pack.offline?.languages || []) {
    const fileId = `language:${item.id}`;
    declare(fileId, {
      source: item.path,
      target: `/__eagler/language/${item.id}.zip`,
      blob: item.blob,
      bytes: item.bytes,
      sha256: item.sha256,
    });
    languageEntries.push({ id: item.id, title: item.title, file: fileId });
  }
  if (languageEntries.length) components.language = { type: "language", entries: languageEntries };

  const dataIdentity = manifest.data.sha256.toLowerCase().slice(0, 16);
  const musicIdentity = manifest.music?.version?.replace(/^sha256-/i, "").slice(0, 8) || "noogg";
  const offlineIdentity = pack.offline?.runtime?.version?.slice(0, 8) || "data";
  const descriptor = {
    schema: PACKAGE_DESCRIPTOR_SCHEMA,
    game: manifest.game,
    revision: `legacy-${dataIdentity}-${musicIdentity}-${offlineIdentity}`,
    runtimeRequirement: {
      protocol,
      target: manifest.game,
      dataFile: "game-data",
      dataLayout: manifest.data.layout,
    },
    files,
    base: { files: baseFiles },
    components,
  };
  validatePackageDescriptor(descriptor);
  return { descriptor, files: parsedFiles, legacy: true };
}
