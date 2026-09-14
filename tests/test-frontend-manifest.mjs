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
assert.ok(BROWSER_MODULE_FILES.includes("assets/launcher/app.mjs"));
assert.equal(BROWSER_MODULE_FILES.length, 2,
  "the published browser graph must contain the facade and optimized bundle instead of the source module graph");
assert.ok(BROWSER_MODULE_FILES.every(path => APP_SHELL_FILES.includes(path)));
assert.equal(new Set(BROWSER_MODULE_FILES).size, BROWSER_MODULE_FILES.length);
assert.ok(!FRONTEND_PACKAGE_FILES.includes("app-shell-sw.js"));
assert.ok(FRONTEND_PACKAGE_FILES.includes("en.html"));
assert.ok(FRONTEND_PACKAGE_FILES.includes("sitemap.xml"));
assert.ok(FRONTEND_PACKAGE_FILES.every(path => !/title00\.(?:jpg|png)$/i.test(path)));
assert.ok(FRONTEND_PACKAGE_FILES.every(path => !/(?:^|\/)th0[678]-card\.webp$/i.test(path)));
assert.deepEqual(hostArtworkFiles(["th06", "th07", "th08"]), [
  "th06-card.webp", "th06.ico", "th07-card.webp", "th08-card.webp",
]);
assert.deepEqual(hostArtworkFiles(["th07"]), ["th07-card.webp", "th06.ico"],
  "the site favicon is required even when TH06 is not a selected product");
assert.match(
  relative(project, resolveFrontendPackageSource("assets/launcher/app.mjs")).replaceAll("\\", "/"),
  /^\.cache\/build\/optimized\/assets\/launcher\/app\.mjs$/,
  "source checkout must publish the optimized Launcher outside the authored assets directory",
);
for (const directory of ["assets/contracts", "assets/launcher"]) {
  await assert.rejects(access(resolve(project, directory)), error => error?.code === "ENOENT",
    `${directory} must not exist as generated source-checkout output`);
}
const publicFiles = await collectFiles(publicRoot);
const declaredPublicFiles = FRONTEND_PACKAGE_FILES.filter(path =>
  resolveFrontendPackageSource(path).startsWith(`${publicRoot}\\`) ||
  resolveFrontendPackageSource(path).startsWith(`${publicRoot}/`)
).concat(["index.html", "styles.css", "touch-guide.css"]).sort();
assert.deepEqual(publicFiles, declaredPublicFiles,
  "public/ must contain exactly the allowlisted authored browser source files");
console.log(JSON.stringify({ frontendManifest: "PASS", packaged: FRONTEND_PACKAGE_FILES.length, appShell: APP_SHELL_FILES.length }));

assert.deepEqual(hostArtworkFiles(["th10"]), ["th10-card.webp", "th06.ico"], "TH10 title artwork is a host-owned resource");
assert.deepEqual(hostArtworkFiles(["th06", "th07", "th08", "th10"]), ["th06-card.webp", "th06.ico", "th07-card.webp", "th08-card.webp", "th10-card.webp"]);
assert.throws(()=>hostArtworkFiles(["unknown"]), /unknown artwork product/);
