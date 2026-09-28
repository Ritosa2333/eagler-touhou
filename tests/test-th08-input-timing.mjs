import assert from "node:assert/strict";
import { recommendTh08InputTiming } from "../.cache/build/browser/assets/launcher/th08-input-timing.mjs";

assert.deepEqual(recommendTh08InputTiming(0, 100, 0),
  { inputDelay: 0, targetRollbackFrames: 8, networkFrames: 0, mobileSeats: 0 });
assert.deepEqual(recommendTh08InputTiming(1, 100, 0),
  { inputDelay: 2, targetRollbackFrames: 4, networkFrames: 6, mobileSeats: 1 });
assert.deepEqual(recommendTh08InputTiming(2, 100, 0),
  { inputDelay: 4, targetRollbackFrames: 2, networkFrames: 6, mobileSeats: 2 });
assert.equal(recommendTh08InputTiming(2, null, null).inputDelay, 6);
assert.equal(recommendTh08InputTiming(2, 5, 0).inputDelay, 0);
console.log("TH08 input timing recommendation: PASS");
