import { createMenuGestures } from "./menu-gestures.mjs";
import { createTouchMovement } from "./touch-movement.mjs";

const ARROWS = ["Left", "Right", "Up", "Down"];

function joystickMode(mode) {
  return mode === "joystick" || mode === "joystick-free";
}

// Launcher touch-controls and direct-touch, translated into the game's keys.
// Shooting follows fireEnabled and stays down until that latch is cleared.
export function createTouchBridge({ keys, surface, options, shift, now = () => performance.now() }) {
  const movement = createTouchMovement();
  const gestures = createMenuGestures();
  const pulses = [];
  let previous = null;
  let focus = false;
  let fire = false;

  function settings() {
    return options() || {};
  }

  function reported() {
    const value = surface();
    return value === "menu" || value === "dialogue" || value === "play" ? value : null;
  }

  function mode() {
    return reported() || "play";
  }

  function applyFire() {
    keys.setKey("Z", fire && reported() === "play", "touch-fire");
  }

  function sensitivity() {
    const value = Number(settings().touchSensitivity);
    return value > 0 ? value : 100;
  }

  function releaseArrows() {
    for (const name of ARROWS) keys.setKey(name, false, "direct");
  }

  function applyMenuHold(scene) {
    const held = gestures.held(scene);
    for (const name of ARROWS) keys.setKey(name, name === held, "menu");
  }

  function pulse(name) {
    keys.setKey(name, true, "gesture");
    pulses.push(name);
  }

  function releasePulses() {
    for (const name of pulses) keys.setKey(name, false, "gesture");
    pulses.length = 0;
  }

  function applyJoystick(x, y) {
    const axisX = Number(x) || 0;
    const axisY = Number(y) || 0;
    keys.setKey("Left", axisX <= -1, "joy");
    keys.setKey("Right", axisX >= 1, "joy");
    keys.setKey("Up", axisY <= -1, "joy");
    keys.setKey("Down", axisY >= 1, "joy");
  }

  function controls(message) {
    const next = message || {};
    fire = !!next.fireEnabled;
    applyFire();
    keys.setKey("ShiftKey", !!next.focusEnabled, "touch-focus");
    focus = !!next.focusEnabled;
    if (previous && next.bombSerial !== previous.bombSerial) pulse("X");
    if (previous && next.escapeSerial !== previous.escapeSerial) pulse("Escape");
    if (joystickMode(settings().touchMovementMode)) applyJoystick(next.joystickX, next.joystickY);
    else applyJoystick(0, 0);
    previous = {
      bombSerial: next.bombSerial,
      escapeSerial: next.escapeSerial,
    };
  }

  function syncSecondFinger() {
    const second = settings().touchFocusMode === "two-finger" && mode() === "play" && gestures.count() >= 2;
    keys.setKey("ShiftKey", second, "touch-second");
  }

  function direct(message) {
    if (!message) return;
    if (message.type === "up" && message.id == null) {
      gestures.cancel();
      movement.clear();
      releaseArrows();
      applyMenuHold(reported());
      keys.setKey("ShiftKey", false, "touch-second");
      keys.setKey("ControlKey", false, "dialogue-skip");
      return;
    }
    const x = Number(message.x);
    const y = Number(message.y);
    const scene = reported();
    const current = scene || "play";
    if (message.type === "down" && Number.isFinite(x) && Number.isFinite(y)) {
      gestures.down(message.id, x, y, now());
      syncSecondFinger();
      return;
    }
    if (message.type === "move" && Number.isFinite(x) && Number.isFinite(y)) {
      const step = gestures.move(message.id, x, y);
      if (step.primary && current === "play" && !joystickMode(settings().touchMovementMode)) {
        movement.add(step.dx, step.dy, sensitivity());
      }
      applyMenuHold(scene);
      syncSecondFinger();
      return;
    }
    if (message.type === "up") {
      const names = gestures.up(
        message.id,
        Number.isFinite(x) ? x : 0,
        Number.isFinite(y) ? y : 0,
        now(),
        scene,
        settings().doubleTapBombEnabled === true,
      );
      for (const name of names) pulse(name);
      applyMenuHold(scene);
      if (!gestures.skipping(now(), scene)) keys.setKey("ControlKey", false, "dialogue-skip");
      syncSecondFinger();
    }
  }

  function presented() {
    applyFire();
    releasePulses();
    const scene = reported();
    applyMenuHold(scene);
    const current = scene || "play";
    keys.setKey("ControlKey", gestures.skipping(now(), scene), "dialogue-skip");
    if (current !== "play" || joystickMode(settings().touchMovementMode)) {
      movement.clear();
      releaseArrows();
      return;
    }
    if (settings().touchMovementMode === "touch-unlimited" && typeof shift === "function") {
      const ahead = movement.queued();
      if ((ahead.x !== 0 || ahead.y !== 0) && shift(ahead.x, ahead.y) === true) {
        movement.clear();
        releaseArrows();
        return;
      }
    }
    const step = movement.drain({ focus });
    keys.setKey("Left", step.left, "direct");
    keys.setKey("Right", step.right, "direct");
    keys.setKey("Up", step.up, "direct");
    keys.setKey("Down", step.down, "direct");
  }

  function cancel() {
    const serials = previous || { bombSerial: 0, escapeSerial: 0 };
    controls({
      fireEnabled: false,
      focusEnabled: false,
      joystickX: 0,
      joystickY: 0,
      bombSerial: serials.bombSerial,
      escapeSerial: serials.escapeSerial,
    });
    gestures.cancel();
    movement.clear();
    releaseArrows();
    applyMenuHold(null);
    keys.setKey("ShiftKey", false, "touch-second");
    keys.setKey("ControlKey", false, "dialogue-skip");
  }

  return { controls, direct, presented, cancel };
}
