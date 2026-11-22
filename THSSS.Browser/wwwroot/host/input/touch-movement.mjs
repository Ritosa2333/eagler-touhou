// Playfield pixels drained at BaseMyPlane's own speed. A drag is queued and
// spent one step per logic frame, so the finger path is kept while held.
export const PLAYFIELD = Object.freeze({
  width: 640,
  height: 480,
  highSpeed: 4.5,
  lowSpeed: 2,
  diagonal: 0.71,
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
    const speed = focus ? field.lowSpeed : field.highSpeed;
    let stepX = queuedX > 0 ? speed : queuedX < 0 ? -speed : 0;
    let stepY = queuedY > 0 ? speed : queuedY < 0 ? -speed : 0;
    if (stepX !== 0 && stepY !== 0) {
      stepX *= field.diagonal;
      stepY *= field.diagonal;
    }
    const shiftX = Math.sign(queuedX) * Math.min(Math.abs(queuedX), Math.abs(stepX));
    const shiftY = Math.sign(queuedY) * Math.min(Math.abs(queuedY), Math.abs(stepY));
    queuedX -= shiftX;
    queuedY -= shiftY;
    return { shiftX, shiftY };
  }

  return {
    add,
    clear,
    drain,
    queued: () => ({ x: queuedX, y: queuedY }),
  };
}
