import {test} from 'vitest';
import assert from "node:assert/strict";
import { createDevelopmentSiteManifest } from "../lib/development-site-manifest.mjs";
import { validateSiteManifest } from "../lib/contracts/site-manifest.mjs";
import { PRODUCT_GAMES } from "../lib/contracts/product-catalog.mjs";

test("test-development-site-manifest-workspace", async () => {
/** L4/workspace integration. Reads the declared sibling Runtime, DATA, and music
 * inputs. This is deliberately excluded from the standalone repository gate. */





const manifest = await createDevelopmentSiteManifest();
assert.deepEqual(Object.keys(manifest.games), Object.keys(PRODUCT_GAMES));
assert.doesNotThrow(() => validateSiteManifest(manifest));
for (const [game, entry] of Object.entries(manifest.games)) {
  assert.match(entry.gameData.version, /^sha256-[a-f0-9]{64}$/i, `${game}: DATA identity`);
  assert.match(entry.gameData.layout, /^sha256-[a-f0-9]{64}$/i, `${game}: DATA layout`);
}
console.log(JSON.stringify({ developmentHostManifestWorkspace: "PASS", games: Object.keys(manifest.games) }));
});
