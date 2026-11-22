import assert from "node:assert/strict";
import { readFileSync } from "node:fs";
import { advanceFrame } from "../wwwroot/host/presentation/frame-step.mjs";

const rates = [60, 75, 90, 120, 144, 165, 240];

function run(hz, seconds, unlocked) {
  let acc = 0;
  let logic = 0;
  let draws = 0;
  const delta = 1000 / hz;
  for (let i = 0; i < hz * seconds; i++) {
    const stepped = advanceFrame({ acc, deltaMs: delta, unlocked });
    acc = stepped.acc;
    if (stepped.logic) logic += 1;
    if (stepped.draw) draws += 1;
  }
  return { logic, draws };
}

for (const hz of rates) {
  const unlocked = run(hz, 10, true);
  const locked = run(hz, 10, false);
  assert.equal(unlocked.logic, locked.logic, `${hz} Hz logic is independent of the refresh rate`);
  assert.ok(Math.abs(unlocked.logic - 600) <= 1, `${hz} Hz stays on the 60 Hz logic step`);
  assert.equal(unlocked.draws, hz * 10, `${hz} Hz presents every refresh`);
  assert.equal(locked.draws, locked.logic, `${hz} Hz locked presentation stays on the logic step`);
}
assert.equal(run(60, 10, true).logic, 600);
assert.equal(run(60, 10, false).logic, 600);

let acc = 4;
for (let i = 0; i < 30; i++) {
  const stepped = advanceFrame({ acc, deltaMs: 0, unlocked: true });
  assert.equal(stepped.logic, false);
  assert.equal(stepped.draw, true);
  assert.equal(stepped.acc, acc);
  acc = stepped.acc;
}

const source = readFileSync(new URL("../wwwroot/main.js", import.meta.url), "utf8");
const loop = source.slice(source.indexOf("const frame = (now)"));
assert.match(loop, /advanceFrame\(/);
assert.match(loop, /host\.Simulate\(\)/);
assert.match(loop, /host\.Draw\(acc \/ LOGIC_STEP_MS\)/);
assert.match(loop, /host\.Frame\(\)/);
