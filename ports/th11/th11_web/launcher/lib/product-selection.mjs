import { PRODUCT_GAMES } from "./contracts/product-catalog.mjs";

export function normalizeProductSelection(value) {
  const registered = Object.keys(PRODUCT_GAMES);
  if (value == null) return registered;
  const requested = typeof value === "string" ? value.split(",").map(item => item.trim()).filter(Boolean) : value;
  if (!Array.isArray(requested) || requested.length === 0 || new Set(requested).size !== requested.length ||
      requested.some(game => typeof game !== "string" || !Object.hasOwn(PRODUCT_GAMES, game))) {
    throw new Error("games must be a non-empty unique subset of the product registry");
  }
  const selected = new Set(requested);
  return registered.filter(game => selected.has(game));
}

export function selectProductEntries(entries, games) {
  return Object.fromEntries(games.map(game => [game, entries[game]]));
}

export function assertProductEntriesRegistered(entries, label = "product entries") {
  if (!entries || typeof entries !== "object" || Array.isArray(entries)) {
    throw new Error(`${label} must be an object`);
  }
  const unknown = Object.keys(entries).filter(game => !Object.hasOwn(PRODUCT_GAMES, game));
  if (unknown.length) {
    throw new Error(`${label} do not match registered adapters: ${unknown.join(", ")}`);
  }
  return entries;
}
