import { writeRuntimeGeneration, publishRuntimeManifest } from "../../lib/runtime-generations.mjs";
import { resolveBrowserPublicationSource } from "../../lib/launcher-build.mjs";
import { mkdir, readFile, writeFile, readdir, rm } from "node:fs/promises";
import { resolve, dirname } from "node:path";
import { FRONTEND_PACKAGE_FILES, APP_SHELL_FILES, resolveFrontendPackageSource } from "../../lib/frontend-manifest.mjs";
import { buildAppShell } from "../../lib/app-shell-build.mjs";
import { writeRuntimeFixture } from "./runtime-recovery-fixture.mjs";

const [directory, version = "a"] = process.argv.slice(2);
if (!directory || !/^[a-z0-9-]+$/.test(version)) throw new Error("usage: build-pwa-fixture.mjs OUTPUT [VERSION]");
const root = resolve(directory);
await mkdir(root, { recursive: true });
for (const path of FRONTEND_PACKAGE_FILES) {
  try {
    const bytes = await readFile(resolveFrontendPackageSource(path));
    await mkdir(dirname(resolve(root, path)), { recursive: true });
    await writeFile(resolve(root, path), bytes);
  } catch (error) {
    if (error.code !== "ENOENT" || APP_SHELL_FILES.includes(path)) throw error;
  }
}
// Change shell bytes too, so corrupt-shell installation tests cannot reuse an
// unchanged index from the preceding generation instead of reading the fault.
const index = resolve(root, "index.html");
await writeFile(index, (await readFile(index, "utf8")).replace("</head>",
  `<meta name="pwa-fixture-shell" content="${version}"></head>`));
// Exercise the same selector source used by the real Launcher, not a test-only
// cache implementation. Its fixture ESM publication is kept outside Runtime.
for (const [source, target] of [
  ["assets/launcher/runtime-launch.mjs", "fixture/launcher/runtime-launch.mjs"],
  ["assets/contracts/runtime-generations.mjs", "fixture/contracts/runtime-generations.mjs"],
]) {
  await mkdir(dirname(resolve(root, target)), { recursive: true });
  await writeFile(resolve(root, target), await readFile(resolveBrowserPublicationSource(source)));
}
const groups = [];
for (const game of ["pwa-test", "pwa-unused"]) {
  const source = resolve(root, ".tmp", game);
  await rm(source, { recursive: true, force: true });
  await writeRuntimeFixture(source, version);
  const current = await writeRuntimeGeneration({ site: root, root: `runtime/${game}/`, source,
    entry: "runtime.html", names: await readdir(source) });
  groups.push({ root: `runtime/${game}/`, current });
  await rm(source, { recursive: true });
}
await publishRuntimeManifest(root, groups);
const result = await buildAppShell({ quiet: true, globDirectory: root,
  swDest: resolve(root, "app-shell-sw.js"), additionalGlobPatterns: ["runtime/pwa-test/**/*", "runtime/pwa-unused/**/*", "fixture/**/*.mjs"],
  deferredPathPrefixes: ["runtime/pwa-test/", "runtime/pwa-unused/"],
});
if (result.warnings.length) throw new Error(result.warnings.join("\n"));
if (result.manifestEntries.some(entry => !/^[a-f0-9]{64}$/.test(entry.revision || ""))) throw new Error("non-SHA-256 precache entry");
console.log(JSON.stringify({ build: result.buildId, entries: result.count }));
