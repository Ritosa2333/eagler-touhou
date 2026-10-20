// Menu and dialogue gestures on the launcher's normalized direct-touch stream.
// A swipe advances one item. A stationary tap confirms. A second stationary
// tap in play is Bomb when that option is on. Two fingers go back. A dialogue
// hold is the skip key.
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
  let swipeSent = false;
  let resolved = false;
  let started = 0;
  let lastTap = null;

  function distance(ax, ay, bx, by) {
    return Math.hypot((ax - bx) * aspect, ay - by);
  }

  function travel(finger) {
    return distance(finger.x, finger.y, finger.originX, finger.originY);
  }

  function direction(dx, dy) {
    const horizontal = Math.abs(dx) * aspect;
    const vertical = Math.abs(dy);
    if (horizontal < threshold && vertical < threshold) return null;
    if (horizontal > vertical) return dx > 0 ? "Right" : "Left";
    return dy > 0 ? "Down" : "Up";
  }

  function reset() {
    fingers.clear();
    primary = null;
    maxFingers = 0;
    swipeSent = false;
    resolved = false;
  }

  function down(id, x, y, now) {
    if (fingers.has(id)) return;
    fingers.set(id, { x, y, originX: x, originY: y });
    if (primary == null) {
      primary = id;
      maxFingers = 1;
      swipeSent = false;
      resolved = false;
      started = now;
      return;
    }
    maxFingers += 1;
  }

  function move(id, x, y, surface) {
    const finger = fingers.get(id);
    if (!finger) return { dx: 0, dy: 0, pulse: null, primary: false };
    const dx = x - finger.x;
    const dy = y - finger.y;
    finger.x = x;
    finger.y = y;
    let pulse = null;
    if (id === primary && !swipeSent) {
      const name = direction(finger.x - finger.originX, finger.y - finger.originY);
      if (name && ARROWS[name]) {
        swipeSent = true;
        if (surface === "menu") pulse = name;
      }
    }
    return { dx, dy, pulse, primary: id === primary };
  }

  function up(id, x, y, now, surface, doubleTapBomb) {
    const finger = fingers.get(id);
    if (!finger) return [];
    finger.x = x;
    finger.y = y;
    const wasPrimary = id === primary;
    const stationary = !swipeSent && travel(finger) <= threshold;
    const pulses = [];
    if (!resolved && maxFingers >= 2 && stationary && surface !== "play") {
      pulses.push("X");
      resolved = true;
    } else if (!resolved && wasPrimary && maxFingers < 2 && stationary) {
      const doubled = lastTap
        && now - lastTap.at <= doubleTapMs
        && distance(finger.originX, finger.originY, lastTap.x, lastTap.y) <= doubleTapRadius;
      const held = now - started;
      if (doubled && surface === "play" && doubleTapBomb) {
        pulses.push("X");
        lastTap = null;
      } else if ((surface === "menu" || surface === "dialogue") && held < holdSkipMs) {
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
    if (surface !== "dialogue" || primary == null || swipeSent) return false;
    const finger = fingers.get(primary);
    if (!finger) return false;
    return travel(finger) <= threshold && now - started >= holdSkipMs;
  }

  return {
    down,
    move,
    up,
    skipping,
    cancel: reset,
    count: () => fingers.size,
  };
}
