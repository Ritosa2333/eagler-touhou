/** L0 artifact/source ownership contract. Mutation: none. Proves packaged
 * frontend/App Shell files have one owner and original-game-derived host art
 * is not part of repository-owned frontend assets. Does NOT prove rendering. */
import assert from "node:assert/strict";
import { access, readdir } from "node:fs/promises";
import { relative, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import {
  APP_SHELL_FILES,
  BROWSER_MODULE_ENTRYPOINTS,
  BROWSER_MODULE_FILES,
  FRONTEND_PACKAGE_FILES,
  hostArtworkFiles,
  resolveFrontendPackageSource,
} from "../lib/frontend-manifest.mjs";

const project = resolve(fileURLToPath(new URL("..", import.meta.url)));
const publicRoot = resolve(project, "public");

async function collectFiles(directory, prefix = "") {
  const files = [];
  for (const entry of await readdir(directory, { withFileTypes: true })) {
    const path = prefix ? `${prefix}/${entry.name}` : entry.name;
    if (entry.isDirectory()) files.push(...await collectFiles(resolve(directory, entry.name), path));
    else if (entry.isFile()) files.push(path);
  }
  return files.sort();
}

assert.ok(FRONTEND_PACKAGE_FILES.includes("index.html"));
assert.ok(FRONTEND_PACKAGE_FILES.includes("robots.txt"));
assert.ok(FRONTEND_PACKAGE_FILES.includes("vendor/fflate.LICENSE"));
assert.ok(APP_SHELL_FILES.includes("vendor/fflate.min.js"));
assert.ok(!APP_SHELL_FILES.includes("vendor/fflate.LICENSE"));
assert.ok(APP_SHELL_FILES.every(path => FRONTEND_PACKAGE_FILES.includes(path)));
assert.deepEqual(BROWSER_MODULE_ENTRYPOINTS, ["app.js"]);
assert.ok(BROWSER_MODULE_FILES.includes("app.js"));
assert.ok(BROWSER_MODULE_FILES.includes("package/package-store.mjs"));
for (const modulePath of [
  "assets/contracts/host-manifest.mjs",
  "assets/contracts/product-catalog.mjs",
  "assets/contracts/release-catalog.mjs",
  "assets/contracts/resource-mode.mjs",
  "assets/launcher/app-shell-client.mjs",
  "assets/launcher/changelog.mjs",
  "assets/launcher/game-zoom.mjs",
  "assets/launcher/game-preferences.mjs",
  "assets/launcher/language-catalog.mjs",
  "assets/launcher/language-pack-validation.mjs",
  "assets/launcher/music-availability.mjs",
  "assets/launcher/multiplayer-identity.mjs",
  "assets/launcher/multiplayer-lobby-snapshot.mjs",
  "assets/launcher/multiplayer-preferences.mjs",
  "assets/launcher/multiplayer-relay-url.mjs",
  "assets/launcher/multiplayer-runtime-options.mjs",
  "assets/launcher/multiplayer-room-session.mjs",
  "assets/launcher/multiplayer-spectator-rail-position.mjs",
  "assets/launcher/network-activity.mjs",
  "assets/launcher/replay-files.mjs",
  "assets/launcher/remote-metadata.mjs",
  "assets/launcher/runtime-diagnostics-model.mjs",
  "assets/launcher/runtime-preparation.mjs",
  "assets/launcher/sha256.mjs",
  "assets/launcher/route-state.mjs",
  "assets/launcher/site-notice.mjs",
  "assets/launcher/touch-layout-editor-state.mjs",
  "assets/launcher/touch-layout-model.mjs",
  "assets/launcher/touch-runtime-protocol.mjs",
]) {
  assert.ok(BROWSER_MODULE_FILES.includes(modulePath), `${modulePath} must stay in the browser/offline module closure`);
}
assert.ok(BROWSER_MODULE_FILES.every(path => APP_SHELL_FILES.includes(path)));
assert.equal(new Set(BROWSER_MODULE_FILES).size, BROWSER_MODULE_FILES.length);
assert.ok(!FRONTEND_PACKAGE_FILES.includes("app-shell-sw.js"));
assert.ok(FRONTEND_PACKAGE_FILES.every(path => !/title00\.(?:jpg|png)$/i.test(path)));
assert.ok(FRONTEND_PACKAGE_FILES.every(path => !/(?:^|\/)th0[678]-card\.webp$/i.test(path)));
assert.deepEqual(hostArtworkFiles(["th06", "th07", "th08"]), [
  "th06-card.webp", "th06.ico", "th07-card.webp", "th08-card.webp",
]);
assert.deepEqual(hostArtworkFiles(["th07"]), ["th07-card.webp", "th06.ico"],
  "the site favicon is required even when TH06 is not a selected product");
assert.match(
  relative(project, resolveFrontendPackageSource("assets/launcher/app.mjs")).replaceAll("\\", "/"),
  /^\.cache\/build\/browser\/assets\/launcher\/app\.mjs$/,
  "source checkout must keep generated browser modules out of the authored assets directory",
);
for (const directory of ["assets/contracts", "assets/launcher"]) {
  await assert.rejects(access(resolve(project, directory)), error => error?.code === "ENOENT",
    `${directory} must not exist as generated source-checkout output`);
}
const publicFiles = await collectFiles(publicRoot);
const declaredPublicFiles = FRONTEND_PACKAGE_FILES.filter(path =>
  resolveFrontendPackageSource(path).startsWith(`${publicRoot}\\`) ||
  resolveFrontendPackageSource(path).startsWith(`${publicRoot}/`)
).sort();
assert.deepEqual(publicFiles, declaredPublicFiles,
  "public/ must contain exactly the allowlisted authored browser source files");
console.log(JSON.stringify({ frontendManifest: "PASS", packaged: FRONTEND_PACKAGE_FILES.length, appShell: APP_SHELL_FILES.length }));

assert.deepEqual(hostArtworkFiles(["th10"]), ["th10-card.webp", "th06.ico"], "TH10 title artwork is a host-owned resource");
assert.deepEqual(hostArtworkFiles(["th06", "th07", "th08", "th10"]), ["th06-card.webp", "th06.ico", "th07-card.webp", "th08-card.webp", "th10-card.webp"]);
assert.throws(()=>hostArtworkFiles(["unknown"]), /unknown artwork product/);
