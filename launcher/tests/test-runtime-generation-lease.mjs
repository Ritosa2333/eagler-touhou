import assert from "node:assert/strict";

import { createManagedRuntimeGenerationLease } from "../.cache/build/browser/assets/launcher/runtime-generation-lease.mjs";

const generationA = { id: "generation-a", game: "th08" };
const generationB = { id: "generation-b", game: "th08" };
const lease = createManagedRuntimeGenerationLease();

lease.bind("th08", generationA);
assert.equal(lease.resolve({ game: "th08", generation: "generation-a" }), generationA);

// Installing optional OGG files may advance Package Store to generation B,
// but the already-loaded Runtime must retain its generation-A DATA lease.
assert.equal(lease.resolve({ game: "th08", generation: "generation-a" }), generationA);
assert.throws(
  () => lease.resolve({ game: "th08", generation: generationB.id }),
  /inactive game generation/,
);

lease.clear();
assert.throws(
  () => lease.resolve({ game: "th08", generation: generationA.id }),
  /inactive game generation/,
);

lease.bind("th08", generationB);
assert.equal(lease.resolve({ game: "th08", generation: generationB.id }), generationB);
assert.throws(
  () => lease.resolve({ game: "th07", generation: generationB.id }),
  /inactive game generation/,
);
assert.throws(
  () => lease.bind("th08", { id: "wrong-game", game: "th07" }),
  /invalid game generation/,
);

console.log("Managed Runtime generation lease: PASS");
