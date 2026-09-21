import { readdir, readFile, stat } from "node:fs/promises";
import { resolve, relative } from "node:path";
import { fileURLToPath } from "node:url";
import { assertPublicSourceTree } from "../lib/public-source-policy.mjs";

const launcher = resolve(fileURLToPath(new URL("..", import.meta.url)));
const workspace = resolve(launcher, "..");
const runtimeRepos = ["th06-eagler", "th07-eagler", "th08-eagler", "th10-eagler"];
const argv = process.argv.slice(2);
const workspaceMode = argv.includes("--workspace");
const rootIndex = argv.indexOf("--root");
const explicitRoot = rootIndex >= 0 ? resolve(argv[rootIndex + 1] || "") : null;
const roots = explicitRoot ? [explicitRoot] : workspaceMode ? [launcher, ...runtimeRepos.map(name => resolve(workspace, name))] : [launcher];

const ignoredDirs = new Set([".git", "node_modules", ".cache", "dist", "web", ".cmake", "build", "__pycache__"]);
const allowedLegacy = new Set([
  "legacy/README.md", "legacy/legacy-game-pack.mjs", "legacy/legacy-import-storage.mjs", "legacy/legacy-package-adapter.mjs",
]);
// Exact reviewed source assets only. PWA ET monograms are original geometric
// project artwork (GPL-3.0-or-later), not extracted game data; see ASSETS.md.
const allowedAssets = new Set([
  "public/assets/touch-rotate-landscape.webp",
  "public/assets/notice-bilibili.svg",
  "public/assets/notice-touhou-cloud.png",
  "public/assets/notice-github.svg",
  "public/assets/notice-qq.svg",
  "public/assets/pwa/icon.svg",
  "public/assets/pwa/icon-192.png",
  "public/assets/pwa/icon-512.png",
  "public/assets/pwa/icon-maskable-512.png",
  "public/assets/pwa/apple-touch-icon.png",
]);
const allowedFonts = new Set([
  "public/assets/fonts/touhou98.woff2",
  "public/assets/fonts/unifont-site.woff2",
  "public/assets/fonts/yatra-one-latin.woff2",
  "public/assets/fonts/chill-round-gothic-site-medium.woff2",
  "public/assets/fonts/chill-round-gothic-site-bold.woff2",
  "public/assets/fonts/chill-round-gothic-site-medium-critical.woff2",
  "public/assets/fonts/chill-round-gothic-site-medium-deferred.woff2",
  "public/assets/fonts/chill-round-gothic-site-bold-critical.woff2",
  "public/assets/fonts/chill-round-gothic-site-bold-deferred.woff2",
  "public/assets/fonts/chill-round-gothic-site-heavy.woff2",
  "public/assets/fonts/OFL-Unifont.txt",
  "public/assets/fonts/OFL-YatraOne.txt",
  "public/assets/fonts/OFL-ChillRoundGothic.txt",
]);
const failures = [];

function fail(repo, path, reason) {
  failures.push(`${repo}/${path} (${reason})`);
}
function check(repo, path) {
  const name = path.split("/").at(-1);
  if (repo === "eagler-touhou" && path.startsWith("legacy/") && !allowedLegacy.has(path)) {
    fail(repo, path, "unreviewed legacy compatibility file");
  }
  if (/(^|\/)(games|replays)(\/|$)/i.test(path)) fail(repo, path, "private data directory");
  if (/(^|\/)(hosted-game-data|game-packages|server-deploy|probe-reports|store-probes|migration-probes)(\/|$)/i.test(path)) fail(repo, path, "generated output directory");
  if (/(^|\/)(netplay-probe(?:[-_].*)?|host-build-probe(?:[-_].*)?|\.self-host[-_].*|self-host-.*|server-deploy.*)(\/|$)/i.test(path)) fail(repo, path, "private/generated probe output");
  if (/^(?:th\d+(?:mp)?\.(?:dat|data|zip|exe)|(?:score|log|thbgm|bgm|wave)\.dat|.*\.rpy)$/i.test(name)) fail(repo, path, "original-game/generated resource");
  if (/\.(?:wasm|pdb|ogg|wav|mp3|flac|ttc|ttf|otf)$/i.test(name)) fail(repo, path, "binary/generated resource");
  if (name === "cloudflare.toml") fail(repo, path, "operator-local configuration");
  if (name === "server.local.ps1" || name === "server.config.local.ps1") fail(repo, path, "operator-local configuration");
  if (name === "eagler-touhou.config.json") fail(repo, path, "operator-local configuration");
  if (repo === "eagler-touhou" && path.startsWith("public/assets/") && !allowedAssets.has(path) && !allowedFonts.has(path)) fail(repo, path, "unreviewed public asset");
}

async function walk(root, directory = root) {
  const repo = root.split(/[\\/]/).at(-1);
  for (const entry of await readdir(directory, { withFileTypes: true })) {
    const path = resolve(directory, entry.name);
    const rel = relative(root, path).replaceAll("\\", "/");
    if (entry.isDirectory()) {
      if (ignoredDirs.has(entry.name) || /^build(?:[-_].*)?$/.test(entry.name)) continue;
      if (/^(?:hosted-game-data|game-packages|server-deploy|netplay-probe|host-build-probe)/.test(entry.name)) {
        fail(repo, rel, "private/generated output directory");
        continue;
      }
      await walk(root, path);
    } else if (entry.isFile()) {
      check(repo, rel);
    }
  }
}

for (const root of roots) {
  if (!(await stat(root)).isDirectory()) throw new Error(`missing publication root: ${root}`);
  if (root === launcher || root.split(/[\\/]/).at(-1) === "eagler-touhou") {
    await assertPublicSourceTree(root);
  }
  await walk(root);
}

if (failures.length) {
  throw new Error(`publication contains private/generated resources:\n${failures.sort().map(item => ` - ${item}`).join("\n")}`);
}
console.log(`publication audit: PASS (${workspaceMode ? "workspace" : "launcher"})`);
