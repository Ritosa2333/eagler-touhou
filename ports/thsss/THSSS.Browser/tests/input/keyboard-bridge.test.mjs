import assert from "node:assert/strict";
import { createKeyOwners } from "../../wwwroot/host/input/key-owners.mjs";
import test from "node:test";

import { createKeyboardInputBridge } from "../../wwwroot/host/input/keyboard-input-bridge.mjs";
import { keyNameFromEvent } from "../../wwwroot/directory-keyboard.mjs";

test("WASD aliases become arrow names after the launcher mapping port lands", () => {
  const target = {};
  const applied = [];
  createKeyboardInputBridge({
    target,
    protocol: "eagler-touhou/1",
    game: "thsss",
    epoch: 1,
    origin: "http://local",
    ready: () => true,
    dispatch(event, down) {
      applied.push([keyNameFromEvent(event), down]);
    },
    reset() {},
  });
  target.__eaglerKeyboardMapping = {
    schema: "eagler-touhou/keyboard-input/1",
    protocol: "eagler-touhou/1",
    game: "thsss",
    epoch: 1,
    origin: "http://local",
    supports: () => true,
    clear: () => [],
    map(event, down) {
      const aliases = { KeyW: "ArrowUp", KeyA: "ArrowLeft", KeyS: "ArrowDown", KeyD: "ArrowRight" };
      const code = aliases[event.code] || event.code;
      return [{ code, key: code, keyCode: 0, location: 0, down }];
    },
  };
  assert.equal(target.__eaglerKeyboardInputBridge.submit({
    protocol: "eagler-touhou/1",
    game: "thsss",
    epoch: 1,
    command: "keyboard",
    code: "KeyW",
    key: "w",
    keyCode: 87,
    down: true,
    inputSource: "physical",
  }), true);
  assert.deepEqual(applied, [["Up", true]]);
});

test("raw KeyW stays a letter when the shell skips submit", () => {
  assert.equal(keyNameFromEvent({ code: "KeyW", key: "w" }), "W");
  assert.equal(keyNameFromEvent({ code: "ArrowUp", key: "ArrowUp" }), "Up");
});
test("fast menu presses survive one logic step and held controls release immediately afterwards", () => {
  const events = [];
  const keys = createKeyOwners((key, down) => events.push([key, down]), { sampleTaps: true });
  for (const key of ["Z", "X", "Up", "Down", "Left", "Right"]) {
    keys.setKey(key, true, "key");
    keys.setKey(key, false, "key");
    assert.equal(keys.isDown(key), true);
    keys.finishStep();
    assert.equal(keys.isDown(key), false);
    assert.deepEqual(events.splice(0), [[key, true], [key, false]]);
  }
  keys.setKey("Z", true, "key");
  keys.setKey("Return", true, "key");
  keys.finishStep();
  keys.setKey("Return", false, "key");
  assert.equal(keys.isDown("Z"), true);
  keys.setKey("Z", false, "key");
  assert.equal(keys.isDown("Z"), false);
  keys.setKey("X", true, "key");
  keys.setKey("X", false, "key");
  keys.releaseSource("key");
  assert.equal(keys.isDown("X"), false);
});
