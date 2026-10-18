import { isGameId, type GameId } from "./product-catalog.mjs";

export const RELEASE_CATALOG_SCHEMA = "eagler-touhou/release-catalog/1";
export const RELEASE_CATALOG_FILE = "release-catalog.json";

export interface ReleaseCatalogEntry {
  revision: string;
  descriptor: string;
  [key: string]: unknown;
}

export interface ReleaseCatalog {
  schema: typeof RELEASE_CATALOG_SCHEMA;
  games: Partial<Record<GameId, ReleaseCatalogEntry>>;
  [key: string]: unknown;
}

const REVISION = /^[a-z0-9][a-z0-9._:-]{0,127}$/i;

function isRecord(value: unknown): value is Record<string, unknown> {
  return value !== null && typeof value === "object" && !Array.isArray(value);
}

export function validateReleaseCatalog(value: unknown): ReleaseCatalog {
  if (!isRecord(value) || value.schema !== RELEASE_CATALOG_SCHEMA || !isRecord(value.games)) {
    throw new Error("invalid Release Catalog");
  }
  for (const [game, entry] of Object.entries(value.games)) {
    if (!isGameId(game) || !isRecord(entry) ||
        typeof entry.revision !== "string" || !REVISION.test(entry.revision) ||
        typeof entry.descriptor !== "string" || !entry.descriptor || entry.descriptor.includes("\\")) {
      throw new Error(`invalid Release Catalog entry: ${game}`);
    }
    const url = new URL(entry.descriptor, `https://catalog.invalid/${RELEASE_CATALOG_FILE}`);
    if (url.origin !== "https://catalog.invalid") throw new Error(`cross-origin Release Catalog descriptor: ${game}`);
  }
  return value as ReleaseCatalog;
}

export function releaseCatalogEntryUrl(catalogUrl: string, catalog: unknown, game: string): string | null {
  const validated = validateReleaseCatalog(catalog);
  if (!isGameId(game)) return null;
  const entry = validated.games[game];
  if (!entry) return null;
  return new URL(entry.descriptor, catalogUrl).href;
}
