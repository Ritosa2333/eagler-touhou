import { existsSync } from "node:fs";
import { stat } from "node:fs/promises";
import { resolve } from "node:path";
import { PRODUCT_GAMES } from "./contracts/product-catalog.mjs";
import { hostConfigWarnings, readHostConfig } from "./host-config.mjs";
import { verifyRuntimeRelease } from "./runtime-release.mjs";

export const HOST_WORKSPACE_SCHEMA = "eagler-touhou/host-workspace/1";
export const HOST_WORKSPACE_GAMES = Object.freeze(Object.keys(PRODUCT_GAMES));

const TH06_ARCHIVES = Object.freeze([
  "紅魔郷CM.DAT",
  "紅魔郷ED.DAT",
  "紅魔郷IN.DAT",
  "紅魔郷MD.DAT",
  "紅魔郷ST.DAT",
  "紅魔郷TL.DAT",
]);

function normalizeMusic(music) {
  const values = Array.isArray(music) ? music : String(music || "midi,ogg").split(",");
  const modes = [...new Set(values.map(value => String(value).trim().toLowerCase()).filter(Boolean))];
  if (!modes.length) modes.push("midi", "ogg");
  if (modes.some(mode => !["midi", "ogg"].includes(mode))) {
    throw new Error("the self-host workflow supports only midi or midi,ogg; use the site assembly entry for WAV/custom music layouts");
  }
  if (!modes.includes("midi")) modes.unshift("midi");
  return modes;
}

async function requireFile(path, label) {
  let info;
  try { info = await stat(path); } catch { throw new Error(`${label} not found: ${path}`); }
  if (!info.isFile() || info.size <= 0) throw new Error(`${label} is not a non-empty file: ${path}`);
  return path;
}

async function requireDirectory(path, label) {
  let info;
  try { info = await stat(path); } catch { throw new Error(`${label} not found: ${path}`); }
  if (!info.isDirectory()) throw new Error(`${label} is not a directory: ${path}`);
  return path;
}

export function hostWorkspacePaths(root) {
  const hostRoot = resolve(root);
  const gamesRoot = resolve(hostRoot, "games");
  const distRoot = resolve(hostRoot, "dist");
  return Object.freeze({
    schema: HOST_WORKSPACE_SCHEMA,
    root: hostRoot,
    config: resolve(hostRoot, "eagler-touhou.config.json"),
    runtimeRelease: resolve(hostRoot, "runtime-release"),
    dist: distRoot,
    site: resolve(distRoot, "site"),
    importSite: resolve(distRoot, "import-site"),
    importPackages: resolve(distRoot, "import"),
    shared: resolve(hostRoot, "shared"),
    games: Object.freeze(Object.fromEntries(HOST_WORKSPACE_GAMES.map(game => [game, resolve(gamesRoot, game)]))),
  });
}

export async function inspectHostWorkspace(root, { music = ["midi", "ogg"] } = {}) {
  const layout = hostWorkspacePaths(root);
  const modes = normalizeMusic(music);
  const hostConfig = await readHostConfig(layout.config);
  await requireDirectory(layout.runtimeRelease, "Runtime Release directory");
  const runtimeRelease = await verifyRuntimeRelease(layout.runtimeRelease);
  for (const game of HOST_WORKSPACE_GAMES) {
    if (!runtimeRelease.games[game]) throw new Error(`Runtime Release does not contain ${game}`);
    await requireDirectory(layout.games[game], `${game.toUpperCase()} game directory`);
  }

  for (const name of TH06_ARCHIVES) {
    await requireFile(resolve(layout.games.th06, name), `TH06 ${name}`);
  }
  await requireFile(resolve(layout.games.th07, "th07.dat"), "TH07 th07.dat");
  await requireFile(resolve(layout.games.th08, "th08.dat"), "TH08 th08.dat");

  if (modes.includes("ogg")) {
    for (let index = 1; index <= 17; index++) {
      const name = `th06_${String(index).padStart(2, "0")}.wav`;
      await requireFile(resolve(layout.games.th06, "bgm", name), `TH06 BGM ${name}`);
    }
    await requireFile(resolve(layout.games.th07, "thbgm.dat"), "TH07 thbgm.dat");
    await requireFile(resolve(layout.games.th08, "thbgm.dat"), "TH08 thbgm.dat");
  }

  const bundledUnicode = resolve(layout.shared, "unifont.otf");
  const bundledJapanese = resolve(layout.shared, "japanese-font.otf");
  return Object.freeze({
    ...layout,
    music: Object.freeze(modes),
    bundledFonts: Object.freeze({
      unicode: existsSync(bundledUnicode) ? bundledUnicode : null,
      japanese: existsSync(bundledJapanese) ? bundledJapanese : null,
    }),
    hostConfig,
    warnings: hostConfigWarnings(hostConfig),
    runtimeReleaseSchema: runtimeRelease.schema,
  });
}
