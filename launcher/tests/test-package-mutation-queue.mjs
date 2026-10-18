import assert from "node:assert/strict";

import { createPackageMutationQueue } from "../package/package-mutation-queue.mjs";

const queue = createPackageMutationQueue();
const events = [];
let releaseFirst;
const firstGate = new Promise(resolve => { releaseFirst = resolve; });

const first = queue.run("th07", async () => {
  events.push("first:start");
  await firstGate;
  events.push("first:end");
  return 1;
});
const second = queue.run("th07", async () => {
  events.push("second:start");
  return 2;
});
const otherGame = queue.run("th08", async () => {
  events.push("other:start");
  return 3;
});

await Promise.resolve();
assert.deepEqual(events, ["first:start", "other:start"], "different games may mutate independently");
releaseFirst();
assert.deepEqual(await Promise.all([first, second, otherGame]), [1, 2, 3]);
assert.deepEqual(events, ["first:start", "other:start", "first:end", "second:start"],
  "mutations for one game must run in submission order");

await assert.rejects(queue.run("th07", async () => { throw new Error("expected failure"); }), /expected failure/);
assert.equal(await queue.run("th07", async () => 4), 4, "a failed mutation must not poison the queue");

console.log("Package mutation queue: PASS");
