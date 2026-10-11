"use strict";

// Read-only migration from the unreleased fixed-URL snapshot format. Never
// manufacture a generation from revision labels without checking every byte.
async function migrateLegacyRuntimeSnapshots({ scopeUrl, root, entry, sha256, importSnapshot }) {
  const { canonicalRuntimePayload, isRuntimePath } = EaglerRuntimeGenerations;
  const prefix = `eagler-touhou-runtime-${encodeURIComponent(scopeUrl.pathname)}-`;
  const marker = new URL("./__runtime-cache__/complete", scopeUrl).href;
  for (const name of await caches.keys()) {
    if (!name.startsWith(prefix) || name.endsWith("-state")) continue;
    try {
      const cache = await caches.open(name);
      const old = await (await cache.match(marker))?.json();
      if (old?.complete !== true || old.group?.root !== root || !Array.isArray(old.group.entries) ||
          !old.group.entries.length || old.group.entries.length > 2048) continue;
      const files = [], responses = new Map();
      for (const item of old.group.entries) {
        if (typeof item.url !== "string" || !item.url.startsWith(root)) throw new Error("Invalid legacy Runtime path");
        const path = item.url.slice(root.length);
        if (!isRuntimePath(path) || responses.has(path) || !/^[a-f0-9]{64}$/.test(item.revision || "")) throw new Error("Invalid legacy Runtime identity");
        const response = await cache.match(new URL(item.url, scopeUrl).href);
        if (!response?.ok) throw new Error("Legacy Runtime is incomplete");
        const bytes = await response.clone().arrayBuffer();
        if (await sha256(bytes) !== item.revision) throw new Error("Legacy Runtime bytes disagree with their revision");
        files.push({ path, bytes: bytes.byteLength, sha256: item.revision });
        responses.set(path, response);
      }
      files.sort((a, b) => a.path < b.path ? -1 : a.path > b.path ? 1 : 0);
      const generation = await sha256(new TextEncoder().encode(canonicalRuntimePayload(entry, files)));
      await importSnapshot(root, { generation, entry, files }, responses, Number(old.observedAt || old.createdAt) || 0);
    } catch { /* Unknown, partial, or poisoned snapshots are not fallback targets. */ }
  }
}
