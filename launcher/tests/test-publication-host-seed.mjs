/** L0 release/dev isolation contract.
 * Preconditions: static product declarations only. Mutation: none.
 * Proves: publication Host seeds contain no workstation paths or generated identity.
 * Does NOT prove: package construction, Runtime build, or deployment. */
import assert from "node:assert/strict";
import { createPublicationHostSeed } from "../lib/publication-host-seed.mjs";
import { PRODUCT_GAMES } from "../lib/contracts/product-catalog.mjs";

const seed = createPublicationHostSeed("web-release-test");
assert.equal(seed.profile, "web-release-test");
assert.deepEqual(Object.keys(seed.games), Object.keys(PRODUCT_GAMES));
const serialized = JSON.stringify(seed);
for (const forbidden of ["../", "build-web", "artifacts/", "archive/", "dist/"]) {
  assert.ok(!serialized.includes(forbidden), `publication seed contains local path: ${forbidden}`);
}
for (const entry of Object.values(seed.games)) {
  assert.equal(entry.runtime, undefined);
  assert.equal(entry.gameData.bytes, undefined);
  assert.equal(entry.gameData.sha256, undefined);
  for (const pack of Object.values(entry.music)) assert.equal(pack.base, undefined);
}
assert.throws(() => createPublicationHostSeed("web-development"), /non-development profile/);
console.log("Publication Host seed isolation: PASS");
