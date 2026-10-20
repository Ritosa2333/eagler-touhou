// Playfield pixels drained at BaseMyPlane's own speed. A drag is queued and
// spent one step per logic frame, so the finger path is kept and the ship
// does not keep flying after the finger stops.
export const PLAYFIELD = Object.freeze({
  width: 640,
  height: 480,
  highSpeed: 4.5,
  lowSpeed: 2,
  diagonal: 0.71,
});

const EMPTY = Object.freeze({
  left: false, right: false, up: false, down: false, shiftX: 0, shiftY: 0,
});

export function createTouchMovement(field = PLAYFIELD) {
  let queuedX = 0;
  let queuedY = 0;

  function add(dx, dy, sensitivity = 100) {
    const scale = Number(sensitivity) > 0 ? Number(sensitivity) / 100 : 1;
    queuedX += dx * field.width * scale;
    queuedY += dy * field.height * scale;
  }

  function clear() {
    queuedX = 0;
    queuedY = 0;
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
      return { left: false, right: false, up: false, down: false, shiftX, shiftY };
    }
    const speed = focus ? field.lowSpeed : field.highSpeed;
    let stepX = queuedX > 0 ? speed : queuedX < 0 ? -speed : 0;
    let stepY = queuedY > 0 ? speed : queuedY < 0 ? -speed : 0;
    if (stepX !== 0 && stepY !== 0) {
      stepX *= field.diagonal;
      stepY *= field.diagonal;
    }
    queuedX -= Math.sign(queuedX) * Math.min(Math.abs(queuedX), Math.abs(stepX));
    queuedY -= Math.sign(queuedY) * Math.min(Math.abs(queuedY), Math.abs(stepY));
    return {
      left: stepX < 0,
      right: stepX > 0,
      up: stepY < 0,
      down: stepY > 0,
      shiftX: 0,
      shiftY: 0,
    };
  }

  return {
    add,
    clear,
    drain,
    queued: () => ({ x: queuedX, y: queuedY }),
  };
}
