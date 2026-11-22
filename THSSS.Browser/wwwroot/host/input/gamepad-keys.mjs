// Launcher owns device mapping. Runtime only samples the semantic bitfield
// each frame and publishes the same key names the keyboard path uses.
// Bit order matches launcher gamepadActions: shoot bomb focus pause up down
// left right skip confirm action restart.
const PAD = Object.freeze([
  [1, "Z"],
  [2, "X"],
  [4, "ShiftKey"],
  [8, "Escape"],
  [16, "Up"],
  [32, "Down"],
  [64, "Left"],
  [128, "Right"],
  [256, "ControlKey"],
  [512, "Return"],
  [1024, "C"],
]);

export function createGamepadKeys({
  keys,
  sample = () => {
    const bridge = globalThis.__eaglerGamepadInputBridge;
    if (!bridge || typeof bridge.sample !== "function") return -1;
    return bridge.sample(0);
  },
}) {
  function release() {
    for (const [, name] of PAD) keys.setKey(name, false, "pad");
  }

  function presented() {
    const bits = sample();
    if (!Number.isInteger(bits) || bits < 0) {
      release();
      return;
    }
    for (const [mask, name] of PAD) keys.setKey(name, !!(bits & mask), "pad");
  }

  return { presented, cancel: release };
}
