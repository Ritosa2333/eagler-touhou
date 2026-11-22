// Fixed 60 Hz logic, with an optional draw on every display refresh.
// One callback advances logic at most once. Extra draws reuse the same step.

export const LOGIC_STEP_MS = 1000 / 60;

export function advanceFrame({ acc, deltaMs, unlocked }) {
  const step = LOGIC_STEP_MS;
  let next = acc + Math.min(100, deltaMs);
  const logic = next >= step;
  if (logic) {
    next -= step;
    if (unlocked) {
      if (next > step) next = step;
    } else if (next > step * 2) {
      next = 0;
    }
  }
  return { acc: next, logic, draw: unlocked || logic };
}
