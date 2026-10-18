import { existsSync, readdirSync } from "node:fs";
import { access, cp, mkdir, readFile, readdir, rm, stat } from "node:fs/promises";
import { spawn } from "node:child_process";
import { dirname, extname, relative, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { localModuleClosure } from "./browser-module-graph.mjs";
import { resolvePublicOrRepositorySource } from "./public-source.mjs";

const project = resolve(fileURLToPath(new URL("..", import.meta.url)));
const sourceRoot = resolve(project, "src");
const buildRoot = resolve(project, ".cache", "build", "browser");
const outputRoot = resolve(buildRoot, "assets");
const generatedOutputRoots = Object.freeze([
  resolve(outputRoot, "contracts"),
  resolve(outputRoot, "launcher"),
]);
const packagedOutputRoots = Object.freeze([
  resolve(project, "assets", "contracts"),
  resolve(project, "assets", "launcher"),
]);
const configPath = resolve(project, "tsconfig.launcher.json");
const tscPath = resolve(project, "node_modules", "typescript", "bin", "tsc");
const browserFacadePaths = new Set([
  "host-manifest.mjs",
  "product-catalog.mjs",
  "release-catalog.mjs",
  "resource-mode.mjs",
  "runtime-protocol.mjs",
]);

function containsTypeScriptSource(directory) {
  if (!existsSync(directory)) return false;
  return readdirSync(directory, { withFileTypes: true }).some(entry => {
    const path = resolve(directory, entry.name);
    return entry.isDirectory()
      ? containsTypeScriptSource(path)
      : entry.isFile() && extname(entry.name) === ".mts";
  });
}

// A self-host bundle intentionally contains src/app-shell-sw.js, so the src/ directory
// alone cannot distinguish it from a maintainer checkout. The TypeScript source
// set is the stable boundary: it exists in the checkout and is absent from a
// packaged self-host bundle.
const hasTypeScriptSource = containsTypeScriptSource(sourceRoot);

async function exists(path) {
  try { await access(path); return true; } catch { return false; }
}

async function collectDeclarationBridges(directory = project) {
  if (!await exists(directory)) return [];
  const files = [];
  for (const entry of await readdir(directory, { withFileTypes: true })) {
    const path = resolve(directory, entry.name);
    if (entry.isDirectory() && directory === project && ["package", "legacy"].includes(entry.name)) {
      files.push(...await collectDeclarationBridges(path));
    } else if (entry.isFile() && entry.name.endsWith(".d.mts")) {
      files.push(path);
    }
  }
  return files.sort();
}

async function collectGeneratedModules(directory) {
  if (!await exists(directory)) return [];
  const files = [];
  for (const entry of await readdir(directory, { withFileTypes: true })) {
    const path = resolve(directory, entry.name);
    if (entry.isDirectory()) files.push(...await collectGeneratedModules(path));
    else if (entry.isFile() && extname(entry.name) === ".mjs") files.push(path);
  }
  return files.sort();
}

async function filesMatch(source, target) {
  try {
    const [sourceInfo, targetInfo] = await Promise.all([stat(source), stat(target)]);
    if (!sourceInfo.isFile() || !targetInfo.isFile() || sourceInfo.size !== targetInfo.size) return false;
    const [sourceBytes, targetBytes] = await Promise.all([readFile(source), readFile(target)]);
    return sourceBytes.equals(targetBytes);
  } catch (error) {
    if (error?.code === "ENOENT") return false;
    throw error;
  }
}

async function collectSources(directory = sourceRoot) {
  const files = [];
  for (const entry of await readdir(directory, { withFileTypes: true })) {
    const path = resolve(directory, entry.name);
    if (entry.isDirectory()) files.push(...await collectSources(path));
    else if (entry.isFile() && extname(entry.name) === ".mts") files.push(path);
  }
  return files.sort();
}

function outputForSource(source) {
  const rel = relative(sourceRoot, source).replaceAll("\\", "/");
  return resolve(outputRoot, rel.replace(/\.mts$/i, ".mjs"));
}

function normalizePublicPath(value) {
  const path = String(value || "").replaceAll("\\", "/").replace(/^\.\//, "");
  if (!path || path.startsWith("/") || path.split("/").some(part => !part || part === "." || part === "..")) {
    throw new Error(`invalid browser publication path: ${value}`);
  }
  return path;
}

export function isLauncherBuildOutputPath(value) {
  const path = normalizePublicPath(value);
  return path.startsWith("assets/contracts/") || path.startsWith("assets/launcher/");
}

export function isMappedBrowserPublicationPath(value) {
  const path = normalizePublicPath(value);
  return isLauncherBuildOutputPath(path) || browserFacadePaths.has(path);
}

export function resolveBrowserPublicationSource(value) {
  const path = normalizePublicPath(value);
  if (isLauncherBuildOutputPath(path)) {
    return hasTypeScriptSource
      ? resolve(buildRoot, path)
      : resolve(project, path);
  }
  if (browserFacadePaths.has(path)) {
    return hasTypeScriptSource
      ? resolve(project, "src", "browser-facades", path)
      : resolve(project, path);
  }
  return resolvePublicOrRepositorySource(path);
}

async function runTypeScript() {
  if (!await exists(tscPath)) {
    throw new Error("Launcher TypeScript compiler is missing; run npm ci/npm install in the source checkout");
  }
  await rm(buildRoot, { recursive: true, force: true });
  await new Promise((resolvePromise, reject) => {
    const child = spawn(process.execPath, [tscPath, "-p", configPath, "--pretty", "false"], {
      cwd: project,
      stdio: "inherit",
      shell: false,
    });
    child.once("error", reject);
    child.once("exit", code => code === 0
      ? resolvePromise()
      : reject(new Error(`Launcher TypeScript build failed with exit code ${code ?? "unknown"}`)));
  });
}

async function materializeBrowserModuleOverlay() {
  const modules = await localModuleClosure({
    root: project,
    entries: ["app.js"],
    resolveFile: resolveBrowserPublicationSource,
  });
  for (const path of modules) {
    if (isLauncherBuildOutputPath(path)) continue;
    const target = resolve(buildRoot, path);
    const source = resolveBrowserPublicationSource(path);
    if (await filesMatch(source, target)) continue;
    await mkdir(resolve(target, ".."), { recursive: true });
    await cp(source, target);
  }
}

async function verifyOutputs(sources) {
  for (const source of sources) {
    const output = outputForSource(source);
    const info = await stat(output);
    if (!info.isFile() || !info.size) throw new Error(`Launcher build output is missing or empty: ${output}`);
  }
}

export async function ensureLauncherBuild({ force = false } = {}) {
  // Packaged self-host bundles intentionally contain compiled browser modules but not
  // TypeScript sources or the compiler. In that product boundary, compilation
  // is a maintainer responsibility that has already happened upstream.
  if (!hasTypeScriptSource) {
    const packagedModules = await Promise.all(packagedOutputRoots.map(collectGeneratedModules));
    if (packagedModules.some(files => files.length === 0)) {
      throw new Error("Prebuilt Launcher modules are missing from this distribution");
    }
    return Object.freeze({ built: false, mode: "prebuilt", sourceCount: 0 });
  }

  const sources = await collectSources();
  if (!sources.length) throw new Error(`Application TypeScript source tree is empty: ${sourceRoot}`);
  const declarationBridges = await collectDeclarationBridges();
  const sharedInputMtime = Math.max(
    (await stat(configPath)).mtimeMs,
    (await stat(tscPath)).mtimeMs,
    ...await Promise.all(declarationBridges.map(async path => (await stat(path)).mtimeMs)),
  );
  let stale = force;
  if (!stale) {
    const expectedOutputs = sources.map(outputForSource).sort();
    const actualOutputs = (await Promise.all(generatedOutputRoots.map(collectGeneratedModules))).flat().sort();
    stale = expectedOutputs.length !== actualOutputs.length
      || expectedOutputs.some((path, index) => path !== actualOutputs[index]);
  }
  if (!stale) {
    for (const source of sources) {
      const output = outputForSource(source);
      let outputInfo;
      try { outputInfo = await stat(output); } catch { stale = true; break; }
      const sourceInfo = await stat(source);
      if (!outputInfo.isFile() || !outputInfo.size || outputInfo.mtimeMs < Math.max(sourceInfo.mtimeMs, sharedInputMtime)) {
        stale = true;
        break;
      }
    }
  }
  if (stale) await runTypeScript();
  await verifyOutputs(sources);
  await materializeBrowserModuleOverlay();
  return Object.freeze({ built: stale, mode: "source", sourceCount: sources.length });
}

export const LAUNCHER_SOURCE_ROOT = resolve(sourceRoot, "launcher");
export const LAUNCHER_OUTPUT_ROOT = resolve(outputRoot, "launcher");
export const BROWSER_BUILD_ROOT = buildRoot;
