import { existsSync, readFileSync } from "node:fs";
import { dirname, resolve } from "node:path";
import { loadSourceLayout } from "./source-layout.mjs";
import { launcherRoot } from "./workspace-layout.mjs";
import { loadRuntimeBuildConfig, cmakeCacheArguments } from "./runtime-build-profiles.mjs";

const compilerPath = sdk => ["install", "upstream"].map(part => resolve(sdk, part, "emscripten/emcc.py")).find(existsSync);
function requireDirectoryInput(path, marker, label) {
  if (!existsSync(resolve(path, marker))) throw new Error(`${label} is missing ${marker}: ${path}`);
  return path;
}
function feature(policy, requested, label) {
  if (requested != null && typeof requested !== "boolean") throw new Error(`${label} must be boolean`);
  if (policy !== "configurable" && requested != null && requested !== policy) throw new Error(`${label} is fixed to ${policy}`);
  return requested ?? (policy === "configurable" ? false : policy);
}

// Source compilation is separate from Runtime Release assembly. This function
// only plans existing per-title builders; it never creates output directories.
export async function planRuntimeSourceBuild(options = {}, context = {}) {
  const environment = context.environment ?? process.env;
  const layout = context.layout ?? loadSourceLayout(launcherRoot(), { environment });
  const config = context.config ?? await loadRuntimeBuildConfig();
  const { game, variant = "normal" } = options;
  const entry = config.games[game], definition = entry?.variants[variant];
  if (!definition) throw new Error(`unknown Runtime build: ${game}/${variant}`);
  const recipe = { ...entry.sourceBuild, ...definition.sourceBuild };
  if (!["cmake", "node"].includes(recipe.kind)) throw new Error(`source build is not declared: ${game}/${variant}`);
  const owner = definition.workspaceRepository ?? entry.workspaceRepository;
  const source = resolve(options.source ?? layout.path(owner));
  requireDirectoryInput(source, recipe.kind === "cmake" ? "CMakeLists.txt" : recipe.build[0], `${game}/${variant} source`);
  const jobs = Number(options.jobs ?? 4);
  if (!Number.isSafeInteger(jobs) || jobs < 1 || jobs > 64) throw new Error("jobs must be an integer from 1 to 64");
  const requestedSdk = options.emsdk ?? environment[`TH${game.slice(2)}_EMSDK`] ?? environment.EMSDK;
  const sdk = requestedSdk && resolve(requestedSdk);
  if (!sdk || !compilerPath(sdk)) throw new Error(`Emscripten compiler not found; specify --emsdk for ${game}/${variant}`);
  const versionFile = resolve(dirname(compilerPath(sdk)), "emscripten-version.txt");
  const version = existsSync(versionFile) ? readFileSync(versionFile, "utf8").trim() : null;
  if (options.emscriptenVersion && version?.replace(/-git$/, "") !== options.emscriptenVersion) {
    throw new Error(`Emscripten version mismatch: expected ${options.emscriptenVersion}, found ${version}`);
  }
  const mono = layout.projectRoot !== layout.webRoot;
  const pinned = resolve(source, "third_party/eagler-common");
  const commonMarker = recipe.commonMarker ?? "include/eagler/input/DialogueGesture.hpp";
  const common = resolve(options.common ?? environment.EAGLER_COMMON_ROOT ??
    (existsSync(pinned) || !mono ? pinned : layout.path("common")));
  requireDirectoryInput(common, commonMarker, "common dependency");
  for (const input of recipe.commonInputs ?? []) requireDirectoryInput(common, input, "common dependency");
  const python = options.python ?? environment.TH_PYTHON ?? environment.PYTHON ?? (process.platform === "win32" ? "python" : "python3");
  const thcrap = feature(definition.features.thcrap, options.thcrap, `${game}/${variant} thcrap`);
  const thprac = feature(definition.features.thprac, options.thprac, `${game}/${variant} thprac`);
  const env = { EMSDK: sdk, TH09_EMSDK: sdk, TH15_EMSDK: sdk, EM_CONFIG: resolve(sdk, ".emscripten"),
    PYTHON: python, TH_PYTHON: python, EMCC_CORES: String(jobs), TH_BUILD_JOBS: String(jobs),
    EAGLER_COMMON_ROOT: common, TH_ENABLE_THCRAP: thcrap ? "1" : "0", TH_ENABLE_THPRAC: thprac ? "1" : "0" };
  if (recipe.netplayCommonDirectory) {
    const netplay = resolve(options.netplayCommon ?? environment.EAGLER_COMMON_NETPLAY_ROOT ??
      (mono && !existsSync(resolve(source, recipe.netplayCommonDirectory)) ? common : resolve(source, recipe.netplayCommonDirectory)));
    requireDirectoryInput(netplay, "include/eagler/netplay/SessionChannel.hpp", "netplay common dependency");
    env.EAGLER_COMMON_NETPLAY_ROOT = netplay;
  }
  const steps = [];
  if (recipe.kind === "cmake") {
    const build = resolve(options.buildDirectory ?? resolve(source, "build/web", variant));
    const toolchain = ["install", "upstream"].map(part => resolve(sdk, part, "emscripten/cmake/Modules/Platform/Emscripten.cmake")).find(existsSync);
    if (!toolchain) throw new Error(`Emscripten CMake toolchain not found: ${sdk}`);
    const cache = { ...config.defaults.cache, ...definition.cache, TH_ENABLE_THCRAP: thcrap, TH_ENABLE_THPRAC: thprac,
      TH_EXTERNAL_ASSETS: !options.assets, CMAKE_TOOLCHAIN_FILE: toolchain, EAGLER_COMMON_ROOT: common,
      TH_RUNTIME_EXTENSION_CMAKE: options.runtimeExtension ?? "" };
    if (options.assets) cache[entry.assetRootVariable] = resolve(options.assets);
    if (thprac && !options.runtimeExtension) throw new Error("THPrac requires an explicit runtimeExtension");
    if (options.imgui) cache.THPRAC_IMGUI_ROOT = resolve(options.imgui);
    if (thcrap || thprac) {
      const font = resolve(options.font ?? layout.path("dependencies", "unifont-15.1.05/unifont-15.1.05.otf"));
      if (!existsSync(font)) throw new Error(`font input not found: ${font}`);
      cache.TH_UNIFONT_FILE = font;
    }
    if (options.ninja) cache.CMAKE_MAKE_PROGRAM = options.ninja;
    const command = options.cmake ?? "cmake";
    steps.push({ command, args: ["-S", source, "-B", build, "-G", "Ninja", ...cmakeCacheArguments(cache)], cwd: source });
    steps.push({ command, args: ["--build", build, "--parallel", String(jobs)], cwd: source });
  } else {
    for (const name of ["buildDirectory", "assets", "font", "imgui", "runtimeExtension", "ninja", "cmake"]) {
      if (options[name] != null) throw new Error(`${name} is only supported by CMake source builds`);
    }
    const args = [...recipe.build];
    if (thprac && recipe.practiceFlag) args.push(recipe.practiceFlag);
    steps.push({ command: process.execPath, args: [resolve(source, args[0]), ...args.slice(1)], cwd: source });
  }
  const metadata = resolve(sdk, "touhou-sdk.json");
  return { game, variant, owner, source, common, features: { thcrap, thprac },
    toolchain: { root: sdk, compiler: compilerPath(sdk), version, ...(existsSync(metadata) ? { metadata: JSON.parse(readFileSync(metadata, "utf8")) } : {}) },
    environment: env, steps };
}
