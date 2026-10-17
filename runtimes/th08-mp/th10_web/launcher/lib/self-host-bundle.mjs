import { FRONTEND_PACKAGE_FILES, resolveFrontendPackageSource } from "./frontend-manifest.mjs";
import { localModuleClosure } from "./browser-module-graph.mjs";
import { resolveBrowserPublicationSource } from "./launcher-build.mjs";
import { relative, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const project = resolve(fileURLToPath(new URL("..", import.meta.url)));

// The self-host bundle is a distributable product, not a trimmed source checkout.
// Every repository-owned file that can reach `npm run host` / `npm run import`
// is declared here. Maintainer tests, browser runners, publication tooling and
// relay operations stay outside this boundary.
const hostRuntimeRoots = [
  "src/app-shell-sw.js",
  "config/runtime-builds.json",
  "config/workspace.json",
  "host/build.mjs",
  "host/build-import.mjs",
  "host/lib/node-environment.mjs",
  "host/lib/process.mjs",
  "host/lib/python-environment.mjs",
  "host/lib/site-builder.mjs",
  "host/lib/thtk.mjs",
  "host/ogg-baselines/th06.json",
  "host/ogg-baselines/th07.json",
  "host/ogg-baselines/th08.json",
  "host/requirements.txt",
  "host/config/site-features.default.json",
  "integrations/thcrap.mjs",
  "lib/app-shell-build.mjs",
  "lib/app-shell-policy.mjs",
  "lib/browser-module-graph.mjs",
  "lib/build-profile.mjs",
  "lib/content-definition.mjs",
  "lib/frontend-manifest.mjs",
  "lib/host-config.mjs",
  "lib/launcher-build.mjs",
  "lib/preload-data-assembler.mjs",
  "lib/product-selection.mjs",
  "lib/publication-host-seed.mjs",
  "lib/host-workspace.mjs",
  "lib/release-manifest.mjs",
  "lib/runtime-build-profiles.mjs",
  "lib/runtime-data-layout.mjs",
  "lib/runtime-data-provider.mjs",
  "lib/runtime-release.mjs",
  "lib/workspace-layout.mjs",
  "scripts/convert_bgm_ogg.py",
  "scripts/inspect-host.mjs",
  "scripts/package-offline-game.mjs",
  "scripts/package-server.mjs",
  "scripts/prepare-host-artwork.py",
  "scripts/prepare-th06-language-pack.mjs",
  "scripts/serve-static.mjs",
  "scripts/subset-font.py",
  "scripts/touhou_formats.py",
  "scripts/verify-offline-game-package.mjs",
  "scripts/verify-runtime-release.mjs",
  "scripts/verify-server-build.mjs",
  "server/static-content-policy.mjs",
  "server/thcrap-ascii-contract.mjs",
  "server/thcrap-compiler.mjs",
  "server/thcrap-static-pack.mjs",
  "server/thcrap-string-contract.mjs",
  "server/thtk-runner.mjs",
];

const hostRuntimeModules = await localModuleClosure({
  root: project,
  entries: hostRuntimeRoots.filter(path => /\.(?:m?js)$/.test(path)),
  allowBareImports: true,
  allowDynamicImports: true,
  resolveFile: resolveBrowserPublicationSource,
});
const hostRuntimeFiles = [...new Set([...hostRuntimeRoots, ...hostRuntimeModules])];

const mappedFiles = [
  ["config/eagler-touhou.config.example.json", "eagler-touhou.config.json"],
  ["host/bundle/package.json", "package.json"],
  ["host/bundle/package-lock.json", "package-lock.json"],
  ["docs/SELF_HOSTING.md", "SELF-HOSTING.md"],
  ["docs/SELF_HOSTING_REFERENCE.md", "SELF-HOSTING-REFERENCE.md"],
];

function copyRule(source, target = source) {
  return Object.freeze({ source, target });
}

export const SELF_HOST_BUNDLE_COPY_RULES = Object.freeze([
  ...FRONTEND_PACKAGE_FILES
    .map(path => copyRule(relative(project, resolveFrontendPackageSource(path)).replaceAll("\\", "/"), path)),
  ...hostRuntimeFiles
    .filter(source => !FRONTEND_PACKAGE_FILES.includes(source))
    .sort()
    .map(source => copyRule(source)),
  ...mappedFiles.map(([source, target]) => copyRule(source, target)),
]);

export const SELF_HOST_BUNDLE_NODE_DEPENDENCIES = Object.freeze([
  "acorn",
  "fflate",
  "workbox-build",
]);
