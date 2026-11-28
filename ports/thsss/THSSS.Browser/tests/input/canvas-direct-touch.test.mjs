import assert from "node:assert/strict";
import test from "node:test";
import { bindCanvasDirectTouch } from "../../wwwroot/host/input/canvas-direct-touch.mjs";

function harness(mode = "joystick") {
  const events = [];
  const listeners = new Map();
  const canvas = {
    getBoundingClientRect: () => ({ left: 0, top: 0, width: 100, height: 100 }),
    setPointerCapture() {},
    addEventListener(type, fn) {
      const list = listeners.get(type) || [];
      list.push(fn);
      listeners.set(type, list);
    },
    removeEventListener(type, fn) {
      const list = listeners.get(type) || [];
      listeners.set(type, list.filter((item) => item !== fn));
    },
  };
  const windowListeners = new Map();
  const previous = globalThis.window;
  globalThis.window = {
    addEventListener(type, fn) {
      const list = windowListeners.get(type) || [];
      list.push(fn);
      windowListeners.set(type, list);
    },
    removeEventListener(type, fn) {
      const list = windowListeners.get(type) || [];
      windowListeners.set(type, list.filter((item) => item !== fn));
    },
    visualViewport: null,
  };
  const binding = bindCanvasDirectTouch({
    canvas,
    direct: (message) => events.push(message),
    options: () => ({ touchMovementMode: mode }),
  });
  function fire(type, event) {
    for (const fn of listeners.get(type) || []) fn(event);
  }
  return {
    events,
    fire,
    binding,
    restore() {
      binding.dispose();
      globalThis.window = previous;
    },
  };
}

test("joystick mode forwards canvas pointers as normalized direct-touch", () => {
  const { events, fire, restore } = harness("joystick");
  try {
    fire("pointerdown", { pointerId: 7, pointerType: "touch", button: 0, clientX: 25, clientY: 50, preventDefault() {} });
    fire("pointermove", { pointerId: 7, clientX: 25, clientY: 80, preventDefault() {} });
    fire("pointerup", { pointerId: 7, clientX: 25, clientY: 80, preventDefault() {} });
    assert.deepEqual(events, [
      { type: "down", id: 7, x: 0.25, y: 0.5 },
      { type: "move", id: 7, x: 0.25, y: 0.8 },
      { type: "up", id: 7, x: 0.25, y: 0.8 },
    ]);
  } finally {
    restore();
  }
});

test("non-joystick modes leave the launcher surface as the sole owner", () => {
  const { events, fire, restore } = harness("touch");
  try {
    fire("pointerdown", { pointerId: 3, pointerType: "touch", button: 0, clientX: 10, clientY: 10, preventDefault() {} });
    assert.equal(events.length, 0);
  } finally {
    restore();
  }
});
