import assert from "node:assert/strict";
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
