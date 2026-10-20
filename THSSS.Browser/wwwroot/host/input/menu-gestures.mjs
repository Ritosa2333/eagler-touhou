// Menu and dialogue gestures on the launcher's normalized direct-touch stream.
// The dead zone matches th06: 5% of the view height, and the same pixel span
// horizontally. While a menu finger stays past that line, its direction is
// held. Coming back inside releases it. A release inside the zone is one
// confirm edge, which is not the fire latch. A second stationary tap in play
// is Bomb when that option is on. Two fingers go back. A dialogue hold is
// the skip key.
const ARROWS = Object.freeze({ Left: true, Right: true, Up: true, Down: true });

export function createMenuGestures({
  threshold = 0.05,
  doubleTapMs = 300,
  doubleTapRadius = 0.08,
  holdSkipMs = 500,
  aspect = 640 / 480,
} = {}) {
  const fingers = new Map();
  let primary = null;
  let maxFingers = 0;
  let resolved = false;
  let started = 0;
  let lastTap = null;

  function distance(ax, ay, bx, by) {
    return Math.hypot((ax - bx) * aspect, ay - by);
  }

  function axis(finger) {
    return { dx: finger.x - finger.originX, dy: finger.y - finger.originY };
  }

  function direction(dx, dy) {
    const horizontal = Math.abs(dx) * aspect;
    const vertical = Math.abs(dy);
    if (horizontal <= threshold && vertical <= threshold) return null;
    if (horizontal > vertical) return dx > 0 ? "Right" : "Left";
    return dy > 0 ? "Down" : "Up";
  }

  function reset() {
    fingers.clear();
    primary = null;
    maxFingers = 0;
    resolved = false;
  }

  function down(id, x, y, now) {
    if (fingers.has(id)) return;
    fingers.set(id, { x, y, originX: x, originY: y });
    if (primary == null) {
      primary = id;
      maxFingers = 1;
      resolved = false;
      started = now;
      return;
    }
    maxFingers += 1;
  }

  function move(id, x, y) {
    const finger = fingers.get(id);
    if (!finger) return { dx: 0, dy: 0, primary: false };
    const dx = x - finger.x;
    const dy = y - finger.y;
    finger.x = x;
    finger.y = y;
    return { dx, dy, primary: id === primary };
  }

  function held(surface) {
    if (surface !== "menu" || primary == null) return null;
    const finger = fingers.get(primary);
    if (!finger) return null;
    const name = direction(finger.x - finger.originX, finger.y - finger.originY);
    return name && ARROWS[name] ? name : null;
  }

  function up(id, x, y, now, surface, doubleTapBomb) {
    const finger = fingers.get(id);
    if (!finger) return [];
    finger.x = x;
    finger.y = y;
    const wasPrimary = id === primary;
    const anchor = fingers.get(primary) || finger;
    const stationary = direction(anchor.x - anchor.originX, anchor.y - anchor.originY) == null;
    const pulses = [];
    if (!resolved && maxFingers >= 2 && stationary && surface !== "play") {
      pulses.push("X");
      resolved = true;
    } else if (!resolved && wasPrimary && maxFingers < 2 && stationary) {
      const doubled = lastTap
        && now - lastTap.at <= doubleTapMs
        && distance(finger.originX, finger.originY, lastTap.x, lastTap.y) <= doubleTapRadius;
      const heldFor = now - started;
      if (doubled && surface === "play" && doubleTapBomb) {
        pulses.push("X");
        lastTap = null;
      } else if (surface !== "play" && heldFor < holdSkipMs) {
        pulses.push("Z");
        lastTap = { x: finger.originX, y: finger.originY, at: now };
      } else if (surface === "play") {
        lastTap = { x: finger.originX, y: finger.originY, at: now };
      }
      resolved = true;
    }
    fingers.delete(id);
    if (wasPrimary) primary = null;
    if (fingers.size === 0) reset();
    return pulses;
  }

  function skipping(now, surface) {
    if (surface !== "dialogue" || primary == null) return false;
    const finger = fingers.get(primary);
    if (!finger) return false;
    const { dx, dy } = axis(finger);
    return direction(dx, dy) == null && now - started >= holdSkipMs;
  }

  return {
    down,
    move,
    up,
    held,
    skipping,
    cancel: reset,
    count: () => fingers.size,
  };
}
