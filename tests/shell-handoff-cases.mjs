import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";
import { createHash, webcrypto } from "node:crypto";
import vm from "node:vm";
const sha = text => createHash("sha256").update(text).digest("hex");
class MemoryCache {
  entries = new Map();
  key(input) { return typeof input === "string" ? input : input.url; }
  async put(input, response) { this.entries.set(this.key(input), response.clone()); }
  async match(input) { return this.entries.get(this.key(input))?.clone(); }
  async keys() { return [...this.entries.keys()].map(url => new Request(url)); }
  async delete(input) { return this.entries.delete(this.key(input)); }
}
// Closing a browser with A active and several newer waiting candidates must
// not erase A during activation: the persisted registration may still be A.
const shellSource = await readFile(new URL('../src/app-shell-sw.js', import.meta.url), 'utf8');
const shellStores = new Map();
const shellCaches = {
  async keys() { return [...shellStores.keys()]; },
  async open(name) { if (!shellStores.has(name)) shellStores.set(name, new MemoryCache()); return shellStores.get(name); },
  async delete(name) { return shellStores.delete(name); },
};
function shellWorker(build) {
  const events = new Map();
  let requests = 0;
  const context = vm.createContext({
    self: { registration: { scope: 'https://handoff.test/' }, __WB_MANIFEST: [{url:'./',revision:sha(build)}],
      addEventListener(type, callback) { events.set(type, callback); } },
    caches: shellCaches, URL, Request, Response, Headers, Uint8Array,
    crypto: webcrypto, AbortController, setTimeout, clearTimeout, console,
    fetch: async () => { requests++; return new Response(build); },
  });
  vm.runInContext(shellSource.replaceAll('__APP_SHELL_BUILD_ID__', build).replace('__APP_SHELL_DEFERRED_PATHS__', '[]'), context);
  return {
    async lifecycle(type) { let task; events.get(type)({waitUntil(value) { task=value; }}); await task; },
    status: () => context.appShellUpdateStatus(),
    home: () => context.shellCacheFirst({cacheUrl:'https://handoff.test/',revision:sha(build)}),
    requests: () => requests,
  };
}
const shellA = shellWorker('a');
await shellA.lifecycle('install'); await shellA.lifecycle('activate'); await shellA.status();
await new Promise(resolve => setTimeout(resolve, 2));
const shellB = shellWorker('b'); await shellB.lifecycle('install');
await new Promise(resolve => setTimeout(resolve, 2));
const shellC = shellWorker('c'); await shellC.lifecycle('install');
await shellC.lifecycle('activate');
assert.equal(shellStores.size, 3, 'activation must retain all committed shell caches until live handoff');
const aNetwork = shellA.requests();
assert.equal(await (await shellA.home()).text(), 'a');
assert.equal(shellA.requests(), aNetwork, 'persisted old worker still cold-starts from cache');
await shellC.status();
assert.equal(shellStores.size, 2, 'acknowledgement prunes unused candidates but keeps the last used shell');
assert.equal(await (await shellA.home()).text(), 'a');
assert.equal(shellA.requests(), aNetwork);
assert.equal(await (await shellC.home()).text(), 'c');
console.log('Shell shutdown activation retains last usable startup generation: PASS');
