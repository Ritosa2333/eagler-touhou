import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";
import { createHash, webcrypto } from "node:crypto";
import vm from "node:vm";
const source = await readFile(new URL("../src/app-shell-sw.js", import.meta.url), "utf8");
const sha = text => createHash("sha256").update(text).digest("hex");
class MemoryCache {
  entries = new Map();
  key(input) { return typeof input === "string" ? input : input.url; }
  async put(input, response) { this.entries.set(this.key(input), response.clone()); }
  async match(input) { return this.entries.get(this.key(input))?.clone(); }
  async keys() { return [...this.entries.keys()].map(url => new Request(url)); }
}
function harness({ stores = new Map(), scope = "https://example.test/", build = "test", files = {}, deferredPaths = [], fetchImpl,
  clientIds = ["client-1"] } = {}) {
  const listeners = new Map();
  const requests = [];
  let skipped = 0, claimed = 0;
  const caches = {
    async keys() { return [...stores.keys()]; },
    async open(name) { if (!stores.has(name)) stores.set(name, new MemoryCache()); return stores.get(name); },
    async delete(name) { return stores.delete(name); },
  };
  const self = {
    __WB_MANIFEST: Object.entries(files).map(([url, body]) => ({ url, revision: sha(body) })),
    registration: { scope },
    addEventListener(type, callback) { listeners.set(type, callback); },
    async skipWaiting() { skipped++; }, clients: {
      async claim() { claimed++; },
      async matchAll() { return clientIds.map(id => ({ id, url: new URL(id, scope).href })); },
    },
  };
  vm.runInNewContext(source.replaceAll("__APP_SHELL_BUILD_ID__", build)
    .replace("__APP_SHELL_DEFERRED_PATHS__", JSON.stringify(deferredPaths)), {
    self, caches, URL, Request, Response, Date, Promise, console,
    crypto: webcrypto, AbortController, setTimeout, clearTimeout, Uint8Array,
    fetch: async request => {
      requests.push(request.url);
      if (fetchImpl) return fetchImpl(request);
      const path = new URL(request.url).pathname.slice(new URL(scope).pathname.length) || "./";
      return new Response(files[path] ?? "not found", { status: path in files ? 200 : 404 });
    },
  });
  const lifecycle = async type => {
    let task;
    listeners.get(type)({ waitUntil(value) { task = value; } });
    await task;
  };
  const request = async path => {
    let task;
    listeners.get("fetch")({ request: new Request(new URL(path, scope)), respondWith(value) { task = value; } });
    return task ? await task : null;
  };
  const message = async (data, sourceId = "client-1") => {
    let response, task;
    listeners.get("message")({ data, source: { id: sourceId }, ports: [{ postMessage(value) { response = value; } }], waitUntil(value) { task = value; } });
    await task; return JSON.parse(JSON.stringify(response));
  };
  const meta = async () => (await request("__app-shell-update-status__")).json();
  return { stores, caches, requests, lifecycle, request, message, meta, forced: () => ({ skipped, claimed }) };
}
const first = harness({ files: { "./": "home" } });
await first.lifecycle("install"); await first.lifecycle("activate");
assert.equal((await first.meta()).updated, false);
assert.equal((await first.meta()).appliedAt, null);
assert.deepEqual(first.forced(), { skipped: 0, claimed: 0 });
assert.equal(await (await first.request("./?game=th06")).text(), "home");
assert.equal(await first.request("unpublished"), null);
assert.deepEqual(await first.message({ type: "CHECK_APP_SHELL_ACTIVATION" }), { ok: true, soleClient: true });
assert.deepEqual(await first.message({ type: "ACTIVATE_APP_SHELL" }), { ok: true, activating: true });
assert.deepEqual(first.forced(), { skipped: 1, claimed: 0 });
const crowded = harness({ files: { "./": "home" }, clientIds: ["client-1", "client-2"] });
assert.deepEqual(await crowded.message({ type: "CHECK_APP_SHELL_ACTIVATION" }), { ok: true, soleClient: true });
assert.deepEqual(await crowded.message({ type: "ACTIVATE_APP_SHELL" }), { ok: true, activating: true });
assert.deepEqual(crowded.forced(), { skipped: 1, claimed: 0 });
const firefoxBlank = harness({ files: { "./": "home" }, clientIds: ["client-1", "about:blank"] });
assert.deepEqual(await firefoxBlank.message({ type: "CHECK_APP_SHELL_ACTIVATION" }), { ok: true, soleClient: true });

const runtime = "runtime/th06/game.wasm";
const a = harness({ files: { "./": "a", [runtime]: "wasm-a" }, deferredPaths: [runtime], build: "a" });
await a.lifecycle("install");
assert.equal(a.requests.length, 1, "unused Runtimes stay lazy");
assert.equal((await a.message({ type: "CACHE_APP_SHELL_PATHS", paths: [runtime] })).ok, true);
assert.deepEqual((await a.message({ type: "GET_APP_SHELL_STATUS" })).runtimeGroups, ["runtime/th06/"]);
const extra = "runtime/th06/new-helper.js";
const unused = Object.fromEntries(["th07", "th08", "th10"].map(game => [`runtime/${game}/game.wasm`, game]));
const b = harness({ stores: a.stores,
  files: { "./": "b", [runtime]: "wasm-b", [extra]: "helper", ...unused },
  deferredPaths: [runtime, extra, ...Object.keys(unused)], build: "b" });
await b.lifecycle("install");
await b.lifecycle("activate");
assert.deepEqual(b.requests, ["https://example.test/"], "updates do not prewarm even previously used Runtimes or new dependencies");
const applied = await b.meta();
assert.equal(applied.updated, true); assert.ok(applied.appliedAt > 0);
assert.equal((await b.meta()).appliedAt, applied.appliedAt);
assert.equal(applied.install.deferred, 5);
assert.deepEqual(applied.runtimeTrusted, {});
assert.deepEqual((await b.message({ type: "GET_APP_SHELL_STATUS" })).runtimeGroups, []);
const bCache = await b.caches.open("eagler-touhou-app-shell-%2F-b");
for (const path of [runtime, extra, ...Object.keys(unused)]) {
  assert.equal(await bCache.match(`https://example.test/${path}`), undefined, "install does not copy deferred Runtime bytes either");
}
assert.equal((await b.message({ type: "CACHE_APP_SHELL_PATHS", paths: [runtime, extra] })).ok, true);
assert.deepEqual((await b.message({ type: "GET_APP_SHELL_STATUS" })).runtimeGroups, ["runtime/th06/"]);
assert.deepEqual(b.requests, ["https://example.test/", `https://example.test/${runtime}`, `https://example.test/${extra}`],
  "explicit preparation fetches only the selected Runtime");
assert.equal(await (await a.request(runtime)).text(), "wasm-a", "a new preparation leaves the old worker's bytes intact");
assert.equal((await b.message({ type: "CACHE_APP_SHELL_PATHS", paths: ["https://other.test/x"] })).ok, false);
assert.equal((await b.message({ type: "CACHE_APP_SHELL_PATHS", paths: ["missing"] })).ok, false);

const reused = harness({ stores: b.stores, build: "reuse", files: { "./": "b", [runtime]: "wasm-b", [extra]: "helper" }, deferredPaths: [runtime, extra], fetchImpl: () => { throw new Error("must reuse"); } });
await reused.lifecycle("install");
assert.equal(reused.requests.length, 0);
assert.deepEqual((await reused.message({ type: "GET_APP_SHELL_STATUS" })).runtimeGroups, [], "replacements do not copy warm Runtime bytes");

const foreign = harness({ stores: a.stores, scope: "https://example.test/nested/", build: "foreign", files: { "./": "nested" } });
await foreign.lifecycle("install");
await foreign.lifecycle("activate");
assert.equal(await (await b.request(runtime)).text(), "wasm-b", "nested cleanup cannot evict the root scope");
await reused.lifecycle("activate");
assert.equal(await (await foreign.request("./")).text(), "nested", "root cleanup cannot evict a nested scope");

const before = [...a.stores.keys()].sort();
const corrupt = harness({ stores: a.stores, build: "corrupt", files: { "./": "c", "script.js": "good" }, fetchImpl: async () => new Response("HTML fallback") });
await assert.rejects(corrupt.lifecycle("install"), /integrity mismatch/);
assert.deepEqual([...a.stores.keys()].sort(), before, "a failed candidate is removed without touching committed caches");
const interrupted = harness({ stores: a.stores, build: "interrupted", files: { "./": "c", "script.js": "new" }, fetchImpl: async request => {
  if (request.url.endsWith("script.js")) { await new Promise(resolve => setTimeout(resolve, 15)); return new Response("new"); }
  return new Response("failed", { status: 503 });
} });
await assert.rejects(interrupted.lifecycle("install"), /HTTP 503/);
assert.deepEqual([...a.stores.keys()].sort(), before, "all writers drain before failed-cache cleanup");

// A replacement must not download even a never-used Runtime. Its absence or
// server failure is not a Launcher installation failure.
const lazyA = harness({ build: "lazy-a", files: { "./": "a", [runtime]: "a" }, deferredPaths: [runtime] });
await lazyA.lifecycle("install");
const lazyB = harness({ stores: lazyA.stores, build: "lazy-b", files: { "./": "b", [runtime]: "b" }, deferredPaths: [runtime],
  fetchImpl: async request => new Response(request.url.endsWith("game.wasm") ? "unavailable" : "b",
    { status: request.url.endsWith("game.wasm") ? 503 : 200 }) });
await lazyB.lifecycle("install");
await lazyB.lifecycle("activate");
assert.deepEqual(lazyB.requests, ["https://example.test/"]);
assert.deepEqual((await lazyB.message({ type: "GET_APP_SHELL_STATUS" })).runtimeGroups, []);
assert.equal((await lazyB.message({ type: "CACHE_APP_SHELL_PATHS", paths: [runtime] })).ok, false,
  "the unavailable Runtime fails only when explicitly requested, not during the shell update");
assert.equal(await (await lazyB.request("./")).text(), "b");

const legacyStores = new Map();
const legacy = new MemoryCache(); legacyStores.set("eagler-touhou-app-shell-legacy", legacy);
await legacy.put("https://example.test/__app-shell-meta__/legacy", new Response(JSON.stringify({
  createdAt: 1, entries: { "https://example.test/": sha("fresh") },
})));
await legacy.put("https://example.test/", new Response("incorrect but labelled with a revision"));
const migrated = harness({ stores: legacyStores, files: { "./": "fresh" } });
await migrated.lifecycle("install");
assert.equal(migrated.requests.length, 1, "legacy revision labels are not proof of verified bytes");
assert.equal(await (await migrated.request("./")).text(), "fresh");
console.log("app shell worker update status, integrity, on-demand Runtimes and scope isolation: PASS");
