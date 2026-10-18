import { resolve } from "node:path";
import { PRODUCT_GAMES } from "./contracts/product-catalog.mjs";
import { normalizeProductSelection } from "./product-selection.mjs";

export const RELEASE_INPUT_SCHEMA = "eagler-touhou/release-input/1";
export const RELEASE_PATH_INPUT_KEYS = Object.freeze([
  "RuntimeRelease",
  "Th06Directory",
  "Th07Directory",
  "Th08Directory",
  "FeatureConfig",
  "FontFile",
  "VanillaFontFile",
  "Th06LanguagePacks",
  "Th07LanguagePacks",
  "ArtworkDirectory",
]);

const FORBIDDEN_FORMAL_BUILD_INPUTS = Object.freeze([
  "Th08Build",
  "EmsdkDirectory",
  "CMake",
  "Ninja",
]);

export function normalizeFormalReleaseInput(input, { inputDirectory, workspace, windir = "C:/Windows" } = {}) {
  if (!input || input.schema !== RELEASE_INPUT_SCHEMA || !input.prepare || typeof input.prepare !== "object") {
    throw new Error("invalid release input");
  }
  const games = Object.keys(PRODUCT_GAMES);
  if (input.games != null) {
    const requested = normalizeProductSelection(input.games);
    if (JSON.stringify(requested) !== JSON.stringify(games)) {
      throw new Error("formal host release must contain every registered product; product subsets are development/validation only");
    }
  }

  const prepare = { ...input.prepare };
  if (prepare.OutputDirectory || prepare.PythonEnvironmentDirectory || prepare.GeneratedCacheDirectory) {
    throw new Error("release-owned output parameters must not be supplied");
  }
  for (const key of FORBIDDEN_FORMAL_BUILD_INPUTS) {
    if (prepare[key]) {
      throw new Error(`formal release consumes prepare.RuntimeRelease; ${key} belongs to maintainer Runtime compilation`);
    }
  }
  if (!prepare.RuntimeRelease) {
    throw new Error("formal release requires prepare.RuntimeRelease (resource-free all-product Runtime Release)");
  }

  prepare.FontFile ||= resolve(workspace, "dependencies/unifont-15.1.05/unifont-15.1.05.otf");
  prepare.VanillaFontFile ||= resolve(windir, "Fonts/msgothic.ttc");
  prepare.Profile = "web-release-hosted";
  prepare.Games = games;

  for (const key of ["RuntimeRelease", "FeatureConfig", "Th06Directory", "Th07Directory", "Th08Directory"]) {
    if (typeof prepare[key] !== "string" || !prepare[key]) throw new Error(`missing prepare.${key}`);
  }
  for (const key of RELEASE_PATH_INPUT_KEYS) {
    if (!prepare[key]) continue;
    prepare[key] = resolve(inputDirectory, prepare[key]);
  }
  return Object.freeze({ games, prepare: Object.freeze(prepare) });
}

export function formalReleaseSourceOwners() {
  // Game Runtime source provenance belongs to the Runtime Release producer.
  // Host assembly only executes Launcher-owned tooling against explicit inputs.
  return Object.freeze(["launcher"]);
}
