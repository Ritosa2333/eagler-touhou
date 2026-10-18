import { resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { browserModuleClosure } from "./browser-module-graph.mjs";
import { ensureLauncherBuild, resolveBrowserPublicationSource } from "./launcher-build.mjs";

const project = resolve(fileURLToPath(new URL("..", import.meta.url)));
await ensureLauncherBuild();

// Browser entrypoints are the only JavaScript URLs owned manually. Their full
// static ESM dependency closure is derived from source, so adding a module
// cannot accidentally publish an online-only launcher or omit it from precache.
export const BROWSER_MODULE_ENTRYPOINTS = Object.freeze(["app.js"]);
export const BROWSER_MODULE_FILES = await browserModuleClosure({
  root: project,
  entries: BROWSER_MODULE_ENTRYPOINTS,
  resolveFile: resolveBrowserPublicationSource,
});

// Repository-owned non-module frontend delivery manifest. Original-game-
// derived artwork is intentionally absent: host assembly supplies it later.
const entries = [
  ["index.html", true],
  ["site.webmanifest", true],
  ["styles.css", true],
  ["touch-guide.css", true],
  ["about.css", true],
  // Origin migration must always use the current network document. Caching it
  // in the App Shell could leave either side of a cutover on stale protocol
  // code even when the server correctly marks the page for revalidation.
  ["migrate.html", false],
  ["legacy-mount-retirement-sw.js", false],
  ["about.html", true],
  ["faq.html", true],
  ["vendor/fflate.min.js", true],
  ["vendor/webaudio-tinysynth.min.js", true],
  ["vendor/fflate.LICENSE", false],
  ["vendor/webaudio-tinysynth.LICENSE", false],
  ["assets/touch-rotate-landscape.webp", true],
  ["assets/notice-bilibili.svg", true],
  ["assets/notice-touhou-cloud.png", true],
  ["assets/notice-github.svg", true],
  ["assets/notice-qq.svg", true],
  ["assets/fonts/touhou98.woff2", true],
  ["assets/fonts/unifont-site.woff2", true],
  ["assets/fonts/yatra-one-latin.woff2", true],
  ["assets/fonts/chill-round-gothic-site-medium.woff2", true],
  ["assets/fonts/chill-round-gothic-site-bold.woff2", true],
  ["assets/fonts/chill-round-gothic-site-heavy.woff2", true],
  ["assets/fonts/OFL-Unifont.txt", false],
  ["assets/fonts/OFL-YatraOne.txt", false],
  ["assets/fonts/OFL-ChillRoundGothic.txt", false],
  ["NOTICE.txt", false],
  ["CHANGELOG.txt", false],
  ["README.md", false],
  ["ASSETS.md", false],
  ["THIRD_PARTY.md", false],
];

const staticPackageFiles = entries.map(([path]) => path);
const staticAppShellFiles = entries.filter(([, appShell]) => appShell).map(([path]) => path);

export const FRONTEND_PACKAGE_FILES = Object.freeze([
  ...staticPackageFiles,
  ...BROWSER_MODULE_FILES,
]);
export const APP_SHELL_FILES = Object.freeze([
  ...staticAppShellFiles,
  ...BROWSER_MODULE_FILES,
]);
export const PUBLIC_ASSET_FILES = Object.freeze(
  FRONTEND_PACKAGE_FILES.filter(path => path.startsWith("assets/")),
);

export function resolveFrontendPackageSource(path) {
  if (!FRONTEND_PACKAGE_FILES.includes(path)) throw new Error(`unknown frontend package file: ${path}`);
  return resolveBrowserPublicationSource(path);
}

export function hostArtworkFiles(games) {
  const files = games.flatMap(game => [
    `${game}-card.webp`,
    ...(game === "th06" ? ["th06.ico"] : []),
  ]);
  // The shell favicon is a site-level resource even when a deployment selects
  // only TH07/TH08. It is reconstructed from TH06 artwork, but is not owned by
  // the TH06 product-selection boundary.
  if (!files.includes("th06.ico")) files.push("th06.ico");
  return Object.freeze(files);
}
