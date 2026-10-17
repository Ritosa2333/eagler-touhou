import assert from "node:assert/strict";
import {
  createTouchLayoutWindowPositionStore,
  emptyTouchLayoutWindowPositions,
  normalizeTouchLayoutWindowPositions,
  touchLayoutWindowPositionsStorageKey,
  touchLayoutWindowPositionsVersion,
} from "../.cache/build/browser/assets/launcher/touch-layout-editor-state.mjs";

class MemoryStorage {
  constructor(initial = {}) { this.values = new Map(Object.entries(initial)); }
  getItem(key) { return this.values.has(key) ? this.values.get(key) : null; }
  setItem(key, value) { this.values.set(key, String(value)); }
}

assert.equal(touchLayoutWindowPositionsStorageKey, "eagler-touhou-touch-layout-window-positions-v1");
assert.equal(touchLayoutWindowPositionsVersion, 1);
assert.deepEqual(normalizeTouchLayoutWindowPositions(null), emptyTouchLayoutWindowPositions());

const normalized = normalizeTouchLayoutWindowPositions({
  version: 1,
  profiles: {
    landscape: {
      editor: { x: -2, y: 3 },
      settings: { x: 0.25, y: 0.75 },
    },
    portrait: {
      editor: { x: Number.NaN, y: 0.5 },
      settings: { x: 0.4, y: 0.6 },
    },
  },
});
assert.deepEqual(normalized.profiles.landscape.editor, { x: 0, y: 1 });
assert.deepEqual(normalized.profiles.landscape.settings, { x: 0.25, y: 0.75 });
assert.equal(normalized.profiles.portrait.editor, null,
  "one corrupt window position must not invalidate sibling window/orientation entries");
assert.deepEqual(normalized.profiles.portrait.settings, { x: 0.4, y: 0.6 });

const storage = new MemoryStorage({
  [touchLayoutWindowPositionsStorageKey]: JSON.stringify(normalized),
});
const store = createTouchLayoutWindowPositionStore({ storage });
assert.deepEqual(store.get("landscape", "settings"), { x: 0.25, y: 0.75 });

const detached = store.get("landscape", "settings");
detached.x = 1;
assert.deepEqual(store.get("landscape", "settings"), { x: 0.25, y: 0.75 },
  "callers must not mutate persisted editor state through a returned point");

assert.equal(store.set("portrait", "editor", { x: 1.4, y: -0.4 }), true);
assert.deepEqual(store.get("portrait", "editor"), { x: 1, y: 0 });
assert.deepEqual(
  JSON.parse(storage.getItem(touchLayoutWindowPositionsStorageKey)).profiles.portrait.editor,
  { x: 1, y: 0 },
);

storage.setItem(touchLayoutWindowPositionsStorageKey, JSON.stringify({
  version: 1,
  profiles: { landscape: { editor: { x: 0.1, y: 0.2 } } },
}));
store.reload();
assert.deepEqual(store.get("landscape", "editor"), { x: 0.1, y: 0.2 });
assert.equal(store.get("portrait", "settings"), null);

const hostileStorage = {
  getItem() { throw new Error("storage unavailable"); },
  setItem() { throw new Error("storage unavailable"); },
};
const memoryOnly = createTouchLayoutWindowPositionStore({ storage: hostileStorage });
assert.deepEqual(memoryOnly.snapshot(), emptyTouchLayoutWindowPositions());
assert.equal(memoryOnly.set("landscape", "editor", { x: 0.3, y: 0.7 }), true,
  "storage failure must not break the current editor session");
assert.deepEqual(memoryOnly.get("landscape", "editor"), { x: 0.3, y: 0.7 });

console.log(JSON.stringify({
  touchLayoutEditorState: "PASS",
  version: 1,
  storageFailure: "non-fatal",
  positions: "normalized-per-window",
}));
