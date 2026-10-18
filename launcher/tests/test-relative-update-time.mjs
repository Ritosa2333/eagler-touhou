import assert from "node:assert/strict";
import {
  appliedAppShellUpdateAt,
  formatRelativeUpdateAge,
  nextRelativeUpdateRefresh,
} from "../.cache/build/browser/assets/launcher/relative-update-time.mjs";

assert.equal(appliedAppShellUpdateAt(null), null);
assert.equal(appliedAppShellUpdateAt({ updated: false, appliedAt: 123 }), null);
assert.equal(appliedAppShellUpdateAt({ updated: true, appliedAt: "invalid" }), null);
assert.equal(appliedAppShellUpdateAt({ updated: true, appliedAt: 123 }), 123);

assert.equal(formatRelativeUpdateAge(-1), "0s");
assert.equal(formatRelativeUpdateAge(59_999), "59s");
assert.equal(formatRelativeUpdateAge(60_000), "1min");
assert.equal(formatRelativeUpdateAge(59 * 60_000), "59min");
assert.equal(formatRelativeUpdateAge(60 * 60_000), "1h");
assert.equal(formatRelativeUpdateAge(23 * 60 * 60_000), "23h");
assert.equal(formatRelativeUpdateAge(24 * 60 * 60_000), "1d");
assert.equal(formatRelativeUpdateAge(9 * 24 * 60 * 60_000), "9d");

assert.equal(nextRelativeUpdateRefresh(1_500), 500);
assert.equal(nextRelativeUpdateRefresh(61_000), 59_000);
assert.equal(nextRelativeUpdateRefresh(61 * 60_000), 59 * 60_000);
assert.equal(nextRelativeUpdateRefresh(25 * 60 * 60_000), 23 * 60 * 60_000);

console.log("relative update time: PASS");
