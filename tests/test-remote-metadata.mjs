import assert from "node:assert/strict";
import { ensureLauncherBuild } from "../lib/launcher-build.mjs";
import { HOST_MANIFEST_SCHEMA } from "../lib/contracts/host-manifest.mjs";
import { RELEASE_CATALOG_SCHEMA } from "../lib/contracts/release-catalog.mjs";

await ensureLauncherBuild();
const { loadRemoteMetadata } = await import("../.cache/build/browser/assets/launcher/remote-metadata.mjs");

const hostManifest = {
  schema: HOST_MANIFEST_SCHEMA,
  protocol: "eagler-touhou/1",
  profile: "web-validation-test",
  shared: { resourceMode: "hosted", vanillaFont: "../shared/msgothic.ttc", unicodeFont: "../shared/unifont.otf" },
  games: {
    th08: {
      runtime: "runtime/th08/th08.html",
      gameData: {
        version: `sha256-${"a".repeat(64)}`,
        layout: `sha256-${"b".repeat(64)}`,
        path: "th08.data",
        bytes: 1,
        sha256: "a".repeat(64),
      },
      music: { midi: { files: [] } },
    },
  },
};
const releaseCatalog = { schema: RELEASE_CATALOG_SCHEMA, games: {} };

const both = await loadRemoteMetadata(async (_file, kind) => kind === "host-manifest" ? hostManifest : releaseCatalog);
assert.equal(both.hostManifest.ok, true);
assert.equal(both.releaseCatalog.ok, true);

const hostOnly = await loadRemoteMetadata(async (_file, kind) => {
  if (kind === "release-catalog") throw new Error("catalog offline");
  return hostManifest;
});
assert.equal(hostOnly.hostManifest.ok, true);
assert.equal(hostOnly.releaseCatalog.ok, false);

const catalogOnly = await loadRemoteMetadata(async (_file, kind) => {
  if (kind === "host-manifest") throw new Error("manifest offline");
  return releaseCatalog;
});
assert.equal(catalogOnly.hostManifest.ok, false);
assert.equal(catalogOnly.releaseCatalog.ok, true);

console.log(JSON.stringify({ remoteMetadata: "PASS", independentFailures: true }));
