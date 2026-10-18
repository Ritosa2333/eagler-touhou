import assert from "node:assert/strict";
import {
  createMultiplayerSpectatorRailPositionStore,
  multiplayerSpectatorRailLegacyPositionStorageKey,
  multiplayerSpectatorRailPositionStorageKey,
} from "../.cache/build/browser/assets/launcher/multiplayer-spectator-rail-position.mjs";

class MemoryStorage {
  values = new Map();
  getItem(key) { return this.values.has(key) ? this.values.get(key) : null; }
  setItem(key, value) { this.values.set(key, String(value)); }
  removeItem(key) { this.values.delete(key); }
}

assert.equal(multiplayerSpectatorRailPositionStorageKey, "eagler-touhou-mp-spectator-rail-position-v1");
assert.equal(multiplayerSpectatorRailLegacyPositionStorageKey, "eagler.mpSpectatorRail.mobilePosition.v1");

const storage = new MemoryStorage();
const store = createMultiplayerSpectatorRailPositionStore({ storage });
store.save({ x: 12.6, y: 48.2 });
assert.deepEqual(JSON.parse(storage.getItem(multiplayerSpectatorRailPositionStorageKey)), { x: 13, y: 48 });
assert.deepEqual(store.load(), { x: 13, y: 48 });

storage.setItem(multiplayerSpectatorRailPositionStorageKey, JSON.stringify({ x: 40, y: 50 }));
storage.setItem(multiplayerSpectatorRailLegacyPositionStorageKey, JSON.stringify({ x: 1, y: 2 }));
assert.deepEqual(store.load(), { x: 40, y: 50 }, "canonical data must win over the legacy migration input");
assert.notEqual(storage.getItem(multiplayerSpectatorRailLegacyPositionStorageKey), null,
  "legacy data is only retired when it is actually used as the migration input");

storage.removeItem(multiplayerSpectatorRailPositionStorageKey);
assert.deepEqual(store.load(), { x: 1, y: 2 });
assert.equal(storage.getItem(multiplayerSpectatorRailLegacyPositionStorageKey), null,
  "successful one-way migration must retire the exact historical key");
assert.deepEqual(JSON.parse(storage.getItem(multiplayerSpectatorRailPositionStorageKey)), { x: 1, y: 2 });

storage.setItem(multiplayerSpectatorRailPositionStorageKey, JSON.stringify({ x: "bad", y: 2 }));
assert.equal(store.load(), null);
storage.setItem(multiplayerSpectatorRailPositionStorageKey, "not-json");
assert.equal(store.load(), null);

const legacyOnly = new MemoryStorage();
legacyOnly.setItem(multiplayerSpectatorRailLegacyPositionStorageKey, JSON.stringify({ x: 7, y: 8 }));
const failingMigrationStorage = {
  getItem: key => legacyOnly.getItem(key),
  setItem() { throw new Error("quota"); },
  removeItem: key => legacyOnly.removeItem(key),
};
const failingMigration = createMultiplayerSpectatorRailPositionStore({ storage: failingMigrationStorage });
assert.deepEqual(failingMigration.load(), { x: 7, y: 8 },
  "legacy data remains readable even if the canonical write cannot complete");
assert.notEqual(legacyOnly.getItem(multiplayerSpectatorRailLegacyPositionStorageKey), null,
  "legacy key must not be deleted unless the canonical write succeeds");

const hostile = createMultiplayerSpectatorRailPositionStore({ storage: {
  getItem() { throw new Error("blocked"); },
  setItem() { throw new Error("blocked"); },
  removeItem() { throw new Error("blocked"); },
} });
assert.equal(hostile.load(), null);
assert.doesNotThrow(() => hostile.save({ x: 1, y: 2 }));

console.log(JSON.stringify({
  multiplayerSpectatorRailPosition: "PASS",
  migration: "exact-one-way",
  canonicalWriteBeforeLegacyDelete: true,
  storageFailure: "non-fatal",
}));
