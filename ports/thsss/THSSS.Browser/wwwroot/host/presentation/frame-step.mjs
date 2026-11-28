// Keep game time at 60 Hz even when the display misses a refresh.

export const LOGIC_STEP_MS = 1000 / 60;
const MAX_STEPS = 3;
const CLOCK_EPSILON_MS = 1e-6;

export function advanceFrame({ acc, deltaMs, unlocked }) {
  const elapsed = acc + Math.min(LOGIC_STEP_MS * MAX_STEPS, Math.max(0, deltaMs));
  const steps = Math.floor((elapsed + CLOCK_EPSILON_MS) / LOGIC_STEP_MS);
  return { acc: Math.max(0, elapsed - steps * LOGIC_STEP_MS), steps, draw: unlocked || steps > 0 };
}
