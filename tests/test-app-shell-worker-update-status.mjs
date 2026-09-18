import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";
import vm from "node:vm";

const workerSource = await readFile(new URL("../src/app-shell-sw.js", import.meta.url), "utf8");

class MemoryCache {
  entries = new Map();
  key(input) { return typeof input === "string" ? input : input.url; }
  async put(input, response) { this.entries.set(this.key(input), response.clone()); }
  async match(input) { return this.entries.get(this.key(input))?.clone(); }
  async delete(input) { return this.entries.delete(this.key(input)); }
  async keys() { return [...this.entries.keys()].map(url => new Request(url)); }
}

async function installWorker({ existingCacheNames = [], manifest = [], deferredPaths = [], seed, fetchImpl = fetch } = {}) {
  const stores = new Map(existingCacheNames.map(name => [name, new MemoryCache()]));
  const listeners = new Map();
  const caches = {
    async keys() { return [...stores.keys()]; },
    async open(name) {
      if (!stores.has(name)) stores.set(name, new MemoryCache());
      return stores.get(name);
    },
    async delete(name) { return stores.delete(name); },
  };
  if (seed) await seed(stores);
  const self = {
    __WB_MANIFEST: manifest,
    registration: { scope: "https://example.test/" },
    addEventListener(type, callback) { listeners.set(type, callback); },
    async skipWaiting() {},
    clients: { async claim() {} },
  };
  const source = workerSource.replace("__APP_SHELL_DEFERRED_PATHS__", JSON.stringify(deferredPaths));
  vm.runInNewContext(source, { self, caches, URL, Request, Response, fetch: fetchImpl, Date, Promise, console });
  let installTask;
  listeners.get("install")({ waitUntil(task) { installTask = task; } });
  await installTask;

  const currentName = [...stores.keys()].find(name => name.includes("__APP_SHELL_BUILD_ID__"));
  const current = stores.get(currentName);
  const metaKey = [...current.entries.keys()].find(url => url.includes("/__app-shell-meta__/"));
  const readMeta = async () => (await current.match(metaKey)).json();
  const requestStatus = async () => {
    let responseTask;
    listeners.get("fetch")({
      request: new Request("https://example.test/__app-shell-update-status__"),
      respondWith(task) { responseTask = task; },
    });
    return (await responseTask).json();
  };
  const requestMessage = async data => {
    let response;
    let task;
    const port = { postMessage(value) { response = value; } };
    listeners.get("message")({
      data,
      ports: [port],
      waitUntil(value) { task = value; },
    });
    await task;
    return response;
  };
  return { readMeta, requestStatus, requestMessage, stores, listeners, currentName, current };
}

const firstInstall = await installWorker();
assert.deepEqual(await firstInstall.readMeta(), {
  build: "__APP_SHELL_BUILD_ID__",
  createdAt: (await firstInstall.readMeta()).createdAt,
  appliedAt: null,
  updated: false,
  entries: {},
  install: { reused: 0, fetched: 0, deferred: 0 },
});
assert.equal((await firstInstall.requestStatus()).appliedAt, null, "first install must not count as an update");

const replacement = await installWorker({ existingCacheNames: ["eagler-touhou-app-shell-previous"] });
assert.equal((await replacement.readMeta()).appliedAt, null, "installing a replacement must wait for the refreshed app");
const applied = await replacement.requestStatus();
assert.equal(applied.updated, true);
assert.ok(Number(applied.appliedAt) > 0, "the refreshed app must stamp the applied update time");
assert.equal((await replacement.requestStatus()).appliedAt, applied.appliedAt, "later loads must retain the original applied time");

const unchangedUrl = "https://example.test/unchanged.js";
const changedUrl = "https://example.test/changed.js";
const deferredUrl = "https://example.test/runtime/game.wasm";
const networkRequests = [];
const revisioned = await installWorker({
  existingCacheNames: ["eagler-touhou-app-shell-old"],
  manifest: [
    { url: "unchanged.js", revision: "same" },
    { url: "changed.js", revision: "new" },
    { url: "runtime/game.wasm", revision: "runtime" },
  ],
  deferredPaths: ["runtime/game.wasm"],
  seed: async stores => {
    const old = stores.get("eagler-touhou-app-shell-old");
    await old.put(unchangedUrl, new Response("reused"));
    await old.put(deferredUrl, new Response("reused"));
    await old.put("https://example.test/__app-shell-meta__/old", new Response(JSON.stringify({
      createdAt: 1,
      entries: { [unchangedUrl]: "same", [deferredUrl]: "runtime" },
    })));
  },
  fetchImpl: async request => {
    networkRequests.push(request.url);
    return new Response(request.url === changedUrl ? "changed" : "network");
  },
});
const revisionedMeta = await revisioned.readMeta();
assert.deepEqual(revisionedMeta.install, { reused: 1, fetched: 1, deferred: 1 });
assert.deepEqual(networkRequests, [changedUrl], "install must reuse unchanged shell bytes and defer Runtime");
let deferredResponse;
revisioned.listeners.get("fetch")({
  request: new Request(deferredUrl),
  respondWith(task) { deferredResponse = task; },
});
assert.equal(await (await deferredResponse).text(), "reused", "deferred Runtime should reuse a matching older revision on demand");
assert.deepEqual(networkRequests, [changedUrl]);

const fallbackPaths = [
  "runtime/game.html",
  "runtime/shell.mjs",
  "runtime/eagler-host.mjs",
  "runtime/game.wasm",
];
const fallbackUrls = Object.fromEntries(fallbackPaths.map(path => [path, `https://example.test/${path}`]));
const fallbackNetworkRequests = [];
const fallback = await installWorker({
  existingCacheNames: ["eagler-touhou-app-shell-runtime-old"],
  manifest: fallbackPaths.map((url, index) => ({ url, revision: `current-${index}` })),
  deferredPaths: fallbackPaths,
  seed: async stores => {
    const old = stores.get("eagler-touhou-app-shell-runtime-old");
    await old.put(fallbackUrls["runtime/game.html"], new Response(
      "<script>const protocol='eagler-touhou/1';const epoch=Number(new URLSearchParams(location.search).get('runtimeEpoch'));window.parent.__eaglerPrepareManagedRuntimeDataV1({game:'th10',generation:'old',epoch});</script>"
    ));
    await old.put(fallbackUrls["runtime/shell.mjs"], new Response(
      "const protocol='eagler-touhou/1';const epoch=Number(new URLSearchParams(location.search).get('runtimeEpoch'));"
    ));
    await old.put(fallbackUrls["runtime/eagler-host.mjs"], new Response(
      "const epoch=Number(query.get('runtimeEpoch'));parentWindow.__eaglerPrepareManagedRuntimeDataV1({game,generation,epoch});"
    ));
    await old.put(fallbackUrls["runtime/game.wasm"], new Response("old-runtime-wasm"));
    await old.put("https://example.test/__app-shell-meta__/old-runtime", new Response(JSON.stringify({
      createdAt: 50,
      entries: Object.fromEntries(fallbackPaths.map((path, index) => [fallbackUrls[path], `old-${index}`])),
    })));
  },
  fetchImpl: async request => {
    fallbackNetworkRequests.push(request.url);
    return new Response(`network:${request.url}`);
  },
});
const fallbackProbe = await fallback.requestMessage({ type: "PROBE_RUNTIME_CACHE_FALLBACK", paths: fallbackPaths });
assert.equal(fallbackProbe.available, true, "complete epoch-compatible retained Runtime should be offered as one-shot fallback");
const restored = await fallback.requestMessage({ type: "RESTORE_RUNTIME_CACHE_FALLBACK", paths: fallbackPaths });
assert.equal(restored.ok, true);
assert.equal(await (await fallback.current.match(fallbackUrls["runtime/game.wasm"])).text(), "old-runtime-wasm");
assert.equal((await fallback.readMeta()).temporaryRuntimeFallback.paths.length, fallbackPaths.length);
assert.equal((await fallback.readMeta()).entries[fallbackUrls["runtime/game.wasm"]], "old-3",
  "temporary fallback metadata must describe the actual cached generation");
const cleared = await fallback.requestMessage({ type: "CLEAR_RUNTIME_CACHE_FALLBACK" });
assert.equal(cleared.cleared, fallbackPaths.length);
assert.equal((await fallback.readMeta()).entries[fallbackUrls["runtime/game.wasm"]], "current-3",
  "cleanup must restore the active manifest revision before the next network attempt");
let forcedNetworkResponse;
fallback.listeners.get("fetch")({
  request: new Request(fallbackUrls["runtime/game.wasm"]),
  respondWith(task) { forcedNetworkResponse = task; },
});
assert.match(await (await forcedNetworkResponse).text(), /^network:/,
  "the launch after a one-shot fallback must bypass retained Runtime caches and retry the network");
assert.deepEqual(fallbackNetworkRequests, [fallbackUrls["runtime/game.wasm"]]);

const incompatible = await installWorker({
  existingCacheNames: ["eagler-touhou-app-shell-incompatible-old"],
  manifest: fallbackPaths.map((url, index) => ({ url, revision: `current-${index}` })),
  deferredPaths: fallbackPaths,
  seed: async stores => {
    const old = stores.get("eagler-touhou-app-shell-incompatible-old");
    for (const path of fallbackPaths) await old.put(fallbackUrls[path], new Response(`legacy:${path}`));
    await old.put("https://example.test/__app-shell-meta__/legacy", new Response(JSON.stringify({
      createdAt: 60,
      entries: Object.fromEntries(fallbackPaths.map((path, index) => [fallbackUrls[path], `legacy-${index}`])),
    })));
  },
});
assert.equal((await incompatible.requestMessage({
  type: "PROBE_RUNTIME_CACHE_FALLBACK",
  paths: fallbackPaths,
})).available, false, "pre-epoch or otherwise incompatible Runtime caches must never be offered");

console.log("app shell worker update status: PASS");
