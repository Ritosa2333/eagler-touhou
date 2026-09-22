#!/usr/bin/env node
import { freezeHostRuntimes, verifyRuntimePublication } from "../lib/runtime-generations.mjs";
import { createHash, randomUUID } from "node:crypto";
import { copyFile, mkdir, readFile, readdir, rename, rm, writeFile } from "node:fs/promises";
import { dirname, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { buildAppShell } from "../lib/app-shell-build.mjs";
import { deploymentAppShellPatterns, runtimeAppShellPaths } from "../lib/app-shell-policy.mjs";
import { FRONTEND_PACKAGE_FILES, hostArtworkFiles, resolveFrontendPackageSource } from "../lib/frontend-manifest.mjs";
import { ensureLauncherBuild } from "../lib/launcher-build.mjs";
import { HOST_MANIFEST_FILE, validateHostManifest } from "../lib/contracts/host-manifest.mjs";
import { sourceIdentity, writeReleaseManifest } from "../lib/release-manifest.mjs";
import { WORKSPACE_REPOSITORIES } from "../lib/workspace-layout.mjs";
import { normalizeSiteUrl, writeSiteMetadata } from "../lib/site-metadata.mjs";
import { PRIVATE_FRONTEND_ASSETS, privateFrontendAssetSource } from "../lib/private-frontend-assets.mjs";

const project = resolve(fileURLToPath(new URL("..", import.meta.url)));
const values = process.argv.slice(2);
const refreshFrontend = values.includes("--frontend");
const siteUrlOption = values.find(value => value.startsWith("--site-url="));
const artworkOption = values.find(value => value.startsWith("--artwork-dir="));
const rootValue = values.find(value => !value.startsWith("--"));
if (!rootValue || values.some(value => value !== rootValue && value !== "--frontend" && value !== siteUrlOption && value !== artworkOption)) {
  throw new Error("usage: node scripts/refresh-deployment-app-shell.mjs <deployment-root> [--frontend] [--site-url=https://example.com/] [--artwork-dir=<path>]");
}
const root = resolve(rootValue);
const artworkRoot = artworkOption ? resolve(artworkOption.slice("--artwork-dir=".length)) : null;
if (artworkRoot && !refreshFrontend) throw new Error("--artwork-dir requires --frontend");

const appRoot = root;
const swPath = resolve(appRoot, "app-shell-sw.js");
const deploymentPath = resolve(root, "deployment.json");
const hostManifestPath = resolve(appRoot, HOST_MANIFEST_FILE);
const [deployment, hostManifest] = await Promise.all([
  readFile(deploymentPath, "utf8").then(JSON.parse),
  readFile(hostManifestPath, "utf8").then(JSON.parse).then(validateHostManifest),
]);
if (deployment.format !== "eagler-touhou-deployment/1" || !Array.isArray(deployment.files)) {
  throw new Error("invalid deployment manifest");
}
const siteUrl = normalizeSiteUrl(siteUrlOption?.slice("--site-url=".length) || deployment.siteUrl);

// Source/deployment preparation, not a hot in-place server update. Migrate any
// legacy loose code as one verified generation before rebuilding the shell.
await freezeHostRuntimes(root, hostManifest);
await writeFile(hostManifestPath, `${JSON.stringify(hostManifest, null, 2)}\n`);
await verifyRuntimePublication(root, hostManifest);
const runtimePaths = runtimeAppShellPaths(hostManifest);
const inventoryPaths = new Set(deployment.files.map(item => item.path));
const availableHostArtwork = hostArtworkFiles(Object.keys(hostManifest.games)).filter(name =>
  inventoryPaths.has(`assets/${name}`));

const temporaryRoot = resolve(dirname(swPath), ".tmp");
await mkdir(temporaryRoot, { recursive: true });
const temporary = resolve(temporaryRoot, `app-shell-sw-${process.pid}-${randomUUID()}.js`);
try {
  if (refreshFrontend) {
    await ensureLauncherBuild();
    const inventory = new Map(deployment.files.map(item => [item.path, item]));
    for (const path of FRONTEND_PACKAGE_FILES) {
      const target = resolve(appRoot, path);
      await mkdir(dirname(target), { recursive: true });
      await copyFile(resolveFrontendPackageSource(path), target);
      const bytes = await readFile(target);
      const identity = { path, bytes: bytes.length, sha256: createHash("sha256").update(bytes).digest("hex") };
      const current = inventory.get(path);
      if (current) Object.assign(current, identity);
      else {
        deployment.files.push(identity);
        inventory.set(path, identity);
      }
    }
    for (const asset of PRIVATE_FRONTEND_ASSETS) {
      const source = privateFrontendAssetSource(asset.target);
      let bytes;
      try { bytes = await readFile(source); }
      catch (error) { if (error?.code === "ENOENT") continue; throw error; }
      const target = resolve(appRoot, asset.target);
      await mkdir(dirname(target), { recursive: true });
      await writeFile(target, bytes);
      const identity = {
        path: asset.target,
        bytes: bytes.length,
        sha256: createHash("sha256").update(bytes).digest("hex"),
      };
      const current = inventory.get(asset.target);
      if (current) Object.assign(current, identity);
      else {
        deployment.files.push(identity);
        inventory.set(asset.target, identity);
      }
    }
    if (artworkRoot) {
      for (const name of availableHostArtwork) {
        const path = `assets/${name}`;
        const target = resolve(appRoot, path);
        await copyFile(resolve(artworkRoot, name), target);
        const bytes = await readFile(target);
        Object.assign(inventory.get(path), {
          bytes: bytes.length,
          sha256: createHash("sha256").update(bytes).digest("hex"),
        });
      }
    }
    deployment.files.sort((left, right) => left.path.localeCompare(right.path, "en"));
    const metadataPaths = await writeSiteMetadata(appRoot, siteUrl);
    for (const path of metadataPaths) {
      const bytes = await readFile(resolve(appRoot, path));
      Object.assign(inventory.get(path), { bytes: bytes.length, sha256: createHash("sha256").update(bytes).digest("hex") });
    }
    if (siteUrl) deployment.siteUrl = siteUrl;
  }
  const result = await buildAppShell({
    quiet: true,
    globDirectory: appRoot,
    swDest: temporary,
    additionalGlobPatterns: deploymentAppShellPatterns({
      games: Object.keys(hostManifest.games),
      hostArtwork: availableHostArtwork,
    }),
    deferredPaths: runtimePaths,
    deferredPathPrefixes: ["runtime/"],
  });
  if (result.contract.entries.some(path => path.startsWith("runtime/"))) throw new Error("Runtime leaked into Launcher precache");
  await rename(temporary, swPath);

  const bytes = await readFile(swPath);
  const identity = {
    bytes: bytes.length,
    sha256: createHash("sha256").update(bytes).digest("hex"),
  };
  const inventoryEntry = deployment.files.find(item => item.path === "app-shell-sw.js");
  if (!inventoryEntry) throw new Error("deployment inventory does not own app-shell-sw.js");
  Object.assign(inventoryEntry, identity);
  // Regenerate the complete inventory: migration adds immutable directories and
  // removes old mutable files. Never carry a stale path/hash from the old tree.
  const files = [];
  async function inventoryTree(directory, prefix = "") {
    for (const item of await readdir(directory, { withFileTypes: true })) {
      if (item.name === ".tmp") continue;
      const path = prefix + item.name;
      if (item.isDirectory()) await inventoryTree(resolve(directory, item.name), path + "/");
      else if (!["deployment.json", "release-manifest.json", "checksums.txt"].includes(path)) {
        if (!item.isFile()) throw new Error(`Non-file in publication: ${path}`);
        const contents = await readFile(resolve(directory, item.name));
        files.push({ path, bytes: contents.length, sha256: createHash("sha256").update(contents).digest("hex") });
      }
    }
  }
  await inventoryTree(root);
  deployment.files = files.sort((a, b) => a.path.localeCompare(b.path, "en"));
  deployment.generatedAt = new Date().toISOString();
  deployment.appShell = result.contract;
  await writeFile(deploymentPath, `${JSON.stringify(deployment, null, 2)}\n`);

  if (deployment.releaseManifest === "release-manifest.json") {
    const previous = JSON.parse(await readFile(resolve(root, "release-manifest.json"), "utf8"));
    let launcherSource;
    try {
      launcherSource = await sourceIdentity(project);
    } catch {
      const provenance = JSON.parse(await readFile(resolve(project, "self-host-provenance.json"), "utf8"));
      if (provenance.schema !== "eagler-touhou/self-host-bundle-provenance/1" ||
          provenance.launcherRepository !== WORKSPACE_REPOSITORIES.launcher ||
          !provenance.launcherSource || typeof provenance.launcherSource !== "object") {
        throw new Error("cannot identify Launcher source for refreshed release provenance");
      }
      launcherSource = provenance.launcherSource;
    }
    await writeReleaseManifest(root, {
      profile: previous.profile,
      sources: { ...previous.sources, [WORKSPACE_REPOSITORIES.launcher]: launcherSource },
      parameters: previous.parameters || {},
    });
  }

  console.log(JSON.stringify({
    refreshed: true,
    frontend: refreshFrontend,
    artwork: Boolean(artworkRoot),
    buildId: result.buildId,
    precache: result.count,
    runtimeFiles: runtimePaths.length,
    appShellWorker: identity,
  }));
} finally {
  await rm(temporary, { force: true });
}
