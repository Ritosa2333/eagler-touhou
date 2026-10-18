import assert from "node:assert/strict";
import {
  BUILD_AUTHORITY_PRIVATE,
  BUILD_AUTHORITY_PUBLICATION,
  classifyBuildProfile,
} from "../lib/build-profile.mjs";

assert.equal(classifyBuildProfile("web-development"), BUILD_AUTHORITY_PRIVATE);
assert.equal(classifyBuildProfile("web-validation"), BUILD_AUTHORITY_PRIVATE);
assert.equal(classifyBuildProfile("web-validation-import"), BUILD_AUTHORITY_PRIVATE);
assert.equal(classifyBuildProfile("web-release"), BUILD_AUTHORITY_PUBLICATION);
assert.equal(classifyBuildProfile("web-release-hosted"), BUILD_AUTHORITY_PUBLICATION);
for (const invalid of ["", "test", "web-package", "release", "web-development-local"]) {
  assert.equal(classifyBuildProfile(invalid), null, `unexpected valid profile: ${invalid}`);
}

console.log(JSON.stringify({ authorities: [BUILD_AUTHORITY_PRIVATE, BUILD_AUTHORITY_PUBLICATION] }));
