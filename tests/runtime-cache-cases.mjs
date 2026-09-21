import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";
import { createHash, webcrypto } from "node:crypto";
import vm from "node:vm";
const source = await readFile(new URL("../src/runtime-cache-sw.js", import.meta.url), "utf8");
const sha = text => createHash("sha256").update(text).digest("hex");
class MemoryCache {
  entries = new Map();
  key(input) { return typeof input === "string" ? input : input.url; }
  async put(input, response) { this.entries.set(this.key(input), response.clone()); }
  async match(input) { return this.entries.get(this.key(input))?.clone(); }
  async keys() { return [...this.entries.keys()].map(url => new Request(url)); }
  async delete(input) { return this.entries.delete(this.key(input)); }
}
const root = "runtime/th08/";
function harness() {
  const stores = new Map(), clients = new Map(), requests = [];
  const state = { files: {}, catalog: null, outage: false, corrupt: false, slow: false, quota: false };
  const scopeUrl = new URL("https://example.test/game/");
  const caches = {
    async keys() { return [...stores.keys()]; },
    async open(name) {
      if (!stores.has(name)) {
        const cache = new MemoryCache();
        const put = cache.put.bind(cache);
        cache.put = async (url, response) => {
          if (state.quota && String(url).includes("glue.mjs")) throw new Error("QuotaExceededError");
          await put(url, response);
        };
        stores.set(name, cache);
      }
      return stores.get(name);
    },
    async delete(name) { return stores.delete(name); },
  };
  function publish(version) {
    state.files = {
      [root + "th08.html"]: `<script type="module" src="shell.mjs"></script>${version}`,
      [root + "shell.mjs"]: `shell ${version}`,
      [root + "glue.mjs"]: `glue ${version}`,
      [root + "game.wasm"]: `wasm ${version}`,
      [root + "worker.mjs"]: `worker ${version}`,
    };
    if (version !== "a") state.files[root + "new-helper.mjs"] = `helper ${version}`;
    state.catalog = { schema: "eagler-touhou/runtime-cache/1", hostProtocol: "eagler-touhou/1",
      groups: [{ root, entries: Object.entries(state.files).map(([url, text]) => ({ url, revision: sha(text) })) }] };
    return JSON.parse(JSON.stringify(state.catalog));
  }
  const first = publish("a");
  function worker(embedded = first, embeddedAt = 0) {
    const context = vm.createContext({
      self: { clients: { async get(id) { return clients.get(id); } } },
      caches, crypto: webcrypto, URL, Request, Response, Headers, Uint8Array, Uint32Array,
      AbortController, setTimeout, clearTimeout, console,
      fetch: async request => {
        requests.push(request.url);
        if (state.outage) throw new Error("Offline");
        if (state.slow) return new Promise((_, reject) => request.signal.addEventListener("abort", () => reject(new Error("Timeout"))));
        const path = new URL(request.url).pathname.slice(scopeUrl.pathname.length);
        if (path === "app-shell-sw.js") return new Response(`// EAGLER_RUNTIME_CATALOG_V1 ${JSON.stringify(state.catalog)}\n`);
        const body = state.corrupt && path.endsWith("game.wasm") ? "bad wasm" : state.files[path];
        return new Response(body || "missing", { status: body === undefined ? 404 : 200 });
      },
    });
    vm.runInContext(source, context);
    return context.createRuntimeCache({ scopeUrl, catalog: embedded, fetchTimeoutMs: 10, embeddedCreatedAt: async () => embeddedAt });
  }
  async function navigate(sw, id, path = root + "th08.html") {
    clients.set(id, { id });
    // Node's Request disallows constructing mode:navigate, unlike browser events.
    return sw.handle({ request: { url: new URL(path, scopeUrl).href, method: "GET", mode: "navigate" }, resultingClientId: id, clientId: "" });
  }
  async function resource(sw, id, path, workerId = "") {
    return sw.handle({ request: new Request(new URL(root + path, scopeUrl)), clientId: id, resultingClientId: workerId });
  }
  return { stores, clients, requests, state, publish, worker, navigate, resource, scopeUrl, caches };
}
const h = harness();
let sw = h.worker();
assert.match(await (await h.navigate(sw, "a")).text(), /a$/);
assert.equal(await (await h.resource(sw, "a", "game.wasm")).text(), "wasm a");
const initialRequests = h.requests.length;
await h.navigate(sw, "a-again");
assert.equal(h.requests.length, initialRequests + 1, "every launch checks the latest catalog but reuses verified unchanged bytes");
h.publish("b");
assert.match(await (await h.navigate(sw, "b")).text(), /b$/);
assert.equal(await (await h.resource(sw, "b", "new-helper.mjs")).text(), "helper b");
assert.equal(await (await h.resource(sw, "a", "glue.mjs")).text(), "glue a");
assert.equal(await (await h.resource(sw, "a", "game.wasm")).text(), "wasm a", "new launches must not switch old clients");
// SW termination: pins are persisted, not held only in a JavaScript Map.
sw = h.worker();
assert.equal(await (await h.resource(sw, "a", "game.wasm")).text(), "wasm a");
h.clients.set("a-worker", { id: "a-worker" });
assert.equal(await (await h.resource(sw, "a", "worker.mjs", "a-worker")).text(), "worker a");
assert.equal(await (await h.resource(sw, "a-worker", "game.wasm")).text(), "wasm a");
h.publish("c"); h.state.corrupt = true;
assert.match(await (await h.navigate(sw, "fallback-b")).text(), /b$/, "a corrupt new Wasm falls back and actually serves the old document");
assert.equal(await (await h.resource(sw, "fallback-b", "glue.mjs")).text(), "glue b");
h.state.corrupt = false; h.state.outage = true;
assert.match(await (await h.navigate(sw, "offline")).text(), /b$/);
h.state.outage = false; h.state.slow = true;
assert.match(await (await h.navigate(sw, "timeout")).text(), /b$/);
h.state.slow = false; h.state.quota = true;
assert.match(await (await h.navigate(sw, "quota")).text(), /b$/);
h.state.quota = false;
assert.match(await (await h.navigate(sw, "c")).text(), /c$/);
assert.equal(await (await h.resource(sw, "a", "game.wasm")).text(), "wasm a", "GC must preserve an old, live session beyond the newest two snapshots");
// Poison the newest complete snapshot, then take the server offline. A complete
// older set must be selected before even the Runtime HTML is returned.
for (const [name, cache] of h.stores) {
  const response = await cache.match(new URL(root + "game.wasm", h.scopeUrl).href);
  if (response && await response.text() === "wasm c") await cache.delete(new URL(root + "game.wasm", h.scopeUrl).href);
}
h.state.outage = true;
assert.match(await (await h.navigate(sw, "evicted-c")).text(), /b$/);
assert.equal(await (await h.resource(sw, "evicted-c", "game.wasm")).text(), "wasm b");

const legacy = harness();
const oldCache = await legacy.caches.open("eagler-touhou-app-shell-legacy");
await oldCache.put(new URL(root + "glue.mjs", legacy.scopeUrl).href, new Response("glue a"));
legacy.publish("b");
await oldCache.put(new URL(root + "game.wasm", legacy.scopeUrl).href, new Response("wasm b"));
const migrated = legacy.worker();
assert.match(await (await legacy.navigate(migrated, "migration")).text(), /b$/);
assert.equal(await (await legacy.resource(migrated, "migration", "glue.mjs")).text(), "glue b");
assert.equal(await (await legacy.resource(migrated, "migration", "game.wasm")).text(), "wasm b");
const previousSnapshotCount = [...legacy.stores.keys()].length;
legacy.state.catalog.groups[0].entries[0].url = "https://other.test/evil.html";
assert.match(await (await legacy.navigate(migrated, "invalid-catalog")).text(), /b$/);
assert.equal([...legacy.stores.keys()].length, previousSnapshotCount);
const offlineMigration = harness();
const verifiedCache = await offlineMigration.caches.open("eagler-touhou-app-shell-new");
for (const [path, body] of Object.entries(offlineMigration.state.files)) await verifiedCache.put(new URL(path, offlineMigration.scopeUrl).href, new Response(body));
offlineMigration.state.outage = true;
assert.match(await (await offlineMigration.navigate(offlineMigration.worker(), "offline-migration")).text(), /a$/);
console.log("Runtime latest-first, complete fallback, poisoned legacy repair, per-client pinning and restart: PASS");

// An offline SW upgrade must not overlook a newer full Runtime already cached
// by the App Shell. An older embedded catalog must not undo a newer live launch.
const upgrade = harness();
const activeA = upgrade.worker();
await upgrade.navigate(activeA, "ua");
await new Promise(resolve => setTimeout(resolve, 2));
const catalogB = upgrade.publish("b");
const embeddedAt = Date.now();
const preparedB = await upgrade.caches.open("eagler-touhou-app-shell-prepared-b");
for (const [path, body] of Object.entries(upgrade.state.files)) await preparedB.put(new URL(path, upgrade.scopeUrl).href, new Response(body));
upgrade.state.outage = true;
assert.match(await (await upgrade.navigate(upgrade.worker(catalogB, embeddedAt), "ub")).text(), /b$/);
upgrade.state.outage = false;
await new Promise(resolve => setTimeout(resolve, 2));
upgrade.publish("c");
const activeB = upgrade.worker(catalogB, embeddedAt);
await upgrade.navigate(activeB, "uc");
upgrade.state.outage = true;
assert.match(await (await upgrade.navigate(activeB, "uc-offline")).text(), /c$/);
// Operator rollback reuses a prior snapshot, but records the NEW observation;
// a later outage must not silently undo that successful rollback.
upgrade.state.outage = false;
await new Promise(resolve => setTimeout(resolve, 2));
upgrade.publish("b");
await upgrade.navigate(activeB, "server-rollback");
upgrade.state.outage = true;
assert.match(await (await upgrade.navigate(activeB, "server-rollback-offline")).text(), /b$/);
console.log("Runtime offline SW handoff and server rollback ordering: PASS");

assert.equal((await upgrade.resource(activeB, "server-rollback-offline", "game.wasm")).headers.get("Cache-Control"), "no-store");
