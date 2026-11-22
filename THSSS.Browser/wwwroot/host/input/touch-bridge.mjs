import { createMenuGestures } from "./menu-gestures.mjs";
import { createTouchMovement, PLAYFIELD } from "./touch-movement.mjs";

const ARROWS = ["Left", "Right", "Up", "Down"];

function joystickMode(mode) {
  return mode === "joystick" || mode === "joystick-free";
}

function quantizeStick(nx, ny) {
  if (nx === 0 && ny === 0) return { x: 0, y: 0 };
  const angle = Math.atan2(ny, nx);
  const step = Math.PI / 4;
  const snapped = Math.round(angle / step) * step;
  return { x: Math.cos(snapped), y: Math.sin(snapped) };
}

// Launcher touch-controls and direct-touch. Play movement goes through shift
// (ApplyTouch); shooting follows fireEnabled until that latch is cleared.
export function createTouchBridge({ keys, surface, options, shift, now = () => performance.now() }) {
  const movement = createTouchMovement();
  const gestures = createMenuGestures();
  const pulses = [];
  let previous = null;
  let focus = false;
  let fire = false;
  let stickX = 0;
  let stickY = 0;
  let primaryId = null;

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
    for (const name of pulses) keys.setKey(name, false, "gesture");
    pulses.length = 0;
  }

  function playSpeed() {
    return focus ? PLAYFIELD.lowSpeed : PLAYFIELD.highSpeed;
  }

  function applyStickShift() {
    if (typeof shift !== "function") return;
    const nx = stickX / 32767;
    const ny = stickY / 32767;
    const mag = Math.hypot(nx, ny);
    if (mag < 1e-6) return;
    const modeName = settings().touchMovementMode;
    let ux = nx;
    let uy = ny;
    let scale = mag;
    if (modeName === "joystick") {
      const q = quantizeStick(nx, ny);
      ux = q.x;
      uy = q.y;
      scale = 1;
      if (ux !== 0 && uy !== 0) scale = PLAYFIELD.diagonal;
    } else {
      ux = nx / mag;
      uy = ny / mag;
    }
    const speed = playSpeed() * (modeName === "joystick-free" ? mag : scale);
    const dx = ux * speed;
    const dy = uy * speed;
    if (dx !== 0 || dy !== 0) shift(dx, dy);
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
    if (current !== "play") {
      movement.clear();
      return;
    }
    if (joystickMode(settings().touchMovementMode)) {
      movement.clear();
      applyStickShift();
      return;
    }
    const unlimited = settings().touchMovementMode === "touch-unlimited";
    const step = movement.drain({ focus, unlimited });
    if ((step.shiftX !== 0 || step.shiftY !== 0) && typeof shift === "function") {
      shift(step.shiftX, step.shiftY);
    }
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
    releaseArrows();
    applyMenuHold(null);
    keys.setKey("ShiftKey", false, "touch-second");
    keys.setKey("ControlKey", false, "dialogue-skip");
  }

  return { controls, direct, presented, cancel };
}
