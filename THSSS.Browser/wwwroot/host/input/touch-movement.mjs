// Playfield pixels drained at BaseMyPlane's own speed. A drag is queued in
// high-speed units; each logic frame consumes only the step that Move can walk
// (th06-style euclid clamp + focus remapping of the remainder).
export const PLAYFIELD = Object.freeze({
  width: 640,
  height: 480,
  highSpeed: 4.5,
  lowSpeed: 2,
});

const EMPTY = Object.freeze({ shiftX: 0, shiftY: 0 });

export function createTouchMovement(field = PLAYFIELD) {
  let queuedX = 0;
  let queuedY = 0;

  function add(dx, dy, sensitivity = 150) {
    const scale = Number(sensitivity) > 0 ? Number(sensitivity) / 100 : 1;
    queuedX += dx * field.width * scale;
    queuedY += dy * field.height * scale;
  }

  function clear() {
    queuedX = 0;
    queuedY = 0;
  }

  function restore(x, y) {
    queuedX = x;
    queuedY = y;
  }

  function credit(dx, dy, ratio) {
    const scale = ratio > 0 ? ratio : 1;
    queuedX += dx / scale;
    queuedY += dy / scale;
  }

  function drain({ focus = false, unlimited = false } = {}) {
    if (Math.abs(queuedX) < 0.05 && Math.abs(queuedY) < 0.05) {
      queuedX = 0;
      queuedY = 0;
      return EMPTY;
    }
    if (unlimited) {
      const shiftX = queuedX;
      const shiftY = queuedY;
      queuedX = 0;
      queuedY = 0;
      return { shiftX, shiftY };
    }
    const focusRatio = focus && field.highSpeed !== 0 ? field.lowSpeed / field.highSpeed : 1;
    const reqX = queuedX * focusRatio;
    const reqY = queuedY * focusRatio;
    const reqMag = Math.hypot(reqX, reqY);
    const maxSpeed = focus ? field.lowSpeed : field.highSpeed;
    if (reqMag > maxSpeed && reqMag > 0) {
      const scale = maxSpeed / reqMag;
      const shiftX = reqX * scale;
      const shiftY = reqY * scale;
      if (focusRatio !== 0) {
        queuedX -= shiftX / focusRatio;
        queuedY -= shiftY / focusRatio;
      } else {
        queuedX = 0;
        queuedY = 0;
      }
      return { shiftX, shiftY };
    }
    queuedX = 0;
    queuedY = 0;
    return { shiftX: reqX, shiftY: reqY };
  }

  return {
    add,
    clear,
    restore,
    credit,
    drain,
    queued: () => ({ x: queuedX, y: queuedY }),
  };
}
