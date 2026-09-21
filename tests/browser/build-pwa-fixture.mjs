import { mkdir, readFile, writeFile } from "node:fs/promises";
import { resolve, dirname } from "node:path";
import { FRONTEND_PACKAGE_FILES, APP_SHELL_FILES, resolveFrontendPackageSource } from "../../lib/frontend-manifest.mjs";
import { buildAppShell } from "../../lib/app-shell-build.mjs";

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
// Deliberately NOT a Touhou Runtime. This tests the real shell around an
// original-resource-free same-origin iframe -> ESM -> WASM dependency chain.
const runtime = resolve(root, "runtime/pwa-test");
await mkdir(runtime, { recursive: true });
await writeFile(resolve(runtime, "runtime.html"), '<!doctype html><canvas width="2" height="2"></canvas><script type="module" src="boot.mjs"></script>');
await writeFile(resolve(runtime, "empty.wasm"), new Uint8Array([0, 97, 115, 109, 1, 0, 0, 0]));
await writeFile(resolve(runtime, "boot.mjs"), `${version === "a" ? `const version = "${version}";` : 'import { version } from "./helper.mjs";'}
await WebAssembly.instantiate(await (await fetch("./empty.wasm")).arrayBuffer());
document.querySelector("canvas").getContext("2d").fillRect(0, 0, 2, 2);
window.fixtureVersion = version;
`);
if (version !== "a") await writeFile(resolve(runtime, "helper.mjs"), `export const version = "${version}";\n`);
const result = await buildAppShell({ quiet: true, globDirectory: root,
  swDest: resolve(root, "app-shell-sw.js"), additionalGlobPatterns: ["runtime/pwa-test/*"],
  deferredPathPrefixes: ["runtime/pwa-test/"],
});
if (result.warnings.length) throw new Error(result.warnings.join("\n"));
if (result.manifestEntries.some(entry => !/^[a-f0-9]{64}$/.test(entry.revision || ""))) throw new Error("non-SHA-256 precache entry");
console.log(JSON.stringify({ build: result.buildId, entries: result.count }));
