"use strict";

// A Runtime is a transaction, not a collection of independent cache-first URLs.
// Public URLs stay unchanged. Browser client IDs pin immutable private snapshots.
// This file is concatenated into the sole App Shell worker by app-shell-build.
function createRuntimeCache({ scopeUrl, catalog, fetchTimeoutMs = 8000, embeddedCreatedAt = async () => 0 }) {
  const prefix = `eagler-touhou-runtime-${encodeURIComponent(scopeUrl.pathname)}-`;
  const stateName = `${prefix}state`;
  const metaUrl = new URL("./__runtime-cache__/complete", scopeUrl).href;
  const pinPrefix = new URL("./__runtime-cache__/client/", scopeUrl).href;
  const marker = "// EAGLER_RUNTIME_CATALOG_V1 ";
  const inFlight = new Map();
  const protectedSnapshots = new Set();
  const known = validateCatalog(catalog);
  const jsonResponse = value => new Response(JSON.stringify(value), { headers: { "Content-Type": "application/json" } });

  function validateCatalog(value) {
    if (value?.schema !== "eagler-touhou/runtime-cache/1" || value.hostProtocol !== "eagler-touhou/1" ||
        !Array.isArray(value.groups) || value.groups.length > 64) throw new Error("Unsupported Runtime catalog");
    return value.groups.map(group => {
      if (!/^runtime\/[a-z0-9_-]+\/(?:[a-z0-9_-]+\/)*$/i.test(group.root) ||
          !Array.isArray(group.entries) || !group.entries.length || group.entries.length > 1024) {
        throw new Error("Invalid Runtime group");
      }
      const seen = new Set();
      const entries = group.entries.map(entry => {
        const url = new URL(entry.url, scopeUrl);
        if (typeof entry.url !== "string" || !entry.url.startsWith(group.root) ||
            url.origin !== scopeUrl.origin || url.pathname !== scopeUrl.pathname + entry.url ||
            url.search || url.hash || seen.has(entry.url) || !/^[a-f0-9]{64}$/.test(entry.revision || "")) {
          throw new Error("Invalid Runtime entry");
        }
        seen.add(entry.url);
        return { url: entry.url, revision: entry.revision, cacheUrl: url.href };
      });
      if (!entries.some(entry => entry.url.endsWith(".html"))) throw new Error("Runtime entry document missing");
      return { root: group.root, entries: entries.sort((a, b) => a.url.localeCompare(b.url)) };
    });
  }
  function groupFor(path, groups = known) {
    return groups.filter(group => path.startsWith(group.root)).sort((a, b) => b.root.length - a.root.length)[0];
  }
  function relativePath(request) {
    const url = new URL(request.url);
    return url.origin === scopeUrl.origin && url.pathname.startsWith(scopeUrl.pathname)
      ? url.pathname.slice(scopeUrl.pathname.length) : null;
  }
  async function digest(bytes) {
    return Array.from(new Uint8Array(await crypto.subtle.digest("SHA-256", bytes)), byte => byte.toString(16).padStart(2, "0")).join("");
  }
  async function checked(response, entry) {
    if (!response?.ok || await digest(await response.clone().arrayBuffer()) !== entry.revision) {
      throw new Error(`Runtime integrity mismatch: ${entry.url}`);
    }
    return response;
  }
  async function network(url, read) {
    const controller = new AbortController();
    const timer = setTimeout(() => controller.abort(), fetchTimeoutMs);
    try {
      const response = await fetch(new Request(url, { cache: "no-store", signal: controller.signal }));
      if (!response.ok) throw new Error(`Runtime HTTP ${response.status}: ${url}`);
      return await read(response);
    } finally { clearTimeout(timer); }
  }
  async function latestGroup(path) {
    // The build emits a JSON-only, machine-readable comment in the existing SW
    // artifact. No eval/import, extra public endpoint or versioned URL is needed.
    return network(new URL("./app-shell-sw.js", scopeUrl).href, async response => {
      const source = await response.text();
      if (source.length > 4 * 1024 * 1024) throw new Error("Runtime catalog exceeds size limit");
      const line = source.split("\n").find(line => line.startsWith(marker));
      if (!line) throw new Error("Runtime catalog is not published");
      const group = groupFor(path, validateCatalog(JSON.parse(line.slice(marker.length))));
      if (!group || !group.entries.some(entry => entry.url === path)) throw new Error("Runtime is not published");
      return group;
    });
  }
  function identity(group) {
    return JSON.stringify(group.entries.map(({ url, revision }) => [url, revision]));
  }
  async function snapshots(root) {
    const result = [];
    for (const name of await caches.keys()) {
      if (!name.startsWith(prefix) || name === stateName) continue;
      try {
        const cache = await caches.open(name);
        const raw = await (await cache.match(metaUrl))?.json();
        const group = validateCatalog({ schema: "eagler-touhou/runtime-cache/1", hostProtocol: "eagler-touhou/1", groups: [raw?.group] })[0];
        if (group.root === root && raw.complete === true) result.push({ name, group, createdAt: raw.createdAt, observedAt: raw.observedAt || raw.createdAt });
      } catch { /* Incomplete candidates are not rollback targets. */ }
    }
    return result.sort((a, b) => b.observedAt - a.observedAt);
  }
  async function complete(snapshot) {
    const cache = await caches.open(snapshot.name);
    try {
      for (const entry of snapshot.group.entries) await checked(await cache.match(entry.cacheUrl), entry);
      return true;
    } catch { return false; }
  }
  async function stage(group, { localOnly = false, observedAt = 0 } = {}) {
    const existing = await snapshots(group.root);
    for (const candidate of existing) {
      if (identity(candidate.group) === identity(group) && await complete(candidate)) {
        if (observedAt > candidate.observedAt) {
          candidate.observedAt = observedAt;
          await (await caches.open(candidate.name)).put(metaUrl, jsonResponse({ ...candidate, complete: true }));
        }
        return candidate;
      }
    }
    const random = Array.from(crypto.getRandomValues(new Uint32Array(4)), value => value.toString(16)).join("-");
    const name = `${prefix}${encodeURIComponent(group.root)}-${random}`;
    const cache = await caches.open(name);
    let cursor = 0;
    let stopped = false;
    const legacyNames = (await caches.keys()).filter(name => name.startsWith("eagler-touhou-app-shell-"));
    try {
      await cache.put(metaUrl, jsonResponse({ group, createdAt: Date.now(), complete: false }));
      // Reuse only verified bytes from committed snapshots. Never trust an old
      // per-file SW cache's revision labels; that is the TH08 failure mode.
      const results = await Promise.allSettled(Array.from({ length: Math.min(2, group.entries.length) }, async () => {
        while (!stopped && cursor < group.entries.length) {
          try {
            const entry = group.entries[cursor++];
            let response = null;
            for (const old of existing) {
              if (!old.group.entries.some(item => item.url === entry.url && item.revision === entry.revision)) continue;
              try { response = await checked(await (await caches.open(old.name)).match(entry.cacheUrl), entry); break; }
              catch { /* Try another verified snapshot or the network. */ }
            }
            // Legacy labels may be poisoned, but bytes matching the newly published
            // build digest are safe to reuse. This also permits offline migration.
            if (!response) for (const old of legacyNames) {
              try { response = await checked(await (await caches.open(old)).match(entry.cacheUrl), entry); break; }
              catch { /* No trustworthy bytes at this URL in this cache. */ }
            }
            if (!response && localOnly) throw new Error(`Runtime is not locally complete: ${entry.url}`);
            response ||= await network(entry.cacheUrl, response => checked(response, entry));
            await cache.put(entry.cacheUrl, response);
          } catch (error) { stopped = true; throw error; }
        }
      }));
      const failure = results.find(result => result.status === "rejected");
      if (failure) throw failure.reason;
      const snapshot = { name, group, createdAt: Date.now(), observedAt };
      // The completion marker is the commit point, after every writer drained.
      await cache.put(metaUrl, jsonResponse({ ...snapshot, complete: true }));
      return snapshot;
    } catch (error) {
      await caches.delete(name);
      throw error;
    }
  }
  async function select(path) {
    let error;
    // Always begin with the network, even if a complete older Runtime exists.
    // The Runtime is independent of a waiting Launcher SW update.
    try { const observedAt = Date.now(); return await stage(await latestGroup(path), { observedAt }); }
    catch (cause) { error = cause; }
    const group = groupFor(path);
    if (group) {
      const candidates = await snapshots(group.root);
      // A newer App Shell may have prepared a whole Runtime without it ever
      // being launched. Include that LOCAL set in fallback ordering, but do not
      // downgrade a Runtime learned later from the network to an old SW catalog.
      const localObservation = Number(await embeddedCreatedAt()) || 0;
      let embeddedTried = false;
      for (const candidate of candidates) {
        if (!embeddedTried && localObservation > candidate.observedAt) {
          embeddedTried = true;
          try { return await stage(group, { localOnly: true, observedAt: localObservation }); }
          catch { /* An incomplete embedded set cannot replace a usable one. */ }
        }
        if (candidate.group.entries.some(entry => entry.url === path) && await complete(candidate)) return candidate;
      }
      // Migration from the old per-file cache: rebuild and verify a complete
      // current set, rather than blessing poisoned legacy entries.
      try { return await stage(group, { observedAt: localObservation }); } catch (cause) { error = cause; }
    }
    // A publication may have completed while the first candidate was loading.
    // Retry once with a fresh catalog only when no usable fallback exists.
    try { const observedAt = Date.now(); return await stage(await latestGroup(path), { observedAt }); } catch (cause) { error = cause; }
    throw error || new Error("No complete Runtime is available");
  }
  async function prepare(path) {
    const root = groupFor(path)?.root || path;
    // Only overlapping launches coalesce. Each later launch checks the server.
    if (!inFlight.has(root)) {
      const task = select(path);
      inFlight.set(root, task);
      task.finally(() => { if (inFlight.get(root) === task) inFlight.delete(root); }).catch(() => {});
    }
    return inFlight.get(root);
  }
  async function pin(id, snapshot) {
    if (!id) throw new Error("Runtime client identity unavailable");
    await (await caches.open(stateName)).put(pinPrefix + encodeURIComponent(id), jsonResponse({ ...snapshot, pinnedAt: Date.now() }));
  }
  async function readPin(id) {
    if (!id) return null;
    return (await (await caches.open(stateName)).match(pinPrefix + encodeURIComponent(id)))?.json() || null;
  }
  async function collect(root) {
    // Keep two committed generations plus every live/in-flight client pin.
    // Never prune the fallback to make room for an uncommitted candidate.
    const state = await caches.open(stateName);
    const retained = new Set(protectedSnapshots);
    for (const key of await state.keys()) {
      if (!key.url.startsWith(pinPrefix)) continue;
      const id = decodeURIComponent(key.url.slice(pinPrefix.length));
      const saved = await (await state.match(key)).json();
      if (protectedSnapshots.has(saved.name) || Date.now() - Number(saved.pinnedAt || 0) < 60000 ||
          await self.clients.get(id)) retained.add(saved.name);
      else await state.delete(key);
    }
    const previous = await snapshots(root);
    for (const snapshot of previous.slice(0, 2)) retained.add(snapshot.name);
    for (const snapshot of previous) if (!retained.has(snapshot.name)) await caches.delete(snapshot.name);
    // Reap only old, explicitly uncommitted stages left by a terminated worker.
    for (const name of await caches.keys()) {
      if (!name.startsWith(prefix) || name === stateName || retained.has(name)) continue;
      try {
        const staged = await (await (await caches.open(name)).match(metaUrl))?.json();
        if (staged?.complete === false && staged.group?.root === root && Date.now() - staged.createdAt > 600000) {
          await caches.delete(name);
        }
      } catch { /* Unknown cache metadata is not ours to prune. */ }
    }
  }
  function deliver(response) {
    // Browser memory/HTTP caches must not key a prior client's response only by
    // the unchanged public URL and bypass another client's snapshot selection.
    const headers = new Headers(response.headers);
    headers.set("Cache-Control", "no-store");
    headers.delete("Content-Length");
    headers.delete("Content-Encoding");
    return new Response(response.body, { status: response.status, statusText: response.statusText, headers });
  }
  async function respond(event, path) {
    let snapshot;
    if (event.request.mode === "navigate" && path.endsWith(".html")) {
      snapshot = await prepare(path);
      protectedSnapshots.add(snapshot.name);
      try {
        // Write before delivering any HTML, so all nested module/Wasm requests
        // use the same snapshot even after this SW process is terminated.
        await pin(event.resultingClientId, snapshot);
        await collect(snapshot.group.root);
      } finally { protectedSnapshots.delete(snapshot.name); }
    } else {
      snapshot = await readPin(event.clientId);
      if (!snapshot) {
        // Requests from the Launcher (preparation probes) are not game sessions.
        // Do not create a per-file Runtime cache outside the transaction.
        return fetch(new Request(event.request, { cache: "no-store" }));
      }
      if (event.resultingClientId) await pin(event.resultingClientId, snapshot); // dedicated workers
    }
    const entry = snapshot.group.entries.find(entry => entry.url === path);
    if (!entry) return fetch(new Request(event.request, { cache: "no-store" })); // Package Store / unrelated data
    const response = await (await caches.open(snapshot.name)).match(entry.cacheUrl);
    if (response) return deliver(response);
    // A storage eviction after launch can only be repaired with matching bytes,
    // not by slipping a different Wasm underneath the already executing glue.
    return deliver(await network(entry.cacheUrl, response => checked(response, entry)));
  }
  function handle(event) {
    if (event.request.method !== "GET") return null;
    const path = relativePath(event.request);
    if (path === null || !path.startsWith("runtime/")) return null;
    return respond(event, path);
  }
  async function preparePaths(paths) {
    const groups = new Map();
    for (const path of paths) {
      const group = groupFor(path);
      if (!group) continue;
      const entry = group.entries.find(entry => entry.url.endsWith(".html"));
      groups.set(group.root, entry.url);
    }
    for (const path of groups.values()) await prepare(path);
  }
  async function readyGroups() {
    const result = [];
    for (const group of known) {
      for (const candidate of await snapshots(group.root)) {
        if (await complete(candidate)) { result.push(group.root); break; }
      }
    }
    return result;
  }
  return { handle, preparePaths, readyGroups };
}
