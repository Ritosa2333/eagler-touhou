import assert from "node:assert/strict";
import test from "node:test";

import { createKeyOwners } from "../../wwwroot/host/input/key-owners.mjs";
import { createGamepadKeys } from "../../wwwroot/host/input/gamepad-keys.mjs";

test("launcher gamepad bits become the same key names as the keyboard path", () => {
  const down = new Set();
  const keys = createKeyOwners((name, pressed) => {
    if (pressed) down.add(name);
    else down.delete(name);
  });
  let bits = -1;
  const pad = createGamepadKeys({ keys, sample: () => bits });

  pad.presented();
  assert.deepEqual([...down], []);

  bits = 128 | 1; // right + shoot
  pad.presented();
  assert.equal(down.has("Right"), true);
  assert.equal(down.has("Z"), true);
  assert.equal(down.has("Left"), false);

  bits = 16 | 4; // up + focus
  pad.presented();
  assert.equal(down.has("Right"), false);
  assert.equal(down.has("Z"), false);
  assert.equal(down.has("Up"), true);
  assert.equal(down.has("ShiftKey"), true);

  bits = -1;
  pad.presented();
  assert.deepEqual([...down], []);
});

test("cancel releases pad-owned keys without touching other owners", () => {
  const events = [];
  const keys = createKeyOwners((name, pressed) => events.push([name, pressed]));
  keys.setKey("Right", true, "key");
  const pad = createGamepadKeys({ keys, sample: () => 128 });
  pad.presented();
  pad.cancel();
  assert.equal(keys.isDown("Right"), true);
  assert.deepEqual(events.filter(([name]) => name === "Right"), [["Right", true]]);
});
