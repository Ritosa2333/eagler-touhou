// When the launcher hides #touchDirectSurface for joystick modes, menu and
// dialogue gestures still need a finger stream. Capture on the game canvas
// (th06-style in-iframe touch) and feed the same normalized direct() path.
// Non-joystick modes leave capture off so the launcher surface stays sole owner.

function joystickMode(mode) {
  return mode === "joystick" || mode === "joystick-free";
}

export function bindCanvasDirectTouch({ canvas, direct, options }) {
  if (!canvas || typeof direct !== "function" || typeof options !== "function") {
    return { dispose() {}, cancel() {} };
  }

  const points = new Map();
  let rect = null;

  function enabled() {
    return joystickMode((options() || {}).touchMovementMode);
  }

  function invalidate() {
    rect = null;
  }

  function measure(force) {
    if (force || !rect) {
      const next = canvas.getBoundingClientRect();
      if (!(next.width > 0) || !(next.height > 0)) return null;
      rect = { left: next.left, top: next.top, width: next.width, height: next.height };
    }
    return rect;
  }

  function point(event, force = false) {
    const box = measure(force);
    if (!box) return null;
    return {
      x: (event.clientX - box.left) / box.width,
      y: (event.clientY - box.top) / box.height,
    };
  }

  function onDown(event) {
    if (!enabled() || event.pointerType === "mouse" && event.button !== 0) return;
    if (points.has(event.pointerId)) return;
    const p = point(event, points.size === 0);
    if (!p) return;
    event.preventDefault();
    const contact = { id: event.pointerId, ...p };
    points.set(event.pointerId, contact);
    try {
      canvas.setPointerCapture(event.pointerId);
    } catch {
      // Ignore browsers that reject capture on this element.
    }
    direct({ type: "down", id: contact.id, x: contact.x, y: contact.y });
  }

  function onMove(event) {
    const contact = points.get(event.pointerId);
    if (!contact) return;
    const p = point(event);
    if (!p) return;
    event.preventDefault();
    Object.assign(contact, p);
    direct({ type: "move", id: contact.id, x: contact.x, y: contact.y });
  }

  function onUp(event) {
    const contact = points.get(event.pointerId);
    if (!contact) return;
    event.preventDefault();
    const p = Number.isFinite(event.clientX) ? point(event) : null;
    if (p) Object.assign(contact, p);
    points.delete(event.pointerId);
    direct({ type: "up", id: contact.id, x: contact.x, y: contact.y });
    if (!points.size) invalidate();
  }

  function cancel() {
    if (!points.size) return;
    points.clear();
    invalidate();
    direct({ type: "up", id: null });
  }

  const opts = { passive: false };
  canvas.addEventListener("pointerdown", onDown, opts);
  canvas.addEventListener("pointermove", onMove, opts);
  canvas.addEventListener("pointerup", onUp, opts);
  canvas.addEventListener("pointercancel", onUp, opts);
  canvas.addEventListener("lostpointercapture", onUp, opts);
  window.addEventListener("resize", invalidate, { passive: true });
  window.visualViewport?.addEventListener("resize", invalidate, { passive: true });

  return {
    cancel,
    dispose() {
      cancel();
      canvas.removeEventListener("pointerdown", onDown, opts);
      canvas.removeEventListener("pointermove", onMove, opts);
      canvas.removeEventListener("pointerup", onUp, opts);
      canvas.removeEventListener("pointercancel", onUp, opts);
      canvas.removeEventListener("lostpointercapture", onUp, opts);
      window.removeEventListener("resize", invalidate);
      window.visualViewport?.removeEventListener("resize", invalidate);
    },
  };
}
