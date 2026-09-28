import assert from "node:assert/strict";
import { recommendMultiplayerInputTiming } from "../.cache/build/browser/assets/launcher/multiplayer-input-timing.mjs";

assert.deepEqual(recommendMultiplayerInputTiming(0, 100, 0, 8),
  { inputDelay: 0, targetRollbackFrames: 8, networkFrames: 0, mobileSeats: 0 });
assert.deepEqual(recommendMultiplayerInputTiming(1, 100, 0, 8),
  { inputDelay: 2, targetRollbackFrames: 4, networkFrames: 6, mobileSeats: 1 });
assert.deepEqual(recommendMultiplayerInputTiming(2, 100, 0, 8),
  { inputDelay: 4, targetRollbackFrames: 2, networkFrames: 6, mobileSeats: 2 });
assert.deepEqual(recommendMultiplayerInputTiming(0, 100, 0, 12),
  { inputDelay: 0, targetRollbackFrames: 12, networkFrames: 0, mobileSeats: 0 });
assert.equal(recommendMultiplayerInputTiming(2, null, null, 8).inputDelay, 6);
assert.equal(recommendMultiplayerInputTiming(2, 5, 0, 12).inputDelay, 0);
console.log("Multiplayer input timing recommendation: PASS");
