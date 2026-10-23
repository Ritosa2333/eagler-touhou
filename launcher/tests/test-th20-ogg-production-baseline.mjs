/** L3/module. Verifies the TH20 production OGG baseline declaration against
 * its cataloged music-track inventory. */
import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";
import { resolve } from "node:path";

import { TH20_MUSIC_FILES, TH20_MUSIC_LAYOUT } from "../lib/th20-content-layout.mjs";

const root = resolve(import.meta.dirname, "..");
const baseline = JSON.parse(await readFile(resolve(root, "host/ogg-baselines/th20.json"), "utf8"));
assert.equal(baseline.schema, "eagler-touhou/ogg-server-baseline/1");
assert.equal(baseline.game, "th20");
assert.equal(baseline.quality, 5);
assert.equal(TH20_MUSIC_LAYOUT.length, 19);
assert.deepEqual(Object.keys(baseline.files), [...TH20_MUSIC_FILES]);
for (const identity of Object.values(baseline.files)) {
  assert.ok(identity.bytes > 0);
  assert.match(identity.sha256, /^[a-f0-9]{64}$/);
  assert.match(identity.serial, /^0x[a-f0-9]{8}$/);
}

console.log(JSON.stringify({ th20OggProductionBaseline: "PASS", tracks: Object.keys(baseline.files).length }));
