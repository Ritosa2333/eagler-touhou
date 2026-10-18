export const GAME_VERSIONS = Object.freeze(["original", "beauty"]);

export function normalizeGameVersion(value) {
  return value === "beauty" ? "beauty" : "original";
}

export function packageInstallationKey(game, version = "original") {
  if (!GAME_VERSIONS.includes(version)) throw new Error("invalid game version");
  // Preserve the existing original installation key without a DB migration.
  return version === "original" ? game : [game, version];
}

export function loadGameVersion(storage, game) {
  try { return normalizeGameVersion(storage?.getItem(`touhou.${game}.gameVersion`)); }
  catch { return "original"; }
}

export function persistGameVersion(storage, game, version) {
  if (!GAME_VERSIONS.includes(version)) throw new Error("invalid game version");
  try { storage?.setItem(`touhou.${game}.gameVersion`, version); } catch {}
}

export function descriptorGameVersion(descriptor) {
  return normalizeGameVersion(descriptor?.gameVersion);
}
