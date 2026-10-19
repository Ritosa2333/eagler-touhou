import assert from "node:assert/strict";
import {
  InstalledGameDataError,
  managedRuntimeUrl,
  readManagedRuntimeData,
  readManagedRuntimeResource,
} from "../.cache/build/browser/assets/launcher/runtime-preparation.mjs";

function bytes(...values) {
  return new Uint8Array(values).buffer;
}

function generation({ files, installed, dataFile } = {}) {
  return {
    id: "gen-test",
    game: "th07",
    descriptor: {
      files,
      ...(dataFile ? { runtimeRequirement: { dataFile } } : {}),
    },
    files: installed,
  };
}

const explicitBuffer = bytes(1, 2, 3, 4);
const explicit = generation({
  dataFile: "runtime-data",
  files: {
    "runtime-data": { target: "/custom.data", bytes: 4 },
    "game-data": { target: "/fallback.data", bytes: 4 },
  },
  installed: {
    "runtime-data": { objectId: "obj-explicit" },
    "game-data": { objectId: "obj-fallback" },
  },
});
const explicitResult = await readManagedRuntimeData(explicit, {
  readObject: async id => id === "obj-explicit" ? { data: explicitBuffer } : { data: bytes(9, 9, 9, 9) },
});
assert.equal(explicitResult.fileId, "runtime-data", "runtimeRequirement.dataFile has first priority");
assert.deepEqual([...new Uint8Array(explicitResult.buffer)], [1, 2, 3, 4]);
assert.notEqual(explicitResult.buffer, explicitBuffer, "runtime reads must not expose the Package Store ArrayBuffer by reference");

const gameDataFallback = generation({
  files: {
    "game-data": { target: "/th07.data", bytes: 3 },
    other: { target: "/other.data", bytes: 2 },
  },
  installed: {
    "game-data": { objectId: "obj-game" },
    other: { objectId: "obj-other" },
  },
});
assert.equal((await readManagedRuntimeData(gameDataFallback, {
  readObject: async id => ({ data: id === "obj-game" ? bytes(5, 6, 7) : bytes(8, 9) }),
})).fileId, "game-data", "canonical game-data wins before target inference");

const inferred = generation({
  files: {
    payload: { target: "/nested/th06.data", bytes: 2 },
    font: { target: "/shared/font.otf", bytes: 1 },
  },
  installed: {
    payload: { objectId: "obj-data" },
    font: { objectId: "obj-font" },
  },
});
assert.equal((await readManagedRuntimeData(inferred, {
  readObject: async id => ({ data: id === "obj-data" ? bytes(4, 2) : bytes(1) }),
})).fileId, "payload", "a unique installed .data target is the final compatibility fallback");

const legacyBlob = generation({
  files: { "game-data": { target: "/th07.data", bytes: 3 } },
  installed: { "game-data": { objectId: "obj-blob" } },
});
assert.deepEqual([...new Uint8Array((await readManagedRuntimeData(legacyBlob, {
  readObject: async () => ({ blob: new Blob([new Uint8Array([7, 8, 9])]) }),
})).buffer)], [7, 8, 9], "legacy Blob-backed Package objects remain readable");

await assert.rejects(
  readManagedRuntimeData(generation({
    files: { a: { target: "/a.data" }, b: { target: "/b.data" } },
    installed: { a: { objectId: "obj-a" }, b: { objectId: "obj-b" } },
  }), { readObject: async () => ({ data: bytes(1) }) }),
  /exactly one installed DATA file/,
  "ambiguous inferred DATA must fail instead of guessing",
);
await assert.rejects(
  readManagedRuntimeData(legacyBlob, { readObject: async () => null }),
  InstalledGameDataError,
);
const unavailableStore = new Error("Package Store open blocked");
await assert.rejects(readManagedRuntimeData(legacyBlob, { readObject: () => new Promise(() => {}), timeoutMs: 10 }),
  error => !(error instanceof InstalledGameDataError) && /读取超时/.test(error.message),
  "a stalled local read must terminate without claiming the DATA is absent");
await assert.rejects(readManagedRuntimeData(legacyBlob, { readObject: async () => { throw unavailableStore; } }),
  error => error === unavailableStore && !(error instanceof InstalledGameDataError),
  "storage availability errors must not claim that installed DATA is missing");
await assert.rejects(readManagedRuntimeData({ ...explicit, files: { "game-data": { objectId: "obj-fallback" } } }, {
  readObject: async () => ({ data: bytes(1, 2, 3, 4) }),
}), InstalledGameDataError, "a missing explicitly required DATA cannot fall through to a different installed file");
await assert.rejects(
  readManagedRuntimeData(legacyBlob, { readObject: async () => ({ data: bytes(1, 2) }) }),
  /Installed game DATA size mismatch: 2\/3/,
  "declared DATA bytes remain a commit/read integrity gate",
);
await assert.rejects(
  readManagedRuntimeData(legacyBlob, { readObject: async () => ({}) }),
  /Installed game DATA cannot be read/,
);

const resourceGeneration = generation({
  files: {
    lang: { target: "/thcrap/th07/strings.js", bytes: 3 },
  },
  installed: {
    lang: { objectId: "obj-lang" },
  },
});
const resource = await readManagedRuntimeResource(resourceGeneration, "lang", {
  readObject: async () => ({ data: bytes(3, 2, 1) }),
});
assert.equal(resource.path, "/thcrap/th07/strings.js");
assert.equal(resource.bytes, 3);
assert.deepEqual([...new Uint8Array(resource.buffer)], [3, 2, 1]);
assert.equal(await readManagedRuntimeResource(resourceGeneration, "missing", { readObject: async () => ({ data: bytes(1) }) }), null);
assert.equal(await readManagedRuntimeResource(resourceGeneration, "lang", { readObject: async () => null }), null);
await assert.rejects(
  readManagedRuntimeResource(resourceGeneration, "lang", { readObject: async () => ({ data: bytes(1, 2) }) }),
  /lang: installed resource size mismatch/,
);

const runtimeHref = managedRuntimeUrl(
  "./runtime/th07/th07.html?existing=1",
  { id: "gen-123", game: "th07" },
  "multiplayer",
  "https://example.test/eagler-touhou/",
);
const runtimeUrl = new URL(runtimeHref);
assert.equal(runtimeUrl.pathname, "/eagler-touhou/runtime/th07/th07.html");
assert.equal(runtimeUrl.searchParams.get("existing"), "1");
assert.equal(runtimeUrl.searchParams.get("hosted"), "1");
assert.equal(runtimeUrl.searchParams.get("managedData"), "1");
assert.equal(runtimeUrl.searchParams.get("gameGeneration"), "gen-123");
assert.equal(runtimeUrl.searchParams.get("runtimeVariant"), "multiplayer");
assert.equal(new URL(managedRuntimeUrl("runtime.html", { id: "gen-1", game: "th06" }, "", "https://example.test/")).searchParams.get("runtimeVariant"), "normal");
assert.throws(() => managedRuntimeUrl("", { id: "gen-1", game: "th06" }), /Managed Runtime URL is unavailable/);
assert.throws(() => managedRuntimeUrl("runtime.html", { id: "", game: "th06" }), /Package generation is unavailable/);

console.log(JSON.stringify({ runtimePreparation: "PASS", dataSelection: ["declared", "game-data", "unique-target"], storage: ["arraybuffer", "legacy-blob"] }));
