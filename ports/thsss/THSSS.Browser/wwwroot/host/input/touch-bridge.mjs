import { createMenuGestures } from "./menu-gestures.mjs";
import { createTouchMovement, PLAYFIELD } from "./touch-movement.mjs";

const ARROWS = ["Left", "Right", "Up", "Down"];
const STICK_MIDPOINT = 16384;

function joystickMode(mode) {
  return mode === "joystick" || mode === "joystick-free";
}

// Launcher touch-controls and direct-touch. Direct drag and free stick go
// through shift (ApplyTouch); 8-way stick synthesizes arrow keys like th06.
export function createTouchBridge({ keys, surface, options, shift, residual, now = () => performance.now() }) {
  const movement = createTouchMovement();
  const gestures = createMenuGestures();
  // Gesture edges stay down through the next presented frame (onBeforeFrame →
  // Frame), then release. Releasing at the start of that same presented call
  // cleared Z/X/Escape before the game sampled them, so menu taps never
  // confirmed. th06 keeps pendingButton until the frame OR's it in.
  const pulses = [];
  let heldPulses = [];
  let previous = null;
  let focus = false;
  let fire = false;
  let stickX = 0;
  let stickY = 0;
  let primaryId = null;
  let stepRatio = 1;

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
    keys.setKey("Z", settings().touchEnabled !== false && fire && reported() === "play", "touch-fire");
  }

  function sensitivity() {
    const value = Number(settings().touchSensitivity);
    if (!(value >= 100 && value <= 300)) return 150;
    return Math.max(100, Math.min(300, value));
  }

  function releaseArrows() {
    for (const name of ARROWS) {
      keys.setKey(name, false, "direct");
      keys.setKey(name, false, "joy");
    }
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
    for (const name of heldPulses) keys.setKey(name, false, "gesture");
    heldPulses = pulses.splice(0);
  }

  function playSpeed() {
    return focus ? PLAYFIELD.lowSpeed : PLAYFIELD.highSpeed;
  }

  function applyStickKeys() {
    keys.setKey("Right", stickX > STICK_MIDPOINT, "joy");
    keys.setKey("Left", stickX < -STICK_MIDPOINT, "joy");
    keys.setKey("Down", stickY > STICK_MIDPOINT, "joy");
    keys.setKey("Up", stickY < -STICK_MIDPOINT, "joy");
  }

  function applyFreeStickShift() {
    if (typeof shift !== "function") return;
    const nx = stickX / 32767;
    const ny = stickY / 32767;
    if (Math.abs(nx) < 1e-6 && Math.abs(ny) < 1e-6) return;
    const speed = playSpeed();
    const dx = nx * speed;
    const dy = ny * speed;
    if (dx !== 0 || dy !== 0) shift(dx, dy, false);
  }

  function controls(message) {
    const next = message || {};
    fire = !!next.fireEnabled;
    applyFire();
    keys.setKey("ShiftKey", !!next.focusEnabled, "touch-focus");
    focus = !!next.focusEnabled;
    if (previous && next.bombSerial !== previous.bombSerial) pulse("X");
    if (previous && next.escapeSerial !== previous.escapeSerial) pulse("Escape");
    if (joystickMode(settings().touchMovementMode)) {
      stickX = Number(next.joystickX) || 0;
      stickY = Number(next.joystickY) || 0;
    } else {
      stickX = 0;
      stickY = 0;
    }
    releaseArrows();
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
      primaryId = null;
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
      if (primaryId == null) primaryId = message.id;
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
      const wasPrimary = message.id === primaryId;
      const names = gestures.up(
        message.id,
        Number.isFinite(x) ? x : 0,
        Number.isFinite(y) ? y : 0,
        now(),
        scene,
        settings().doubleTapBombEnabled === true,
      );
      for (const name of names) pulse(name);
      if (wasPrimary) {
        movement.clear();
        primaryId = null;
      }
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
    releaseArrows();
    const modeName = settings().touchMovementMode;
    // Menus need arrow keys; free-stick shifts stay play-only.
    if (current !== "play") {
      movement.clear();
      if (joystickMode(modeName)) applyStickKeys();
      return;
    }
    if (modeName === "joystick") {
      movement.clear();
      applyStickKeys();
      return;
    }
    if (modeName === "joystick-free") {
      movement.clear();
      applyFreeStickShift();
      return;
    }
    const unlimited = modeName === "touch-unlimited";
    if (typeof residual === "function") {
      const left = residual();
      if (left && (left.x || left.y)) movement.credit(left.x, left.y, stepRatio);
    }
    const before = movement.queued();
    const ratio = focus ? PLAYFIELD.lowSpeed / PLAYFIELD.highSpeed : 1;
    const step = movement.drain({ focus, unlimited });
    if (step.shiftX === 0 && step.shiftY === 0) return;
    if (typeof shift !== "function") return;
    stepRatio = ratio;
    const ok = shift(step.shiftX, step.shiftY, unlimited);
    if (ok === false) movement.restore(before.x, before.y);
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
    primaryId = null;
    for (const name of heldPulses) keys.setKey(name, false, "gesture");
    heldPulses = [];
    for (const name of pulses) keys.setKey(name, false, "gesture");
    pulses.length = 0;
    releaseArrows();
    applyMenuHold(null);
    keys.setKey("ShiftKey", false, "touch-second");
    keys.setKey("ControlKey", false, "dialogue-skip");
  }

  return { controls, direct, presented, cancel };
}
