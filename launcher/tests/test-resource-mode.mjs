import assert from "node:assert/strict";
import {
  RESOURCE_MODE_HOSTED,
  RESOURCE_MODE_EXTERNAL,
  RESOURCE_MODE_IMPORT,
  normalizeResourceMode,
} from "../lib/contracts/resource-mode.mjs";

assert.equal(normalizeResourceMode(), RESOURCE_MODE_HOSTED);
assert.equal(normalizeResourceMode("hosted"), RESOURCE_MODE_HOSTED);
assert.equal(normalizeResourceMode("external"), RESOURCE_MODE_EXTERNAL);
assert.equal(normalizeResourceMode("import"), RESOURCE_MODE_IMPORT);
assert.equal(normalizeResourceMode("import-only"), null);
assert.equal(normalizeResourceMode("import-partial"), null);
assert.equal(normalizeResourceMode("unknown"), null);

console.log(JSON.stringify({ canonical: [RESOURCE_MODE_HOSTED, RESOURCE_MODE_EXTERNAL, RESOURCE_MODE_IMPORT], legacyReadAliases: 0 }));
