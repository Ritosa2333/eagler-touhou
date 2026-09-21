import { createHash } from "node:crypto";
import { mkdtemp, mkdir, readFile, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { getManifest, injectManifest } from "workbox-build";
import { APP_SHELL_FILES, resolveFrontendPackageSource } from "./frontend-manifest.mjs";
import { APP_SHELL_SOURCE_FILE, createAppShellContract } from "./app-shell-policy.mjs";

const project = resolve(fileURLToPath(new URL("..", import.meta.url)));
const defaultSourcePath = resolve(project, APP_SHELL_SOURCE_FILE);

function buildHash(entries, source, deferredPaths) {
  const hash = createHash("sha256");
  hash.update(source);
  for (const entry of [...entries].sort((left, right) => left.url.localeCompare(right.url))) {
    hash.update("\0");
    hash.update(entry.url);
    hash.update("\0");
    hash.update(entry.revision || "");
  }
  for (const path of [...deferredPaths].sort()) {
    hash.update("\0deferred\0");
    hash.update(path);
  }
  return hash.digest("hex").slice(0, 20);
}

export async function buildAppShell({
  quiet = false,
  globDirectory = project,
  swDest = null,
  sourcePath = defaultSourcePath,
  additionalGlobPatterns = [],
  deferredPaths = [],
  deferredPathPrefixes = [],
} = {}) {
  const resolvedGlobDirectory = resolve(globDirectory);
  const localAppShellPatterns = [];
  const mappedManifestEntries = [];
  let mappedManifestBytes = 0;
  for (const path of APP_SHELL_FILES) {
    try {
      const bytes = await readFile(resolve(resolvedGlobDirectory, path));
      if (!bytes.length) throw new Error(`App Shell input is empty: ${path}`);
      localAppShellPatterns.push(path);
    } catch (error) {
      if (error?.code !== "ENOENT" || resolvedGlobDirectory !== project) throw error;
      const bytes = await readFile(resolveFrontendPackageSource(path));
      if (!bytes.length) throw new Error(`App Shell input is empty: ${path}`);
      mappedManifestBytes += bytes.length;
      mappedManifestEntries.push({
        url: path,
        revision: createHash("sha256").update(bytes).digest("hex"),
      });
    }
  }
  const manifestConfig = {
    globDirectory: resolvedGlobDirectory,
    globPatterns: [...localAppShellPatterns, ...additionalGlobPatterns],
    maximumFileSizeToCacheInBytes: 8 * 1024 * 1024,
    // Both Workbox passes must use the same SHA-256 byte identities.
    manifestTransforms: [async entries => ({
      manifest: await Promise.all(entries.map(async entry => ({
        ...entry,
        revision: createHash("sha256").update(await readFile(resolve(resolvedGlobDirectory, entry.url))).digest("hex"),
      }))),
      warnings: [],
    })],
  };
  const runtimeSource = await readFile(resolve(project, "src/runtime-cache-sw.js"), "utf8");
  const shellSource = await readFile(sourcePath, "utf8");
  const source = `${runtimeSource}\n${shellSource}`;
  if (!source.includes("__APP_SHELL_BUILD_ID__") || !source.includes("__APP_SHELL_DEFERRED_PATHS__") || !source.includes("self.__WB_MANIFEST")) {
    throw new Error("App Shell Service Worker source is missing Workbox build placeholders");
  }
  const firstPass = await getManifest(manifestConfig);
  const sourceEntries = [...firstPass.manifestEntries, ...mappedManifestEntries];
  const indexEntry = sourceEntries.find(entry => entry.url === "index.html");
  if (!indexEntry?.revision) throw new Error("Workbox App Shell manifest is missing index.html");
  const navigationEntry = { url: "./", revision: indexEntry.revision };
  const manifestEntries = [...sourceEntries, navigationEntry];
  const normalizePath = path => String(path).replaceAll("\\", "/").replace(/^\.\//, "");
  const normalizedDeferredPrefixes = [...new Set(deferredPathPrefixes.map(normalizePath))];
  const normalizedDeferredPaths = [...new Set([
    ...deferredPaths.map(normalizePath),
    ...manifestEntries.map(entry => normalizePath(entry.url))
      .filter(path => normalizedDeferredPrefixes.some(prefix => path.startsWith(prefix))),
  ])];
  const runtimeEntries = manifestEntries.filter(entry => entry.url.startsWith("runtime/"));
  // A nested multiplayer entry is a distinct Runtime, not a dependency of the
  // single-player one. Other nested module/font assets stay in their owner set.
  const runtimeRoots = [...new Set(runtimeEntries.filter(entry => entry.url.endsWith(".html"))
    .map(entry => entry.url.slice(0, entry.url.lastIndexOf("/") + 1)))].sort((a, b) => b.length - a.length);
  const runtimeCatalog = { schema: "eagler-touhou/runtime-cache/1", hostProtocol: "eagler-touhou/1",
    groups: runtimeRoots.map(root => ({ root, entries: runtimeEntries
      .filter(entry => runtimeRoots.find(candidate => entry.url.startsWith(candidate)) === root)
      .map(({ url, revision }) => ({ url, revision })).sort((a, b) => a.url.localeCompare(b.url)) })) };
  // Machine-readable JSON in the existing release artifact; no extra endpoint,
  // source evaluation or public version-directory deployment contract.
  const catalogJson = JSON.stringify(runtimeCatalog);
  const catalogSource = `// EAGLER_RUNTIME_CATALOG_V1 ${catalogJson}\nself.__EAGLER_RUNTIME_CATALOG = ${catalogJson};\n`;
  const buildId = buildHash(manifestEntries, source, normalizedDeferredPaths);
  const temp = await mkdtemp(join(tmpdir(), "eagler-touhou-app-shell-"));
  const temporarySource = resolve(temp, "sw-src.js");
  const temporaryDestination = resolve(temp, "sw.js");
  try {
    await writeFile(temporarySource, catalogSource + source
      .replaceAll("__APP_SHELL_BUILD_ID__", buildId)
      .replace("__APP_SHELL_DEFERRED_PATHS__", JSON.stringify(normalizedDeferredPaths)), "utf8");
    const injected = await injectManifest({
      ...manifestConfig,
      additionalManifestEntries: [...mappedManifestEntries, navigationEntry],
      swSrc: temporarySource,
      swDest: temporaryDestination,
      injectionPoint: "self.__WB_MANIFEST",
    });
    const worker = await readFile(temporaryDestination);
    if (swDest) {
      await mkdir(dirname(swDest), { recursive: true });
      await writeFile(swDest, worker);
    }
    const warnings = [...firstPass.warnings, ...injected.warnings];
    if (warnings.length && !quiet) warnings.forEach(warning => console.warn(`Workbox: ${warning}`));
    const size = firstPass.size + mappedManifestBytes;
    if (!quiet) console.log(`App Shell Workbox: ${buildId} - ${injected.count} files - ${size} bytes`);
    return Object.freeze({
      buildId, count: injected.count, size,
      warnings: Object.freeze(warnings), worker,
      manifestEntries: Object.freeze(manifestEntries.map(entry => Object.freeze({ ...entry }))),
      contract: createAppShellContract({ buildId, manifestEntries }),
    });
  } finally { await rm(temp, { recursive: true, force: true }); }
}
