import assert from "node:assert/strict";
import { createGameZoomController } from "../.cache/build/browser/assets/launcher/game-zoom.mjs";

class FakeClassList {
  #values = new Set();
  add(...names) { for (const name of names) this.#values.add(name); }
  remove(...names) { for (const name of names) this.#values.delete(name); }
  contains(name) { return this.#values.has(name); }
}

class FakeElement {
  constructor({ width = 100, height = 100, left = 0, top = 0 } = {}) {
    this.clientWidth = width;
    this.clientHeight = height;
    this.hidden = false;
    this.textContent = "";
    this.style = { transform: "" };
    this.classList = new FakeClassList();
    this.rect = { width, height, left, top };
  }

  getBoundingClientRect() {
    const { width, height, left, top } = this.rect;
    return { width, height, left, top, right: left + width, bottom: top + height };
  }
}

class FakeWindow {
  constructor() {
    this.listeners = new Map();
    this.addCount = 0;
    this.removeCount = 0;
  }

  addEventListener(type, listener) {
    this.addCount++;
    this.listeners.set(type, listener);
  }

  removeEventListener(type, listener) {
    this.removeCount++;
    if (this.listeners.get(type) === listener) this.listeners.delete(type);
  }
}

const player = new FakeElement({ width: 100, height: 100 });
const frame = new FakeElement({ width: 100, height: 100, left: 10, top: 20 });
const viewport = new FakeElement();
const toggle = new FakeElement();
const toggleLabel = new FakeElement();
const scaleLabel = new FakeElement();
const directSurface = {};
let available = true;
let baseOffset = { x: 10, y: 0 };

const zoom = createGameZoomController({
  player,
  frame,
  viewport,
  toggle,
  toggleLabel,
  scaleLabel,
  directSurface,
  getBaseOffset: () => baseOffset,
  getResetLabel: () => "Reset",
  isAvailable: () => available,
  minScale: 1,
  maxScale: 3,
});

zoom.refreshUi();
assert.equal(zoom.isActive(), true);
assert.equal(toggle.hidden, false);
assert.equal(toggleLabel.textContent, "Reset");
assert.equal(scaleLabel.textContent, "100%");

zoom.applyTransform(4, -500, 20);
assert.deepEqual(zoom.snapshot(), {
  active: true,
  scale: 3,
  x: -200,
  y: 0,
  pointerCount: 0,
});
assert.equal(viewport.style.transform, "translate3d(-190px,0px,0) scale(3)",
  "viewport transform combines the persisted base offset with clamped zoom translation");
assert.equal(scaleLabel.textContent, "300%");

baseOffset = { x: 0, y: 0 };
zoom.reset();
zoom.beginPointer({ pointerType: "touch", pointerId: 1, clientX: 25, clientY: 50, currentTarget: null });
zoom.beginPointer({ pointerType: "touch", pointerId: 2, clientX: 75, clientY: 50, currentTarget: null });
assert.equal(zoom.snapshot().pointerCount, 2);
zoom.movePointer({ pointerType: "touch", pointerId: 2, clientX: 100, clientY: 50, currentTarget: null });
const pinch = zoom.snapshot();
assert.equal(pinch.scale, 1.5);
assert.equal(pinch.x, -12.5);
assert.equal(pinch.y, -25);
assert.equal(viewport.style.transform, "translate3d(-12.5px,-25px,0) scale(1.5)");
zoom.endPointer({ pointerId: 2 });
assert.equal(zoom.snapshot().pointerCount, 1);
zoom.cancelGesture();
assert.equal(zoom.snapshot().pointerCount, 0);

const firstWindow = new FakeWindow();
const secondWindow = new FakeWindow();
zoom.bindInputWindow(firstWindow);
assert.equal(firstWindow.addCount, 4);
zoom.bindInputWindow(secondWindow);
assert.equal(firstWindow.removeCount, 4,
  "rebinding after iframe navigation must detach listeners from the previous Window realm");
assert.equal(secondWindow.addCount, 4);
zoom.uninstallInputBridge();
assert.equal(secondWindow.removeCount, 4);

available = false;
zoom.refreshUi();
assert.equal(zoom.isActive(), false);
assert.equal(toggle.hidden, true);

console.log(JSON.stringify({
  gameZoom: "PASS",
  scaleRange: [1, 3],
  pinch: "behavioral",
  runtimeWindowRebind: "detach-then-attach",
}));
