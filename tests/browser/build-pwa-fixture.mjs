import { mkdir, readFile, writeFile } from "node:fs/promises";
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
await writeFile(index, `${await readFile(index, "utf8")}\n<!-- fixture shell ${version} -->\n`);
// Original-resource-free, but actual incompatible JS/Wasm generations.
await writeRuntimeFixture(resolve(root, "runtime/pwa-test"), version);
// This published, unselected Runtime must remain untouched during shell updates
// and while preparing or launching pwa-test, even if its server files are broken.
await writeRuntimeFixture(resolve(root, "runtime/pwa-unused"), version);
const result = await buildAppShell({ quiet: true, globDirectory: root,
  swDest: resolve(root, "app-shell-sw.js"), additionalGlobPatterns: ["runtime/pwa-test/*", "runtime/pwa-unused/*"],
  deferredPathPrefixes: ["runtime/pwa-test/", "runtime/pwa-unused/"],
});
if (result.warnings.length) throw new Error(result.warnings.join("\n"));
if (result.manifestEntries.some(entry => !/^[a-f0-9]{64}$/.test(entry.revision || ""))) throw new Error("non-SHA-256 precache entry");
console.log(JSON.stringify({ build: result.buildId, entries: result.count }));
