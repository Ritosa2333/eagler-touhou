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
// Original-resource-free, but actual incompatible JS/Wasm generations.
await writeRuntimeFixture(resolve(root, "runtime/pwa-test"), version);
const result = await buildAppShell({ quiet: true, globDirectory: root,
  swDest: resolve(root, "app-shell-sw.js"), additionalGlobPatterns: ["runtime/pwa-test/*"],
  deferredPathPrefixes: ["runtime/pwa-test/"],
});
if (result.warnings.length) throw new Error(result.warnings.join("\n"));
if (result.manifestEntries.some(entry => !/^[a-f0-9]{64}$/.test(entry.revision || ""))) throw new Error("non-SHA-256 precache entry");
console.log(JSON.stringify({ build: result.buildId, entries: result.count }));
