import assert from "node:assert/strict";
import test from "node:test";
import { createMenuGestures } from "../../wwwroot/host/input/menu-gestures.mjs";

test("a menu swipe pulses one direction and a stationary tap confirms", () => {
  const gestures = createMenuGestures();
  gestures.down(1, 0.5, 0.5, 0);
  const swipe = gestures.move(1, 0.5, 0.7, "menu");
  assert.equal(swipe.pulse, "Down");
  assert.equal(gestures.move(1, 0.5, 0.9, "menu").pulse, null);
  assert.deepEqual(gestures.up(1, 0.5, 0.9, 100, "menu", true), []);

  gestures.down(2, 0.4, 0.4, 1000);
  assert.deepEqual(gestures.up(2, 0.41, 0.41, 1100, "menu", true), ["Z"]);
});

test("two fingers go back and a play double-tap bombs once", () => {
  const gestures = createMenuGestures();
  gestures.down(1, 0.5, 0.5, 0);
  gestures.down(2, 0.52, 0.5, 20);
  assert.deepEqual(gestures.up(2, 0.52, 0.5, 80, "menu", true), ["X"]);
  assert.deepEqual(gestures.up(1, 0.5, 0.5, 90, "menu", true), []);

  gestures.down(3, 0.2, 0.2, 0);
  assert.deepEqual(gestures.up(3, 0.2, 0.2, 40, "play", true), []);
  gestures.down(4, 0.21, 0.2, 120);
  assert.deepEqual(gestures.up(4, 0.21, 0.2, 160, "play", true), ["X"]);
});

test("dialogue uses a short tap to advance and a hold to skip", () => {
  const gestures = createMenuGestures();
  gestures.down(1, 0.5, 0.5, 0);
  assert.equal(gestures.skipping(400, "dialogue"), false);
  assert.deepEqual(gestures.up(1, 0.5, 0.5, 200, "dialogue", false), ["Z"]);

  gestures.down(2, 0.5, 0.5, 0);
  assert.equal(gestures.skipping(500, "dialogue"), true);
  assert.deepEqual(gestures.up(2, 0.5, 0.5, 700, "dialogue", false), []);
});
