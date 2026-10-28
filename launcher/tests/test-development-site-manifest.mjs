import assert from "node:assert/strict";
import { mkdtemp, mkdir, rm, writeFile } from "node:fs/promises";
import { join, relative, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { PRODUCT_CONTENT } from "../lib/content-definition.mjs";
import { createDevelopmentSiteManifestFromContent } from "../lib/development-site-manifest.mjs";
import { DEVELOPMENT_CONTENT } from "../lib/development-content.mjs";
import { SITE_MANIFEST_SCHEMA, validateSiteManifest } from "../lib/contracts/site-manifest.mjs";
import { PRODUCT_GAMES } from "../lib/contracts/product-catalog.mjs";
import { DEFAULT_BUILD_GAMES } from "../lib/product-selection.mjs";

const root = await mkdtemp(join(resolve(import.meta.dirname, "../.."), "eagler-development-manifest-"));
const project = fileURLToPath(new URL("..", import.meta.url));
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
const createManifest = options => createDevelopmentSiteManifestFromContent(fixtureContent, { sourceRoot: root, ...options });
const manifest = await createManifest();
assert.equal(manifest.schema, SITE_MANIFEST_SCHEMA);
assert.equal(manifest.profile, "web-development");
assert.equal(manifest.shared.testBuild, true);
assert.equal(manifest.shared.resourceMode, "hosted");
assert.deepEqual(Object.keys(manifest.games), [...DEFAULT_BUILD_GAMES]);
const th13Only = await createManifest({ games: ["th13"] });
assert.deepEqual(Object.keys(th13Only.games), ["th13"]);
assert.doesNotThrow(() => validateSiteManifest(th13Only));
assert.equal(th13Only.games.th13.runtime, DEVELOPMENT_CONTENT.games.th13.runtime);
assert.doesNotThrow(() => validateSiteManifest(manifest));
assert.equal(manifest.shared.netplayRelay, undefined);
const th09Only = await createManifest({ games: ["th09"] });
assert.deepEqual(Object.keys(th09Only.games), ["th09"]);
assert.doesNotThrow(() => validateSiteManifest(th09Only));

const relayManifest = await createManifest({ netplayRelay: "ws://127.0.0.1:18142/" });
assert.equal(relayManifest.shared.netplayRelay, "ws://127.0.0.1:18142/");
await assert.rejects(
  createManifest({ netplayRelay: "https://example.invalid/" }),
  /netplayRelay/,
);
for (const [game, entry] of Object.entries(manifest.games)) {
  assert.match(entry.runtime, /^\.\.\//, `${game}: development Runtime must remain an explicit workspace-relative input`);
  assert.equal(entry.runtime, DEVELOPMENT_CONTENT.games[game].runtime,
    `${game}: development Site Manifest must preserve the declared Runtime owner`);
  if (PRODUCT_GAMES[game].multiplayerRuntime) {
    assert.equal(entry.multiplayerRuntime, DEVELOPMENT_CONTENT.games[game].multiplayerRuntime,
      `${game}: development Site Manifest must preserve the declared multiplayer Runtime owner`);
  }
  assert.match(entry.gameData.version, /^sha256-[a-f0-9]{64}$/i);
  assert.match(entry.gameData.layout, /^sha256-[a-f0-9]{64}$/i);
}
const multiplayerRuntimeOwners = {
  th06: "../ports/th06/build/web/multiplayer/th06.html",
  th07: "../ports/th07/build/web/multiplayer/th07.html",
  th075: "../ports/th075-mp/build-eagler-multiplayer/th075.html",
  th08: "../ports/th08-mp/build-eagler-multiplayer/th08.html",
  th09: "../ports/th09/web/build-eagler-multiplayer/th09.html",
  th10: "../ports/th10-mp/build-eagler-multiplayer/th10.html",
  th11: "../ports/th11-mp/build-eagler-multiplayer/th11.html",
  th12: "../ports/th12-mp/build-eagler-multiplayer/th12.html",
  th15: "../ports/th15-mp/build-eagler-multiplayer/th15.html",
};
const multiplayerGames = Object.keys(PRODUCT_GAMES).filter(game => PRODUCT_GAMES[game].multiplayerRuntime);
assert.deepEqual(multiplayerGames, Object.keys(multiplayerRuntimeOwners), "every catalogued multiplayer game has a development Runtime owner");
for (const [game, owner] of Object.entries(multiplayerRuntimeOwners)) {
  const actual = manifest.games[game].multiplayerRuntime;
  assert.ok(actual, `${game}: development Site Manifest must expose a multiplayer entrance`);
  assert.equal(resolve(project, actual.split("?")[0]), resolve(project, owner),
    `${game}: development multiplayer Runtime must point to its owning port build`);
}
for (const [game, owner] of Object.entries({
  th06: "../ports/th06/build/web/normal/th06.html",
  th07: "../ports/th07/build/web/normal/th07.html",
})) {
  assert.equal(resolve(project, manifest.games[game].runtime.split("?")[0]), resolve(project, owner),
    `${game}: normal development Runtime must use the root recipe default output`);
}
// Prepared content outside the Launcher tree must still produce HTTP URLs,
// rather than Windows drive schemes or Unix filesystem-root URLs.
for (const game of ["th06", "th07", "th09", "th10", "th11", "th15", "th20"]) {
  const contentRoot = join(root, game);
  const musicDirectory = ["th09", "th11", "th15"].includes(game) ? "music" : "bgm-ogg";
  await mkdir(join(contentRoot, musicDirectory), { recursive: true });
  await writeFile(join(contentRoot, `${game}.data`), fixtureData);
  if (preloadGames.includes(game)) {
    await writeFile(join(contentRoot, `${game}.js`), fixtureScript(game));
    const wavRoot = game === "th06" ? join(contentRoot, "bgm") : contentRoot;
    await mkdir(wavRoot, { recursive: true });
    for (const name of PRODUCT_CONTENT[game].music.wav.files) {
      await writeFile(join(wavRoot, name), fixtureData);
    }
  }
  for (const name of PRODUCT_CONTENT[game].music.ogg.files) {
    await writeFile(join(contentRoot, musicDirectory, name), fixtureData);
  }
  const sourcePath = path => relative(project, path).replaceAll("\\", "/");
  const declaration = DEVELOPMENT_CONTENT.games[game];
  const preparedGame = {
    runtime: declaration.runtime,
    ...(declaration.multiplayerRuntime ? { multiplayerRuntime: declaration.multiplayerRuntime } : {}),
    data: PRODUCT_GAMES[game].dataProvider === "emscripten-preload" ? {
      source: sourcePath(join(contentRoot, `${game}.data`)),
      runtimeScript: sourcePath(join(contentRoot, `${game}.js`)),
    } : {
      source: sourcePath(join(contentRoot, `${game}.data`)),
      layout: declaration.data.layout || declaration.data.identity?.layout,
    },
    music: Object.fromEntries(Object.entries(PRODUCT_CONTENT[game].music).map(([mode, music]) => {
      const directory = mode === "ogg" ? musicDirectory : game === "th06" ? "bgm" : "";
      const basePath = directory ? join(contentRoot, directory) : contentRoot;
      return [mode, { ...music, base: `${sourcePath(basePath)}/` }];
    })),
  };
  const prepared = await createDevelopmentSiteManifestFromContent({
    shared: DEVELOPMENT_CONTENT.shared,
    games: { [game]: preparedGame },
  }, { sourceRoot: project, games: [game] });
  const entry = prepared.games[game];
  assert.equal(resolve(project, entry.gameData.source), join(contentRoot, `${game}.data`));
  assert.match(entry.gameData.source, /^\.\.\//);
  assert.match(entry.music.ogg.base, /^\.\.\//);
  const url = new URL(entry.music.ogg.files[0], new URL(entry.music.ogg.base, "http://127.0.0.1:8130/"));
  assert.equal(url.origin, "http://127.0.0.1:8130");
  assert.equal(entry.gameData.bytes, fixtureData.length);
  assert.equal(entry.gameData.sha256, "9f64a747e1b97f131fabb6b447296c9b6f0201e79fb3c5356e6c77e89b6a806a");
  assert.equal(resolve(project, entry.music.ogg.base), join(contentRoot, musicDirectory));
}
await rm(root, { recursive: true, force: true });
console.log(JSON.stringify({ developmentHostManifest: "PASS", games: Object.keys(manifest.games) }));
