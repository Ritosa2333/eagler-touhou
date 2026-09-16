import assert from "node:assert/strict";
import { mkdtemp, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import { createDevelopmentHostManifestFromContent } from "../lib/development-host-manifest.mjs";
import { DEVELOPMENT_CONTENT } from "../lib/development-content.mjs";
import { HOST_MANIFEST_SCHEMA, validateHostManifest } from "../lib/contracts/host-manifest.mjs";
import { PRODUCT_GAMES } from "../lib/contracts/product-catalog.mjs";

const root = await mkdtemp(join(tmpdir(), "eagler-development-manifest-"));
const fixtureData = Buffer.from([1, 2, 3, 4]);
const fixtureScript = game => `loadPackage({files:[{filename:"/${game}-fixture.dat",start:0,end:4}],remote_package_size:4});`;
const preloadGames = Object.entries(PRODUCT_GAMES)
  .filter(([, product]) => product.dataProvider === "emscripten-preload")
  .map(([game]) => game);
for (const game of preloadGames) {
  await writeFile(join(root, `${game}.data`), fixtureData);
  await writeFile(join(root, `${game}.js`), fixtureScript(game));
}
const fixtureContent = {
  shared: DEVELOPMENT_CONTENT.shared,
  games: Object.fromEntries(Object.entries(DEVELOPMENT_CONTENT.games).map(([game, declaration]) => [game, {
    runtime: declaration.runtime,
    ...(declaration.multiplayerRuntime ? { multiplayerRuntime: declaration.multiplayerRuntime } : {}),
    data: PRODUCT_GAMES[game].dataProvider === "retail-memory" ? { identity: { bytes:4, sha256:"a".repeat(64), layout:declaration.data.identity?.layout || declaration.data.layout } } : {
      source: `${game}.data`,
      runtimeScript: `${game}.js`,
    },
  }])),
};
const createManifest = options => createDevelopmentHostManifestFromContent(fixtureContent, { sourceRoot: root, ...options });
const manifest = await createManifest();
assert.equal(manifest.schema, HOST_MANIFEST_SCHEMA);
assert.equal(manifest.profile, "web-development");
assert.equal(manifest.shared.testBuild, true);
assert.equal(manifest.shared.resourceMode, "hosted");
assert.deepEqual(Object.keys(manifest.games), Object.keys(PRODUCT_GAMES));
assert.doesNotThrow(() => validateHostManifest(manifest));
assert.equal(manifest.shared.netplayRelay, undefined);

const relayManifest = await createManifest({ netplayRelay: "ws://127.0.0.1:18142/" });
assert.equal(relayManifest.shared.netplayRelay, "ws://127.0.0.1:18142/");
await assert.rejects(
  createManifest({ netplayRelay: "https://example.invalid/" }),
  /netplayRelay/,
);
for (const [game, entry] of Object.entries(manifest.games)) {
  assert.match(entry.runtime, /^\.\.\//, `${game}: development Runtime must remain an explicit workspace-relative input`);
  assert.equal(entry.runtime, DEVELOPMENT_CONTENT.games[game].runtime,
    `${game}: development Host Manifest must preserve the declared Runtime owner`);
  if (PRODUCT_GAMES[game].multiplayerRuntime) {
    assert.equal(entry.multiplayerRuntime, DEVELOPMENT_CONTENT.games[game].multiplayerRuntime,
      `${game}: development Host Manifest must preserve the declared multiplayer Runtime owner`);
  }
  assert.match(entry.gameData.version, /^sha256-[a-f0-9]{64}$/i);
  assert.match(entry.gameData.layout, /^sha256-[a-f0-9]{64}$/i);
}
await rm(root, { recursive: true, force: true });
console.log(JSON.stringify({ developmentHostManifest: "PASS", games: Object.keys(manifest.games) }));
