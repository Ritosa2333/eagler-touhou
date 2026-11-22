// Bundle the browser host into the directory runtime eagler-touhou accepts.
// esbuild minifies every hand-written module and the Emscripten loader into
// one shell.mjs. The game and runtime stay in THSSS.Browser.wasm.
//
//   dotnet publish THSSS.Browser/THSSS.Browser.csproj -c Release
//   npm install
//   npm run stage:eagler

import * as esbuild from "esbuild";
import { createHash } from "node:crypto";
import { existsSync, lstatSync, symlinkSync } from "node:fs";
import { mkdtemp, mkdir, readFile, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { dirname, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const browser = dirname(fileURLToPath(import.meta.url));
const repo = resolve(browser, "..");
const publish = resolve(browser, "bin/Release/net10.0/browser-wasm/publish");
const wwwroot = resolve(browser, "wwwroot");
const output = resolve(repo, "build-eagler");

const allowed = name =>
  name === "thsss.html" || name === "manifest.json" ||
  /^[a-z0-9._-]+\.mjs$/i.test(name) ||
  /^[a-z0-9._-]+\.css$/i.test(name) ||
  /^[a-z0-9._-]+\.wasm$/i.test(name) ||
  /^[a-z0-9._-]+\.json$/i.test(name) ||
  /^fonts\/[a-z0-9._-]+\.(?:bin|woff2?)$/i.test(name);

const sha256 = bytes => createHash("sha256").update(bytes).digest("hex");

const files = {};
async function put(name, bytes) {
  if (!allowed(name)) throw new Error(`staged name is not a directory runtime file: ${name}`);
  if (Object.hasOwn(files, name)) throw new Error(`duplicate staged file: ${name}`);
  await mkdir(dirname(resolve(output, name)), { recursive: true });
  await writeFile(resolve(output, name), bytes);
  files[name] = { bytes: bytes.length, sha256: sha256(bytes) };
}

async function text(path) {
  return readFile(path, "utf8");
}

function stripMap(source) {
  return source.replaceAll(/^[ \t]*\/\/# sourceMappingURL=.*\r?\n?/gm, "");
}

function bundleSources(moduleJs) {
  const requireLine = 'nodeCrypto = require("node:crypto");';
  if (!moduleJs.includes(requireLine)) throw new Error("Emscripten loader no longer requires node:crypto");
  // The call sits in a Node-only branch. Leaving require() would become an
  // import of a module the browser runtime is not allowed to reference.
  moduleJs = moduleJs.replace(requireLine, "nodeCrypto = undefined;");

  let main = stripMap((sources.main).replaceAll("\r\n", "\n"));
  const shaderImport = 'import { putRuntimeFile } from "./runtime-files.mjs";';
  if (!main.includes(shaderImport)) throw new Error("main.js no longer imports the runtime file helper");
  main = main.replace(shaderImport, `import { shaderSource } from "./shaders.mjs";\n${shaderImport}`);
  const shaderLoader = `async function loadShader(path) {
  const response = await fetch(new URL(path, document.baseURI));
  if (!response.ok) {
    throw new Error("Missing shader " + path);
  }
  return response.text();
}`;
  const embeddedLoader = `async function loadShader(path) {
  const name = path.slice(path.lastIndexOf("/") + 1);
  const text = shaderSource[name];
  if (typeof text !== "string") throw new Error("Missing shader " + path);
  return text;
}`;
  if (!main.includes(shaderLoader)) throw new Error("main.js shader loader changed; update the stager");
  main = main.replace(shaderLoader, embeddedLoader);
  return { moduleJs, main };
}

const sources = {};
await rm(output, { recursive: true, force: true });
await mkdir(output, { recursive: true });

const wasm = await readFile(resolve(publish, "THSSS.Browser.wasm"));
if (wasm.length < 4 || wasm[0] !== 0 || wasm[1] !== 0x61 || wasm[2] !== 0x73 || wasm[3] !== 0x6d) {
  throw new Error("published THSSS.Browser.wasm is not a wasm module");
}
await put("THSSS.Browser.wasm", wasm);

const moduleJs = stripMap(await text(resolve(publish, "THSSS.Browser.js")));
if (!moduleJs.includes("export default createThsssModule")) {
  throw new Error("published THSSS.Browser.js is not the ES module loader");
}
sources.main = await text(resolve(wwwroot, "main.js"));
const rewritten = bundleSources(moduleJs);

const shaderNames = ["Filter.vert", "Filter.frag", "Blur.vert", "Blur.frag", "Warp.vert", "Warp.frag"];
const shaderSource = {};
for (const name of shaderNames) shaderSource[name] = await text(resolve(wwwroot, "shaders", name));

const temp = await mkdtemp(resolve(tmpdir(), "thsss-shell-"));
try {
  const graph = {
    "shell.mjs": await text(resolve(wwwroot, "shell.mjs")),
    "host/input/key-owners.mjs": await text(resolve(wwwroot, "host/input/key-owners.mjs")),
    "host/input/touch-movement.mjs": await text(resolve(wwwroot, "host/input/touch-movement.mjs")),
    "host/input/menu-gestures.mjs": await text(resolve(wwwroot, "host/input/menu-gestures.mjs")),
    "host/input/touch-bridge.mjs": await text(resolve(wwwroot, "host/input/touch-bridge.mjs")),
    "host/storage/files.mjs": await text(resolve(wwwroot, "host/storage/files.mjs")),
    "directory-keyboard.mjs": await text(resolve(wwwroot, "directory-keyboard.mjs")),
    "eagler-host.mjs": await text(resolve(wwwroot, "eagler-host.mjs")),
    "runtime-files.mjs": await text(resolve(wwwroot, "runtime-files.mjs")),
    "gl.js": stripMap(await text(resolve(wwwroot, "gl.js"))),
    "audio.js": stripMap(await text(resolve(wwwroot, "audio.js"))),
    "main.js": rewritten.main,
    "shaders.mjs": `export const shaderSource = ${JSON.stringify(shaderSource)};\n`,
    "THSSS.Browser.js": rewritten.moduleJs,
  };
  for (const [name, contents] of Object.entries(graph)) {
    const destination = resolve(temp, name);
    await mkdir(dirname(destination), { recursive: true });
    await writeFile(destination, contents);
  }
  const bundled = resolve(temp, "bundled", "shell.mjs");
  await esbuild.build({
    absWorkingDir: temp,
    entryPoints: ["shell.mjs"],
    bundle: true,
    format: "esm",
    platform: "browser",
    target: "es2022",
    minify: true,
    legalComments: "none",
    sourcemap: false,
    outfile: bundled,
    plugins: [{
      name: "keep-wasm-url",
      setup(build) {
        build.onResolve({ filter: /\.wasm$/ }, () => ({
          path: "THSSS.Browser.wasm",
          external: true,
        }));
      },
    }],
  });
  const shell = await text(bundled);
  if (/\bfrom\s*["']\./.test(shell)) throw new Error("bundled shell still imports a local module");
  if (!shell.includes("THSSS.Browser.wasm")) throw new Error("bundled shell lost the wasm URL");
  if (!shell.includes("runtimeEpoch")) throw new Error("bundled shell lost the navigation epoch");
  if (!shell.includes("__eaglerPrepareManagedRuntimeDataV1")) throw new Error("bundled shell lost the managed data bridge");
  if (!shell.includes("ShiftKey")) throw new Error("bundled shell lost the keyboard owner");
  await put("shell.mjs", Buffer.from(shell, "utf8"));
} finally {
  await rm(temp, { recursive: true, force: true });
}

for (const name of ["thsss.html", "manifest.json", "directory-keyboard.mjs"]) {
  await put(name, Buffer.from(await text(resolve(wwwroot, name)), "utf8"));
}

const manifest = JSON.parse(await text(resolve(output, "manifest.json")));
if (manifest.game !== "thsss" || manifest.protocol !== "eagler-touhou/1" || manifest.entry !== "thsss.html") {
  throw new Error("wwwroot/manifest.json is not the thsss directory manifest");
}
for (const feature of ["thprac", "languages", "focusHitbox"]) {
  if (manifest.features?.[feature] !== false) throw new Error(`manifest.features.${feature} must be false`);
}

const html = await text(resolve(output, "thsss.html"));
if (!html.includes('src="./shell.mjs"')) throw new Error("thsss.html must load the bundled shell");

const ordered = Object.fromEntries(Object.keys(files).sort().map(name => [name, files[name]]));
await writeFile(resolve(output, "runtime-files.json"), JSON.stringify({
  schema: "eagler-touhou/runtime-directory/1",
  files: ordered,
}, null, 2) + "\n");

const counts = {};
for (const name of Object.keys(ordered)) {
  const kind = name.startsWith("fonts/") ? "fonts" : name.slice(name.lastIndexOf(".") + 1);
  counts[kind] = (counts[kind] || 0) + 1;
}

// Standalone thsss.html fetches assets/thsss.dat beside the runtime. Keep that
// path available for local opens without putting game data into runtime-files.
const assetRoot = resolve(wwwroot, "assets");
const stagedAssets = resolve(output, "assets");
if (existsSync(resolve(assetRoot, "thsss.dat"))) {
  await mkdir(stagedAssets, { recursive: true });
  for (const name of ["thsss.dat", "BGM"]) {
    const target = resolve(assetRoot, name);
    const link = resolve(stagedAssets, name);
    if (!existsSync(target)) continue;
    if (existsSync(link) || lstatSync(link, { throwIfNoEntry: false })?.isSymbolicLink()) {
      await rm(link, { recursive: true, force: true });
    }
    symlinkSync(target, link, existsSync(target) && lstatSync(target).isDirectory() ? "dir" : "file");
  }
}

console.log(`staged ${Object.keys(ordered).length} files in ${output}`);
console.log(counts);
console.log(`shell.mjs ${files["shell.mjs"].bytes} bytes`);
