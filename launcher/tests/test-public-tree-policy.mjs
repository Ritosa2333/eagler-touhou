import assert from "node:assert/strict";
import { access } from "node:fs/promises";
import { spawnSync } from "node:child_process";
import { publicTreePolicyViolation } from "../lib/public-tree-policy.mjs";

assert.match(publicTreePolicyViolation("screenshots/mobile.png"), /not public source/);
assert.match(publicTreePolicyViolation("debug.pid"), /ephemeral/);
assert.match(publicTreePolicyViolation("host/config/site-features.customer.json"), /site-specific/);
assert.equal(publicTreePolicyViolation("host/config/site-features.default.json"), "");
assert.match(
  publicTreePolicyViolation("host/config/site-features.import.example.json"),
  /site-specific self-host configuration/,
);
assert.equal(publicTreePolicyViolation("tests/fixtures/reference.png"), "");

const listed = spawnSync("git", ["ls-files", "--cached", "--others", "--exclude-standard", "-z"], {
  cwd: new URL("..", import.meta.url),
  encoding: "utf8",
  windowsHide: true,
});
if (listed.status !== 0) throw new Error(listed.stderr || "git ls-files failed");

const violations = [];
for (const path of listed.stdout.split("\0").filter(Boolean)) {
  try { await access(new URL(`../${path.replaceAll("\\", "/")}`, import.meta.url)); }
  catch { continue; } // A tracked deletion is absent from the candidate tree.
  const reason = publicTreePolicyViolation(path);
  if (reason) violations.push(`${path}: ${reason}`);
}
assert.deepEqual(violations, [], `public-tree policy violations:\n${violations.join("\n")}`);
console.log(JSON.stringify({ publicTreePolicy: "PASS", candidates: listed.stdout.split("\0").filter(Boolean).length }));
