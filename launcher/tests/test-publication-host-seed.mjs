import {test} from 'vitest';
import assert from "node:assert/strict";
import { createPublicationHostSeed } from "../lib/publication-host-seed.mjs";
import { PRODUCT_CONTENT } from "../lib/content-definition.mjs";

test("publication host seed", async () => {
/** L0 release/dev isolation contract.
 * Preconditions: static product declarations only. Mutation: none.
 * Proves: publication Host seeds contain no workstation paths or generated identity.
 * Does NOT prove: package construction, Runtime build, or deployment. */




const seed = createPublicationHostSeed("web-release-test");
assert.equal(seed.profile, "web-release-test");
assert.deepEqual(Object.keys(seed.games), Object.keys(PRODUCT_CONTENT));
const selected = createPublicationHostSeed("web-release-test", ["th14"]);
assert.deepEqual(Object.keys(selected.games), ["th14"]);
assert.equal(selected.games.th14.gameData.path, "th14.dat");
assert.throws(() => createPublicationHostSeed("web-release-test", ["hsob"]), /content declaration is missing/);
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
});
