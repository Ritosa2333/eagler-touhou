import assert from "node:assert/strict";
import {
  createMultiplayerPreferenceStore,
  multiplayerLoadoutStorageKey,
  multiplayerShareSettingsStorageKey,
  normalizePreferredLoadout,
} from "../.cache/build/browser/assets/launcher/multiplayer-preferences.mjs";

class MemoryStorage {
  constructor(entries = []) { this.values = new Map(entries); }
  getItem(key) { return this.values.has(key) ? this.values.get(key) : null; }
  setItem(key, value) { this.values.set(key, String(value)); }
}

assert.notEqual(multiplayerShareSettingsStorageKey("th06mp"), multiplayerShareSettingsStorageKey("th07mp"));
assert.notEqual(multiplayerLoadoutStorageKey("th06mp"), multiplayerLoadoutStorageKey("th07mp"));
assert.equal(normalizePreferredLoadout("3", 4), 3);
assert.equal(normalizePreferredLoadout("4", 4), 0);
assert.equal(normalizePreferredLoadout("1.5", 4), 0);

const storage = new MemoryStorage([
  [multiplayerShareSettingsStorageKey("th06mp"), "0"],
  [multiplayerLoadoutStorageKey("th06mp"), "3"],
  [multiplayerLoadoutStorageKey("th07mp"), "5"],
]);
const preferences = createMultiplayerPreferenceStore({ storage });
assert.equal(preferences.replayAutoSave('th09mp'), true);
preferences.persistReplayAutoSave('th09mp', false);
assert.equal(createMultiplayerPreferenceStore({storage}).replayAutoSave('th09mp'), false);
preferences.persistReplayAutoSave('th09mp', true);
assert.equal(preferences.replayAutoSave('th09mp'), true);
assert.equal(preferences.replayAutoSave('th07mp'), false);
assert.deepEqual(preferences.load({ product: "th06mp", multiplayer: true, maxLoadout: 4 }), {
  shareSingleplayerSettings: false,
  preferredLoadout: 3,
});
assert.deepEqual(preferences.load({ product: "th07mp", multiplayer: true, maxLoadout: 6 }), {
  shareSingleplayerSettings: true,
  preferredLoadout: 5,
});
assert.deepEqual(preferences.load({ product: "th06", multiplayer: false, maxLoadout: 0 }), {
  shareSingleplayerSettings: true,
  preferredLoadout: null,
});

preferences.persistShareSingleplayerSettings("th07mp", false);
preferences.persistPreferredLoadout("th07mp", 4);
assert.equal(storage.getItem(multiplayerShareSettingsStorageKey("th07mp")), "0");
assert.equal(storage.getItem(multiplayerLoadoutStorageKey("th07mp")), "4");

const hostile = { getItem() { throw new Error("blocked"); }, setItem() { throw new Error("blocked"); } };
const hostilePreferences = createMultiplayerPreferenceStore({ storage: hostile });
assert.equal(hostilePreferences.replayAutoSave('th09mp'), true);
assert.deepEqual(hostilePreferences.load({ product: "th06mp", multiplayer: true, maxLoadout: 4 }), {
  shareSingleplayerSettings: true,
  preferredLoadout: 0,
});
assert.doesNotThrow(() => hostilePreferences.persistShareSingleplayerSettings("th06mp", false));
assert.doesNotThrow(() => hostilePreferences.persistPreferredLoadout("th06mp", 2));

console.log(JSON.stringify({
  multiplayerPreferences: "PASS",
  isolation: "per-product",
  ordinaryProduct: "share-default-no-loadout-reset",
  storageFailure: "non-fatal",
}));
