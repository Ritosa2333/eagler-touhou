import assert from "node:assert/strict";
import {
  RELEASE_CATALOG_FILE,
  RELEASE_CATALOG_SCHEMA,
  releaseCatalogEntryUrl,
  validateReleaseCatalog,
} from "../lib/contracts/release-catalog.mjs";

const catalog = validateReleaseCatalog({
  schema: RELEASE_CATALOG_SCHEMA,
  games: {
    th06: { revision: "abc123", descriptor: "../th06.package.json" },
    th08: { revision: "next-8", descriptor: "../packages/th08.package.json" },
  },
});
assert.equal(releaseCatalogEntryUrl("https://touhou.vip/eagler-touhou/release-catalog.json", catalog, "th08"),
  "https://touhou.vip/packages/th08.package.json");
assert.equal(releaseCatalogEntryUrl("https://touhou.vip/eagler-touhou/release-catalog.json", catalog, "th07"), null);
assert.equal(RELEASE_CATALOG_FILE, "release-catalog.json");
assert.throws(() => validateReleaseCatalog({
  schema: RELEASE_CATALOG_SCHEMA,
  games: { th99: { revision: "x", descriptor: "../th99.package.json" } },
}), /entry: th99/);
assert.equal(releaseCatalogEntryUrl("https://touhou.vip/eagler-touhou/release-catalog.json", catalog, "th99"), null);
assert.throws(() => validateReleaseCatalog({ schema: RELEASE_CATALOG_SCHEMA, games: { th07: { revision: "x", descriptor: "https://other.invalid/x" } } }),
  /cross-origin/);
console.log("Release Catalog contract: PASS");
