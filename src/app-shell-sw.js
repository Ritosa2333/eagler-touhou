"use strict";

// App Shell only. Package/game content and Package DATA preparation are not
// routed through this worker. Deployment builds may add App-owned Runtime
// HTML/JS/WASM to this same precache so an installed game can start offline.
const CACHE_PREFIX = "eagler-touhou-app-shell-";
const CACHE_NAME = `${CACHE_PREFIX}__APP_SHELL_BUILD_ID__`;
const CACHE_RETENTION = 2;
const PRECACHE_CONCURRENCY = 3;
const scopeUrl = new URL(self.registration.scope);
const PRECACHE_MANIFEST = self.__WB_MANIFEST;
const DEFERRED_PATHS = new Set(__APP_SHELL_DEFERRED_PATHS__.map(path => new URL(path, scopeUrl).href));
const cacheMetaUrl = new URL(`./__app-shell-meta__/__APP_SHELL_BUILD_ID__`, scopeUrl).href;
const updateStatusUrl = new URL("./__app-shell-update-status__", scopeUrl).href;
const forceNetworkOnce = new Set();
const runtimeFallbackLockedPaths = new Set();

const manifestByPathname = new Map(PRECACHE_MANIFEST.map(entry => {
  const url = new URL(entry.url, scopeUrl);
  return [url.pathname, { ...entry, cacheUrl: url.href }];
}));

async function precacheShell() {
  const existingShellCaches = (await caches.keys()).filter(name => name.startsWith(CACHE_PREFIX) && name !== CACHE_NAME);
  const createdAt = Date.now();
  const cache = await caches.open(CACHE_NAME);
  const rankedExisting = await Promise.all(existingShellCaches.map(async name => ({
    name,
    createdAt: await cacheCreatedAt(name),
    metadata: await cacheMetadata(name),
  })));
  rankedExisting.sort((a, b) => b.createdAt - a.createdAt);
  const reusable = rankedExisting.find(item => item.metadata?.entries && typeof item.metadata.entries === "object") || null;
  const reusableCache = reusable ? await caches.open(reusable.name) : null;
  let reused = 0;
  let fetched = 0;
  const installEntries = PRECACHE_MANIFEST.filter(entry => !DEFERRED_PATHS.has(new URL(entry.url, scopeUrl).href));
  let cursor = 0;
  await Promise.all(Array.from({ length: Math.min(PRECACHE_CONCURRENCY, installEntries.length) }, async () => {
    while (cursor < installEntries.length) {
      const entry = installEntries[cursor++];
      const cacheUrl = new URL(entry.url, scopeUrl).href;
      if (reusableCache && entry.revision && reusable.metadata.entries[cacheUrl] === entry.revision) {
        const previous = await reusableCache.match(cacheUrl);
        if (previous) {
          await cache.put(cacheUrl, previous);
          reused++;
          continue;
        }
      }
      const request = new Request(cacheUrl, { cache: "reload" });
      const response = await fetch(request);
      if (!response.ok) throw new Error(`App Shell precache failed: ${request.url} HTTP ${response.status}`);
      await cache.put(cacheUrl, response);
      fetched++;
    }
  }));
  await cache.put(cacheMetaUrl, new Response(JSON.stringify({
    build: "__APP_SHELL_BUILD_ID__",
    createdAt,
    appliedAt: null,
    updated: existingShellCaches.length > 0,
    entries: Object.fromEntries(PRECACHE_MANIFEST.map(entry => [new URL(entry.url, scopeUrl).href, entry.revision || null])),
    install: { reused, fetched, deferred: PRECACHE_MANIFEST.length - installEntries.length },
  }), { headers: { "Content-Type": "application/json", "Cache-Control": "no-store" } }));
}

async function appShellUpdateStatus() {
  const cache = await caches.open(CACHE_NAME);
  const response = await cache.match(cacheMetaUrl);
  if (!response) return new Response(null, { status: 503 });
  const status = await response.json();
  if (status.updated === true && !(Number(status.appliedAt) > 0)) {
    status.appliedAt = Date.now();
    const applied = new Response(JSON.stringify(status), {
      headers: { "Content-Type": "application/json", "Cache-Control": "no-store" },
    });
    await cache.put(cacheMetaUrl, applied.clone());
    return applied;
  }
  return new Response(JSON.stringify(status), {
    headers: { "Content-Type": "application/json", "Cache-Control": "no-store" },
  });
}

self.addEventListener("install", event => {
  event.waitUntil((async () => {
    await precacheShell();
    await self.skipWaiting();
  })());
});

async function cacheCreatedAt(name) {
  try {
    const cache = await caches.open(name);
    const keys = await cache.keys();
    const metaKey = keys.find(request => new URL(request.url).pathname.includes("/__app-shell-meta__/"));
    if (!metaKey) return 0;
    const response = await cache.match(metaKey);
    const parsed = await response?.json();
    return Number(parsed?.createdAt) || 0;
  } catch {
    return 0;
  }
}

async function cacheMetadata(name) {
  try {
    const cache = await caches.open(name);
    const keys = await cache.keys();
    const metaKey = keys.find(request => new URL(request.url).pathname.includes("/__app-shell-meta__/"));
    return metaKey ? await (await cache.match(metaKey))?.json() : null;
  } catch {
    return null;
  }
}

async function writeCurrentCacheMetadata(metadata) {
  const cache = await caches.open(CACHE_NAME);
  await cache.put(cacheMetaUrl, new Response(JSON.stringify(metadata), {
    headers: { "Content-Type": "application/json", "Cache-Control": "no-store" },
  }));
}

function runtimeEntriesForPaths(paths) {
  return paths.map(path => {
    const url = new URL(String(path), scopeUrl);
    if (url.origin !== scopeUrl.origin) throw new Error(`Runtime cache path is cross-origin: ${url.href}`);
    const entry = manifestByPathname.get(url.pathname);
    if (!entry) throw new Error(`Runtime cache path is not published: ${url.pathname}`);
    return entry;
  });
}

async function fallbackCacheForEntries(entries) {
  const names = (await caches.keys()).filter(name => name.startsWith(CACHE_PREFIX) && name !== CACHE_NAME);
  const ranked = await Promise.all(names.map(async name => ({
    name,
    createdAt: await cacheCreatedAt(name),
    metadata: await cacheMetadata(name),
  })));
  ranked.sort((left, right) => right.createdAt - left.createdAt);
  for (const item of ranked) {
    if (!item.metadata?.entries || typeof item.metadata.entries !== "object") continue;
    const cache = await caches.open(item.name);
    const responses = new Map();
    let complete = true;
    for (const entry of entries) {
      if (!(entry.cacheUrl in item.metadata.entries)) {
        complete = false;
        break;
      }
      const response = await cache.match(entry.cacheUrl);
      if (!response) {
        complete = false;
        break;
      }
      responses.set(entry.cacheUrl, response);
    }
    if (!complete) continue;
    let protocolCompatible = false;
    let managedDataCompatible = false;
    for (const entry of entries) {
      if (!/.(?:html|m?js)$/i.test(new URL(entry.cacheUrl).pathname)) continue;
      const text = await responses.get(entry.cacheUrl).clone().text().catch(() => "");
      if (text.includes("eagler-touhou/1") && text.includes("runtimeEpoch")) protocolCompatible = true;
      if (text.includes("__eaglerPrepareManagedRuntimeDataV1") && text.includes("runtimeEpoch")) {
        managedDataCompatible = true;
      }
    }
    if (protocolCompatible && managedDataCompatible) return { ...item, responses };
  }
  return null;
}

async function probeRuntimeCacheFallback(entries) {
  const candidate = await fallbackCacheForEntries(entries);
  return candidate ? { ok: true, available: true, sourceCache: candidate.name } : { ok: true, available: false };
}

async function restoreRuntimeCacheFallback(entries) {
  const candidate = await fallbackCacheForEntries(entries);
  if (!candidate) return { ok: false, available: false, error: "Compatible cached Runtime is unavailable" };
  const cache = await caches.open(CACHE_NAME);
  for (const entry of entries) {
    await cache.put(entry.cacheUrl, candidate.responses.get(entry.cacheUrl).clone());
    runtimeFallbackLockedPaths.add(entry.cacheUrl);
  }
  const metadata = await cacheMetadata(CACHE_NAME) || {};
  metadata.entries ||= {};
  for (const entry of entries) metadata.entries[entry.cacheUrl] = candidate.metadata.entries[entry.cacheUrl] ?? null;
  metadata.temporaryRuntimeFallback = {
    createdAt: Date.now(),
    sourceCache: candidate.name,
    paths: entries.map(entry => entry.cacheUrl),
  };
  await writeCurrentCacheMetadata(metadata);
  return { ok: true, available: true, restored: entries.length, sourceCache: candidate.name };
}

async function clearRuntimeCacheFallback() {
  const metadata = await cacheMetadata(CACHE_NAME);
  const paths = Array.isArray(metadata?.temporaryRuntimeFallback?.paths)
    ? metadata.temporaryRuntimeFallback.paths.filter(path => typeof path === "string")
    : [];
  if (!paths.length) return { ok: true, cleared: 0 };
  const cache = await caches.open(CACHE_NAME);
  for (const path of paths) {
    await cache.delete(path);
    runtimeFallbackLockedPaths.delete(path);
    forceNetworkOnce.add(path);
    const currentEntry = manifestByPathname.get(new URL(path).pathname);
    if (metadata.entries && currentEntry) metadata.entries[path] = currentEntry.revision || null;
  }
  delete metadata.temporaryRuntimeFallback;
  await writeCurrentCacheMetadata(metadata);
  return { ok: true, cleared: paths.length };
}

self.addEventListener("activate", event => {
  event.waitUntil((async () => {
    const shellCaches = (await caches.keys()).filter(name => name.startsWith(CACHE_PREFIX));
    const ranked = await Promise.all(shellCaches.map(async name => ({ name, createdAt: await cacheCreatedAt(name) })));
    ranked.sort((a, b) => b.createdAt - a.createdAt);
    for (const entry of ranked.slice(CACHE_RETENTION)) await caches.delete(entry.name);
    await self.clients.claim();
  })());
});

function manifestEntryForRequest(request) {
  if (request.method !== "GET") return null;
  const url = new URL(request.url);
  if (url.origin !== scopeUrl.origin) return null;
  return manifestByPathname.get(url.pathname) || null;
}

async function shellCacheFirst(request, entry) {
  const cache = await caches.open(CACHE_NAME);
  const forceNetwork = forceNetworkOnce.delete(entry.cacheUrl);
  if (!forceNetwork) {
    const cached = await cache.match(entry.cacheUrl);
    if (cached) return cached;
    if (entry.revision) {
      const otherCaches = (await caches.keys()).filter(name => name.startsWith(CACHE_PREFIX) && name !== CACHE_NAME);
      for (const name of otherCaches) {
        const metadata = await cacheMetadata(name);
        if (metadata?.entries?.[entry.cacheUrl] !== entry.revision) continue;
        const previous = await (await caches.open(name)).match(entry.cacheUrl);
        if (previous) {
          await cache.put(entry.cacheUrl, previous.clone());
          return previous;
        }
      }
    }
  }
  const response = await fetch(request);
  if (response.ok && !runtimeFallbackLockedPaths.has(entry.cacheUrl)) {
    await cache.put(entry.cacheUrl, response.clone());
  }
  return response;
}

self.addEventListener("fetch", event => {
  if (event.request.method === "GET" && event.request.url === updateStatusUrl) {
    event.respondWith(appShellUpdateStatus());
    return;
  }
  const entry = manifestEntryForRequest(event.request);
  if (!entry) return;
  event.respondWith(shellCacheFirst(event.request, entry));
});

self.addEventListener("message", event => {
  const type = event.data?.type;
  const runtimeCacheCommand = new Set([
    "PROBE_RUNTIME_CACHE_FALLBACK",
    "RESTORE_RUNTIME_CACHE_FALLBACK",
    "CLEAR_RUNTIME_CACHE_FALLBACK",
  ]).has(type);
  if (type !== "CACHE_APP_SHELL_PATHS" && !runtimeCacheCommand) return;
  if (type !== "CLEAR_RUNTIME_CACHE_FALLBACK" && !Array.isArray(event.data.paths)) return;
  const port = event.ports?.[0];
  event.waitUntil((async () => {
    try {
      if (type === "CLEAR_RUNTIME_CACHE_FALLBACK") {
        port?.postMessage(await clearRuntimeCacheFallback());
        return;
      }
      const entries = runtimeEntriesForPaths(event.data.paths);
      if (type === "PROBE_RUNTIME_CACHE_FALLBACK") {
        port?.postMessage(await probeRuntimeCacheFallback(entries));
        return;
      }
      if (type === "RESTORE_RUNTIME_CACHE_FALLBACK") {
        port?.postMessage(await restoreRuntimeCacheFallback(entries));
        return;
      }
      let cursor = 0;
      await Promise.all(Array.from({ length: Math.min(PRECACHE_CONCURRENCY, entries.length) }, async () => {
        while (cursor < entries.length) {
          const entry = entries[cursor++];
          const response = await shellCacheFirst(new Request(entry.cacheUrl), entry);
          if (!response.ok) throw new Error(`App Shell cache failed: ${entry.cacheUrl} HTTP ${response.status}`);
        }
      }));
      port?.postMessage({ ok: true, cached: entries.length });
    } catch (error) {
      port?.postMessage({ ok: false, error: error instanceof Error ? error.message : String(error) });
      throw error;
    }
  })());
});
