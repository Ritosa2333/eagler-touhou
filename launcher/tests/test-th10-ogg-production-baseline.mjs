/** L3/module. Verifies the TH10 production OGG baseline declaration against
 * its cataloged music-track inventory. */
import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";
import { resolve } from "node:path";

import { TH10_MUSIC_NAMES } from "../lib/th10-content-layout.mjs";

const root = resolve(import.meta.dirname, "..");
const baseline = JSON.parse(await readFile(resolve(root, "host/ogg-baselines/th10.json"), "utf8"));
assert.equal(baseline.schema, "eagler-touhou/ogg-server-baseline/1");
assert.equal(baseline.game, "th10");
assert.equal(baseline.quality, 5);
assert.deepEqual(Object.keys(baseline.files), TH10_MUSIC_NAMES.map(name => `th10_${name}.ogg`));
for (const identity of Object.values(baseline.files)) {
  assert.ok(identity.bytes > 0);
  assert.match(identity.sha256, /^[a-f0-9]{64}$/);
  assert.match(identity.serial, /^0x[a-f0-9]{8}$/);
}

console.log(JSON.stringify({ th10OggProductionBaseline: "PASS", tracks: Object.keys(baseline.files).length }));
