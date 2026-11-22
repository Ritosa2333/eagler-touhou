import assert from "node:assert/strict";
import test from "node:test";
import { createTouchMovement } from "../../wwwroot/host/input/touch-movement.mjs";

test("a drag is spent at the original high speed and then stops", () => {
  const movement = createTouchMovement();
  movement.add(0.1, 0, 100);
  assert.equal(movement.queued().x, 64);
  const first = movement.drain();
  assert.equal(first.shiftX, 4.5);
  assert.equal(first.shiftY, 0);
  assert.ok(movement.queued().x > 0);
  let guard = 0;
  let last = first;
  while ((last.shiftX !== 0 || last.shiftY !== 0) && guard < 40) {
    last = movement.drain();
    guard += 1;
  }
  assert.equal(last.shiftX, 0);
  assert.equal(movement.queued().x, 0);
});

test("focus and diagonal steps use the plane's own speed", () => {
  const movement = createTouchMovement();
  movement.add(0.1, 0.1, 100);
  const step = movement.drain({ focus: true });
  assert.ok(Math.abs(step.shiftX - 1.42) < 0.001);
  assert.ok(Math.abs(step.shiftY - 1.42) < 0.001);
  assert.ok(Math.abs(movement.queued().x - (64 - 1.42)) < 0.001);
  assert.ok(Math.abs(movement.queued().y - (48 - 1.42)) < 0.001);
});

test("sensitivity scales distance and unlimited movement returns the whole queue", () => {
  const movement = createTouchMovement();
  movement.add(0.1, 0, 200);
  assert.equal(movement.queued().x, 128);
  const step = movement.drain({ unlimited: true });
  assert.equal(step.shiftX, 128);
  assert.equal(step.shiftY, 0);
  assert.equal(movement.queued().x, 0);
});
