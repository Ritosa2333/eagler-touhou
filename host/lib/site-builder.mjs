import { existsSync } from "node:fs";
import { cp, mkdir, readFile, readdir, rename, rm, stat, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { resolve } from "node:path";
import { randomUUID } from "node:crypto";
import { inspectHostWorkspace } from "../../lib/host-workspace.mjs";
import { workspacePath } from "../../lib/workspace-layout.mjs";
import { ensurePythonEnvironment } from "./python-environment.mjs";
import { ensureThtk } from "./thtk.mjs";
import { run } from "./process.mjs";

const GAMES = Object.freeze(["th06", "th07", "th08", "th10"]);
const TH06_ARCHIVES = Object.freeze([
  "紅魔郷CM.DAT", "紅魔郷ED.DAT", "紅魔郷IN.DAT",
  "紅魔郷MD.DAT", "紅魔郷ST.DAT", "紅魔郷TL.DAT",
]);
const DEFAULT_LANGUAGES = Object.freeze(["ja", "lang_zh-hans", "lang_en"]);

function script(projectRoot, path) {
  return resolve(projectRoot, path);
}

async function fileExists(path) {
  try { return (await stat(path)).isFile(); } catch { return false; }
}

async function treeHasFileNewerThan(root, cutoff) {
  let entries;
  try {
    entries = await readdir(root, { withFileTypes: true });
  } catch (error) {
    if (error?.code === "ENOENT") return false;
    throw error;
  }
  for (const entry of entries) {
    const path = resolve(root, entry.name);
    if (entry.isDirectory()) {
      if (await treeHasFileNewerThan(path, cutoff)) return true;
    } else if (entry.isFile() && (await stat(path)).mtimeMs > cutoff) {
      return true;
    }
  }
  return false;
}

async function reusableHostedBase(projectRoot, layout) {
  let deployment;
  try {
    deployment = JSON.parse(await readFile(resolve(layout.site, "deployment.json"), "utf8"));
  } catch {
    return false;
  }
  if (deployment.format !== "eagler-touhou-deployment/1" ||
      deployment.profile !== "web-validation-self-host" ||
      deployment.resourceMode !== "hosted") return false;
  const actualGames = Array.isArray(deployment.games)
    ? [...new Set(deployment.games.map(value => String(value).toLowerCase()))].sort()
    : [];
  if (actualGames.join(",") !== [...GAMES].sort().join(",")) return false;
  const actualMusic = Array.isArray(deployment.music)
    ? [...new Set(deployment.music.map(value => String(value).toLowerCase()))].sort()
    : [];
  if (actualMusic.join(",") !== [...layout.music].sort().join(",")) return false;
  const generatedAt = Date.parse(String(deployment.generatedAt || ""));
  if (!Number.isFinite(generatedAt)) return false;
  const provenancePath = resolve(projectRoot, "self-host-provenance.json");
  if (existsSync(provenancePath)) {
    try {
      const provenance = JSON.parse(await readFile(provenancePath, "utf8"));
      const releaseManifest = JSON.parse(await readFile(resolve(layout.site, "release-manifest.json"), "utf8"));
      if (provenance.schema !== "eagler-touhou/self-host-bundle-provenance/1" ||
          !provenance.launcherRepository || !provenance.launcherSource ||
          JSON.stringify(releaseManifest.sources?.[provenance.launcherRepository]) !== JSON.stringify(provenance.launcherSource)) {
        return false;
      }
    } catch {
      return false;
    }
  }
  for (const root of [layout.runtimeRelease, layout.shared, ...GAMES.map(game => layout.games[game])]) {
    if (await treeHasFileNewerThan(root, generatedAt)) return false;
  }
  if (existsSync(layout.config) && (await stat(layout.config)).mtimeMs > generatedAt) return false;
  const verification = await run(process.execPath, [script(projectRoot, "scripts/verify-server-build.mjs"), layout.site], {
    cwd: projectRoot,
    capture: true,
    allowFailure: true,
  });
  return verification.code === 0;
}

async function resolveFonts(projectRoot, layout) {
  let unicode = layout.bundledFonts.unicode;
  if (!unicode) unicode = workspacePath("dependencies", "unifont-15.1.05", "unifont-15.1.05.otf");
  if (!await fileExists(unicode)) throw new Error(`Unicode font not found: ${unicode}`);

  let japanese = layout.bundledFonts.japanese;
  if (!japanese && process.platform === "win32" && process.env.WINDIR) {
    const windowsFont = resolve(process.env.WINDIR, "Fonts", "msgothic.ttc");
    if (await fileExists(windowsFont)) japanese = windowsFont;
  }
  if (!japanese) {
    japanese = unicode;
    console.warn("[Build] Japanese runtime font is not bundled; using the Unicode font fallback");
  }
  return Object.freeze({ unicode, japanese });
}

async function prepareFeatureConfig(projectRoot, layout, resourceMode) {
  const basePath = script(projectRoot, "host/config/site-features.default.json");
  const features = JSON.parse(await readFile(basePath, "utf8"));
  features.resourceMode = resourceMode;
  for (const game of ["th06", "th07"]) {
    features.games[game] ||= {};
    features.games[game].languages = [...DEFAULT_LANGUAGES];
  }
  if (layout.hostConfig.netplay.relay) features.netplayRelay = layout.hostConfig.netplay.relay;
  else delete features.netplayRelay;
  if (layout.hostConfig.externalImportSource.url) {
    features.gameDataFallback = {
      url: layout.hostConfig.externalImportSource.url,
      ...(layout.hostConfig.externalImportSource.hint ? { hint: layout.hostConfig.externalImportSource.hint } : {}),
    };
  } else {
    delete features.gameDataFallback;
  }
  const path = resolve(tmpdir(), `eagler-touhou-host-features-${randomUUID()}.json`);
  await writeFile(path, `${JSON.stringify(features, null, 2)}\n`);
  return path;
}

async function prepareLanguages(projectRoot, layout, python, thtk, font) {
  const languageRoot = resolve(layout.root, ".cache", "generated", "language-packs");
  const th06Archives = TH06_ARCHIVES.map(name => resolve(layout.games.th06, name));
  let task = 0;
  const total = 4;
  for (const language of ["lang_zh-hans", "lang_en"]) {
    const label = language === "lang_zh-hans" ? "Simplified Chinese" : "English";
    console.log(`[Languages ${++task}/${total}] TH06 ${label}`);
    await run(process.execPath, [
      script(projectRoot, "scripts/prepare-th06-language-pack.mjs"),
      "--game", "th06", "--language", language,
      "--thdat", thtk.thdat, "--thmsg", thtk.thmsg,
      "--archives", th06Archives.join(";"),
      "--output", resolve(languageRoot, "th06"),
      "--font-file", font, "--font-python", python,
    ], { cwd: projectRoot });
    console.log(`[Languages ${++task}/${total}] TH07 ${label}`);
    await run(process.execPath, [
      script(projectRoot, "scripts/prepare-th06-language-pack.mjs"),
      "--game", "th07", "--language", language,
      "--thdat", thtk.thdat, "--thmsg", thtk.thmsg,
      "--archive", resolve(layout.games.th07, "th07.dat"),
      "--output", resolve(languageRoot, "th07"),
      "--font-file", font, "--font-python", python,
    ], { cwd: projectRoot });
  }
  console.log("Default languages ready: Japanese / Simplified Chinese / English");
  return Object.freeze({
    th06: resolve(languageRoot, "th06"),
    th07: resolve(languageRoot, "th07"),
  });
}

async function prepareArtwork(projectRoot, layout, python, thtk) {
  const output = resolve(layout.root, ".cache", "generated", "host-artwork");
  const args = [
    script(projectRoot, "scripts/prepare-host-artwork.py"),
    `--output=${output}`,
    "--games=th06,th07,th08,th10",
    `--th06-dir=${layout.games.th06}`,
    `--th07-dir=${layout.games.th07}`,
    `--th08-dir=${layout.games.th08}`,
    `--th10-dir=${layout.games.th10}`,
  ];
  if (thtk?.thdat) args.push(`--thdat=${thtk.thdat}`);
  if (thtk?.thanm) args.push(`--thanm=${thtk.thanm}`);
  await run(python, args, { cwd: projectRoot });
  return output;
}

async function prepareTh10Content(projectRoot, layout) {
  const output = resolve(layout.root, ".cache", "generated", "th10");
  await rm(output, { recursive: true, force: true });
  const supplied = resolve(layout.games.th10, "assets-ogg");
  if (await fileExists(resolve(supplied, "th10.data")) && await fileExists(resolve(supplied, "bgm-ogg", "th10_00.ogg"))) {
    await cp(supplied, output, { recursive: true });
    return output;
  }
  const bundledRoot = resolve(projectRoot, "th10-runtime");
  const bundledScript = resolve(bundledRoot, "scripts", "prepare-eagler-content.mjs");
  const workspaceScript = workspacePath("th10", "scripts", "prepare-eagler-content.mjs");
  const useBundledPreparer = await fileExists(bundledScript);
  const preparationScript = useBundledPreparer ? bundledScript : workspaceScript;
  if (!await fileExists(preparationScript)) {
    throw new Error(`TH10 content is missing: provide ${supplied} or the bundled TH10 content preparer`);
  }
  const args = [
    preparationScript,
    `--original=${layout.games.th10}`,
    `--output=${output}`,
  ];
  if (useBundledPreparer) args.push(`--runtime=${bundledRoot}`);
  await run(process.execPath, args, { cwd: projectRoot });
  return output;
}

async function prepareTh06DataAssets(projectRoot, layout, python, fonts) {
  const output = resolve(layout.root, ".cache", "generated", "th06-data-assets");
  await rm(output, { recursive: true, force: true });
  await mkdir(output, { recursive: true });
  await cp(fonts.unicode, resolve(output, "unifont.otf"));
  await cp(fonts.japanese, resolve(output, "msgothic.ttc"));
  for (const name of TH06_ARCHIVES) await cp(resolve(layout.games.th06, name), resolve(output, name));

  const hitbox = resolve(output, "eagler-hitbox.png");
  const result = await run(python, [
    script(projectRoot, "scripts/touhou_formats.py"), "extract-th07-texture",
    "--archive", resolve(layout.games.th07, "th07.dat"),
    "--anm", "etama.anm",
    "--texture", "data/etama/etama2.png",
    "--output", hitbox,
  ], { cwd: projectRoot, capture: true, allowFailure: true });
  if (result.code !== 0) {
    await rm(hitbox, { force: true });
    console.warn(`[Host] TH06 focus-hitbox extraction was unavailable${result.stderr ? `: ${result.stderr}` : ""}`);
  }
  return output;
}

async function prepareOgg(projectRoot, layout, python, th10Content) {
  const roots = {};
  for (const game of GAMES.filter(game => game !== "th10")) {
    const output = game === "th06"
      ? resolve(layout.root, ".cache", "generated", game, "bgm")
      : resolve(layout.root, ".cache", "generated", game, "bgm-ogg");
    await run(python, [
      script(projectRoot, "scripts/convert_bgm_ogg.py"),
      "--game", game,
      "--original-dir", layout.games[game],
      "--output", output,
      "--baseline", script(projectRoot, `host/ogg-baselines/${game}.json`),
    ], { cwd: projectRoot });
    roots[game] = resolve(layout.root, ".cache", "generated", game);
  }
  roots.th10 = th10Content;
  return Object.freeze(roots);
}

export async function buildHostedSite({ projectRoot, hostRoot, music = "midi,ogg", python = "python" }) {
  const layout = await inspectHostWorkspace(hostRoot, { music });
  const modes = [...layout.music];
  console.log("[Build 1/6] Preparing build environment");
  const hostPython = await ensurePythonEnvironment({ projectRoot, hostRoot: layout.root, python });
  const thtk = await ensureThtk({ hostRoot: layout.root });
  const fonts = await resolveFonts(projectRoot, layout);

  console.log("[Build 2/6] Preparing language packs");
  const languages = await prepareLanguages(projectRoot, layout, hostPython, thtk, fonts.unicode);
  console.log("[Build 3/6] Preparing Launcher artwork");
  const artwork = await prepareArtwork(projectRoot, layout, hostPython, thtk);
  const th06DataAssets = await prepareTh06DataAssets(projectRoot, layout, hostPython, fonts);
  const th10DataAssets = await prepareTh10Content(projectRoot, layout);
  console.log("[Build 4/6] Preparing music");
  const ogg = modes.includes("ogg") ? await prepareOgg(projectRoot, layout, hostPython, th10DataAssets) : null;
  const features = await prepareFeatureConfig(projectRoot, layout, "hosted");
  try {
    console.log("[Build 5/6] Assembling static site");
    const args = [
      script(projectRoot, "scripts/package-server.mjs"),
      `--output=${layout.site}`,
      `--font=${fonts.unicode}`,
      `--vanilla-font=${fonts.japanese}`,
      `--music=${modes.join(",")}`,
      `--feature-config=${features}`,
      `--artwork-dir=${artwork}`,
      "--games=th06,th07,th08,th10",
      "--profile=web-validation-self-host",
      `--runtime-release=${layout.runtimeRelease}`,
      `--th06-assets=${layout.games.th06}`,
      `--th06-data-assets=${th06DataAssets}`,
      `--th07-assets=${layout.games.th07}`,
      `--th08-assets=${layout.games.th08}`,
      `--th10-assets=${layout.games.th10}`,
      `--th10-data-assets=${th10DataAssets}`,
      `--th06-language-packs=${languages.th06}`,
      `--th07-language-packs=${languages.th07}`,
    ];
    if (ogg) {
      for (const game of GAMES) args.push(`--${game}-ogg=${ogg[game]}`);
    }
    await run(process.execPath, args, { cwd: projectRoot });
    console.log("[Build 6/6] Verifying generated site");
    await run(process.execPath, [script(projectRoot, "scripts/verify-server-build.mjs"), layout.site], { cwd: projectRoot });
  } finally {
    await rm(features, { force: true });
  }
  return Object.freeze({ layout, python: hostPython, thtk, fonts });
}

export async function buildImportArtifacts({
  projectRoot,
  hostRoot,
  music = "midi,ogg",
  python = "python",
  rebuildHostedBase = false,
  testBuild = false,
}) {
  let layout = await inspectHostWorkspace(hostRoot, { music });
  if (!rebuildHostedBase && await reusableHostedBase(projectRoot, layout)) {
    console.log(`Reusing verified hosted base: ${layout.site}`);
  } else {
    ({ layout } = await buildHostedSite({ projectRoot, hostRoot, music, python }));
  }
  const features = await prepareFeatureConfig(projectRoot, layout, "import");
  const packageTemporaryRoot = resolve(layout.dist, ".tmp");
  const packageStaging = resolve(packageTemporaryRoot, `import.staging-${randomUUID()}`);
  try {
    console.log("[Import 1/3] Assembling import site");
    await run(process.execPath, [
      script(projectRoot, "scripts/package-server.mjs"),
      `--output=${layout.importSite}`,
      `--feature-config=${features}`,
      `--host-manifest=${resolve(layout.site, "host-manifest.json")}`,
      `--runtime-release=${layout.runtimeRelease}`,
      `--artwork-dir=${resolve(layout.site, "assets")}`,
      "--games=th06,th07,th08,th10",
      "--profile=web-validation-self-host-import",
      `--test-build=${testBuild ? "1" : "0"}`,
    ], { cwd: projectRoot });
    await run(process.execPath, [script(projectRoot, "scripts/verify-server-build.mjs"), layout.importSite], { cwd: projectRoot });

    console.log("[Import 2/3] Building game-content packages");
    await mkdir(packageStaging, { recursive: true });
    for (const game of GAMES) {
      const archive = resolve(packageStaging, `${game}.zip`);
      await run(process.execPath, [script(projectRoot, "scripts/package-offline-game.mjs"), layout.site, game, archive], { cwd: projectRoot });
      await run(process.execPath, [script(projectRoot, "scripts/verify-offline-game-package.mjs"), archive, game], { cwd: projectRoot });
    }
    await rm(layout.importPackages, { recursive: true, force: true });
    await mkdir(resolve(layout.importPackages, ".."), { recursive: true });
    await rename(packageStaging, layout.importPackages);
    console.log("[Import 3/3] Import artifacts verified");
  } finally {
    await rm(features, { force: true });
    await rm(packageStaging, { recursive: true, force: true });
  }
  return Object.freeze({ layout });
}
