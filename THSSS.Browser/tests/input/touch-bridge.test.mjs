import assert from "node:assert/strict";
import test from "node:test";
import { createKeyOwners } from "../../wwwroot/host/input/key-owners.mjs";
import { createTouchBridge } from "../../wwwroot/host/input/touch-bridge.mjs";

function harness(options = {}, surface = "play") {
  const sent = [];
  const keys = createKeyOwners((name, down) => sent.push([name, down]));
  const shifts = [];
  const scene = { value: surface };
  let clock = 0;
  const bridge = createTouchBridge({
    keys,
    surface: () => scene.value,
    options: () => options,
    shift(x, y) {
      shifts.push([x, y]);
      return true;
    },
    now: () => clock,
  });
  return {
    sent,
    shifts,
    keys,
    bridge,
    at(value) { clock = value; },
    scene,
  };
}

test("the fire latch holds Z only on a reported play surface", () => {
  const menu = harness({}, "menu");
  menu.bridge.controls({ fireEnabled: true, focusEnabled: false, bombSerial: 0, escapeSerial: 0, joystickX: 0, joystickY: 0 });
  assert.equal(menu.keys.isDown("Z"), false);
  const unknown = harness({}, null);
  unknown.bridge.controls({ fireEnabled: true, focusEnabled: false, bombSerial: 0, escapeSerial: 0, joystickX: 0, joystickY: 0 });
  assert.equal(unknown.keys.isDown("Z"), false);
});

test("shooting stays on until the fire latch is cleared", () => {
  const { bridge, keys, sent } = harness();
  bridge.controls({ fireEnabled: true, focusEnabled: false, bombSerial: 0, escapeSerial: 0, joystickX: 0, joystickY: 0 });
  bridge.direct({ type: "up", id: 1, x: 0.5, y: 0.5 });
  bridge.presented();
  assert.equal(keys.isDown("Z"), true);
  bridge.controls({ fireEnabled: false, focusEnabled: false, bombSerial: 0, escapeSerial: 0, joystickX: 0, joystickY: 0 });
  assert.equal(keys.isDown("Z"), false);
  assert.deepEqual(sent.filter(([name]) => name === "Z"), [["Z", true], ["Z", false]]);
  const resumed = harness();
  resumed.bridge.controls({ fireEnabled: true, focusEnabled: false, bombSerial: 0, escapeSerial: 0, joystickX: 0, joystickY: 0 });
  resumed.scene.value = "menu";
  resumed.bridge.presented();
  assert.equal(resumed.keys.isDown("Z"), false);
  resumed.scene.value = "play";
  resumed.bridge.presented();
  assert.equal(resumed.keys.isDown("Z"), true);
});

test("a play drag moves along the path and a joystick uses the launcher axis", () => {
  const play = harness({ touchMovementMode: "touch", touchSensitivity: 100 });
  play.bridge.direct({ type: "down", id: 1, x: 0.2, y: 0.5 });
  play.bridge.direct({ type: "move", id: 1, x: 0.3, y: 0.5 });
  play.bridge.direct({ type: "up", id: 1, x: 0.3, y: 0.5 });
  play.bridge.presented();
  assert.equal(play.keys.isDown("Right"), true);
  let guard = 0;
  while (play.keys.isDown("Right") && guard < 40) {
    play.bridge.presented();
    guard += 1;
  }
  assert.equal(play.keys.isDown("Right"), false);

  const stick = harness({ touchMovementMode: "joystick" });
  stick.bridge.controls({ fireEnabled: false, focusEnabled: false, bombSerial: 0, escapeSerial: 0, joystickX: 100, joystickY: 0 });
  assert.equal(stick.keys.isDown("Right"), true);
  stick.bridge.controls({ fireEnabled: false, focusEnabled: false, bombSerial: 0, escapeSerial: 0, joystickX: 0, joystickY: 0 });
  assert.equal(stick.keys.isDown("Right"), false);
});

test("menu tap, swipe, and play double-tap use one edge each", () => {
  const menu = harness({ doubleTapBombEnabled: true }, "menu");
  menu.bridge.direct({ type: "down", id: 1, x: 0.5, y: 0.5 });
  menu.bridge.direct({ type: "move", id: 1, x: 0.5, y: 0.7 });
  assert.equal(menu.keys.isDown("Down"), true);
  menu.bridge.presented();
  assert.equal(menu.keys.isDown("Down"), false);
  menu.bridge.presented();
  assert.equal(menu.keys.isDown("Down"), false);
  menu.bridge.direct({ type: "up", id: 1, x: 0.5, y: 0.7 });

  menu.at(1000);
  menu.bridge.direct({ type: "down", id: 2, x: 0.2, y: 0.2 });
  menu.at(1100);
  menu.bridge.direct({ type: "up", id: 2, x: 0.2, y: 0.2 });
  assert.equal(menu.keys.isDown("Z"), true);
  menu.bridge.presented();
  assert.equal(menu.keys.isDown("Z"), false);

  const play = harness({ doubleTapBombEnabled: true, touchMovementMode: "touch" }, "play");
  play.bridge.direct({ type: "down", id: 1, x: 0.4, y: 0.4 });
  play.at(30);
  play.bridge.direct({ type: "up", id: 1, x: 0.4, y: 0.4 });
  play.at(80);
  play.bridge.direct({ type: "down", id: 2, x: 0.41, y: 0.4 });
  play.at(100);
  play.bridge.direct({ type: "up", id: 2, x: 0.41, y: 0.4 });
  assert.equal(play.keys.isDown("X"), true);
  assert.equal(play.keys.isDown("Z"), false);
});

test("unlimited touch places the ship when the host accepts the shift", () => {
  const { bridge, shifts, keys } = harness({ touchMovementMode: "touch-unlimited", touchSensitivity: 100 });
  bridge.direct({ type: "down", id: 1, x: 0.2, y: 0.5 });
  bridge.direct({ type: "move", id: 1, x: 0.3, y: 0.5 });
  bridge.presented();
  assert.equal(shifts.length, 1);
  assert.ok(Math.abs(shifts[0][0] - 64) < 0.001);
  assert.equal(shifts[0][1], 0);
  assert.equal(keys.isDown("Right"), false);
});
